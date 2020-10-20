/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gx_MacroOld.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GX_CTR_GX_MACRO_OLD_H_
#define NN_GX_CTR_GX_MACRO_OLD_H_

#include <nn/gx/CTR/gx_MacroCommon.h>



enum
{
    PICA_REG_FRAG_OP_CLIP           = 0x047,
    PICA_REG_FRAG_OP_CLIP_DATA1     = 0x048,
    PICA_REG_FRAG_OP_CLIP_DATA2     = 0x049,
    PICA_REG_FRAG_OP_CLIP_DATA3     = 0x04a,
    PICA_REG_FRAG_OP_CLIP_DATA4     = 0x04b,

    PICA_REG_FRAG_OP_WSCALE_DATA1   = 0x04d,
    PICA_REG_FRAG_OP_WSCALE_DATA2   = 0x04e,

    PICA_REG_FRAG_OP_WSCALE         = 0x06d,

    PICA_REG_RENDER_BUF_RESOLUTION1 = 0x06e
};

enum
{
    PICA_REG_TEXTURE3_PROTEX0     = 0x0a8,
    PICA_REG_TEXTURE3_PROTEX1     = 0x0a9,  
    PICA_REG_TEXTURE3_PROTEX2     = 0x0aa,  
    PICA_REG_TEXTURE3_PROTEX3     = 0x0ab,  
    PICA_REG_TEXTURE3_PROTEX4     = 0x0ac,  
    PICA_REG_TEXTURE3_PROTEX5     = 0x0ad,

    PICA_REG_PROTEX_LUT           = 0x0af,
    PICA_REG_PROTEX_LUT_DATA0     = 0x0b0,  
    PICA_REG_PROTEX_LUT_DATA1     = 0x0b1,  
    PICA_REG_PROTEX_LUT_DATA2     = 0x0b2,  
    PICA_REG_PROTEX_LUT_DATA3     = 0x0b3,  
    PICA_REG_PROTEX_LUT_DATA4     = 0x0b4,  
    PICA_REG_PROTEX_LUT_DATA5     = 0x0b5,  
    PICA_REG_PROTEX_LUT_DATA6     = 0x0b6,  
    PICA_REG_PROTEX_LUT_DATA7     = 0x0b7
};

enum
{
    PICA_REG_TEX_ENV_0            = 0x0c0,
    PICA_REG_TEX_ENV_0_OPERAND    = 0x0c1,
    PICA_REG_TEX_ENV_0_COMBINE    = 0x0c2,
    PICA_REG_TEX_ENV_0_COLOR      = 0x0c3,
    PICA_REG_TEX_ENV_0_SCALE      = 0x0c4,

    PICA_REG_TEX_ENV_1            = 0x0c8,
    PICA_REG_TEX_ENV_1_OPERAND    = 0x0c9,
    PICA_REG_TEX_ENV_1_COMBINE    = 0x0ca,
    PICA_REG_TEX_ENV_1_COLOR      = 0x0cb,
    PICA_REG_TEX_ENV_1_SCALE      = 0x0cc,

    PICA_REG_TEX_ENV_2            = 0x0d0,
    PICA_REG_TEX_ENV_2_OPERAND    = 0x0d1,
    PICA_REG_TEX_ENV_2_COMBINE    = 0x0d2,
    PICA_REG_TEX_ENV_2_COLOR      = 0x0d3,
    PICA_REG_TEX_ENV_2_SCALE      = 0x0d4,

    PICA_REG_TEX_ENV_3            = 0x0d8,
    PICA_REG_TEX_ENV_3_OPERAND    = 0x0d9,
    PICA_REG_TEX_ENV_3_COMBINE    = 0x0da,
    PICA_REG_TEX_ENV_3_COLOR      = 0x0db,
    PICA_REG_TEX_ENV_3_SCALE      = 0x0dc,

    PICA_REG_TEX_ENV_4            = 0x0f0,
    PICA_REG_TEX_ENV_4_OPERAND    = 0x0f1,
    PICA_REG_TEX_ENV_4_COMBINE    = 0x0f2,
    PICA_REG_TEX_ENV_4_COLOR      = 0x0f3,
    PICA_REG_TEX_ENV_4_SCALE      = 0x0f4,

