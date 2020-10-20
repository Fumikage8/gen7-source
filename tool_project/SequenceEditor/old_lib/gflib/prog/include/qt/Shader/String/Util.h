#if !defined( GFL_MAYA_STRING_UTIL_H_ )
#define GFL_MAYA_STRING_UTIL_H_

#include <string>
#include <vector>
#include <map>

#include <qt/Shader/NativeMath/Math.h>
#include <qt/Shader/NativeMath/NativeUtil.h>
//#include <qt/Common/Platform.h>

namespace gfl { namespace maya { namespace String {

class Util
{
public:

	//-----------------------------------------------------------------------------
	/// @brief	�����R�s�[�֐�
	//-----------------------------------------------------------------------------
	static int CopyStr( ::std::string &rDest, ::std::string src, unsigned int readPos, ::std::string from, ::std::string to );

	//-----------------------------------------------------------------------------
	/// @brief	�����u�������֐�
	//-----------------------------------------------------------------------------
	static void Util::ReplaceStr(::std::string& str, const ::std::string& from, const ::std::string& to);

	//-----------------------------------------------------------------------------
	/// @brief	�����u�������֐�(���C�h������)
	//-----------------------------------------------------------------------------
	static void Util::ReplaceWStr(std::wstring& str, const std::wstring& from, const std::wstring& to);

	//-----------------------------------------------------------------------------
	/// @brief	�����񕪗�
	//-----------------------------------------------------------------------------
	static std::vector<::std::string> Util::SplitStr( ::std::string& SrcCode, const ::std::string& separate );

	//-----------------------------------------------------------------------------
	/// @brief	�t�@�C���p�X�݂̂��擾
	//-----------------------------------------------------------------------------
	static void GetFilePath( char* p_out, const char* filename );
	static ::std::string GetFilePathStr( const char* filename );

	//-----------------------------------------------------------------------------
	/// @brief	�t�@�C�����݂̂��擾
	//-----------------------------------------------------------------------------
	static int GetFileName( char* p_out, const char* filename );
	static ::std::string GetFileNameStr( const char* filename );

	//-----------------------------------------------------------------------------
	/// @brief	�t�@�C���g���q�݂̂��擾
	//-----------------------------------------------------------------------------
	static int GetFileType( char* p_out, const char* filename );
	static ::std::string Util::GetFileTypeStr( const char* filename );

	static void PrintMatrix( Math::Matrix mat );

	//-----------------------------------------------------------------------------
	/// @brief	�啶���������ϊ�
	//-----------------------------------------------------------------------------
	static ::std::string StrToLower(::std::string src);
	static ::std::string StrToUpper(::std::string src);

};

}}}

#endif
