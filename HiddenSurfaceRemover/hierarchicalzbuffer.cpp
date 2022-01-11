#include "hierarchicalzbuffer.h"
#include<iostream>
#include<algorithm>
HierarchialZBuffer::HierarchialZBuffer(int w, int h, unsigned char * framebuffer, Color bg_color) :Zbuffer(w, h, framebuffer, bg_color) {
	int w_temp = w;
	// 分配层次zbuffer的内存
	while (w_temp >= 1)
	{
		float* zbuffer = (float*)malloc(w_temp * w_temp * sizeof(float));
		hi_zbuffers.push_back(zbuffer);
		zbuffer_size.push_back(make_pair(w_temp * w_temp, w_temp));
		w_temp = w_temp / 2; // 二分划分层次zbuffer
	}
	zbuffer_height = hi_zbuffers.size();
}
void HierarchialZBuffer::clearZbuffer()
{
	for (int i = 0; i < hi_zbuffers.size(); ++i)
	{
		float* zbuffer = hi_zbuffers[i];
		for (int j = 0; j < zbuffer_size[i].first; ++j)
		{
			zbuffer[j] = -FLT_MAX;
		}
	}
}
// 得到二维平面的三角形的所在zbuffer树的层数和坐标, 最低为0层

bool HierarchialZBuffer::in_single_block(int l, int min_x, int min_y, int max_x, int max_y) {
	// 如果是属于某个block，那么往后一层则汇聚成一个点，寻找到对应的层次zbuffer的层数。
	min_x /= pow(2.0, l+1); 
	min_y /= pow(2.0, l+1);
	max_x /= pow(2.0, l+1);
	max_y /= pow(2.0, l+1);
	if ((min_y != max_y) || (min_x != max_y)) return false;
	else return true;
}
void HierarchialZBuffer::drawNewTriangle(Vec2i * vis, float * Z, Color color)
{
	int max_x = 0, min_x = width - 1, max_y = 0, min_y = height - 1;
	float z_max = -FLT_MAX;		// 多边形中距离视点最近的z值
	for (int i = 0; i < 3; ++i)
	{
		max_x = max(vis[i].x, max_x);
		min_x = min(vis[i].x, min_x);
		max_y = max(vis[i].y, max_y);
		min_y = min(vis[i].y, min_y);
	}
	for (int i = min_x; i <= max_x; i++) {
		for (int j = min_y; j <= max_y; j++) {
			Vec3f bc_screen = barycentric(vis, Vec2i(i, j));
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			float z = Z[2] * bc_screen.x + Z[1] * bc_screen.y + Z[0] * bc_screen.z;
			if (z > z_max) {
				z_max = z;
			}
			
		}
	}

	for (int i = zbuffer_height - 1; i >= 0; i--) {
		// 如果多边形距离视点的z小于层次zbuffer树中的z，那么不被绘制
		if (in_single_block(i, min_x, min_y, max_x, max_y)) {
			int x = max_x / pow(2, i + 1);
			int y = max_y / pow(2, i + 1);
			if (hi_zbuffers[i][y * zbuffer_size[i].second + x] < z_max) {
				// 继续进行判断
			}
			else {
				return; // 测试失败，无需继续判断
			}
		}
	}
		for (int i = min_x; i < max_x; i++) {
			for (int j = min_y; j < max_y; j++) {
				Vec3f bc_screen = barycentric(vis, Vec2i(i, j));
				if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
				float z = Z[2] * bc_screen.x + Z[1] * bc_screen.y + Z[0] * bc_screen.z;
				if (hi_zbuffers[0][i + j * width] < z) {
					hi_zbuffers[0][i + j * width] = z;
					draw_pixel(i, j, color);
					updateHiZbuffers(i, j, z);
				}
			}
		}
	
	return;
}

//每一个像素是当前层四个像素深度的最小值(离视点最远)
float z_Min(float z1, float z2, float z3, float z4) {
	float Min = FLT_MAX;
	Min = min(Min, z1);
	Min = min(Min, z2);
	Min = min(Min, z3);
	Min = min(Min, z4);
	return Min;
}

void HierarchialZBuffer::updateHiZbuffers(int x, int y, float z)
{
	// 根据第0层的zbuffer递归记录后续zbuffer的值
	// 只需要更新比当前像素位置下zbuffer小于z的位置
	for (int i = 1; i < zbuffer_height; ++i)
	{

		if (x % 2 == 1) x--;
		if (y % 2 == 1) y--;
		
		/*if (z < hi_zbuffers[i][(y / 2) * zbuffer_size[i].second + x / 2]) {
			hi_zbuffers[i][(y / 2) * zbuffer_size[i].second + x / 2] = z;
		}*/
		hi_zbuffers[i][(y / 2) * zbuffer_size[i].second + x / 2] = z_Min(
			hi_zbuffers[i - 1][y * zbuffer_size[i - 1].second + x],
			hi_zbuffers[i - 1][y * zbuffer_size[i - 1].second + x + 1],
			hi_zbuffers[i - 1][(y + 1) * zbuffer_size[i - 1].second + x],
			hi_zbuffers[i - 1][(y + 1) * zbuffer_size[i - 1].second + x + 1]
		);
		x /= 2;
		y /= 2;
	}
}
void HierarchialZBuffer::drawHiMesh(vector<Mesh*>& triMeshs)
{
	this->clearZbuffer();
	for (Mesh* Mesh : triMeshs)
	{
		// 参考opengl的光照绘制图形方法
		// 光的方向是 Vec3f light(0, 0, -1)
		Vec3f light(0, 0, 1);

		for (int i = 0; i < Mesh->face_number; ++i)
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
			Vec3f n = (vec3fs[1] - vec3fs[0]).cross(vec3fs[2] - vec3fs[1]);		// 三角面片的法向量可以根据逆时针的走向，根据两条边的叉乘得出。
			n = n.norm();
			// 法向量乘相应权重
			float color_w = n.dot(light);
			if (color_w > 0) drawNewTriangle(screens, Zbuffer, Color(255 * color_w, 255 * color_w, 255 * color_w, 255));
		}
	}
}