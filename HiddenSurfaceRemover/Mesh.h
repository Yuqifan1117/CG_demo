#pragma once
#include<vector>
#include<string>
#include"utils.h"
using namespace std;

struct Face {
	//�������Σ���Ľṹ��
	int v[3]; // ������±����
	int id; // ������
	//float normal[3];         //��¼��ķ��������ֱ���x��y��z��������
};

class Mesh {
public:
	vector<Vec3f> verts;  // ��¼���еĶ�����Ϣ
	vector<Face> faces;  // ��¼���е�������Ƭ
	//ģ�͵İ�Χ��  
	BoundingBox bounding_box;
	int vert_number{ 0 };
	int face_number{ 0 };

	void ReadFile(string file_name);
	
};
