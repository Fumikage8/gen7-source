/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gx_MacroFragment.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GX_CTR_GX_MACRO_FRAGMENT_H_
#define NN_GX_CTR_GX_MACRO_FRAGMENT_H_

#include <nn/gx/CTR/gx_MacroCommon.h>

/*!
  @addtogroup   nn_gx_CTR_Pica   PicaMacroHeader
  @{
*/

// PICA_REG_FRAG_LIGHT_EN0           0x08f
// PICA_REG_FRAG_LIGHT_EN1           0x1c6
#define PICA_CMD_DATA_FRAG_LIGHT_EN(flag) ( (flag) ? 1 : 0 )
#define PICA_CMD_DATA_FRAG_LIGHT_EN_INV(flag) ( (flag) ? 0 : 1 )

#define PICA_CMD_SET_FRAG_LIGHT_ENABLE(flag) \
    PICA_CMD_DATA_FRAG_LIGHT_EN(flag), PICA_CMD_HEADER_SINGLE(PICA_REG_FRAG_LIGHT_EN0), \
    PICA_CMD_DATA_FRAG_LIGHT_EN_INV(flag), PICA_CMD_HEADER_SINGLE(PICA_REG_FRAG_LIGHT_EN1)
    
// PICA_REG_FRAG_LIGHT_SRC_NUM     0x1c2
#define PICA_CMD_DATA_FRAG_LIGHT_NUM(num) ( (num > 0) ? (num - 1) : 0 )

#define PICA_CMD_SET_FRAG_LIGHT_NUM(num) \
    PICA_CMD_DATA_FRAG_LIGHT_NUM(num), PICA_CMD_HEADER_SINGLE( PICA_REG_FRAG_LIGHT_SRC_NUM )

/*
struct CommandFragLightSrc
{
    u32 Id1   : 3;
    u32 Id2   : 3;
    u32 Id3   : 3;
    u32 Id4   : 3;
    u32 Id5   : 3;
    u32 Id6   : 3;
    u32 Id7   : 3;
    u32 Id8   : 3;
    CMD_PADDING(8);
};
*/

// PICA_REG_FRAG_LIGHT_SRC_EN_ID      0x1d9
#define PICA_CMD_DATA_FRAG_LIGHT_EN_ID(Id1, Id2, Id3, Id4, Id5, Id6, Id7, Id8) \
    ( (Id1)       | (Id2) <<  4 | (Id3) <<  8 | (Id4) << 12 | \
      (Id5) << 16 | (Id6) << 20 | (Id7) << 24 | (Id8) << 28 )

///////////////////////////////////
// Fragment Global Ambient
// Fragment LightEnv MiscGlobal

// PICA_REG_FRAG_LIGHT_AMBIENT     0x1c0
#define PICA_CMD_DATA_FRAG_LIGHT_AMBIENT(color) \
    ( (color.b) | (color.g) << 10 | (color.r) << 20 )

///////////////////////////////////
// Fragment Light Source
// Fragment LightEnv Misc

// PICA_REG_FRAG_LIGHT0_SPECULAR0       0x140 to 0x1b0
// PICA_REG_FRAG_LIGHT0_SPECULAR1       0x141 to 0x1b1
// PICA_REG_FRAG_LIGHT0_DIFFUSE       0x142 to 0x1b2
// PICA_REG_FRAG_LIGHT0_AMBIENT        0x143 to 0x1b3
#define PICA_CMD_DATA_FRAG_LIGHT_SRC(color) \
    ( (color.b) | (color.g) << 10 | (color.r) << 20 )

/*
struct CommandFragLightPositionXY
{
    u32 x   : 16;
    u32 y   : 16;
};
*/
// PICA_REG_FRAG_LIGHT0_POSITION_XY         0x144 to 0x1b4
#define PICA_CMD_DATA_FRAG_LIGHT_POSITION_XY(x, y) \
    ( (x) | (y) << 16 )

// PICA_REG_FRAG_LIGHT0_SPOT_XY    0x146 to 0x1b6
#define PICA_CMD_DATA_FRAG_LIGHT_SPOT_XY(x, y) \
    ( (x) | (y) << 16 )

/*
struct CommandFragLightPositionZ
{
    u32 z   : 16;
    CMD_PADDING(16);
};
*/

