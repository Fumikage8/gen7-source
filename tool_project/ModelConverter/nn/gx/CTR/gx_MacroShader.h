/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gx_MacroShader.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GX_CTR_GX_MACRO_SHADER_H_
#define NN_GX_CTR_GX_MACRO_SHADER_H_

#include <nn/gx/CTR/gx_MacroCommon.h>

/*!
  @addtogroup   nn_gx_CTR_Pica   PicaMacroHeader
  @{   
*/

/////////////////////////////////// 
// Draw API 
// Geometry Misc Registers
/*
struct CommandDrawAPIMode0     
{
    u32 geometryUse     : 2;
    CMD_PADDING(6);
    u32 mode            : 1;
    CMD_PADDING(1);        // set to 0 [9:9]
    CMD_PADDING(21);
    u32 geometryUseMode : 1;
};
*/

// PICA_REG_DRAW_MODE0      0x229
#define PICA_CMD_DATA_DRAW_MODE0(useGeometryShader, drawMode, useGeometryShaderSubdivision) \
    ( (useGeometryShader ? 2 : 0)                   | \
      (drawMode)                              <<  8 | \
       0                                      <<  9 | \
    ( (useGeometryShaderSubdivision) ? 1 : 0) << 31)

#define PICA_CMD_SET_DRAW_MODE0(drawMode)               \
    PICA_CMD_DATA_DRAW_MODE0(0x0, drawMode, 0x0),       \
    PICA_CMD_HEADER_SINGLE_BE(PICA_REG_DRAW_MODE0, 0x2)

#define PICA_CMD_SET_VERTEX_ATTR_ARRAYS_BASE_ADDR_DUMMY()                          \
    0x0, PICA_CMD_HEADER_BURST_BE(PICA_REG_VERTEX_ATTR_ARRAYS_BASE_ADDR, 30, 0x0), \
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                                        \
    0x0, 0x0,                                                                      \
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                                        \
    0x0, 0x0,                                                                      \
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                                        \
    0x0, 0x0

#define PICA_CMD_SET_DRAW_MODE0_DUMMY_BEGIN()                         \
    0x0, PICA_CMD_HEADER_BURST_BE(PICA_REG_VS_OUT_REG_NUM2, 10, 0x0), \
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                           \
    0x0, 0x0,                                                         \
    PICA_CMD_SET_VERTEX_ATTR_ARRAYS_BASE_ADDR_DUMMY()
    
#define PICA_CMD_SET_DRAW_MODE0_DUMMY_END() \
  PICA_CMD_SET_VERTEX_ATTR_ARRAYS_BASE_ADDR_DUMMY()

#define PICA_CMD_SET_VS_GS_MODE(useGeometryShader, useGeometryShaderSubdivision)    \
    PICA_CMD_SET_DRAW_MODE0_DUMMY_BEGIN(),                                          \
    PICA_CMD_DATA_DRAW_MODE0(useGeometryShader, 0x0, useGeometryShaderSubdivision), \
    PICA_CMD_HEADER_SINGLE_BE(PICA_REG_DRAW_MODE0, 0x9),                            \
    PICA_CMD_SET_DRAW_MODE0_DUMMY_END()

/*
struct CommandDrawAPIMode0     
{
    u32 func  : 1;
    CMD_PADDING(7);
    u32 mode  : 1;    
    CMD_PADDING(23);
};
*/

// PICA_REG_START_DRAW_FUNC  0x245
#define PICA_CMD_DATA_START_DRAW_FUNC(data) \
    ( (data) ? 1 : 0 )

// PICA_REG_DRAW_MODE1         0x253
#define PICA_CMD_DATA_DRAW_MODE1(func, mode) \
    ( (func) | (mode) << 8 )

#define PICA_CMD_SET_DRAW_MODE1(func, mode) \
    PICA_CMD_DATA_DRAW_MODE1(func, mode), PICA_CMD_HEADER_SINGLE_BE(PICA_REG_DRAW_MODE1, 0x3)

// PICA_REG_DRAW_VERTEX_NUM    0x228
#define PICA_CMD_DATA_DRAW_VERTEX_NUM(num) (num)

// PICA_REG_DRAW_VERTEX_OFFSET 0x22a
#define PICA_CMD_DATA_DRAW_VERTEX_OFFSET(offset) (offset)

// PICA_REG_START_DRAW_ARRAY   0x22e
#define PICA_CMD_DATA_START_DRAW_ARRAY(start) (start)

#define PICA_CMD_SET_START_DRAW_ARRAY(start)                                                          \
    PICA_CMD_DATA_START_DRAW_FUNC(0x0),    PICA_CMD_HEADER_SINGLE_BE(PICA_REG_START_DRAW_FUNC0, 0x1), \
    PICA_CMD_DATA_START_DRAW_ARRAY(start), PICA_CMD_HEADER_SINGLE(PICA_REG_START_DRAW_ARRAY),         \
    PICA_CMD_DATA_START_DRAW_FUNC(0x1),    PICA_CMD_HEADER_SINGLE_BE(PICA_REG_START_DRAW_FUNC0, 0x1), \
    0x1, PICA_CMD_HEADER_SINGLE(PICA_REG_VERTEX_FUNC),                                                \
    0, PICA_CMD_HEADER_SINGLE_BE(PICA_REG_DRAW_MODE2, 0x8),                                           \
    0, PICA_CMD_HEADER_SINGLE_BE(PICA_REG_DRAW_MODE2, 0x8)

