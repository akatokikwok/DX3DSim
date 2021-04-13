﻿#define FULL_WINTARD
//#include "Surface.h"
//#include <algorithm>
//namespace Gdiplus
//{
//	using std::min;
//	using std::max;
//}
//#include <gdiplus.h>
//#include <sstream>
//
//#pragma comment( lib,"gdiplus.lib" )
//
////Surface::Surface(unsigned int width, unsigned int height, std::unique_ptr<Color[]> pBufferParam, bool alphaLoaded) noexcept
////	:
////	width( width ),
////	height( height ),
////	pBuffer(std::move(pBufferParam)),
////	alphaLoaded(alphaLoaded)
////{
////
////}
//
//Surface& Surface::operator=( Surface&& donor ) noexcept
//{
//	width = donor.width;
//	height = donor.height;
//	pBuffer = std::move( donor.pBuffer );
//	donor.pBuffer = nullptr;
//	return *this;
//}
//
//Surface::Surface( Surface && source ) noexcept
//	:
//	pBuffer( std::move( source.pBuffer ) ),
//	width( source.width ),
//	height( source.height )
//{}
//
//Surface::~Surface()
//{}
//
//void Surface::Clear( Color fillValue ) noexcept
//{
//	memset( pBuffer.get(),fillValue.dword,width * height * sizeof( Color ) );
//}
//
//void Surface::PutPixel( unsigned int x,unsigned int y,Color c ) noxnd
//{
//	assert( x >= 0 );
//	assert( y >= 0 );
//	assert( x < width );
//	assert( y < height );
//	pBuffer[y * width + x] = c;
//}
//
//Surface::Color Surface::GetPixel( unsigned int x,unsigned int y ) const noxnd
//{
//	assert( x >= 0 );
//	assert( y >= 0 );
//	assert( x < width );
//	assert( y < height );
//	return pBuffer[y * width + x];
//}
//
//unsigned int Surface::GetWidth() const noexcept
//{
//	return width;
//}
//
//unsigned int Surface::GetHeight() const noexcept
//{
//	return height;
//}
//
//Surface::Color* Surface::GetBufferPtr() noexcept
//{
//	return pBuffer.get();
//}
//
//const Surface::Color* Surface::GetBufferPtr() const noexcept
//{
//	return pBuffer.get();
//}
//
//const Surface::Color* Surface::GetBufferPtrConst() const noexcept
//{
//	return pBuffer.get();
//}
//
//Surface Surface::FromFile( const std::string& name )
//{
//	unsigned int width = 0;
//	unsigned int height = 0;
//	std::unique_ptr<Color[]> pBuffer;
//
//	bool alphaLoaded = false;
//
//	{
//		// convert filenam to wide string (for Gdiplus)
//		wchar_t wideName[512];
//		mbstowcs_s( nullptr,wideName,name.c_str(),_TRUNCATE );
//
//		Gdiplus::Bitmap bitmap( wideName );
//		if( bitmap.GetLastStatus() != Gdiplus::Status::Ok )
//		{
//			std::stringstream ss;
//			ss << "Loading image [" << name << "]: failed to load.";
//			throw Exception( __LINE__,__FILE__ ,ss.str() );
//		}
//
//		width = bitmap.GetWidth();
//		height = bitmap.GetHeight();
//		pBuffer = std::make_unique<Color[]>( width * height );
//
//		for( unsigned int y = 0; y < height; y++ )
//		{
//			for( unsigned int x = 0; x < width; x++ )
//			{
//				Gdiplus::Color c;
//				bitmap.GetPixel( x,y,&c );
//				pBuffer[y * width + x] = c.GetValue();
//
//				if (c.GetAlpha() != 255)//当不存在具备255值的像素，则表面具备有效的透明通道
//				{
//					alphaLoaded = true;
//				}
//			}
//		}
//	}
//
//	return Surface(width, height, std::move(pBuffer), alphaLoaded);
//}
//
//bool Surface::AlphaLoaded() const noexcept
//{
//	return alphaLoaded;
//}
//
//
//void Surface::Save( const std::string& filename ) const
//{
//	auto GetEncoderClsid = [&filename]( const WCHAR* format,CLSID* pClsid ) -> void
//	{
//		UINT  num = 0;          // number of image encoders
//		UINT  size = 0;         // size of the image encoder array in bytes
//
//		Gdiplus::ImageCodecInfo* pImageCodecInfo = nullptr;
//
//		Gdiplus::GetImageEncodersSize( &num,&size );
//		if( size == 0 )
//		{
//			std::stringstream ss;
//			ss << "Saving surface to [" << filename << "]: failed to get encoder; size == 0.";
//			throw Exception( __LINE__,__FILE__ ,ss.str() );
//		}
//
//		pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc( size ));
//		if( pImageCodecInfo == nullptr )
//		{
//			std::stringstream ss;
//			ss << "Saving surface to [" << filename << "]: failed to get encoder; failed to allocate memory.";
//			throw Exception( __LINE__,__FILE__,ss.str() );
//		}
//
//		GetImageEncoders( num,size,pImageCodecInfo );
//
//		for( UINT j = 0; j < num; ++j )
//		{
//			if( wcscmp( pImageCodecInfo[j].MimeType,format ) == 0 )
//			{
//				*pClsid = pImageCodecInfo[j].Clsid;
//				free( pImageCodecInfo );
//				return;
//			}
//		}
//
//		free( pImageCodecInfo );
//		std::stringstream ss;
//		ss << "Saving surface to [" << filename << 
//			"]: failed to get encoder; failed to find matching encoder.";
//		throw Exception( __LINE__,__FILE__,ss.str() );
//	};
//
//	CLSID bmpID;
//	GetEncoderClsid( L"image/bmp",&bmpID );
//
//
//	// convert filenam to wide string (for Gdiplus)
//	wchar_t wideName[512];
//	mbstowcs_s( nullptr,wideName,filename.c_str(),_TRUNCATE );
//	
//	Gdiplus::Bitmap bitmap( width,height,width * sizeof( Color ),PixelFormat32bppARGB,(BYTE*)pBuffer.get() );
//	if( bitmap.Save( wideName,&bmpID,nullptr ) != Gdiplus::Status::Ok )
//	{
//		std::stringstream ss;
//		ss << "Saving surface to [" << filename << "]: failed to save.";
//		throw Exception( __LINE__,__FILE__,ss.str() );
//	}
//}
//
//void Surface::Copy( const Surface& src ) noxnd
//{
//	assert( width == src.width );
//	assert( height == src.height );
//	memcpy( pBuffer.get(),src.pBuffer.get(),width * height * sizeof( Color ) );
//}
//
//Surface::Surface(unsigned int width, unsigned int height, std::unique_ptr<Color[]> pBufferParam, bool alphaLoaded) noexcept
//	:
//	width( width ),
//	height( height ),
//	pBuffer(std::move(pBufferParam)),
//	alphaLoaded(alphaLoaded)
//{
//
//}
//
//
//// surface exception stuff
//Surface::Exception::Exception( int line,const char* file,std::string note ) noexcept
//	:
//	ChiliException( line,file ),
//	note( std::move( note ) )
//{}
//
//const char* Surface::Exception::what() const noexcept
//{
//	std::ostringstream oss;
//	oss << ChiliException::what() << std::endl
//		<< "[Note] " << GetNote();
//	whatBuffer = oss.str();
//	return whatBuffer.c_str();
//}
//
//const char* Surface::Exception::GetType() const noexcept
//{
//	return "Chili Graphics Exception";
//}
//
//const std::string& Surface::Exception::GetNote() const noexcept
//{
//	return note;
//}

