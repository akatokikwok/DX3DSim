#pragma once
#include <chrono>/*chrono是一个time library, 源于boost，现在已经是C++标准*/

class GrbTimer
{
public:
	GrbTimer();
	/*调用该函数,获取从上次mark到本次所经历的时间*/
	float Mark();
	/*同mark函数,但是不会重设mark计时器*/
	float Peek() const;
private:
	std::chrono::steady_clock::time_point last;
};