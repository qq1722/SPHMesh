#pragma once
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <string>

// 定义我们自己的网格类型，方便使用
struct MyTraits : public OpenMesh::DefaultTraits
{
    VertexAttributes(OpenMesh::Attributes::Normal); // 确保顶点有法线属性
    FaceAttributes(OpenMesh::Attributes::Normal);   // 确保面有法线属性
};
using MyMesh = OpenMesh::TriMesh_ArrayKernelT<MyTraits>;

class Mesh {
public:
    Mesh() = default;
    bool load(const std::string& filename);
    const MyMesh& get_mesh_data() const { return mesh_; }

private:
    MyMesh mesh_;
};