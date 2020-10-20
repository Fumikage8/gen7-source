#ifndef GFLIB2_UTIL_STRING_CLR_H_INCLUDED
#define GFLIB2_UTIL_STRING_CLR_H_INCLUDED

#include <string>

namespace gfl2 { namespace clr { namespace util { 

public class String
{
public:
		
	static inline std::string GetNativeString( System::String^ str )
	{
		std::string				nativeStr;
		System::IntPtr		ptr;
		const char				*code_ = NULL;

		ptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( str );
		code_ = (const char*)ptr.ToPointer();
		nativeStr = code_;

		System::Runtime::InteropServices::Marshal::FreeHGlobal( System::IntPtr((void*)code_) );

    return nativeStr;
	}
};

}}}

#endif
