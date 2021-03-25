#include "Blender.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	Blender::Blender(Graphics& gfx, bool blending)
		:
		blending(blending)
	{
		INFOMAN(gfx);

		D3D11_BLEND_DESC blendDesc = {};
		auto& brt = blendDesc.RenderTarget[0];
		// 若启用混合效果
		if (blending)
		{
			brt.BlendEnable = TRUE;
			brt.SrcBlend = D3D11_BLEND_SRC_ALPHA;//源像素以Alpha作为乘数
			brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;//目标像素以1-Alpha作为乘数
			brt.BlendOp = D3D11_BLEND_OP_ADD;
			brt.SrcBlendAlpha = D3D11_BLEND_ZERO;  //可以使用混合alpha值,但是此时并不需要用到,暂设为0
			brt.DestBlendAlpha = D3D11_BLEND_ZERO;
			brt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;//此处允许写入; 但若想值只写入蓝色或绿色通道,可以再微调这个参数
		}
		else //若不启用混合效果
		{
			brt.BlendEnable = FALSE;
			brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}
		GFX_THROW_INFO(GetDevice(gfx)->CreateBlendState(&blendDesc, &pBlender));//创建混合器
	}

	void Blender::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->OMSetBlendState(pBlender.Get(), nullptr, 0xFFFFFFFFu);//第二参数可以影响淡入淡出之类的效果;第三参数和多重采样或者抗锯齿相关
	}

	std::shared_ptr<Blender> Blender::Resolve(Graphics& gfx, bool blending)
	{
		return Codex::Resolve<Blender>(gfx, blending);
	}
	std::string Blender::GenerateUID(bool blending)
	{
		using namespace std::string_literals;
		return typeid(Blender).name() + "#"s + (blending ? "b" : "n");
	}
	std::string Blender::GetUID() const noexcept
	{
		return GenerateUID(blending);
	}
}