#include "Mesh.h"
#include "Viewer.h"
#include "Simulation2D.h"
#include <iostream>

int main() {
    // ȷ����Ĺ���Ŀ¼���� bunny.obj ����ļ�
    // ����Դ� Stanford 3D Scanning Repository ����
    const char* modelPath = "test.obj";

    // --- 2D ԭ����֤ ---
    int num_particles = 400; // 20x20������
    float domain_size = 5.0f;
    Simulation2D sim(num_particles, domain_size);

    Viewer viewer(1280, 720, "SPH Remeshing - Stage 1.3: 2D Prototype");
    viewer.set_simulation2d(&sim);
    viewer.run();

    //Mesh mesh;
    //if (!mesh.load(modelPath)) {
    //    std::cerr << "Failed to load model: " << modelPath << std::endl;
    //    // ��ͣһ�£�������VS�￴������Ϣ
    //    system("pause");
    //    return -1;
    //}

    //// ����һ��1280x720�Ĵ���
    //Viewer viewer(1280, 720, "SPH Remeshing - Stage 1.2");

    //// �������������ø��۲���
    //viewer.set_mesh(&mesh);

    //// ������ѭ��
    //viewer.run();

    return 0;
}