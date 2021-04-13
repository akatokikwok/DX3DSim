/* 工具方法类*/

#pragma once
#include <vector>
#include <string>

/* 此函数可以token一个字符串;若存在很多引用,则可以token它们*/
std::vector<std::string> TokenizeQuoted(const std::string& input);

/* 两个方法,负责互相转化宽字符串和窄字符串*/
std::wstring ToWide(const std::string& narrow);
std::string ToNarrow(const std::wstring& wide);