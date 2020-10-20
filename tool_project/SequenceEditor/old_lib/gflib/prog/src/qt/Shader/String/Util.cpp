#include "Util.h"

namespace gfl { namespace maya { namespace String {

//-----------------------------------------------------------------------------
/// @brief	�t�@�C���p�X�̎擾
//-----------------------------------------------------------------------------
void Util::GetFilePath( char* p_out, const char* filename )a
{
	int			size;

	size = (int)strlen( filename );

	while( size ){
		if ( filename[size] == '/' )		break;
		if ( filename[size] == '\\' )		break;
		--size;
	}

	if ( size )
		memcpy( p_out, filename, size + 1 );
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

}}}