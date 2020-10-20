//==============================================================================
/**
 * @file	gfl_HioConfig.h
 * @brief	
 * @author	kimura_shin
 * @data	2011/06/24, 11:32
 */
// =============================================================================

#ifndef GFL_HIOCONFIG_H
#define	GFL_HIOCONFIG_H
#pragma once

namespace gfl { namespace hio {

enum {
	// ‚È‚ñ‚©“®‚©‚ñ‚ç‚µ‚¢‚Ì‚Å
	GFL_HIO_CHANNEL_FROM_3DS = 10, // pawn hio
	GFL_HIO_CHANNEL_TO_3DS = 11, // pawn hio
	GFL_HIO_CHANNEL_MAX,

	INIT = 0,
	WAIT_FOR_CONNECT,
	CONNECTED,
	TERMINATE,
	RESET,
		
	GFL_HIO_ALIGN = 32
};

#define USE_GFL_NEW 1
#define RECEIVED_STRING "!"

//static const c8 * const s_pTerminationString = "GflHioTerminate";

} }

#endif	/* GFL_HIOCONFIG_H */

