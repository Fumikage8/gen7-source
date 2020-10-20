/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gx_MacroReg.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GX_CTR_GX_MACRO_REG_H_
#define NN_GX_CTR_GX_MACRO_REG_H_

/*!
  @addtogroup   nn_gx_CTR_Pica   PicaMacroHeader
  @{
*/

/*!
    @brief PICA レジスタの列挙体です。
*/
enum PicaReg
{
    //! 0x010 です。
    PICA_REG_INTERRUPT                 = 0x010, 

    //! 0x040 です。
    PICA_REG_CULL_FACE                 = 0x040,

    //! 0x041 です。
    PICA_REG_VIEWPORT_WIDTH1           = 0x041,
    //! 0x042 です。
    PICA_REG_VIEWPORT_WIDTH2           = 0x042,
    //! 0x043 です。
    PICA_REG_VIEWPORT_HEIGHT1          = 0x043,
    //! 0x044 です。
    PICA_REG_VIEWPORT_HEIGHT2          = 0x044,

    //! 0x047 です。
    PICA_REG_FRAGOP_CLIP               = 0x047,
    //! 0x048 です。
    PICA_REG_FRAGOP_CLIP_DATA1         = 0x048,
    //! 0x049 です。
    PICA_REG_FRAGOP_CLIP_DATA2         = 0x049,
    //! 0x04a です。
    PICA_REG_FRAGOP_CLIP_DATA3         = 0x04a,
    //! 0x04b です。
    PICA_REG_FRAGOP_CLIP_DATA4         = 0x04b,

    //! 0x04d です。
    PICA_REG_FRAGOP_WSCALE_DATA1       = 0x04d,
    //! 0x04e です。
    PICA_REG_FRAGOP_WSCALE_DATA2       = 0x04e,

    //! 0x04f です。
    PICA_REG_VS_OUT_REG_NUM0           = 0x04f,
    //! 0x04f です。
    PICA_REG_GS_OUT_REG_NUM0           = 0x04f,
    
    //! 0x050 です。
    PICA_REG_VS_OUT_ATTR0              = 0x050,
    //! 0x050 です。
    PICA_REG_GS_OUT_ATTR0              = 0x050,
    //! 0x051 です。
    PICA_REG_VS_OUT_ATTR1              = 0x051,
    //! 0x051 です。
    PICA_REG_GS_OUT_ATTR1              = 0x051,
    //! 0x052 です。
    PICA_REG_VS_OUT_ATTR2              = 0x052,
    //! 0x052 です。
    PICA_REG_GS_OUT_ATTR2              = 0x052,
    //! 0x053 です。
    PICA_REG_VS_OUT_ATTR3              = 0x053,
    //! 0x053 です。
    PICA_REG_GS_OUT_ATTR3              = 0x053,
    //! 0x054 です。
    PICA_REG_VS_OUT_ATTR4              = 0x054,
    //! 0x054 です。
    PICA_REG_GS_OUT_ATTR4              = 0x054,
    //! 0x055 です。
    PICA_REG_VS_OUT_ATTR5              = 0x055,
    //! 0x055 です。
    PICA_REG_GS_OUT_ATTR5              = 0x055,
    //! 0x056 です。
    PICA_REG_VS_OUT_ATTR6              = 0x056,
    //! 0x056 です。
    PICA_REG_GS_OUT_ATTR6              = 0x056,

    //! 0x061 です。
    PICA_REG_EARLY_DEPTH_FUNC          = 0x061,
    //! 0x062 です。
    PICA_REG_EARLY_DEPTH_TEST1         = 0x062,
    //! 0x063 です。
    PICA_REG_EARLY_DEPTH_CLEAR         = 0x063,
    //! 0x064 です。
    PICA_REG_VS_OUT_ATTR_MODE          = 0x064,
    //! 0x064 です。
    PICA_REG_GS_OUT_ATTR_MODE          = 0x064,
 
    //! 0x065 です。
    PICA_REG_SCISSOR                   = 0x065,
    //! 0x066 です。
    PICA_REG_SCISSOR_XY                = 0x066,
    //! 0x067 です。
    PICA_REG_SCISSOR_SIZE              = 0x067,

    //! 0x068 です。
    PICA_REG_VIEWPORT_XY               = 0x068,

    //! 0x06a です。
    PICA_REG_EARLY_DEPTH_DATA          = 0x06a,

    //! 0x06d です。
    PICA_REG_FRAGOP_WSCALE             = 0x06d,

    //! 0x06e です。
    PICA_REG_RENDER_BUFFER_RESOLUTION1 = 0x06e,

    //! 0x06f です。
    PICA_REG_VS_OUT_ATTR_CLK           = 0x06f,
    //! 0x06f です。
    PICA_REG_GS_OUT_ATTR_CLK           = 0x06f,



    //! 0x080 です。
    PICA_REG_TEXTURE_FUNC              = 0x080,

