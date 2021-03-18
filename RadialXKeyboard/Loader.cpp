//
// Created by LuisValencia on 2/28/21.
//

#include "Loader.hpp"
#include <stdio.h>
#include <bx/file.h>
#include <meshoptimizer/src/meshoptimizer.h>
#include <tinystl/allocator.h>
#include <tinystl/string.h>

#pragma mark methods

bx::FileReader *fileReader = new bx::FileReader();
bx::AllocatorI *allocator = new bx::DefaultAllocator();

bgfx::ShaderHandle Loader::loadShader(const char *resourcePath, const char *fileName) {
    char filePath[1024];

    bx::strCopy(filePath, BX_COUNTOF(filePath), resourcePath);
    bx::strCat(filePath, BX_COUNTOF(filePath), fileName);
    bx::strCat(filePath, BX_COUNTOF(filePath), ".bin");

    bx::open(fileReader, filePath);
    uint32_t size = (uint32_t) bx::getSize(fileReader);
    const bgfx::Memory *mem = bgfx::alloc(size + 1);
    bx::read(fileReader, mem->data, size);
    bx::close(fileReader);
    mem->data[mem->size - 1] = '\0';

    bgfx::ShaderHandle handle = bgfx::createShader(mem);
    bgfx::setName(handle, fileName);

    return handle;
}

