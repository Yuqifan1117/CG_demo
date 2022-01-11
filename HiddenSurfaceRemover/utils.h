#pragma once
#pragma once
#ifndef _UTILS_H_
#define _UTILS_H_

#include <math.h>
#include<algorithm>
#include <windows.h>
#include <tchar.h>
#include <assert.h>
// 参考opencv的固定向量类实现，根据绘制需求实现Vec2i,Vec2i1f,Vec3f
// 包含了Vec2i，Vec2i1f，Vec3f，分别表示平面坐标和立体坐标
struct Vec2i
{
	int x, y;
	Vec2i(int _x = 0, int _y = 0) : x(_x), y(_y) {}
	Vec2i operator + (const Vec2i& v)
	{
		return Vec2i(x + v.x, y + v.y);
	}
	Vec2i operator - (const Vec2i& v)
	{
		return Vec2i(x - v.x, x - v.y);
	}
	Vec2i operator * (const float a)
	{
		return Vec2i(x * a, y * a);
	}
};

struct Vec2i1f
{
	int x, y;
	float z;

	Vec2i1f(int _x = 0, int _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}
	Vec2i1f operator + (const Vec2i1f& v)
	{
		return Vec2i1f(x + v.x, y + v.y, z + v.z);
	}
	Vec2i1f operator - (const Vec2i1f& v)
	{
		return Vec2i1f(x - v.x, y - v.y, z - v.z);
	}
	Vec2i1f operator * (const int a)
	{
		return Vec2i1f(x * a, y * a, z * a);
	}
};

struct Vec3f
{
	float x, y, z;
	Vec3f(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}
	Vec3f operator + (const Vec3f& v)
	{
		return Vec3f(x + v.x, y + v.y, z + v.z);
	}
	Vec3f operator - (const Vec3f& v)
	{
		return Vec3f(x - v.x, y - v.y, z - v.z);
	}
	Vec3f operator / (const float a)
	{
		return Vec3f(x / a, y / a, z / a);
	}
	Vec3f operator * (const float a)
	{
		return Vec3f(x * a, y * a, z * a);
	}
	Vec3f norm()
	{
		float norm_scale = (float)sqrt(x * x + y * y + z * z);
		 
		return *this/ norm_scale;
	}
	float dot(const Vec3f& v)
	{
		return x * v.x + y * v.y + z * v.z;
	}
	Vec3f cross(const Vec3f& v)
	{
		return Vec3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};
// 表示像素点的颜色信息（rgba表示）
struct Color {
	int r{ 0 };
	int g{ 0 };
	int b{ 0 };
	int alpha{ 255 };
	Color() {}
	Color(int r_, int g_, int b_, int a_) : r(r_), g(g_), b(b_), alpha(a_) {}
};

/*
实现Axis-Align包围盒算法，为物体添加包围体的目的是快速的进行碰撞检测或者进行精确的碰撞检测之前进行过滤
*/
struct BoundingBox
{
	float x_min, x_max, y_min, y_max, z_min, z_max;

	BoundingBox(float _x_min, float _x_max, float _y_ymin, float _y_max, float _z_min, float _z_max) :
		x_min(_x_min), x_max(_x_max), y_min(_x_min), y_max(_y_max), z_min(_z_min), z_max(_z_max) {}

	BoundingBox() : x_min(FLT_MAX), x_max(-FLT_MAX), y_min(FLT_MAX), y_max(-FLT_MAX), z_min(FLT_MAX), z_max(-FLT_MAX) {}

	BoundingBox(Vec3f vec_min, Vec3f vec_max)
	{
		x_min = vec_min.x;
		x_max = vec_max.x;
		y_min = vec_min.y;
		y_max = vec_max.y;
		z_min = vec_min.z;
		z_max = vec_max.z;
	}
	// 获取包围盒的中心点
	Vec3f center() {
		float x = (x_min + x_max)*0.5f;
		float y = (y_min + y_max)*0.5f;
		float z = (z_min + z_max)*0.5f;
		return Vec3f(x, y, z);
	}
	// 判断某个多边形是否在包围盒内部
	bool containTri(Vec3f* vec3fs)
	{
		for (int i = 0; i < 3; ++i)
		{
			if (vec3fs[i].x > x_max || vec3fs[i].x <= x_min)
			{
				return false;
			}
			if (vec3fs[i].y > y_max || vec3fs[i].y <= y_min)
			{
				return false;
			}
			if (vec3fs[i].z > z_max || vec3fs[i].z <= z_min)
			{
				return false;
			}
		}
		return true;
	}
};

// 利用window窗口输出图片
struct Window
{
	int width{ 800 };
	int height{ 600 };
	HWND hwnd_{ nullptr };
	HDC hdc{ nullptr };
	HBITMAP hitmap_old{ nullptr }; // 位图信息用于绘制图片
	HBITMAP hitmap{ nullptr };
	unsigned char* framebuffer{ nullptr };
	bool window_close{ false };
	char keys[512];
};

/*
	参考扫描线z-buffer课件提供的数据结构，为了方便后续实现过程，对多边形
	中加入了分类边类，从而能够在扫描过程中确定了多边形后快速加入活化边
*/

// 根据边的上端点y坐标ymax将边放入到相应类中
struct Edges
{
	int x;      // 边上端点的x坐标
	float dx;   // 相邻两条扫描线交点的x坐标差(-1/k) k是斜率
	int dy;     // 边跨越的扫描线数目
	int id;    //交点对所在的多边形编号
	int ymax;
	float z;    // 边上端点的深度值
};
// 多边形，根据多边形最大y坐标ymax将多边形放入相应类中
struct Polygons
{
	float a, b, c, d;   // 多边形平面ax + by + cz + d = 0
	int id;     // 多边形的序号
	int dy;     // 多边形跨越的扫描线数目
	Color back_color;   // 多边形的颜色
	Edges* edgetable;     // 多边形的边
};

// 活化边节点，记录多边形投影边界与扫描线相交的边对
struct DynamicEdgeNode
{
	float xl;     // 左交点的x坐标
	float dxl;      // 左交点边上两相邻扫描线交点的x坐标之差
	int dyl;    // 左交点剩余跨越扫描线数目

	float xr;     // 右交点的x坐标
	float dxr;   // 同上
	int dyr;

	float zl;   // 左交点处多边形所在平面的深度值
	float dzx;  // 沿扫描线向右走一个像素时，多边形所在平面的深度增量。对于平面方程，dzx=-a/c(c≠0)
	float dzy;  // 沿y方向向下移过一根扫描线时，多边形所在平面的深度增量。对于平面方程，dzy=b/c;
	Color back_color;   // 背景颜色

	// int id;  // 交点对所在的多边形编号
	Polygons* p; // 活化边对应多边形指针
	bool deleted{false}; // 为true表示从活化边表中删除
};
struct DynamicPolyNode {
	float a, b, c, d;   // 多边形平面ax + by + cz + d = 0
	int id;     // 多边形的序号
	int dy;     // 多边形跨越的**剩余**扫描线数目
	Color back_color;   // 多边形的颜色
};
#endif // _UTILS_H_