    //! 0x081 です。
    PICA_REG_TEXTURE0_BORDER_COLOR     = 0x081,
    //! 0x082 です。
    PICA_REG_TEXTURE0_SIZE             = 0x082,
    //! 0x083 です。
    PICA_REG_TEXTURE0_WRAP_FILTER      = 0x083,
    //! 0x084 です。    
    PICA_REG_TEXTURE0_LOD              = 0x084,
    //! 0x085 です。
    PICA_REG_TEXTURE0_ADDR1            = 0x085,
    //! 0x086 です。
    PICA_REG_TEXTURE0_ADDR2            = 0x086,
    //! 0x087 です。
    PICA_REG_TEXTURE0_ADDR3            = 0x087,
    //! 0x088 です。    
    PICA_REG_TEXTURE0_ADDR4            = 0x088,
    //! 0x089 です。
    PICA_REG_TEXTURE0_ADDR5            = 0x089,
    //! 0x08a です。
    PICA_REG_TEXTURE0_ADDR6            = 0x08a,
    //! 0x08b です。
    PICA_REG_TEXTURE0_SHADOW           = 0x08b,

    //! 0x08e です。
    PICA_REG_TEXTURE0_FORMAT           = 0x08e,
    //! 0x08f です。
    PICA_REG_FRAG_LIGHT_EN0            = 0x08f,

    //! 0x091 です。
    PICA_REG_TEXTURE1_BORDER_COLOR     = 0x091,
    //! 0x092 です。
    PICA_REG_TEXTURE1_SIZE             = 0x092,
    //! 0x093 です。
    PICA_REG_TEXTURE1_WRAP_FILTER      = 0x093,
    //! 0x094 です。
    PICA_REG_TEXTURE1_LOD              = 0x094,
    //! 0x095 です。
    PICA_REG_TEXTURE1_ADDR             = 0x095,
    //! 0x096 です。
    PICA_REG_TEXTURE1_FORMAT           = 0x096,

    //! 0x099 です。
    PICA_REG_TEXTURE2_BORDER_COLOR     = 0x099,
    //! 0x09a です。
    PICA_REG_TEXTURE2_SIZE             = 0x09a,
    //! 0x09b です。
    PICA_REG_TEXTURE2_WRAP_FILTER      = 0x09b,
    //! 0x09c です。
    PICA_REG_TEXTURE2_LOD              = 0x09c,
    //! 0x09d です。
    PICA_REG_TEXTURE2_ADDR             = 0x09d,
    //! 0x09e です。
    PICA_REG_TEXTURE2_FORMAT           = 0x09e,

    //! 0x0a8 です。
    PICA_REG_TEXTURE3_PROCTEX0         = 0x0a8,
    //! 0x0a9 です。
    PICA_REG_TEXTURE3_PROCTEX1         = 0x0a9,
    //! 0x0aa です。
    PICA_REG_TEXTURE3_PROCTEX2         = 0x0aa,
    //! 0x0ab です。
    PICA_REG_TEXTURE3_PROCTEX3         = 0x0ab,
    //! 0x0ac です。
    PICA_REG_TEXTURE3_PROCTEX4         = 0x0ac,
    //! 0x0ad です。
    PICA_REG_TEXTURE3_PROCTEX5         = 0x0ad,

    //! 0x0af です。
    PICA_REG_PROCTEX_LUT               = 0x0af,
    //! 0x0b0 です。
    PICA_REG_PROCTEX_LUT_DATA0         = 0x0b0,
    //! 0x0b1 です。
    PICA_REG_PROCTEX_LUT_DATA1         = 0x0b1,
    //! 0x0b2 です。
    PICA_REG_PROCTEX_LUT_DATA2         = 0x0b2,
    //! 0x0b3 です。
    PICA_REG_PROCTEX_LUT_DATA3         = 0x0b3,
    //! 0x0b4 です。
    PICA_REG_PROCTEX_LUT_DATA4         = 0x0b4,
    //! 0x0b5 です。
    PICA_REG_PROCTEX_LUT_DATA5         = 0x0b5,
    //! 0x0b6 です。
    PICA_REG_PROCTEX_LUT_DATA6         = 0x0b6,
    //! 0x0b7 です。
    PICA_REG_PROCTEX_LUT_DATA7         = 0x0b7,

    //! 0x0c0 です。
    PICA_REG_TEX_ENV0                  = 0x0c0,
    //! 0x0c1 です。
    PICA_REG_TEX_ENV0_OPERAND          = 0x0c1,
    //! 0x0c2 です。
    PICA_REG_TEX_ENV0_COMBINE          = 0x0c2,
    //! 0x0c3 です。
    PICA_REG_TEX_ENV0_COLOR            = 0x0c3,
    //! 0x0c4 です。
    PICA_REG_TEX_ENV0_SCALE            = 0x0c4,

    //! 0x0c8 です。
    PICA_REG_TEX_ENV1                  = 0x0c8,
    //! 0x0c9 です。
    PICA_REG_TEX_ENV1_OPERAND          = 0x0c9,
    //! 0x0ca です。
    PICA_REG_TEX_ENV1_COMBINE          = 0x0ca,
    //! 0x0cb です。
    PICA_REG_TEX_ENV1_COLOR            = 0x0cb,
    //! 0x0cc です。
    PICA_REG_TEX_ENV1_SCALE            = 0x0cc,

    //! 0x0d0 です。
    PICA_REG_TEX_ENV2 = 0x0d0,
    //! 0x0d1 です。
    PICA_REG_TEX_ENV2_OPERAND          = 0x0d1,
    //! 0x0d2 です。
    PICA_REG_TEX_ENV2_COMBINE          = 0x0d2,
    //! 0x0d3 です。
    PICA_REG_TEX_ENV2_COLOR            = 0x0d3,
    //! 0x0d4 です。
    PICA_REG_TEX_ENV2_SCALE            = 0x0d4,