// PICA_REG_START_DRAW_ELEMENT 0x22f
#define PICA_CMD_DATA_START_DRAW_ELEMENT(start) (start)

#define PICA_CMD_SET_START_DRAW_ELEMENT(start)                                                          \
    PICA_CMD_DATA_START_DRAW_FUNC(0x0),      PICA_CMD_HEADER_SINGLE_BE(PICA_REG_START_DRAW_FUNC0, 0x1), \
    PICA_CMD_DATA_START_DRAW_ELEMENT(start), PICA_CMD_HEADER_SINGLE(PICA_REG_START_DRAW_ELEMENT),       \
    PICA_CMD_DATA_START_DRAW_FUNC(0x1),      PICA_CMD_HEADER_SINGLE_BE(PICA_REG_START_DRAW_FUNC0, 0x1), \
    0x1, PICA_CMD_HEADER_SINGLE(PICA_REG_VERTEX_FUNC),                                                  \
    0, PICA_CMD_HEADER_SINGLE_BE(PICA_REG_DRAW_MODE2, 0x8),                                             \
    0, PICA_CMD_HEADER_SINGLE_BE(PICA_REG_DRAW_MODE2, 0x8)

/////////////////////////////////// 
// Vertex Shader float value
/*
struct CommandVSFloatAddr     
{
    u32 index : 8;
    CMD_PADDING(23);
    u32 mode  : 1;

};
*/
// PICA_REG_VS_FLOAT_ADDR  0x2c0

/*!
    @brief 頂点シェーダの浮動小数点の入力モードを設定します。
           PICA_REG_VS_FLOAT_ADDR レジスタ ( 0x2c0[31:31] ) に対して使用します。           
*/
enum PicaDataVSFloat
{
    //! 24 bit の浮動小数点の入力モードです。
    PICA_DATA_VS_F24 = 0x0,
    //! 32 bit の浮動小数点の入力モードです。
    PICA_DATA_VS_F32 = 0x1
};

#define PICA_CMD_DATA_VS_FLOAT_ADDR(mode, index) \
    ( (index) & 0xff | ( (mode) ? 0x80000000 : 0) )

#define PICA_CMD_SET_VS_FLOAT_ADDR(mode, index) \
    PICA_CMD_DATA_VS_FLOAT_ADDR(mode, index), PICA_CMD_HEADER_SINGLE(PICA_REG_VS_FLOAT_ADDR)

// PICA_REG_VS_FLOAT 0x2c1 to 0x2c8
#define PICA_CMD_DATA_VS_FLOAT(data) (data)

/////////////////////////////////// 
// Vertex Shader Bool

// PICA_REG_VS_BOOL      0x2b0

#define PICA_CMD_DATA_VS_BOOL(b) \
    ( (b) | 0x7fff0000 )

/////////////////////////////////// 
// Vertex Shader Integer
/* 
struct CommandVSInt          
{
	u32 x     : 8; 
	u32 y     : 8; 
	u32 z     : 8; 
	CMD_PADDING(8);
};
*/
// PICA_REG_VS_INT0     0x2b1 to 0x2b4
#define PICA_CMD_DATA_VS_INT(x, y, z) ( (x) | (y) << 8 | (z) << 16 )

/////////////////////////////////// 
// Vertex Shader Start Address

// PICA_REG_VS_START_ADDR  0x2ba
#define PICA_CMD_DATA_VS_START_ADDR(data) \
    ( (data) | 0x7fff0000 )

/////////////////////////////////// 
// Vertex Shader Attribute Number
  
// PICA_REG_VS_ATTR_NUM0  0x2b9
#define PICA_CMD_DATA_VS_ATTR_NUM0(num) \
    ( ((num - 1) & 0xf) | ( 0xa0000000 ) )

// PICA_REG_VS_ATTR_NUM1  0x242
#define PICA_CMD_DATA_VS_ATTR_NUM1(num) \
    ( (num - 1) & 0xf )
        
#define PICA_CMD_SET_VS_ATTR_NUM(num) \
    PICA_CMD_DATA_VS_ATTR_NUM0(num), PICA_CMD_HEADER_SINGLE_BE(PICA_REG_VS_ATTR_NUM0, 0xB), \
    PICA_CMD_DATA_VS_ATTR_NUM1(num), PICA_CMD_HEADER_SINGLE(PICA_REG_VS_ATTR_NUM1) \

/////////////////////////////////// 
// Draw API
// Vertex Shader Output Register Number
// Geometry Shader Output Register Number

#define PICA_CMD_DATA_VS_GS_OUT_REG_NUM(num) (num)

