#pragma once
#include "Window.h"
#include "ChiliTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "Mesh.h"
#include <set>

class App
{
public:
	App();
	// master frame / message loop
	int Go();
	~App();
private:
	void DoFrame();
	// 模型控制窗口
	void ShowImguiDemoWindow();
	// IMGUI窗口:显示光标的累加值
	void ShowRawInputWindow();
private:
	int x = 0, y = 0;

	ImguiManager imgui;
	Window wnd;
	ChiliTimer timer;
	float speed_factor = 1.0f;
	Camera cam;// 摄像机
	PointLight light;//点光源
	//Model nano{ wnd.Gfx(),"Models\\nanosuit.obj" };// 指定一个 纳米服 模型
	Model nano{ wnd.Gfx(),"Models\\nano.gltf" };// 换成带层级表示的gltf模型

	// 一些有关于导入模型的控制IMGUI配置
	/*struct
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	} pos;*/
};