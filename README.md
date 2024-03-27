# OpenGL 学习记录

## 2024/3/26
```cpp
// 你好，窗口
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

struct color {
    float r, g, b, a;
};

constexpr uint32_t SCR_WIDTH = 800;
constexpr uint32_t SCR_HEIGHT = 600;
constexpr color GREEN{ 0.2f, 0.3f, 0.3f, 1.0f};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "learn_opengl", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        glClearColor(GREEN.r, GREEN.g, GREEN.b, GREEN.a);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
```

```cpp
// 你好三角形（基础部分）
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <array>

#include "logger.hpp"

struct color {
    float r, g, b, a;
};

constexpr uint32_t SCR_WIDTH = 800;
constexpr uint32_t SCR_HEIGHT = 600;
constexpr color GREEN{ 0.2f, 0.3f, 0.3f, 1.0f};
constexpr uint32_t LOG_BUFFER_SIZE = 512;

constexpr const char* vertex_shader_src {
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}"
};

constexpr const char* fragment_shader_src {
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}"
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "learn_opengl", nullptr, nullptr);
    if (window == nullptr) {
        Log::error("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        Log::error("Failed to initialize GLAD");
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 顶点着色器对象
    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_src, nullptr);
    glCompileShader(vertex_shader);
    // check compile result
    int success{};
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[LOG_BUFFER_SIZE];
        glGetShaderInfoLog(vertex_shader, LOG_BUFFER_SIZE, nullptr, info_log);
        Log::error("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n{}", info_log);
    }

    // 片元着色器对象
    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_src, nullptr);
    glCompileShader(fragment_shader);
    // check compile result
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[LOG_BUFFER_SIZE];
        glGetShaderInfoLog(fragment_shader, LOG_BUFFER_SIZE, nullptr, info_log);
        Log::error("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n{}", info_log);
    }

    // 着色器程序，合并链接多个着色器
    uint32_t shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    // check link result
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[LOG_BUFFER_SIZE];
        glGetProgramInfoLog(shader_program, LOG_BUFFER_SIZE, nullptr, info_log);
        Log::error("ERROR::SHADER::PROGRAM::LINK_FAILED\n{}", info_log);
    }
    // delete shader after link
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    std::array<float, 9> vertices {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };

    // 顶点缓冲对象 vertex buffer object
    uint32_t VBO{}, VAO{};
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

    // 解析缓冲顶点
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 解绑
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        glClearColor(GREEN.r, GREEN.g, GREEN.b, GREEN.a);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw the triangle
        glUseProgram(shader_program);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader_program);

    glfwTerminate();
    return 0;
}
```

## 2024/3/27
```cpp
// 你好，三角形(EBO绘制正方形)
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <array>

#include "logger.hpp"

struct color {
    float r, g, b, a;
};

constexpr uint32_t SCR_WIDTH = 800;
constexpr uint32_t SCR_HEIGHT = 600;
constexpr color GREEN{ 0.2f, 0.3f, 0.3f, 1.0f};
constexpr uint32_t LOG_BUFFER_SIZE = 512;

constexpr const char* vertex_shader_src {
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}"
};

constexpr const char* fragment_shader_src {
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}"
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "learn_opengl", nullptr, nullptr);
    if (window == nullptr) {
        Log::error("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        Log::error("Failed to initialize GLAD");
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 顶点着色器对象
    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_src, nullptr);
    glCompileShader(vertex_shader);
    // check compile result
    int success{};
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[LOG_BUFFER_SIZE];
        glGetShaderInfoLog(vertex_shader, LOG_BUFFER_SIZE, nullptr, info_log);
        Log::error("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n{}", info_log);
    }

    // 片元着色器对象
    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_src, nullptr);
    glCompileShader(fragment_shader);
    // check compile result
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[LOG_BUFFER_SIZE];
        glGetShaderInfoLog(fragment_shader, LOG_BUFFER_SIZE, nullptr, info_log);
        Log::error("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n{}", info_log);
    }

    // 着色器程序，合并链接多个着色器
    uint32_t shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    // check link result
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[LOG_BUFFER_SIZE];
        glGetProgramInfoLog(shader_program, LOG_BUFFER_SIZE, nullptr, info_log);
        Log::error("ERROR::SHADER::PROGRAM::LINK_FAILED\n{}", info_log);
    }
    // delete shader after link
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    std::array<float, 12> vertices {
        0.5f, 0.5f, 0.0f,   // right-up
        0.5f, -0.5f, 0.0f,  // right-bottom
        -0.5f, -0.5f, 0.0f, // left-bottom
        -0.5, 0.5f, 0.0f    // left-up
    };

    std::array<uint32_t, 6> indices {
        0, 1, 3,
        1, 2, 3
    };

    // 顶点缓冲对象 vertex buffer object
    uint32_t VBO{}, VAO{}, EBO{};
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

    // 解析缓冲顶点
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 解绑
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        glClearColor(GREEN.r, GREEN.g, GREEN.b, GREEN.a);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw the triangle
        glUseProgram(shader_program);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shader_program);

    glfwTerminate();
    return 0;
}
```