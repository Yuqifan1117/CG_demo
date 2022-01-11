#pragma once
#include "hierarchicalzbuffer.h"
#include "Octree.h"

class OctreeZbuffer :public HierarchialZBuffer {
	// �̳���ͨ�Ĳ��ZBufferͬ��ʹ�ò��ZBuffer
public:
	OctreeZbuffer(int w, int h, unsigned char* fb, Color back_color);
	~OctreeZbuffer();
	void drawOctreeNode(OctreeNode * current_node, int depth, vector<Vec3f>& vertices);
	void drawOctMesh(vector<Mesh*> triMeshs);
	int oct_hidden_num{ 0 }; // ͨ���˲����޳�������
};
