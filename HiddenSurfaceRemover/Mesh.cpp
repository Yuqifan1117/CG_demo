#include"Mesh.h"
#include<fstream>
#include<iostream>
#include <math.h> 
#include<algorithm>
using namespace std;



void Mesh::ReadFile(string file_name)
{
	ifstream read(file_name);
	string line;
	int id = 0;
	while (!read.eof())
	{
		getline(read, line);
		vector<string> parameters;
		string tailMark = " ";
		string tag = "";
		line = line.append(tailMark);
		// v -0.511812 -0.845392 0.127809
		for (int i = 0; i < line.length(); i++) {
			char ch = line[i];
			if (ch != ' ') {
				tag += ch;
			}
			// 将元素进行保存去除空格
			else {
				if (tag != "") {
					parameters.push_back(tag); //取出字符串中的元素，以空格切分
				}
				tag = "";
			}
		}

		if (parameters.size() == 4) {
			if (parameters[0] == "v") {   //如果是顶点的话
				vector<float> node;
				Vec3f vert;
				for (int i = 0; i < 3; i++) {   //从1开始，将顶点的xyz三个坐标放入顶点vector
					float xyz = atof(parameters[i+1].c_str());
					node.push_back(xyz);
				}
				vert.x = node[0];
				vert.y = node[1];
				vert.z = node[2];
				verts.push_back(vert);
			}

			else if (parameters[0] == "f") {   //如果是面的话，存放三个顶点的索引
				Face face;
				//f 28/5/28 30/7/30 27/8/27
				for (int i = 0; i < 3; i++) {
					string x = parameters[i+1];
					string ans = "";
					for (int j = 0; j < x.length(); j++) {   //跳过‘/’
						char ch = x[j];
						if (ch != '/') {
							ans += ch;
						}
						else {
							break;
						}
					}
					int index = atof(ans.c_str());
					face.v[i] = index - 1; // index减一，之后可直接提取点
				}
				face.id = id++;
				faces.push_back(face);
			}
		}

	}
	face_number = faces.size();
	vert_number = verts.size();
	//calculate the bounding box  
	for (int i = 0; i < vert_number; i++) {
		bounding_box.x_min = min(verts[i].x, bounding_box.x_min);
		bounding_box.x_max = max(verts[i].x, bounding_box.x_max);
		bounding_box.y_min = min(verts[i].y, bounding_box.y_min);
		bounding_box.y_max = max(verts[i].y, bounding_box.y_max);
		bounding_box.z_min = min(verts[i].z, bounding_box.z_min);
		bounding_box.z_max = max(verts[i].z, bounding_box.z_max);

	}
	cout << "----------obj文件加载完成-------------" << endl;
	cout << "面片数:" << faces.size() << " 顶点数:" << verts.size() << endl;
	cout << "----------生成相应的AABB包围盒-------------" << endl;
	cout << "包围盒坐标: min:("
		<< bounding_box.x_min << "," << bounding_box.y_min << "," << bounding_box.z_min << ") max:("
		<< bounding_box.x_max << "," << bounding_box.y_max << "," << bounding_box.z_max << ")" << endl;

	//normalization, 把坐标移到第一象限并把图像移到中心
	float diam_x = bounding_box.x_max - bounding_box.x_min;
	float diam_y = bounding_box.y_max - bounding_box.y_min;
	float diam_z = bounding_box.z_max - bounding_box.z_min;

	float max_diam = max(diam_x, diam_y);
	max_diam = max(max_diam, diam_z);
	max_diam = max_diam * 1.1f;
	float scale = 2.0f / max_diam;
	// cout << scale << endl;
	Vec3f center = bounding_box.center();

	// 移动坐标并进行缩放
	for (int i = 0; i < vert_number; i++) {
		verts[i] = (verts[i] - center)*scale;
	}
	/*cout << "----------obj file loaded-------------" << endl;
	cout << "number of faces:" << faces.size() << " number of vertices:" << verts.size() << endl;
	cout << "obj bounding box: min:("
		<< bounding_box.x_min << "," << bounding_box.y_min << "," << bounding_box.z_min << ") max:("
		<< bounding_box.x_max << "," << bounding_box.y_max << "," << bounding_box.z_max << ")" << endl;*/
}


