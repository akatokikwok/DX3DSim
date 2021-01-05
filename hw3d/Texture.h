#pragma once
#include "Bindable.h"

// 继承自Bindable类的纹理类
class Texture : public Bindable
{
public:
	// 构造方法:创建出2D纹理和着色器资源视图; 参数需要Surface实例
	Texture( Graphics& gfx,const class Surface& s );
	// 在PS阶段绑定着色器资源,此处是绑定纹理视图pTextureView
	void Bind( Graphics& gfx ) noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;// 字段:着色器资源视图
};