    PICA_REG_TEX_ENV_BUF_COLOR    = 0x0fd,

    PICA_REG_TEX_ENV_5            = 0x0f8,
    PICA_REG_TEX_ENV_5_OPERAND    = 0x0f9,
    PICA_REG_TEX_ENV_5_COMBINE    = 0x0fa,
    PICA_REG_TEX_ENV_5_COLOR      = 0x0fb,
    PICA_REG_TEX_ENV_5_SCALE      = 0x0fc,

    PICA_REG_TEX_ENV_BUF_INPUT    = 0x0e0
};

enum
{
    PICA_REG_FRAG_OP_ALPHA_TEST     = 0x104,

    PICA_REG_COLOR_BUFFER_CLEAR0    = 0x110,
    PICA_REG_COLOR_BUFFER_CLEAR1    = 0x111,    
    
    PICA_REG_DEPTH_STENCIL_READ     = 0x114,
    PICA_REG_DEPTH_STENCIL_WRITE    = 0x115,   

    PICA_REG_RENDER_BUF_DEPTH_MODE  = 0x116,
    PICA_REG_RENDER_BUF_COLOR_MODE  = 0x117,

    PICA_REG_RENDER_BUF_DEPTH_ADDR  = 0x11c,
    PICA_REG_RENDER_BUF_COLOR_ADDR  = 0x11d,
    PICA_REG_RENDER_BUF_RESOLUTION0 = 0x11e,

    PICA_REG_FRAG_OP_SHADOW         = 0x130
};

enum
{    
    PICA_REG_VTX_ATTR_ARRAYS_BASE_ADDR = 0x200,
    PICA_REG_VTX_ATTR_ARRAYS0          = 0x201,
    PICA_REG_VTX_ATTR_ARRAYS1          = 0x202,
    
    PICA_REG_VTX_FUNC                  = 0x231,

    PICA_REG_GS_PROG_RENEWAL_END       = 0x28f,
    PICA_REG_GS_FLOAT_DATA1            = 0x291,
    PICA_REG_GS_FLOAT_DATA2            = 0x292,
    PICA_REG_GS_FLOAT_DATA3            = 0x293,
    PICA_REG_GS_FLOAT_DATA4            = 0x294,
    PICA_REG_GS_FLOAT_DATA5            = 0x295,
    PICA_REG_GS_FLOAT_DATA6            = 0x296,
    PICA_REG_GS_FLOAT_DATA7            = 0x297,
    PICA_REG_GS_FLOAT_DATA8            = 0x298,

    PICA_REG_VS_PROG_RENEWAL_END       = 0x2bf,
    PICA_REG_VS_FLOAT_DATA1            = 0x2c1,
    PICA_REG_VS_FLOAT_DATA2            = 0x2c2,
    PICA_REG_VS_FLOAT_DATA3            = 0x2c3,
    PICA_REG_VS_FLOAT_DATA4            = 0x2c4,
    PICA_REG_VS_FLOAT_DATA5            = 0x2c5,
    PICA_REG_VS_FLOAT_DATA6            = 0x2c6,
    PICA_REG_VS_FLOAT_DATA7            = 0x2c7,
    PICA_REG_VS_FLOAT_DATA8            = 0x2c8
};



//
// gx_MacroCommon.h
//

#define PICA_CMD_HEADER_V_SHADER_F32( size ) PICA_CMD_HEADER(0x2c1, (size) * 4 -1, 0xf, 0)



//
// gx_MacroMisc.h
//

enum 
{
    PICA_DATA_1_0  = 0x0,
    PICA_DATA_2_0  = 0x1,
    PICA_DATA_4_0  = 0x2,
    PICA_DATA_8_0  = 0x3,
    PICA_DATA_0_25 = 0x6,
    PICA_DATA_0_5  = 0x7
}; 

///////////////////////////////////
// Fragment Operation Double buffer

