#pragma once
#include "GrbWin.h"
#include <vector>
#include <dxgidebug.h>
#include <string>
#include <wrl\internal.h>

class DxgiInfoManager
{
public:
	DxgiInfoManager();
	~DxgiInfoManager();
	DxgiInfoManager( const DxgiInfoManager& ) = delete;
	DxgiInfoManager& operator=( const DxgiInfoManager& ) = delete;
	/* DxgiInfoManager::GetMessages()可能会获取到所有消息,所以这里设置为自上次使用set函数出现的消息即可*/
	void Set() noexcept;
	/* 拿取DXGI消息队列里的所有消息*/
	std::vector<std::string> GetMessages() const;
private:
	//For的索引开头值
	unsigned long long next = 0u;
	//DXGI消息队列
	struct IDXGIInfoQueue* pDxgiInfoQueue = nullptr;
};