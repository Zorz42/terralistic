#include "graphics-internal.hpp"
#include <fstream>
#include <thread>
#include <iostream>
#include <cmath>

const char* vertex_shader_code =
"#version 330 core\n"
"layout(location = 0) in vec2 vertex_position;"
"layout(location = 1) in vec4 vertex_color;"
"layout(location = 2) in vec2 vertex_uv;"
"out vec4 fragment_color;"
"out vec2 uv;"
"uniform int has_color_buffer;"
"uniform vec4 default_color;"
"uniform mat3 transform_matrix;"
"uniform mat3 texture_transform_matrix;"
"void main() {"
"   gl_Position = vec4(transform_matrix * vec3(vertex_position.xy, 1), 1);"
"   fragment_color = has_color_buffer * vertex_color + (1 - has_color_buffer) * default_color;"
"   uv = (texture_transform_matrix * vec3(vertex_uv, 1)).xy;"
"}";

const char* fragment_shader_code =
"#version 330 core\n"
"in vec4 fragment_color;"
"in vec2 uv;"
"layout(location = 0) out vec4 color;"
"uniform sampler2D texture_sampler;"
"uniform int has_texture;"
"void main() {"
"   color = (texture(texture_sampler, uv).rgba * has_texture + (1 - has_texture) * vec4(1.f, 1.f, 1.f, 1.f)) * fragment_color;"
"}";

const char* blur_vertex_shader_code =
"#version 330 core\n"
"layout(location = 0) in vec2 vertex_position;"
"out vec2 uv;"
"uniform mat3 transform_matrix;"
"uniform mat3 texture_transform_matrix;"
"void main() {"
"   gl_Position = vec4(transform_matrix * vec3(vertex_position.xy, 1), 1);"
"   uv = (texture_transform_matrix * vec3(vertex_position.xy, 1)).xy;"
"}";

const char* blur_fragment_shader_code =
"#version 330 core\n"
"in vec2 uv;"
"layout(location = 0) out vec4 color;"
"uniform sampler2D texture_sampler;"
"uniform vec2 blur_offset;"
"uniform vec4 limit;"
"uniform mat3 transform_matrix;"
"uniform mat3 texture_transform_matrix;"
"float gauss[21] = float[](0.0012, 0.0015, 0.0038, 0.0087, 0.0180, 0.0332, 0.0547, 0.0807, 0.1065, 0.1258, 0.1330, 0.1258, 0.1065, 0.0807, 0.0547, 0.0332, 0.0180, 0.0087, 0.0038, 0.0015, 0.0012);"
"void main() {"
"   color = vec4(0, 0, 0, 0);"
"   for(int i = 0; i < 21; i++)"
"       color += texture(texture_sampler, max(min(uv + (i - 10.0) * blur_offset, vec2(limit.x, limit.y)), vec2(limit.z, limit.w))) * gauss[i];"
"}";

void checkForError(GLuint id) {
    int info_log_length;
    
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &info_log_length);
    if(info_log_length > 0) {
        std::vector<char> error_message(info_log_length + 1);
        glGetShaderInfoLog(id, info_log_length, NULL, &error_message[0]);
        throw std::runtime_error(&error_message[0]);
    }
}

GLuint CompileShaders(const char* vertex_code, const char* fragment_code) {
    GLuint vertex_id = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_id = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertex_id, 1, &vertex_code , NULL);
    glCompileShader(vertex_id);
    
    checkForError(vertex_id);

    glShaderSource(fragment_id, 1, &fragment_code , NULL);
    glCompileShader(fragment_id);

    checkForError(fragment_id);

    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vertex_id);
    glAttachShader(program_id, fragment_id);
    glLinkProgram(program_id);

    checkForError(program_id);
    
    glDetachShader(program_id, vertex_id);
    glDetachShader(program_id, fragment_id);
    
    glDeleteShader(vertex_id);
    glDeleteShader(fragment_id);
    
    return program_id;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    gfx::window_width = width / gfx::global_scale_x;
    gfx::window_height = height / gfx::global_scale_y;
    gfx::window_width_reciprocal = 1.f / gfx::window_width;
    gfx::window_height_reciprocal = 1.f / gfx::window_height;
    gfx::window_resized_counter++;
    
    gfx::window_normalization_transform.reset();
    gfx::window_normalization_transform.stretch(gfx::window_width_reciprocal * 2, -gfx::window_height_reciprocal * 2);
    gfx::window_normalization_transform.translate(-float(gfx::window_width) / 2, -float(gfx::window_height) / 2);
    
    glBindTexture(GL_TEXTURE_2D, gfx::window_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gfx::window_width, gfx::window_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    glBindTexture(GL_TEXTURE_2D, gfx::window_texture_back);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gfx::window_width, gfx::window_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    if(gfx::curr_scene != nullptr)
        gfx::curr_scene->renderAll();
}

