#pragma once
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <string>

// ���������Լ����������ͣ�����ʹ��
struct MyTraits : public OpenMesh::DefaultTraits
{
    VertexAttributes(OpenMesh::Attributes::Normal); // ȷ�������з�������
    FaceAttributes(OpenMesh::Attributes::Normal);   // ȷ�����з�������
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