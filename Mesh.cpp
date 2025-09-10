#include "Mesh.h"
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <iostream>

bool Mesh::load(const std::string& filename) {
    if (!OpenMesh::IO::read_mesh(mesh_, filename)) {
        std::cerr << "Error: Cannot read mesh from file " << filename << std::endl;
        return false;
    }

    // 加载后的一些必要处理
    mesh_.request_face_normals();
    mesh_.request_vertex_normals();
    mesh_.update_normals();

    std::cout << "Mesh loaded successfully: " << std::endl;
    std::cout << "  Vertices: " << mesh_.n_vertices() << std::endl;
    std::cout << "  Faces: " << mesh_.n_faces() << std::endl;
    return true;
}