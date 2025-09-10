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

 //�����߼������� L-infinity ������������
void Simulation2D::compute_forces() {
    // 1. �Ƚ��������ӵ�������
    for (auto& p : particles_) {
        p.force = glm::vec2(0.0f, 0.0f);
    }

    // 2. ����ÿһ������֮����ų��� (O(N^2) ���Ӷȣ�����ԭ���㹻��)
    for (int i = 0; i < num_particles_; ++i) {
        for (int j = i + 1; j < num_particles_; ++j) {
            glm::vec2 diff = particles_[i].position - particles_[j].position;

            // ���� L-infinity ����
            float dist_inf = std::max(std::abs(diff.x), std::abs(diff.y));

            // ��������ð뾶�ڣ���ʩ��һ���򻯵ġ����ɡ�����
            if (dist_inf < h_ && dist_inf > 1e-6) {
                float force_magnitude = stiffness_ * (h_ - dist_inf);

                // ���ķ����������������� (L2����)�����Ǽ򻯵�����
                glm::vec2 dir = glm::normalize(diff);
                glm::vec2 force = force_magnitude * dir;

                particles_[i].force += force;
                particles_[j].force -= force; // ţ�ٵ�������
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
//                // �������Сʱ����ֹ��ĸΪ0�������������
//                float dist_l2_sq = diff.x * diff.x + diff.y * diff.y;
//                if (dist_l2_sq < 1e-8) {
//                    continue; // ����̫�������Դ����Ա��ⲻ�ȶ�
//                }
//
//                float force_magnitude = stiffness_ * (h_ - dist_inf);
//
//                // ���ķ�������L2���򣬵�����ȷ������ֵ�ȶ���
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
    float mass = 1.0f; // ����������������Ϊ1
    for (auto& p : particles_) {
        // ʹ�� Symplectic Euler ���֣����ȶ�
        p.velocity += (p.force / mass) * time_step_;
        p.velocity *= damping_; // ʩ������
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

    // ����������Ⱦ��λ������
    for (int i = 0; i < num_particles_; ++i) {
        positions_for_render_[i] = particles_[i].position;
    }
}

const std::vector<glm::vec2>& Simulation2D::get_particle_positions() const {
    return positions_for_render_;
}