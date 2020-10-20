/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gx_MacroMisc.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GX_CTR_GX_MACRO_MISC_H_
#define NN_GX_CTR_GX_MACRO_MISC_H_

#include <nn/gx/CTR/gx_MacroCommon.h>

/*!
  @addtogroup   nn_gx_CTR_Pica   PicaMacroHeader
  @{
*/

///////////////////////////////////
// Render Buffer

// PICA_REG_RENDER_BUFFER_COLOR_ADDR   0x11d
#define PICA_CMD_DATA_RENDER_BUFFER_COLOR_ADDR( addr ) ( (addr) >> 3 )

   
// PICA_REG_RENDER_BUFFER_DEPTH_ADDR   0x11c
#define PICA_CMD_DATA_RENDER_BUFFER_DEPTH_ADDR( addr ) ( (addr) >> 3 )
 
/*
struct CommandRenderBufColorMode
{
    u32 pixSize : 2;
    CMD_PADDING(14);
    u32 format  : 3;
    CMD_PADDING(13);
};
*/

/*!
    @brief カラーバッファのフォーマットのピクセルサイズです。
           PICA_REG_RENDER_BUFFER_COLOR_MODE レジスタ ( 0x117[1:0] ) で使います。
*/
enum PicaDataColorPixelSize
{
    //! カラーバッファのフォーマットのピクセルサイズが 16 bit です。
    PICA_DATA_COLOR_PIXEL_SIZE16 = 0x0,
    //! カラーバッファのフォーマットのピクセルサイズが 32 bit です。
    PICA_DATA_COLOR_PIXEL_SIZE32 = 0x2
};


/*!
    @brief カラーバッファのフォーマットです。
           PICA_REG_RENDER_BUFFER_COLOR_MODE レジスタ ( 0x117[18:16] ) で使います。
*/
enum PicaDataColor
{
    //! RGBA8_OES フォーマットです。
    PICA_DATA_COLOR_RGBA8_OES = 0x0,
    //! GAS_DMP フォーマットです。
    PICA_DATA_COLOR_GAS_DMP   = 0x0,
    //! RGB5_A1 フォーマットです。
    PICA_DATA_COLOR_RGB5_A1   = 0x2,
    //! RGB565 フォーマットです。
    PICA_DATA_COLOR_RGB565    = 0x3,
    //! RGBA4 フォーマットです。
    PICA_DATA_COLOR_RGBA4     = 0x4
};

// PICA_REG_RENDER_BUFFER_COLOR_MODE   0x117
#define PICA_CMD_DATA_RENDER_BUFFER_COLOR_MODE( pixSize, format ) \
    ( (pixSize) | (format) << 16 )
    

/*!
    @brief デプスバッファのフォーマットです。
           PICA_REG_RENDER_BUFFER_DEPTH_MODE レジスタ ( 0x116[1:0] ) で使います。
*/
enum PicaDataDepth
{
    //! DEPTH_COMPONENT16 フォーマットです。    
    PICA_DATA_DEPTH_COMPONENT16     = 0x0,
    //! DEPTH_COMPONENT24_OES フォーマットです。
    PICA_DATA_DEPTH_COMPONENT24_OES = 0x2,
    //! DEPTH_COMPONENT24_STENCIL8_EXT フォーマットです。
    PICA_DATA_DEPTH24_STENCIL8_EXT  = 0x3
};

// PICA_REG_RENDER_BUFFER_DEPTH_MODE   0x116
#define PICA_CMD_DATA_RENDER_BUFFER_DEPTH_MODE( mode ) (mode)

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
// PICA_REG_RENDER_BUFFER_RESOLUTION0    0x11e
// PICA_REG_RENDER_BUFFER_RESOLUTION1    0x6e
#define PICA_CMD_DATA_RENDER_BUFFER_RESOLUTION( width, height ) \
    ( (width)            | \
      (height - 1) << 12 | \
      0x01000000 )


///////////////////////////////////
// Gas Light
// Fog
// Depth Test
// Gas
/*
struct CommandGasLightXY
{
    u32 lightXY1 : 8;
    u32 lightXY2 : 8;
    u32 lightXY3 : 8;
    CMD_PADDING(8);
};
*/
// PICA_REG_GAS_LIGHT_XY            0x120
#define PICA_CMD_DATA_GAS_LIGHT_XY(lightXY1, lightXY2, lightXY3) \
    ( (lightXY1) | (lightXY2) << 8 | (lightXY3) << 16 )

// PICA_REG_GAS_LIGHT_Z             0x121
#define PICA_CMD_DATA_GAS_LIGHT_Z(lightZ1, lightZ2, lightZ3) \
    ( (lightZ1) | (lightZ2) << 8 | (lightZ3) << 16 )

/*
struct CommandGasLightZColor
{
    u32 lightz         : 8;
    u32 colorLutInput  : 1;
    CMD_PADDING(23);
};
*/

