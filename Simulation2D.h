#pragma once
#include <vector>
#include <glm/glm.hpp>

class Simulation2D {
public:
    // 构造函数，传入粒子数量和模拟区域大小
    Simulation2D(int num_particles, float domain_size);

    // 执行一个时间步的模拟
    void step();

    // 获取所有粒子的位置，用于渲染
    const std::vector<glm::vec2>& get_particle_positions() const;

private:
    // 初始化时，随机放置粒子
    void initialize_particles();

    // 计算所有粒子受到的力 (核心！)
    void compute_forces();

    // 根据力来更新粒子的速度和位置
    void update_positions();

    // 处理边界，防止粒子跑出区域
    void handle_boundaries();

    struct Particle {
        glm::vec2 position;
        glm::vec2 velocity;
        glm::vec2 force;
    };

    std::vector<Particle> particles_;
    std::vector<glm::vec2> positions_for_render_; // 单独存储位置，方便传输给GPU

    int num_particles_;
    float domain_size_;

    // 模拟参数
	float time_step_ = 0.002f;   // 时间步长
    float h_ = 0.25f;          // 排斥力作用半径 (基于 L-infinity)
    float stiffness_ = 1000.0f; // 力的强度
    float damping_ = 0.99f;   // 阻尼，帮助系统稳定下来
};