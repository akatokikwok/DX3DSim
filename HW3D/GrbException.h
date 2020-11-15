#pragma once
#include <exception>
#include <string>
class GrbException:public std::exception
{
public:
	GrbException(int line, const char* file) noexcept;
	/*��what����������д*/
	const char* what() const noexcept override;

	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;

	/*�Ѵ�����Ϣ��ʽ�����ַ��� Ȼ�������ӡ*/
	std::string GetOriginString() const noexcept;
private:
	int line;//���������
	std::string file;//������ֵľ����ļ�
protected:
	mutable std::string whatBuffer;//��mutable���εı���������Զ���ڿɱ��״̬


};