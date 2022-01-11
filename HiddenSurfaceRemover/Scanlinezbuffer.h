#pragma once

#include <algorithm>
#include <cfloat>
#include <vector>
#include <map>
#include <iostream>


#include "zBuffer.h"
#include "utils.h"
class ScanlineZBuffer : public Zbuffer {
public:
	ScanlineZBuffer(int width, int height, unsigned char* fb, Color color);
	~ScanlineZBuffer();
	void clearZbuffer();
	void drawScanlineMesh(vector<Mesh*>& triMeshs);
	void RenderchangeDET(int y);
	void constructPolygonEdge(Vec2i1f* v, Vec3f n, Vec3f* vec3fs, int id, Color color);
	vector<float> scanlinezbuffer;
	vector<vector<Polygons>> PolygonTable; // ����Ķ���α�
	vector<vector<Edges>> EdgeTable;   // ����ı߱�
	vector<DynamicEdgeNode> DET;  // ��߱�
	vector<DynamicPolyNode> DPT; // �����α�

};
