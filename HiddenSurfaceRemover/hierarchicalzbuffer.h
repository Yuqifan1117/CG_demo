#pragma once
#include"zbuffer.h"
#include"utils.h"

class HierarchialZBuffer : public Zbuffer {
	// 没有涉及八叉树的层次Z-buffer算法，快速剔除无法看到的面
public:
	HierarchialZBuffer(int w, int h, unsigned char* framebuffer, Color bg_color = Color(0, 0, 0, 255));
	void clearZbuffer();
	vector<float*> hi_zbuffers;  //存储每一层的zbuffer
	vector<pair<int, int>> zbuffer_size;  // 每一层z-buffer的宽度，zbuffer_len[i]表示第i层zbuffer，表示不同像素点对应的zbuffer
	int zbuffer_height{ 0 };
	int hidden_num{ 0 };
	bool in_single_block(int l, int min_x, int min_y, int max_x, int max_y);
	// 剔除的面数
	void drawNewTriangle(Vec2i* vis, float* Z, Color color);
	void drawHiMesh(vector<Mesh*>& triMeshs);
	void updateHiZbuffers(int x, int y, float z);
};
	