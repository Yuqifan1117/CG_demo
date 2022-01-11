#include "Octreehierarchicalzbuffer.h"
#include<iostream>
OctreeZbuffer::OctreeZbuffer(int w, int h, unsigned char * framebuffer, Color bg_color) :HierarchialZBuffer(w, h, framebuffer, bg_color) {

}

OctreeZbuffer::~OctreeZbuffer()
{
	for (int i = 0; i < hi_zbuffers.size(); ++i)
	{
		free(hi_zbuffers[i]);
	}
}

// 递归剖分八叉树进行深度测试绘制像素点
void OctreeZbuffer::drawOctreeNode(OctreeNode* current_node, int depth, vector<Vec3f>& vertices)
{
	if (current_node == nullptr)
	{
		return;
	}

	int x = (current_node->boundingbox.x_min + 1.0f) * width / 2.0f + .5;	//三维坐标转换为二维屏幕坐标
	int y = height - ((current_node->boundingbox.y_min + 1.0f) * height / 2.0f) + .5;	//三维坐标转换为二维屏幕坐标
	// 选择恰当层次zbuffer，对八叉树节点(立方体的面)进行深度测试, 根据八叉树节点中的包围盒的最大z值进行快速剔除
	int current_h = zbuffer_height - depth;
	for (int i = 0; i < current_h; ++i)
	{
		x /= 2;
		y /= 2;
	}
	// 如果测试失败，则整个节点被遮挡，停止；否则，剖分该八叉树节点
	// cout << hi_zbuffers[current_h][y * int(pow(2, depth - 1)) + x] << endl;
	int m = zbuffer_size[current_h].second;
	if (current_node->boundingbox.z_max < hi_zbuffers[current_h][y * m + x])
	{
		oct_hidden_num += 1;
		return;
	}
	// 优先递归测试子节点并更新ZBuffer
	for (int i = 0; i < 8; ++i)
	{
		drawOctreeNode(current_node->nodes[i], depth + 1, vertices);
	}

	// 绘制三角形
	vector<Face> tris = current_node->trifaces;
	Vec3f light(0, 0, 1);
	for (int i = 0; i < tris.size(); ++i)
	{
		Vec3f vec3fs[3];
		Vec2i vec2is[3];	// 屏幕中的坐标
		float Z[3];

		for (int j = 0; j < 3; ++j)
		{
			vec3fs[j] = vertices[tris[i].v[j]];
			vec2is[j] = world2screen(vec3fs[j]);
			Z[j] = vec3fs[j].z;
		}
		// 计算法向量进行着色
		Vec3f n = (vec3fs[1] - vec3fs[0]).cross(vec3fs[2] - vec3fs[1]);		// 三角面片的法向量可以根据逆时针的走向，根据两条边的叉乘得出。
		n = n.norm();
		// 法向量乘相应权重
		float color_w = n.dot(light);
		int b = rand() % 256;
		int g = rand() % 256;
		int r = rand() % 256;
		if (color_w > 0) drawNewTriangle(vec2is, Z, Color(255 * color_w, 255 * color_w, 255 * color_w, 255));
	}
}

void OctreeZbuffer::drawOctMesh(vector<Mesh*> triMeshs) {
	clearZbuffer();
	for (Mesh* Mesh : triMeshs) {
		Octree octree;
		hidden_num = 0;
		oct_hidden_num = 0;
		OctreeNode* r = new OctreeNode;
		int max_depth = 4;
		// octree.OctreeCreate(r, Mesh->faces, Mesh->verts, max_depth, Mesh->bounding_box.x_min, Mesh->bounding_box.x_max, Mesh->bounding_box.y_min, Mesh->bounding_box.y_max, Mesh->bounding_box.z_min, Mesh->bounding_box.z_max);
		octree.OctreeCreate(r, Mesh->faces, Mesh->verts, max_depth, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
		octree.root = r;
		drawOctreeNode(octree.root, 1, Mesh->verts);
		octree.OctreeDestroy();		// 销毁八叉树
	}
}