    //! 0x0d8 です。
    PICA_REG_TEX_ENV3                  = 0x0d8,
    //! 0x0d9 です。
    PICA_REG_TEX_ENV3_OPERAND          = 0x0d9,
    //! 0x0da です。
    PICA_REG_TEX_ENV3_COMBINE          = 0x0da,
    //! 0x0db です。
    PICA_REG_TEX_ENV3_COLOR            = 0x0db,
    //! 0x0dc です。
    PICA_REG_TEX_ENV3_SCALE            = 0x0dc,

    //! 0x0e0 です。
    PICA_REG_GAS_FOG_MODE              = 0x0e0,
    //! 0x0e0 です。
    PICA_REG_TEX_ENV_BUFFER_INPUT      = 0x0e0,

    //! 0x0e1 です。
    PICA_REG_FOG_COLOR                 = 0x0e1,
    //! 0x0e4 です。
    PICA_REG_GAS_ATTENUATION           = 0x0e4,
    //! 0x0e5 です。
    PICA_REG_GAS_ACCMAX                = 0x0e5,

    //! 0x0e6 です。
    PICA_REG_FOG_LUT_INDEX             = 0x0e6,
    //! 0x0e8 です。
    PICA_REG_FOG_LUT_DATA0             = 0x0e8,
    //! 0x0e9 です。
    PICA_REG_FOG_LUT_DATA1             = 0x0e9,
    //! 0x0ea です。
    PICA_REG_FOG_LUT_DATA2             = 0x0ea,
    //! 0x0eb です。
    PICA_REG_FOG_LUT_DATA3             = 0x0eb,
    //! 0x0ec です。    
    PICA_REG_FOG_LUT_DATA4             = 0x0ec,
    //! 0x0ed です。
    PICA_REG_FOG_LUT_DATA5             = 0x0ed,
    //! 0x0ee です。
    PICA_REG_FOG_LUT_DATA6             = 0x0ee,
    //! 0x0ef です。
    PICA_REG_FOG_LUT_DATA7             = 0x0ef,

    //! 0x0f0 です。
    PICA_REG_TEX_ENV4                  = 0x0f0,
    //! 0x0f1 です。
    PICA_REG_TEX_ENV4_OPERAND          = 0x0f1,
    //! 0x0f2 です。
    PICA_REG_TEX_ENV4_COMBINE          = 0x0f2,
    //! 0x0f3 です。
    PICA_REG_TEX_ENV4_COLOR            = 0x0f3,
    //! 0x0f4 です。
    PICA_REG_TEX_ENV4_SCALE            = 0x0f4,

    //! 0x0f8 です。
    PICA_REG_TEX_ENV5                  = 0x0f8,
    //! 0x0f9 です。
    PICA_REG_TEX_ENV5_OPERAND          = 0x0f9,
    //! 0x0fa です。
    PICA_REG_TEX_ENV5_COMBINE          = 0x0fa,
    //! 0x0fb です。
    PICA_REG_TEX_ENV5_COLOR            = 0x0fb,
    //! 0x0fc です。
    PICA_REG_TEX_ENV5_SCALE            = 0x0fc,
    //! 0x0fd です。
    PICA_REG_TEX_ENV_BUFFER_COLOR      = 0x0fd,


    //! 0x100 です。
    PICA_REG_COLOR_OPERATION            = 0x100,
    //! 0x101 です。
    PICA_REG_BLEND_FUNC                 = 0x101,
    //! 0x102 です。
    PICA_REG_LOGIC_OP                   = 0x102,
    //! 0x103 です。
    PICA_REG_BLEND_COLOR                = 0x103,
    //! 0x104 です。
    PICA_REG_FRAGOP_ALPHA_TEST          = 0x104,

    //! 0x105 です。
    PICA_REG_STENCIL_TEST               = 0x105,
    //! 0x106 です。
    PICA_REG_STENCIL_OP                 = 0x106,
    //! 0x107 です。
    PICA_REG_DEPTH_COLOR_MASK           = 0x107,

    //! 0x110 です。
    PICA_REG_COLOR_DEPTH_BUFFER_CLEAR0  = 0x110,
    //! 0x111 です。
    PICA_REG_COLOR_DEPTH_BUFFER_CLEAR1  = 0x111,
    //! 0x112 です。
    PICA_REG_COLOR_BUFFER_READ          = 0x112,
    //! 0x113 です。
    PICA_REG_COLOR_BUFFER_WRITE         = 0x113,
    //! 0x114 です。
    PICA_REG_DEPTH_STENCIL_BUFFER_READ  = 0x114,
    //! 0x115 です。
    PICA_REG_DEPTH_STENCIL_BUFFER_WRITE = 0x115,

    //! 0x116 です。
    PICA_REG_RENDER_BUFFER_DEPTH_MODE   = 0x116,
    //! 0x117 です。
    PICA_REG_RENDER_BUFFER_COLOR_MODE   = 0x117,

    //! 0x118 です。
    PICA_REG_EARLY_DEPTH_TEST2          = 0x118,

    //! 0x11b です。
    PICA_REG_RENDER_BLOCK_FORMAT        = 0x11b,