#include "Surface.h"
#include "Window.h"
#include <algorithm>
#include <cassert>
#include <sstream>
#include <filesystem>
#include "ScriptCommander.h"

Surface::Surface(unsigned int width, unsigned int height)
{
	HRESULT hr = scratch.Initialize2D(
		format,
		width, height, 1u, 1u
	);
	if (FAILED(hr))
	{
		throw Surface::Exception(__LINE__, __FILE__, "Failed to initialize ScratchImage", hr);
	}
}

void Surface::Clear(Color fillValue) noexcept
{
	const auto width = GetWidth();
	const auto height = GetHeight();
	auto& imgData = *scratch.GetImage(0, 0, 0);
	for (size_t y = 0u; y < height; y++)
	{
		auto rowStart = reinterpret_cast<Color*>(imgData.pixels + imgData.rowPitch * y);
		std::fill(rowStart, rowStart + imgData.width, fillValue);
	}
}

/* 根据xy所代表的行数列数计算正确的序号*/
void Surface::PutPixel(unsigned int x, unsigned int y, Color c) noxnd
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < GetWidth());
	assert(y < GetHeight());
	auto& imgData = *scratch.GetImage(0, 0, 0);
	reinterpret_cast<Color*>(&imgData.pixels[y * imgData.rowPitch])[x] = c;
}