// PICA_REG_FRAG_LIGHT0_POSITION_Z          0x145 to 0x1b5
#define PICA_CMD_DATA_FRAG_LIGHT_POSITION_Z(z) ( z )

// PICA_REG_FRAG_LIGHT0_SPOT_Z     0x147 to 0x1b7
#define PICA_CMD_DATA_FRAG_LIGHT_SPOT_Z(z) ( z )

//  PICA_REG_FRAG_LIGHT0_DIST_ATTN_BIAS       0x14a to 0x1ba
#define PICA_CMD_DATA_FRAG_LIGHT_DIST_ATTN_BIAS(bias) ( bias )

// PICA_REG_FRAG_LIGHT0_DIST_ATTN_SCALE       0x14b to 0x1bb
#define PICA_CMD_DATA_FRAG_LIGHT_DIST_ATTN_SCALE(scale) ( scale )


/*
struct CommandFragLightType
{
    u32 w              : 1;
    u32 twoSideDiffuse : 1;
    u32 geomFactor0    : 1;
    u32 geomFactor1    : 1;
    CMD_PADDING(28);
};
*/
// PICA_REG_FRAG_LIGHT0_TYPE       0x149 to 0x1b9
#define PICA_CMD_DATA_FRAG_LIGHT_TYPE(w, twoSideDiffuse, geomFactor0, geomFactor1) \
    ( ((w) == 0) ? 1 : 0              | \
      ((twoSideDiffuse) ? 1 : 0) << 1 | \
      ((geomFactor0) ? 1 : 0)    << 2 | \
      ((geomFactor1) ? 1 : 0)    << 3)
 
/*
struct CommandFragLightFuncMode1
{
    u32 shadowed                   : 8;
    u32 spotEnabled                : 8;
    u32 lutEnabledD0               : 1;
    u32 lutEnabledD1               : 1;
    CMD_PADDING(1);
    u32 fresnelSelector            : 1;
    u32 lutEnabledRefl             : 3;
    CMD_PADDING(1);
    u32 distanceAttenuationEnabled : 8;
};
*/

// PICA_REG_FRAG_LIGHT_FUNC_MODE1       0x1c4
// i is the number of light source number
#define PICA_CMD_DATA_FRAG_LIGHT_FUNC_MODE1(shadowed, spotEnabled, lutEnabledD0,  \
    lutEnabledD1, fresnelSelector, lutEnabledRefl, distanceAttenuationEnabled, i) \
    ( ((shadowed) ? 0 : 1)        << i   | \
      ((spotEnabled) ? 0 : 1)     << 8+i | \
      ((lutEnabledD0) ? 0 : 1)    << 16  | \
      ((lutEnabledD1) ? 0 : 1)    << 17  | \
      1                           << 18  | \
      ((fresnelSelector) ? 0 : 1) << 19  | \
      ((lutEnabledRefl) ? 0 : 7)  << 20  | \
      ((distanceAttenuationEnabled) ? 0 : 1) << 24+i )

#define PICA_CMD_DATA_FRAG_LIGHT_FUNC_MODE1_LIGHT_SOURCE(i, \
    shadowed, spotEnabled, distanceAttenuationEnabled)      \
    ( ((shadowed) ? 0 : 1)        << i   |                  \
      ((spotEnabled) ? 0 : 1)     << 8+i |                  \
      1                           << 18  |                  \
      ((distanceAttenuationEnabled) ? 0 : 1) << 24+i )

#define PICA_CMD_DATA_FRAG_LIGHT_FUNC_MODE1_LUT(lutEnabledD0, \
    lutEnabledD1, fresnelSelector, lutEnabledRefl)            \
    ( ((lutEnabledD0) ? 0 : 1)    << 16 |                     \
      ((lutEnabledD1) ? 0 : 1)    << 17 |                     \
      1 << 18                           |                     \
      ((fresnelSelector) ? 0 : 1) << 19 |                     \
      ((lutEnabledRefl) ? 0 : 7)  << 20 )

///////////////////////////////////
// Fragment light Lut Table
/*
struct CommandFragLightLut
{
    u32 index  : 8;
    u32 type   : 4;
    CMD_PADDING(20);
};
*/