// PICA_REG_FOG_OP_WSCALE          0x06d
#define PICA_CMD_DATA_FOG_OP_WSCALE(mode) ((mode == 0) ? 1 : 0)

// PICA_REG_FOG_OP_WSCALE_DATA1    0x04d
// PICA_REG_FOG_OP_WSCALE_DATA2    0x04e
#define PICA_CMD_DATA_FOG_OP_WSCALE_DATA(data) (data)

///////////////////////////////////
// Fragment Operation User Clipping

// PICA_REG_FOG_OP_CLIP            0x047

// 0x47 [0:0]
#define PICA_CMD_DATA_FOG_OP_CLIP(mode) ((mode) ? 1 : 0)

#define PICA_CMD_SET_FOG_OP_CLIP(mode) \
    PICA_CMD_DATA_FOG_OP_CLIP(mode),   \
    PICA_CMD_HEADER_SINGLE_BE(PICA_REG_FRAGOP_CLIP, 0x1)

// PICA_REG_FOG_OP_CLIP_DATA1      0x048
// PICA_REG_FOG_OP_CLIP_DATA2      0x049
// PICA_REG_FOG_OP_CLIP_DATA3      0x04a
// PICA_REG_FOG_OP_CLIP_DATA4      0x04b
#define PICA_CMD_DATA_FOG_OP_CLIP_DATA(data) (data)

enum
{
    PICA_DATA_FRAGOP_MODE         = 0x0,
    PICA_DATA_FRAGOP_MODE_GAS_ACC = 0x1,
    PICA_DATA_FRAGOP_MODE_SHADOW  = 0x3
};

#define PICA_CMD_DATA_FRAG_OP_ALPHA_TEST_DISABLE() 0x0
  
// PICA_REG_FRAG_OP_ALPHA_TEST      0x104
#define PICA_CMD_DATA_FRAG_OP_ALPHA_TEST( enable, func, value ) \
    ( enable | (func) << 4 | (value) << 8 )

// PICA_REG_RENDER_BUF_COLOR_ADDR   0x11d
#define PICA_CMD_DATA_RENDER_BUF_COLOR_ADDR( addr ) ( (addr) >> 3 )
   
// PICA_REG_RENDER_BUF_DEPTH_ADDR   0x11c
#define PICA_CMD_DATA_RENDER_BUF_DEPTH_ADDR( addr ) ( (addr) >> 3 )

// PICA_REG_RENDER_BUF_COLOR_MODE   0x117
#define PICA_CMD_DATA_RENDER_BUF_COLOR_MODE( pixSize, format ) \
    ( (pixSize) | (format) << 16 )

// PICA_REG_RENDER_BUF_DEPTH_MODE   0x116
#define PICA_CMD_DATA_RENDER_BUF_DEPTH_MODE( mode ) (mode)

/*
struct CommandRenderBufResolution
{
    u32 width   : 11;
    CMD_PADDING(1);
    u32 height  : 10;
    CMD_PADDING(2);
    CMD_PADDING(8):   // 0x01
};
*/
// PICA_REG_RENDER_BUF_RESOLUTION0    0x11e
// PICA_REG_RENDER_BUF_RESOLUTION1    0x06e
#define PICA_CMD_DATA_RENDER_BUF_RESOLUTION( width, height ) ( (width) | (height) << 12 | 0x01000000 )

#define PICA_CMD_DATA_COLOR_BUFFER_CLEAR( data ) \
    ( (data) ? 1 : 0 )

#define PICA_CMD_SET_COLOR_BUFFER_CLEAR( data1, data2 ) \
    PICA_CMD_DATA_COLOR_BUFFER_CLEAR(data1), PICA_CMD_HEADER_SINGLE(PICA_REG_COLOR_BUFFER_CLEAR1), \
    PICA_CMD_DATA_COLOR_BUFFER_CLEAR(data2), PICA_CMD_HEADER_SINGLE(PICA_REG_COLOR_BUFFER_CLEAR0)

#define PICA_CMD_DATA_FOG_OP_SHADOW(penumbraScale, penumbraBias) \
    (( (penumbraScale) << 16) | (penumbraBias) )