// PICA_REG_VS_OUT_REG_NUM0 0x04f
// PICA_REG_GS_OUT_REG_NUM0 0x04f
#define PICA_CMD_DATA_VS_GS_OUT_REG_NUM0(num) (num)

// PICA_REG_VS_OUT_REG_NUM1 0x24a
#define PICA_CMD_DATA_VS_GS_OUT_REG_NUM1(num) ( (num) - 1 )

// PICA_REG_VS_OUT_REG_NUM2 0x251
#define PICA_CMD_DATA_VS_GS_OUT_REG_NUM2(num) ( (num) - 1 )


/* 
struct CommandVSOutRegNum
{
	u32 num   : 4;
	CMD_PADDING(4);
	u32 mode  : 1;
	CMD_PADDING(23);
};
*/


/*!
    @brief 描画モードを設定します。
           PICA_REG_VS_OUT_REG_NUM3 レジスタ ( 0x25e[8:8] ) に対して使用します。           
*/
enum PicaDataDrawMode
{
    //! GL_TRIANGLE_STRIP です。
    PICA_DATA_DRAW_TRIANGLE_STRIP      = 0x1,
    //! GL_TRIANGLE_FAN です。
    PICA_DATA_DRAW_TRIANGLE_FAN        = 0x2,
    //! GL_TRIANGLES です。
    PICA_DATA_DRAW_TRIANGLES           = 0x3,
    //! GL_GEOMETRY_PRIMITIVE です。
    PICA_DATA_DRAW_GEOMETRY_PRIMITIVE  = 0x3
};

// PICA_REG_VS_OUT_REG_NUM3 0x25e
// PICA_REG_GS_OUT_REG_NUM3 0x25e
#define PICA_CMD_DATA_VS_GS_OUT_REG_NUM3(num, mode) \
    ( (num - 1) | (mode) << 8 )

#define PICA_CMD_SET_DRAW_MODE2(mode)                        \
    ((mode) << 8),                                           \
    PICA_CMD_HEADER_SINGLE_BE(PICA_REG_VS_OUT_REG_NUM3, 0x2)

#define PICA_CMD_SET_VS_GS_OUT_REG_NUM(num)                      \
    num, PICA_CMD_HEADER_SINGLE(PICA_REG_VS_OUT_REG_NUM0),       \
    (num - 1), PICA_CMD_HEADER_SINGLE(PICA_REG_VS_OUT_REG_NUM1), \
    (num - 1), PICA_CMD_HEADER_SINGLE(PICA_REG_VS_OUT_REG_NUM2), \
    (num - 1), PICA_CMD_HEADER_SINGLE_BE(PICA_REG_VS_OUT_REG_NUM3, 0x1)

/////////////////////////////////// 
// Vertex Shader Output Register Mask

// PICA_REG_VS_OUT_REG_MASK 0x2bd
#define PICA_CMD_DATA_VS_OUT_MASK(mask) \
    ( (mask) & 0xffff )

/////////////////////////////////// 
// Vertex Shader Output Attribute
// Geometry Shader Output Attribute

/*!
    @brief 頂点シェーダからの出力属性を設定します。
           PICA_REG_VS_OUT_REG_NUM3 レジスタ ( 0x050-0x056 ) に対して使用します。           
*/
enum PicaDataVSOutAttr
{
    //! 頂点座標 x です。
    PICA_DATA_VS_OUT_ATTR_X        = 0x00,
    //! 頂点座標 y です。
    PICA_DATA_VS_OUT_ATTR_Y        = 0x01,
    //! 頂点座標 z です。
    PICA_DATA_VS_OUT_ATTR_Z        = 0x02,
    //! 頂点座標 w です。
    PICA_DATA_VS_OUT_ATTR_W        = 0x03,
    //! クォータニオン x です。
    PICA_DATA_VS_OUT_ATTR_QUART_X  = 0x04,
    //! クォータニオン y です。
    PICA_DATA_VS_OUT_ATTR_QUART_Y  = 0x05,
    //! クォータニオン z です。
    PICA_DATA_VS_OUT_ATTR_QUART_Z  = 0x06,
    //! クォータニオン w です。
    PICA_DATA_VS_OUT_ATTR_QUART_W  = 0x07,
    //! 頂点カラー R です。
    PICA_DATA_VS_OUT_ATTR_R        = 0x08,
    //! 頂点カラー G です。
    PICA_DATA_VS_OUT_ATTR_G        = 0x09,
    //! 頂点カラー B です。
    PICA_DATA_VS_OUT_ATTR_B        = 0x0a,
    //! 頂点カラー A です。
    PICA_DATA_VS_OUT_ATTR_A        = 0x0b,
    //! テクスチャ座標 0 の u です。
    PICA_DATA_VS_OUT_ATTR_TEX0_U   = 0x0c,
    //! テクスチャ座標 0 の v です。
    PICA_DATA_VS_OUT_ATTR_TEX0_V   = 0x0d,
    //! テクスチャ座標 1 の u です。
    PICA_DATA_VS_OUT_ATTR_TEX1_U   = 0x0e,
    //! テクスチャ座標 1 の v です。
    PICA_DATA_VS_OUT_ATTR_TEX1_V   = 0x0f,
    //! テクスチャ座標 0 の w です。
    PICA_DATA_VS_OUT_ATTR_TEX0_W   = 0x10,
    //! ビューベクタの x です。
    PICA_DATA_VS_OUT_ATTR_VIEW_X   = 0x12,
    //! ビューベクタの y です。
    PICA_DATA_VS_OUT_ATTR_VIEW_Y   = 0x13,
    //! ビューベクタの z です。
    PICA_DATA_VS_OUT_ATTR_VIEW_Z   = 0x14,
    //! テクスチャ座標 2 の u です。
    PICA_DATA_VS_OUT_ATTR_TEX2_U   = 0x16,
    //! テクスチャ座標 2 の v です。
    PICA_DATA_VS_OUT_ATTR_TEX2_V   = 0x17,
    //! 無効です。
    PICA_DATA_VS_OUT_ATTR_INVALID  = 0x1f
};

