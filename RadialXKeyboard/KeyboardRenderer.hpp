//
// Created by LuisValencia on 2/28/21.
//

#ifndef TECLAT_KEYBOARDRENDERER_HPP
#define TECLAT_KEYBOARDRENDERER_HPP

#include "Renderer.hpp"
#include <bgfx/embedded_shader.h>

class KeyboardRenderer : public Renderer {
private:
    bgfx::VertexBufferHandle vertexBufferHandle;
    bgfx::IndexBufferHandle indexBufferHandle;
    bgfx::ProgramHandle program;

    void processEvents(void* inputHandler);
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
