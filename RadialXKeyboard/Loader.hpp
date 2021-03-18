//
// Created by LuisValencia on 2/28/21.
//

#ifndef TECLAT_LOADER_HPP
#define TECLAT_LOADER_HPP

#include <bgfx/embedded_shader.h>
#include <bx/math.h>
#include <tinystl/allocator.h>
#include <tinystl/vector.h>

#pragma mark mesh structures

struct AxisAlignedBoundingBox {
    bx::Vec3 min;
    bx::Vec3 max;
};

struct OrientedBoundingBox {
    float mtx[16];
};

struct Sphere {
    bx::Vec3 center;
    float radius;
};

struct Primitive {
    uint32_t startIndex;
    uint32_t numberOfIndices;
    uint32_t startVertex;
    uint32_t numberOfVertices;

    Sphere sphere;
    AxisAlignedBoundingBox axisAlignedBoundingBox;
    OrientedBoundingBox orientedBoundingBox;
};

typedef tinystl::vector<Primitive> PrimitiveArray;

struct Group {
    Group() {
        reset();
    }

    void reset() {
        vertexBufferHandle.idx = bgfx::kInvalidHandle;
        indexBufferHandle.idx = bgfx::kInvalidHandle;
        numberOfVertices = 0;
        vertices = NULL;
        numberOfIndices = 0;
        indices = NULL;
        primitives.clear();
    }

    bgfx::VertexBufferHandle vertexBufferHandle;
    bgfx::IndexBufferHandle indexBufferHandle;
    uint16_t numberOfVertices;
    uint8_t *vertices;
    uint32_t numberOfIndices;
    uint16_t *indices;
    Sphere sphere;
    AxisAlignedBoundingBox axisAlignedBoundingBox;
    OrientedBoundingBox orientedBoundingBox;
    PrimitiveArray primitives;
};

typedef tinystl::vector<Group> GroupArray;
struct Mesh {
    bgfx::VertexLayout layout;
    GroupArray groups;
};

#pragma mark interface

class Loader {
public:
    static bgfx::ShaderHandle loadShader(const char* resourcePath, const char *fileName);
    static Mesh loadMesh(const char *resourcePath, const char *fileName);
};

#endif //TECLAT_LOADER_HPP