// colorLutInput

/*!
    @brief ガスのシェーディング参照テーブルの入力です。
           PICA_REG_GAS_LIGHT_Z_COLOR レジスタ ( 0x122[8:8] ) で使います。
           dmp_Gas.colorLutInput ユニフォームに相当します。

*/
enum PicaDataGasColorLutInput
{
    //! シェーディング参照テーブルへの入力値を密度情報にします。
    PICA_DATA_GAS_DENSITY_DMP      = 0x0,
    //! シェーディング参照テーブルへの入力値をシェーディング強度にします。
    PICA_DATA_GAS_LIGHT_FACTOR_DMP = 0x1
};

// PICA_REG_GAS_LIGHT_Z_COLOR         0x122
#define PICA_CMD_DATA_GAS_LIGHT_Z_COLOR(lightZ4, colorLutInput) \
    ( (lightZ4) | ((colorLutInput) ? 1 : 0) << 8 )

/*
struct CommandGasDeltaZDepth
{
    u32 deltaZ     : 24;
    u32 depthFunc  : 2;
    CMD_PADDING(6);
};
*/

/*!
    @brief デプステストの比較関数を設定します。
           PICA_REG_GAS_DELTAZ_DEPTH レジスタ ( 0x126[25:24] ) で使います。
           glDepthFunc() の引数 func に相当します
*/
enum PicaDataDepthTest2
{
    //! GL_NEVER です。
    PICA_DATA_DEPTH_TEST2_NEVER   = 0x0,
    //! GL_ALWAYS です。
    PICA_DATA_DEPTH_TEST2_ALWAYS  = 0x1,
    //! GL_GREATER です。
    PICA_DATA_DEPTH_TEST2_GREATER = 0x2,
    //! GL_GEQUAL です。
    PICA_DATA_DEPTH_TEST2_GEQUAL  = 0x2,
    //! GL_OTHER です。
    PICA_DATA_DEPTH_TEST2_OTHER   = 0x3
};

// PICA_REG_GAS_DELTAZ_DEPTH        0x126
#define PICA_CMD_DATA_GAS_DELTAZ_DEPTH(deltaZ, depthTest2Func) \
    ( (deltaZ) | (depthTest2Func) << 24 )

/*
struct CommandGasFogMode
{
    u32 FogMode            : 3;
    u32 shadingDensitySrc  : 1;
    CMD_PADDING(12);
    u32 zFlip              : 1;
    CMD_PADDING(15);
};
*/

// Fog

/*!
    @brief フォグ機能を設定します。
           PICA_REG_GAS_FOG_MODE レジスタ ( 0x0e0[2:0] ) で使います。
           dmp_Fog.mode ユニフォームに相当します。
*/
enum PicaDataFogMode
{
    //! フォグ機能を無効にします。
    PICA_DATA_FOG_FALSE  = 0x0,
    //! フォグモードを有効にします。
    PICA_DATA_FOG        = 0x5,
    //! ガスモードを有効にします。
    PICA_DATA_GAS        = 0x7
};

/*!
    @brief ガスのシェーディングで使用する密度情報を設定します。
           PICA_REG_GAS_FOG_MODE レジスタ ( 0x0e0[3:3] ) で使います。
           dmp_Gas.shadingDensitySrc ユニフォームに相当します。
*/
enum PicaDataGasShadingDensitySrc
{
    //! フォグへの入力密度情報 D1 をシェーディングに使用します。
    PICA_DATA_GAS_PLAIN_DENSITY_DMP = 0x0,
    //! フォグへの入力密度情報 D2 をシェーディングに使用します。    
    PICA_DATA_GAS_DEPTH_DENSITY_DMP = 0x1
};

// PICA_REG_GAS_FOG_MODE            0x0e0
#define PICA_CMD_DATA_GAS_FOG_MODE(fogMode, shadingDensitySrc, zFlip) \
    ( (fogMode) | \
      ((shadingDensitySrc) ? 1 : 0) << 3 | \
      ((zFlip) ? 1 : 0) << 16)

#define PICA_CMD_SET_GAS_FOG_MODE(fogMode, shadingDensitySrc, zFlip) \
    PICA_CMD_DATA_GAS_FOG_MODE(fogMode, shadingDensitySrc, zFlip), \
    PICA_CMD_HEADER_SINGLE_BE(PICA_REG_GAS_FOG_MODE, 0x5)

// PICA_REG_GAS_ATTENUATION         0x0e4
#define PICA_CMD_DATA_GAS_ATTENUATION(attenuation) (attenuation)

// PICA_REG_GAS_ACCMAX              0x0e5
#define PICA_CMD_DATA_GAS_ACCMAX(accMax) (accMax)

///////////////////////////////////
// Gas shading Lut Table

// PICA_REG_GAS_LUT_INDEX           0x123
#define PICA_CMD_DATA_GAS_LUT_INDEX(index) (index)