//
// gx_MacroShader.h
//
enum
{
    PICA_DATA_VS_OUTATTR_X        = 0x00,
    PICA_DATA_VS_OUTATTR_Y        = 0x01,
    PICA_DATA_VS_OUTATTR_Z        = 0x02,
    PICA_DATA_VS_OUTATTR_W        = 0x03,
    PICA_DATA_VS_OUTATTR_QAURTX   = 0x04,
    PICA_DATA_VS_OUTATTR_QAURTY   = 0x05,
    PICA_DATA_VS_OUTATTR_QAURTZ   = 0x06,
    PICA_DATA_VS_OUTATTR_QAURTW   = 0x07,
    PICA_DATA_VS_OUTATTR_R        = 0x08,
    PICA_DATA_VS_OUTATTR_G        = 0x09,
    PICA_DATA_VS_OUTATTR_B        = 0x0a,
    PICA_DATA_VS_OUTATTR_A        = 0x0b,
    PICA_DATA_VS_OUTATTR_TEX0_U   = 0x0c,
    PICA_DATA_VS_OUTATTR_TEX0_V   = 0x0d,
    PICA_DATA_VS_OUTATTR_TEX1_U   = 0x0e,
    PICA_DATA_VS_OUTATTR_TEX1_V   = 0x0f,
    PICA_DATA_VS_OUTATTR_TEX0_W   = 0x10,
    PICA_DATA_VS_OUTATTR_VIEW_X   = 0x12,
    PICA_DATA_VS_OUTATTR_VIEW_Y   = 0x13,
    PICA_DATA_VS_OUTATTR_VIEW_Z   = 0x14,
    PICA_DATA_VS_OUTATTR_TEX2_U   = 0x16,
    PICA_DATA_VS_OUTATTR_TEX2_V   = 0x17,
    PICA_DATA_VS_OUTATTR_INVALID  = 0x1f
};

// PICA_REG_VS_FLOAT_DATA  0x2c1 to 0x2c8
#define PICA_CMD_DATA_VS_FLOAT_DATA(data) (data)

// PICA_REG_VS_ATTR_IN_REG_MAP1  0x2bc
#define PICA_CMD_DATA_VS_ATTR_IN_MAP1(index9, index10, index11, index12) \
      ((index9) | (index10) << 4 | (index11) << 8 | (index12) << 12)

#define PICA_CMD_DATA_VTX_ATTR_ARRAYS_BASE_ADDR(addr) ( ((u32)(addr) >> 4) << 1 )

#define PICA_CMD_DATA_VTX_ATTR_ARRAYS(attr0, attr1, attr2, attr3, attr4, attr5, attr6, attr7) \
   ((attr0)       | (attr1) <<  4 | (attr2) <<  8 | (attr3) << 12 | \
    (attr4) << 16 | (attr5) << 20 | (attr6) << 24 | (attr7) << 28)

// PICA_REG_VERTEX_ATTR_ARRAYS0         0x201
#define PICA_CMD_DATA_VERTEX_ATTR_ARRAYS(attr0, attr1, attr2, attr3, attr4, attr5, attr6, attr7) \
   ((attr0)       | (attr1) <<  4 | (attr2) <<  8 | (attr3) << 12 | \
    (attr4) << 16 | (attr5) << 20 | (attr6) << 24 | (attr7) << 28)

// PICA_REG_LOAD_ARRAY0_ELEMENT0         0x204
#define PICA_CMD_DATA_LOAD_ARRAY0_ELEMENT(attr0, attr1, attr2, attr3, attr4, attr5, attr6, attr7) \
   ( (attr0)       | (attr1) << 4  | (attr2) << 8  | (attr3) << 12 | \
     (attr4) << 16 | (attr5) << 20 | (attr6) << 24 | (attr7) << 28)

enum
{
    PICA_DATA_VS_OUT_ATTR_QAURTX   = 0x04,
    PICA_DATA_VS_OUT_ATTR_QAURTY   = 0x05,
    PICA_DATA_VS_OUT_ATTR_QAURTZ   = 0x06,
    PICA_DATA_VS_OUT_ATTR_QAURTW   = 0x07
};

