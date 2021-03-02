//
// Created by LuisValencia on 2/28/21.
//

#ifndef TECLAT_SHADERLOADER_HPP
#define TECLAT_SHADERLOADER_HPP

#include <bgfx/embedded_shader.h>

class ShaderLoader {
public:
    static bgfx::ShaderHandle loadShader(const char* resourcePath, const char *fileName);
};

#endif //TECLAT_SHADERLOADER_HPP
