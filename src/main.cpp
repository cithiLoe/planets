#include <iostream>
#include <array>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "planet.h"
#include "texture.h"

void glfwError(int id, const char* description);
void onKeyboard(GLFWwindow* window, int key, int scancode, int action, int mode);
void onMouseMove(GLFWwindow* window, double xpos, double ypos);
void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
void Do_Movement(GLfloat dt);

Camera camera(glm::vec3(0.0f, 1.0f, 15.0f));

bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int main() {
    const unsigned WIDTH = 800, HEIGHT = 600;

    glfwSetErrorCallback(&glfwError);
    if (!glfwInit()) {
        return -1;
    }

    // Minimum required OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Use the core profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // macOS specific option
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // How many samples to use for multisampling
    glfwWindowHint(GLFW_SAMPLES, 4);
    // Make it static for now
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    auto window = glfwCreateWindow(WIDTH, HEIGHT, "Planets", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, onKeyboard);
    glfwSetCursorPosCallback(window, onMouseMove);
    glfwSetScrollCallback(window, onMouseScroll);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Ensures GLEW uses more modern techniques for managing OpenGL functionality.
    // Its default value of GL_FALSE might give issues when using the core profile of OpenGL.
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    Shader skyboxShader("../shaders/skybox.vert", "../shaders/skybox.frag");

    static const GLfloat vertices[] = {
        // Positions            // Colors
        // top (+z)
        -1.0f, -1.0f,  1.0f,    1.0f, 0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,    1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,    1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,    1.0f, 0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,    1.0f, 0.0f, 0.0f,
         1.0f,  1.0f,  1.0f,    1.0f, 0.0f, 0.0f,

        // bottom (-z)
        -1.0f, -1.0f, -1.0f,    0.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 0.0f,
         1.0f, -1.0f, -1.0f,    0.0f, 1.0f, 0.0f,
         1.0f, -1.0f, -1.0f,    0.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 0.0f,

        // right (+x)
         1.0f, -1.0f, -1.0f,    0.0f, 0.0f, 1.0f,
         1.0f,  1.0f, -1.0f,    0.0f, 0.0f, 1.0f,
         1.0f, -1.0f,  1.0f,    0.0f, 0.0f, 1.0f,
         1.0f, -1.0f,  1.0f,    0.0f, 0.0f, 1.0f,
         1.0f,  1.0f, -1.0f,    0.0f, 0.0f, 1.0f,
         1.0f,  1.0f,  1.0f,    0.0f, 0.0f, 1.0f,

        // left (-x)
        -1.0f, -1.0f, -1.0f,    1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,    1.0f, 0.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,    1.0f, 0.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,    1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,    1.0f, 0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,    1.0f, 0.0f, 1.0f,

        // front (+y)
        -1.0f, -1.0f, -1.0f,    1.0f, 1.0f, 0.0f,
         1.0f, -1.0f, -1.0f,    1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,    1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,    1.0f, 1.0f, 0.0f,
         1.0f, -1.0f, -1.0f,    1.0f, 1.0f, 0.0f,
         1.0f, -1.0f,  1.0f,    1.0f, 1.0f, 0.0f,

        // back (-y)
        -1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,    0.0f, 1.0f, 1.0f,
         1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 1.0f,
         1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,    0.0f, 1.0f, 1.0f,
         1.0f,  1.0f,  1.0f,    0.0f, 1.0f, 1.0f,
    };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind VAO

    GLuint skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);

    // Cubemap (Skybox)
    std::vector<const GLchar*> faces;
    faces.push_back("../textures/galaxy.tga");
    faces.push_back("../textures/galaxy.tga");
    faces.push_back("../textures/galaxy.tga");
    faces.push_back("../textures/galaxy.tga");
    faces.push_back("../textures/galaxy.tga");
    faces.push_back("../textures/galaxy.tga");
    GLuint cubemapTexture = loadCubemap(faces);

    std::array<PlanetModel, 9> planetmodels = {
        {PlanetModel("Sun", "../objects/sun/sun.obj", 10, 0, 0),
        PlanetModel("Mercury", "../objects/mercury/mercury.obj", 2, 100, 47.362f),
        PlanetModel("Venus", "../objects/venus/venus.obj", 2, 200, 35.02f),
        PlanetModel("Earth", "../objects/earth/earth.obj", 2, 300, 29.78f),
        PlanetModel("Mars", "../objects/mars/mars.obj", 2, 410, 24.077f),
        PlanetModel("Jupiter", "../objects/jupiter/jupiter.obj", 2, 540, 13.07f),
        PlanetModel("Saturn", "../objects/saturn/saturn.obj", 2, 690, 9.69f),
        PlanetModel("Uranus", "../objects/uranus/uranus.obj", 2, 780, 6.80f),
        PlanetModel("Neptune", "../objects/neptune/neptune.obj", 2, 990, 5.43f)}
    };

    while (!glfwWindowShouldClose(window)) {
        // Set frame time
        GLfloat currentFrame = (GLfloat)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        Do_Movement(deltaTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (auto &planet : planetmodels) {
            planet.draw(deltaTime, camera);
        }

        // Draw skybox as last
        glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix()));  // Remove any translation component of the view matrix
        glm::mat4 projection = camera.getPerspectiveMatrix();
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(skyboxShader.program, "skybox"), 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // Set depth function back to default


        glfwSwapBuffers(window);
        glfwSwapInterval(1);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

void glfwError(int id, const char* description) {
    std::cerr << description << std::endl;
}

void Do_Movement(GLfloat dt) {
    // Camera controls
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) {
        camera.onKeyboard(FORWARD, dt);
    }
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) {
        camera.onKeyboard(BACKWARD, dt);
    }
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT] ) {
        camera.onKeyboard(LEFT, dt);
    }
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) {
        camera.onKeyboard(RIGHT, dt);
    }
}

void onKeyboard(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            keys[key] = true;
        } else if (action == GLFW_RELEASE) {
            keys[key] = false;
        }
    }
}

void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
    camera.onMouseScroll((GLfloat)yoffset);
}

void onMouseMove(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = (GLfloat)xpos;
        lastY = (GLfloat)ypos;
        firstMouse = false;
    }

    GLfloat xoffset = (GLfloat)xpos - lastX;
    GLfloat yoffset = lastY - (GLfloat)ypos;  // Reversed since y-coordinates go from bottom to left

    lastX = (GLfloat)xpos;
    lastY = (GLfloat)ypos;

    camera.onMouseMove(xoffset, yoffset);
}