    //! 0x11c です。
    PICA_REG_RENDER_BUFFER_DEPTH_ADDR   = 0x11c,
    //! 0x11d です。
    PICA_REG_RENDER_BUFFER_COLOR_ADDR   = 0x11d,
    //! 0x11e です。
    PICA_REG_RENDER_BUFFER_RESOLUTION0  = 0x11e,
    
    //! 0x120 です。
    PICA_REG_GAS_LIGHT_XY               = 0x120,
    //! 0x121 です。
    PICA_REG_GAS_LIGHT_Z                = 0x121,
    //! 0x122 です。
    PICA_REG_GAS_LIGHT_Z_COLOR          = 0x122,
    //! 0x123 です。
    PICA_REG_GAS_LUT_INDEX              = 0x123,
    //! 0x124 です。
    PICA_REG_GAS_LUT_DATA               = 0x124,
    //! 0x126 です。    
    PICA_REG_GAS_DELTAZ_DEPTH           = 0x126,

    //! 0x130 です。
    PICA_REG_FRAGOP_SHADOW              = 0x130,


    //! 1 個のフラグメントライトのレジスタのストライドです。
    PICA_FRAG_LIGHT_STRIDE               = 16,
    //! 0x140 です。
    PICA_REG_FRAG_LIGHT_START            = 0x140,
    //! 0x140 です。
    PICA_REG_FRAG_LIGHT0_SPECULAR0       = PICA_REG_FRAG_LIGHT_START,
    //! 0x141 です。
    PICA_REG_FRAG_LIGHT0_SPECULAR1       = 0x141, 
    //! 0x142 です。
    PICA_REG_FRAG_LIGHT0_DIFFUSE         = 0x142, 
    //! 0x143 です。
    PICA_REG_FRAG_LIGHT0_AMBIENT         = 0x143, 
    //! 0x144 です。
    PICA_REG_FRAG_LIGHT0_POSITION_XY     = 0x144, 
    //! 0x145 です。
    PICA_REG_FRAG_LIGHT0_POSITION_Z      = 0x145, 
    //! 0x146 です。
    PICA_REG_FRAG_LIGHT0_SPOT_XY         = 0x146, 
    //! 0x147 です。
    PICA_REG_FRAG_LIGHT0_SPOT_Z          = 0x147, 
    //! 0x149 です。
    PICA_REG_FRAG_LIGHT0_TYPE            = 0x149, 
    //! 0x14a です。
    PICA_REG_FRAG_LIGHT0_DIST_ATTN_BIAS  = 0x14a, 
    //! 0x14b です。
    PICA_REG_FRAG_LIGHT0_DIST_ATTN_SCALE = 0x14b, 

    //! 0x150 です。
    PICA_REG_FRAG_LIGHT1_SPECULAR0       = 0x150, 
    //! 0x151 です。
    PICA_REG_FRAG_LIGHT1_SPECULAR1       = 0x151, 
    //! 0x152 です。
    PICA_REG_FRAG_LIGHT1_DIFFUSE         = 0x152, 
    //! 0x153 です。
    PICA_REG_FRAG_LIGHT1_AMBIENT         = 0x153, 
    //! 0x154 です。
    PICA_REG_FRAG_LIGHT1_POSITION_XY     = 0x154, 
    //! 0x155 です。
    PICA_REG_FRAG_LIGHT1_POSITION_Z      = 0x155, 
    //! 0x156 です。    
    PICA_REG_FRAG_LIGHT1_SPOT_XY         = 0x156, 
    //! 0x157 です。
    PICA_REG_FRAG_LIGHT1_SPOT_Z          = 0x157, 
    //! 0x159 です。
    PICA_REG_FRAG_LIGHT1_TYPE            = 0x159, 
    //! 0x15a です。
    PICA_REG_FRAG_LIGHT1_DIST_ATTN_BIAS  = 0x15a, 
    //! 0x15b です。
    PICA_REG_FRAG_LIGHT1_DIST_ATTN_SCALE = 0x15b, 

    //! 0x160 です。
    PICA_REG_FRAG_LIGHT2_SPECULAR0       = 0x160, 
    //! 0x161 です。    
    PICA_REG_FRAG_LIGHT2_SPECULAR1       = 0x161, 
    //! 0x162 です。
    PICA_REG_FRAG_LIGHT2_DIFFUSE         = 0x162, 
    //! 0x163 です。
    PICA_REG_FRAG_LIGHT2_AMBIENT         = 0x163, 
    //! 0x164 です。
    PICA_REG_FRAG_LIGHT2_POSITION_XY     = 0x164, 
    //! 0x165 です。
    PICA_REG_FRAG_LIGHT2_POSITION_Z      = 0x165, 
    //! 0x166 です。
    PICA_REG_FRAG_LIGHT2_SPOT_XY         = 0x166, 
    //! 0x167 です。
    PICA_REG_FRAG_LIGHT2_SPOT_Z          = 0x167,
    //! 0x168 です。
    PICA_REG_FRAG_LIGHT2_TYPE            = 0x169, 
    //! 0x16a です。
    PICA_REG_FRAG_LIGHT2_DIST_ATTN_BIAS  = 0x16a, 
    //! 0x16b です。
    PICA_REG_FRAG_LIGHT2_DIST_ATTN_SCALE = 0x16b, 

