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
			Vec3f vec3fs[3];	// 空间中的坐标
			Vec2i1f screens[3];	// 屏幕中的坐标
			// 3D位置信息确定好了之后，需要做投影变换，即把3D映射到2D，同时保留z值
			for (int j = 0; j < 3; ++j)
			{
				vec3fs[j] = Mesh->verts[vids[j]]; // 根据顶点下标获取顶点信息
				Vec2i xy = world2screen(vec3fs[j]);
				screens[j] = Vec2i1f(xy.x, xy.y, (vec3fs[j].z + 1.0f) * width / 2.0f + .5);
			}
			Vec3f n = (vec3fs[2] - vec3fs[1]).cross(vec3fs[1] - vec3fs[0]);		// 三角面片的法向量可以根据逆时针的走向，根据两条边的叉乘得出。
			n = n.norm(); // 正则化
			float color_w = n.dot(light);
			if (color_w > 0) {		// 背面剔除加速
				//建立分类的多边形表和边表
				constructPolygonEdge(screens, n, vec3fs, i, Color(255 * color_w, 255 * color_w, 255 * color_w, 255));
			}
			
		}
		// 扫描顺序从上到下
		for (int y = height-1; y >= 0; y--) {
			clearZbuffer();
			for (auto activepolygon : DPT) {
				activepolygon.dy--;
			}
			for (int m = 0; m < PolygonTable[y].size();m++) {
				Polygons* p = &PolygonTable[y][m];
				bool judgment = false;
				// 检查分类的多边形表，如果有新的多边形涉及该扫描线，则把它放入活化的多边形表中
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
				// 将扫描线涉及的多边形与扫描线相交的边加入活化边表
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

	// 按照y坐标大小对三角形顶点升序排序,最大y坐标ymax存放多边形
	//cout << v[0].y << "--" << v[1].y << "--" << v[2].y << endl;
	if (v[0].y > v[1].y) swap(v[0], v[1]);
	if (v[0].y > v[2].y) swap(v[0], v[2]);
	if (v[1].y > v[2].y) swap(v[1], v[2]);
	//cout << v[0].y << "--" << v[1].y << "--" << v[2].y << endl;
	// 构建分类多边形表, 平面方程系数 a,b,c对应法向量
	Polygons P;
	P.a = n.x;
	P.b = n.y;
	P.c = n.z;
	// 带入一点坐标求d
	P.d = -n.dot(vec3fs[0]);
	P.id = id;
	P.back_color = color;
	int ymax = v[2].y;
	P.dy = v[2].y - v[0].y;
	// 构建分类边表
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
	// 根据活化边表绘制
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
				current_z += dynamicedge.dzx; // 沿扫描线向右走一个像素深度增加
			}
		}
		
	}
	// 活化边表中元素的修改：修改后的活化边表是下一条扫描线的边表
	for (int i=0;i<DET.size();i++)
	{
		DET[i].dyl--;
		DET[i].dyr--;

		// 如果两条边都不剩扫描线,则去掉整个边对
		if (DET[i].dyl < 0 && DET[i].dyr < 0  )
		{
			DET[i].deleted = true;
		}
		else
		{
			// 一条边绘制结束，取多边形的另一条边
			// 利用id先确定去掉的边对对应的多边形
			Polygons* dynamicP;
			/*for (int y = 0; y < height; y++) {
				for (auto polygon : PolygonTable[y]) {
					if (polygon.id == DET[i].id) {
						dynamicP = polygon;
					}
				}
			}*/
			dynamicP = DET[i].p;
			// 若dyl或dyr小于0，相应的边就要从一个边对中去掉，从活化边表中找到合适的边来代替
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
			// 边和下一条扫描线交点的x值：xl = xl + dxl xr = xr + dxr
			//if (DET[i].dxl < -10000) cout << DET[i].dxl<<"---"<< DET[i].dyl << endl;
			DET[i].xl = DET[i].xl + DET[i].dxl;
			DET[i].xr = DET[i].xr + DET[i].dxr;
			DET[i].zl += DET[i].dzy + DET[i].dzx * DET[i].dxl;
		}
	}
}
