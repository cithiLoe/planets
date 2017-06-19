#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

#include "model.h"
#include "camera.h"

class Planet {
private:
    glm::vec3 position;
    const std::string name;
    GLfloat eccentricity;
    const GLfloat speed;
    const GLfloat mean_anomaly;
    const GLfloat perihelion;
    const GLfloat aphelion;
    float theta;

public:
    Planet(const std::string &name, GLfloat mean_anomaly, GLfloat perihelion, GLfloat aphelion, GLfloat speed);
    glm::mat4 getModelMatrix(GLfloat dt);
};

class PlanetModel {
private:
    glm::vec3 position;
    const std::string name;
    Model *model;
    Shader *shader;
    GLfloat speed;
    float theta;
    GLfloat planet_radius;
    GLfloat orbit_radius;
    glm::mat4 getModelMatrix(GLfloat dt);
public:
    PlanetModel(const std::string &name, const std::string &model, GLfloat planet_radius, GLfloat orbit_radius, GLfloat speed);
    void draw(GLfloat dt, Camera camera);
};
