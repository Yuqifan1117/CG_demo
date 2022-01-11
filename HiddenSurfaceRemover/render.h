#pragma once
#ifndef  _RENDER_H_  
#define  _RENDER_H_  
#include<vector>
#include"Mesh.h"
#include"utils.h"
class Render {
	/*
		定义绘制类，完成对背景以及前置像素的绘制
	*/
public:
	int width{ 800 };
	int height{ 600 };
	Color bg_color; // 背景颜色，帧缓冲器中的颜色

	unsigned char* framebuffer{ nullptr }; // 帧缓冲器

	Render(int w, int h, Color color, unsigned char* fb);
	~Render();
	void clear_framebuffer(); // 清空帧缓冲器
	Vec3f barycentric(Vec2i* vis, Vec2i p); // 用于判断点是否在三角形内，三角形用重心表示
	void draw_pixel(int x, int y, Color color); // 绘制像素点, 参考https://github.com/ssloy/tinyrenderer/wiki/Lesson-0-getting-started
	void draw_line(int x1, int y1, int x2, int y2, Color color); // 画线算法，绘制一条直线，使用Bresenham算法，参考CG Lesson02
	virtual void drawMesh(std::vector<Mesh *>& triMeshs) = 0;
	virtual void clearZbuffer() = 0;
};
#endif
