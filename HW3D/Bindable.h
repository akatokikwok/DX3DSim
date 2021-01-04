#pragma once
#include "Graphics.h"

class Bindable
{
public:
	/* 纯虚函数Bind, 供子类改写*/
	virtual void Bind(Graphics& gfx) noexcept = 0;
	virtual ~Bindable() = default;
protected:
	/* 这么设计的原因是在bindable基类里设置一些静态方法可以获取graphics类的私有变量并提供给Bindable类的派生类*/
	static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;//获取上下文
	static ID3D11Device* GetDevice(Graphics& gfx) noexcept;//获取设备
	static DxgiInfoManager& GetInfoManager(Graphics& gfx) noexcept/*(!IS_DEBUG)*/;//获取InfoManager 
};