// PICA_REG_GAS_LUT_DATA            0x124
#define PICA_CMD_DATA_GAS_LUT_DATA(color8) \
    ( (color8.r) | (color8.g) << 8 | (color8.b) << 16 )

///////////////////////////////////
// Fog

// PICA_REG_FOG_COLOR               0x0e1
#define PICA_CMD_DATA_FOG_COLOR(color8) \
    ( (color8.r) | (color8.g) << 8 | (color8.b) << 16 )

///////////////////////////////////
// Fog Reference Table

// PICA_REG_FOG_LUT_INDEX       0x0e6
#define PICA_CMD_DATA_FOG_LUT_INDEX(index) (index)

// PICA_REG_FOG_LUT_DATA0       0x0e8 to 0x0ef
#define PICA_CMD_DATA_FOG_LUT_DATA(data) (data)


///////////////////////////////////
// Fragment Operation mode
// Blend / LogicOp
/*
struct CommandColorOperation
{
    u32 fragOpMode : 2;
    CMD_PADDING(6);
    u32 blendMode  : 1;
    CMD_PADDING(7);
    CMD_PADDING(16);      // 0x0e4

};
*/

/*!
    @brief フラグメントオペレーションの設定をします。
           PICA_REG_COLOR_OPERATION レジスタ ( 0x100[1:0] ) で使います。
           dmp_FragOperation.mode ユニフォームに相当します。
*/
enum PicaDataFragOpMode
{
    //! フラグメントオペレーションを標準モードにします。
    PICA_DATA_FRAGOP_MODE_DMP         = 0x0,
    //! フラグメントオペレーションをガスモードにします。
    PICA_DATA_FRAGOP_MODE_GAS_ACC_DMP = 0x1,
    //! フラグメントオペレーションをシャドウモードにします。
    PICA_DATA_FRAGOP_MODE_SHADOW_DMP  = 0x3
};

/*!
    @brief 論理演算とブレンドの設定をします。
           PICA_REG_COLOR_OPERATION レジスタ ( 0x100[8:8] ) で使います。
           glEnable(GL_COLOR_LOGIC_OP), glDisable(GL_COLOR_LOGIC_OP) に相当します。
*/
enum PicaDataColorLogicOp
{
    //! 論理演算を有効にします。
    PICA_DATA_ENABLE_COLOR_LOGIC_OP = 0x0,
    //! ブレンドを有効にします。
    PICA_DATA_ENABLE_BLEND          = 0x1
};

// PICA_REG_COLOR_OPERATION             0x100
#define PICA_CMD_DATA_COLOR_OPERATION(fragOpMode, blendMode) \
    ( (fragOpMode) | (blendMode) << 8 | 0x0e4 << 16 )

///////////////////////////////////
// Fragment Operation  shadow

// PICA_REG_FRAGOP_SHADOW          0x130
#define PICA_CMD_DATA_FRAGOP_SHADOW(penumbraScale, penumbraBias) \
    (( (penumbraScale) << 16) | (penumbraBias) )

///////////////////////////////////
// Fragment Operation Double buffer

// PICA_REG_FRAGOP_WSCALE          0x06d
#define PICA_CMD_DATA_FRAGOP_WSCALE(value) ((value == 0) ? 1 : 0)

// PICA_REG_FRAGOP_WSCALE_DATA1    0x04d
// PICA_REG_FRAGOP_WSCALE_DATA2    0x04e
#define PICA_CMD_DATA_FRAGOP_WSCALE_DATA(data) (data)

///////////////////////////////////
// Fragment Operation User Clipping

// PICA_REG_FRAGOP_CLIP            0x047

// 0x47 [0:0]
#define PICA_CMD_DATA_FRAGOP_CLIP(mode) ((mode) ? 1 : 0)

#define PICA_CMD_SET_FRAGOP_CLIP(mode) \
    PICA_CMD_DATA_FRAGOP_CLIP(mode),   \
    PICA_CMD_HEADER_SINGLE_BE(PICA_REG_FRAGOP_CLIP, 0x1)

// PICA_REG_FRAGOP_CLIP_DATA1      0x048
// PICA_REG_FRAGOP_CLIP_DATA2      0x049
// PICA_REG_FRAGOP_CLIP_DATA3      0x04a
// PICA_REG_FRAGOP_CLIP_DATA4      0x04b
#define PICA_CMD_DATA_FRAGOP_CLIP_DATA(data) (data)

/////////////////////////////////// 
// Fragment Operation AlphaTest

