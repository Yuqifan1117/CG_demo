#include "hierarchicalzbuffer.h"
#include<iostream>
#include<algorithm>
HierarchialZBuffer::HierarchialZBuffer(int w, int h, unsigned char * framebuffer, Color bg_color) :Zbuffer(w, h, framebuffer, bg_color) {
	int w_temp = w;
	// ������zbuffer���ڴ�
	while (w_temp >= 1)
	{
		float* zbuffer = (float*)malloc(w_temp * w_temp * sizeof(float));
		hi_zbuffers.push_back(zbuffer);
		zbuffer_size.push_back(make_pair(w_temp * w_temp, w_temp));
		w_temp = w_temp / 2; // ���ֻ��ֲ��zbuffer
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
// �õ���άƽ��������ε�����zbuffer���Ĳ���������, ���Ϊ0��

bool HierarchialZBuffer::in_single_block(int l, int min_x, int min_y, int max_x, int max_y) {
	// ���������ĳ��block����ô����һ�����۳�һ���㣬Ѱ�ҵ���Ӧ�Ĳ��zbuffer�Ĳ�����
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
	float z_max = -FLT_MAX;		// ������о����ӵ������zֵ
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
		// �������ξ����ӵ��zС�ڲ��zbuffer���е�z����ô��������
		if (in_single_block(i, min_x, min_y, max_x, max_y)) {
			int x = max_x / pow(2, i + 1);
			int y = max_y / pow(2, i + 1);
			if (hi_zbuffers[i][y * zbuffer_size[i].second + x] < z_max) {
				// ���������ж�
			}
			else {
				return; // ����ʧ�ܣ���������ж�
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

//ÿһ�������ǵ�ǰ���ĸ�������ȵ���Сֵ(���ӵ���Զ)
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
	// ���ݵ�0���zbuffer�ݹ��¼����zbuffer��ֵ
	// ֻ��Ҫ���±ȵ�ǰ����λ����zbufferС��z��λ��
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
		// �ο�opengl�Ĺ��ջ���ͼ�η���
		// ��ķ����� Vec3f light(0, 0, -1)
		Vec3f light(0, 0, 1);

		for (int i = 0; i < Mesh->face_number; ++i)
		{
			int* vids = Mesh->faces[i].v;	// �������еĶ������
			Vec3f vec3fs[3];	// �ռ��е�����
			Vec2i screens[3];	// ��Ļ�е�����
			float Zbuffer[3];
			// 3Dλ����Ϣȷ������֮����Ҫ��ͶӰ�任������3Dӳ�䵽2D��ͬʱ����zֵ
			for (int j = 0; j < 3; ++j)
			{
				vec3fs[j] = Mesh->verts[vids[j]]; // ���ݶ����±��ȡ������Ϣ
				screens[j] = world2screen(vec3fs[j]);
				Zbuffer[j] = vec3fs[j].z;
			}
			// ���㷨����������ɫ
			Vec3f n = (vec3fs[1] - vec3fs[0]).cross(vec3fs[2] - vec3fs[1]);		// ������Ƭ�ķ��������Ը�����ʱ������򣬸��������ߵĲ�˵ó���
			n = n.norm();
			// ����������ӦȨ��
			float color_w = n.dot(light);
			if (color_w > 0) drawNewTriangle(screens, Zbuffer, Color(255 * color_w, 255 * color_w, 255 * color_w, 255));
		}
	}
}