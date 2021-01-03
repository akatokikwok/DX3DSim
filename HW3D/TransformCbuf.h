#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>
/*
Drawable���ܳ��д�����Bindable���󣬵���֪���ĸ�ƥ��TransCbuf;
���ಢû�в����任��������Ҫ������,ֻ��������������Ż���
������TransformCbuf���Ӷ��㻺��̳�,����ֱ�Ӵ�Bindable�̳�
Ȼ�󽫶��㻺����ΪTransCbuf������ֶ�
����TransformCbuf������1��ָ��Drawable������,	���Դ�Drawable���ȡ��תmatrix��Update(��map�߶�)�����Լ��ڲ��������ɫ������
*/



class TransformCbuf : public Bindable
{
public:
	// �õ�gfx��ͶӰ�����Drawable���������ת���������³���
	TransformCbuf(Graphics& gfx, const Drawable& parent);
	void Bind(Graphics& gfx) noexcept override;
private:
	VertexConstantBuffer<DirectX::XMMATRIX> vcbuf;
	const Drawable& parent;//�ֶ�: 1��Drawable����
};