/*!
    @brief アルファテストの比較関数を設定をします。
           PICA_REG_FRAGOP_ALPHA_TEST レジスタ ( 0x104[6:4] ) で使います。
           dmp_FragOperation.alphaTestFunc ユニフォームに相当します。
*/
enum PicaDataAlphaTest
{
    //! GL_NEVER です。
    PICA_DATA_ALPHA_TEST_NEVER    = 0x0,
    //! GL_ALWAYS です。
    PICA_DATA_ALPHA_TEST_ALWAYS   = 0x1,
    //! GL_EQUAL です。
    PICA_DATA_ALPHA_TEST_EQUAL    = 0x2,
    //! GL_NOTEQUAL です。
    PICA_DATA_ALPHA_TEST_NOTEQUAL = 0x3,
    //! GL_LESS です。
    PICA_DATA_ALPHA_TEST_LESS     = 0x4,
    //! GL_LEQUAL です。
    PICA_DATA_ALPHA_TEST_LEQUAL   = 0x5,
    //! GL_GREATER です。
    PICA_DATA_ALPHA_TEST_GREATER  = 0x6,
    //! GL_GEQUAL です。
    PICA_DATA_ALPHA_TEST_GEQUAL   = 0x7
};

/*
struct CommandAlphaTest
{
    u32 alphaTestFunc   : 7;
    CMD_PADDING(1);
    u32 alphaRefValue   : 8; // [0-255]
    CMD_PADDING(16);
};
*/

#define PICA_CMD_DATA_FRAGOP_ALPHA_TEST_DISABLE() 0x0
    
// PICA_REG_FRAGOP_ALPHA_TEST      0x104
#define PICA_CMD_DATA_FRAGOP_ALPHA_TEST( enable, func, value ) \
    ( ((enable) ? 1 : 0) | \
       (func)  << 4      | \
       (value) << 8 )

#define PICA_CMD_SET_DISABLE_ALPHA_TEST()      \
    PICA_CMD_DATA_FRAGOP_ALPHA_TEST_DISABLE(), \
    PICA_CMD_HEADER_SINGLE_BE( PICA_REG_FRAGOP_ALPHA_TEST, 0x1 )

#define PICA_CMD_SET_ENABLE_ALPHA_TEST(func, value)    \
    PICA_CMD_DATA_FRAGOP_ALPHA_TEST(0x1, func, value), \
    PICA_CMD_HEADER_SINGLE_BE( PICA_REG_FRAGOP_ALPHA_TEST, 0x3 )

///////////////////////////////////
// Frame Buffer Access Control

// PICA_REG_FRAME_BUFFER_MODE1      0x112
// PICA_REG_FRAME_BUFFER_MODE2      0x113
// PICA_REG_FRAME_BUFFER_MODE3      0x114
// PICA_REG_FRAME_BUFFER_MODE4      0x115

#define PICA_CMD_DATA_FRAME_BUFFER_MODE(mode) (mode)



///////////////////////////////////
// Viewport Position Setting

// PICA_REG_VIEWPORT_WIDTH1         0x041
// PICA_REG_VIEWPORT_WIDTH2         0x042
#define PICA_CMD_DATA_VIEWPORT_WIDTH(width)   (width)

// PICA_REG_VIEWPORT_HEIGHT1        0x043
// PICA_REG_VIEWPORT_HEIGHT2        0x044
#define PICA_CMD_DATA_VIEWPORT_HEIGHT(height) (height)

/*
struct CommandViewportXY
{
    u32 x : 10;
    CMD_PADDING(6);
    u32 y : 10;
    CMD_PADDING(6);
};
*/
// PICA_REG_VIEWPORT_XY             0x068
#define PICA_CMD_DATA_VIEWPORT_XY(x, y)  ( (x) | (y) << 16 )


/////////////////////////////////// 
// Depth test
// Color Mask

/*!
    @brief デプステストの比較関数を設定します。
           PICA_REG_DEPTH_COLOR_MASK レジスタ ( 0x107[6:4] ) で使います。
           glDepthFunc() の引数 func に相当します。
*/
enum PicaDataDepthTest
{
    //! GL_NEVER です。
    PICA_DATA_DEPTH_TEST_NEVER    = 0x0,
    //! GL_ALWAYS です。
    PICA_DATA_DEPTH_TEST_ALWAYS   = 0x1,
    //! GL_EQUAL です。
    PICA_DATA_DEPTH_TEST_EQUAL    = 0x2,
    //! GL_NOTEQUAL です。
    PICA_DATA_DEPTH_TEST_NOTEQUAL = 0x3,
    //! GL_LESS です。
    PICA_DATA_DEPTH_TEST_LESS     = 0x4,
    //! GL_LEQUAL です。
    PICA_DATA_DEPTH_TEST_LEQUAL   = 0x5,
    //! GL_GREATER です。
    PICA_DATA_DEPTH_TEST_GREATER  = 0x6,
    //! GL_GEQUAL です。
    PICA_DATA_DEPTH_TEST_GEQUAL   = 0x7
};

/*
struct CommandDepthColorMask
{
    u32 depthFunc   : 7;
    CMD_PADDING(1);
    u32 red   : 1;
    u32 green : 1;
    u32 blue  : 1;
    u32 alpha : 1;
    u32 depthMask   : 1;
    CMD_PADDING(19);
};
*/