/*
struct CommandVSOutAttr
{
	u32 attr_x : 5; 
	CMD_PADDING(3);
	u32 attr_y : 5;
	CMD_PADDING(3);
	u32 attr_z : 5;
	CMD_PADDING(3);
	u32 attr_w : 5;
	CMD_PADDING(3);
};
*/

// PICA_REG_VS_OUT_ATTR0  0x050 to 0x056
// PICA_REG_GS_OUT_ATTR0  0x050 to 0x056
#define PICA_CMD_DATA_VS_GS_OUT_ATTR(attr_x, attr_y, attr_z, attr_w) \
    ( (attr_x) | (attr_y) << 8 | (attr_z) << 16 | (attr_w) << 24 )

// PICA_REG_VS_OUT_ATTR_MODE 0x064
// PICA_REG_GS_OUT_ATTR_MODE 0x064
#define PICA_CMD_DATA_VS_GS_OUT_ATTR_MODE( mode ) (mode)

/////////////////////////////////// 
// Vertex Shader Output Attribute clock
// Geometry Shader Output Attribute clock
/*
struct CommandVSOutAttrClk
{
	u32 verZ : 1; 
	u32 col  : 1;
	CMD_PADDING(6);
	u32 tex0 : 1;
	u32 tex1 : 1;
	u32 tex2 : 1;
	CMD_PADDING(5);
	u32 tex0_w : 1;
	CMD_PADDING(7);
	u32 view_quart : 1;
	CMD_PADDING(7);
};
*/

// PICA_REG_VS_OUT_ATTR_CLK  0x06f
// PICA_REG_GS_OUT_ATTR_CLK  0x06f
#define PICA_CMD_DATA_VS_GS_OUT_ATTR_CLK(posZ, col, tex0, tex1, tex2, tex0_w, view_quart) \
    ( ((posZ)       ? 1 : 0)       | \
      ((col)        ? 1 : 0) <<  1 | \
      ((tex0)       ? 1 : 0) <<  8 | \
      ((tex1)       ? 1 : 0) <<  9 | \
      ((tex2)       ? 1 : 0) << 10 | \
      ((tex0_w)     ? 1 : 0) << 16 | \
      ((view_quart) ? 1 : 0) << 24 )
    
/////////////////////////////////// 
// Vertex Shader Program Code

// PICA_REG_VS_PROG_ADDR          0x2cb
#define PICA_CMD_DATA_VS_PROG_ADDR(addr) (addr)

// PICA_REG_VS_PROG_SWIZZLE_ADDR  0x2d5
#define PICA_CMD_DATA_VS_PROG_SWIZZLE_ADDR(addr) (addr)

// PICA_REG_VS_PROG_DATA0         0x2cc to 0x2d3
#define PICA_CMD_DATA_VS_PROG_DATA(data) (data)

// PICA_REG_VS_PROG_SWIZZLE_DATA0 0x2d6 to 0x2dd
#define PICA_CMD_DATA_VS_PROG_SWIZZLE_DATA(data) (data)

// PICA_REG_VS_PROG_END           0x2bf
#define PICA_CMD_DATA_VS_PROG_END(data) (data)

/////////////////////////////////// 
// Vertex Shader Attribute Input Register Map
/*
struct CommandVSAttrInputRegMap0
    u32 index1 : 4;
    u32 index2 : 4;
    u32 index3 : 4;
    u32 index4 : 4;
    u32 index5 : 4;
    u32 index6 : 4;
    u32 index7 : 4;
    u32 index8 : 4;
};
*/

// PICA_REG_VS_ATTR_IN_REG_MAP0  0x2bb
#define PICA_CMD_DATA_VS_ATTR_IN_REG_MAP0(index1, index2, index3, index4, index5, index6, index7, index8) \
      ((index1)       | (index2) << 4  | (index3) << 8  | (index4) << 12 | \
       (index5) << 16 | (index6) << 20 | (index7) << 24 | (index8) << 28 )

