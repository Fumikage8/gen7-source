#ifndef __GFL_G3DH3DEX_H__
#define __GFL_G3DH3DEX_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dEx.h
 *	@brief  H3D追加
 *	@author	Koji Kawada
 *	@date		2012.08.24
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#if 0
//説明
//公式のH3Dに追加して欲しいようなクラスを扱います。
#endif


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>
#include <gfl_fs.h>

// gflib grp
#include <grp/gfl_GraphicsType.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>

#include <grp/util/gfl_UtilDrawUtil.h>



#if GFL_GRP_G3D_H3D_USE

// NW4C
#include <nw/h3d.h>
// gflib grp g3dh3d



namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3D追加定数クラス
//=====================================

// TextureSampler
struct H3dTextureSamplerExConst
{
  // PICAコマンド生成にそのまま使用しているので値を変えないように。
  // テクスチャマッパー  サンプラータイプ
  enum SamplerType
  {
    SAMPLER_TYPE_TEXTURE_2D  = PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_2D             ,  // テクスチャ座標です。 
    SAMPLER_TYPE_CUBE_MAP    = PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_CUBE_MAP       ,  // カメラキューブ座標です。 
    SAMPLER_TYPE_SHADOW      = PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_SHADOW_2D_DMP  ,  // シャドウ用の投影です。 
    SAMPLER_TYPE_PROJECTION  = PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_PROJECTION_DMP ,  // 投影します。   
    SAMPLER_TYPE_SHADOW_CUBE = PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_SHADOW_CUBE_DMP,  // シャドウキューブ用の投影です。
    SAMPLER_TYPE_FALSE       = PICA_DATA_TEXTURE0_SAMPLER_TYPE_FALSE                  ,  // FALSE
  };
  enum SamplerTypeEnable
  {
    SAMPLER_TYPE_ENABLE_FALSE      = PICA_DATA_TEXTURE1_SAMPLER_TYPE_FALSE,      // TEXTURE1もTEXTURE2も同じ値なので、TEXTURE1のほうを書いておく。
    SAMPLER_TYPE_ENABLE_TEXTURE_2D = PICA_DATA_TEXTURE1_SAMPLER_TYPE_TEXTURE_2D,
  };

  // PICAコマンド生成にそのまま使用しているので値を変えないように。
  // テクスチャマッパー  ラップ設定(リピート設定)
  enum Wrap
  {
    WRAP_CLAMP_TO_EDGE   = PICA_DATA_TEXTURE_WRAP_CLAMP_TO_EDGE  ,  // テクスチャの端のピクセルを引き伸ばします。 
    WRAP_CLAMP_TO_BORDER = PICA_DATA_TEXTURE_WRAP_CLAMP_TO_BORDER,  // テクスチャの端でボーダーカラーを引き伸ばします。
    WRAP_REPEAT          = PICA_DATA_TEXTURE_WRAP_REPEAT         ,  // テクスチャの端で繰り返します。 
    WRAP_MIRRORED_REPEAT = PICA_DATA_TEXTURE_WRAP_MIRRORED_REPEAT,  // テクスチャの端で反転しながら繰り返します。 
  };

  // PICAコマンド生成にそのまま使用しているので値を変えないように。
  // テクスチャマッパー  拡大時フィルタ
  enum MagFilter
  {
    MAG_FILTER_NEAREST = PICA_DATA_TEXTURE_MAG_FILTER_NEAREST,  // ニアレスト(補間なし)です。
    MAG_FILTER_LINEAR  = PICA_DATA_TEXTURE_MAG_FILTER_LINEAR ,  // リニア(補間あり)です。
  };

  // PICAコマンド生成にそのまま使用しているので値を変えないように。
  // テクスチャマッパー  縮小時フィルタ
  enum MinFilter
  {
    MIN_FILTER_NEAREST                = PICA_DATA_TEXTURE_MIN_FILTER_NEAREST                ,  // ニアレスト(補間なし)で、ミップマップは使用しません。 
    MIN_FILTER_NEAREST_MIPMAP_NEAREST = PICA_DATA_TEXTURE_MIN_FILTER_NEAREST_MIPMAP_NEAREST ,  // ニアレスト(補間なし)で、ミップマップもニアレスト(補間なし)です。
    MIN_FILTER_NEAREST_MIPMAP_LINEAR  = PICA_DATA_TEXTURE_MIN_FILTER_NEAREST_MIPMAP_LINEAR  ,  // ニアレスト(補間なし)で、ミップマップはリニア(補間あり)です。 
    MIN_FILTER_LINEAR                 = PICA_DATA_TEXTURE_MIN_FILTER_LINEAR                 ,  // リニア(補間あり)で、ミップマップは使用しません。 
    MIN_FILTER_LINEAR_MIPMAP_NEAREST  = PICA_DATA_TEXTURE_MIN_FILTER_LINEAR_MIPMAP_NEAREST  ,  // リニア(補間あり)で、ミップマップはニアレスト(補間なし)です。 
    MIN_FILTER_LINEAR_MIPMAP_LINEAR   = PICA_DATA_TEXTURE_MIN_FILTER_LINEAR_MIPMAP_LINEAR   ,  // リニア(補間あり)で、ミップマップもリニア(補間あり)です。      
  };
  enum MinFilterDown  // 2:2
  {
    MIN_FILTER_DOWN_0 = 0,
    MIN_FILTER_DOWN_1 = 1,
  };
  enum MinFilterUp  // 24:24
  {
    MIN_FILTER_UP_0 = 0,
    MIN_FILTER_UP_1 = 1,
  };
  // Up Down Mipmap 組合せ結果
  // 0  0    0      MIN_FILTER_NEAREST
  // 0  1    0      MIN_FILTER_LINEAR
  // 0  0    1      MIN_FILTER_NEAREST_MIPMAP_NEAREST
  // 0  1    1      MIN_FILTER_LINEAR_MIPMAP_NEAREST
  // 1  0    *      MIN_FILTER_NEAREST_MIPMAP_LINEAR
  // 1  1    *      MIN_FILTER_LINEAR_MIPMAP_LINEAR

  //MinLodLevelもここ。MaxLodLevelも追加してもいいかも。enumなしで数字でいいかな。
  //LodBiasもここ。enumなしで数字でいいかな。
  
  enum
  {
    TEXTURE_NUM = nw::h3d::Constants::PIXELBASED_TEXTURE_UNIT_QUANTITY,  // = 3  // テクスチャ数(テクスチャマッパー数である、テクスチャサンプラー数である)(テクスチャコーディネータ数ではない)
                                                                                 // 0,1,2,ProcだがProcは除く
  };
};



// TextureCombiner
struct H3dTextureCombinerExConst
{
  // PICAコマンド生成にそのまま使用しているので値を変えないように。
  enum Combine
  {
    COMBINE_REPLACE      = PICA_DATA_TEX_ENV_COMBINE_REPLACE     ,    //!< A です
    COMBINE_MODULATE     = PICA_DATA_TEX_ENV_COMBINE_MODULATE    ,    //!< A * B です。
    COMBINE_ADD          = PICA_DATA_TEX_ENV_COMBINE_ADD         ,    //!< A + B です。
    COMBINE_ADDSIGNED    = PICA_DATA_TEX_ENV_COMBINE_ADD_SIGNED  ,    //!< A + B - 0.5 です。
    COMBINE_INTERPOLATE  = PICA_DATA_TEX_ENV_COMBINE_INTERPOLATE ,    //!< A * C + B * ( 1 - C ) です。
    COMBINE_SUBTRACT     = PICA_DATA_TEX_ENV_COMBINE_SUBTRACT    ,    //!< A - B です。
    COMBINE_DOT3_RGB     = PICA_DATA_TEX_ENV_COMBINE_DOT3_RGB    ,    //!< RGB ← Dot ( A , B ) です。
    COMBINE_DOT3_RGBA    = PICA_DATA_TEX_ENV_COMBINE_DOT3_RGBA   ,    //!< RGBA ← Dot ( A , B ) です。
    COMBINE_MULT_ADD     = PICA_DATA_TEX_ENV_COMBINE_MULT_ADD_DMP,    //!< ( A * B ) + C です。
    COMBINE_ADD_MULT     = PICA_DATA_TEX_ENV_COMBINE_ADD_MULT_DMP     //!< ( A + B ) * C です。
  };

  // PICAコマンド生成にそのまま使用しているので値を変えないように。
  enum Scale
  {
    SCALE_ONE   = PICA_DATA_TEX_ENV_SCALE_1,    //!< 1.0 倍。
    SCALE_TWO   = PICA_DATA_TEX_ENV_SCALE_2,    //!< 2.0 倍。
    SCALE_FOUR  = PICA_DATA_TEX_ENV_SCALE_4     //!< 4.0 倍。
  };

