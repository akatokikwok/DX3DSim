#pragma once
#include <vector>
#include <DirectXMath.h>
#include <type_traits>
#include "Graphics.h"
#include "Color.h"
#include "ConditionalNoexcept.h"

namespace Dvtx
{
	/*struct BGRAColor
	{
		unsigned char a;
		unsigned char r;
		unsigned char g;
		unsigned char b;
	};*/
	
	class VertexLayout
	{
	public:
		// 因为缺乏反射机制,需要自己强加顶点型类型解释,而非动态反射
		enum ElementType
		{
			Position2D,//2D位置
			Position3D,//3D位置
			Texture2D,//纹理
			Normal,//法线
			Float3Color,
			Float4Color,
			BGRAColor,
			Count,
		};

		template<ElementType> struct Map;// 允许将类型名称映射到某种包含编译时的数据结构体;用作编译时的查找表

		template<> struct Map<Position2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Position";// 着色器语义名
		};
		template<> struct Map<Position3D>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Position";
		};
		template<> struct Map<Texture2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Texcoord";
		};
		template<> struct Map<Normal>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Normal";
		};
		template<> struct Map<Float3Color>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Color";
		};
		template<> struct Map<Float4Color>
		{
			using SysType = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic = "Color";
		};
		template<> struct Map<BGRAColor>
		{
			//using SysType = hw3dexp::BGRAColor;
			using SysType = ::BGRAColor;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			static constexpr const char* semantic = "Color";
		};


		/// VertexLayout::Element类
		class Element
		{
		public:
			// 构造函数,需要参数布局元素类型(normal/pos),和参数字节偏移
			Element(ElementType type, size_t offset);	
			// 拿取当前元素偏移量加上 单当前元素的大小,单位是字节
			size_t GetOffsetAfter() const noxnd;
			// 接口，获取从顶点开始的字节数偏移量
			size_t GetOffset() const;
			// 拿到元素类型大小
			size_t Size() const noxnd;
			// 动态拿取相对应顶点型的值;因为使用字面值常量,所以不会占用运行时带宽;
			static constexpr size_t SizeOf(ElementType type) noxnd;
			// 接口,拿取元素类型
			ElementType GetType() const noexcept;
			// 为单元素返回一个输入元素 描述布局
			D3D11_INPUT_ELEMENT_DESC GetDesc() const noxnd;
		private:
			// 构建单元素的描述值({语义名,0,DXGI格式,0，偏移，(UINT)offset,D3D11_INPUT_PER_VERTEX_DATA,0 })
			template<ElementType type>
			static constexpr D3D11_INPUT_ELEMENT_DESC GenerateDesc(size_t offset) noxnd
			{
				return { Map<type>::semantic,0,Map<type>::dxgiFormat,0,(UINT)offset,D3D11_INPUT_PER_VERTEX_DATA,0 };
			}

		private:
			ElementType type;//元素类型
			size_t offset;	//从顶点开始的字节数偏移量
		};
	
	/// VertexLayout类里的一些方法
	public:
		/* 给定指定的元素类型,然后解析这个型号的元素实例*/
		template<ElementType Type>
		const Element& Resolve() const noxnd
		{
			for (auto& e : elements)
			{
				if (e.GetType() == Type)
				{
					return e;
				}
			}
			assert("Could not resolve element type" && false);
			return elements.front();
		}
	
		/* 按指定索引拿取元素引用*/
		const Element& ResolveByIndex(size_t i) const noxnd;
	
		/*template<ElementType Type>
		VertexLayout& Append() noxnd*/
		/* 若要构建Layout整体数据,使用此方法将其附着到数组的最后*/
		VertexLayout& Append(ElementType type) noxnd;
	
		/* 拿取元素数组里最后一个元素引用的偏移*/
		size_t Size() const noxnd;
		/* 接口,获取元素的数量*/
		size_t GetElementCount() const noexcept;
		// 获取输入布局
		std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout() const noxnd;

	private:
		std::vector<Element> elements;// Element实例数组
	};
	/// //////////////////////////////////////////////////////////////////////////
	
	/* 代理类,是我们可以访问单顶点的数据
		Vertex类只是以代理或者视图的形式存在于VertexBuffer类里,
		Vertex实际上是VertexBuffer里单个顶点的视图
	*/
	class Vertex
	{
		friend class VertexBuffer;
	public:
		// 通过元素类型访问数据,获取每个单独顶点数据;Template参数是客户要访问的数据类型
		template<VertexLayout::ElementType Type>
		auto& Attr() noxnd
	{
		//using namespace DirectX;
		//// 获取相符合类型的数据
		//const auto& element = layout.Resolve<Type>();
		//// 获取element的实际地址;	==最开始的元素地址+从顶点开始的字节数偏移量
		//auto pAttribute = pData + element.GetOffset();
		//// 根据"顶点数据类型"来解析 实际地址的XMFLOAT
		//if constexpr (Type == VertexLayout::Position2D)
		//{
		//	return *reinterpret_cast<XMFLOAT2*>(pAttribute);
		//}
		//else if constexpr (Type == VertexLayout::Position3D)
		//{
		//	return *reinterpret_cast<XMFLOAT3*>(pAttribute);
		//}
		//else if constexpr (Type == VertexLayout::Texture2D)
		//{
		//	return *reinterpret_cast<XMFLOAT2*>(pAttribute);
		//}
		//else if constexpr (Type == VertexLayout::Normal)
		//{
		//	return *reinterpret_cast<XMFLOAT3*>(pAttribute);
		//}
		//else if constexpr (Type == VertexLayout::Float3Color)
		//{
		//	return *reinterpret_cast<XMFLOAT3*>(pAttribute);
		//}
		//else if constexpr (Type == VertexLayout::Float4Color)
		//{
		//	return *reinterpret_cast<XMFLOAT4*>(pAttribute);
		//}
		//else if constexpr (Type == VertexLayout::BGRAColor)
		//{
		//	return *reinterpret_cast<BGRAColor*>(pAttribute);
		//}
		//else
		//{
		//	assert("Bad element type" && false);
		//	return *reinterpret_cast<char*>(pAttribute);
		//}
		
		auto pAttribute = pData + layout.Resolve<Type>().GetOffset();// 当前的元素地址==开始的元素地址+偏移量
		return *reinterpret_cast<typename VertexLayout::Map<Type>::SysType*>(pAttribute);//使用Map映射出来具体的SysType成员

	}

		// 通过索引来访问并设置所有的属性;要进行完美转发参数val;从而不丢失任何参数信息只是转发给下一个方法
		template<typename T>
		void SetAttributeByIndex(size_t i, T&& val) noxnd
	{
		using namespace DirectX;
		const auto& element = layout.ResolveByIndex(i);//先根据给定索引解析出布局中的元素
		// 获取element属性的实际地址;
		auto pAttribute = pData + element.GetOffset();
		// 进行转发
		switch (element.GetType())
		{
		case VertexLayout::Position2D:
			SetAttribute<VertexLayout::Position2D>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Position3D:
			SetAttribute<VertexLayout::Position3D>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Texture2D:
			SetAttribute<VertexLayout::Texture2D>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Normal:
			SetAttribute<VertexLayout::Normal>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Float3Color:
			SetAttribute<VertexLayout::Float3Color>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Float4Color:
			SetAttribute<VertexLayout::Float4Color>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::BGRAColor:
			SetAttribute<VertexLayout::BGRAColor>(pAttribute, std::forward<T>(val));
			break;
		default:
			assert("Bad element type" && false);
		}
	}
	
	protected:
		Vertex(char* pData, const VertexLayout& layout) noxnd;
		
	private:
		// 私有接口方法;enables parameter pack setting of multiple parameters by element index
		template<typename First, typename ...Rest>	
		void SetAttributeByIndex(size_t i, First&& first,/*拆解出来的第一参数*/ Rest&&... rest/*拆解出来的剩余的参数包*/) noxnd
		{
			// 原句式是SetAttributeByIndex(size_t i, T&& val);递归到最后1+0的情况就会执行最原先的双参数SetAttributeByIndex()
			SetAttributeByIndex(i, std::forward<First>(first));
			// 递归设置其余属性
			SetAttributeByIndex((i+1), std::forward<Rest>(rest)...);
		}
	
		
		/*template<typename Dest, typename Src>
		void SetAttribute(char* pAttribute, Src&& val) noxnd*/
		// 私有接口方法;helper to reduce code duplication in SetAttributeByIndex;SRC是之前传过来的val参数
		template<VertexLayout::ElementType DestLayoutEnum, typename SrcType>
		void SetAttribute(char* pAttribute, SrcType&& val) noxnd
	{
		using Dest = typename VertexLayout::Map<DestLayoutEnum>::SysType;
		if constexpr (std::is_assignable<Dest, SrcType>::value)
		{
			*reinterpret_cast<Dest*>(pAttribute) = val;
		}
		else
		{
			// 做安全类型检查,使编译器错误更容易调试
			assert("Parameter attribute type mismatch" && false);
		}
	}
	
	private:
		char* pData = nullptr;		// 最开始的Element的地址
		const VertexLayout& layout; //一个对顶点布局类的引用
	};
	/// //////////////////////////////////////////////////////////////////////////
	
	/* 该视图仅能读取数据*/
	class ConstVertex
	{
	public:
		ConstVertex(const Vertex& v) noxnd;
		template<VertexLayout::ElementType Type>
		const auto& Attr() const noxnd
		{
			return const_cast<Vertex&>(vertex).Attr<Type>();
		}
	private:
		Vertex vertex;
	};
	
	/// //////////////////////////////////////////////////////////////////////////
	
	/* 实际上是真正的顶点数据,含有字节缓存和布局*/
	class VertexBuffer
	{
	public:
		VertexBuffer(VertexLayout layout) noxnd;
		// 获取字节缓存的地址
		const char* GetData() const noxnd;
		// 用于获取layout
		const VertexLayout& GetLayout() const noexcept;
		// 用于获取顶点缓存大小除以输入布局的大小,以layout为单位而非以字节为单位
		size_t Size() const noxnd;
		// 获取字节缓存数组的大小
		size_t SizeBytes() const noxnd;
		// 此方法允许在缓存的末端构建1个新的顶点;
		// 模板元编程,参数类型和数量是不确定的,EmplaceBack方法会根据它们构造1个新对象;具体查询通用引用\递归\模板元;参数包\模板递归
		// 此方法和前面的SetAttributeByIndex()联系在一起
		template<typename ...Params>
		void EmplaceBack(Params&&... params) noxnd
		{
			// 检查 确保传进来的参数包size必须匹配 输入布局里元素的数量
			assert(sizeof...(params) == layout.GetElementCount() && "Param count doesn't match number of vertex elements");
			// 重分配字节缓存大小
			buffer.resize(buffer.size() + layout.Size());
			// 对最后1个顶点 从[0]属性开始设置，然后将所有的参数包传递给它
			Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
		}
		// 此方法返回Vertex型的数据,Vertex在这里只是代理或者视图而非真正的数据
		Vertex Back() noxnd;
		// 此方法返回Vertex型的数据,Vertex在这里只是代理或者视图而非真正的数据
		Vertex Front() noxnd;
		// 重载操作符[]来通过给定的索引i来获取顶点
		Vertex operator[](size_t i) noxnd;
	
		ConstVertex Back() const noxnd;
		ConstVertex Front() const noxnd;
		ConstVertex operator[](size_t i) const noxnd;
	
	private:
		std::vector<char> buffer; // 缓存字节数组
		VertexLayout layout;	  // 布局的引用.它用于描述顶点的结构
	};
};