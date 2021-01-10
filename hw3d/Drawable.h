#pragma once
#include "Graphics.h"
#include <DirectXMath.h>

class Bindable;

class Drawable
{
	template<class T>
	friend class DrawableBase;
public:
	Drawable() = default;
	Drawable( const Drawable& ) = delete;
	virtual ~Drawable() = default;
	// �û����� ��ȡ��ģ�;�����Ը��Զ������ת����\���߱任����
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	// �û����� �󶨵���ˮ��ͬʱ����������
	void Draw( Graphics& gfx ) const noexcept(!IS_DEBUG);
	// �û����� ��ʱ�����¸��Ե�yaw\pith\roll\theta 
	virtual void Update( float dt ) noexcept = 0;
	
protected:
	void AddBind( std::unique_ptr<Bindable> bind ) noexcept(!IS_DEBUG);
	void AddIndexBuffer( std::unique_ptr<class IndexBuffer> ibuf ) noexcept(!IS_DEBUG);
private:
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;
private:
	const class IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> binds;
};