  // PICAコマンド生成にそのまま使用しているので値を変えないように。
  enum OperandRgb
  {
    OPERANDRGB_SRC_COLOR            = PICA_DATA_OPE_RGB_SRC_COLOR          ,     //!< RGB です。
    OPERANDRGB_ONE_MINUS_SRC_COLOR  = PICA_DATA_OPE_RGB_ONE_MINUS_SRC_COLOR,     //!< 1 - RGB です。
    OPERANDRGB_SRC_ALPHA            = PICA_DATA_OPE_RGB_SRC_ALPHA          ,     //!< アルファです。
    OPERANDRGB_ONE_MINUS_SRC_ALPHA  = PICA_DATA_OPE_RGB_ONE_MINUS_SRC_ALPHA,     //!< 1 - アルファです。
    OPERANDRGB_RED                  = PICA_DATA_OPE_RGB_SRC_R_DMP          ,     //!< R 成分です。
    OPERANDRGB_ONE_MINUS_RED        = PICA_DATA_OPE_RGB_ONE_MINUS_SRC_R_DMP,     //!< 1 - R 成分です。
    OPERANDRGB_GREEN                = PICA_DATA_OPE_RGB_SRC_G_DMP          ,     //!< G 成分です。
    OPERANDRGB_ONE_MINUS_GREEN      = PICA_DATA_OPE_RGB_ONE_MINUS_SRC_G_DMP,     //!< 1 - G 成分です。
    OPERANDRGB_BLUE                 = PICA_DATA_OPE_RGB_SRC_B_DMP          ,     //!< B 成分です。
    OPERANDRGB_ONE_MINUS_BLUE       = PICA_DATA_OPE_RGB_ONE_MINUS_SRC_B_DMP      //!< 1 - B 成分です。
  };

  // PICAコマンド生成にそのまま使用しているので値を変えないように。
  enum OperandAlpha
  {
    OPERANDALPHA_SRC_ALPHA            = PICA_DATA_OPE_ALPHA_SRC_ALPHA          ,   //!< アルファです。
    OPERANDALPHA_SRC_ONE_MINUS_ALPHA  = PICA_DATA_OPE_ALPHA_ONE_MINUS_SRC_ALPHA,   //!< 1 - アルファです 
    OPERANDALPHA_RED                  = PICA_DATA_OPE_ALPHA_SRC_R_DMP          ,   //!< R 成分です。
    OPERANDALPHA_ONE_MINUS_RED        = PICA_DATA_OPE_ALPHA_ONE_MINUS_SRC_R_DMP,   //!< 1 - R 成分です。
    OPERANDALPHA_GREEN                = PICA_DATA_OPE_ALPHA_SRC_G_DMP          ,   //!< G 成分です。
    OPERANDALPHA_ONE_MINUS_GREEN      = PICA_DATA_OPE_ALPHA_ONE_MINUS_SRC_G_DMP,   //!< 1 - G 成分です。
    OPERANDALPHA_BLUE                 = PICA_DATA_OPE_ALPHA_SRC_B_DMP          ,   //!< B 成分です。
    OPERANDALPHA_ONE_MINUS_BLUE       = PICA_DATA_OPE_ALPHA_ONE_MINUS_SRC_B_DMP    //!< 1 - B 成分です。
  };

  // PICAコマンド生成にそのまま使用しているので値を変えないように。
  enum Source
  {
    SOURCE_TEXTURE0                  = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0                    ,    //!< テクスチャ 0 です。
    SOURCE_TEXTURE1                  = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE1                    ,    //!< テクスチャ 1 です。
    SOURCE_TEXTURE2                  = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE2                    ,    //!< テクスチャ 2 です。
    SOURCE_TEXTURE3                  = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE3                    ,    //!< テクスチャ 3 です。
    SOURCE_CONSTANT                  = PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT                    ,    //!< コンスタントカラーです。
    SOURCE_PRIMARY_COLOR             = PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR               ,    //!< 頂点シェーダーの出力結果です。
    SOURCE_FRAGMENT_PRIMARY_COLOR    = PICA_DATA_TEX_ENV_SRC_RGBA_FRAGMENT_PRIMARY_COLOR_DMP  ,    //!< プライマリカラーです。
    SOURCE_FRAGMENT_SECONDARY_COLOR  = PICA_DATA_TEX_ENV_SRC_RGBA_FRAGMENT_SECONDARY_COLOR_DMP,    //!< セカンダリカラーです。
    SOURCE_PREVIOUS                  = PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS                    ,    //!< 前段の出力結果です。
    SOURCE_PREVIOUS_BUFFER           = PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS_BUFFER_DMP              //!< 前段のバッファです。
  };

  enum
  {
    STEP_NUM    = 6,  // テクスチャコンバイナの段数
    SOURCE_NUM  = 3   // テクスチャコンバイナ1段におけるソース数
  };
};

// FragmentShader
struct H3dFragmentShaderExConst
{
  // PICAコマンド生成にそのまま使用しているので値を変えないように。
  enum BufferInput
  {
    BUFFERINPUT_PREVIOUS_BUFFER  = PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS_BUFFER_DMP,   //!< 前段のバッファです。
    BUFFERINPUT_PREVIOUS         = PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS               //!< 前段の出力結果です。
  };
};

// StencilOperation
struct H3dStencilOperationExConst
{
  // PICAコマンド生成にそのまま使用しているので値を変えないように。
  enum TestFunc
  {
    TESTFUNC_NEVER     = PICA_DATA_STENCIL_TEST_NEVER   ,  //!< すべて不通過です。 
    TESTFUNC_ALWAYS    = PICA_DATA_STENCIL_TEST_ALWAYS  ,  //!< すべて通過です。 
    TESTFUNC_EQUAL     = PICA_DATA_STENCIL_TEST_EQUAL   ,  //!< 参照値と等しければ通過します。 
    TESTFUNC_NOTEQUAL  = PICA_DATA_STENCIL_TEST_NOTEQUAL,  //!< 参照値と等しくなければ通過します。 
    TESTFUNC_LESS      = PICA_DATA_STENCIL_TEST_LESS    ,  //!< 参照値より小さければ通過します。 
    TESTFUNC_LEQUAL    = PICA_DATA_STENCIL_TEST_LEQUAL  ,  //!< 参照値以下であれば通過します。 
    TESTFUNC_GREATER   = PICA_DATA_STENCIL_TEST_GREATER ,  //!< 参照値より大きければ通過します。 
    TESTFUNC_GEQUAL    = PICA_DATA_STENCIL_TEST_GEQUAL  ,  //!< 参照値以上であれば通過します。 
  };

  // PICAコマンド生成にそのまま使用しているので値を変えないように。
  enum StencilOp
  {
    STENCILOP_KEEP           = PICA_DATA_STENCIL_OP_KEEP     ,   //!< 現在の値を保持します。 
    STENCILOP_ZERO           = PICA_DATA_STENCIL_OP_ZERO     ,   //!< 0 に置換します。 
    STENCILOP_REPLACE        = PICA_DATA_STENCIL_OP_REPLACE  ,   //!< 参照値に置換します。 
    STENCILOP_INCREASE       = PICA_DATA_STENCIL_OP_INCR     ,   //!< 増加させます。極値処理はクランプです。 
    STENCILOP_DECREASE       = PICA_DATA_STENCIL_OP_DECR     ,   //!< 減少させます。極値処理はクランプです。 
    STENCILOP_INVERT         = PICA_DATA_STENCIL_OP_INVERT   ,   //!< ビット単位で反転させます。 
    STENCILOP_INCREASE_WRAP  = PICA_DATA_STENCIL_OP_INCR_WRAP,   //!< 増加させます。極値処理は回り込みです。 
    STENCILOP_DECREASE_WRAP  = PICA_DATA_STENCIL_OP_DECR_WRAP,   //!< 減少させます。極値処理は回り込みです。 
  };
};

// AlphaTest
struct H3dAlphaTestExConst
{
  // PICAコマンド生成にそのまま使用しているので値を変えないように。
  enum TestFunc
  {
    TESTFUNC_NEVER     = PICA_DATA_ALPHA_TEST_NEVER   ,  //!< すべて不通過です。 
    TESTFUNC_ALWAYS    = PICA_DATA_ALPHA_TEST_ALWAYS  ,  //!< すべて通過です。 
    TESTFUNC_EQUAL     = PICA_DATA_ALPHA_TEST_EQUAL   ,  //!< 参照値と等しければ通過します。 
    TESTFUNC_NOTEQUAL  = PICA_DATA_ALPHA_TEST_NOTEQUAL,  //!< 参照値と等しくなければ通過します。 
    TESTFUNC_LESS      = PICA_DATA_ALPHA_TEST_LESS    ,  //!< 参照値より小さければ通過します。 
    TESTFUNC_LEQUAL    = PICA_DATA_ALPHA_TEST_LEQUAL  ,  //!< 参照値以下であれば通過します。 
    TESTFUNC_GREATER   = PICA_DATA_ALPHA_TEST_GREATER ,  //!< 参照値より大きければ通過します。 
    TESTFUNC_GEQUAL    = PICA_DATA_ALPHA_TEST_GEQUAL  ,  //!< 参照値以上であれば通過します。 
  };
};

