#pragma once
#include <concrt.h>
#include <Windows.h>
#include <string>
#include <unordered_map>

class WindowsMessageMap
{
public:
	/* ���캯�� ʹ��һ�����ƺ��map���� �����ٸ���Ϣ����ӳ�䵽�ַ����� ��ʼ������*/
	WindowsMessageMap();
	/** ����һ�� ��ʽ���ַ���*/
	std::string operator() (DWORD msg, LPARAM lp, WPARAM wp) const;
protected:
private:
	std::unordered_map<DWORD, std::string> map;//��������ϢIDӳ�䵽�ַ�����map
};