Mesh Loader::loadMesh(const char *resourcePath, const char *fileName) {
    char filePath[1024];

    bx::strCopy(filePath, BX_COUNTOF(filePath), resourcePath);
    bx::strCat(filePath, BX_COUNTOF(filePath), fileName);
    bx::strCat(filePath, BX_COUNTOF(filePath), ".bin");

    bx::open(fileReader, filePath);

    // Chunk Identifiers for Reading
    constexpr uint32_t kChunkVertexBuffer = BX_MAKEFOURCC('V', 'B', ' ', 0x1);
    constexpr uint32_t kChunkVertexBufferCompressed = BX_MAKEFOURCC('V', 'B', 'C', 0x0);
    constexpr uint32_t kChunkIndexBuffer = BX_MAKEFOURCC('I', 'B', ' ', 0x0);
    constexpr uint32_t kChunkIndexBufferCompressed = BX_MAKEFOURCC('I', 'B', 'C', 0x1);
    constexpr uint32_t kChunkPrimitive = BX_MAKEFOURCC('P', 'R', 'I', 0x0);

    Group group;
    bgfx::VertexLayout layout;
    GroupArray groups;

    uint32_t chunk;
    bx::Error error;
    // return type of read is the # of elements read which (in the case of bx::read)
    // is determined by the sizeof (uint32_t)
    while (sizeof(uint32_t) == bx::read(fileReader, chunk, &error) && error.isOk()) {
        switch (chunk) {
            case kChunkVertexBuffer: {
                // Read the Chunks into the expected values
                fileReader->read(&group.sphere, sizeof(Sphere), NULL);
                fileReader->read(&group.axisAlignedBoundingBox, sizeof(AxisAlignedBoundingBox), NULL);
                fileReader->read(&group.orientedBoundingBox, sizeof(OrientedBoundingBox), NULL);

                fileReader->read(&layout, sizeof(layout), NULL);

                uint16_t stride = layout.getStride();

                fileReader->read(&group.numberOfVertices, sizeof(uint16_t), NULL);
                const bgfx::Memory *memory = bgfx::alloc(group.numberOfVertices * stride);
                fileReader->read(memory->data, memory->size, NULL);

                group.vertexBufferHandle = bgfx::createVertexBuffer(memory, layout);
                break;
            }
            case kChunkVertexBufferCompressed: {
                // Read the Chunks into the expected values
                fileReader->read(&group.sphere, sizeof(Sphere), NULL);
                fileReader->read(&group.axisAlignedBoundingBox, sizeof(AxisAlignedBoundingBox), NULL);
                fileReader->read(&group.orientedBoundingBox, sizeof(OrientedBoundingBox), NULL);

                fileReader->read(&layout, sizeof(layout), NULL);

                uint16_t stride = layout.getStride();

                fileReader->read(&group.numberOfVertices, sizeof(uint16_t), NULL);
                const bgfx::Memory *memory = bgfx::alloc(group.numberOfVertices * stride);

                uint32_t compressedSize;
                fileReader->read(&compressedSize, sizeof(compressedSize), NULL);
                void *compressedVertices = BX_ALLOC(allocator, compressedSize);

                fileReader->read(compressedVertices, compressedSize, NULL);

                meshopt_decodeVertexBuffer(
                        memory->data,
                        group.numberOfVertices,
                        stride,
                        (uint8_t *) compressedVertices,
                        compressedSize
                );

                BX_FREE(allocator, compressedVertices);
                group.vertexBufferHandle = bgfx::createVertexBuffer(memory, layout);

                break;
            }
            case kChunkIndexBuffer: {
                fileReader->read(&group.numberOfIndices, sizeof(uint32_t), NULL);
                // Not sure where the 2 comes from
                const bgfx::Memory *memory = bgfx::alloc(group.numberOfIndices * 2);
                fileReader->read(memory->data, memory->size, NULL);

                group.indexBufferHandle = bgfx::createIndexBuffer(memory);

                break;
            }
            case kChunkIndexBufferCompressed: {
                fileReader->read(&group.numberOfIndices, sizeof(uint32_t), NULL);
                // Not sure where the 2 comes from
                const bgfx::Memory *memory = bgfx::alloc(group.numberOfIndices * 2);

                uint32_t compressedSize;
                fileReader->read(&compressedSize, sizeof(compressedSize), NULL);

                void *compressedIndices = BX_ALLOC(allocator, compressedSize);

                fileReader->read(compressedIndices, compressedSize, NULL);
                meshopt_decodeIndexBuffer(
                        memory->data,
                        group.numberOfIndices,
                        2,
                        (uint8_t *) compressedIndices,
                        compressedSize
                );

                BX_FREE(allocator, compressedIndices);
                group.indexBufferHandle = bgfx::createIndexBuffer(memory);

                break;
            }
            case kChunkPrimitive: {
                uint16_t length;
                fileReader->read(&length, sizeof(length), NULL);

                tinystl::string material;
                material.resize(length);
                fileReader->read(const_cast<char *>(material.c_str()), length, NULL);

                uint16_t number;
                fileReader->read(&number, sizeof(number), NULL);

                for (uint32_t i = 0; i < number; i++) {
                    fileReader->read(&length, sizeof(length), NULL);

                    tinystl::string name;
                    name.resize(length);
                    fileReader->read(const_cast<char *>(name.c_str()), length, NULL);

                    Primitive primitive;
                    fileReader->read(&primitive.startIndex, sizeof(uint32_t), NULL);
                    fileReader->read(&primitive.numberOfIndices, sizeof(uint32_t), NULL);
                    fileReader->read(&primitive.startVertex, sizeof(uint32_t), NULL);
                    fileReader->read(&primitive.numberOfVertices, sizeof(uint32_t), NULL);
                    fileReader->read(&primitive.sphere, sizeof(Sphere), NULL);
                    fileReader->read(&primitive.axisAlignedBoundingBox, sizeof(AxisAlignedBoundingBox), NULL);
                    fileReader->read(&primitive.orientedBoundingBox, sizeof(OrientedBoundingBox), NULL);

                    group.primitives.push_back(primitive);
                }

                groups.push_back(group);
                group.reset();

                break;
            }
            default:
                printf("%08x at %d", chunk, (int)bx::skip(fileReader, 0));
                break;
        }
    }

    Mesh mesh;
    mesh.groups = groups;
    mesh.layout = layout;

    bx::close(fileReader);

    return mesh;
}