// DepthTest
struct H3dDepthTestExConst
{
  // PICAコマンド生成にそのまま使用しているので値を変えないように。
  enum TestFunc
  {
    TESTFUNC_NEVER     = PICA_DATA_DEPTH_TEST_NEVER   ,  //!< すべて不通過です。 
    TESTFUNC_ALWAYS    = PICA_DATA_DEPTH_TEST_ALWAYS  ,  //!< すべて通過です。 
    TESTFUNC_EQUAL     = PICA_DATA_DEPTH_TEST_EQUAL   ,  //!< 参照値と等しければ通過します。 
    TESTFUNC_NOTEQUAL  = PICA_DATA_DEPTH_TEST_NOTEQUAL,  //!< 参照値と等しくなければ通過します。 
    TESTFUNC_LESS      = PICA_DATA_DEPTH_TEST_LESS    ,  //!< 参照値より小さければ通過します。 
    TESTFUNC_LEQUAL    = PICA_DATA_DEPTH_TEST_LEQUAL  ,  //!< 参照値以下であれば通過します。 
    TESTFUNC_GREATER   = PICA_DATA_DEPTH_TEST_GREATER ,  //!< 参照値より大きければ通過します。 
    TESTFUNC_GEQUAL    = PICA_DATA_DEPTH_TEST_GEQUAL  ,  //!< 参照値以上であれば通過します。 
  };
};

// Rasterization
struct H3dRasterizationExConst
{
  // PICAコマンド生成にそのまま使用しているので値を変えないように。
  enum CullingMode  // PICA_REG_CULL_FACE, PICA_CMD_DATA_CULL_FACEしかなく、カリングモードの定義がないので定数直指定。
  {
    CULLINGMODE_NONE  = 0,  //!< 両面表示です。
    CULLINGMODE_FRONT = 1,  //!< 表面カリングです。
    CULLINGMODE_BACK  = 2,  //!< 裏面カリングです。
  };
};


// Blend
struct H3dBlendExConst
{
  // PICA_DATA_ENABLE_COLOR_LOGIC_OPかPICA_DATA_ENABLE_BLENDをPICA_REG_COLOR_OPERATIONに設定する
  enum Mode
  {
    MODE_NOT_USE       ,  // 使用しません。
    MODE_BLEND         ,  // ブレンド処理です。           // MODE_BLENDにしてもMODE_SEPARATE_BLENDが設定されます。  // @note h3dでも有効な記述か？→h3dの場合は自分でgfxのこの「MODE_BLENDにしてもMODE_SEPARATE_BLENDが設定されます。」に合うように設定している。
    MODE_SEPARATE_BLEND,  // セパレートブレンド処理です。
    MODE_LOGIC            // 論理演算です。
  };

  // PICAコマンド生成にそのまま使用しているので値を変えないように。
  enum LogicOperation
  {
    LOGIC_OPERATION_CLEAR         = PICA_DATA_LOGIC_CLEAR         ,  // すべてのビット値を 0 にします。 
    LOGIC_OPERATION_AND           = PICA_DATA_LOGIC_AND           ,  // ソースカラーとディスティネーションカラーの論理積をとります。 
    LOGIC_OPERATION_AND_REVERSE   = PICA_DATA_LOGIC_AND_REVERSE   ,  // ソースカラーとディスティネーションカラーの反転ビット値の論理積をとります。 
    LOGIC_OPERATION_COPY          = PICA_DATA_LOGIC_COPY          ,  // ソースカラーのビット値をとります。 
    LOGIC_OPERATION_SET           = PICA_DATA_LOGIC_SET           ,  // すべてのビット値を 1 にします。 
    LOGIC_OPERATION_COPY_INVERTED = PICA_DATA_LOGIC_COPY_INVERTED ,  // ソースカラーの反転ビット値をとります。 
    LOGIC_OPERATION_NOOP          = PICA_DATA_LOGIC_NOOP          ,  // ディスティネーションカラーのビット値をとります。 
    LOGIC_OPERATION_INVERT        = PICA_DATA_LOGIC_INVERT        ,  // ディスティネーションカラーの反転ビット値をとります。 
    LOGIC_OPERATION_NAND          = PICA_DATA_LOGIC_NAND          ,  // ソースカラーとディスティネーションカラーの論理積の反転ビット値をとります。 
    LOGIC_OPERATION_OR            = PICA_DATA_LOGIC_OR            ,  // ソースカラーとディスティネーションカラーの論理和をとります。 
    LOGIC_OPERATION_NOR           = PICA_DATA_LOGIC_NOR           ,  // ソースカラーとディスティネーションカラーの論理和の反転ビット値をとります。 
    LOGIC_OPERATION_XOR           = PICA_DATA_LOGIC_XOR           ,  // ソースカラーとディスティネーションカラーの排他的論理和をとります。 
    LOGIC_OPERATION_EQUIV         = PICA_DATA_LOGIC_EQUIV         ,  // ソースカラーとディスティネーションカラーの排他的論理和の反転ビット値をとります。
    LOGIC_OPERATION_AND_INVERTED  = PICA_DATA_LOGIC_AND_INVERTED  ,  // ソースカラーの反転ビット値と、ディスティネーションカラーの論理積をとります。 
    LOGIC_OPERATION_OR_REVERSE    = PICA_DATA_LOGIC_OR_REVERSE    ,  // ソースカラーとディスティネーションカラーの反転ビット値の論理和をとります。 
    LOGIC_OPERATION_OR_INVERTED   = PICA_DATA_LOGIC_OR_INVERTED   ,  // ソースカラーの反転ビット値と、ディスティネーションカラーの論理和をとります。 
  };

  // PICAコマンド生成にそのまま使用しているので値を変えないように。
  enum Factor
  {
    FACTOR_ZERO                     = PICA_DATA_BLEND_FUNC_ZERO                     ,  // 指定の成分に「 0 」を利用します。 
    FACTOR_ONE                      = PICA_DATA_BLEND_FUNC_ONE                      ,  // 指定の成分に「 1 」を利用します。 
    FACTOR_SRC_COLOR                = PICA_DATA_BLEND_FUNC_SRC_COLOR                ,  // 指定の成分に「ソースカラー」を利用します。 
    FACTOR_ONE_MINUS_SRC_COLOR      = PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_COLOR      ,  // 指定の成分に「1 - ソースカラー」 を利用します。 
    FACTOR_DST_COLOR                = PICA_DATA_BLEND_FUNC_DST_COLOR                ,  // 指定の成分に「デスティネーションカラー」を利用します。 
    FACTOR_ONE_MINUS_DST_COLOR      = PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_COLOR      ,  // 指定の成分に「 1 - デスティネーションカラー」を利用します。 
    FACTOR_SRC_ALPHA                = PICA_DATA_BLEND_FUNC_SRC_ALPHA                ,  // 指定の成分に「ソースアルファ」を利用します。 
    FACTOR_ONE_MINUS_SRC_ALPHA      = PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA      ,  // 指定の成分に「 1 - ソースアルファ」 を利用します。 
    FACTOR_DST_ALPHA                = PICA_DATA_BLEND_FUNC_DST_ALPHA                ,  // 指定の成分に「デスティネーションアルファ」を利用します。 
    FACTOR_ONE_MINUS_DST_ALPHA      = PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_ALPHA      ,  // 指定の成分に「 1 - デスティネーションアルファ」を利用します。 
    FACTOR_CONSTANT_COLOR           = PICA_DATA_BLEND_FUNC_CONSTANT_COLOR           ,  // 指定の成分に「コンスタントカラー(ブレンドカラー)」を利用します。 
    FACTOR_ONE_MINUS_CONSTANT_COLOR = PICA_DATA_BLEND_FUNC_ONE_MINUS_CONSTANT_COLOR ,  // 指定の成分に「 1 - コンスタントカラー」を利用します。 
    FACTOR_CONSTANT_ALPHA           = PICA_DATA_BLEND_FUNC_CONSTANT_ALPHA           ,  // 指定の成分に「コンスタントアルファ(ブレンドアルファ)」を利用します。 
    FACTOR_ONE_MINUS_CONSTANT_ALPHA = PICA_DATA_BLEND_FUNC_ONE_MINUS_CONSTANT_ALPHA ,  // 指定の成分に「 1 - コンスタントアルファ」を利用します。 
    FACTOR_SRC_ALPHA_SATURATE       = PICA_DATA_BLEND_FUNC_SRC_ALPHA_SATURATE       ,  // 指定の成分に「 MIN( ソースアルファ , 1 - デスティネーションアルファ ) 」を利用します。 
  };

  // PICAコマンド生成にそのまま使用しているので値を変えないように。
  enum Equation
  {
    EQUATION_FUNC_ADD              = PICA_DATA_BLEND_EQUATION_ADD              ,  // ソースとデスティネーションを加算します。 
    EQUATION_FUNC_SUBTRACT         = PICA_DATA_BLEND_EQUATION_SUBTRACT         ,  // ソースからデスティネーションを減算します。
    EQUATION_FUNC_REVERSE_SUBTRACT = PICA_DATA_BLEND_EQUATION_REVERSE_SUBTRACT ,  // デスティネーションからソースを減算します。
    EQUATION_MIN                   = PICA_DATA_BLEND_EQUATION_MIN              ,  // ソースとデスティネーションのより小さい方です。
    EQUATION_MAX                   = PICA_DATA_BLEND_EQUATION_MAX              ,  // ソースとデスティネーションのより大きい方です。
  };
};