void windowContentScaleCallback(GLFWwindow* window, float scale_x, float scale_y) {
    gfx::global_scale_x = scale_x;
    gfx::global_scale_y = scale_y;
    
    framebufferSizeCallback(gfx::glfw_window, gfx::window_width * scale_x, gfx::window_height * scale_y);
}

void gfx::setMinimumWindowSize(int width, int height) {
    glfwSetWindowSizeLimits(glfw_window, width, height, -1, -1);
}

void gfx::init(int window_width_, int window_height_) {
    window_width = window_width_;
    window_height = window_height_;
    
    glewExperimental = true;
    if(!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");
    
    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    glfw_window = glfwCreateWindow(window_width, window_height, "Test Window", nullptr, nullptr);
    glfwSetFramebufferSizeCallback(glfw_window, framebufferSizeCallback);
    glfwSetWindowContentScaleCallback(glfw_window, windowContentScaleCallback);
    glfwSetKeyCallback(glfw_window, gfx::keyCallback);
    glfwSetScrollCallback(glfw_window, gfx::scrollCallback);
    glfwSetCharCallback(glfw_window, gfx::characterCallback);
    glfwSetMouseButtonCallback(glfw_window, gfx::mouseButtonCallback);
    
    float scale_x, scale_y;
    glfwGetWindowContentScale(glfw_window, &scale_x, &scale_y);
    
    if(!glfw_window)
        throw std::runtime_error("Failed to open GLFW window.");
    
    glfwMakeContextCurrent(glfw_window);
    glewExperimental = true;
    if(glewInit() != GLEW_OK)
        throw std::runtime_error("Failed to initialize GLEW");
    
    //glfwSwapInterval(0); // this disabled vsync
    
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);
    
    glfwSetInputMode(glfw_window, GLFW_STICKY_KEYS, GL_TRUE);
    
    shader_program = CompileShaders(vertex_shader_code, fragment_shader_code);
    uniform_has_texture = glGetUniformLocation(shader_program, "has_texture");
    uniform_default_color = glGetUniformLocation(shader_program, "default_color");
    uniform_texture_sampler = glGetUniformLocation(shader_program, "texture_sampler");
    uniform_has_color_buffer = glGetUniformLocation(shader_program, "has_color_buffer");
    uniform_transform_matrix = glGetUniformLocation(shader_program, "transform_matrix");
    uniform_texture_transform_matrix = glGetUniformLocation(shader_program, "texture_transform_matrix");
    
    blur_shader_program = CompileShaders(blur_vertex_shader_code, blur_fragment_shader_code);
    
    uniform_blur_transform_matrix = glGetUniformLocation(blur_shader_program, "transform_matrix");
    uniform_blur_texture_transform_matrix = glGetUniformLocation(blur_shader_program, "texture_transform_matrix");
    uniform_blur_texture_sampler = glGetUniformLocation(blur_shader_program, "texture_sampler");
    uniform_blur_offset = glGetUniformLocation(blur_shader_program, "blur_offset");
    uniform_blur_limit = glGetUniformLocation(blur_shader_program, "limit");
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glUseProgram(shader_program);
    
    glGenTextures(1, &window_texture);
    glGenTextures(1, &window_texture_back);
    glGenFramebuffers(1, &default_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, default_framebuffer);
    
    windowContentScaleCallback(glfw_window, scale_x, scale_y);
    
    GLfloat rect_outline_vertex_array[] = {
        0.f, 0.f,
        1.f, 0.f,
        1.f, 1.f,
        0.f, 1.f,
    };
    
    glGenBuffers(1, &rect_outline_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, rect_outline_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect_outline_vertex_array), rect_outline_vertex_array, GL_STATIC_DRAW);
    
    GLfloat rect_vertex_array[] = {
        0.f, 0.f,
        1.f, 0.f,
        0.f, 1.f,
        1.f, 0.f,
        0.f, 1.f,
        1.f, 1.f,
    };
    
    glGenBuffers(1, &rect_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, rect_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect_vertex_array), rect_vertex_array, GL_STATIC_DRAW);
    
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);
    
    glEnableVertexAttribArray(0);
}

bool fontColEmpty(const unsigned char* data, int x, int y) {
    int curr_index = (256 - y) * 256 + x;
    for(int i = 0; i < 16; i++) {
        if(data[curr_index * 4 + 3] != 0)
            return false;
        curr_index -= 256;
    }
    
    return true;
}

void gfx::loadFont(const unsigned char* data) {
    font_texture.loadFromData(data, 256, 256);
    for(int y = 0; y < 16; y++)
        for(int x = 0; x < 16; x++) {
            RectShape rect(x * 16, y * 16, 16, 16);
            
            while(rect.w > 0 && fontColEmpty(data, rect.x, rect.y)) {
                rect.x++;
                rect.w--;
            }
            
            while(rect.w > 0 && fontColEmpty(data, rect.x + rect.w - 1, rect.y))
                rect.w--;
            
            if(y * 16 + x == ' ') {
                rect.x = x * 16;
                rect.w = 2;
            }
            
            font_rects[y * 16 + x] = rect;
        }
}