#define PICA_CMD_DATA_DEPTH_TEST_DISABLE() 0x0

// PICA_REG_DEPTH_COLOR_MASK          0x107
#define PICA_CMD_DATA_DEPTH_COLOR_MASK( enableDepthTest, depthFunc,          \
                                        red, green, blue, alpha, depthMask ) \
    ( ((enableDepthTest) ? 1 : 0) | \
      (depthFunc << 4)            | \
      ((red)   ? 0x100 : 0)       | \
      ((green) ? 0x200 : 0)       | \
      ((blue)  ? 0x400 : 0)       | \
      ((alpha) ? 0x800 : 0)       | \
      ((depthMask) ? 0x1000 : 0) )


/////////////////////////////////// 
// Blend / LogicOp

// Blend Equation

/*!
    @brief ブレンドのモードを設定します。
           PICA_REG_DEPTH_COLOR_MASK レジスタ ( 0x101[2:0] ) で使います。
           glBlendEquation(), glBlendEquationSeparate() に相当します。
*/
enum PicaDataBlendEquation
{
    //! GL_FUNC_ADD です。
    PICA_DATA_BLEND_EQUATION_ADD                = 0,
    //! GL_FUNC_SUBTRACT です。
    PICA_DATA_BLEND_EQUATION_SUBTRACT           = 1,
    //! GL_FUNC_REVERSE_SUBTRACT です。
    PICA_DATA_BLEND_EQUATION_REVERSE_SUBTRACT   = 2,
    //! GL_MIN です。
    PICA_DATA_BLEND_EQUATION_MIN                = 3,
    //! GL_MAX です。
    PICA_DATA_BLEND_EQUATION_MAX                = 4
};

/*!
    @brief ブレンドの関数を設定します。
           PICA_REG_DEPTH_COLOR_MASK レジスタ( 0x101[19:16] ) で使います。
           glBlendFunc(), glBlendFuncSeparate() の引数に相当します。
*/
enum PicaDataBlendFunc
{
    //! GL_ZERO です。
    PICA_DATA_BLEND_FUNC_ZERO                     =  0,
    //! GL_ONE です。
    PICA_DATA_BLEND_FUNC_ONE                      =  1,
    //! GL_SRC_COLOR です。
    PICA_DATA_BLEND_FUNC_SRC_COLOR                =  2,
    //! GL_ONE_MINUS_SRC_COLOR です。
    PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_COLOR      =  3,
    //! GL_DST_COLOR です。
    PICA_DATA_BLEND_FUNC_DST_COLOR                =  4,
    //! GL_ONE_MINUS_DST_COLOR です。
    PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_COLOR      =  5,
    //! GL_SRC_ALPHA です。
    PICA_DATA_BLEND_FUNC_SRC_ALPHA                =  6,
    //! GL_ONE_MINUS_SRC_ALPHA です。
    PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA      =  7,
    //! GL_DST_ALPHA です。
    PICA_DATA_BLEND_FUNC_DST_ALPHA                =  8,
    //! GL_ONE_MINUS_DST_ALPHA です。
    PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_ALPHA      =  9,
    //! GL_CONSTANT_COLOR です。
    PICA_DATA_BLEND_FUNC_CONSTANT_COLOR           = 10,
    //! GL_ONE_MINUS_CONSTANT_COLOR です。
    PICA_DATA_BLEND_FUNC_ONE_MINUS_CONSTANT_COLOR = 11,
    //! GL_CONSTANT_ALPHA です。
    PICA_DATA_BLEND_FUNC_CONSTANT_ALPHA           = 12,
    //! GL_ONE_MINUS_CONSTANT_ALPHA です。
    PICA_DATA_BLEND_FUNC_ONE_MINUS_CONSTANT_ALPHA = 13,
    //! GL_SRC_ALPHA_SATURATE です。
    PICA_DATA_BLEND_FUNC_SRC_ALPHA_SATURATE       = 14
};

// PICA_REG_BLEND_FUNC               0x101
#define PICA_CMD_DATA_BLEND_FUNC_SEPARATE(eqRgb, eqAlpha, srcRgb, dstRgb, srcAlpha, dstAlpha) \
    ((eqRgb) | (eqAlpha) << 8 | (srcRgb) << 16 | (dstRgb) << 20 | (srcAlpha) << 24 | static_cast<u32>(dstAlpha) << 28)

#define PICA_CMD_DATA_BLEND_FUNC( eq, src, dst) \
    PICA_CMD_DATA_BLEND_FUNC_SEPARATE(eq, eq, src, dst, src, dst)

