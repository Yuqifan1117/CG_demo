#include "ScanlinezBuffer.h"
//void constructPolygonEdge(Vec2i1f* v, Vec3f n, Vec3f* vec3fs, int id, Color color, vector<vector<Polygons>>& Pt, vector<vector<Edges>>& Et);

ScanlineZBuffer::ScanlineZBuffer(int width, int height, unsigned char* fb, Color color) : Zbuffer(width, height,fb,color) 
{
	scanlinezbuffer.resize(width);
	PolygonTable.resize(height);
	EdgeTable.resize(height);

}

ScanlineZBuffer::~ScanlineZBuffer() {
	
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < PolygonTable[i].size(); ++i)
		{
			delete[]PolygonTable[i][j].edgetable;
		}
	}
	vector<float> tmp = scanlinezbuffer;
	scanlinezbuffer.swap(tmp);
}
void ScanlineZBuffer::clearZbuffer() {
	for (int i = 0; i < width; ++i)
	{
		scanlinezbuffer[i] = -FLT_MAX;
	}
}

void ScanlineZBuffer::drawScanlineMesh(vector<Mesh*>& triMeshs) {
	clearZbuffer();
	for (Mesh* Mesh : triMeshs)
	{
		Vec3f light(0, 0, -1);

		for (int i = 0; i < Mesh->face_number; ++i)
		{
			int* vids = Mesh->faces[i].v;
			Vec3f vec3fs[3];	// �ռ��е�����
			Vec2i1f screens[3];	// ��Ļ�е�����
			// 3Dλ����Ϣȷ������֮����Ҫ��ͶӰ�任������3Dӳ�䵽2D��ͬʱ����zֵ
			for (int j = 0; j < 3; ++j)
			{
				vec3fs[j] = Mesh->verts[vids[j]]; // ���ݶ����±��ȡ������Ϣ
				Vec2i xy = world2screen(vec3fs[j]);
				screens[j] = Vec2i1f(xy.x, xy.y, (vec3fs[j].z + 1.0f) * width / 2.0f + .5);
			}
			Vec3f n = (vec3fs[2] - vec3fs[1]).cross(vec3fs[1] - vec3fs[0]);		// ������Ƭ�ķ��������Ը�����ʱ������򣬸��������ߵĲ�˵ó���
			n = n.norm(); // ����
			float color_w = n.dot(light);
			if (color_w > 0) {		// �����޳�����
				//��������Ķ���α�ͱ߱�
				constructPolygonEdge(screens, n, vec3fs, i, Color(255 * color_w, 255 * color_w, 255 * color_w, 255));
			}
			
		}
		// ɨ��˳����ϵ���
		for (int y = height-1; y >= 0; y--) {
			clearZbuffer();
			for (auto activepolygon : DPT) {
				activepolygon.dy--;
			}
			for (int m = 0; m < PolygonTable[y].size();m++) {
				Polygons* p = &PolygonTable[y][m];
				bool judgment = false;
				// ������Ķ���α�������µĶ�����漰��ɨ���ߣ�����������Ķ���α���
				for (auto activepolygon : DPT) {
					if (activepolygon.id == p->id) {
						judgment = true;
					}
				}
				if (!judgment) {
					DynamicPolyNode DP;
					DP.a = p->a;
					DP.b = p->b;
					DP.c = p->c;
					DP.d = p->d;
					DP.back_color = p->back_color;
					DP.dy = y;
					DP.id = p->id;
					DPT.push_back(DP);
				}
				// ��ɨ�����漰�Ķ������ɨ�����ཻ�ı߼����߱�
				DynamicEdgeNode DE;
				
				DE.back_color = p->back_color;
				DE.p = p;
				// Polygons* p = &polyton;
				// DE.id = polyton.id;
				Edges edge1 = p->edgetable[0];
				Edges edge2 = p->edgetable[1];
				if (edge1.dx < edge2.dx) {
					//cout << edge1.dx<<"--"<<edge2.x << "--" <<edge2.ymax<< "--" <<  polyton.id << endl;
					DE.xl = edge1.x;
					DE.dxl = edge1.dx;
					DE.dyl = edge1.dy;// - (edge1.ymax - y);
					DE.xr = edge2.x;// +edge2.dy * (edge2.ymax - y);
					DE.dxr = edge2.dx;
					DE.dyr = edge2.dy;// - (edge2.ymax - y);
					DE.zl = edge1.z;
					DE.dzx = -p->a / p->c;
					DE.dzy = p->b / p->c;
				}
				else {
					//cout << edge1.x << endl << edge1.ymax << endl << y << endl;
					DE.xr = edge1.x;// +edge1.dy * (edge1.ymax - y);
					DE.dxr = edge1.dx;
					DE.dyr = edge1.dy;// -(edge1.ymax - y);
					DE.xl = edge2.x;// +edge2.dy * (edge2.ymax - y);
					DE.dxl = edge2.dx;
					DE.dyl = edge2.dy;// -(edge2.ymax - y);
					DE.zl = edge2.z;
					DE.dzx = -p->a / p->c;
					DE.dzy = p->b / p->c;
				}
				//if (DE.dxl<-10000)cout << DE.dyl <<"---"<<DE.dyr<< endl;
				DE.deleted = false;
				DET.push_back(DE);


			}
			RenderchangeDET(y);
		}

	}
}
void ScanlineZBuffer::constructPolygonEdge(Vec2i1f* v, Vec3f n, Vec3f* vec3fs, int id, Color color) {

	// ����y�����С�������ζ�����������,���y����ymax��Ŷ����
	//cout << v[0].y << "--" << v[1].y << "--" << v[2].y << endl;
	if (v[0].y > v[1].y) swap(v[0], v[1]);
	if (v[0].y > v[2].y) swap(v[0], v[2]);
	if (v[1].y > v[2].y) swap(v[1], v[2]);
	//cout << v[0].y << "--" << v[1].y << "--" << v[2].y << endl;
	// �����������α�, ƽ�淽��ϵ�� a,b,c��Ӧ������
	Polygons P;
	P.a = n.x;
	P.b = n.y;
	P.c = n.z;
	// ����һ��������d
	P.d = -n.dot(vec3fs[0]);
	P.id = id;
	P.back_color = color;
	int ymax = v[2].y;
	P.dy = v[2].y - v[0].y;
	// ��������߱�
	Edges* edgeNode = new Edges[3];
	edgeNode[0].x = v[2].x;
	edgeNode[0].dy = v[2].y - v[0].y;
	edgeNode[0].dx = -(float)(v[2].x - v[0].x) / edgeNode[0].dy;
	edgeNode[0].ymax = v[2].y;
	edgeNode[0].id = P.id;
	edgeNode[0].z = v[2].z;
	edgeNode[1].x = v[2].x;
	edgeNode[1].dy = v[2].y - v[1].y;
	edgeNode[1].dx = -(float)(v[2].x - v[1].x) / edgeNode[1].dy;
	edgeNode[1].id = P.id;
	edgeNode[1].ymax = v[2].y;
	edgeNode[1].z = v[2].z;
	edgeNode[2].x = v[1].x;
	edgeNode[2].dy = v[1].y - v[0].y;
	//if (edgeNode[2].dy == 0)cout << v[1].y - v[0].y<<"--------"<< P.id << endl;
	edgeNode[2].dx = -(float)(v[1].x - v[0].x) / edgeNode[2].dy;
	edgeNode[2].id = P.id;
	edgeNode[2].ymax = v[1].y;
	edgeNode[2].z = v[1].z;
	P.edgetable = edgeNode;
	PolygonTable[v[2].y].push_back(P);
	EdgeTable[v[2].y].push_back(edgeNode[0]);
	EdgeTable[v[2].y].push_back(edgeNode[1]);
	EdgeTable[v[1].y].push_back(edgeNode[2]);
}
void ScanlineZBuffer::RenderchangeDET(int y) {
	// ���ݻ�߱����
	for (DynamicEdgeNode dynamicedge : DET) {
		if (dynamicedge.deleted != true) {
			
			float current_z = dynamicedge.zl;
			int l = (int)dynamicedge.xl;
			int r = (int)dynamicedge.xr;
			for (int x = l; x < r; x++) {
				if (current_z > scanlinezbuffer[x])
				{
					draw_pixel(x, y, dynamicedge.back_color);
					scanlinezbuffer[x] = current_z;
				}
				current_z += dynamicedge.dzx; // ��ɨ����������һ�������������
			}
		}
		
	}
	// ��߱���Ԫ�ص��޸ģ��޸ĺ�Ļ�߱�����һ��ɨ���ߵı߱�
	for (int i=0;i<DET.size();i++)
	{
		DET[i].dyl--;
		DET[i].dyr--;

		// ��������߶���ʣɨ����,��ȥ�������߶�
		if (DET[i].dyl < 0 && DET[i].dyr < 0  )
		{
			DET[i].deleted = true;
		}
		else
		{
			// һ���߻��ƽ�����ȡ����ε���һ����
			// ����id��ȷ��ȥ���ı߶Զ�Ӧ�Ķ����
			Polygons* dynamicP;
			/*for (int y = 0; y < height; y++) {
				for (auto polygon : PolygonTable[y]) {
					if (polygon.id == DET[i].id) {
						dynamicP = polygon;
					}
				}
			}*/
			dynamicP = DET[i].p;
			// ��dyl��dyrС��0����Ӧ�ı߾�Ҫ��һ���߶���ȥ�����ӻ�߱����ҵ����ʵı�������
			if (DET[i].dyl < 0)
			{
				//cout << DET[i].dxl << endl;
				DET[i].xl = dynamicP->edgetable[2].x;
				DET[i].dxl = dynamicP->edgetable[2].dx;
				DET[i].dyl = dynamicP->edgetable[2].dy - 1;
				//cout<< DET[i].xl<<"---"<<DET[i].dxl<<"---"<< DET[i].dyl <<"---"<<DET[i].dyr << endl;
				//cout << dynamicP->edgetable[0].dy << "****" << dynamicP->edgetable[1].dy << "****" << dynamicP->edgetable[2].dy << "****" << endl;
			}
			if (DET[i].dyr < 0)
			{
				DET[i].xr = dynamicP->edgetable[2].x;
				DET[i].dxr = dynamicP->edgetable[2].dx;
				DET[i].dyr = dynamicP->edgetable[2].dy - 1;
			}
			// �ߺ���һ��ɨ���߽����xֵ��xl = xl + dxl xr = xr + dxr
			//if (DET[i].dxl < -10000) cout << DET[i].dxl<<"---"<< DET[i].dyl << endl;
			DET[i].xl = DET[i].xl + DET[i].dxl;
			DET[i].xr = DET[i].xr + DET[i].dxr;
			DET[i].zl += DET[i].dzy + DET[i].dzx * DET[i].dxl;
		}
	}
}