void gfx::quit() {
    glfwTerminate();
}

void gfx::addAGlobalUpdateFunction(GlobalUpdateFunction* global_update_function) {
    global_update_functions.push_back(global_update_function);
}

int gfx::getWindowWidth() {
    return window_width;
}

int gfx::getWindowHeight() {
    return window_height;
}

void gfx::updateWindow() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, window_width * global_scale_x, window_height * global_scale_y);
    
    Transformation texture_transform = window_normalization_transform;
    texture_transform.stretch(window_width * 0.5f, window_height * 0.5f);
    glUniformMatrix3fv(uniform_texture_transform_matrix, 1, GL_FALSE, texture_transform.getArray());
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, window_texture);
    
    glUniform1i(uniform_texture_sampler, 0);
    glUniform1i(uniform_has_texture, 1);
    glUniform1i(uniform_has_color_buffer, 0);
    Transformation transform = normalization_transform;
    
    transform.stretch(window_width, window_height);
    
    glUniformMatrix3fv(uniform_transform_matrix, 1, GL_FALSE, transform.getArray());
    glUniform4f(uniform_default_color, 1.f, 1.f, 1.f, 1.f);

    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, rect_vertex_buffer);
    glVertexAttribPointer(SHADER_VERTEX_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    
    glBindBuffer(GL_ARRAY_BUFFER, rect_vertex_buffer);
    glVertexAttribPointer(SHADER_TEXTURE_COORD_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(2);
    
    glfwSwapBuffers(glfw_window);
    
    glBindFramebuffer(GL_FRAMEBUFFER, default_framebuffer);
}

void gfx::sleep(float ms) {
    std::this_thread::sleep_for(std::chrono::microseconds(int(ms * 1000)));
}

void blurRect(gfx::RectShape rect, float offset_x, float offset_y) {
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gfx::window_texture_back, 0);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gfx::window_texture);

    glEnableVertexAttribArray(2);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(2);
    
    
    
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gfx::window_texture, 0);
    
    glUseProgram(gfx::blur_shader_program);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gfx::window_texture_back);
    
    glUniform2f(gfx::uniform_blur_offset, offset_x, offset_y);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glUseProgram(gfx::shader_program);
}

#define BLUR_QUALITY 2

void gfx::blurRectangle(RectShape rect, int radius) {
    glUseProgram(gfx::blur_shader_program);
    
    float x1 = (rect.x + 1) * gfx::window_width_reciprocal, y1 = (rect.y + 1) * gfx::window_height_reciprocal, x2 = (rect.x + rect.w) * gfx::window_width_reciprocal, y2 = (rect.y + rect.h) * gfx::window_height_reciprocal;
    glUniform4f(gfx::uniform_blur_limit, x2, -y1, x1, -y2);
    
    glUniform1i(gfx::uniform_blur_texture_sampler, 0);
    
    Transformation transform = gfx::normalization_transform;
    transform.translate(rect.x, rect.y);
    transform.stretch(rect.w, rect.h);
    
    glUniformMatrix3fv(gfx::uniform_blur_transform_matrix, 1, GL_FALSE, transform.getArray());
    
    transform.reset();
    transform.stretch(gfx::window_width_reciprocal, -gfx::window_height_reciprocal);
    transform.translate(rect.x, rect.y);
    transform.stretch(rect.w, rect.h);
    glUniformMatrix3fv(gfx::uniform_blur_texture_transform_matrix, 1, GL_FALSE, transform.getArray());
    
    glBindBuffer(GL_ARRAY_BUFFER, gfx::rect_vertex_buffer);
    glVertexAttribPointer(SHADER_VERTEX_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glVertexAttribPointer(SHADER_TEXTURE_COORD_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    
    
    
    glUseProgram(gfx::shader_program);
    
    glUniform1i(gfx::uniform_texture_sampler, 0);
    glUniform1i(gfx::uniform_has_texture, 1);
    glUniform1i(gfx::uniform_has_color_buffer, 0);
    transform = gfx::normalization_transform;
    
    transform.stretch(gfx::window_width, gfx::window_height);
    
    glUniformMatrix3fv(gfx::uniform_transform_matrix, 1, GL_FALSE, transform.getArray());
    glUniform4f(gfx::uniform_default_color, 1.f, 1.f, 1.f, 1.f);
    
    gfx::Transformation texture_transform = gfx::window_normalization_transform;
    texture_transform.stretch(gfx::window_width * 0.5f, gfx::window_height * 0.5f);
    glUniformMatrix3fv(gfx::uniform_texture_transform_matrix, 1, GL_FALSE, texture_transform.getArray());
    
    
    while(radius > 10) {
        blurRect(rect, window_width_reciprocal / 10 * radius, 0);
        blurRect(rect, 0, window_height_reciprocal / 10 * radius);
        radius = std::sqrt(radius) * BLUR_QUALITY;
    }
}