/*
struct CommandVSAttrInputRegMap1
{
    u32 index9  : 4;
    u32 index10 : 4;
    u32 index11 : 4;
    u32 index12 : 4;
    CMD_PADDING(16);

};
*/

// PICA_REG_VS_ATTR_IN_REG_MAP1  0x2bc
#define PICA_CMD_DATA_VS_ATTR_IN_REG_MAP1(index9, index10, index11, index12) \
      ((index9) | (index10) << 4 | (index11) << 8 | (index12) << 12)

/////////////////////////////////// 
//  Vertex Shader Fixed Attribute

// PICA_REG_VS_FIXED_ATTR          0x232
#define PICA_CMD_DATA_VS_FIXED_ATTR(order)  ( (order) & 0xf )

// PICA_REG_VS_FIXED_ATTR_DATA0  0x233 to 0x235
#define PICA_CMD_DATA_VS_FIXED_ATTR_DATA(data)  (data)

#define PICA_CMD_SET_VS_FIXED_ATTR(order, c0, c1, c2) \
    PICA_CMD_DATA_VS_FIXED_ATTR(order), PICA_CMD_HEADER_BURSTSEQ( PICA_REG_VS_FIXED_ATTR, 4), \
    (u32)(c0),                          (u32)(c1), \
    (u32)(c2),                          PICA_CMD_DATA_ZERO()

/////////////////////////////////// 
//  Vertex Attribute Arrays
/*
struct CommandVtxAttrArraysBaseAddr
{
    CMD_PADDING(1);
    u32 addr : 28;
    CMD_PADDING(3);
};
*/
// PICA_REG_VERTEX_ATTR_ARRAYS_BASE_ADDR   0x200
#define PICA_CMD_DATA_VERTEX_ATTR_ARRAYS_BASE_ADDR(addr) ( ((u32)(addr) >> 4) << 1 )

/*
struct CommandVAttrArrays
{
    u32 attr0 : 4;
    u32 attr1 : 4;
    u32 attr2 : 4;
    u32 attr3 : 4;
    u32 attr4 : 4;
    u32 attr5 : 4;
    u32 attr6 : 4;
    u32 attr7 : 4;
};
*/


/*!
    @brief 内部頂点属性の型を設定します。
           PICA_REG_VERTEX_ATTR_ARRAYS0-1 レジスタ ( 0x201-0x202 ) に対して使用します。           
           glVertexAttribPointer() に相当します。
*/
enum PicaDataVertexAttrType
{
    //! size = 1, type = GL_BYTE です。
    PICA_DATA_SIZE_1_BYTE          = 0x0,
    //! size = 1, type = GL_UNSIGNED_BYTE です。
    PICA_DATA_SIZE_1_UNSIGNED_BYTE = 0x1,
    //! size = 1, type = GL_SHORT です。
    PICA_DATA_SIZE_1_SHORT         = 0x2,
    //! size = 1, type = GL_FLOAT です。
    PICA_DATA_SIZE_1_FLOAT         = 0x3,
    //! size = 2, type = GL_BYTE です。
    PICA_DATA_SIZE_2_BYTE          = 0x4,
    //! size = 2, type = GL_UNSIGNED_BYTE です。
    PICA_DATA_SIZE_2_UNSIGNED_BYTE = 0x5,
    //! size = 2, type = GL_SHORT です。
    PICA_DATA_SIZE_2_SHORT         = 0x6,
    //! size = 2, type = GL_FLOAT です。
    PICA_DATA_SIZE_2_FLOAT         = 0x7,
    //! size = 3, type = GL_BYTE です。
    PICA_DATA_SIZE_3_BYTE          = 0x8,
    //! size = 3, type = GL_UNSIGNED_BYTE です。
    PICA_DATA_SIZE_3_UNSIGNED_BYTE = 0x9,
    //! size = 3, type = GL_SHORT です。
    PICA_DATA_SIZE_3_SHORT         = 0xa,
    //! size = 3, type = GL_FLOAT です。
    PICA_DATA_SIZE_3_FLOAT         = 0xb,
    //! size = 4, type = GL_BYTE です。
    PICA_DATA_SIZE_4_BYTE          = 0xc,
    //! size = 4, type = GL_UNSIGNED_BYTE です。
    PICA_DATA_SIZE_4_UNSIGNED_BYTE = 0xd,
    //! size = 4, type = GL_SHORT です。
    PICA_DATA_SIZE_4_SHORT         = 0xe,
    //! size = 4, type = GL_FLOAT です。
    PICA_DATA_SIZE_4_FLOAT         = 0xf
};

// PICA_REG_VERTEX_ATTR_ARRAYS0         0x201
#define PICA_CMD_DATA_VERTEX_ATTR_ARRAYS0(attr0, attr1, attr2, attr3, attr4, attr5, attr6, attr7) \
   ((attr0)       | (attr1) <<  4 | (attr2) <<  8 | (attr3) << 12 |                               \
    (attr4) << 16 | (attr5) << 20 | (attr6) << 24 | (attr7) << 28)

