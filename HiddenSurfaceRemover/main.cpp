#include<iostream>
#include"Mesh.h"
#include <ctime>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/glfw3.h>
#include"render.h"
#include"zbuffer.h"
#include"hierarchicalzbuffer.h"
#include"Octreehierarchicalzbuffer.h"
#include"Scanlinezbuffer.h"
#include "utils.h"
const int WIDTH = 512;
const int HEIGHT = 512;

int type = 1;
Zbuffer *render;
HierarchialZBuffer* Hie_render;
OctreeZbuffer* Oct_render;
Window* my_window;
ScanlineZBuffer* Scanline_render;
void windowRender(unsigned char* framebuffer);
void windowClose();
void windowInit(int w = 512, int h = 512);
static LRESULT CALLBACK msgCallback(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int main(int argc, char* argv[]) {
	int stop = 1; 
	clock_t start, end;
	// 初始化画布
	unsigned char* framebuffer = (unsigned char*)malloc(WIDTH * HEIGHT * 4);

	windowInit(WIDTH, HEIGHT);

	vector<Mesh* > triMeshs;
	Mesh* Mesh0 = new Mesh;

	string path = "../models/";
	vector<string> models = { "cross.obj" ,"dolphins.obj"  ,"teapot.obj" ,"bunny.obj" ,"african_head.obj","armadillo.obj" };
	int model;
	string model_name = "";
	cout << "请选择需要绘制的模型: " << "1 cross " << "2 dolphins " << "3 teapot " << "4 bunny " << "5 african_head " <<"6 armadillo"<< endl;
	cin >> model;
	switch (model)
	{
	case 1:
		model_name = models[0];
		cout << "**************************************************************************" << endl;
		cout << "			绘制：" << model_name << endl;
		cout << "**************************************************************************" << endl;
		break;
	case 2:
		model_name = models[1];
		cout << "**************************************************************************" << endl;
		cout << "			绘制：" << model_name << endl;
		cout << "**************************************************************************" << endl;
		break;
	case 3:
		model_name = models[2];
		cout << "**************************************************************************" << endl;
		cout << "			绘制：" << model_name << endl;
		cout << "**************************************************************************" << endl;
		break;
	case 4:
		model_name = models[3];
		cout << "**************************************************************************" << endl;
		cout << "			绘制：" << model_name << endl;
		cout << "**************************************************************************" << endl;
		break;
	case 5:
		model_name = models[4];
		cout << "**************************************************************************" << endl;
		cout << "			绘制：" << model_name << endl;
		cout << "**************************************************************************" << endl;
		break;
	case 6:
		model_name = models[5];
		cout << "**************************************************************************" << endl;
		cout << "			绘制：" << model_name << endl;
		cout << "**************************************************************************" << endl;
		break;
	default:
		break;
	}
	start = clock();
	Mesh0->ReadFile(path + model_name);
	end = clock();
	cout << "Load model cost time : " << end - start << "ms" << endl;
	triMeshs.push_back(Mesh0);

	cout << "请选择render算法: " << endl << "1 base zbuffer " << endl << "2 scan-zbuffer  " << endl << "3 hierarchy zbuffer（without octree） " << endl <<
		"4 hierarchy zbuffer（with octree） " << endl;
	cin >> type;
	bool writen = false;
	switch (type)
	{
	case 1: 
		while (my_window->window_close == false) {
			if (!writen) {
				render = new Zbuffer(WIDTH, HEIGHT, framebuffer, Color(0, 0, 0, 255));
				render->clear_framebuffer();
				start = clock();
				render->drawMesh(triMeshs);
				end = clock();
				cout << "Render cost time : " << end - start << "ms" << endl;
				writen = true;
			}
			windowRender(framebuffer);
			Sleep(0);
		}
		break;
	case 4:
		while (my_window->window_close == false) {
			if (!writen) {
				Hie_render = new HierarchialZBuffer(WIDTH, HEIGHT, framebuffer, Color(0, 0, 0, 255));
				Hie_render->clear_framebuffer();
				start = clock();
				Hie_render->drawHiMesh(triMeshs);
				end = clock();
				cout << "Render cost time : " << end - start << "ms" << endl;
				cout << "hidden_num: " << Hie_render->hidden_num << endl;
				writen = true;
			}
			windowRender(framebuffer);
			Sleep(0);
		}
		break;
	case 3:
		while (my_window->window_close == false) {
			

			if (!writen) {
				Oct_render = new OctreeZbuffer(WIDTH, HEIGHT, framebuffer, Color(0, 0, 0, 255));
				Oct_render->clear_framebuffer();
				start = clock();
				Oct_render->drawOctMesh(triMeshs);
				end = clock();
				cout << "Render cost time : " << end - start << "ms" << endl;
				cout << "oct_hidden_num: " << Oct_render->oct_hidden_num << endl;
				writen = true;
			}
			windowRender(framebuffer);
			Sleep(0);
		}
		break;
	case 2:
		while (my_window->window_close == false) {
			
			if (!writen) {
				Scanline_render = new ScanlineZBuffer(WIDTH, HEIGHT, framebuffer, Color(0, 0, 0, 255));
				start = clock();
				Scanline_render->drawScanlineMesh(triMeshs);
				end = clock();
				cout << "Render cost time : " << end - start << "ms" << endl;
				//cout << "scanline:" << Scanline_render->DET.size() << endl;
				writen = true;
			}
			windowRender(framebuffer);
			Sleep(0);
		}
	default:
		break;
	}
	windowClose();

	for (int i = 0; i < triMeshs.size(); ++i)
	{
		delete triMeshs[i];
	}
	free(framebuffer);
	return 0;
}


/*
	绘制windows窗口，用于显示framebuffer中绘制的物体
*/
void windowInit(int w, int h)
{
	my_window = new Window;
	WNDCLASS wc = { CS_BYTEALIGNCLIENT, (WNDPROC)msgCallback, 0, 0, 0,
			NULL, NULL, NULL, NULL, _T("RenderWindow") };
	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB,
		w * h * 4, 0, 0, 0, 0 } };
	RECT rect = { 0, 0, w, h };
	int wx, wy, sx, sy;
	LPVOID ptr;
	HDC hDC;

	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hInstance = GetModuleHandle(NULL);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClass(&wc);

	my_window->hwnd_ = CreateWindow(_T("RenderWindow"), _T("RenderLab"),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);
	assert(my_window->hwnd_ != NULL);

	my_window->window_close = 0;
	hDC = GetDC(my_window->hwnd_);
	my_window->hdc = CreateCompatibleDC(hDC);
	ReleaseDC(my_window->hwnd_, hDC);

	my_window->hitmap = CreateDIBSection(my_window->hdc, &bi, DIB_RGB_COLORS, &ptr, 0, 0);
	assert(my_window->hitmap != NULL);

	my_window->hitmap_old = (HBITMAP)SelectObject(my_window->hdc, my_window->hitmap);
	my_window->framebuffer = (unsigned char*)ptr;
	my_window->width = w;
	my_window->height = h;

	AdjustWindowRect(&rect, GetWindowLong(my_window->hwnd_, GWL_STYLE), 0);
	wx = rect.right - rect.left;
	wy = rect.bottom - rect.top;
	sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;
	sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;
	if (sy < 0) sy = 0;
	SetWindowPos(my_window->hwnd_, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
	SetForegroundWindow(my_window->hwnd_);

	ShowWindow(my_window->hwnd_, SW_NORMAL);
	MSG msg;
	while (1) {
		if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) break;
		if (!GetMessage(&msg, NULL, 0, 0)) break;
		DispatchMessage(&msg);
	}

	memset(my_window->framebuffer, 0, w * h * 4);

}
void windowRender(unsigned char* framebuffer)
{
	for (int i = 0; i < my_window->width; ++i)
	{
		for (int j = 0; j < my_window->height; ++j)
		{
			for (int k = 0; k < 4; ++k)
			{
				my_window->framebuffer[(j * my_window->width + i) * 4 + k] = framebuffer[(j * my_window->width + i) * 4 + k];
			}
		}
	}

	HDC hDC = GetDC(my_window->hwnd_);
	BitBlt(hDC, 0, 0, my_window->width, my_window->height, my_window->hdc, 0, 0, SRCCOPY);
	ReleaseDC(my_window->hwnd_, hDC);
	MSG msg;

	while (1) {

		if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) break;
		if (!GetMessage(&msg, NULL, 0, 0)) break;
		// 当前没有应用消息持续等待
		DispatchMessage(&msg);
	}

}
void windowClose()
{
	if (my_window->hdc) {
		if (my_window->hitmap_old) {
			SelectObject(my_window->hdc, my_window->hitmap_old);
			my_window->hitmap_old = NULL;
		}
		DeleteDC(my_window->hdc);
		my_window->hdc = NULL;
	}
	if (my_window->hitmap_old) {
		DeleteObject(my_window->hitmap);
		my_window->hitmap = NULL;
	}
	if (my_window->hwnd_) {
		CloseWindow(my_window->hwnd_);
		my_window->hwnd_ = NULL;
	}
	free(my_window);
}
LRESULT CALLBACK msgCallback(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{

	if (msg == WM_CLOSE) {
		my_window->window_close = true;
	}
	else {
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}
