#pragma once
#include<vector>
#include<string>
#include"utils.h"
using namespace std;

struct Face {
	//（三角形）面的结构体
	int v[3]; // 顶点的下标序号
	int id; // 面的序号
	//float normal[3];         //记录点的法向量，分别是x，y，z三个方向
};

class Mesh {
public:
	vector<Vec3f> verts;  // 记录所有的顶点信息
	vector<Face> faces;  // 记录所有的三角面片
	//模型的包围盒  
	BoundingBox bounding_box;
	int vert_number{ 0 };
	int face_number{ 0 };

	void ReadFile(string file_name);
	
};
