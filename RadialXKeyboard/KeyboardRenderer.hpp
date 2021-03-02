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
public:
    KeyboardRenderer(const RendererArgs& args);

    void init() override;

    void update() override;

    void teardown() override;

    ~KeyboardRenderer() {
        // Release anything that needs releasing
    }
};


#endif //TECLAT_KEYBOARDRENDERER_HPP