/*!
    @brief フラグメントライティングの参照テーブルの種類です。
           PICA_REG_FRAG_LIGHT_LUT レジスタ( 0x1c5 ) で使います。
*/
enum PicaDataFragLightSampler
{
    //! dmp_FragmentMaterial.samplerD0 です。
    PICA_DATA_SAMPLER_D0 = 0x0,
    //! dmp_FragmentMaterial.samplerD1 です。
    PICA_DATA_SAMPLER_D1 = 0x1,
    //! dmp_FragmentMaterial.samplerFR です。
    PICA_DATA_SAMPLER_FR = 0x3,
    //! dmp_FragmentMaterial.samplerRB です。
    PICA_DATA_SAMPLER_RB = 0x4,
    //! dmp_FragmentMaterial.samplerRG です。
    PICA_DATA_SAMPLER_RG = 0x5,
    //! dmp_FragmentMaterial.samplerRR です。
    PICA_DATA_SAMPLER_RR = 0x6,
    
    //! dmp_FragmentLightSource.samplerSP です。
    PICA_DATA_SAMPLER_SP = 0x8,
    //! dmp_FragmentLightSource.samplerSP[0] です。
    PICA_DATA_FRAG_LIGHT0_SAMPLER_SP = 0x8 + 0x0,
    //! dmp_FragmentLightSource.samplerSP[1] です。
    PICA_DATA_FRAG_LIGHT1_SAMPLER_SP = 0x8 + 0x1,
    //! dmp_FragmentLightSource.samplerSP[2] です。
    PICA_DATA_FRAG_LIGHT2_SAMPLER_SP = 0x8 + 0x2,
    //! dmp_FragmentLightSource.samplerSP[3] です。
    PICA_DATA_FRAG_LIGHT3_SAMPLER_SP = 0x8 + 0x3,
    //! dmp_FragmentLightSource.samplerSP[4] です。
    PICA_DATA_FRAG_LIGHT4_SAMPLER_SP = 0x8 + 0x4,
    //! dmp_FragmentLightSource.samplerSP[5] です。
    PICA_DATA_FRAG_LIGHT5_SAMPLER_SP = 0x8 + 0x5,
    //! dmp_FragmentLightSource.samplerSP[6] です。
    PICA_DATA_FRAG_LIGHT6_SAMPLER_SP = 0x8 + 0x6,
    //! dmp_FragmentLightSource.samplerSP[7] です。
    PICA_DATA_FRAG_LIGHT7_SAMPLER_SP = 0x8 + 0x7,

    //! dmp_FragmentLightSource.samplerDA です。
    PICA_DATA_SAMPLER_DA  = 0x10,
    //! dmp_FragmentLightSource.samplerDA[0] です。
    PICA_DATA_FRAG_LIGHT0_SAMPLER_DA = 0x10 + 0x0,
    //! dmp_FragmentLightSource.samplerDA[1] です。
    PICA_DATA_FRAG_LIGHT1_SAMPLER_DA = 0x10 + 0x1,
    //! dmp_FragmentLightSource.samplerDA[2] です。
    PICA_DATA_FRAG_LIGHT2_SAMPLER_DA = 0x10 + 0x2,
    //! dmp_FragmentLightSource.samplerDA[3] です。
    PICA_DATA_FRAG_LIGHT3_SAMPLER_DA = 0x10 + 0x3,
    //! dmp_FragmentLightSource.samplerDA[4] です。
    PICA_DATA_FRAG_LIGHT4_SAMPLER_DA = 0x10 + 0x4,
    //! dmp_FragmentLightSource.samplerDA[5] です。
    PICA_DATA_FRAG_LIGHT5_SAMPLER_DA = 0x10 + 0x5,
    //! dmp_FragmentLightSource.samplerDA[6] です。
    PICA_DATA_FRAG_LIGHT6_SAMPLER_DA = 0x10 + 0x6,
    //! dmp_FragmentLightSource.samplerDA[7] です。
    PICA_DATA_FRAG_LIGHT7_SAMPLER_DA = 0x10 + 0x7
};
// PICA_REG_FRAG_LIGHT_LUT          0x1c5
#define PICA_CMD_DATA_FRAG_LIGHT_LUT(index, type) \
    ( (index) | (type) << 8)
    