//-------------------------------------
///	H3Dシェーダ追加ステートクラス
//=====================================
class H3dShaderExState
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dShaderExState);

  friend class H3dModel;
  friend class H3dMaterialExState;


public:  // 公開しているメンバ

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 構造体
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  // @brief  シェーダシンボル情報
  struct ShaderSymbolVSInputParameter
  {
    bit32                          bitflag;      // 1のとき変更した。0のとき変更していない。
    nn::gr::BindSymbolVSInput      bind_symbol;
    gfl::math::VEC4                value;
  };

  struct ShaderSymbolVSFloatParameter
  {
    bit32                          bitflag;      // 1のとき変更した。0のとき変更していない。
    nn::gr::BindSymbolVSFloat      bind_symbol;
    gfl::math::VEC4                value;
  };

  struct ShaderSymbolVSIntegerParameter
  {
    bit32                            bitflag;      // 1のとき変更した。0のとき変更していない。
    nn::gr::BindSymbolVSInteger      bind_symbol;
    gfl::math::VEC4                  value;
  };
  // @note 他にもあるけど未対応。もっとうまくまとめたい。
  
  // @brief  設定内容
  struct Description
  {
    struct SyambolData{
      b32               fTopNode;
      b32               hasDefaultValue;
      gfl::math::VEC4   defaultValue;
      s32               fixIndex;
    };
    
    //SyambolData。
    const char** shader_symbol_vs_float_name_list;  // これは呼び出し元でこのまま保持しておいて下さい。
                                                    // このクラス内でポインタを覚えておいてアクセスします。
    const SyambolData* shader_symbol_vs_float_param_list;  // これは呼び出し元でこのまま保持しておいて下さい。
                                                             // このクラス内でポインタを覚えておいてアクセスします。
    
    ShaderSymbolVSFloatParameter        *p_shader_symbol_vs_float_param_static_buffer;
    
    s32          shader_symbol_vs_float_list_num;
    // @note シェーダシンボルは他にもあるけど未対応。
    
    Description(void)
      : shader_symbol_vs_float_name_list(NULL),
        shader_symbol_vs_float_param_list(NULL),
        p_shader_symbol_vs_float_param_static_buffer(NULL),
        shader_symbol_vs_float_list_num(0)
    {}
  };
  /*
    これら全部必要か？
      const char** shader_symbol_vs_input_name_list,
      const char** shader_symbol_vs_float_name_list,
      const char** shader_symbol_vs_integer_name_list,
      const char** shader_symbol_vs_bool_name_list,
      const char** shader_symbol_gs_float_name_list,
      const char** shader_symbol_gs_bool_name_list,
      s32 shader_symbol_vs_input_list_num,
      s32 shader_symbol_vs_float_list_num,
      s32 shader_symbol_vs_integer_list_num,
      s32 shader_symbol_vs_bool_list_num,
      s32 shader_symbol_gs_float_list_num,
      s32 shader_symbol_gs_bool_list_num
  */ 


private:  // friendには公開している非公開メンバ

  // コンストラクタ / デストラクタ
  H3dShaderExState(void);
  virtual ~H3dShaderExState();

  // 生成 / 破棄
  void Create(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      const nw::h3d::res::MaterialContent* cont,
      const Description* desc
  ); 
  void Destroy(void);

  // 確認用
  void DumpShaderSymbolName(
      const nw::h3d::res::MaterialContent* cont
  ) const;

  // シェーダシンボル
  void SetShaderSymbolVSFloat(const char* name, const gfl::math::VEC4& value);  // index版を使って下さい。
  void SetShaderSymbolVSFloat(s32 index, const gfl::math::VEC4& value);  // Createで渡したshader_symbol_vs_float_name_listのインデックス

  void GetShaderSymbolVSFloat(const char* name, gfl::math::VEC4* value) const;  // index版を使って下さい。
  void GetShaderSymbolVSFloat(s32 index, gfl::math::VEC4* value) const;  // Createで渡したshader_symbol_vs_float_name_listのインデックス
  
  s32 GetShaderSymbolIndex(const char* name) const;

  // コマンド
  void MakeShaderCommand(H3dCommand* command);

private:  // friendにも公開していない非公開メンバ
  void constructorThis(void);

private:  // friendにも公開していない非公開メンバ
  
  struct ShaderSymbolInfo
  {
    ShaderSymbolInfo() :
      vs_input_name_list(NULL),
      vs_float_name_list(NULL),
      vs_integer_name_list(NULL),
      vs_input_list_num(0),
      vs_float_list_num(0),
      vs_integer_list_num(0),
      f_static_vs_float_list(false),
      vs_input_list(NULL),
      vs_float_list(NULL),
      vs_integer_list(NULL)
    {
      
    }
    
    const char** vs_input_name_list;  // const char* vs_input_name_list[list_num];  // 他のところでつくったリストのポインタを覚えておくだけ
    const char** vs_float_name_list;
    const char** vs_integer_name_list;
    // @note 他にもあるけど未対応。もっとうまくまとめたい。
    
    s32          vs_input_list_num;
    s32          vs_float_list_num;
    s32          vs_integer_list_num;
    b32          f_static_vs_float_list;
    // @note 他にもあるけど未対応。もっとうまくまとめたい。

    ShaderSymbolVSInputParameter**    vs_input_list;     // vs_input_list = GFL_NEW() ShaderSymbolVSInputParameter*[list_num];
    ShaderSymbolVSFloatParameter**    vs_float_list;
    ShaderSymbolVSIntegerParameter**  vs_integer_list;
    // @note 他にもあるけど未対応。もっとうまくまとめたい。
  };

private:  // friendにも公開しない非公開メンバ
  ShaderSymbolInfo                          m_shader_symbol_info;

#if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
public:
  // コマンド数カウントをリセットする
  void ResetCommandCount(void);
  // 1フレームのコマンド数カウント開始
  void StartOneFrameCommandCount(void);
  // 1フレームのコマンド数カウント終了
  void EndOneFrameCommandCount(void);
  // コマンド数を出力する
  void PrintCommandCount(void);
private:
  // コマンド数カウント
  struct CommandCount  // bit32で数えたときのコマンド数
  {
    s16 shader_command;
  };
  static CommandCount m_shader_command_max_set;   // shader_commandが最大数になったときのCommandCount
  static CommandCount m_command_curr_set;   // 今のCommandCount
  
  void printCommandCount(const CommandCount& command_count);
#endif  // #if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT

};



//-------------------------------------
///	H3Dマテリアル追加ステートクラス
//=====================================
class H3dMaterialExState
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dMaterialExState);


  friend class H3dModel;
  friend class H3dCommandListenerReadMaterial;


