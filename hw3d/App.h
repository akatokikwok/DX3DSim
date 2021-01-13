#pragma once
#include "Window.h"
#include "ChiliTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "Box.h"
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

	void SpawnSimulationWindow() noexcept;
	void SpawnBoxWindowManagerWindow() noexcept;
	void SpawnBoxWindows() noexcept;

private:
	ImguiManager imgui;
	Window wnd;
	ChiliTimer timer;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	std::vector<class Box*> boxes;// 有一些盒子实例
	float speed_factor = 1.0f;
	Camera cam;// 摄像机
	PointLight light; //点光源
	static constexpr size_t nDrawables = 180;

	// 为了控制不同BOX实例的IMGUI窗口
	std::optional<int> comboBoxIndex; // 被点击的盒子ID
	std::set<int> boxControlIds;// 被控制的盒子ID
};