/*
[pref]:画描边思路,第一步绘制所有的正常物体, 第二步绘制有描边的物体,把它当做mask而不绘制到RenderTarget,第三步,绘制描边效果,即把大一号的物体绘制到渲染目标上但仍然启用mask,为了让大物体不遮住原来的物体
*/

#pragma once
#include "Bindable.h"

namespace Bind
{
	/* 深度模板,管线绑定物
	* 构造器: Stencil(Graphics& gfx, Mode mode)*/
	class Stencil : public Bindable
	{
	public:
		/* 模板使用模式*/
		enum class Mode
		{
			Off,// 关闭
			Write,//写入模式下,绘制几何体它会被绘制到mask上
			Mask,//遮罩模式下,绘制几何体到渲染目标上,但同时也使用了mask强迫特定像素不被绘制
		};
		/* 构造器,负责创建出"深度模板缓存状态",需要指定1个使用模式*/
		Stencil(Graphics& gfx, Mode mode)
		{
			D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };

			// 写入模式在这里 把几何体像素写进模板缓存
			if (mode == Mode::Write)
			{
				dsDesc.StencilEnable = TRUE;
				dsDesc.StencilWriteMask = 0xFF;
				dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;// 比较函数设为永远通过,从而绘制到mask上,达到不影响RenderTarget的效果
				dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;// 设为REPLACE,从而让上一行引用的模板值传进来(引用的值是45行的第2参数!!!!),能写到模板缓存里 
			} // 遮罩模式下, 即使模板测试通过失败,我们也保留模板值
			else if (mode == Mode::Mask)
			{
				dsDesc.DepthEnable = FALSE;// 禁用深度缓存,因为描边仅仅是一种特效,不是场景里的物体,不需要开zbuffer去干扰描边
				dsDesc.StencilEnable = TRUE;// 仍然启用模板缓存,
				dsDesc.StencilReadMask = 0xFF;// 无论读还是写,相应的mask都会作用到这个像素上,此处设置使用所有的bit
				dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;// 这是一个比较选项,用以设置模板什么时候失败,(失败的话stencil就写不进renderTarget),这里设置为NOT_EQUAL,如果mask里的值和引用物体的值不匹配,就绘制
				dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;// 如若通过深度缓存,D3D11_STENCIL_OP_KEEP表示什么也不做 ,保留深度缓存的值
			}

			GetDevice(gfx)->CreateDepthStencilState(&dsDesc, &pStencil);
		}
		/* 管线上"输出合并阶段"绑定深度模板缓存*/
		void Bind(Graphics& gfx) noexcept override
		{
			GetContext(gfx)->OMSetDepthStencilState(pStencil.Get(), 0xFF);
		}
		//static std::shared_ptr<Stencil> Resolve( Graphics& gfx,bool blending,std::optional<float> factor = {} );
		//static std::string GenerateUID( bool blending,std::optional<float> factor );
		//std::string GetUID() const noexcept override;
	private:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pStencil;// 字段,深度模板缓存
	};
}
