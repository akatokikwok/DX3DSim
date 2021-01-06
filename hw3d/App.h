#pragma once
#include "Window.h"
#include "ChiliTimer.h"
#include "ImguiManager.h"

class App
{
public:
	App();
	// master frame / message loop
	int Go();
	~App();
private:
	void DoFrame();
private:
	ImguiManager imgui;// 字段:imgui 管理器

	Window wnd;
	ChiliTimer timer;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	static constexpr size_t nDrawables = 180;
	/*bool show_demo_window = true;*/
	float speed_factor = 1.0f;
};