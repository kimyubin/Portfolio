#include "CSVParser.h"
#include <iostream>
#include <fstream>
#include <sstream>

#define TRIM_SPACE L"\n\t\r "
inline std::wstring trim(std::wstring& s,const std::wstring& drop = TRIM_SPACE)
{
	std::wstring r=s.erase(s.find_last_not_of(drop)+1);
	return r.erase(0,r.find_first_not_of(drop));
}

inline std::wstring rtrim(std::wstring s,const std::wstring& drop = TRIM_SPACE)
{
	return s.erase(s.find_last_not_of(drop)+1);
}

inline std::wstring ltrim(std::wstring s,const std::wstring& drop = TRIM_SPACE)
{ 
	return s.erase(0,s.find_first_not_of(drop)); 
}

CSVParser::CSVParser(std::wstring path)
{
	success = false;
	
    char cstrPath[256];
    wcstombs(cstrPath, path.c_str(), 256);
    std::wifstream file(cstrPath);
	if(!file.is_open())
		return;

	std::wstring wline;
	while( std::getline(file, wline))
	{
		content.push_back(std::vector<std::wstring>());
		auto &vec = content.back();

		std::wstringstream wss(wline);
		std::wstring tok;
		while(std::getline(wss, tok, L','))
			vec.push_back(trim(tok));
	}
	success = true;
}
