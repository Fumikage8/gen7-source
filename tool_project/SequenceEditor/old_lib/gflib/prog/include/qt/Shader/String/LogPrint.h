#ifndef __LogPrint_h_
#define __LogPrint_h_

#include <stdio.h>
#include <string.h>
#include <vector>

namespace gfl { namespace maya { namespace String {

//-----------------------------------------------------------------------------
/**
 * @class	LogPrint
 * @brief	LogPrintÉNÉâÉX
 */
class LogPrint
{
public:

	enum{
		Console = 0,
		Text,
		String,
	};

	///! èâä˙âª
	static void Initialize( unsigned int type, const char* log_file_path = NULL )
	{
		if ( s_Gp == NULL )
		{
			s_Gp = new LogPrint( type, log_file_path );
		}
	}
	
	///! îjä¸
	static void Terminate()
	{
		if ( s_Gp )
		{
			delete s_Gp;
			s_Gp = NULL;
		}
	}

	static inline void PrintStr( std::string str );
	static void Print( const char* message, ... );
	static inline std::vector<std::string> GetStringArray();
	static inline unsigned int GetType();

	~LogPrint();

private:
	LogPrint(){}
	LogPrint( unsigned int type, const char* log_file_path = NULL );

	static LogPrint								*s_Gp;

	FILE													*m_hFile;
	unsigned int									m_Type;
	std::vector<std::string>			m_StringArray;
};

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
inline void LogPrint::PrintStr( std::string str )
{
	Print( str.c_str() );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
inline std::vector<std::string> LogPrint::GetStringArray()
{
	return s_Gp->m_StringArray;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
inline unsigned int LogPrint::GetType()
{
	return s_Gp->m_Type;
}

}}}

#endif