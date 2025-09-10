#include "Mesh.h"
#include "Viewer.h"
#include "Simulation2D.h"
#include <iostream>

int main() {
    // 确保你的工作目录下有 bunny.obj 这个文件
    // 你可以从 Stanford 3D Scanning Repository 下载
    const char* modelPath = "test.obj";

    // --- 2D 原型验证 ---
    int num_particles = 400; // 20x20的网格
    float domain_size = 5.0f;
    Simulation2D sim(num_particles, domain_size);

    Viewer viewer(1280, 720, "SPH Remeshing - Stage 1.3: 2D Prototype");
    viewer.set_simulation2d(&sim);
    viewer.run();

    //Mesh mesh;
    //if (!mesh.load(modelPath)) {
    //    std::cerr << "Failed to load model: " << modelPath << std::endl;
    //    // 暂停一下，方便在VS里看错误信息
    //    system("pause");
    //    return -1;
    //}

    //// 创建一个1280x720的窗口
    //Viewer viewer(1280, 720, "SPH Remeshing - Stage 1.2");

    //// 将网格数据设置给观察器
    //viewer.set_mesh(&mesh);

    //// 启动主循环
    //viewer.run();

    return 0;
}