/*
struct CommandFragLightLutData
{
    u32 d1  : 12;
    u32 d2  : 12;
    CMD_PADDING(8);
};
*/
// PICA_REG_FRAG_LIGHT_LUT_DATA0   0x1c8 to 0x1cf
#define PICA_CMD_DATA_FRAG_LIGHT_LUT_DATA(data1, data2) \
    ( (data1) | (data2) << 12 )

///////////////////////////////////
// Fragment LightEnv absLutInput
/*
struct CommandFragLightEnvAbsLutInput
{
    CMD_PADDING(1);
    u32 D0  : 1;
    CMD_PADDING(3);
    u32 D1  : 1;
    CMD_PADDING(3);
    u32 SP  : 1;
    CMD_PADDING(3);
    u32 FR  : 1;
    CMD_PADDING(3);
    u32 RB  : 1;
    CMD_PADDING(3);
    u32 RG  : 1;
    CMD_PADDING(3);
    u32 RR  : 1;
    CMD_PADDING(6);
};
*/
// PICA_REG_FRAG_LIGHT_ABSLUTINPUT  0x1d0
#define PICA_CMD_DATA_FRAG_LIGHT_ABSLUTINPUT(D0, D1, SP, FR, RB, RG, RR) \
    ( ((D0) ? 0 : 1) <<  1 | \
      ((D1) ? 0 : 1) <<  5 | \
      ((SP) ? 0 : 1) <<  9 | \
      ((FR) ? 0 : 1) << 13 | \
      ((RB) ? 0 : 1) << 17 | \
      ((RG) ? 0 : 1) << 21 | \
      ((RR) ? 0 : 1) << 25 )

///////////////////////////////////
// Fragment LightEnv LutInput
/*
struct CommandFragLightEnvLutInput
{
    u32 D0  : 3;
    CMD_PADDING(1);
    u32 D1  : 3;
    CMD_PADDING(1);
    u32 SP  : 3;
    CMD_PADDING(1);
    u32 FR  : 3;
    CMD_PADDING(1);
    u32 RB  : 3;
    CMD_PADDING(1);
    u32 RG  : 3;
    CMD_PADDING(1);
    u32 RR  : 3;
    CMD_PADDING(5);
};
*/

/*!
    @brief 参照テーブルの入力値を設定するのに使い、PICA_REG_FRAG_LIGHT_LUTINPUT レジスタ ( 0x1d1 ) に対して使用します。
           dmp_LightEnv.lutInput* のユニフォームを設定することに相当します。    
*/
enum PicaDataFragLightEnvLutInput
{
    //! 法線とハーフベクトルのなす角の cos です。
    PICA_DATA_FRAG_LIGHT_ENV_NH_DMP = 0x0,
    //! 視線とハーフベクトルのなす角の cos です。
    PICA_DATA_FRAG_LIGHT_ENV_VH_DMP = 0x1,
    //! 法線と視線のなす角の cos です。
    PICA_DATA_FRAG_LIGHT_ENV_NV_DMP = 0x2,
    //! ライトベクトルと法線のなす角の cos です。
    PICA_DATA_FRAG_LIGHT_ENV_LN_DMP = 0x3,
    //! ライトベクトルの逆ベクトルとスポットの方向ベクトルのなす角の cos です。
    PICA_DATA_FRAG_LIGHT_ENV_SP_DMP = 0x4,
    //! 接平面のハーフベクトルの射影と接線がなす角の cos です。
    PICA_DATA_FRAG_LIGHT_ENV_CP_DMP = 0x5
};    

// PICA_REG_FRAG_LIGHT_LUTINPUT    0x1d1
#define PICA_CMD_DATA_FRAG_LIGHT_ENV_LUTINPUT(D0, D1, SP, FR, RB, RG, RR) \
    ( (D0) | (D1) << 4 | (SP) << 8 | \
      (FR) << 12 | (RB) << 16 | (RG) << 20 | (RR) << 24 )
    
///////////////////////////////////
// Fragment LightEnv lutScale
/*
struct CommandFragLightEnvLutScale
{
    u32 D0  : 3;
    CMD_PADDING(1);
    u32 D1  : 3;
    CMD_PADDING(1);
    u32 SP  : 3;
    CMD_PADDING(1);
    u32 FR  : 3;
    CMD_PADDING(1);
    u32 RB  : 3;
    CMD_PADDING(1);
    u32 RG  : 3;
    CMD_PADDING(1);
    u32 RR  : 3;
    CMD_PADDING(5);
};
*/

