#pragma once
#include "Window.h"
#include "ChiliTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "Mesh.h"
#include <set>
#include "TestPlane.h"
#include "TestCube.h"

class App
{
public:
	/* App(const std::string& commandLine = "") */
	App(const std::string& commandLine = "");
	// master frame / message loop
	int Go();
	~App();
private:
	void DoFrame();
	// 根据助手开关来动态显隐Imgui助手窗口
	void ShowImguiDemoWindow();
	//// IMGUI窗口:显示光标的累加值
	//void ShowRawInputWindow();
private:
	std::string commandLine;
	bool showDemoWindow = false;//Imgui助手窗口开关,默认关闭

	ImguiManager imgui;
	Window wnd;
	ChiliTimer timer;
	float speed_factor = 1.0f;
	Camera cam;// 摄像机
	PointLight light;//点光源
	//Model nano{ wnd.Gfx(),"Models\\nanosuit.obj" };// 指定一个 纳米服 模型
	//Model nano{ wnd.Gfx(),"Models\\nano_textured\\nanosuit.obj" };// 换成带层级表示的gltf模型
	//Model nano2{ wnd.Gfx(),"Models\\nano_textured\\nanosuit.obj" };
	
	//TestCube cube;

	//Model gobber{ wnd.Gfx(),"Models\\gobber\\GoblinX.obj",6.0f };
	//Model wall{ wnd.Gfx(),"Models\\brick_wall\\brick_wall.obj",6.0f };
	//TestPlane tp{ wnd.Gfx(),6.0 };
	//Model nano{ wnd.Gfx(),"Models\\nano_textured\\nanosuit.obj",2.0f };

	Model sponza{ wnd.Gfx(), "Models\\sponza\\sponza.obj",1.0f / 20.0f };
	
	TestPlane bluePlane{ wnd.Gfx(),6.0f,{ 0.3f,0.3f,1.0f,0.0f } };
	TestPlane redPlane{ wnd.Gfx(),6.0f,{ 1.0f,0.3f,0.3f,0.0f } };
	//TestPlane tp{ wnd.Gfx(),1.0 };
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