#pragma once
#include "Bindable.h"

class IndexBuffer : public Bindable
{
public:
	/* 索引缓存构造方法来在构造里创建索引缓存;需要1个图形实例, 一个索引数组*/
	IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices);
	void Bind(Graphics& gfx) noexcept override;
	/* 拿取索引的数量*/
	UINT GetCount() const noexcept;
protected:
	UINT count;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
};