private:  // friendには公開している非公開メンバ

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // コンストラクタ / デストラクタ
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  H3dMaterialExState(void);
  ~H3dMaterialExState();


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 生成 / 破棄
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  void Initialize(gfl::heap::NwAllocator* device_allocator, const nw::h3d::res::ModelContent* model, s32 materialIndex );
 
  // no  テクスチャマッパー番号(0,1,2,Proc)(テクスチャコーディネータ番号ではない)
  // textureSamplerはnoのテクスチャサンプラのデータ
  void InitializeTextureSampler(s32 no, const nw::h3d::res::TextureSamplerData* textureSampler, s32 mipmapSize = 1 );



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // TextureCombiner
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
 
  // step段目
  void InitializeTextureCombinerCombineRgb(s32 step, H3dTextureCombinerExConst::Combine combine);
  void SetTextureCombinerCombineRgb(s32 step, H3dTextureCombinerExConst::Combine combine);
  H3dTextureCombinerExConst::Combine  GetTextureCombinerCombineRgb(s32 step);

  // step段目
  void InitializeTextureCombinerCombineAlpha(s32 step, H3dTextureCombinerExConst::Combine combine);
  void SetTextureCombinerCombineAlpha(s32 step, H3dTextureCombinerExConst::Combine combine);
  H3dTextureCombinerExConst::Combine  GetTextureCombinerCombineAlpha(s32 step);



  // step段目
  void InitializeTextureCombinerScaleRgb(s32 step, H3dTextureCombinerExConst::Scale scale);
  void SetTextureCombinerScaleRgb(s32 step, H3dTextureCombinerExConst::Scale scale);
  H3dTextureCombinerExConst::Scale  GetTextureCombinerScaleRgb(s32 step);
  
  // step段目
  void InitializeTextureCombinerScaleAlpha(s32 step, H3dTextureCombinerExConst::Scale scale);
  void SetTextureCombinerScaleAlpha(s32 step, H3dTextureCombinerExConst::Scale scale);
  H3dTextureCombinerExConst::Scale  GetTextureCombinerScaleAlpha(s32 step);



  // step段目
  // no ソースno  A:ソース0  B:ソース1  C:ソース2
  void InitializeTextureCombinerOperandRgb(s32 step, s32 no, H3dTextureCombinerExConst::OperandRgb ope);
  void SetTextureCombinerOperandRgb(s32 step, s32 no, H3dTextureCombinerExConst::OperandRgb ope);
  H3dTextureCombinerExConst::OperandRgb  GetTextureCombinerOperandRgb(s32 step, s32 no);
  
  // step段目
  // no ソースno  A:ソース0  B:ソース1  C:ソース2
  void InitializeTextureCombinerOperandAlpha(s32 step, s32 no, H3dTextureCombinerExConst::OperandAlpha ope);
  void SetTextureCombinerOperandAlpha(s32 step, s32 no, H3dTextureCombinerExConst::OperandAlpha ope);
  H3dTextureCombinerExConst::OperandAlpha  GetTextureCombinerOperandAlpha(s32 step, s32 no);



  // step段目
  // no ソースno  A:ソース0  B:ソース1  C:ソース2
  void InitializeTextureCombinerSourceRgb(s32 step, s32 no, H3dTextureCombinerExConst::Source source);
  void SetTextureCombinerSourceRgb(s32 step, s32 no, H3dTextureCombinerExConst::Source source);
  H3dTextureCombinerExConst::Source  GetTextureCombinerSourceRgb(s32 step, s32 no);
  
  // step段目
  // no ソースno  A:ソース0  B:ソース1  C:ソース2
  void InitializeTextureCombinerSourceAlpha(s32 step, s32 no, H3dTextureCombinerExConst::Source source);
  void SetTextureCombinerSourceAlpha(s32 step, s32 no, H3dTextureCombinerExConst::Source source);
  H3dTextureCombinerExConst::Source  GetTextureCombinerSourceAlpha(s32 step, s32 no);
  
  //SDK 5.0 対応
  
  void SetMaterialTextureCombinerConstant(s32 step, s32 no, const gfl::grp::ColorU8& rColor );
  s32 GetMaterialTextureCombinerConstant(s32 step) const;
  gfl::grp::ColorU8 GetMaterialTextureCombinerConstantColor(s32 step) const;



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // FragmentShader
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  // step段目は前段の何を使うか(step段目は後段に何を渡すかではないので注意すること)
  void InitializeFragmentShaderBufferInputRgb(s32 step, H3dFragmentShaderExConst::BufferInput input);
  void SetFragmentShaderBufferInputRgb(s32 step, H3dFragmentShaderExConst::BufferInput input);
  H3dFragmentShaderExConst::BufferInput  GetFragmentShaderBufferInputRgb(s32 step);

  // step段目は前段の何を使うか(step段目は後段に何を渡すかではないので注意すること)
  void InitializeFragmentShaderBufferInputAlpha(s32 step, H3dFragmentShaderExConst::BufferInput input);
  void SetFragmentShaderBufferInputAlpha(s32 step, H3dFragmentShaderExConst::BufferInput input);
  H3dFragmentShaderExConst::BufferInput  GetFragmentShaderBufferInputAlpha(s32 step);




  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // StencilOperation
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  void InitializeStencilOperationTest(u8 enable, H3dStencilOperationExConst::TestFunc func_func, u8 mask, u8 func_ref, u8 func_mask);
  void InitializeStencilOperationOp(
      H3dStencilOperationExConst::StencilOp op_fail, H3dStencilOperationExConst::StencilOp op_zfail, H3dStencilOperationExConst::StencilOp op_zpass);

  b32 IsStencilOperationTestEnable(void) const;
  void SetStencilOperationTestEnable(b32 enable);
  H3dStencilOperationExConst::TestFunc GetStencilOperationTestFunc(void) const;
  void SetStencilOperationTestFunc(H3dStencilOperationExConst::TestFunc func_func);
  
  void SetStencilOperationTestFuncRef(u8 func_ref);
  u8 GetStencilOperationTestFuncRef(void) const;

  u8 GetStencilOperationTestMask(void) const;  // @note MaskかFuncMaskどちらかを使用禁止にしたい。どっちがオーサリングツールで設定できるほうか？
  void SetStencilOperationTestMask(u8 mask);  // @note MaskかFuncMaskどちらかを使用禁止にしたい。どっちがオーサリングツールで設定できるほうか？
  u8 GetStencilOperationTestFuncMask(void) const;  // @note MaskかFuncMaskどちらかを使用禁止にしたい。どっちがオーサリングツールで設定できるほうか？
  void SetStencilOperationTestFuncMask(u8 func_mask);  // @note MaskかFuncMaskどちらかを使用禁止にしたい。どっちがオーサリングツールで設定できるほうか？

  H3dStencilOperationExConst::StencilOp GetStencilOperationOpFail(void) const;
  void SetStencilOperationOpFail(H3dStencilOperationExConst::StencilOp op_fail);
  H3dStencilOperationExConst::StencilOp GetStencilOperationOpZFail(void) const;
  void SetStencilOperationOpZFail(H3dStencilOperationExConst::StencilOp op_zfail);
  H3dStencilOperationExConst::StencilOp GetStencilOperationOpZPass(void) const;
  void SetStencilOperationOpZPass(H3dStencilOperationExConst::StencilOp op_zpass);



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // AlphaTest
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  void InitializeAlphaTest(u8 enable, H3dAlphaTestExConst::TestFunc func, u8 ref);

  b32 IsAlphaTestEnable(void) const;
  void SetAlphaTestEnable(b32 enable);  // trueのときアルファテスト有効
  H3dAlphaTestExConst::TestFunc GetAlphaTestFunc(void) const;
  void SetAlphaTestFunc(H3dAlphaTestExConst::TestFunc func);

  void SetAlphaTestRef(u8 ref);
  u8 GetAlphaTestRef(void) const;



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // DepthTest
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  void InitializeDepthTest(u8 enable, u8 mask_enable, H3dDepthTestExConst::TestFunc func);
  b32 IsDepthTestEnable(void) const;
  void SetDepthTestEnable(b32 enable);  // trueのときデプステスト有効
  b32 IsDepthTestMaskEnable(void) const;
  void SetDepthTestMaskEnable(b32 enable);  // trueのときデプスマスク有効(trueのときデプスバッファを更新する)
  H3dDepthTestExConst::TestFunc GetDepthTestFunc(void) const;
  void SetDepthTestFunc(H3dDepthTestExConst::TestFunc func);



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // ブレンダー
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  void InitializeBlendMode(H3dBlendExConst::Mode mode);
  void InitializeBlendFunction(
      H3dBlendExConst::Equation rgb_equation,
      H3dBlendExConst::Factor   rgb_src_function,
      H3dBlendExConst::Factor   rgb_dst_function,
      H3dBlendExConst::Equation alpha_equation,
      H3dBlendExConst::Factor   alpha_src_function,
      H3dBlendExConst::Factor   alpha_dst_function
  );
  void InitializeBlendLogicOperation(H3dBlendExConst::LogicOperation operation);
  // 全ての設定が終わった後に呼ぶ初期化
  void InitializeBlendAfterAllSet(void);

  // @attention 【注意】ブレンドモードによって設定できたりしなかったりするように制御しているので、まず最初にブレンドモードを変更して下さい。
  void SetBlendMode(H3dBlendExConst::Mode mode);
  H3dBlendExConst::Mode GetBlendMode(void) const;

  //void SetBlendColor(const gfl::grp::ColorF32& c);  // nw::h3d::MaterialStateにある
  //void GetBlendColor(gfl::grp::ColorF32* c) const;  // nw::h3d::MaterialStateにある
  
  H3dBlendExConst::LogicOperation GetBlendLogicOperation(void) const;
  void SetBlendLogicOperation(H3dBlendExConst::LogicOperation operation);

  H3dBlendExConst::Factor GetBlendFunctionSourceRgb(void) const;
  void SetBlendFunctionSourceRgb(H3dBlendExConst::Factor factor);
  H3dBlendExConst::Factor GetBlendFunctionSourceAlpha(void) const;
  void SetBlendFunctionSourceAlpha(H3dBlendExConst::Factor factor);

  H3dBlendExConst::Factor GetBlendFunctionDestinationRgb(void) const;
  void SetBlendFunctionDestinationRgb(H3dBlendExConst::Factor factor);
  H3dBlendExConst::Factor GetBlendFunctionDestinationAlpha(void) const;
  void SetBlendFunctionDestinationAlpha(H3dBlendExConst::Factor factor);

  H3dBlendExConst::Equation GetBlendEquationRgb(void) const;
  void SetBlendEquationRgb(H3dBlendExConst::Equation equation);
  H3dBlendExConst::Equation GetBlendEquationAlpha(void) const;
  void SetBlendEquationAlpha(H3dBlendExConst::Equation equation);


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // バッファ操作
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  void InitializeColorBufferRead( b8 fColorRead );  
  void InitializeColorBufferWrite( b8 fColorWrite );  
  void InitializeDepthBufferRead( b8 fDepthRead, b8 fStencilRead );  
  void InitializeDepthBufferWrite( b8 fDepthWrite, b8 fStencilWrite );  

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // テクスチャマッパー(テクスチャサンプラー)
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  // no  テクスチャマッパー番号(0,1,2,Proc)(テクスチャコーディネータ番号ではない)
  // U方向繰り返し設定を行う。
  void InitializeTextureSamplerWrapU(s32 no, H3dTextureSamplerExConst::Wrap wrap);
  void SetTextureSamplerWrapU(s32 no, H3dTextureSamplerExConst::Wrap wrap);
  // U方向繰り返し設定を得る。
  H3dTextureSamplerExConst::Wrap  GetTextureSamplerWrapU(s32 no) const;

  // V方向繰り返し設定を行う。
  void InitializeTextureSamplerWrapV(s32 no, H3dTextureSamplerExConst::Wrap wrap);
  void SetTextureSamplerWrapV(s32 no, H3dTextureSamplerExConst::Wrap wrap);
  // V方向繰り返し設定を得る。
  H3dTextureSamplerExConst::Wrap  GetTextureSamplerWrapV(s32 no) const;
  // テクスチャの座標は、STは使わず、UVで統一しておく。

  // no  テクスチャマッパー番号(0,1,2,Proc)(テクスチャコーディネータ番号ではない)
  // テクスチャサンプラータイプ有効無効
  void InitializeTextureSamplerTypeEnable(s32 no, H3dTextureSamplerExConst::SamplerTypeEnable enable);
  void SetTextureSamplerTypeEnable(s32 no, H3dTextureSamplerExConst::SamplerTypeEnable enable);
  H3dTextureSamplerExConst::SamplerTypeEnable  GetTextureSamplerTypeEnable(s32 no) const;

  // no  テクスチャマッパー番号(0,1,2,Proc)(テクスチャコーディネータ番号ではない)
  // テクスチャサンプラータイプ
  void InitializeTextureSamplerType(s32 no, H3dTextureSamplerExConst::SamplerType sampler_type);
  void SetTextureSamplerType(s32 no, H3dTextureSamplerExConst::SamplerType sampler_type);
  H3dTextureSamplerExConst::SamplerType  GetTextureSamplerType(s32 no) const;
  //res_tex_sampler.SetSamplerType( nw::gfx::ResTextureSampler::SAMPLERTYPE_PROJECTION );  // テクスチャサンプラータイプを設定します。 テクスチャサンプラータイプが設定可能なのは０番目のテクスチャマッパーのみです。 １、２番目に設定すると正しく動作しません。

  // no  テクスチャマッパー番号(0,1,2,Proc)(テクスチャコーディネータ番号ではない)
  // 拡大時の補間処理
  void InitializeTextureSamplerMagFilter(s32 no, H3dTextureSamplerExConst::MagFilter mag_filter);
  void SetTextureSamplerMagFilter(s32 no, H3dTextureSamplerExConst::MagFilter mag_filter);
  H3dTextureSamplerExConst::MagFilter GetTextureSamplerMagFilter(s32 no) const;

  // 縮小時の補間処理
  void InitializeTextureSamplerMinFilter(s32 no, H3dTextureSamplerExConst::MinFilter min_filter);
  void InitializeTextureSamplerMinFilterDown(s32 no, H3dTextureSamplerExConst::MinFilterDown min_filter_down);
  void InitializeTextureSamplerMinFilterUp(s32 no, H3dTextureSamplerExConst::MinFilterUp min_filter_up);
  void SetTextureSamplerMinFilter(s32 no, H3dTextureSamplerExConst::MinFilter min_filter);
  H3dTextureSamplerExConst::MinFilter GetTextureSamplerMinFilter(s32 no) const;
  
  // ミップマップ設定
  void SetTextureMipMapLodMinLevel( s32 no, s32 level );
  void SetTextureMipMapLodBias( s32 no, s32 bias );

