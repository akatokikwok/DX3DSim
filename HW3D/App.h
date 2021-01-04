#pragma once
#include "Window.h"
#include "GrbTimer.h"

class App
{
public:
	App();
	//主线程 /消息循环
	~App();

	/*初始化游戏逻辑*/
	int Go();
private:
	/*在此撰写每一帧的游戏逻辑*/
	void DoFrame();
private:
	Window wnd;//使用应用层获取每一个窗口
	GrbTimer timer;//给窗口添加1个计时器
	std::vector<std::unique_ptr<class Box>> boxes;//Box对象数组
};