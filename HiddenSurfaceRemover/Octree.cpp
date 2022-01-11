#include "Octree.h"
#include<iostream>
Octree::Octree()
{
}

Octree::~Octree()
{
	OctreeDestroy();
}
bool getFaceLocation(Face face, vector<Vec3f>& vertices, float& min_x, float& min_y,
	float& min_z, float& max_x, float& max_y, float& max_z,
	double xMin, double xMax, double yMin, double yMax, double zMin, double zMax);
bool intersect(float min_value, float max_value, double min_target, double max_target);
void Octree::OctreeCreate(OctreeNode*& root, vector<Face> faces, vector<Vec3f>& vertices, int maxdepth, double xMin, double xMax, double yMin, double yMax, double zMin, double zMax)
{
	maxdepth--;
	
	if (maxdepth >= 0) {
		root = new OctreeNode;
		root->boundingbox = BoundingBox(xMin, xMax, yMin, yMax, zMin, zMax);
		vector<Face> face_list;
		root->trifaces = face_list;
		// 用于计算包围盒的center
		double mid_x = (xMax - xMin) / 2;//计算节点个维度上的半边长
		double mid_y = (yMax - yMin) / 2;
		double mid_z = (zMax - zMin) / 2;
		// 判断面片属于哪些block
		for (int i = 0; i < faces.size(); i++)
		{
			float min_x, min_y, min_z, max_x, max_y, max_z;
			Vec3f face[3];
			for (int j = 0; j < 3; j++) {
				face[j] = vertices[faces[i].v[j]];
			}
			//bool inBlock = root->boundingbox.containTri(face);
			bool inBlock = getFaceLocation(faces[i], vertices, min_x, min_y, min_z, max_x, max_y, max_z,
				xMin, xMax, yMin, yMax, zMin, zMax);
			if (inBlock) root->trifaces.push_back(faces[i]);

		}
		// 如果立方体中包含的多边形很少，则这些多边形属于该立方体，并退出不再进行剖分
		if (faces.size() <= 10) return;
		//递归创建子树，根据每一个节点的位置决定其子结点的坐标，包围盒的边界划分到立方体的1/8中
		OctreeCreate(root->nodes[0], root->trifaces, vertices, maxdepth, xMin, xMax - mid_x, yMax - mid_y, yMax, zMax - mid_z, zMax);
		OctreeCreate(root->nodes[1], root->trifaces, vertices, maxdepth, xMin, xMax - mid_x, yMin, yMax - mid_y, zMax - mid_z, zMax);
		OctreeCreate(root->nodes[2], root->trifaces, vertices, maxdepth, xMax - mid_x, xMax, yMax - mid_y, yMax, zMax - mid_z, zMax);
		OctreeCreate(root->nodes[3], root->trifaces, vertices, maxdepth, xMax - mid_x, xMax, yMin, yMax - mid_y, zMax - mid_z, zMax);
		OctreeCreate(root->nodes[4], root->trifaces, vertices, maxdepth, xMin, xMax - mid_x, yMax - mid_y, yMax, zMin, zMax - mid_z);
		OctreeCreate(root->nodes[5], root->trifaces, vertices, maxdepth, xMin, xMax - mid_x, yMin, yMax - mid_y, zMin, zMax - mid_z);
		OctreeCreate(root->nodes[6], root->trifaces, vertices, maxdepth, xMax - mid_x, xMax, yMax - mid_y, yMax, zMin, zMax - mid_z);
		OctreeCreate(root->nodes[7], root->trifaces, vertices, maxdepth, xMax - mid_x, xMax, yMin, yMax - mid_y, zMin, zMax - mid_z);
	}
}
bool getFaceLocation(Face face, vector<Vec3f>& vertices, float& min_x, float& min_y,
	float& min_z, float& max_x, float& max_y, float& max_z,
	double xMin, double xMax, double yMin, double yMax, double zMin, double zMax) {
	int f_idx = face.v[0];
	int s_idx = face.v[1];
	int t_idx = face.v[2];
	Vec3f f_vert = (vertices)[f_idx];
	Vec3f s_vert = (vertices)[s_idx];
	Vec3f t_vert = (vertices)[t_idx];

	// 遍历面片所在的矩形区域
	min_x = min(f_vert.x, min(s_vert.x, t_vert.x));
	max_x = max(f_vert.x, max(s_vert.x, t_vert.x));
	min_y = min(f_vert.y, min(s_vert.y, t_vert.y));
	max_y = max(f_vert.y, max(s_vert.y, t_vert.y));
	min_z = min(f_vert.z, min(s_vert.z, t_vert.z));
	max_z = max(f_vert.z, max(s_vert.z, t_vert.z));

	/* cout << "(" << min_x << "," << min_y << "," << min_z << ")" << "(" << max_x << "," << max_y << "," << max_z << ")" <<
		 "(" << xMin << "," << yMin << "," << zMin << ")" << "(" << xMax << "," << yMax << "," << zMax << ")" << endl;*/
	bool inBlock = false;
	// 三个方向上都存在投影，则两个AABB-box相交
	if (intersect(min_x, max_x, xMin, xMax) && intersect(min_y, max_y, yMin, yMax) && intersect(min_z, max_z, zMin, zMax)) {
		inBlock = true;
	}

	return inBlock;

}

bool intersect(float min_value, float max_value, double min_target, double max_target) {
	if ((max_value <= min_target) || (min_value >= max_target)) return false;
	else return true;
}
void Octree::OctreeDestroy()
{
	if (root != nullptr)
	{
		delete root;
		root = nullptr;
	}
}