    //! 0x170 です。
    PICA_REG_FRAG_LIGHT3_SPECULAR0       = 0x170, 
    //! 0x171 です。
    PICA_REG_FRAG_LIGHT3_SPECULAR1       = 0x171, 
    //! 0x172 です。
    PICA_REG_FRAG_LIGHT3_DIFFUSE         = 0x172, 
    //! 0x173 です。
    PICA_REG_FRAG_LIGHT3_AMBIENT         = 0x173, 
    //! 0x174 です。
    PICA_REG_FRAG_LIGHT3_POSITION_XY     = 0x174, 
    //! 0x175 です。
    PICA_REG_FRAG_LIGHT3_POSITION_Z      = 0x175, 
    //! 0x176 です。
    PICA_REG_FRAG_LIGHT3_SPOT_XY         = 0x176, 
    //! 0x177 です。
    PICA_REG_FRAG_LIGHT3_SPOT_Z          = 0x177, 
    //! 0x179 です。
    PICA_REG_FRAG_LIGHT3_TYPE            = 0x179, 
    //! 0x17a です。
    PICA_REG_FRAG_LIGHT3_DIST_ATTN_BIAS  = 0x17a, 
    //! 0x17b です。
    PICA_REG_FRAG_LIGHT3_DIST_ATTN_SCALE = 0x17b, 

    //! 0x180 です。
    PICA_REG_FRAG_LIGHT4_SPECULAR0       = 0x180, 
    //! 0x181 です。
    PICA_REG_FRAG_LIGHT4_SPECULAR1       = 0x181, 
    //! 0x182 です。
    PICA_REG_FRAG_LIGHT4_DIFFUSE         = 0x182, 
    //! 0x183 です。
    PICA_REG_FRAG_LIGHT4_AMBIENT         = 0x183, 
    //! 0x184 です。
    PICA_REG_FRAG_LIGHT4_POSITION_XY     = 0x184, 
    //! 0x185 です。
    PICA_REG_FRAG_LIGHT4_POSITION_Z      = 0x185, 
    //! 0x186 です。
    PICA_REG_FRAG_LIGHT4_SPOT_XY         = 0x186, 
    //! 0x187 です。
    PICA_REG_FRAG_LIGHT4_SPOT_Z          = 0x187, 
    //! 0x189 です。
    PICA_REG_FRAG_LIGHT4_TYPE            = 0x189, 
    //! 0x18a です。
    PICA_REG_FRAG_LIGHT4_DIST_ATTN_BIAS  = 0x18a, 
    //! 0x18b です。
    PICA_REG_FRAG_LIGHT4_DIST_ATTN_SCALE = 0x18b, 

    //! 0x190 です。
    PICA_REG_FRAG_LIGHT5_SPECULAR0       = 0x190, 
    //! 0x191 です。
    PICA_REG_FRAG_LIGHT5_SPECULAR1       = 0x191, 
    //! 0x192 です。
    PICA_REG_FRAG_LIGHT5_DIFFUSE         = 0x192, 
    //! 0x193 です。
    PICA_REG_FRAG_LIGHT5_AMBIENT         = 0x193, 
    //! 0x194 です。
    PICA_REG_FRAG_LIGHT5_POSITION_XY     = 0x194, 
    //! 0x195 です。
    PICA_REG_FRAG_LIGHT5_POSITION_Z      = 0x195, 
    //! 0x196 です。
    PICA_REG_FRAG_LIGHT5_SPOT_XY         = 0x196, 
    //! 0x197 です。
    PICA_REG_FRAG_LIGHT5_SPOT_Z          = 0x197, 
    //! 0x199 です。
    PICA_REG_FRAG_LIGHT5_TYPE            = 0x199, 
    //! 0x19a です。
    PICA_REG_FRAG_LIGHT5_DIST_ATTN_BIAS  = 0x19a, 
    //! 0x19b です。
    PICA_REG_FRAG_LIGHT5_DIST_ATTN_SCALE = 0x19b, 

    //! 0x1a0 です。
    PICA_REG_FRAG_LIGHT6_SPECULAR0       = 0x1a0, 
    //! 0x1a1 です。
    PICA_REG_FRAG_LIGHT6_SPECULAR1       = 0x1a1, 
    //! 0x1a2 です。
    PICA_REG_FRAG_LIGHT6_DIFFUSE         = 0x1a2, 
    //! 0x1a3 です。
    PICA_REG_FRAG_LIGHT6_AMBIENT         = 0x1a3, 
    //! 0x1a4 です。
    PICA_REG_FRAG_LIGHT6_POSITION_XY     = 0x1a4, 
    //! 0x1a5 です。
    PICA_REG_FRAG_LIGHT6_POSITION_Z      = 0x1a5, 
    //! 0x1a6 です。
    PICA_REG_FRAG_LIGHT6_SPOT_XY         = 0x1a6, 
    //! 0x1a7 です。
    PICA_REG_FRAG_LIGHT6_SPOT_Z          = 0x1a7, 
    //! 0x1a9 です。
    PICA_REG_FRAG_LIGHT6_TYPE            = 0x1a9, 
    //! 0x1aa です。
    PICA_REG_FRAG_LIGHT6_DIST_ATTN_BIAS  = 0x1aa, 
    //! 0x1ab です。
    PICA_REG_FRAG_LIGHT6_DIST_ATTN_SCALE = 0x1ab, 

