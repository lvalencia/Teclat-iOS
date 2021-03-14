//
// Created by LuisValencia on 2/28/21.
//

#include "KeyboardRenderer.hpp"
#include <QuartzCore/QuartzCore.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>
#include <cstdio>
#include <sstream>
#include <bx/rng.h>
#include "ShaderLoader.hpp"
#include "InputHandler.hpp"

#define RENDER_PASS_SHADING 0  // Default forward rendered geo with simple shading
#define RENDER_PASS_ID      1  // ID buffer for picking
#define RENDER_PASS_BLIT    2  // Blit GPU render target to CPU texture

#define DEBUG false

#pragma mark cube mesh
struct PositionalColorVertex {
    float x;
    float y;
    float z;
    uint32_t alphaBlueGreenRed;
};

static PositionalColorVertex cubeVertices[] =
        {
                {-1.0f, 1.0f, 1.0f, 0xff000000},
                {1.0f, 1.0f, 1.0f, 0xff0000ff},
                {-1.0f, -1.0f, 1.0f, 0xff00ff00},
                {1.0f, -1.0f, 1.0f, 0xff00ffff},
                {-1.0f, 1.0f, -1.0f, 0xffff0000},
                {1.0f, 1.0f, -1.0f, 0xffff00ff},
                {-1.0f, -1.0f, -1.0f, 0xffffff00},
                {1.0f, -1.0f, -1.0f, 0xffffffff},
        };

static const uint16_t cubeTrianglesList[] =
        {
                0, 1, 2,
                1, 3, 2,
                4, 6, 5,
                5, 6, 7,
                0, 2, 4,
                4, 2, 6,
                1, 5, 3,
                5, 7, 3,
                0, 4, 1,
                4, 5, 1,
                2, 3, 6,
                6, 3, 7,
        };

KeyboardRenderer::KeyboardRenderer(const RendererArgs &args) : Renderer(args) {
}

