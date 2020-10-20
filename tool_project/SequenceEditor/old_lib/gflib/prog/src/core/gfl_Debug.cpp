
// =================================================================
// Debug
// =================================================================
// -----------------------------------------------------------------
// Print console
// -----------------------------------------------------------------
#include <stdarg.h>
void gfl::core::Debug::PrintConsole( const c8 * const str, ... )
{
#ifndef GFL_NO_PRINT_CONSOLE
#if defined(GFL_PLATFORM_3DS)
	va_list VA;

	va_start( VA, str );

	nn::dbg::detail::VPrintf( str, VA );

	va_end( VA );

#else // if 1 //def GFL_DEBUG
	va_list VA;

#if 0 // Ç«Ç¡ÇøÇ™Ç¢Ç¢ÇÃÇ‚ÇÁÅc
	const size_t GFL_DEBUG_BUFFER_SIZE = strlen( str ) + 0x100;
	c8* tmp = static_cast < c8* >( malloc( GFL_DEBUG_BUFFER_SIZE ) );
#else
	const size_t GFL_DEBUG_BUFFER_SIZE = 0x200;
	c8 tmp[GFL_DEBUG_BUFFER_SIZE];
#endif

	va_start( VA, str );
	vsprintf( tmp, str, VA );
	va_end( VA );

	PrintConsoleDirect( tmp );
	//	free(tmp);
#endif
#endif
}
