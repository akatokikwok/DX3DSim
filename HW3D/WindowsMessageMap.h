#pragma once
#include <concrt.h>
#include <Windows.h>
#include <string>
#include <unordered_map>

class WindowsMessageMap
{
public:
	/* 构造函数 使用一个自制宏对map进行 将几百个消息符号映射到字符串的 初始化操作*/
	WindowsMessageMap();
	/** 返回一个 格式化字符串*/
	std::string operator() (DWORD msg, LPARAM lp, WPARAM wp) const;
protected:
private:
	std::unordered_map<DWORD, std::string> map;//将窗口消息ID映射到字符串的map
};