#pragma once
#include "render.h"
#include<vector>
#include"Mesh.h"
using namespace std;

// 基本z-buffer算法
class Zbuffer : public Render {
public:
	Zbuffer(int w, int h, unsigned char* framebuffer, Color bg_color = Color(0, 0, 0, 255));
	~Zbuffer();
	void drawTriangle(Vec2i* vis, float* Z, Color color);
	Vec2i world2screen(Vec3f v);
	virtual void drawMesh(std::vector<Mesh*>& triMeshs) override;
	virtual void clearZbuffer() override;

private:
	vector<vector<float>> zbuffer; // z缓冲器，记录当前z值(width, height)
	// 帧缓冲器在Render.h中定义，颜色绘制利用Render完成
};
