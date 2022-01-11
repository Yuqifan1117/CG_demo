#include"render.h"
#include<algorithm>
#include<iostream>
using namespace std;


Render::Render(int w, int h, Color color, unsigned char* fb) : width(w), height(h) {
	framebuffer = fb;
	bg_color = color;
}

Render::~Render() {

}


void Render::draw_pixel(int x, int y, Color color)
{
	// ���ݸ���Ҫ�������ص���������rgba��ɫ�Ļ��ƣ�������ɫrgb��color������͸����Ĭ��Ϊ255
	if (x >= 0 && x < width&&y >= 0 && y < height) {
		// cout << (int)color.getB()<< (int)color.getG()<< (int)color.getR() <<endl;
		framebuffer[(y*width + x) * 4 + 0] = color.b;
		framebuffer[(y*width + x) * 4 + 1] = color.g;
		framebuffer[(y*width + x) * 4 + 2] = color.r;
		framebuffer[(y*width + x) * 4 + 3] = color.alpha; // ͸���ȣ���ʾ���岻͸��
	}
}
int sign(int x)
{
	// Sign(x) = 1,0,-1 ��x>0,=0,<0
	if (x < 0)
		return -1;
	else if (x > 0)
		return 1;
	else
		return 0;
}
void Render::draw_line(int x1, int y1, int x2, int y2, Color color)
{
	// ͨ��Bresenham�㷨
	int x = x1, y = y1;
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	int s1 = sign(x2 - x1), s2 = sign(y2 - y1);
	int interchange = 0;
	
	// ����ֱ��б�ʵķ��ţ�����dx��dy
	if (dy > dx) {
		int temp = dx;
		dx = dy;
		dy = temp;
		interchange = 1;
	}
	else {
		interchange = 0;
	}

	int e = 2 * dy - dx;// ����
	// ��ѭ��
	for (int i = 1; i <= dx; i++) {
		draw_pixel(x, y, color);
		while (e > 0) {
			if (interchange == 1) {
				x = x + s1;
			}
			else {
				y = y + s2;
			}
			e = e - 2 * dx;
		}
		if (interchange == 1) {
			y = y + s2;
		}
		else {
			x = x + s1;
		}
		e = e + 2 * dy;
	}
}

/*
��P��ʾ������������������������,
�����������Բ�ֵ���������������������
*/
// ����һ�������������е���������
Vec3f Render::barycentric(Vec2i* vis, Vec2i p)
{
	Vec3f v1 = Vec3f(vis[2].x - vis[0].x, vis[1].x - vis[0].x, vis[0].x - p.x);
	Vec3f v2 = Vec3f(vis[2].y - vis[0].y, vis[1].y - vis[0].y, vis[0].y - p.y);

	Vec3f u = v1.cross(v2);
	if (abs(u.z) < 1)
	{
		return Vec3f(-1, 1, 1);
	}
	return Vec3f(1.0f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}
void Render::clear_framebuffer()
{
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			framebuffer[(i*width + j) * 4 + 0] = bg_color.b;
			framebuffer[(i*width + j) * 4 + 1] = bg_color.g;
			framebuffer[(i*width + j) * 4 + 2] = bg_color.r;
			framebuffer[(i*width + j) * 4 + 3] = bg_color.alpha;
		}
	}
}