/*!
    @brief 参照テーブルの出力値のスケーリングを設定するのに使い、PICA_REG_FRAG_LIGHT_LUTSCALE レジスタ ( 0x1d2 ) に対して使用します。
           dmp_LightEnv.lutScale* のユニフォームを設定することに相当します。    
*/
enum PicaDataFragLightEnvLutScale
{
    //! 1.0 を設定します。
    PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_1_0  = 0x0,
    //! 2.0 を設定します。
    PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_2_0  = 0x1,
    //! 4.0 を設定します。
    PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_4_0  = 0x2,
    //! 8.0 を設定します。
    PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_8_0  = 0x3,
    //! 0.25 を設定します。
    PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_0_25 = 0x6,
    //! 0.5 を設定します。
    PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_0_5  = 0x7
}; 

//  PICA_REG_FRAG_LIGHT_LUTSCALE    0x1d2
#define PICA_CMD_DATA_FRAG_LIGHT_ENV_LUTSCALE(D0, D1, SP, FR, RB, RG, RR) \
    ( (D0) | (D1) << 4 | (SP) << 8 | \
      (FR) << 12 | (RB) << 16 | (RG) << 20 | (RR) << 24 )
    

///////////////////////////////////
// Fragment LightEnv Shadow
// Fragment LightEnv Misc
/*
struct CommandFragLightEnvFuncMode0
{
    u32 common          : 1;
    CMD_PADDING(1);
    u32 fresnelSelector : 2;
    u32 config          : 4;
    CMD_PADDING(8);
    u32 shadowPrimary   : 1;
    u32 shadowSecondary : 1;
    u32 invertShadow    : 1;
    u32 shadowAlpha     : 1;
    CMD_PADDING(2);
    u32 bumpSelector    : 2;
    u32 shadowSelector  : 2;
    CMD_PADDING(1);
    u32 clampHighlights : 1;
    u32 bumpMode        : 2;
    u32 bumpRenorm      : 1;
    CMD_PADDING(1);      // 0x1
};
*/

// 0x1c3 [0:0]
#define PICA_CMD_DATA_FRAG_LIGHT_ENV_SHADOW_ATTN(shadowPrimary, shadowSecondary, shadowAlpha) \
    ( ( (shadowPrimary) | (shadowSecondary) | (shadowAlpha) ) ? 1 : 0 )

/*!
    @brief フラグメントライティングのテクスチャを設定するのに使い、    
           PICA_REG_FRAG_LIGHT_FUNC_MODE レジスタ ( 0x1c3[23:22], 0x1c3[25:24] ) に対して使用します。
           dmp_LightEnv.shadowSelector, dmp_LightEnv.bumpSelector のユニフォームを設定することに相当します。    
*/
enum PicaDataFragLightEnvTexture
{
    //! TEXTURE0 を設定します。
    PICA_DATA_FRAG_LIGHT_ENV_TEXTURE0 = 0x0,
    //! TEXTURE1 を設定します。
    PICA_DATA_FRAG_LIGHT_ENV_TEXTURE1 = 0x1,
    //! TEXTURE2 を設定します。
    PICA_DATA_FRAG_LIGHT_ENV_TEXTURE2 = 0x2,
    //! TEXTURE3 を設定します。
    PICA_DATA_FRAG_LIGHT_ENV_TEXTURE3 = 0x3
};

// 0x1c3 [7:4]

/*!
    @brief フラグメントライティングのレイヤーコンフィグを設定するのに使い、
           PICA_REG_FRAG_LIGHT_FUNC_MODE レジスタ ( 0x1c3[7:4] ) に対して使用します。
           dmp_LightEnv.config のユニフォームを設定することに相当します。    
*/
enum PicaDataFragLightEnvLayerConfig
{
    //! レイヤーコンフィグ 0 を設定します。
    PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG0 = 0x0,
    //! レイヤーコンフィグ 1 を設定します。
    PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG1 = 0x1,
    //! レイヤーコンフィグ 2 を設定します。
    PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG2 = 0x2,
    //! レイヤーコンフィグ 3 を設定します。
    PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG3 = 0x3,
    //! レイヤーコンフィグ 4 を設定します。
    PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG4 = 0x4,
    //! レイヤーコンフィグ 5 を設定します。
    PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG5 = 0x5,
    //! レイヤーコンフィグ 6 を設定します。
    PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG6 = 0x6,
    //! レイヤーコンフィグ 7 を設定します。
    PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG7 = 0x8
};

