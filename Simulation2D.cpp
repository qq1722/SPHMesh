#include "Simulation2D.h"
#include <random>
#include <algorithm> // for std::max

Simulation2D::Simulation2D(int num_particles, float domain_size)
    : num_particles_(num_particles), domain_size_(domain_size) {
    particles_.resize(num_particles_);
    positions_for_render_.resize(num_particles_);
    initialize_particles();
}

void Simulation2D::initialize_particles() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, domain_size_);

    for (auto& p : particles_) {
        p.position = glm::vec2(dis(gen), dis(gen));
        p.velocity = glm::vec2(0.0f, 0.0f);
        p.force = glm::vec2(0.0f, 0.0f);
    }
}

 //核心逻辑：基于 L-infinity 范数的力计算
void Simulation2D::compute_forces() {
    // 1. 先将所有粒子的力清零
    for (auto& p : particles_) {
        p.force = glm::vec2(0.0f, 0.0f);
    }

    // 2. 计算每一对粒子之间的排斥力 (O(N^2) 复杂度，对于原型足够了)
    for (int i = 0; i < num_particles_; ++i) {
        for (int j = i + 1; j < num_particles_; ++j) {
            glm::vec2 diff = particles_[i].position - particles_[j].position;

            // 计算 L-infinity 距离
            float dist_inf = std::max(std::abs(diff.x), std::abs(diff.y));

            // 如果在作用半径内，则施加一个简化的“弹簧”斥力
            if (dist_inf < h_ && dist_inf > 1e-6) {
                float force_magnitude = stiffness_ * (h_ - dist_inf);

                // 力的方向是沿着两点连线 (L2方向)，这是简化的做法
                glm::vec2 dir = glm::normalize(diff);
                glm::vec2 force = force_magnitude * dir;

                particles_[i].force += force;
                particles_[j].force -= force; // 牛顿第三定律
            }
        }
    }
}

//void Simulation2D::compute_forces() {
//    for (auto& p : particles_) {
//        p.force = glm::vec2(0.0f, 0.0f);
//    }
//
//    for (int i = 0; i < num_particles_; ++i) {
//        for (int j = i + 1; j < num_particles_; ++j) {
//            glm::vec2 diff = particles_[i].position - particles_[j].position;
//
//            float dist_inf = std::max(std::abs(diff.x), std::abs(diff.y));
//
//            if (dist_inf < h_) {
//                // 当距离过小时，防止分母为0或产生无穷大的力
//                float dist_l2_sq = diff.x * diff.x + diff.y * diff.y;
//                if (dist_l2_sq < 1e-8) {
//                    continue; // 距离太近，忽略此力以避免不稳定
//                }
//
//                float force_magnitude = stiffness_ * (h_ - dist_inf);
//
//                // 力的方向还是沿L2方向，但我们确保了数值稳定性
//                glm::vec2 dir = diff / std::sqrt(dist_l2_sq);
//                glm::vec2 force = force_magnitude * dir;
//
//                particles_[i].force += force;
//                particles_[j].force -= force;
//            }
//        }
//    }
//}

void Simulation2D::update_positions() {
    float mass = 1.0f; // 假设所有粒子质量为1
    for (auto& p : particles_) {
        // 使用 Symplectic Euler 积分，更稳定
        p.velocity += (p.force / mass) * time_step_;
        p.velocity *= damping_; // 施加阻尼
        p.position += p.velocity * time_step_;
    }
}

void Simulation2D::handle_boundaries() {
    for (auto& p : particles_) {
        if (p.position.x < 0.0f) { p.position.x = 0.0f; p.velocity.x *= -0.5f; }
        if (p.position.x > domain_size_) { p.position.x = domain_size_; p.velocity.x *= -0.5f; }
        if (p.position.y < 0.0f) { p.position.y = 0.0f; p.velocity.y *= -0.5f; }
        if (p.position.y > domain_size_) { p.position.y = domain_size_; p.velocity.y *= -0.5f; }
    }
}

void Simulation2D::step() {
    compute_forces();
    update_positions();
    handle_boundaries();

    // 更新用于渲染的位置数据
    for (int i = 0; i < num_particles_; ++i) {
        positions_for_render_[i] = particles_[i].position;
    }
}

const std::vector<glm::vec2>& Simulation2D::get_particle_positions() const {
    return positions_for_render_;
}