enum
{
    PICA_DATA_BIT_MODE_F32 = 0x80000000,
    PICA_DATA_BIT_MODE_F24 = 0x0
};

enum
{
    PICA_DATA_GS_USE    = 0x08,
    PICA_DATA_GS_NO_USE = 0xa0
};

#define PICA_CMD_SET_GS_MISC_REG0_DISABLE() \
    0x00000000, PICA_CMD_HEADER_SINGLE(PICA_REG_GS_MISC_REG0)



//
// gx_MacroTexture.h
//
// PICA_REG_TEXTURE0_ADDR1      0x085
#define PICA_CMD_DATA_TEXTURE_CUBE(cubeMap) \
    ( (cubeMap) & 0x3fffff )

enum
{
    PICA_DATA_PRIMARY_COLOR            = 0x0,
    PICA_DATA_FRAGMENT_PRIMARY_COLOR   = 0x1,
    PICA_DATA_FRAGMENT_SECONDARY_COLOR = 0x2,
    PICA_DATA_TEXTURE0                 = 0x3,
    PICA_DATA_TEXTURE1                 = 0x4,
    PICA_DATA_TEXTURE2                 = 0x5,
    PICA_DATA_TEXTURE3                 = 0x6,
    PICA_DATA_PREVIOUS_BUFFER          = 0xd,
    PICA_DATA_CONSTANT                 = 0xe,
    PICA_DATA_PREVIOUS                 = 0xf
};

enum
{
    PICA_DATA_REPLACE     = 0x0,
    PICA_DATA_MODULATE    = 0x1,
    PICA_DATA_ADD         = 0x2,
    PICA_DATA_ADD_SIGNED  = 0x3,
    PICA_DATA_INTERPOLATE = 0x4,
    PICA_DATA_SUBTRACT    = 0x5,
    PICA_DATA_DOT3_RGB    = 0x6,
    PICA_DATA_DOT3_RGBA   = 0x7,
    PICA_DATA_MULT_ADD    = 0x8,
    PICA_DATA_ADD_MULT    = 0x9
};

enum
{
    PICA_DATA_SCALE_1 = 0x0,
    PICA_DATA_SCALE_2 = 0x1,
    PICA_DATA_SCALE_4 = 0x2
};

#define PICA_CMD_DATA_COLOR_WHITE 0xffffffff
#define PICA_CMD_DATA_COLOR_BLACK 0x00000000

enum
{
    PICA_DATA_TEX_ENV_PREVIOUS_BUFFER = 0x0,
    PICA_DATA_TEX_ENV_PREVIOUS        = 0x1
};

#define PICA_CMD_DATA_BUFFER_INPUT(i, bufInput1, bufInput2) \
    ((bufInput1) << 7+(i) | (bufInput2) << 11+(i))

// PICA_REG_TEXTURE0_SHADOW      0x08b
#define PICA_CMD_DATA_TEX_SHADOW(perspectiveShadow, shadowZBias, shadowZScale) \
    (( (perspectiveShadow) ? 0 : 1) | (shadowZBias) << 1 | (shadowZScale) << 24)

// PICA_REG_TEXTURE_FUNC          0x080
#define PICA_CMD_DATA_TEX_FUNC(type0_0, type1, type2, texcoord3, type3, texcoord2, clear) \
    ( ((type0_0) ? 1 : 0 | ((type1) ? 1 : 0) << 1 | ((type2) ? 1 : 0) << 2 | \
    0 << 3 | (texcoord3) << 8 | ((type3) ? 1 : 0) << 10) | 1 << 12 | ((texcoord2) ? 1 : 0) << 13 | (clear) << 16 | 0 << 24 )

// PICA_REG_PROTEX_LUT         0x0af
#define PICA_CMD_DATA_PROTEX_LUT(index, attrib) \
    ((index) | (attrib) << 11)

