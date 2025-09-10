#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <string>

#include "Shader.h"
#include "Mesh.h"

#include "Simulation2D.h" 

class Viewer {
public:
    Viewer(int width, int height, const std::string& title);
    ~Viewer();

    // ��ֹ�����͸�ֵ
    Viewer(const Viewer&) = delete;
    Viewer& operator=(const Viewer&) = delete;

    void set_mesh(Mesh* mesh);
    void run();

    void set_simulation2d(Simulation2D* sim);

private:
    // ��ʼ��
    void init();

    // ����GPU����
    void setup_buffers();

    // ��ѭ��
    void main_loop();

    // �����������
    void process_input();

    // �����������
    void update_camera_vectors();

    // GLFW�ص����� (��̬)
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    void setup_2d_buffers();
    void update_2d_buffers();

private:
    // ��������
    GLFWwindow* window_;
    int width_, height_;
    std::string title_;

    // ��Ⱦ���
    Shader* shader_ = nullptr;
    unsigned int VAO_ = 0, VBO_ = 0, EBO_ = 0;
    unsigned int indices_count_ = 0;

    // ����
    Mesh* mesh_ = nullptr;

    // ������� (Arcball Camera)
    glm::vec3 camera_target_ = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 camera_up_ = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 camera_pos_;
    float camera_radius_ = 5.0f;
    float camera_yaw_ = -90.0f; // Yaw, ƫ����
    float camera_pitch_ = 0.0f; // Pitch, ������

    // ���״̬
    bool mouse_left_pressed_ = false;
    double last_x_, last_y_;



    Simulation2D* sim2d_ = nullptr;
    Shader* point_shader_ = nullptr;
    unsigned int VAO_2d_ = 0, VBO_2d_ = 0;
};