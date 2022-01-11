#pragma once
#include "render.h"
#include<vector>
#include"Mesh.h"
using namespace std;

// ����z-buffer�㷨
class Zbuffer : public Render {
public:
	Zbuffer(int w, int h, unsigned char* framebuffer, Color bg_color = Color(0, 0, 0, 255));
	~Zbuffer();
	void drawTriangle(Vec2i* vis, float* Z, Color color);
	Vec2i world2screen(Vec3f v);
	virtual void drawMesh(std::vector<Mesh*>& triMeshs) override;
	virtual void clearZbuffer() override;

private:
	vector<vector<float>> zbuffer; // z����������¼��ǰzֵ(width, height)
	// ֡��������Render.h�ж��壬��ɫ��������Render���
};
