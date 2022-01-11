#pragma once
#include "utils.h"
#include <vector>
#include "Mesh.h"


// 八叉树节点，类似二叉树实现方式
struct OctreeNode
{
	// 选择场景物体的包围盒作为根立方体
	BoundingBox boundingbox;   // 八叉树构成六面体三个轴坐标范围
	vector<Face> trifaces;    // 存储在节点中的三角形的信息，节点数据

	// t--top d--down l--left r--right f--front b--back
	// 0-tlf 1-tlb 2-trf 3-trb 4-dlf 5-dlb 6-drf  7-drb
	OctreeNode* nodes[8]; // 八叉树子节点

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