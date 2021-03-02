//
// Created by LuisValencia on 2/28/21.
//

#include "ShaderLoader.hpp"
#include <map>
#include <string>
#include <bx/file.h>

bx::FileReader *fileReader = new bx::FileReader();

bgfx::ShaderHandle ShaderLoader::loadShader(const char *resourcePath, const char *fileName) {
    char filePath[1024];

    bx::strCopy(filePath, BX_COUNTOF(filePath), resourcePath);
    bx::strCat(filePath, BX_COUNTOF(filePath), fileName);
    bx::strCat(filePath, BX_COUNTOF(filePath), ".bin");

    bx::open(fileReader, filePath);
    uint32_t size = (uint32_t)bx::getSize(fileReader);
    const bgfx::Memory* mem = bgfx::alloc(size+1);
    bx::read(fileReader, mem->data, size);
    bx::close(fileReader);
    mem->data[mem->size-1] = '\0';

    bgfx::ShaderHandle handle = bgfx::createShader(mem);
    bgfx::setName(handle, fileName);

    return handle;
}
