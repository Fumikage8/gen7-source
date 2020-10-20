#include "qt/Shader/String/Util.h"

#include <ctype.h>

namespace gfl { namespace maya { namespace String {

//-----------------------------------------------------------------------------
/// @brief	�����R�s�[�֐�
//-----------------------------------------------------------------------------
int Util::CopyStr( std::string &rDest, std::string src, unsigned int readPos, std::string from, std::string to )
{
	int			headPos;
	int			tailPos;

	headPos = src.find( from, readPos );

	if ( headPos == std::string::npos )
		return -1;

	tailPos = src.find( to, headPos );

	if ( tailPos == std::string::npos )
		return -1;

	int			rength = tailPos - headPos + to.length();

	rDest = src.substr( headPos, rength );

	return (headPos + rength);
}

//-----------------------------------------------------------------------------
/// @brief	�����u�������֐�
//-----------------------------------------------------------------------------
void Util::ReplaceStr(std::string& str, const std::string& from, const std::string& to)
{
	std::string::size_type pos = 0;

	while(pos = str.find(from, pos), pos != std::string::npos) {
		str.replace(pos, from.length( ), to);
		pos += to.length();
	}
}

//-----------------------------------------------------------------------------
/// @brief	�����u�������֐�(���C�h������)
//-----------------------------------------------------------------------------
void Util::ReplaceWStr(std::wstring& str, const std::wstring& from, const std::wstring& to)
{
	std::wstring::size_type pos = 0;

	while(pos = str.find(from, pos), pos != std::string::npos) {
		str.replace(pos, from.length( ), to);
		pos += to.length();
	}
}

//-----------------------------------------------------------------------------
/// @brief	�����񕪗�
//-----------------------------------------------------------------------------
std::vector<std::string> Util::SplitStr( std::string& SrcCode, const std::string& separate )
{
	std::vector<std::string>			StrArray;
	int				read_pos = 0;
	int				tag_head = 0;
	int				tag_tail = 0;

	while( read_pos < static_cast<int>( SrcCode.length() ) )
	{
		//�O��̌������ʈʒu����separate������
		tag_head = static_cast<int>( SrcCode.find( separate, read_pos ) );

		if ( tag_head != -1 )
		{
			if ( (tag_head - read_pos) > 0 )
			{//�����܂ł̕�������o��
				std::string message = SrcCode.substr(read_pos, tag_head - read_pos);
				StrArray.push_back( message );
			}
			else
			{
				std::string message = "";
				StrArray.push_back( message );
			}

			tag_tail = tag_head + 1;
			read_pos = tag_tail;//�����܂œǂݍ���
		}
		else
		{//����separate�͂Ȃ��B
			break;
		}
	}

	if ( (SrcCode.length() - read_pos) > 0 )
	{//�c��̕������o��
		std::string message = SrcCode.substr(read_pos, SrcCode.length() - read_pos);
		StrArray.push_back( message );
	}

	return StrArray;
}

//-----------------------------------------------------------------------------
/// @brief	�t�@�C���p�X�̎擾
//-----------------------------------------------------------------------------
void Util::GetFilePath( char* p_out, const char* filename )
{
	int			size;

	size = (int)strlen( filename );

	while( size ){
		if ( filename[size] == '/' )		break;
		if ( filename[size] == '\\' )		break;
		--size;
	}

	if ( size )
	{
		memcpy( p_out, filename, size + 1 );
		p_out[ size + 1 ] = 0;//term code
	}
}

//-----------------------------------------------------------------------------
/// @brief	�t�@�C���p�X�̎擾
//-----------------------------------------------------------------------------
std::string Util::GetFilePathStr( const char* filename )
{
	char					filePath[128];
	String::Util::GetFilePath( filePath, filename );

	return filePath;
}

//-----------------------------------------------------------------------------
/// @brief	�t�@�C�����̎擾
//-----------------------------------------------------------------------------
int Util::GetFileName( char* p_out, const char* filename )
{
	int			tail;
	int			len;

	tail = len = (int)strlen( filename );

	while( tail ){
		if ( filename[tail] == '/' )
		{
			++tail;
			break;
		}

		if ( filename[tail] == '\\' )
		{
			++tail;
			break;
		}

		--tail;
	}

	int			copy_size = len - tail;

	if ( p_out )
	{
		memcpy( p_out, &filename[tail], copy_size );
		p_out[ copy_size ] = 0;//term code
	}
	
	return copy_size;
}

//-----------------------------------------------------------------------------
/// @brief	�t�@�C�����̎擾
//-----------------------------------------------------------------------------
std::string Util::GetFileNameStr( const char* filename )
{
	char					fileNameType[128];
	std::string		fileNameTypeStr;

	GetFileName( fileNameType, filename );
	fileNameTypeStr = fileNameType;

	return fileNameTypeStr;
}

//-----------------------------------------------------------------------------
/// @brief	�t�@�C���^�C�v�̎擾
//-----------------------------------------------------------------------------
int Util::GetFileType( char* p_out, const char* filename )
{
	int			tail;
	int			len;

	tail = len = (int)strlen( filename );

	while( tail ){
		if ( filename[tail] == '.' )
		{
			++tail;
			break;
		}

		--tail;
	}

	int			copy_size = len - tail;

	if ( p_out )
	{
		memcpy( p_out, &filename[tail], copy_size );
		p_out[ copy_size ] = 0;//term code
	}
	
	return copy_size;
}

//-----------------------------------------------------------------------------
/// @brief	�t�@�C���^�C�v�̎擾
//-----------------------------------------------------------------------------
std::string Util::GetFileTypeStr( const char* filename )
{
	char					fileType[128];
	std::string		fileTypeStr;
	String::Util::GetFileType( fileType, filename );
	fileTypeStr = fileType;

	return fileTypeStr;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
std::string Util::StrToLower(std::string src)
{
	int						size = src.length() + 1;
	char					*pLow = new char[size];
	const char*		pSrc = src.c_str();

	memset( pLow, 0, size );

	for( unsigned int i = 0; i < src.length(); ++i )
	{
		pLow[i] = tolower( pSrc[i] );
	}

	std::string			Dst = pLow;

	delete [] pLow;

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

	for( unsigned int i = 0; i < src.length(); ++i )
	{
		pUp[i] = toupper( pSrc[i] );
	}

	std::string			Dst = pUp;

	delete [] pUp;

	return ( Dst );
}

}}}