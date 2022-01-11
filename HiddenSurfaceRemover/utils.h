#pragma once
#pragma once
#ifndef _UTILS_H_
#define _UTILS_H_

#include <math.h>
#include<algorithm>
#include <windows.h>
#include <tchar.h>
#include <assert.h>
// �ο�opencv�Ĺ̶�������ʵ�֣����ݻ�������ʵ��Vec2i,Vec2i1f,Vec3f
// ������Vec2i��Vec2i1f��Vec3f���ֱ��ʾƽ���������������
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
// ��ʾ���ص����ɫ��Ϣ��rgba��ʾ��
struct Color {
	int r{ 0 };
	int g{ 0 };
	int b{ 0 };
	int alpha{ 255 };
	Color() {}
	Color(int r_, int g_, int b_, int a_) : r(r_), g(g_), b(b_), alpha(a_) {}
};

/*
ʵ��Axis-Align��Χ���㷨��Ϊ������Ӱ�Χ���Ŀ���ǿ��ٵĽ�����ײ�����߽��о�ȷ����ײ���֮ǰ���й���
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
	// ��ȡ��Χ�е����ĵ�
	Vec3f center() {
		float x = (x_min + x_max)*0.5f;
		float y = (y_min + y_max)*0.5f;
		float z = (z_min + z_max)*0.5f;
		return Vec3f(x, y, z);
	}
	// �ж�ĳ��������Ƿ��ڰ�Χ���ڲ�
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

// ����window�������ͼƬ
struct Window
{
	int width{ 800 };
	int height{ 600 };
	HWND hwnd_{ nullptr };
	HDC hdc{ nullptr };
	HBITMAP hitmap_old{ nullptr }; // λͼ��Ϣ���ڻ���ͼƬ
	HBITMAP hitmap{ nullptr };
	unsigned char* framebuffer{ nullptr };
	bool window_close{ false };
	char keys[512];
};

/*
	�ο�ɨ����z-buffer�μ��ṩ�����ݽṹ��Ϊ�˷������ʵ�ֹ��̣��Զ����
	�м����˷�����࣬�Ӷ��ܹ���ɨ�������ȷ���˶���κ���ټ�����
*/

// ���ݱߵ��϶˵�y����ymax���߷��뵽��Ӧ����
struct Edges
{
	int x;      // ���϶˵��x����
	float dx;   // ��������ɨ���߽����x�����(-1/k) k��б��
	int dy;     // �߿�Խ��ɨ������Ŀ
	int id;    //��������ڵĶ���α��
	int ymax;
	float z;    // ���϶˵�����ֵ
};
// ����Σ����ݶ�������y����ymax������η�����Ӧ����
struct Polygons
{
	float a, b, c, d;   // �����ƽ��ax + by + cz + d = 0
	int id;     // ����ε����
	int dy;     // ����ο�Խ��ɨ������Ŀ
	Color back_color;   // ����ε���ɫ
	Edges* edgetable;     // ����εı�
};

// ��߽ڵ㣬��¼�����ͶӰ�߽���ɨ�����ཻ�ı߶�
struct DynamicEdgeNode
{
	float xl;     // �󽻵��x����
	float dxl;      // �󽻵����������ɨ���߽����x����֮��
	int dyl;    // �󽻵�ʣ���Խɨ������Ŀ

	float xr;     // �ҽ����x����
	float dxr;   // ͬ��
	int dyr;

	float zl;   // �󽻵㴦���������ƽ������ֵ
	float dzx;  // ��ɨ����������һ������ʱ�����������ƽ����������������ƽ�淽�̣�dzx=-a/c(c��0)
	float dzy;  // ��y���������ƹ�һ��ɨ����ʱ�����������ƽ����������������ƽ�淽�̣�dzy=b/c;
	Color back_color;   // ������ɫ

	// int id;  // ��������ڵĶ���α��
	Polygons* p; // ��߶�Ӧ�����ָ��
	bool deleted{false}; // Ϊtrue��ʾ�ӻ�߱���ɾ��
};
struct DynamicPolyNode {
	float a, b, c, d;   // �����ƽ��ax + by + cz + d = 0
	int id;     // ����ε����
	int dy;     // ����ο�Խ��**ʣ��**ɨ������Ŀ
	Color back_color;   // ����ε���ɫ
};
#endif // _UTILS_H_