// PICA_REG_PROTEX_LUT_DATA0  0x0b0
// PICA_REG_PROTEX_LUT_DATA1  0x0b1
// PICA_REG_PROTEX_LUT_DATA2  0x0b2
// PICA_REG_PROTEX_LUT_DATA3  0x0b3
// PICA_REG_PROTEX_LUT_DATA4  0x0b4
// PICA_REG_PROTEX_LUT_DATA5  0x0b5
// PICA_REG_PROTEX_LUT_DATA6  0x0b6
// PICA_REG_PROTEX_LUT_DATA7  0x0b7
#define PICA_CMD_DATA_PROTEX_LUT_DATA(data) (data)

enum
{
    PICA_DATA_DOT_3_RGB   = 0x6,
    PICA_DATA_DOT_3_RGBA  = 0x7
};

enum   //  Texture Sampler
{
    PICA_DATA_TEX0_1_GL_TEXTURE_2D              = 0x0,
    PICA_DATA_TEX0_1_GL_TEXTURE_CUBE_MAP        = 0x1,
    PICA_DATA_TEX0_1_GL_TEXTURE_SHADOW_2D_DMP   = 0x2,
    PICA_DATA_TEX0_1_GL_TEXTURE_PROJECTION_DMP  = 0x3,
    PICA_DATA_TEX0_1_GL_TEXTURE_SHADOW_CUBE_DMP = 0x4
};

enum
{
    PICA_DATA_TEXCOORD3_TEXTURE0 = 0x0,
    PICA_DATA_TEXCOORD3_TEXTURE1 = 0x1,
    PICA_DATA_TEXCOORD3_TEXTURE2 = 0x2
};

#define PICA_CMD_DATA_BUF_INPUT(bufInput1, bufInput2, i) \
    ((bufInput1) << 7+(i) | (bufInput2) << 11+(i))
    
enum
{
    PICA_DATA_CLAMP_TO_ZERO_DMP       = 0x0,
    PICA_DATA_CLAMP_TO_EDGE           = 0x1,
    PICA_DATA_SYMMETRICAL_REPEAT_DMP  = 0x2,
    PICA_DATA_MIRRORED_REPEAT         = 0x3,
    PICA_DATA_PULSE_DMP               = 0x4
};

enum
{
    PICA_DATA_NONE_DMP     = 0x0,
    PICA_DATA_ODD_DMP      = 0x1,
    PICA_DATA_EVEN_DMP     = 0x2
};

// PICA_REG_TEXTURE3_PROTEX0     0x0a8
#define PICA_CMD_DATA_PROTEX0(ptClampU, ptClampV, ptRgbMap, ptAlphaMap, \
    ptAlphaSeparate, ptNoiseEnable, ptShiftU, ptShiftV, ptTexBias1) \
    ((ptClampU) | (ptClampV) << 3 | (ptRgbMap) << 6 | (ptAlphaMap) << 10 | \
    (ptAlphaSeparate) << 14 | (ptNoiseEnable) << 15 | (ptShiftU) << 16 | \
    (ptShiftV) << 18 | (ptTexBias1) << 20)
   
/*
struct CommandProTex1 
{
    u32 ptNoiseU1  : 32;
};
*/

// PICA_REG_TEXTURE3_PROTEX1    0x0a9
#define PICA_CMD_DATA_PROTEX1(ptNoiseU1) (ptNoiseU1)

/*
struct CommandProTex2   
{
    u32 ptNoiseV1  : 32;
};
*/
// PICA_REG_TEXTURE3_PROTEX2    0x0aa
#define PICA_CMD_DATA_PROTEX2(ptNoiseV1) (ptNoiseV1)

/*
struct CommandProTex3  
{
    u32 ptNoiseU2  : 16;
    u32 ptNoiseV2  : 16;
};
*/
// PICA_REG_TEXTURE3_PROTEX3    0x0ab
#define PICA_CMD_DATA_PROTEX3(ptNoiseU2, ptNoiseV2) \
    ( (ptNoiseU2) | (ptNoiseV2) << 16 )

