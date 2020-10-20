#if !defined( GFL_STRING_UTIL_H_ )
#define GFL_STRING_UTIL_H_

#if defined(WIN32)//Windowsでしか提供するつもりなし
#include <Windows.h>
#include <string>
#include <vector>
#include <map>
#endif

namespace gfl { namespace String {

class Util
{
public:
	//-----------------------------------------------------------------------------
	/// @brief	大文字小文字変換
	//-----------------------------------------------------------------------------
	static std::string StrToLower(std::string src);
	static std::string StrToUpper(std::string src);

	//-----------------------------------------------------------------------------
	/// @brief
	//-----------------------------------------------------------------------------
	static bool ConvUtf8toSJis( BYTE* pSource, BYTE* pDist, int* pSize );

	static std::wstring ShitJIStoUTF16( const std::string& str );
	static std::string UTF16toUTF8( const std::wstring& str );
	static std::string UTF16toShitJIS( const std::wstring& wstr );
	static std::wstring UTF8toUTF16( const std::string& str );

};

}}

#endif
