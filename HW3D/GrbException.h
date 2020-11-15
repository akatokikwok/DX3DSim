#pragma once
#include <exception>
#include <string>
class GrbException:public std::exception
{
public:
	GrbException(int line, const char* file) noexcept;
	/*对what方法进行重写*/
	const char* what() const noexcept override;

	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;

	/*把错误信息格式化成字符串 然后输出打印*/
	std::string GetOriginString() const noexcept;
private:
	int line;//错误的行数
	std::string file;//错误出现的具体文件
protected:
	mutable std::string whatBuffer;//被mutable修饰的变量，将永远处于可变的状态


};