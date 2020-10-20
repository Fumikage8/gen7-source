//===========================================================================
/**
 * @file    gfl2_TagProc.h
 * @brief   GAMEFREAK Library String System ( タグプロセッサ )
 * @author  GAMEFREAK inc.
 * @date    2010.11.12
 */
//===========================================================================
#if !defined( GFL2_TAGPROC_H_INCLUDED )
#define GFL2_TAGPROC_H_INCLUDED
#pragma once

#if defined( GF_PLATFORM_CTR )
#include <nw/font2.h>
#elif defined( GF_PLATFORM_WIN32 )
#include <nw/font.h>
#endif
#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)

typedef nw::font::TagProcessorBase<wchar_t>  TagProcBase;

GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_TAGPROC_H_INCLUDED
