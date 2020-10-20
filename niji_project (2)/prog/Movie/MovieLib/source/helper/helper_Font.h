#if defined(GF_PLATFORM_CTR)
#if !defined( __HELPER_FONT_H__ )
#define __HELPER_FONT_H__
/*---------------------------------------------------------------------------*
 File:        helper_Font.h
 Description: Helper file that manages simple font drawing.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#ifndef helper_Font_h
#define helper_Font_h

/*---------------------------------------------------------------------------*/

#include <nn/types.h>

/*---------------------------------------------------------------------------*
  C++ API interface
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*/

namespace font
{
    u32  GetFontCharWidth  (void);
    u32  GetFontCharHeight (void);

    void PrintLineRgb24    (u8* dst,u32 width,u32 height,u32 posX,u32 posY,u8 r,u8 g,u8 b,const char* fmt,...);
} // end of namespace font

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/

#endif // ifdef __cplusplus

/*---------------------------------------------------------------------------*/

#endif // ifndef helper_Font_h

/*---------------------------------------------------------------------------*/

#endif // __HELPER_FONT_H__
#endif// defined(GF_PLATFORM_CTR)