Surface::Color Surface::GetPixel(unsigned int x, unsigned int y) const noxnd
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < GetWidth());
	assert(y < GetHeight());
	auto& imgData = *scratch.GetImage(0, 0, 0);
	return reinterpret_cast<Color*>(&imgData.pixels[y * imgData.rowPitch])[x];
}

unsigned int Surface::GetWidth() const noexcept
{
	return (unsigned int)scratch.GetMetadata().width;
}

unsigned int Surface::GetHeight() const noexcept
{
	return (unsigned int)scratch.GetMetadata().height;
}

Surface::Color* Surface::GetBufferPtr() noexcept
{
	return reinterpret_cast<Color*>(scratch.GetPixels());
}

const Surface::Color* Surface::GetBufferPtr() const noexcept
{
	return const_cast<Surface*>(this)->GetBufferPtr();
}

const Surface::Color* Surface::GetBufferPtrConst() const noexcept
{
	return const_cast<Surface*>(this)->GetBufferPtr();
}

Surface Surface::FromFile(const std::string& name)
{
	wchar_t wideName[512];
	mbstowcs_s(nullptr, wideName, name.c_str(), _TRUNCATE);

	DirectX::ScratchImage scratch;
	HRESULT hr = DirectX::LoadFromWICFile(wideName, DirectX::WIC_FLAGS_NONE, nullptr, scratch);//使用LoadFromWICFile把结果放进scratch里

	if (FAILED(hr))
	{
		throw Surface::Exception(__LINE__, __FILE__, name, "Failed to load image", hr);
	}

	if (scratch.GetImage(0, 0, 0)->format != format)//检查这个图片的格式是否匹配我们字段里的format
	{
		DirectX::ScratchImage converted;
		hr = DirectX::Convert(
			*scratch.GetImage(0, 0, 0),// 原生图像
			format,// 目标格式
			DirectX::TEX_FILTER_DEFAULT,
			DirectX::TEX_THRESHOLD_DEFAULT,
			converted
		);

		if (FAILED(hr))
		{
			throw Surface::Exception(__LINE__, __FILE__, name, "Failed to convert image", hr);
		}

		return Surface(std::move(converted));
	}

	return Surface(std::move(scratch));
}

/* 保存图像*/
void Surface::Save(const std::string& filename) const
{
	// 要保存图像,先获取图像的id
	const auto GetCodecID = [](const std::string& filename) {
		const std::filesystem::path path = filename;
		const auto ext = path.extension().string();
		if (ext == ".png")
		{
			return DirectX::WIC_CODEC_PNG;
		}
		else if (ext == ".jpg")
		{
			return DirectX::WIC_CODEC_JPEG;
		}
		else if (ext == ".bmp")
		{
			return DirectX::WIC_CODEC_BMP;
		}
		throw Exception(__LINE__, __FILE__, filename, "Image format not supported");
	};

	wchar_t wideName[512];
	mbstowcs_s(nullptr, wideName, filename.c_str(), _TRUNCATE);

	HRESULT hr = DirectX::SaveToWICFile(
		*scratch.GetImage(0, 0, 0),
		DirectX::WIC_FLAGS_NONE,
		GetWICCodec(GetCodecID(filename)),
		wideName
	);
	if (FAILED(hr))
	{
		throw Surface::Exception(__LINE__, __FILE__, filename, "Failed to save image", hr);
	}
}

/* 检查图像是否携带alpha通道*/
bool Surface::AlphaLoaded() const noexcept
{
	return !scratch.IsAlphaAllOpaque();
}

Surface::Surface(DirectX::ScratchImage scratch) noexcept
	:
	scratch(std::move(scratch))
{}


// surface exception stuff
Surface::Exception::Exception(int line, const char* file, std::string note, std::optional<HRESULT> hr) noexcept
	:
	ChiliException(line, file),
	note("[Note] " + note)
{
	if (hr)
	{
		note = "[Error String] " + Window::Exception::TranslateErrorCode(*hr) + "\n" + note;
	}
}

Surface::Exception::Exception(int line, const char* file, std::string filename, std::string note_in, std::optional<HRESULT> hr) noexcept
	:
	ChiliException(line, file)
{
	using namespace std::string_literals;
	note = "[File] "s + filename + "\n"s + "[Note] "s + note_in;

	if (hr)
	{
		note = "[Error String] " + Window::Exception::TranslateErrorCode(*hr) + note;
	}
}

const char* Surface::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << ChiliException::what() << std::endl << GetNote();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Surface::Exception::GetType() const noexcept
{
	return "Chili Surface Exception";
}

const std::string& Surface::Exception::GetNote() const noexcept
{
	return note;
}