// PICA_REG_VERTEX_ATTR_ARRAYS1         0x202
#define PICA_CMD_DATA_VERTEX_ATTR_ARRAYS1(attr8, attr9, attr10, attr11, mask, attrNum) \
   ( (attr8)               |                                                           \
     (attr9)         <<  4 |                                                           \
     (attr10)        <<  8 |                                                           \
     (attr11)        << 12 |                                                           \
     (mask)          << 16 |                                                           \
     ((attrNum) - 1) << 28 )

// PICA_REG_LOAD_ARRAY0_ATTR_OFFSET  0x203
#define PICA_CMD_DATA_LOAD_ARRAY0_ATTR_OFFSET(offset) (offset)

/*
struct CommandLodArray0Element
{
    u32 attr0 : 4;
    u32 attr1 : 4;
    u32 attr2 : 4;
    u32 attr3 : 4;
    u32 attr4 : 4;
    u32 attr5 : 4;
    u32 attr6 : 4;
    u32 attr7 : 4;
};
*/

/*!
    @brief ロードアレイの要素を設定します。
           PICA_REG_LOAD_ARRAY_ELEMENTS0-1 レジスタ ( 0x204-0x205 ) に対して使用します。                      
*/
enum PicaDataVertexAttr
{
    //! 内部頂点属性 0 です。
    PICA_DATA_VERTEX_0_ATTR          = 0x0,
    //! 内部頂点属性 1 です。
    PICA_DATA_VERTEX_1_ATTR          = 0x1,
    //! 内部頂点属性 2 です。
    PICA_DATA_VERTEX_2_ATTR          = 0x2,
    //! 内部頂点属性 3 です。
    PICA_DATA_VERTEX_3_ATTR          = 0x3,
    //! 内部頂点属性 4 です。
    PICA_DATA_VERTEX_4_ATTR          = 0x4,
    //! 内部頂点属性 5 です。
    PICA_DATA_VERTEX_5_ATTR          = 0x5,
    //! 内部頂点属性 6 です。
    PICA_DATA_VERTEX_6_ATTR          = 0x6,
    //! 内部頂点属性 7 です。
    PICA_DATA_VERTEX_7_ATTR          = 0x7,
    //! 内部頂点属性 8 です。
    PICA_DATA_VERTEX_8_ATTR          = 0x8,
    //! 内部頂点属性 9 です。
    PICA_DATA_VERTEX_9_ATTR          = 0x9,
    //! 内部頂点属性 10 です。
    PICA_DATA_VERTEX_a_ATTR          = 0xa,
    //! 内部頂点属性 11 です。
    PICA_DATA_VERTEX_b_ATTR          = 0xb,
    //! 4 Byte のパッディングです。
    PICA_DATA_PADDING_4_BYTE         = 0xc,
    //! 8 Byte のパッディングです。
    PICA_DATA_PADDING_8_BYTE         = 0xd,
    //! 12 Byte のパッディングです。
    PICA_DATA_PADDING_12_BYTE        = 0xe,
    //! 16 Byte のパッディングです。
    PICA_DATA_PADDING_16_BYTE        = 0xf
};

// PICA_REG_LOAD_ARRAY0_ELEMENT0         0x204
#define PICA_CMD_DATA_LOAD_ARRAY_ELEMENT0(attr0, attr1, attr2, attr3, attr4, attr5, attr6, attr7) \
   ( (attr0)       | (attr1) << 4  | (attr2) << 8  | (attr3) << 12 |                               \
     (attr4) << 16 | (attr5) << 20 | (attr6) << 24 | (attr7) << 28)

// PICA_REG_LOAD_ARRAY0_ELEMENT1         0x205
#define PICA_CMD_DATA_LOAD_ARRAY_ELEMENT1(attr8, attr9, attr10, attr11, byteNum, elementNum) \
   ( (attr8)            |                                                                    \
     (attr9)      << 4  |                                                                    \
     (attr10)     << 8  |                                                                    \
     (attr11)     << 12 |                                                                    \
     (byteNum)    << 16 |                                                                    \
     (elementNum) << 28 )
     

/*
struct CommandIndexArrayAddrOffset
{
    u32 offset : 28;
    CMD_PADDING(3);
    u32 type   : 1;
};
*/

enum PicaDataIndexArray
{
    PICA_DATA_INDEX_ARRAY_UNSIGNED_BYTE  = 0x0,
    PICA_DATA_INDEX_ARRAY_UNSIGNED_SHORT = 0x1
};

// PICA_REG_INDEX_ARRAY_ADDR_OFFSET   0x227
#define PICA_CMD_DATA_INDEX_ARRAY_ADDR_OFFSET(offset, type) \
    ( (offset) | ((type) ? 0x80000000 : 0 ) )

/////////////////////////////////// 
// Geometry Shader Over View 

//    PICA_REG_VS_COM_MODE            0x244
#define PICA_CMD_DATA_VS_COM_MODE(mode) ((mode) ? 1 : 0)

#define PICA_CMD_SET_VS_COM_MODE(mode) \
    PICA_CMD_DATA_VS_COM_MODE(mode), PICA_CMD_HEADER_SINGLE_BE(PICA_REG_VS_COM_MODE, 0x1)

