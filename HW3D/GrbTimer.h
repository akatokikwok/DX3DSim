#pragma once
#include <chrono>/*chrono��һ��time library, Դ��boost�������Ѿ���C++��׼*/

class GrbTimer
{
public:
	GrbTimer();
	/*���øú���,��ȡ���ϴ�mark��������������ʱ��*/
	float Mark();
	/*ͬmark����,���ǲ�������mark��ʱ��*/
	float Peek() const;
private:
	std::chrono::steady_clock::time_point last;
};