//
// Created by LuisValencia on 2/28/21.
//

#include "KeyboardRenderer.hpp"
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>
#include "ShaderLoader.hpp"

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

    // Set view 0 clear state.
    bgfx::setViewClear(
            0,
            BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
            0x303030ff,
            1.0f,
            0
    );

    bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));

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
}

unsigned int counter = 0;

void KeyboardRenderer::update() {
    uint32_t width = this->width();
    uint32_t height = this->height();

    const bx::Vec3 at = {0.0f, -6.0f, 0.0f};
    const bx::Vec3 eye = {0.0f, 0.0f, -14.0f};
    {
        float view[16];
        bx::mtxLookAt(view, eye, at);

        float proj[16];
        bx::mtxProj(
                proj,
                60.0f,
                float(width) / float(height),
                0.1f,
                100.0f,
                bgfx::getCaps()->homogeneousDepth
        );
        bgfx::setViewTransform(0, view, proj);
        // Set view 0 default viewport.
        bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
    }
    // This dummy draw call is here to make sure that view 0 is cleared
    // if no other draw calls are submitted to view 0.
    bgfx::touch(0);

    float mtx[16];
    bx::mtxRotateXY(mtx, counter * 0.01f, counter * 0.01f);
    bgfx::setTransform(mtx);

    bgfx::setVertexBuffer(0, vertexBufferHandle);
    bgfx::setIndexBuffer(indexBufferHandle);

    bgfx::submit(0, program);

    bgfx::frame();
    counter++;
}

void KeyboardRenderer::teardown() {

}