// 0x1c3 [3:2]
/*!
    @brief フラグメントライティングのフレネルファクタを設定するのに使い、
           PICA_REG_FRAG_LIGHT_FUNC_MODE レジスタ ( 0x1c3[3:2] ) に対して使用します。
           dmp_LightEnv.fresnelSelector のユニフォームを設定することに相当します。    
*/
enum PicaDataFragLightEnvFresnel
{
    //! プライマリカラーとセカンダリカラーのアルファ成分は 1.0 のままです。
    PICA_DATA_FRAG_LIGHT_ENV_NO_FRESNEL            = 0x0,
    //! プライマリカラーのアルファ成分だけが Fr で置き換わります。
    PICA_DATA_FRAG_LIGHT_ENV_PRI_ALPHA_FRESNEL     = 0x1,
    //! セカンダリカラーのアルファ成分だけが Fr で置き換わります。
    PICA_DATA_FRAG_LIGHT_ENV_SEC_ALPHA_FRESNEL     = 0x2,
    //! プライマリカラーとセカンダリカラーのアルファ成分は Fr で置き換わります。
    PICA_DATA_FRAG_LIGHT_ENV_PRI_SEC_ALPHA_FRESNEL = 0x3
};

// 0x1c3 [29:28]
/*!
    @brief フラグメントライティング時のバンプマッピングの法線や接線の摂動の設定をするのに使い、
           PICA_REG_FRAG_LIGHT_FUNC_MODE レジスタ ( 0x1c3[29:28] ) に対して使用します。
           dmp_LightEnv.bumpMode のユニフォームを設定することに相当します。    
*/
enum PicaDataFragLightEnvBump
{
    //! 法線と接線は摂動の影響を受けません。
    PICA_DATA_FRAG_LIGHT_ENV_BUMP_NOT_USED_DMP = 0x0,
    //! 法線だけが摂動の影響を受けます。
    PICA_DATA_FRAG_LIGHT_ENV_BUMP_AS_BUMP_DMP  = 0x1,
    //! 接線だけが摂動の影響を受けます。
    PICA_DATA_FRAG_LIGHT_ENV_BUMP_AS_TANG_DMP  = 0x2
};

#define PICA_CMD_DATA_FRAG_LIGHT_ENV_BUMP(bumpMode, bumpRenorm) \
    ( ((bumpRenorm) | (bumpMode ==  PICA_DATA_FRAG_LIGHT_ENV_BUMP_NOT_USED_DMP) ) ? 0 : 1 )

// PICA_REG_FRAG_LIGHT_FUNC_MODE0      0x1c3
#define PICA_CMD_DATA_FRAG_LIGHT_FUNC_MODE0(fresnelSelector, config,          \
     shadowPrimary, shadowSecondary, invertShadow, shadowAlpha, bumpSelector, \
     shadowSelector,  clampHighlights, bumpMode, bumpRenorm)                  \
     ( PICA_CMD_DATA_FRAG_LIGHT_ENV_SHADOW_ATTN(shadowPrimary, shadowSecondary, shadowAlpha) | \
       ( (fresnelSelector)           <<  2) | \
       ( (config)                    <<  4) | \
       ( 4                           <<  8) | \
       ( ((shadowPrimary) ? 1 : 0)   << 16) | \
       ( ((shadowSecondary) ? 1 : 0) << 17) | \
       ( ((invertShadow) ? 1 : 0)    << 18) | \
       ( ((shadowAlpha) ? 1 : 0)     << 19) | \
       ( (bumpSelector)              << 22) | \
       ( (shadowSelector)            << 24) | \
       ( ((clampHighlights) ? 1 : 0) << 27) | \
       ( (bumpMode)                  << 28) | \
       ( PICA_CMD_DATA_FRAG_LIGHT_ENV_BUMP(bumpMode, bumpRenorm) << 30) | \
         0x80000000 )

/*!
  @}
*/

#endif // NN_GX_CTR_GX_MACRO_FRAGMENT_H_
