/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gx_MacroCommon.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GX_CTR_GX_MACRO_COMMON_H_
#define NN_GX_CTR_GX_MACRO_COMMON_H_

#include <GLES2/gl2.h>
#include <nn/gx/CTR/gx_MacroReg.h>

#define CMD_PADDING_NAME2( x, name ) u32 padding_##name : x
#define CMD_PADDING_NAME1( x, name ) CMD_PADDING_NAME2( x, name )
#define CMD_PADDING( x )  CMD_PADDING_NAME1( x, __LINE__ )
//----------------------------------------------------------

/*
struct CommandHeader
{
    u32 addr    : 16;
    u32 be      : 4;
    u32 size    : 8;
    CMD_PADDING(3);
    u32 seq     : 1;
};
*/

/*!  
  @defgroup     nn_gx_CTR_Pica       PicaMacroHeader
  
  @brief        @ref PICA レジスタのマクロヘッダです。    
  @{
*/

#define PICA_CMD_HEADER( addr, size, be, seq )  ((addr) | (be) << 16 | (size) << 20 | ((seq) ? 0x80000000 : 0))

#define PICA_CMD_HEADER_BURST( addr, size )            PICA_CMD_HEADER(addr, size - 1, 0xf, 0)
#define PICA_CMD_HEADER_BURST_BE( addr, size, be )     PICA_CMD_HEADER(addr, size - 1,  be, 0)
#define PICA_CMD_HEADER_BURSTSEQ( addr, size )         PICA_CMD_HEADER(addr, size - 1, 0xf, 1)
#define PICA_CMD_HEADER_BURSTSEQ_BE( addr, size, be )  PICA_CMD_HEADER(addr, size - 1,  be, 1)

#define PICA_CMD_HEADER_SINGLE( addr )                 PICA_CMD_HEADER(addr, 0, 0xf, 0)
#define PICA_CMD_HEADER_SINGLE_BE( addr, be )          PICA_CMD_HEADER(addr, 0,  be, 0)

// 浮動小数点数レジスタへの書き込み用コマンド
// PICA_REG_VS_FLOAT0 0x2c1
#define PICA_CMD_HEADER_VS_F32( size )           PICA_CMD_HEADER(PICA_REG_VS_FLOAT0, (size) * 4 -1, 0xf, 0)

// PICA_REG_GS_FLOAT0 0x291
#define PICA_CMD_HEADER_GS_F32( size )           PICA_CMD_HEADER(PICA_REG_GS_FLOAT0, (size) * 4 -1, 0xf, 0)

// 
#define PICA_CMD_DATA_ZERO() 0x00000000

typedef struct _color8
{
    GLubyte r;
    GLubyte g;
    GLubyte b;
    GLubyte a;
} color8;

#define PICA_CMD_DATA_COLOR_RGB8( r, g, b ) \
    ( r | g << 8 | b << 16 )

#define PICA_CMD_DATA_COLOR_RGBA8( r, g, b, a ) \
    ( r | g << 8 | b << 16 | a << 24 )

/*!
  @}
*/

#endif // NN_GX_CTR_GX_MACRO_COMMON_H_