// PICA_REG_TEXTURE3_PROTEX4    0x0ac
#define PICA_CMD_DATA_PROTEX4(ptMinFilter, ptTexWidth, ptTexBias2) \
    ((ptMinFilter) | 0x60 << 3 | (ptTexWidth) << 11 | (ptTexBias2) << 19)

/*
struct CommandProTex5  
{
    u32 ptTexOffset  : 8;
    CMD_PADDING(24);   // 0xe0c080
};
*/

// PICA_REG_TEXTURE3_PROTEX5    0x0ad
#define PICA_CMD_DATA_PROTEX5(ptTexOffset) ( (ptTexOffset) | 0xe0c0080 )

enum
{
    PICA_DATA_NOISE_REF_TABLE      = 0x0,
    PICA_DATA_RGB_MAP_REF_TABLE    = 0x2,
    PICA_DATA_ALPHA_MAP_REF_TABLE  = 0x3,
    PICA_DATA_COLOR_REF_TABLE      = 0x4,
    PICA_DATA_COLOR_DIFF_REF_TABLE = 0x5
};

// PICA_REG_TEXTURE0_SIZE        0x082
// PICA_REG_TEXTURE1_SIZE        0x092
// PICA_REG_TEXTURE2_SIZE        0x09a
#define PICA_CMD_DATA_TEX_SIZE( width, height ) \
    ( (width) << 16 | (height) )

enum
{
    PICA_DATA_TEX_FORMAT_RGBA8  = 0x0,
    PICA_DATA_TEX_FORMAT_RGB8   = 0x1,
    PICA_DATA_TEX_FORMAT_RGB5A1 = 0x2,
    PICA_DATA_TEX_FORMAT_RGB565 = 0x3,
    PICA_DATA_TEX_FORMAT_RGBA4  = 0x4,
    PICA_DATA_TEX_FORMAT_LA8    = 0x5,
    PICA_DATA_TEX_FORMAT_HILO8  = 0x6,
    PICA_DATA_TEX_FORMAT_L8     = 0x7,
    PICA_DATA_TEX_FORMAT_A8     = 0x8,
    PICA_DATA_TEX_FORMAT_LA4    = 0x9,
    PICA_DATA_TEX_FORMAT_L4     = 0xa,
    PICA_DATA_TEX_FORMAT_A4     = 0xb,
    PICA_DATA_TEX_FORMAT_ETC1   = 0xc,
    PICA_DATA_TEX_FORMAT_ETC1A4 = 0xd
};

enum
{
    PICA_DATA_TEXTURE_FORMAT_HILO8_DMP_UNSIGNED_BYTE_DMP = 0x6
};

// PICA_REG_TEXTURE0_FORMAT      0x08e
// PICA_REG_TEXTURE1_FORMAT      0x096
// PICA_REG_TEXTURE2_FORMAT      0x09e
#define PICA_CMD_DATA_TEX_FORMAT_TYPE( type ) (type)

enum
{
    PICA_DATA_TEX_NEAREST = 0,
    PICA_DATA_TEX_LINEAR  = 1
};

enum
{
    PICA_DATA_NEAREST                = 0x0,
    PICA_DATA_LINEAR                 = 0x1,
    PICA_DATA_NEAREST_MIPMAP_NEAREST = 0x2,
    PICA_DATA_LINEAR_MIPMAP_NEAREST  = 0x3,
    PICA_DATA_NEAREST_MIPMAP_LINEAR  = 0x4,
    PICA_DATA_LINEAR_MIPMAP_LINEAR   = 0x5
};

enum
{
    PICA_DATA_TEX_CLAMP_TO_EDGE   = 0,
    PICA_DATA_TEX_CLAMP_TO_BORDER = 1,
    PICA_DATA_TEX_REPEAT          = 2,
    PICA_DATA_TEX_MIRRORED_REPEAT = 3
};

