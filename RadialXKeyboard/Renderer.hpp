//
// Created by LuisValencia on 2/28/21.
//

#ifndef TECLAT_RENDERER_HPP
#define TECLAT_RENDERER_HPP

#include <bx/uint32_t.h>

struct RendererArgs {
    void *layer;
    void *device;
    uint32_t width;
    uint32_t height;
};

class Renderer {
protected:
    RendererArgs _args;
public:
    Renderer(const RendererArgs& args) {
        _args = args;
    };

    virtual void init() = 0;

    virtual void update() = 0;

    virtual void teardown() = 0;

    void *layer() {
        return _args.layer;
    }

    void *device() {
        return _args.device;
    }

    uint32_t width() {
        return _args.width;
    }

    uint32_t height() {
        return _args.height;
    }

    virtual ~Renderer() = default;
};

#endif //TECLAT_RENDERER_HPP