void KeyboardRenderer::init() {
    uint32_t width = this->width();
    uint32_t height = this->height();

    bgfx::PlatformData platformData;
    platformData.ndt = NULL;
    platformData.nwh = this->layer();
    platformData.context = this->device();
    platformData.backBuffer = NULL;
    platformData.backBufferDS = NULL;
    bgfx::setPlatformData(platformData);

    bgfx::Init init;
    init.type = bgfx::RendererType::Count;
    init.vendorId = BGFX_PCI_ID_NONE;
    init.resolution.width = width;
    init.resolution.height = height;
    init.resolution.reset = BGFX_RESET_VSYNC;
    bgfx::init(init);

    // Enable debug text.
    bgfx::setDebug(BGFX_DEBUG_TEXT);

    // Set view RENDER_PASS_SHADING clear state.
    bgfx::setViewClear(
            RENDER_PASS_SHADING,
            BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
            0x303030ff,
            1.0f,
            0
    );

    // ID buffer clears to black, which represents clicking on nothing (background)
    bgfx::setViewClear(
            RENDER_PASS_ID,
            BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
            0x000000ff,
            1.0f,
            0
    );

    bgfx::setViewRect(RENDER_PASS_SHADING, 0, 0, uint16_t(width), uint16_t(height));

    // Vertex Declaration
    bgfx::VertexLayout positionalColorVertexDeclaration;
    positionalColorVertexDeclaration.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
    vertexBufferHandle =
            bgfx::createVertexBuffer(
                    bgfx::makeRef(cubeVertices, sizeof(cubeVertices)),
                    positionalColorVertexDeclaration
            );
    indexBufferHandle = bgfx::createIndexBuffer(
            bgfx::makeRef(cubeTrianglesList, sizeof(cubeTrianglesList))
    );

    const char *path = this->resourcePath();
    bgfx::ShaderHandle vectorShader = ShaderLoader::loadShader(path, "/vs_cubes");
    bgfx::ShaderHandle fragmentShader = ShaderLoader::loadShader(path, "/fs_cubes");
    program = bgfx::createProgram(vectorShader, fragmentShader, true);

    // Create Uniform(s)
    u_id = bgfx::createUniform("u_id", bgfx::UniformType::Vec4); // ID for drawing into ID buffer

    bgfx::ShaderHandle pickingVectorShader = ShaderLoader::loadShader(path, "/vs_cubes");
    bgfx::ShaderHandle pickingFragmentShader = ShaderLoader::loadShader(path, "/fs_picking");
    pickingProgram = bgfx::createProgram(pickingVectorShader, pickingFragmentShader, true);

    reading = 0;
    fieldOfView = 8.0f;
    currentFrame = UINT32_MAX;

    // Set up ID buffer, which has a color target and depth buffer
    pickingRenderTarget = bgfx::createTexture2D(ID_DIMENSION, ID_DIMENSION, false, 1, bgfx::TextureFormat::RGBA8, 0
            | BGFX_TEXTURE_RT
            | BGFX_SAMPLER_MIN_POINT
            | BGFX_SAMPLER_MAG_POINT
            | BGFX_SAMPLER_MIP_POINT
            | BGFX_SAMPLER_U_CLAMP
            | BGFX_SAMPLER_V_CLAMP
    );
    pickingRenderTargetDepth = bgfx::createTexture2D(ID_DIMENSION, ID_DIMENSION, false, 1, bgfx::TextureFormat::D24S8, 0
            | BGFX_TEXTURE_RT
            | BGFX_SAMPLER_MIN_POINT
            | BGFX_SAMPLER_MAG_POINT
            | BGFX_SAMPLER_MIP_POINT
            | BGFX_SAMPLER_U_CLAMP
            | BGFX_SAMPLER_V_CLAMP
    );

    // CPU texture for blitting to and reading ID buffer so we can see what was clicked on.
    // Impossible to read directly from a render target, you *must* blit to a CPU texture
    // first. Algorithm Overview: Render on GPU -> Blit to CPU texture -> Read from CPU
    // texture.
    blitTexture = bgfx::createTexture2D(ID_DIMENSION, ID_DIMENSION, false, 1, bgfx::TextureFormat::RGBA8, 0
            | BGFX_TEXTURE_BLIT_DST
            | BGFX_TEXTURE_READ_BACK
            | BGFX_SAMPLER_MIN_POINT
            | BGFX_SAMPLER_MAG_POINT
            | BGFX_SAMPLER_MIP_POINT
            | BGFX_SAMPLER_U_CLAMP
            | BGFX_SAMPLER_V_CLAMP
    );

    bgfx::TextureHandle rt[2] =
            {
                    pickingRenderTarget,
                    pickingRenderTargetDepth
            };
    pickingFrameBuffer = bgfx::createFrameBuffer(BX_COUNTOF(rt), rt, true);

    touchEvent.x = 0;
    touchEvent.y = 0;
}

unsigned int counter = 0;

