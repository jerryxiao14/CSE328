#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include "shape/Circle.h"
#include "util/Shader.h"


Circle::Circle(Shader * shader, const std::vector<glm::vec3> & parameters) : GLShape(shader), parameters(parameters)
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Vertex coordinate attribute array "layout (position = 0) in vec3 aPos"
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,                             // index: corresponds to "0" in "layout (position = 0)"
                          3,                             // size: each "vec3" generic vertex attribute has 3 values
                          GL_FLOAT,                      // data type: "vec3" generic vertex attributes are GL_FLOAT
                          GL_FALSE,                      // do not normalize data
                          sizeof(glm::vec3),             // stride between attributes in VBO data
                          reinterpret_cast<void *>(0));  // offset of 1st attribute in VBO data

    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizei>(parameters.size() * sizeof(glm::vec3)),
                 parameters.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void Circle::render(float timeElapsedSinceLastFrame, bool animate)
{
    if (animate)
    {
        //model = glm::rotate(model, timeElapsedSinceLastFrame);
    }

    pShader->use();
    pShader->setMat3("model", model);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glPatchParameteri(GL_PATCH_VERTICES, 1);
    glDrawArrays(GL_PATCHES,
                 0,                                          // start from index 0 in current VBO
                 static_cast<GLsizei>(parameters.size()));  // draw these number of elements

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Circle::setCircleGeometry(float x, float y, float r) {
    parameters[0] = glm::vec3(x, y, r);
    syncToGPU();
}

void Circle::syncToGPU() {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,                                          // offset of 1st attribute in VBO data
                    sizeof(glm::vec3),                         // size of data being replaced
                    parameters.data());                        // pointer to new data
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
