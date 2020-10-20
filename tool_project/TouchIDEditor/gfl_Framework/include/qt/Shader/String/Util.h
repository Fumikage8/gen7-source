#if !defined( GFL_MAYA_STRING_UTIL_H_ )
#define GFL_MAYA_STRING_UTIL_H_

#include <string>
#include <vector>
#include <map>

#include <qt/Shader/NativeMath/Math.h>
#include <qt/Shader/NativeMath/Util.h>
//#include <qt/Common/Platform.h>

namespace gfl { namespace maya { namespace String {

class Util
{
public:

	//-----------------------------------------------------------------------------
	/// @brief	文字コピー関数
	//-----------------------------------------------------------------------------
	static int CopyStr( ::std::string &rDest, ::std::string src, unsigned int readPos, ::std::string from, ::std::string to );

	//-----------------------------------------------------------------------------
	/// @brief	文字置き換え関数
	//-----------------------------------------------------------------------------
	static void Util::ReplaceStr(::std::string& str, const ::std::string& from, const ::std::string& to);

	//-----------------------------------------------------------------------------
	/// @brief	文字置き換え関数(ワイド文字版)
	//-----------------------------------------------------------------------------
	static void Util::ReplaceWStr(std::wstring& str, const std::wstring& from, const std::wstring& to);

	//-----------------------------------------------------------------------------
	/// @brief	文字列分離
	//-----------------------------------------------------------------------------
	static std::vector<::std::string> Util::SplitStr( ::std::string& SrcCode, const ::std::string& separate );

	//-----------------------------------------------------------------------------
	/// @brief	ファイルパスのみを取得
	//-----------------------------------------------------------------------------
	static void GetFilePath( char* p_out, const char* filename );
	static ::std::string GetFilePathStr( const char* filename );

	//-----------------------------------------------------------------------------
	/// @brief	ファイル名のみを取得
	//-----------------------------------------------------------------------------
	static int GetFileName( char* p_out, const char* filename );
	static ::std::string GetFileNameStr( const char* filename );

	//-----------------------------------------------------------------------------
	/// @brief	ファイル拡張子のみを取得
	//-----------------------------------------------------------------------------
	static int GetFileType( char* p_out, const char* filename );
	static ::std::string Util::GetFileTypeStr( const char* filename );

	static void PrintMatrix( Math::Matrix mat );

	//-----------------------------------------------------------------------------
	/// @brief	大文字小文字変換
	//-----------------------------------------------------------------------------
	static ::std::string StrToLower(::std::string src);
	static ::std::string StrToUpper(::std::string src);

};

}}}

#endif
