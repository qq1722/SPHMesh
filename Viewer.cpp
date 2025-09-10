#include "Viewer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

Viewer::Viewer(int width, int height, const std::string& title)
    : width_(width), height_(height), title_(title), window_(nullptr) {
    init();
}

Viewer::~Viewer() {
    delete shader_;
    glDeleteVertexArrays(1, &VAO_);
    glDeleteBuffers(1, &VBO_);
    glDeleteBuffers(1, &EBO_);
    if (window_) {
        glfwDestroyWindow(window_);
    }
    glfwTerminate();
}

void Viewer::init() {
    // ---- GLFW 初始化 ----
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_ = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
    if (window_ == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window_);

    // ---- GLAD 初始化 ----
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // ---- 设置回调函数 ----
    // 将this指针存储在窗口的用户指针中，以便在静态回调中访问Viewer实例
    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window_, mouse_button_callback);
    glfwSetCursorPosCallback(window_, cursor_pos_callback);
    glfwSetScrollCallback(window_, scroll_callback);

    // ---- OpenGL 全局状态设置 ----
    glEnable(GL_DEPTH_TEST); // 开启深度测试
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 使用线框模式渲染，方便观察网格
}

void Viewer::set_mesh(Mesh* mesh) {
    mesh_ = mesh;
    setup_buffers();
}

void Viewer::setup_buffers() {
    if (!mesh_) return;

    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    glGenBuffers(1, &EBO_);

    glBindVertexArray(VAO_);

    // 提取顶点数据
    const auto& mesh_data = mesh_->get_mesh_data();
    std::vector<float> vertices;
    vertices.reserve(mesh_data.n_vertices() * 3);
    for (const auto& vh : mesh_data.vertices()) {
        const auto& p = mesh_data.point(vh);
        vertices.push_back(p[0]);
        vertices.push_back(p[1]);
        vertices.push_back(p[2]);
    }
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // 提取面索引数据
    std::vector<unsigned int> indices;
    indices.reserve(mesh_data.n_faces() * 3);
    for (const auto& fh : mesh_data.faces()) {
        for (const auto& fvh : mesh_data.fv_range(fh)) {
            indices.push_back(fvh.idx());
        }
    }
    indices_count_ = indices.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


//void Viewer::run() {
//    shader_ = new Shader("shaders/simple.vert", "shaders/simple.frag");
//    update_camera_vectors();
//    main_loop();
//}
//
//void Viewer::main_loop() {
//    while (!glfwWindowShouldClose(window_)) {
//        process_input();
//
//        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        shader_->use();
//
//        glm::mat4 model = glm::mat4(1.0f);
//        glm::mat4 view = glm::lookAt(camera_pos_, camera_target_, camera_up_);
//        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width_ / (float)height_, 0.1f, 100.0f);
//
//        shader_->setMat4("model", model);
//        shader_->setMat4("view", view);
//        shader_->setMat4("projection", projection);
//
//        if (VAO_ != 0) {
//            glBindVertexArray(VAO_);
//            glDrawElements(GL_TRIANGLES, indices_count_, GL_UNSIGNED_INT, 0);
//        }
//
//        glfwSwapBuffers(window_);
//        glfwPollEvents();
//    }
//}

void Viewer::process_input() {
    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window_, true);
}

void Viewer::update_camera_vectors() {
    // 从球坐标计算摄像机的笛卡尔坐标位置
    float x = camera_target_.x + camera_radius_ * cos(glm::radians(camera_yaw_)) * cos(glm::radians(camera_pitch_));
    float y = camera_target_.y + camera_radius_ * sin(glm::radians(camera_pitch_));
    float z = camera_target_.z + camera_radius_ * sin(glm::radians(camera_yaw_)) * cos(glm::radians(camera_pitch_));
    camera_pos_ = glm::vec3(x, y, z);
}

// ---- 静态回调函数实现 ----

void Viewer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    auto* viewer = static_cast<Viewer*>(glfwGetWindowUserPointer(window));
    if (viewer) {
        viewer->width_ = width;
        viewer->height_ = height;
    }
}

void Viewer::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    auto* viewer = static_cast<Viewer*>(glfwGetWindowUserPointer(window));
    if (viewer) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                viewer->mouse_left_pressed_ = true;
                glfwGetCursorPos(window, &viewer->last_x_, &viewer->last_y_);
            }
            else if (action == GLFW_RELEASE) {
                viewer->mouse_left_pressed_ = false;
            }
        }
    }
}

