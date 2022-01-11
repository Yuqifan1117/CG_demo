#include "zbuffer.h"
#include<iostream>
#include<ctime>
#include <glm.hpp>
#include<algorithm>
Zbuffer::Zbuffer(int w, int h, unsigned char* framebuffer, Color bg_color): Render(w, h, bg_color, framebuffer)
{
	for (int i = 0; i < width ; ++i)
	{
		vector<float> temp;
		for (int j = 0; j < height; j++) {
			temp.push_back(-FLT_MAX);
		}
		zbuffer.push_back(temp);
	}
}

Zbuffer::~Zbuffer()
{
	
}

void Zbuffer::clearZbuffer()
{
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; j++) {
			zbuffer[i][j] = -FLT_MAX;
		}
	}
}

void Zbuffer::drawTriangle(Vec2i* vis, float* current_Z, Color color)
{
	/* 对整个三角面片上的像素进行光栅化
	光栅化过程:
	for (each triangle T)
		for (each sample (x,y,z) in T)
			if (z < zbuffer[x,y])  // closest sample so far
				framebuffer[x,y] = rgb; // update color
				zbuffer[x,y] = z;   // update depth
			else
				;  // do nothing, this sample is occluded
	*/
	int bboxmin_x = INT_MAX, bboxmin_y = INT_MAX;
	int bboxmax_x = INT_MIN, bboxmax_y = INT_MIN;
	for (int i = 0; i < 3; i++) {
		bboxmin_x = max(0, min(bboxmin_x, vis[i].x));
		bboxmin_y = max(0, min(bboxmin_y, vis[i].y));
		bboxmax_x = min(width - 1, max(bboxmax_x, vis[i].x));
		bboxmax_y = min(height - 1, max(bboxmax_y, vis[i].y));
	}
	// 利用三角形的 bounding_box, 看其中的每一个整数点，如果在三角形面片中，那就画之。
	for (int i = bboxmin_x; i < bboxmax_x; i++) {
		for (int j = bboxmin_y; j < bboxmax_y; j++) {
			Vec3f bc_screen = barycentric(vis, Vec2i(i,j));
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			float z = current_Z[0] * bc_screen.x + current_Z[1] * bc_screen.y + current_Z[2] * bc_screen.z;
			if (zbuffer[i][j] < z) {
				zbuffer[i][j] = z;
				draw_pixel(i, j, color);
			} 
		}
	}
}
void Zbuffer::drawMesh(vector<Mesh*>& triMeshs)
{
	clearZbuffer();
	for (Mesh* Mesh : triMeshs) 
	{
		// 参考opengl的光照绘制图形方法
		// 光的方向是 Vec3f light(0, 0, -1)
		Vec3f light(0, 0, -1);

		for (int i = 0; i < Mesh ->face_number; ++i)
		{
			int* vids = Mesh->faces[i].v;	// 三角形中的顶点序号
			Vec3f vec3fs[3];	// 空间中的坐标
			Vec2i screens[3];	// 屏幕中的坐标
			float Zbuffer[3];
			// 3D位置信息确定好了之后，需要做投影变换，即把3D映射到2D，同时保留z值
			for (int j = 0; j < 3; ++j)
			{
				vec3fs[j] = Mesh->verts[vids[j]]; // 根据顶点下标获取顶点信息
				screens[j] = world2screen(vec3fs[j]);
				Zbuffer[j] = vec3fs[j].z;
			}
			// 计算法向量进行着色

			Vec3f n = (vec3fs[2] - vec3fs[1]).cross(vec3fs[1] - vec3fs[0]);		// 三角面片的法向量可以根据逆时针的走向，根据两条边的叉乘得出。
			n = n.norm(); // 正则化
			// 法向量乘相应权重，得到相应光强
			float color_w = n.dot(light);
			if (color_w > 0) drawTriangle(screens, Zbuffer, Color(255*color_w, 255*color_w, 255*color_w, 255));
		}
		Sleep(Mesh->face_number / 150);
	}
}
Vec2i Zbuffer::world2screen(Vec3f v) {
	// 保留了int这个操作，用于画像素的for loop要用到x和y
	// float计算结果再加上0.5来四舍五入
	return Vec2i(int((v.x + 1.)*  width / 2. + .5), int(height - (v.y + 1.)*height / 2. + .5));
}
