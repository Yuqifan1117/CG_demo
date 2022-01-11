#pragma once
#include"zbuffer.h"
#include"utils.h"

class HierarchialZBuffer : public Zbuffer {
	// û���漰�˲����Ĳ��Z-buffer�㷨�������޳��޷���������
public:
	HierarchialZBuffer(int w, int h, unsigned char* framebuffer, Color bg_color = Color(0, 0, 0, 255));
	void clearZbuffer();
	vector<float*> hi_zbuffers;  //�洢ÿһ���zbuffer
	vector<pair<int, int>> zbuffer_size;  // ÿһ��z-buffer�Ŀ�ȣ�zbuffer_len[i]��ʾ��i��zbuffer����ʾ��ͬ���ص��Ӧ��zbuffer
	int zbuffer_height{ 0 };
	int hidden_num{ 0 };
	bool in_single_block(int l, int min_x, int min_y, int max_x, int max_y);
	// �޳�������
	void drawNewTriangle(Vec2i* vis, float* Z, Color color);
	void drawHiMesh(vector<Mesh*>& triMeshs);
	void updateHiZbuffers(int x, int y, float z);
};
	