/////////////////////////////////// 
// Geometry Shader float value
/*
struct CommandGSFloatAddr     
{
    u32 index : 8;
    CMD_PADDING(23);
    u32 mode  : 1;

};
*/

/*!
    @brief ジオメトリシェーダの浮動小数点の入力モードを設定します。
           PICA_REG_GS_FLOAT_ADDR レジスタ ( 0x290[31:31] ) に対して使用します。           
*/
enum PicaDataGSFloat
{
    //! 24 bit の浮動小数点の入力モードです。
    PICA_DATA_GS_F24 = 0x0,
    //! 32 bit の浮動小数点の入力モードです。
    PICA_DATA_GS_F32 = 0x1
};

// PICA_REG_GS_FLOAT_ADDR  0x290

#define PICA_CMD_DATA_GS_FLOAT_ADDR(mode, index) \
    ( ((index) & 0xff) | (mode) << 31 )

#define PICA_CMD_SET_GS_FLOAT_ADDR(mode, index) \
    PICA_CMD_DATA_GS_FLOAT_ADDR(mode, index), PICA_CMD_HEADER_SINGLE(PICA_REG_GS_FLOAT_ADDR)

// PICA_REG_GS_FLOAT_DATA  0x291 to 0x298
#define PICA_CMD_DATA_GS_FLOAT_DATA(data) (data)

/////////////////////////////////// 
// Geometry Shader Bool

// PICA_REG_GS_BOOL      0x280
#define PICA_CMD_DATA_GS_BOOL(b) \
    ( (b) | 0x7fff0000 )

/////////////////////////////////// 
// Geometry Shader Integer
/* 
struct CommandGSInt          
{
	u32 x     : 8; 
	u32 y     : 8; 
	u32 z     : 8; 
	CMD_PADDING(8);
};
*/
// PICA_REG_GS_INT0     0x281 to 0x284
#define PICA_CMD_DATA_GS_INT(x, y, z) ( (x) | (y) << 8 | (z) << 16 )

/////////////////////////////////// 
// Geometry Shader Start Address

// PICA_REG_GS_START_ADDR  0x28a
#define PICA_CMD_DATA_GS_START_ADDR(data) \
    ( (data) | 0x7fff0000 )

/////////////////////////////////// 
// Geometry Shader Attribute Number
// Geometry Misc Registers
/* 
struct CommandGSAttrNum
{
	u32 num   : 4;
	CMD_PADDING(4);
	u32 mode0 : 8;
	CMD_PADDING(8);
	u32 mode1 : 8;
};
*/

// 0x289[15:8] : 0x1/0x0, 
// 0x289[31:24] : 0x08/0xa0, 


// PICA_REG_GS_ATTR_NUM  0x289
#define PICA_CMD_DATA_GS_ATTR_NUM(geometryShaderVertexAttrInputNum, useGeometrySubdivision, useGeometryShader) \
    ( ( (geometryShaderVertexAttrInputNum) - 1)   | \
      ( (useGeometrySubdivision) ? 1 : 0)    << 8 | \
      ( ((useGeometryShader) ? 0x08  : 0xa0) << 24 ) )

#define PICA_CMD_SET_GS_ATTR_NUM(geometryShaderVertexAttrInputNum, useGeometrySubdivision, useGeometryShader) \
    PICA_CMD_DATA_GS_ATTR_NUM(geometryShaderVertexAttrInputNum, useGeometrySubdivision, useGeometryShader),   \
    PICA_CMD_HEADER_SINGLE_BE(PICA_REG_GS_ATTR_NUM, 0xB)

/////////////////////////////////// 
// Geometry Shader Output Register Mask

// PICA_REG_GS_OUT_REG_MASK 0x28d
#define PICA_CMD_DATA_GS_OUT_MASK(mask) \
    ( (mask) & 0xffff )

/////////////////////////////////// 
// Geometry Shader Program Code


// PICA_REG_GS_PROG_ADDR          0x29b
#define PICA_CMD_DATA_GS_PROG_ADDR(addr) (addr)

// PICA_REG_GS_PROG_SWIZZLE_ADDR  0x2a5
#define PICA_CMD_DATA_GS_PROG_SWIZZLE_ADDR(addr) (addr)

// PICA_REG_GS_PROG_DATA0         0x29c to 0x2a3
#define PICA_CMD_DATA_GS_PROG_DATA(data) (data)

// PICA_REG_GS_PROG_SWIZZLE_DATA0 0x2a6 to 0x2ad
#define PICA_CMD_DATA_GS_PROG_SWIZZLE_DATA(data) (data)

/////////////////////////////////// 
// Geometry Shader Attribute Input Register Map
/*
struct CommandGSAttrInputRegMap0
    u32 index1 : 4;
    u32 index2 : 4;
    u32 index3 : 4;
    u32 index4 : 4;
    u32 index5 : 4;
    u32 index6 : 4;
    u32 index7 : 4;
    u32 index8 : 4;
};
*/

