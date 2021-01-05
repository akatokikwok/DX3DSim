#pragma once
#include "Bindable.h"

// 采样器类; 渲染纹理还需要一个采样器
class Sampler : public Bindable
{
public:
	// 构造方法，生成1个采样器
	Sampler( Graphics& gfx );
	// 在PS阶段绑定采样器
	void Bind( Graphics& gfx ) noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;// 字段:采样器,目的是告知管线处理图片的方法
};
