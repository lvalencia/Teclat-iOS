//
// Created by LuisValencia on 2/28/21.
//

#ifndef TECLAT_KEYBOARDRENDERER_HPP
#define TECLAT_KEYBOARDRENDERER_HPP

#include "Renderer.hpp"
#include <bgfx/embedded_shader.h>

#define ID_DIMENSION 8  // Size of the ID buffer

class KeyboardRenderer : public Renderer {
private:
    bgfx::VertexBufferHandle vertexBufferHandle;
    bgfx::IndexBufferHandle indexBufferHandle;
    bgfx::ProgramHandle program;
    // Picking
    bgfx::UniformHandle u_id;
    bgfx::ProgramHandle pickingProgram;
    bgfx::TextureHandle pickingRenderTarget;
    bgfx::TextureHandle pickingRenderTargetDepth;
    bgfx::TextureHandle blitTexture;
    bgfx::FrameBufferHandle pickingFrameBuffer;
    uint32_t reading;
    uint32_t currentFrame;
    float fieldOfView;
    uint8_t blitData[ID_DIMENSION*ID_DIMENSION * 4]; // Read blit into this
    CGPoint touchEvent;

public:
    KeyboardRenderer(const RendererArgs& args);

    void init() override;

    // passing this in like this is a hack <-- should probably be a "renderer" arg
    void update(void* inputHandler) override;

    void teardown() override;

    ~KeyboardRenderer() {
        // Release anything that needs releasing
    }
};


#endif //TECLAT_KEYBOARDRENDERER_HPP