// PICA_REG_GS_ATTR_IN_REG_MAP0  0x28b
#define PICA_CMD_DATA_GS_ATTR_IN_REG_MAP0(index1, index2, index3, index4, index5, index6, index7, index8) \
      ( (index1)       | (index2) <<  4 | (index3) <<  8 | (index4) << 12 | \
        (index5) << 16 | (index6) << 20 | (index7) << 24 | (index8) << 28 )

/*
struct CommandGSAttrInputRegMap1
{
    u32 index9  : 4;
    u32 index10 : 4;
    u32 index11 : 4;
    u32 index12 : 4;
    CMD_PADDING(16);

};
*/

// PICA_REG_GS_ATTR_IN_REG_MAP1  0x28c
#define PICA_CMD_DATA_GS_ATTR_IN_MAP1(index9, index10, index11, index12) \
      ((index9) | (index10) << 4 | (index11) << 8 | (index12) << 12)

#define PICA_CMD_SET_GS_ATTR_IN_MAP_RESERVED() \
    0x76543210, PICA_CMD_HEADER_SINGLE(PICA_REG_GS_ATTR_IN_REG_MAP0), \
    0xfedcba98, PICA_CMD_HEADER_SINGLE(PICA_REG_GS_ATTR_IN_REG_MAP1)

/////////////////////////////////// 
// Geometry Misc Registers

/*!
    @brief ジオメトリシェーダのモードを設定します。
           PICA_REG_GS_MISC_REG0レジスタ ( 0x252[31:0] ) に対して使用します。           
*/
enum PicaDataGSMode
{
    //! その他のモードです。
    PICA_DATA_GS_OTHER_MODE               = 0x0,
    //! 予約ジオメトリシェーダのサブディビジョン(ループ、カトマルクラーク)を使用します。
    PICA_DATA_GS_SUBDIVISION_MODE         = 0x1,
    //! 予約ジオメトリシェーダのサブディビジョンであるカトマルクラークを使用します。
    PICA_DATA_GS_SUBDIVISION_CATMULL_MODE = 0x3,
    //! 予約ジオメトリシェーダのサブディビジョンであるループを使用します。
    PICA_DATA_GS_SUBDIVISION_LOOP_MODE    = 0x5,
    //! パーティクルシステムを使用します。
    PICA_DATA_GS_PARTICLE_MODE            = 0x2
};

// PICA_REG_GS_MISC_REG0  0x252
#define PICA_CMD_DATA_GS_MISC_REG0(mode) \
    ( (mode & PICA_DATA_GS_SUBDIVISION_MODE) ? 0x1 : ( (mode == PICA_DATA_GS_PARTICLE_MODE) ? 0x01004302 : 0) )

#define PICA_CMD_SET_GS_MISC_REG0_SUBDIVISION() \
    PICA_CMD_DATA_GS_MISC_REG0(PICA_DATA_GS_SUBDIVISION_MODE), \
    PICA_CMD_HEADER_SINGLE(PICA_REG_GS_MISC_REG0)
   
#define PICA_CMD_SET_GS_MISC_REG0_SUBDIVISION_CATMULL() \
    PICA_CMD_DATA_GS_MISC_REG0(PICA_DATA_GS_SUBDIVISION_CATMULL_MODE), \
    PICA_CMD_HEADER_SINGLE(PICA_REG_GS_MISC_REG0)

#define PICA_CMD_SET_GS_MISC_REG0_SUBDIVISION_LOOP() \
    PICA_CMD_DATA_GS_MISC_REG0(PICA_DATA_GS_SUBDIVISION_LOOP_MODE), \
    PICA_CMD_HEADER_SINGLE(PICA_REG_GS_MISC_REG0)

#define PICA_CMD_SET_GS_MISC_REG0_PARTICLE() \
    PICA_CMD_DATA_GS_MISC_REG0(PICA_DATA_GS_PARTICLE_MODE), \
    PICA_CMD_HEADER_SINGLE(PICA_REG_GS_MISC_REG0)

#define PICA_CMD_SET_GS_MISC_REG0_OTHER() \
    PICA_CMD_DATA_GS_MISC_REG0(PICA_DATA_GS_OTHER_MODE), \
    PICA_CMD_HEADER_SINGLE(PICA_REG_GS_MISC_REG0)

// PICA_REG_GS_MISC_REG1 0x254
#define PICA_CMD_DATA_GS_MISC_REG1(mode) \
    ( (mode == PICA_DATA_GS_SUBDIVISION_CATMULL_MODE) ? 3 : ( (mode == PICA_DATA_GS_SUBDIVISION_LOOP_MODE) ? 2 : 0) )

/////////////////////////////////// 
// Appendex Registers 

//    PICA_REG_START_DRAW_FUNC1          0x25f
#define PICA_CMD_DATA_START_DRAW_FUNC1(resetVertexTriangleIndex) \
    ( (resetVertexTriangleIndex) ? 1 : 0 )

/*!
  @}
*/

#endif  // NN_GX_CTR_GX_MACRO_SHADER_H_
