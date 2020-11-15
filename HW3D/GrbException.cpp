#include "GrbException.h"
#include <sstream>


GrbException::GrbException(int line, const char* file) noexcept
	:
	line(line),
	file(file)
{}

const char* GrbException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();//返回的不是字符串而是字符指针
}

const char* GrbException::GetType() const noexcept
{
	return "Grb Exception";
}

int GrbException::GetLine() const noexcept
{
	return line;
}

const std::string& GrbException::GetFile() const noexcept
{
	return file;
}

std::string GrbException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();
}