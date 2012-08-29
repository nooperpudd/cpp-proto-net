#pragma once

#include <string>


void UTF_8ToUnicode(wchar_t* pOut,char *pText);  // ��UTF-8ת����Unicode
void UnicodeToUTF_8(char* pOut,wchar_t* pText);  //Unicode ת����UTF-8
void UnicodeToGB2312(char* pOut,wchar_t uData);  // ��Unicode ת���� GB2312 
void Gb2312ToUnicode(wchar_t* pOut,char *gbBuffer);// GB2312 ת���ɡ�Unicode
void GB2312ToUTF_8(std::string& pOut, const char *gb2312);//GB2312 תΪ UTF-8
void UTF_8ToGB2312(std::string &pOut, const char *utf8);//UTF-8 תΪ GB2312
