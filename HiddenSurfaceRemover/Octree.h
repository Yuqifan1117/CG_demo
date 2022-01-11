#pragma once
#include "utils.h"
#include <vector>
#include "Mesh.h"


// �˲����ڵ㣬���ƶ�����ʵ�ַ�ʽ
struct OctreeNode
{
	// ѡ�񳡾�����İ�Χ����Ϊ��������
	BoundingBox boundingbox;   // �˲����������������������귶Χ
	vector<Face> trifaces;    // �洢�ڽڵ��е������ε���Ϣ���ڵ�����

	// t--top d--down l--left r--right f--front b--back
	// 0-tlf 1-tlb 2-trf 3-trb 4-dlf 5-dlb 6-drf  7-drb
	OctreeNode* nodes[8]; // �˲����ӽڵ�

	OctreeNode(){
		for (int i = 0; i < 8; ++i)
		{
			nodes[i] = nullptr;
		}
	}
	~OctreeNode(){
		for (int i = 0; i < 8; ++i)
		{
			if (nodes[i] != nullptr)
			{
				delete nodes[i];
			}
		}
	}
};

class Octree
{
public:
	OctreeNode* root{ nullptr };

	Octree();
	~Octree();

	void OctreeCreate(OctreeNode*& root, vector<Face> faces, vector<Vec3f>& vertices, int maxdepth, double xMin, double xMax, double yMin, double yMax, double zMin, double zMax);

	// void OctreeCreate(vector<Mesh*> triMeshs);
	void OctreeDestroy();

};