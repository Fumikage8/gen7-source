//==============================================================================
/**
 * @file	gflib.h
 * @brief	
 * @author	kimura_shin
 * @data	2011/04/06, 16:05
 */
// =============================================================================

#ifndef GFLIB_H
#define	GFLIB_H

#pragma once

// プラットフォーム別の定義が収めてあります
#include <core/gfl_Config.h>

#ifdef GFL_PLATFORM_PC // ======================================================

#include <gfl_Pc.h>

#elif defined(GFL_PLATFORM_3DS) // =============================================

#include <gflib_3ds.h> // ごめんなさい、元々の gflib.h です。

#endif // ======================================================================

#include <gfl_Framework.h>

#endif	/* GFLIB_H */