    //! 0x1b0 です。
    PICA_REG_FRAG_LIGHT7_SPECULAR0       = 0x1b0, 
    //! 0x1b1 です。
    PICA_REG_FRAG_LIGHT7_SPECULAR1       = 0x1b1, 
    //! 0x1b2 です。
    PICA_REG_FRAG_LIGHT7_DIFFUSE         = 0x1b2, 
    //! 0x1b3 です。
    PICA_REG_FRAG_LIGHT7_AMBIENT         = 0x1b3, 
    //! 0x1b4 です。
    PICA_REG_FRAG_LIGHT7_POSITION_XY     = 0x1b4, 
    //! 0x1b5 です。
    PICA_REG_FRAG_LIGHT7_POSITION_Z      = 0x1b5, 
    //! 0x1b6 です。
    PICA_REG_FRAG_LIGHT7_SPOT_XY         = 0x1b6, 
    //! 0x1b7 です。
    PICA_REG_FRAG_LIGHT7_SPOT_Z          = 0x1b7, 
    //! 0x1b9 です。
    PICA_REG_FRAG_LIGHT7_TYPE            = 0x1b9, 
    //! 0x1ba です。
    PICA_REG_FRAG_LIGHT7_DIST_ATTN_BIAS  = 0x1ba, 
    //! 0x1bb です。
    PICA_REG_FRAG_LIGHT7_DIST_ATTN_SCALE = 0x1bb,
 
    //! 0x1c0 です。
    PICA_REG_FRAG_LIGHT_AMBIENT          = 0x1c0,
    //! 0x1c2 です。
    PICA_REG_FRAG_LIGHT_SRC_NUM          = 0x1c2,
    //! 0x1c3 です。
    PICA_REG_FRAG_LIGHT_FUNC_MODE0       = 0x1c3,
    //! 0x1c4 です。
    PICA_REG_FRAG_LIGHT_FUNC_MODE1       = 0x1c4,
    //! 0x1c5 です。
    PICA_REG_FRAG_LIGHT_LUT              = 0x1c5,
    //! 0x1c6 です。
    PICA_REG_FRAG_LIGHT_EN1              = 0x1c6,
    //! 0x1c8 です。    
    PICA_REG_FRAG_LIGHT_LUT_DATA0        = 0x1c8,
    //! 0x1c9 です。
    PICA_REG_FRAG_LIGHT_LUT_DATA1        = 0x1c9,
    //! 0x1ca です。
    PICA_REG_FRAG_LIGHT_LUT_DATA2        = 0x1ca,
    //! 0x1cb です。
    PICA_REG_FRAG_LIGHT_LUT_DATA3        = 0x1cb,
    //! 0x1cc です。
    PICA_REG_FRAG_LIGHT_LUT_DATA4        = 0x1cc,
    //! 0x1cd です。
    PICA_REG_FRAG_LIGHT_LUT_DATA5        = 0x1cd,
    //! 0x1ce です。
    PICA_REG_FRAG_LIGHT_LUT_DATA6        = 0x1ce,
    //! 0x1cf です。
    PICA_REG_FRAG_LIGHT_LUT_DATA7        = 0x1cf,
    //! 0x1d0 です。
    PICA_REG_FRAG_LIGHT_ABSLUTINPUT      = 0x1d0,
    //! 0x1d1 です。
    PICA_REG_FRAG_LIGHT_LUTINPUT         = 0x1d1,
    //! 0x1d2 です。
    PICA_REG_FRAG_LIGHT_LUTSCALE         = 0x1d2,
    //! 0x1d9 です。
    PICA_REG_FRAG_LIGHT_SRC_EN_ID        = 0x1d9,

    //! 0x200 です。
    PICA_REG_VERTEX_ATTR_ARRAYS_BASE_ADDR = 0x200,
    //! 0x201 です。
    PICA_REG_VERTEX_ATTR_ARRAYS0          = 0x201,
    //! 0x202 です。
    PICA_REG_VERTEX_ATTR_ARRAYS1          = 0x202,
    
    //! ロードアレイのストライドです。
    PICA_LOAD_ARRAY_STRIDE                = 3,
    //! 0x203 です。
    PICA_REG_LOAD_ARRAY0_ATTR_OFFSET      = 0x203,
    //! 0x204 です。
    PICA_REG_LOAD_ARRAY0_ELEMENT0         = 0x204,
    //! 0x205 です。
    PICA_REG_LOAD_ARRAY0_ELEMENT1         = 0x205,
    
    //! 0x227 です。
    PICA_REG_INDEX_ARRAY_ADDR_OFFSET      = 0x227,
    //! 0x228 です。
    PICA_REG_DRAW_VERTEX_NUM              = 0x228,
    //! 0x229 です。
    PICA_REG_DRAW_MODE0                   = 0x229,

    //! 0x22a です。    
    PICA_REG_DRAW_VERTEX_OFFSET           = 0x22a,
    //! 0x22d です。
    PICA_REG_POST_VERTEX_CACHE_NUM        = 0x22d,
    //! 0x22e です。
    PICA_REG_START_DRAW_ARRAY             = 0x22e,
    //! 0x22f です。
    PICA_REG_START_DRAW_ELEMENT           = 0x22f,

