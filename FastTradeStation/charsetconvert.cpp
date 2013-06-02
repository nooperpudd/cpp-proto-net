#include "charsetconvert.h"

#if defined(WIN32)
#include <Windows.h>
#else
#include <cstdlib>
#include <cstring>
#include <locale>
#endif

void GB2312ToUTF_8(std::string& pOut, const char *gb2312)
{
#if defined(WIN32)

	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);

	wchar_t* wstr = new wchar_t[len+1];
	memset(wstr, 0, len+1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);

	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);

	char* str = new char[len+1];
	memset(str, 0, len+1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);

	if(wstr) delete[] wstr;

	pOut = str;

	delete []str;   

#else
	
	if(NULL == gb2312)
	{
		return;
	}

	//�����Ƚ�gbk����ת��Ϊunicode����

	if(NULL == setlocale(LC_ALL,"zh_CN.gb2312"))//����ת��Ϊunicodeǰ����,��ǰΪgbk����
	{
		return;
	}

	int unicodeLen = mbstowcs(NULL, gb2312, 0);//����ת����ĳ���
	if(unicodeLen <= 0)
	{
		return;
	}

	wchar_t* unicodeStr = new wchar_t[unicodeLen + 1];
	memset(unicodeStr, 0, unicodeLen + 1);
	mbstowcs(unicodeStr, gb2312, strlen(gb2312));//��gbkת��Ϊunicode

	//��unicode����ת��Ϊutf8����

	if(NULL == setlocale(LC_ALL,"zh_CN.utf8"))//����unicodeת�������,��ǰΪutf8
	{
		return;
	}

	int utfLen = wcstombs(NULL, unicodeStr, 0);//����ת����ĳ���

	if(utfLen<=0)
	{
		return;
	}

	char* utfStr = new char[utfLen + 1];
	memset(utfStr, 0, utfLen + 1);
	wcstombs(utfStr, unicodeStr, utfLen);

	utfStr[utfLen] = 0;//��ӽ�����

	if(unicodeStr) 
		delete[] unicodeStr;

	pOut = utfStr;

	if(utfStr)
		delete[] utfStr;
#endif

}


