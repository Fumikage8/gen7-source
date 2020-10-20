#include "Util.h"

#include <ctype.h>

namespace gfl { namespace String {

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
std::string Util::StrToLower(std::string src)
{
	int						size = src.length() + 1;
	char					*pLow = new char[size];
	const char*		pSrc = src.c_str();

	memset( pLow, 0, size );

	for(unsigned int i = 0; i < src.length(); ++i)
	{
		pLow[i] = tolower( pSrc[i] );
	}

	std::string			Dst = pLow;

	delete pLow;

	return ( Dst );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
std::string Util::StrToUpper(std::string src)
{
	int						size = src.length() + 1;
	char					*pUp = new char[size];
	const char*		pSrc = src.c_str();

	memset( pUp, 0, size );

	for(unsigned int i = 0; i < src.length(); ++i)
	{
		pUp[i] = toupper( pSrc[i] );
	}

	std::string			Dst = pUp;

	delete pUp;

	return ( Dst );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
bool Util::ConvUtf8toSJis( BYTE* pSource, BYTE* pDist, int* pSize )
{
	/*
	*pSize = 0;

	//UTF-8からUTF-16へ変換
	const int nSize = ::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pSource, -1, NULL, 0 );

	BYTE* buffUtf16 = new BYTE[ nSize * 2 + 2 ];
	::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pSource, -1, (LPWSTR)buffUtf16, nSize );

	//UTF-16からShift-JISへ変換
	const int nSizeSJis = ::WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)buffUtf16, -1, NULL, 0, NULL, NULL );
	if( !pDist ){
		*pSize = nSizeSJis;
		delete buffUtf16;
		return true;
	}

	BYTE* buffSJis = new BYTE[ nSizeSJis * 2 ];
	ZeroMemory( buffSJis, nSizeSJis * 2 );
	::WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)buffUtf16, -1, (LPSTR)buffSJis, nSizeSJis, NULL, NULL );

	*pSize = lstrlen( (char*)buffSJis );
	memcpy( pDist, buffSJis, *pSize );

	delete buffUtf16;
	delete buffSJis;
	*/

	return true;
}

//-----------------------------------------------------------------------------
/// @brief	シフトJISをUTF-16に変換
//-----------------------------------------------------------------------------
std::wstring Util::ShitJIStoUTF16( const std::string& str )
{
	int len = ::MultiByteToWideChar( CP_ACP, 0, str.c_str(), -1, NULL, 0 );

	if ( len > 0 ) { 
		std::vector<wchar_t> utf16( len );

		if ( ::MultiByteToWideChar( CP_ACP, 0, str.c_str(), -1, &utf16[0], len ) ){
			return std::wstring( &utf16[0] );
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------------
/// @brief	UTF-16をUTF-8に変換
//-----------------------------------------------------------------------------
std::string Util::UTF16toUTF8( const std::wstring& str )
{
	int len = ::WideCharToMultiByte( CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL );

	if ( len > 0 ) { 
		std::vector<char> utf8( len );

		if ( ::WideCharToMultiByte( CP_UTF8, 0, str.c_str(), -1, &utf8[0], len, NULL, NULL ) ){
			return std::string( &utf8[0] );
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------------
/// @brief	UTF-16をSJISに変換
//-----------------------------------------------------------------------------
std::string Util::UTF16toShitJIS( const std::wstring& wstr )
{
	int len = ::WideCharToMultiByte( CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL );

	if ( len>0 ) {
		std::vector<char> sjis( len );

		if ( ::WideCharToMultiByte( CP_ACP, 0, wstr.c_str(), -1, &sjis[0], len, NULL, NULL ) ){
			return std::string( &sjis[0] );
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------------
/// @brief	UTF-8をUTF-16に変換
//-----------------------------------------------------------------------------
std::wstring Util::UTF8toUTF16( const std::string& str )
{

	int len = ::MultiByteToWideChar( CP_UTF8, 0, str.c_str(), -1, NULL, 0 );

	if ( len > 0 ) { 
		std::vector<wchar_t> utf16( len );

		if ( ::MultiByteToWideChar( CP_UTF8, 0, str.c_str(), -1, &utf16[0], len ) ){
			return std::wstring( &utf16[0] );
		}
	}

	return NULL;
} 

}}