    //! 0x231 です。
    PICA_REG_VERTEX_FUNC                  = 0x231,

    //! 0x232 です。
    PICA_REG_VS_FIXED_ATTR                = 0x232,
    //! 0x233 です。
    PICA_REG_VS_FIXED_ATTR_DATA0          = 0x233,
    //! 0x234 です。
    PICA_REG_VS_FIXED_ATTR_DATA1          = 0x234,
    //! 0x235 です。
    PICA_REG_VS_FIXED_ATTR_DATA2          = 0x235,
    
    //! 0x238 です。
    PICA_REG_COMMAND_BUF_SIZE_CH0         = 0x238,
    //! 0x239 です。
    PICA_REG_COMMAND_BUF_SIZE_CH1         = 0x239,
    //! 0x23a です。
    PICA_REG_COMMAND_BUF_ADDR_CH0         = 0x23a,
    //! 0x23b です。
    PICA_REG_COMMAND_BUF_ADDR_CH1         = 0x23b,
    //! 0x23c です。
    PICA_REG_COMMAND_BUF_KICK_CH0         = 0x23c,
    //! 0x23d です。
    PICA_REG_COMMAND_BUF_KICK_CH1         = 0x23d,
    
    //! 0x242 です。
    PICA_REG_VS_ATTR_NUM1                 = 0x242,
    //! 0x244 です。
    PICA_REG_VS_COM_MODE                  = 0x244,
    
    //! 0x245 です。
    PICA_REG_START_DRAW_FUNC0             = 0x245,

    //! 0x24a です。
    PICA_REG_VS_OUT_REG_NUM1              = 0x24a,
    //! 0x251 です。
    PICA_REG_VS_OUT_REG_NUM2              = 0x251,
    //! 0x252 です。
    PICA_REG_GS_MISC_REG0                 = 0x252,
    //! 0x253 です。
    PICA_REG_DRAW_MODE1                   = 0x253,
    //! 0x254 です。
    PICA_REG_GS_MISC_REG1                 = 0x254,
    //! 0x25e です。
    PICA_REG_VS_OUT_REG_NUM3              = 0x25e,
    //! 0x25e です。
    PICA_REG_DRAW_MODE2                   = 0x25e,
    //! 0x25e です。
    PICA_REG_GS_OUT_REG_NUM3              = 0x25e,
    
    //! 0x25f です。
    PICA_REG_START_DRAW_FUNC1             = 0x25f,
             
    //! 0x280 です。
    PICA_REG_GS_BOOL                      = 0x280,
    //! 0x281 です。
    PICA_REG_GS_INT0                      = 0x281,
    //! 0x282 です。
    PICA_REG_GS_INT1                      = 0x282,
    //! 0x283 です。
    PICA_REG_GS_INT2                      = 0x283,
    //! 0x284 です。
    PICA_REG_GS_INT3                      = 0x284,
    //! 0x289 です。
    PICA_REG_GS_ATTR_NUM                  = 0x289,
    //! 0x28a です。
    PICA_REG_GS_START_ADDR                = 0x28a,
    //! 0x28b です。
    PICA_REG_GS_ATTR_IN_REG_MAP0          = 0x28b,
    //! 0x28c です。
    PICA_REG_GS_ATTR_IN_REG_MAP1          = 0x28c,
    //! 0x28d です。
    PICA_REG_GS_OUT_REG_MASK              = 0x28d,
    //! 0x28f です。
    PICA_REG_GS_PROG_UPDATE_END           = 0x28f,
    //! 0x290 です。
    PICA_REG_GS_FLOAT_ADDR                = 0x290,
    //! 0x291 です。
    PICA_REG_GS_FLOAT0                    = 0x291,
    //! 0x292 です。
    PICA_REG_GS_FLOAT1                    = 0x292,
    //! 0x293 です。
    PICA_REG_GS_FLOAT2                    = 0x293,
    //! 0x294 です。
    PICA_REG_GS_FLOAT3                    = 0x294,
    //! 0x295 です。
    PICA_REG_GS_FLOAT4                    = 0x295,
    //! 0x296 です。
    PICA_REG_GS_FLOAT5                    = 0x296,
    //! 0x297 です。
    PICA_REG_GS_FLOAT6                    = 0x297,
    //! 0x298 です。
    PICA_REG_GS_FLOAT7                    = 0x298,
    //! 0x29b です。
    PICA_REG_GS_PROG_ADDR                 = 0x29b,
    //! 0x29c です。
    PICA_REG_GS_PROG_DATA0                = 0x29c,
    //! 0x29d です。
    PICA_REG_GS_PROG_DATA1                = 0x29d,
    //! 0x29e です。
    PICA_REG_GS_PROG_DATA2                = 0x29e,
    //! 0x29f です。
    PICA_REG_GS_PROG_DATA3                = 0x29f,
    //! 0x2a0 です。
    PICA_REG_GS_PROG_DATA4                = 0x2a0,
    //! 0x2a1 です。
    PICA_REG_GS_PROG_DATA5                = 0x2a1,
    //! 0x2a2 です。
    PICA_REG_GS_PROG_DATA6                = 0x2a2,
    //! 0x2a3 です。
    PICA_REG_GS_PROG_DATA7                = 0x2a3,
    //! 0x2a5 です。
    PICA_REG_GS_PROG_SWIZZLE_ADDR         = 0x2a5,
    //! 0x2a6 です。    
    PICA_REG_GS_PROG_SWIZZLE_DATA0        = 0x2a6,
    //! 0x2a7 です。
    PICA_REG_GS_PROG_SWIZZLE_DATA1        = 0x2a7,
    //! 0x2a8 です。    
    PICA_REG_GS_PROG_SWIZZLE_DATA2        = 0x2a8,
    //! 0x2a9 です。
    PICA_REG_GS_PROG_SWIZZLE_DATA3        = 0x2a9,
    //! 0x2aa です。
    PICA_REG_GS_PROG_SWIZZLE_DATA4        = 0x2aa,
    //! 0x2ab です。
    PICA_REG_GS_PROG_SWIZZLE_DATA5        = 0x2ab,
    //! 0x2ac です。
    PICA_REG_GS_PROG_SWIZZLE_DATA6        = 0x2ac,
    //! 0x2ad です。
    PICA_REG_GS_PROG_SWIZZLE_DATA7        = 0x2ad,