#define PICA_CMD_SET_BLEND_FUNC( eq, src, dst ) \
    PICA_CMD_DATA_COLOR_OPERATION( PICA_DATA_FRAGOP_MODE, PICA_DATA_ENABLE_BLEND ), \
    PICA_CMD_HEADER_SINGLE( PICA_REG_COLOR_OPERATION ), \
    PICA_CMD_DATA_BLEND_FUNC( eq, src, dst ), \
    PICA_CMD_HEADER_SINGLE( PICA_REG_BLEND_FUNC )

#define PICA_CMD_SET_BLEND_FUNC_SEPARATE( eqRgb, eqAlpha, srcRgb, dstRgb, srcAlpha, dstAlpha) \
    PICA_CMD_DATA_COLOR_OPERATION( PICA_DATA_FRAGOP_MODE, PICA_DATA_ENABLE_BLEND ), \
    PICA_CMD_HEADER_SINGLE( PICA_REG_COLOR_OPERATION ), \
    PICA_CMD_DATA_BLEND_FUNC_SEPARATE( eqRgb, eqAlpha, srcRgb, dstRgb, srcAlpha, dstAlpha ),  \
    PICA_CMD_HEADER_SINGLE( PICA_REG_BLEND_FUNC )

#define PICA_CMD_SET_BLEND_DEFAULT() \
    PICA_CMD_SET_BLEND_FUNC( PICA_DATA_BLEND_EQUATION_ADD, PICA_DATA_BLEND_FUNC_SRC_ALPHA, PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA )

#define PICA_CMD_SET_BLEND_NOTHING() \
    PICA_CMD_SET_BLEND_FUNC( PICA_DATA_BLEND_EQUATION_ADD, PICA_DATA_BLEND_FUNC_ONE, PICA_DATA_BLEND_FUNC_ZERO )


/*!
    @brief 論理演算を設定します。
           PICA_REG_LOGIC_OP レジスタ( 0x102[3:0] ) で使います。
           glLogicOp() の引数 opcode に相当します。
*/
enum PicaDataLogicOp
{
    //! GL_CLEAR です。
    PICA_DATA_LOGIC_CLEAR                   = 0x0,
    //! GL_AND です。
    PICA_DATA_LOGIC_AND                     = 0x1,
    //! GL_AND_REVERSE です。
    PICA_DATA_LOGIC_AND_REVERSE             = 0x2,
    //! GL_COPY です。
    PICA_DATA_LOGIC_COPY                    = 0x3,
    //! GL_SET です。
    PICA_DATA_LOGIC_SET                     = 0x4,
    //! GL_COPY_INVERTED です。    
    PICA_DATA_LOGIC_COPY_INVERTED           = 0x5,
    //! GL_NOOP です。
    PICA_DATA_LOGIC_NOOP                    = 0x6,
    //! GL_INVERT です。
    PICA_DATA_LOGIC_INVERT                  = 0x7,
    //! GL_NAND です。
    PICA_DATA_LOGIC_NAND                    = 0x8,
    //! GL_OR です。
    PICA_DATA_LOGIC_OR                      = 0x9,
    //! GL_NOR です。
    PICA_DATA_LOGIC_NOR                     = 0xa,
    //! GL_XOR です。
    PICA_DATA_LOGIC_XOR                     = 0xb,
    //! GL_EQUIV です。
    PICA_DATA_LOGIC_EQUIV                   = 0xc,
    //! GL_AND_INVERTED です。
    PICA_DATA_LOGIC_AND_INVERTED            = 0xd,
    //! GL_OR_REVERSE です。
    PICA_DATA_LOGIC_OR_REVERSE              = 0xe,
    //! GL_OR_INVERTED です。
    PICA_DATA_LOGIC_OR_INVERTED             = 0xf
};

#define PICA_CMD_DATA_LOGIC_OP( logicOp ) (logicOp)

// 0x101
// [31:28] = 0x0, [27:24] = 0x1, [23:20] = 0x0, [19:16] = 0x1
#define PICA_CMD_DATA_LOGIC_OP_ENABLE() \
    ( (0x1 << 16) | (0x1 << 24) )
    
#define PICA_CMD_SET_LOGIC_OP( logicOp ) \
    PICA_CMD_DATA_COLOR_OPERATION( PICA_DATA_FRAGOP_MODE, PICA_DATA_ENABLE_COLOR_LOGIC_OP ), \
    PICA_CMD_HEADER_SINGLE( PICA_REG_COLOR_OPERATION ), \
    PICA_CMD_DATA_LOGIC_OP(logicOp), PICA_CMD_HEADER_SINGLE( PICA_REG_LOGIC_OP )

// PICA_REG_BLEND_COLOR             0x103
#define PICA_CMD_DATA_BLEND_COLOR( color8 ) \
    ( (color8.r) | (color8.g) << 8 | (color8.b) << 16 | (color8.a) << 24 )

/////////////////////////////////// 
// Early Depth Test

// PICA_REG_EARLY_DEPTH_TEST1       0x062
// PICA_REG_EARLY_DEPTH_TEST2       0x118
#define PICA_CMD_DATA_EARLY_DEPTH_TEST( mode ) ((mode) ? 1 : 0)