void Viewer::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    auto* viewer = static_cast<Viewer*>(glfwGetWindowUserPointer(window));
    if (viewer && viewer->mouse_left_pressed_) {
        float xoffset = xpos - viewer->last_x_;
        float yoffset = viewer->last_y_ - ypos; // 注意y轴是反的
        viewer->last_x_ = xpos;
        viewer->last_y_ = ypos;

        float sensitivity = 0.2f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        viewer->camera_yaw_ += xoffset;
        viewer->camera_pitch_ += yoffset;

        // 限制俯仰角，防止万向节死锁
        if (viewer->camera_pitch_ > 89.0f)
            viewer->camera_pitch_ = 89.0f;
        if (viewer->camera_pitch_ < -89.0f)
            viewer->camera_pitch_ = -89.0f;

        viewer->update_camera_vectors();
    }
}

void Viewer::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    auto* viewer = static_cast<Viewer*>(glfwGetWindowUserPointer(window));
    if (viewer) {
        viewer->camera_radius_ -= (float)yoffset * 0.5f;
        if (viewer->camera_radius_ < 1.0f)
            viewer->camera_radius_ = 1.0f;
        if (viewer->camera_radius_ > 20.0f)
            viewer->camera_radius_ = 20.0f;

        viewer->update_camera_vectors();
    }
}


void Viewer::set_simulation2d(Simulation2D* sim) {
    sim2d_ = sim;
    // 切换到线框模式，方便观察
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    setup_2d_buffers();
}

void Viewer::setup_2d_buffers() {
    if (!sim2d_) return;

    glGenVertexArrays(1, &VAO_2d_);
    glGenBuffers(1, &VBO_2d_);

    glBindVertexArray(VAO_2d_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_2d_);

    // 先分配空间，但暂时不上传数据，因为数据是动态的
    glBufferData(GL_ARRAY_BUFFER, sim2d_->get_particle_positions().size() * sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Viewer::update_2d_buffers() {
    if (!sim2d_ || VBO_2d_ == 0) return;

    const auto& positions = sim2d_->get_particle_positions();
    glBindBuffer(GL_ARRAY_BUFFER, VBO_2d_);
    // 每帧都更新VBO中的数据
    glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(glm::vec2), positions.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


// 修改 run() 和 main_loop() 方法
void Viewer::run() {
    // 根据是2D还是3D来加载不同的shader
    if (sim2d_) {
        point_shader_ = new Shader("shaders/point.vert", "shaders/point.frag");
    }
    else {
        shader_ = new Shader("shaders/simple.vert", "shaders/simple.frag");
    }
    update_camera_vectors();
    main_loop();
}

void Viewer::main_loop() {
    while (!glfwWindowShouldClose(window_)) {
        process_input();

        // 如果是2D模拟，每帧都执行一步
        if (sim2d_) {
            sim2d_->step();
            update_2d_buffers();
        }

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // --- 矩阵变换，让2D平面看起来像在地上 ---
        glm::mat4 model = glm::mat4(1.0f);
        // 将2D模拟的中心平移到原点
        if (sim2d_) {
            float domain_size = 5.0f; // 假设和Simulation2D里的一致
            model = glm::translate(model, glm::vec3(-domain_size / 2.0f, -domain_size / 2.0f, 0.0f));
        }
        glm::mat4 view = glm::lookAt(camera_pos_, camera_target_, camera_up_);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width_ / (float)height_, 0.1f, 100.0f);

        // --- 根据模式选择渲染路径 ---
        if (mesh_ && shader_) {
            shader_->use();
            shader_->setMat4("model", model);
            shader_->setMat4("view", view);
            shader_->setMat4("projection", projection);
            glBindVertexArray(VAO_);
            glDrawElements(GL_TRIANGLES, indices_count_, GL_UNSIGNED_INT, 0);
        }

        if (sim2d_ && point_shader_) {
            // 开启 gl_PointSize
            glEnable(GL_PROGRAM_POINT_SIZE);
            point_shader_->use();
            point_shader_->setMat4("model", model);
            point_shader_->setMat4("view", view);
            point_shader_->setMat4("projection", projection);
            glBindVertexArray(VAO_2d_);
            glDrawArrays(GL_POINTS, 0, sim2d_->get_particle_positions().size());
            glDisable(GL_PROGRAM_POINT_SIZE);
        }


        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
}