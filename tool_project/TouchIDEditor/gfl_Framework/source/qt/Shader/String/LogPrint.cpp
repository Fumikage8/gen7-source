#include "qt/Shader/String/LogPrint.h"
//#include <stdarg.h>
#include <windows.h>

#include "qt/Shader/String/Util.h"

#pragma warning ( disable : 4996 )

namespace gfl { namespace maya { namespace String {

LogPrint*		LogPrint::s_Gp = NULL;

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
LogPrint::LogPrint( unsigned int type, const char* log_file_path )
{
	m_hFile = NULL;
	m_Type = type;

	if ( m_Type == LogPrint::Text )
	{
		if ( log_file_path )
		{
			m_hFile = fopen( log_file_path, "w" );
		}
		else
		{
			char Path[MAX_PATH+1];
			char Dir[MAX_PATH+1];

			// 実行ファイルの完全パスを取得
			memset( Path, 0, sizeof(Path) );
			memset( Dir, 0, sizeof(Dir) );
			//GetModuleFileName( NULL, Path, MAX_PATH );
            GetModuleFileNameA( NULL, Path, MAX_PATH );

			String::Util::GetFilePath( Dir, Path );
			strcat( Dir, "\\LogPrint.txt" );
			
			m_hFile = fopen( Dir, "w" );
		}
	}
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
LogPrint::~LogPrint()
{
	if ( m_hFile )
	{
		fclose( m_hFile );
		m_hFile = NULL;
	}
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void LogPrint::Print( const char* format, ... )
{
	if ( s_Gp == NULL )		return;
	static const int			CharMax = 1024 * 512;//512k

	char tmp[CharMax];

	va_list list;

	va_start( list, format );
	vsprintf( tmp, format, list );
	va_end( list );

	switch( s_Gp->m_Type ){
	case Text:
		fprintf( s_Gp->m_hFile, tmp );
		break;
	case String:
		s_Gp->m_StringArray.push_back( tmp );
		break;
	}


}

}}}