private:
  // 現在設定されているdown、upを使って、min_filterを更新しておく。
  void setTextureSamplerMinFilterByUpDown(s32 no);


public:
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // Rasterization
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  void InitializeRasterizationCullingMode(H3dRasterizationExConst::CullingMode cull);
  void SetRasterizationCullingMode(H3dRasterizationExConst::CullingMode cull);
  H3dRasterizationExConst::CullingMode GetRasterizationCullingMode(void);





  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // コマンド
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  void MakeMutableCommand(nn::gr::CTR::CommandBufferJumpHelper *dcs);
  void ResetDirtyBits();

public:  
  
  // SDK 5.0対応
  void MakeCombinerColorCommand(nn::gr::CTR::CommandBufferJumpHelper *dcs, const nw::h3d::MaterialState* materialState) const;
  // textureSamplersはnw::h3d::Constants::PIXELBASED_TEXTURE_UNIT_QUANTITY個のテクスチャサンプラのデータの配列
  void MakeTextureSamplerCommand(nn::gr::CTR::CommandBufferJumpHelper *dcs, const nw::h3d::MaterialState* materialState/*, const nw::h3d::res::TextureSamplerData* textureSamplers*/) const;
  
private:  
  void makeMutableBufferBlensCommand( nn::gr::CTR::CommandBufferJumpHelper& dcs ) const;

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // friendが取得するためのメンバ関数
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:  
  H3dShaderExState* GetShaderExState(void) { return &m_shader_ex_state; }




private:  // friendにも公開しない非公開メンバ

  // コンストラクタでやっているのと同じ初期化
  void constructorThis(void);
  void debugPrint(void);


private:  // friendにも公開しない非公開メンバ

  // @brief  テクスチャコンバイナ1ソース分の情報
  struct TextureCombinerSource
  {
    enum
    {
      TEXTURE_COMBINER_OPERAND_RGB_MUTABLE    = 1 <<  0,
      TEXTURE_COMBINER_OPERAND_ALPHA_MUTABLE  = 1 <<  1,
      TEXTURE_COMBINER_SOURCE_RGB_MUTABLE     = 1 <<  2,
      TEXTURE_COMBINER_SOURCE_ALPHA_MUTABLE   = 1 <<  3
    };
    bit32 bitflag;  // 変更したもののビットを立てる

    //H3dTextureCombinerExConst::OperandRgb    operand_rgb;
    //H3dTextureCombinerExConst::OperandAlpha  operand_alpha;
    //H3dTextureCombinerExConst::Source        source_rgb;
    //H3dTextureCombinerExConst::Source        source_alpha;

    u8  operand_rgb;
    u8  operand_alpha;
    u8  source_rgb;
    u8  source_alpha;
  };
 
  // @brief  テクスチャコンバイナ1段分の情報
  struct TextureCombinerStep
  {
    enum
    {
      TEXTURE_COMBINER_COMBINE_RGB_MUTABLE        = 1 <<  0,
      TEXTURE_COMBINER_COMBINE_ALPHA_MUTABLE      = 1 <<  1,
      TEXTURE_COMBINER_SCALE_RGB_MUTABLE          = 1 <<  2,
      TEXTURE_COMBINER_SCALE_ALPHA_MUTABLE        = 1 <<  3,
      FRAGMENT_SHADER_BUFFER_INPUT_RGB_MUTABLE    = 1 <<  4,
      FRAGMENT_SHADER_BUFFER_INPUT_ALPHA_MUTABLE  = 1 <<  5
    };
    bit32 bitflag;  // 変更したもののビットを立てる

    //H3dTextureCombinerExConst::Combine  combine_rgb;
    //H3dTextureCombinerExConst::Combine  combine_alpha;
    //H3dTextureCombinerExConst::Scale    scale_rgb;
    //H3dTextureCombinerExConst::Scale    scale_alpha;

    u8  combine_rgb;
    u8  combine_alpha;
    u8  scale_rgb;
    u8  scale_alpha;
    
    TextureCombinerSource  source[H3dTextureCombinerExConst::SOURCE_NUM];
    
    //H3dFragmentShaderExConst::BufferInput  buffer_input_rgb;
    //H3dFragmentShaderExConst::BufferInput  buffer_input_alpha;

    u8  buffer_input_rgb;
    u8  buffer_input_alpha;
  };
  
  // @brief  ステンシルオペレーションの情報
  struct StencilOperationInfo
  {
    enum
    {
      STENCIL_TEST_MUTABLE    = 1 <<  0,
      STENCIL_OP_MUTABLE      = 1 <<  1,
    };
    bit32 bitflag;  // 変更したもののビットを立てる

    u8 enable;
    u8 func_func;
    u8 mask;
    u8 func_ref;
    u8 func_mask;
    u8 op_fail;
    u8 op_zfail;
    u8 op_zpass;

#if 0
/*
CTR
DMPGL2.0システムAPI仕様書

glDisable(GL_STENCIL_TEST)、
glEnable(GL_STENCIL_TEST)
glStencilFuncのfunc
glStencilMask
glStencilFuncのref
glStencilFuncのmask
glStencilOpのfail
glStencilOpのzfail
glStencilOpのzpass
*/
#endif

  };

  // @brief  アルファテストの情報
  struct AlphaTestInfo
  {
    enum
    {
      ALPHA_TEST_MUTABLE     = 1 <<  0,
    };
    bit32 bitflag;  // 変更したもののビットを立てる

    u8 enable;
    u8 func;
    u8 ref;
  };

  // @brief  デプステストの情報
  struct DepthColorMaskInfo
  {
    enum
    {
      DEPTH_TEST_MUTABLE        = 1 <<  0,
      DEPTH_COLOR_MASK_MUTABLE  = 1 <<  1,
    };
    u32 bitflag            :8;  // 変更したもののビットを立てる

    u32 depth_test_enable  :1;  // 1のときデプステスト有効
    u32 depth_test_func    :7;
    u32 red_mask           :1;  // 今のところ変更する予定はないので常に1にしておく
    u32 green_mask         :1;  // 今のところ変更する予定はないので常に1にしておく
    u32 blue_mask          :1;  // 今のところ変更する予定はないので常に1にしておく
    u32 alpha_mask         :1;  // 今のところ変更する予定はないので常に1にしておく
    u32 depth_mask         :1;  // 1のときデプスマスク有効(1のときデプスバッファを更新する)
  };

  // @brief  ブレンダーの情報
  struct BlendInfo
  {
    enum
    {
      BLEND_MODE_MUTABLE             = 1 <<  0,
      BLEND_FUNCTION_MUTABLE         = 1 <<  1,
      BLEND_LOGIC_OPERATION_MUTABLE  = 1 <<  2,
    };
    u32 bitflag                    :3;

    u32 blend_mode                 :3;
    
    u32 blend_equation_rgb         :3;
    u32 blend_equation_alpha       :3;

    u32 blend_function_src_rgb     :4;
    u32 blend_function_src_alpha   :4;
    u32 blend_function_dst_rgb     :4;
    u32 blend_function_dst_alpha   :4;
    
    u32 logic_operation            :4;
  };
  
  // @brief バッファリードの情報
  struct BufferReadInfo{
    u32 fColorRead      : 1;
    u32 fColorWrite     : 1;
    u32 fDepthRead      : 1;
    u32 fDepthWrite     : 1;
    u32 fStencilRead    : 1;
    u32 fStencilWrite   : 1;
    u32 padding         : 26;
  };

  // @brief  テクスチャサンプラー情報
  struct TextureSamplerInfo
  {
    enum
    {
      SAMPLER_TYPE_MUTABLE   = 1 <<  0,
      WRAP_MUTABLE           = 1 <<  1,
      FILTER_MUTABLE         = 1 <<  2,
      MAG_LOD_LEVEL_MUTABLE  = 1 <<  3,
      MIN_LOD_LEVEL_MUTABLE  = 1 <<  4,
      LOD_BIAS_MUTABLE       = 1 <<  5,
    };
    u32 bitflag             :3;  // 変更したもののビットを立てる
    u32 sampler_type_enable :1;  // sampler_type_enableとsampler_typeの両方を
    u32 sampler_type        :3;  // きちんと更新すること。
    u32 wrap_u              :2;
    u32 wrap_v              :2;
    u32 mag_filter          :1;  // down、upが本物のデータなので、これの扱いには注意すること。
    u32 min_filter_down     :1;  // 本物のデータ
    u32 min_filter_up       :1;  // 本物のデータ
    u32 min_filter          :3;
    u32 mag_lod_level       :4;
    u32 min_lod_level       :4;
    u32 mipmapSize          :4;
    f32 lod_bias;              
  };

  // @brief  ラスタライゼーション情報
  struct RasterizationInfo
  {
    enum
    {
      CULLING_MODE_MUTABLE  = 1 << 0,
    };
    bit32 bitflag;  // 変更したもののビットを立てる

    u8 culling_mode;
  };

