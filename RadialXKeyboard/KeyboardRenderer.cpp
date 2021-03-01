//
// Created by LuisValencia on 2/28/21.
//

#include "KeyboardRenderer.hpp"
#import <bgfx/bgfx.h>
#import <bgfx/embedded_shader.h>
#import <bgfx/platform.h>

KeyboardRenderer::KeyboardRenderer(const RendererArgs &args) : Renderer(args) {
}

void KeyboardRenderer::init() {
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
    init.resolution.width = this->width();
    init.resolution.height = this->height();
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
}

void KeyboardRenderer::update() {
    bgfx::setViewRect(0, 0, 0, uint16_t(this->width()), uint16_t(this->height()));
    // This dummy draw call is here to make sure that view 0 is cleared
    // if no other draw calls are submitted to view 0.
    bgfx::touch(0);
}

void KeyboardRenderer::teardown() {

}