// PICA_REG_TEXTURE0_WRAP_FILTER  0x083
#define PICA_CMD_DATA_TEX0_WRAP_FILTER( magFilter, minFilter, isETC1, wrapT, wrapS, enable, minFilter2, type1_1 ) \
    ( (magFilter) << 1 | (minFilter) << 2 | ( (isETC1) ? 2 : 0 ) << 4 | (wrapT) << 8 | (wrapS) << 12 | 0 << 16 | (enable) << 20 | \
     (minFilter2) << 24 | (type1_1) << 28)

// PICA_REG_TEXTURE1_WRAP_FILTER   0x093
#define PICA_CMD_DATA_TEX1_WRAP_FILTER( magFilter, minFilter1, isETC1, wrapT, wrapS, minFilter2) \
    ( (magFilter) << 1 | (minFilter) << 2 | ((isETC1) ? 2 : 0) << 4 | (wrapT) << 8 | (wrapS) << 12 | 0 << 16 | (minFilter2) << 24 )

// PICA_REG_TEXTURE2_WRAP_FILTER   0x09b
#define PICA_CMD_DATA_TEX2_WRAP_FILTER( magFilter, minFilter1, isETC1, wrapT, wrapS, minFilter2) \
    ( (magFilter) << 1 | (minFilter) << 2 | ((isETC1) ? 2 : 0) << 4 | (wrapT) << 8 | (wrapS) << 12 | 0 << 16 | (minFilter2) << 24 )

#define PICA_CMD_DATA_TEX_LOD_REV(bias, max, min) \
    ( (bias) | (max) << 16 | (((min) < 0) ? 0 : (min)) << 24 )



//
// gx_MacroFragment.h
//

// PICA_REG_FRAG_LIGHT_AMBIENT     0x1c0
#define PICA_CMD_DATA_FRAG_AMBIENT(color) \
    ( (color.b) | (color.g) << 10 | (color.r) << 20 )

enum
{
    PICA_DATA_SAMPLER_FG = 0x5,
    PICA_DATA_SAMPLER_FB = 0x4
};

enum
{
    PICA_DATA_FRAG_TEXTURE0 = 0x0,
    PICA_DATA_FRAG_TEXTURE1 = 0x1,
    PICA_DATA_FRAG_TEXTURE2 = 0x2,
    PICA_DATA_FRAG_TEXTURE3 = 0x3
};

enum
{
    PICA_DATA_FRAG_LIGHT_TEXTURE0 = 0x0,
    PICA_DATA_FRAG_LIGHT_TEXTURE1 = 0x1,
    PICA_DATA_FRAG_LIGHT_TEXTURE2 = 0x2,
    PICA_DATA_FRAG_LIGHT_TEXTURE3 = 0x3
};

enum
{
    PICA_DATA_SAMPLERD0 = 0x0,
    PICA_DATA_SAMPLERD1 = 0x1,
    PICA_DATA_SAMPLERFR = 0x3,
    PICA_DATA_SAMPLERFB = 0x4,
    PICA_DATA_SAMPLERFG = 0x5,
    PICA_DATA_SAMPLERRR = 0x6,
    PICA_DATA_SAMPLERSP = 0x8,
    PICA_DATA_SAMPLERDA = 0x10
};

enum
{
    PICA_DATA_NH_DMP = 0x0,
    PICA_DATA_VH_DMP = 0x1,
    PICA_DATA_NV_DMP = 0x2,
    PICA_DATA_LN_DMP = 0x3,
    PICA_DATA_SP_DMP = 0x4,
    PICA_DATA_CP_DMP = 0x5
};    

#define PICA_CMD_DATA_FRAG_LIGHT_LUTINPUT(D0, D1, SP, FR, RB, RG, RR) \
    ( (D0) | (D1) << 4 | (SP) << 8 | \
      (FR) << 12 | (RB) << 16 | (RG) << 20 | (RR) << 24 )

#define PICA_CMD_DATA_FRAG_LIGHT_LUTSCALE(D0, D1, SP, FR, RB, RG, RR) \
    ( (D0) | (D1) << 4 | (SP) << 8 | \
      (FR) << 12 | (RB) << 16 | (RG) << 20 | (RR) << 24 )

#endif // NN_GX_CTR_PICA_MACRO_OLD_H