private:  // friendにも公開しない非公開メンバ
  TextureCombinerStep  m_tex_comb_step[H3dTextureCombinerExConst::STEP_NUM];
  StencilOperationInfo m_stencil_op_info;
  AlphaTestInfo        m_alpha_test_info;
  DepthColorMaskInfo   m_depth_color_mask_info;
  BlendInfo            m_blend_info;
  BufferReadInfo       m_buffer_read_info;
  TextureSamplerInfo   m_tex_sampler_info[H3dTextureSamplerExConst::TEXTURE_NUM];
  RasterizationInfo    m_raster_info;
  
  s32                  m_MaterialTextureCombinerConstantNo[H3dTextureCombinerExConst::STEP_NUM];
  gfl::grp::ColorU8    m_ConbinerColor[H3dTextureCombinerExConst::STEP_NUM];
  
  H3dShaderExState     m_shader_ex_state;


#if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
public:
  // コマンド数カウントをリセットする
  void ResetCommandCount(void);
  // 1フレームのコマンド数カウント開始
  void StartOneFrameCommandCount(void);
  // 1フレームのコマンド数カウント終了
  void EndOneFrameCommandCount(void);
  // コマンド数を出力する
  void PrintCommandCount(void);
private:
  // コマンド数カウント
  struct CommandCount  // bit32で数えたときのコマンド数
  {
    s16 mutable_command;
    s16 tex_sampler_command;
  };
  static CommandCount m_mutable_command_max_set;  // mutable_commandが最大数になったときのCommandCount
  static CommandCount m_tex_sampler_command_max_set;  // tex_sampler_commandが最大数になったときのCommandCount
  static CommandCount m_command_curr_set;   // 今のCommandCount
  
  void printCommandCount(const CommandCount& command_count);
#endif  // #if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT

};

//--------------------------------------------------------------------------
///	任天堂様から頂いた、拡張ユーザーデータアクセッサ
//==========================================================================
enum CullFaceFlag
{
    CULL_FACE_FRONT,
    CULL_FACE_BACK,
    CULL_FACE_DISABLE
};

//! @name Material Optional Data のヘッダのビット配置です。
//@{

static const u32 FRAGMENT_OP_BIT = (1 << 0);
static const u32 CULL_FACE_FLAG_SHIFT = 6;
static const u32 CULL_FACE_FLAG_MASK = (0x3 << CULL_FACE_FLAG_SHIFT);
static const u32 COMBINER_CMD_OFFSET_SHIFT = 8;
static const u32 COMBINER_CMD_OFFSET_MASK = (0xff << COMBINER_CMD_OFFSET_SHIFT);

//@}
//! @name Material Optional Data のフラグメントオペレーション情報のビット配置です。
//@{

static const u32 STENCIL_CMD_OFFSET_SHIFT = 0;
static const u32 STENCIL_CMD_OFFSET_MASK = (0xffu << STENCIL_CMD_OFFSET_SHIFT);
static const u32 ALPHA_TEST_CMD_OFFSET_SHIFT = 8;
static const u32 ALPHA_TEST_CMD_OFFSET_MASK = (0xffu << ALPHA_TEST_CMD_OFFSET_SHIFT);
static const u32 DEPTH_COLOR_MASK_CMD_OFFSET_SHIFT = 16;
static const u32 DEPTH_COLOR_MASK_CMD_OFFSET_MASK = (0xffu << DEPTH_COLOR_MASK_CMD_OFFSET_SHIFT);
static const u32 BLEND_LOGIC_OP_CMD_OFFSET_SHIFT = 24;
static const u32 BLEND_LOGIC_OP_CMD_OFFSET_MASK = (0xffu << BLEND_LOGIC_OP_CMD_OFFSET_SHIFT);

//@}

//---------------------------------------------------------------------------
//! @brief Material Optional Data の情報を取得するためのヘルパーです。
//---------------------------------------------------------------------------
class ModHelper
{
    ModHelper();

public:

    //! MetaData がフラグメントオペレーションデータを持って場合に true を返します。
    static bool HasFragmentOpData(const nw::h3d::res::MetaData& metaData)
    {
        NW_ASSERT(metaData.dataCount > 0);
        return (static_cast<int*>(metaData.data)[0] & FRAGMENT_OP_BIT) && (metaData.dataCount > 1);
    }

    //! MetaData から CullFace の設定を取得します。
    static CullFaceFlag GetCullFace(const nw::h3d::res::MetaData& metaData)
    {
        NW_ASSERT(metaData.dataCount > 0);
        return static_cast<CullFaceFlag>(
            (static_cast<int*>(metaData.data)[0] & CULL_FACE_FLAG_MASK) >> CULL_FACE_FLAG_SHIFT);
    }

    //! @name コンバイナ
    //@{

