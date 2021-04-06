#include "Blender.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"
#include <array>

namespace Bind
{
	Blender::Blender( Graphics& gfx,bool blending,std::optional<float> factors_in )
		:
		blending(blending)
	{
		INFOMAN(gfx);

		if (factors_in) //若存在混合因子,就把混合因子初始化为这些值
		{
			factors.emplace();
			factors->fill(*factors_in);
		}

		D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} };
		auto& brt = blendDesc.RenderTarget[0];
		// 若启用混合效果
		if (blending)
		{
			brt.BlendEnable = TRUE;
			//brt.SrcBlend = D3D11_BLEND_SRC_ALPHA;//源像素以Alpha作为乘数
			//brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;//目标像素以1-Alpha作为乘数
			//brt.BlendOp = D3D11_BLEND_OP_ADD;
			//brt.SrcBlendAlpha = D3D11_BLEND_ZERO;  //可以使用混合alpha值,但是此时并不需要用到,暂设为0
			//brt.DestBlendAlpha = D3D11_BLEND_ZERO;
			//brt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			//brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;//此处允许写入; 但若想值只写入蓝色或绿色通道,可以再微调这个参数
		
			if (factors_in)//若存在有上述自定义的混合因子,就让像素使用这些 自定义值的混合因子
			{
				brt.SrcBlend = D3D11_BLEND_BLEND_FACTOR;
				brt.DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
			}
			else //不存在上述混合因子,就使用alpha值作为乘数
			{
				brt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
				brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			}
		}
		//else //若不启用混合效果
		//{
		//	brt.BlendEnable = FALSE;
		//	brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		//}
		GFX_THROW_INFO(GetDevice(gfx)->CreateBlendState(&blendDesc, &pBlender));//创建混合器
	}

	void Blender::Bind(Graphics& gfx) noexcept
	{
		const float* data = factors ? factors->data() : nullptr;
		GetContext(gfx)->OMSetBlendState(pBlender.Get(), data, 0xFFFFFFFFu);//把之前处理过的混合因子传进去,第二参数可以影响淡入淡出之类的效果;第三参数和多重采样或者抗锯齿相关
	}

	std::shared_ptr<Blender> Blender::Resolve( Graphics& gfx,bool blending,std::optional<float> factor )
	{
		return Codex::Resolve<Blender>( gfx,blending,factor );
	}

	void Blender::SetFactor(float factor) noxnd
	{
		assert(factors);
		return factors->fill(factor);
	}

	float Blender::GetFactor() const noxnd
	{
		assert(factors);
		return factors->front();
	}

	std::string Blender::GenerateUID( bool blending,std::optional<float> factor )
	{
		using namespace std::string_literals;
		return typeid(Blender).name() + "#"s + (blending ? "b"s : "n"s) + (factor ? "#f"s + std::to_string( *factor ) : "");
	}

	std::string Blender::GetUID() const noexcept
	{
		return GenerateUID( blending,factors ? factors->front() : std::optional<float>{} );
	}
}