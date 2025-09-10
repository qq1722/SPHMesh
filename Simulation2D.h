#pragma once
#include <vector>
#include <glm/glm.hpp>

class Simulation2D {
public:
    // ���캯������������������ģ�������С
    Simulation2D(int num_particles, float domain_size);

    // ִ��һ��ʱ�䲽��ģ��
    void step();

    // ��ȡ�������ӵ�λ�ã�������Ⱦ
    const std::vector<glm::vec2>& get_particle_positions() const;

private:
    // ��ʼ��ʱ�������������
    void initialize_particles();

    // �������������ܵ����� (���ģ�)
    void compute_forces();

    // ���������������ӵ��ٶȺ�λ��
    void update_positions();

    // ����߽磬��ֹ�����ܳ�����
    void handle_boundaries();

    struct Particle {
        glm::vec2 position;
        glm::vec2 velocity;
        glm::vec2 force;
    };

    std::vector<Particle> particles_;
    std::vector<glm::vec2> positions_for_render_; // �����洢λ�ã����㴫���GPU

    int num_particles_;
    float domain_size_;

    // ģ�����
	float time_step_ = 0.002f;   // ʱ�䲽��
    float h_ = 0.25f;          // �ų������ð뾶 (���� L-infinity)
    float stiffness_ = 1000.0f; // ����ǿ��
    float damping_ = 0.99f;   // ���ᣬ����ϵͳ�ȶ�����
};