    //! 任意の段のコンバイナのコマンドを取得します。
    static bit32* GetCombinerCmd(const nw::h3d::res::MetaData& metaData, const nw::h3d::res::MaterialContent* content, int combinerId)
    {
        NW_MINMAXLT_ASSERT(combinerId, 0, nw::h3d::Constants::COMBINER_QUANTITY);

        if (! HasFragmentOpData(metaData)) { return NULL; }
        bit32* cmd = GetCmd(metaData, content, 0, COMBINER_CMD_OFFSET_MASK, COMBINER_CMD_OFFSET_SHIFT);
        if (cmd == NULL) { return NULL; }

        cmd += combinerId * MOD_COMBINER_STAGE_CMD_SIZE;
        NW_ASSERT((*(cmd + 1) & 0xffff) == GetCombinerTopAddress(combinerId));
        return cmd;
    }
    //! 任意の段のコンバイナのコマンドから Source 設定を取得します。
    static bit32 GetCombinerSource(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_COMBINER_SOURCE_OFFSET); }
    //! 任意の段のコンバイナのコマンドから Operand 設定を取得します。
    static bit32 GetCombinerOperand(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_COMBINER_OPERAND_OFFSET); }
    //! 任意の段のコンバイナのコマンドから Combine 設定を取得します。
    static bit32 GetCombinerCombine(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_COMBINER_COMBINE_OFFSET); }
    //! 任意の段のコンバイナのコマンドから Const 設定を取得します。
    static bit32 GetCombinerConst(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_COMBINER_CONST_OFFSET); }
    //! 任意の段のコンバイナのコマンドから Scale 設定を取得します。
    static bit32 GetCombinerScale(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_COMBINER_SCALE_OFFSET); }

    //! コンバイナバッファのコマンドを取得します。
    static bit32* GetCombinerBufferCmd(const nw::h3d::res::MetaData& metaData, const nw::h3d::res::MaterialContent* content)
    {
        if (! HasFragmentOpData(metaData)) { return NULL; }
        bit32* cmd = GetCmd(metaData, content, 0, COMBINER_CMD_OFFSET_MASK, COMBINER_CMD_OFFSET_SHIFT);
        if (cmd == NULL) { return NULL; }

        cmd += MOD_COMBINER_BUFFER_OFFSET;
        NW_ASSERT((*(cmd + 1) & 0xffff) == PICA_REG_TEX_ENV_BUFFER_INPUT);
        return cmd;
    }
    //! コンバイナバッファのコマンドから Input 設定を取得します。
    static bit32 GetCombinerBufferInput(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_COMBINER_BUFFER_INPUT_OFFSET); }
    //! コンバイナバッファのコマンドから Color 設定を取得します。
    static bit32 GetCombinerBufferColor(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_COMBINER_BUFFER_COLOR_OFFSET); }

    //@}
    //! @name ステンシルテスト
    //@{

    //! ステンシルテストのコマンドを取得します。
    static bit32* GetStencilCmd(const nw::h3d::res::MetaData& metaData, const nw::h3d::res::MaterialContent* content)
    {
        if (! HasFragmentOpData(metaData)) { return NULL; }
        bit32* cmd = GetCmd(metaData, content, 1, STENCIL_CMD_OFFSET_MASK, STENCIL_CMD_OFFSET_SHIFT);
        if (cmd == NULL) { return NULL; }

        NW_ASSERT((*(cmd + 1) & 0xffff) == PICA_REG_STENCIL_TEST);
        return cmd;
    }

    //! ステンシルテストのコマンドから Test 設定を取得します。
    static bit32 GetStencilTest(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_STENCIL_TEST_OFFSET); }
    //! ステンシルテストのコマンドから Operation 設定を取得します。
    //!
    //! ステンシルテストが無効の場合に取得できません。fnd::Optional で返り値の有無を確認してください。
    static nw::h3d::fnd::Optional<bit32> GetStencilOp(bit32* cmd)
    {
        if (cmd && ((*(cmd + MOD_STENCIL_OP_OFFSET + 1) & 0xffff) == PICA_REG_STENCIL_OP))
        {
            return *(cmd + MOD_STENCIL_OP_OFFSET);
        }
        return nw::h3d::fnd::Optional<bit32>();
    }

    //@}
    //! @name アルファテスト
    //@{

    //! アルファテストのコマンドを取得します。
    static bit32* GetAlphaTestCmd(const nw::h3d::res::MetaData& metaData, const nw::h3d::res::MaterialContent* content)
    {
        if (! HasFragmentOpData(metaData)) { return NULL; }
        bit32* cmd = GetCmd(metaData, content, 1, ALPHA_TEST_CMD_OFFSET_MASK, ALPHA_TEST_CMD_OFFSET_SHIFT);
        if (cmd == NULL) { return NULL; }

        NW_ASSERT((*(cmd + 1) & 0xffff) == PICA_REG_FRAGOP_ALPHA_TEST);
        return cmd;
    }
    //! アルファテストのコマンドから Test 設定を取得します。
    static bit32 GetAlphaTest(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_ALPHA_TEST_OFFSET); }

    //@}
    //! @name デプス・カラーマスク
    //@{

    //! デプス・カラーマスクのコマンドを取得します。
    static bit32* GetDepthColorMaskCmd(const nw::h3d::res::MetaData& metaData, const nw::h3d::res::MaterialContent* content)
    {
        if (! HasFragmentOpData(metaData)) { return NULL; }
        bit32* cmd = GetCmd(metaData, content, 1, DEPTH_COLOR_MASK_CMD_OFFSET_MASK, DEPTH_COLOR_MASK_CMD_OFFSET_SHIFT);
        if (cmd == NULL) { return NULL; }

        NW_ASSERT((*(cmd + 1) & 0xffff) == PICA_REG_DEPTH_COLOR_MASK);
        return cmd;
    }
    //! デプス・カラーマスクのコマンドから Mask 設定を取得します。
    static bit32 GetDepthColorMask(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_DEPTH_COLOR_MASK_TEST_OFFSET); }

    //@}
    //! @name ブレンド・論理演算
    //@{

    //! ブレンド・論理演算のコマンドを取得します。
    static bit32* GetBlendLogicOpCmd(const nw::h3d::res::MetaData& metaData, const nw::h3d::res::MaterialContent* content)
    {
        if (! HasFragmentOpData(metaData)) { return NULL; }
        bit32* cmd = GetCmd(metaData, content, 1, BLEND_LOGIC_OP_CMD_OFFSET_MASK, BLEND_LOGIC_OP_CMD_OFFSET_SHIFT);
        if (cmd == NULL) { return NULL; }

        NW_ASSERT((*(cmd + 1) & 0xffff) == PICA_REG_COLOR_OPERATION);
        return cmd;
    }
    //! ブレンド・論理演算のコマンドから ColorOp 設定を取得します。
    static bit32 GetColorOp(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_COLOR_OP_OFFSET); }
    //! ブレンド・論理演算のコマンドから BlendFunc 設定を取得します。
    static bit32 GetBlendFunc(bit32* cmd) { NW_NULL_ASSERT(cmd); return *(cmd + MOD_BLEND_FUNC_OFFSET); }
    //! ブレンド・論理演算のコマンドから LogicOp 設定を取得します。
    //!
    //! ブレンドと論理嫣然が無効の場合に取得できません。fnd::Optional で返り値の有無を確認してください。
    static nw::h3d::fnd::Optional<bit32> GetLogicOp(bit32* cmd)
    {
        if (cmd && ((*(cmd + MOD_LOGIC_OP_OFFSET + 1) & 0xffff) == PICA_REG_LOGIC_OP))
        {
            return *(cmd + MOD_LOGIC_OP_OFFSET);
        }
        return nw::h3d::fnd::Optional<bit32>();
    }
    //! ブレンド・論理演算のコマンドから BlendColor 設定を取得します。
    //!
    //! ブレンドが無効の場合に取得できません。fnd::Optional で返り値の有無を確認してください。
    static nw::h3d::fnd::Optional<bit32> GetBlendColor(bit32* cmd)
    {
        if (cmd && ((*(cmd + MOD_BLEND_COLOR_OFFSET + 1) & 0xffff) == PICA_REG_BLEND_COLOR))
        {
            return *(cmd + MOD_BLEND_COLOR_OFFSET);
        }
        return nw::h3d::fnd::Optional<bit32>();
    }

    //@}

private:
    enum
    {
        MOD_INVALID_OFFSET = 255,
        MOD_COMBINER_STAGE_CMD_SIZE = 6,
        MOD_COMBINER_BUFFER_OFFSET = MOD_COMBINER_STAGE_CMD_SIZE * nw::h3d::Constants::COMBINER_QUANTITY,

        MOD_COMBINER_SOURCE_OFFSET = 0,
        MOD_COMBINER_OPERAND_OFFSET = 2,
        MOD_COMBINER_COMBINE_OFFSET = 3,
        MOD_COMBINER_CONST_OFFSET = 4,
        MOD_COMBINER_SCALE_OFFSET = 5,

        MOD_COMBINER_BUFFER_INPUT_OFFSET = 0,
        MOD_COMBINER_BUFFER_COLOR_OFFSET = 2,

        MOD_STENCIL_TEST_OFFSET = 0,
        MOD_STENCIL_OP_OFFSET = 2,
        MOD_ALPHA_TEST_OFFSET = 0,
        MOD_DEPTH_COLOR_MASK_TEST_OFFSET = 0,
        MOD_COLOR_OP_OFFSET = 0,
        MOD_BLEND_FUNC_OFFSET = 2,
        MOD_LOGIC_OP_OFFSET = 4,
        MOD_BLEND_COLOR_OFFSET = 6
    };

    static bit32* GetCmd(
        const nw::h3d::res::MetaData& metaData,
        const nw::h3d::res::MaterialContent* content,
        int dataIdx,
        u32 mask,
        u32 shift)
    {
        NW_NULL_ASSERT(content);
        int idx = (static_cast<int*>(metaData.data)[dataIdx] & mask) >> shift;
        if (idx < 0 || MOD_INVALID_OFFSET <= idx) { return NULL; }

        return content->paramCmdSrc + idx;
    }

    static u16 GetCombinerTopAddress(int combinerId) { return PICA_REG_TEX_ENV0 + combinerId * 8 + ((combinerId > 3) ? 0x10 : 0); }

};



}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE



#endif // __GFL_G3DH3DEX_H__

