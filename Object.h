#include <GL/glew.h>
#include <GL/freeglut.h>

#include "GLSLProgram.h"

#ifndef OBJECT_CPP
#define OBJECT_CPP

class Object
{
public:
    inline Object()
        : vao(0),
        positionBuffer(0),
        colorBuffer(0),
        indexBuffer(0)
    {}

    inline ~Object() { // GL context must exist on destruction
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &indexBuffer);
        glDeleteBuffers(1, &colorBuffer);
        glDeleteBuffers(1, &positionBuffer);
    }

    GLuint vao;        // vertex-array-object ID

    GLuint positionBuffer; // ID of vertex-buffer: position
    GLuint colorBuffer;    // ID of vertex-buffer: color

    GLuint indexBuffer;    // ID of index-buffer

    glm::mat4x4 model; // model matrix
};

#endif