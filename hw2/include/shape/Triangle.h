#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shape/GLShape.h"


class Shader;


class Triangle : public Renderable, public GLShape
{
public:
    struct Vertex
    {
        glm::vec2 position;
        glm::vec3 color;
    };

    explicit Triangle(
            Shader * shader,
            const std::vector<Vertex> & vertices,
            const glm::mat3 & model = glm::mat3(1.0f)
    );

    ~Triangle() noexcept override = default;

    void render(float timeElapsedSinceLastFrame, bool animate) override;

    void setTriangle(const glm::vec2 &v0, const glm::vec2 &v1, const glm::vec2 &v2,
                     const glm::vec3 &color = glm::vec3(1.0f, 0.0f, 0.0f));

private:
    std::vector<Vertex> vertices;
};


#endif  // TRIANGLE_H