#define PICA_CMD_DATA_EARLY_DEPTH_TEST_DISABLE() 0x0

#define PICA_CMD_SET_DISABLE_EARLY_DEPTH_TEST() \
    PICA_CMD_DATA_EARLY_DEPTH_TEST_DISABLE(), PICA_CMD_HEADER_SINGLE_BE( PICA_REG_EARLY_DEPTH_TEST1, 0x1), \
    PICA_CMD_DATA_EARLY_DEPTH_TEST_DISABLE(), PICA_CMD_HEADER_SINGLE( PICA_REG_EARLY_DEPTH_TEST2 )

/*!
    @brief アーリーデプステストの比較関数を設定します。
           PICA_REG_EARLY_DEPTH_FUNC レジスタ( 0x061[1:0] ) で使います。
           glEarlyDepthFuncDMP() に相当します。
*/
enum PicaDataEarlyDepth
{
    //! GL_GEQUAL です。
    PICA_DATA_EARLY_DEPTH_GEQUAL  = 0x0,
    //! GL_GREATER です。
    PICA_DATA_EARLY_DEPTH_GREATER = 0x1,
    //! GL_LEQUAL です。
    PICA_DATA_EARLY_DEPTH_LEQUAL  = 0x2,
    //! GL_LESS です。
    PICA_DATA_EARLY_DEPTH_LESS    = 0x3
};

// PICA_REG_EARLY_DEPTH_FUNC        0x061
#define PICA_CMD_DATA_EARLY_DEPTH_FUNC( func ) (func)

#define PICA_CMD_SET_EARLY_DEPTH_FUNC(func) \
    PICA_CMD_DATA_EARLY_DEPTH_FUNC(func), PICA_CMD_HEADER_SINGLE_BE( PICA_REG_EARLY_DEPTH_FUNC, 0x1)    

// PICA_REG_EARLY_DEPTH_DATA        0x06a
#define PICA_CMD_DATA_EARLY_DEPTH_DATA( depth ) (depth)

#define PICA_CMD_SET_EARLY_DEPTH_DATA(data) \
    PICA_CMD_DATA_EARLY_DEPTH_DATA(data), PICA_CMD_HEADER_SINGLE_BE( PICA_REG_EARLY_DEPTH_DATA, 0x7)



/////////////////////////////////// 
// Stencil Test

/*!
    @brief ステンシルテストの比較関数を設定します。
           PICA_REG_STENCIL_TEST レジスタ( 0x105[6:4] ) で使います。
           glStencilFunc() の引数 func に相当します。
*/
enum PicaDataStencilTest
{
    //! GL_NEVER です。
    PICA_DATA_STENCIL_TEST_NEVER    = 0x0,
    //! GL_ALWAYS です。
    PICA_DATA_STENCIL_TEST_ALWAYS   = 0x1,
    //! GL_EQUAL です。
    PICA_DATA_STENCIL_TEST_EQUAL    = 0x2,
    //! GL_NOTEQUAL です。
    PICA_DATA_STENCIL_TEST_NOTEQUAL = 0x3,
    //! GL_LESS です。
    PICA_DATA_STENCIL_TEST_LESS     = 0x4,
    //! GL_LEQUAL です。
    PICA_DATA_STENCIL_TEST_LEQUAL   = 0x5,
    //! GL_GREATER です。
    PICA_DATA_STENCIL_TEST_GREATER  = 0x6,
    //! GL_GEQUAL です。
    PICA_DATA_STENCIL_TEST_GEQUAL   = 0x7
};

/*
struct CommandStencilTest
{
    u32 glStencilFunc : 7;
    CMD_PADDING(1);
    u32 glStencilMask : 8; 
    u32 ref           : 8
    u32 mask          : 8;
};
*/

// PICA_REG_STENCIL_TEST            0x105
#define PICA_CMD_DATA_STENCIL_TEST( enableStencilTest, stencilFunc, stencilMask, ref, mask) \
    ( ((enableStencilTest) ? 1 : 0 ) | \
      (stencilFunc) << 4             | \
      (stencilMask) << 8             | \
      (ref) << 16                    | \
      (mask) << 24 )

#define PICA_CMD_DATA_STENCIL_TEST_DISABLE() 0x0

#define PICA_CMD_SET_DISABLE_STENCIL_TEST()\
    PICA_CMD_DATA_STENCIL_TEST_DISABLE(), PICA_CMD_HEADER_SINGLE( PICA_REG_STENCIL_TEST )