void KeyboardRenderer::update(void *inputHandler) {
    const bgfx::Caps *caps = bgfx::getCaps();
    bool blitSupport = 0 != (caps->supported & BGFX_CAPS_TEXTURE_BLIT);

    if (!blitSupport) {
        printf("blit not supported selection wont work\n");
    }

    bgfx::setViewFrameBuffer(RENDER_PASS_ID, pickingFrameBuffer);

    uint32_t width = this->width();
    uint32_t height = this->height();

    const bx::Vec3 at = {0.0f, -6.0f, 0.0f};
    const bx::Vec3 eye = {0.0f, 0.0f, -14.0f};

    float view[16];
    bx::mtxLookAt(view, eye, at);

    float proj[16];
    bx::mtxProj(
            proj,
            60.0f,
            // fieldOfView,
            float(width) / float(height),
            0.1f,
            100.0f,
            bgfx::getCaps()->homogeneousDepth
    );
    bgfx::setViewTransform(RENDER_PASS_SHADING, view, proj);
    // Set view RENDER_PASS_SHADING default viewport.
    bgfx::setViewRect(RENDER_PASS_SHADING, 0, 0, uint16_t(width), uint16_t(height));

    bool queueWasNotEmpty = !this->queue()->isEmpty();
    if (queueWasNotEmpty) {
        touchEvent = this->queue()->dequeue();
    }

    // Mouse coord in Normalized Device Coordinates
    float normalizedMouseX = ((float) touchEvent.x / (float) width) * 2.0f - 1.0f;
    float normalizedMouseY = (((float) height - (float) touchEvent.y) / (float) height) * 2.0f - 1.0f;

    if (DEBUG) {
        printf("x: %.2f y: %.2f\n", (float) touchEvent.x, (float) touchEvent.y);
    }

    float viewProjection[16];
    bx::mtxMul(viewProjection, view, proj);

    float inverseViewProjection[16];
    bx::mtxInverse(inverseViewProjection, viewProjection);

    // mouse stuff was here

    const bx::Vec3 pickEye = bx::mulH({normalizedMouseX, normalizedMouseY, 0.0f}, inverseViewProjection);
    const bx::Vec3 pickAt = bx::mulH({normalizedMouseX, normalizedMouseY, 1.0f}, inverseViewProjection);

    // Look at our unprojected point
    float pickView[16];
    bx::mtxLookAt(pickView, pickEye, pickAt);

    // Tight field of view is best for picking
    float pickProjection[16];
    bx::mtxProj(pickProjection, fieldOfView, 1, 0.1f, 100.0f, caps->homogeneousDepth);

    // View rect and transforms for picking pass
    bgfx::setViewRect(RENDER_PASS_ID, 0, 0, ID_DIMENSION, ID_DIMENSION);
    bgfx::setViewTransform(RENDER_PASS_ID, pickView, pickProjection);

    bx::RngMwc mwc;  // Random number generator
    uint32_t rr = mwc.gen() % 256;
    uint32_t gg = mwc.gen() % 256;
    uint32_t bb = mwc.gen() % 256;
    float vec4Color[4];
    vec4Color[0] = rr / 255.0f;
    vec4Color[1] = gg / 255.0f;
    vec4Color[2] = bb / 255.0f;
    vec4Color[3] = 1.0f;
    // set uniform for fragColor
    bgfx::setUniform(u_id, vec4Color);

    // This dummy draw call is here to make sure that view RENDER_PASS_SHADING is cleared
    // if no other draw calls are submitted to view RENDER_PASS_SHADING.
    bgfx::touch(RENDER_PASS_SHADING);

    float mtx[16];
    bx::mtxRotateXY(mtx, counter * 0.01f, counter * 0.01f);
    bgfx::setTransform(mtx);

    bgfx::setVertexBuffer(0, vertexBufferHandle);
    bgfx::setIndexBuffer(indexBufferHandle);

    bgfx::submit(RENDER_PASS_SHADING, program);

    // You have to re-set all these
    bgfx::setTransform(mtx);

    bgfx::setVertexBuffer(0, vertexBufferHandle);
    bgfx::setIndexBuffer(indexBufferHandle);
    // And Resubmit
    bgfx::submit(RENDER_PASS_ID, program);

    // If the user previously clicked, and we're done reading data from GPU, look at ID buffer on CPU
    // Whatever mesh has the most pixels in the ID buffer is the one the user clicked on.
    // (but for now we'll just assume that if we have color that we clicked on something)
    if (reading == currentFrame) {
        reading = 0;
        bool didTouchCube = false;
        // BLIT always reads just the background, which means that the cube isn't rendering in view
        for (uint8_t *x = blitData; x < blitData + ID_DIMENSION * ID_DIMENSION * 4;) {
            uint8_t rr = *x++;
            uint8_t gg = *x++;
            uint8_t bb = *x++;
            uint8_t aa = *x++;
            if (0 == (rr | gg | bb)) // Skip background
            {
                continue;
            }

            if (DEBUG) {
                double CurrentTime = CACurrentMediaTime();
                printf("%.2f In FOV\n", CurrentTime);
            }
            // We only care about the one cube right now
            didTouchCube = true;
            break;
        }
        if (didTouchCube) {
            insertText(inputHandler, "CUBE! ");
        } else {
            insertText(inputHandler, "VOID! ");
        }
    }

    if (!reading && queueWasNotEmpty) {
        // Blit and read
        bgfx::blit(RENDER_PASS_BLIT, blitTexture, 0, 0, pickingRenderTarget);
        reading = bgfx::readTexture(blitTexture, blitData);
    }

    currentFrame = bgfx::frame();
    counter++;
}

void KeyboardRenderer::teardown() {
    bgfx::destroy(indexBufferHandle);
    bgfx::destroy(vertexBufferHandle);
    bgfx::destroy(program);

    bgfx::destroy(pickingProgram);
    bgfx::destroy(u_id);
    bgfx::destroy(pickingFrameBuffer);
    bgfx::destroy(pickingRenderTarget);
    bgfx::destroy(pickingRenderTargetDepth);
    bgfx::destroy(blitTexture);

    bgfx::shutdown();
}