    //! 0x2b0 です。
    PICA_REG_VS_BOOL                      = 0x2b0,
    //! 0x2b1 です。
    PICA_REG_VS_INT0                      = 0x2b1,
    //! 0x2b2 です。
    PICA_REG_VS_INT1                      = 0x2b2,
    //! 0x2b3 です。
    PICA_REG_VS_INT2                      = 0x2b3,
    //! 0x2b4 です。
    PICA_REG_VS_INT3                      = 0x2b4,
    //! 0x2b9 です。
    PICA_REG_VS_ATTR_NUM0                 = 0x2b9,
    //! 0x2ba です。
    PICA_REG_VS_START_ADDR                = 0x2ba,
    //! 0x2bb です。
    PICA_REG_VS_ATTR_IN_REG_MAP0          = 0x2bb,
    //! 0x2bc です。
    PICA_REG_VS_ATTR_IN_REG_MAP1          = 0x2bc,
    //! 0x2bd です。
    PICA_REG_VS_OUT_REG_MASK              = 0x2bd,
    //! 0x2bf です。
    PICA_REG_VS_PROG_UPDATE_END           = 0x2bf,
    //! 0x2c0 です。
    PICA_REG_VS_FLOAT_ADDR                = 0x2c0,
    //! 0x2c1 です。
    PICA_REG_VS_FLOAT0                    = 0x2c1,
    //! 0x2c2 です。
    PICA_REG_VS_FLOAT1                    = 0x2c2,
    //! 0x2c3 です。
    PICA_REG_VS_FLOAT2                    = 0x2c3,
    //! 0x2c4 です。
    PICA_REG_VS_FLOAT3                    = 0x2c4,
    //! 0x2c5 です。
    PICA_REG_VS_FLOAT4                    = 0x2c5,
    //! 0x2c6 です。
    PICA_REG_VS_FLOAT5                    = 0x2c6,
    //! 0x2c7 です。
    PICA_REG_VS_FLOAT6                    = 0x2c7,
    //! 0x2c8 です。
    PICA_REG_VS_FLOAT7                    = 0x2c8,
    //! 0x2cb です。
    PICA_REG_VS_PROG_ADDR                 = 0x2cb,
    //! 0x2cc です。
    PICA_REG_VS_PROG_DATA0                = 0x2cc,
    //! 0x2cd です。
    PICA_REG_VS_PROG_DATA1                = 0x2cd,
    //! 0x2ce です。
    PICA_REG_VS_PROG_DATA2                = 0x2ce,
    //! 0x2cf です。
    PICA_REG_VS_PROG_DATA3                = 0x2cf,
    //! 0x2d0 です。
    PICA_REG_VS_PROG_DATA4                = 0x2d0,
    //! 0x2d1 です。
    PICA_REG_VS_PROG_DATA5                = 0x2d1,
    //! 0x2d2 です。
    PICA_REG_VS_PROG_DATA6                = 0x2d2,
    //! 0x2d3 です。
    PICA_REG_VS_PROG_DATA7                = 0x2d3,
    //! 0x2d5 です。
    PICA_REG_VS_PROG_SWIZZLE_ADDR         = 0x2d5,
    //! 0x2d6 です。
    PICA_REG_VS_PROG_SWIZZLE_DATA0        = 0x2d6,
    //! 0x2d7 です。
    PICA_REG_VS_PROG_SWIZZLE_DATA1        = 0x2d7,
    //! 0x2d8 です。
    PICA_REG_VS_PROG_SWIZZLE_DATA2        = 0x2d8,
    //! 0x2d9 です。
    PICA_REG_VS_PROG_SWIZZLE_DATA3        = 0x2d9,
    //! 0x2da です。
    PICA_REG_VS_PROG_SWIZZLE_DATA4        = 0x2da,
    //! 0x2db です。
    PICA_REG_VS_PROG_SWIZZLE_DATA5        = 0x2db,
    //! 0x2dc です。
    PICA_REG_VS_PROG_SWIZZLE_DATA6        = 0x2dc,
    //! 0x2ddです。
    PICA_REG_VS_PROG_SWIZZLE_DATA7        = 0x2dd
};

/*!  
  @}
*/

#endif // NN_GX_CTR_GX_MACRO_REG_H_
