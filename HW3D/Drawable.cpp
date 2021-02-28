#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"
#include <cassert>
//#include <typeinfo>

using namespace Bind;

void Drawable::Draw(Graphics& gfx) const noxnd
{
	for (auto& b : binds)
	{
		b->Bind(gfx);
	}
	//for (auto& b : GetStaticBinds())
	//{
	//	b->Bind(gfx);
	//}
	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::shared_ptr<Bindable> bind) noxnd
{
	//assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
	
	// special case for index buffer
	if (typeid(*bind) == typeid(IndexBuffer)) //运行时获知*bind的类型 ，假若等于索引缓存
	{
		assert("Binding multiple index buffers not allowed" && pIndexBuffer == nullptr);
		pIndexBuffer = &static_cast<IndexBuffer&>(*bind);//把该绑定物转成索引缓存
	}
	binds.push_back(std::move(bind));//并把该绑定物存入绑定物集合
}

//void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noxnd
//{
//	assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
//	pIndexBuffer = ibuf.get();
//	binds.push_back(std::move(ibuf));
//}