/*!
    @brief ステンシルバッファの変更方法を設定します。
           PICA_REG_STENCIL_OP レジスタ( 0x106[2:0], [6:4], [10:8] ) で使います。
           glStencilOp() の引数 func, zfail, zpass に相当します。
*/
enum PicaDataStencilOp
{
    //! GL_KEEP です。
    PICA_DATA_STENCIL_OP_KEEP      = 0x0,
    //! GL_ZERO です。
    PICA_DATA_STENCIL_OP_ZERO      = 0x1,
    //! GL_REPLACE です。
    PICA_DATA_STENCIL_OP_REPLACE   = 0x2,
    //! GL_INCR です。
    PICA_DATA_STENCIL_OP_INCR      = 0x3,
    //! GL_DECR です。
    PICA_DATA_STENCIL_OP_DECR      = 0x4,
    //! GL_INVERT です。
    PICA_DATA_STENCIL_OP_INVERT    = 0x5,
    //! GL_INCR_WRAP です。
    PICA_DATA_STENCIL_OP_INCR_WRAP = 0x6,
    //! GL_DECR_WRAP です。
    PICA_DATA_STENCIL_OP_DECR_WRAP = 0x7
};

/*
struct CommandStencilOp
{
    u32 fail  : 3;
    CMD_PADDING(1);
    u32 zfail : 3; 
    CMD_PADDING(1);
    u32 zpass : 3
    CMD_PADDING(21);
};
*/

// PICA_REG_STENCIL_OP              0x106
#define PICA_CMD_DATA_STENCIL_OP( fail, zfail, zpass) \
    ( (fail)       | \
      (zfail) << 4 | \
      (zpass) << 8 )

/////////////////////////////////// 
// Cull Face

// PICA_REG_CULL_FACE               0x040 [1:0]
#define PICA_CMD_DATA_CULL_FACE( mode ) (mode)

#define PICA_CMD_SET_CULL_FACE( mode ) \
    PICA_CMD_DATA_CULL_FACE( mode ) , PICA_CMD_HEADER_SINGLE( PICA_REG_CULL_FACE )

/////////////////////////////////// 
// Scissor

// PICA_REG_SCISSOR                 0x065 [1:0]
#define PICA_CMD_DATA_SCISSOR( mode ) ((mode) ? 3 : 0)

/*
struct CommandScissorSize
{
    u32 size1  : 10;
    CMD_PADDING(6);
    u32 size2  : 10;
    CMD_PADDING(6);
};
*/

// PICA_REG_SCISSOR_XY              0x066
#define PICA_CMD_DATA_SCISSOR_XY( x, y, colorBufferWidth, colorBufferHeight) \
    ( ((x) < 0) ? 0 : ( (x >= colorBufferWidth)  ? (colorBufferWidth - 1)  : (x) ) ) | \
    ( ((y) < 0) ? 0 : ( (y >= colorBufferHeight) ? (colorBufferHeight - 1) : (y) ) << 16 )
        

// PICA_REG_SCISSOR_SIZE            0x067
#define PICA_CMD_DATA_SCISSOR_SIZE( x, y ) \
    ( ((x) < 0) ? 0 : (x) | \
    ( ((y) < 0) ? 0 : (y)) << 16 )

/////////////////////////////////// 
// Render Block Format


/*!
    @brief レンダリングのブロックフォーマットを設定します。
           PICA_REG_RENDER_BLOCK_FORMAT レジスタ( 0x11b[0:0] ) で使います。
           glRenderBlockModeDMP() に相当します。
*/
enum PicaDataRenderBlockFormat
{
    //! カラーバッファを 8 ブロックフォーマットに設定します。
    PICA_DATA_RENDER_BLOCK8_MODE_DMP  = 0x0,
    //! カラーバッファを 32 ブロックフォーマットに設定します。
    PICA_DATA_RENDER_BLOCK32_MODE_DMP = 0x1
};

// PICA_REG_RENDER_BLOCK_FORMAT     0x11b
#define PICA_CMD_DATA_RENDER_BLOCK_FORMAT( mode ) ( (mode) ? 1 : 0 )

/////////////////////////////////// 
// Appendex registers

// PICA_REG_COLOR_BUFFER_CLEAR0     0x110   Append: clear buffer register
// PICA_REG_COLOR_BUFFER_CLEAR1     0x111   Append: clear buffer register

#define PICA_CMD_DATA_COLOR_DEPTH_BUFFER_CLEAR( data ) \
    ( (data) ? 1 : 0 )

#define PICA_CMD_SET_COLOR_DEPTH_BUFFER_CLEAR( data1, data2 ) \
    PICA_CMD_DATA_COLOR_DEPTH_BUFFER_CLEAR(data1), PICA_CMD_HEADER_SINGLE(PICA_REG_COLOR_DEPTH_BUFFER_CLEAR1), \
    PICA_CMD_DATA_COLOR_DEPTH_BUFFER_CLEAR(data2), PICA_CMD_HEADER_SINGLE(PICA_REG_COLOR_DEPTH_BUFFER_CLEAR0)

/*!
  @}
*/

#endif // NN_GX_CTR_GX_MACRO_MISC_H_

