#pragma once
#ifndef  _RENDER_H_  
#define  _RENDER_H_  
#include<vector>
#include"Mesh.h"
#include"utils.h"
class Render {
	/*
		��������࣬��ɶԱ����Լ�ǰ�����صĻ���
	*/
public:
	int width{ 800 };
	int height{ 600 };
	Color bg_color; // ������ɫ��֡�������е���ɫ

	unsigned char* framebuffer{ nullptr }; // ֡������

	Render(int w, int h, Color color, unsigned char* fb);
	~Render();
	void clear_framebuffer(); // ���֡������
	Vec3f barycentric(Vec2i* vis, Vec2i p); // �����жϵ��Ƿ����������ڣ������������ı�ʾ
	void draw_pixel(int x, int y, Color color); // �������ص�, �ο�https://github.com/ssloy/tinyrenderer/wiki/Lesson-0-getting-started
	void draw_line(int x1, int y1, int x2, int y2, Color color); // �����㷨������һ��ֱ�ߣ�ʹ��Bresenham�㷨���ο�CG Lesson02
	virtual void drawMesh(std::vector<Mesh *>& triMeshs) = 0;
	virtual void clearZbuffer() = 0;
};
#endif
