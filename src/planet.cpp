#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "planet.h"

Planet::Planet(std::string name, GLfloat mean_anomaly, GLfloat perihelion, GLfloat aphelion, GLfloat speed)
    : name(name), speed(speed), mean_anomaly(mean_anomaly * 3.1415f / 180.0f), perihelion(perihelion*10), aphelion(aphelion*10) {
    this->position = glm::vec3(this->perihelion, 0.0f, 0.0f);
    this->theta = 0;
    this->eccentricity = (aphelion - perihelion) / (aphelion + perihelion);
}

glm::mat4 Planet::getModelMatrix(GLfloat dt) {
    glm::mat4 model;
    if (this->perihelion != 0.0f) {
        float x = this->perihelion * glm::sin(this->theta);
        float z = this->perihelion * glm::cos(this->theta);
        this->theta += this->speed / this->perihelion * dt / 2;
        if (this->theta > 2*3.1415) {
            this->theta -= 2*3.1415;
        }
        position = glm::vec3(x, 0.0f, z);
    }
    model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));
    model = glm::translate(model, this->position);
    return model;
}

PlanetModel::PlanetModel(std::string name, std::string model, GLfloat planet_radius, GLfloat orbit_radius, GLfloat speed) : name(name) {
    this->model = new Model((char*)model.c_str());
    this->shader = new Shader("../shaders/model.vert", "../shaders/model.frag");
    this->speed = speed;
    this->planet_radius = planet_radius;
    this->orbit_radius = orbit_radius;
    this->position = glm::vec3(this->orbit_radius, 0.0f, 0.0f);
    this->theta = 0;
}

void PlanetModel::draw(GLfloat dt, Camera camera) {
    this->shader->use();
    glm::mat4 projection = camera.getPerspectiveMatrix();
    glm::mat4 view = camera.getViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(this->shader->program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(this->shader->program, "view"), 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 model = getModelMatrix(dt);
    glUniformMatrix4fv(glGetUniformLocation(this->shader->program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    this->model->draw(*this->shader);
}

glm::mat4 PlanetModel::getModelMatrix(GLfloat dt) {
    glm::mat4 model;
    if (this->orbit_radius != 0.0f) {
        float x = this->orbit_radius * glm::sin(this->theta);
        float z = this->orbit_radius * glm::cos(this->theta);
        this->theta += this->speed / this->orbit_radius * dt / 2;
        if (this->theta > 2*3.1415) {
            this->theta -= 2*3.1415;
        }
        this->position = glm::vec3(x, 0.0f, z);
    }
    model = glm::scale(model, glm::vec3(0.001, 0.001, 0.001) * this->planet_radius);
    model = glm::translate(model, this->position * 100.0f);
    return model;
}
