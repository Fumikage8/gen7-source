//======================================================================
/**
 * @file FadeUtil.cpp
 * @date 2015/07/23 11:12:57
 * @author miyachi_soichi
 * @brief フェード補助関数群
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// Fade
#include "AppLib/include/Fade/FadeUtil.h"
// Arc
#include "niji_conv_header/app/fade/fade_pane.h"
#include "niji_conv_header/app/fade/fade_anim_list.h"

GFL_NAMESPACE_BEGIN(AppLib)
GFL_NAMESPACE_BEGIN(Fade)
GFL_NAMESPACE_BEGIN(Util)

/**
 *  @brief  フェードアウトに必要なパラメータを取得する
 *  @param  in_eType    ロードするフェードの種類
 *  @param  out_Anim    アニメーション番号
 *  @param  out_Pane    ペイン
 *  @retval true        アニメーション
 *  @retval false       カラー
 */
u32 GetFadeParamForFadeOut( gfl2::Fade::FADE_TYPE in_eType, s32 &out_Anim, s32 &out_Pane )
{
  u32 mode = gfl2::Fade::MODE_ANIME;

  switch( in_eType )
  {
  case gfl2::Fade::FADE_TYPE_ALPHA:
    out_Anim = -1;
    out_Pane = PANENAME_FADE_BG_UPPER_PANE_FADE_ANIME_02;
    mode = gfl2::Fade::MODE_ALPHA;
    break;
  case gfl2::Fade::FADE_TYPE_BALL:
    out_Anim = LA_FADE_FADE_BG_UPPER_WB_OUT;
    out_Pane = PANENAME_FADE_BG_UPPER_PANE_WB_OUT;
    break;
  case gfl2::Fade::FADE_TYPE_MASK:
    out_Anim = LA_FADE_FADE_BG_UPPER_WSK_OUT_00;
    out_Pane = PANENAME_FADE_BG_UPPER_PANE_WSK_OUT;
    break;
  case gfl2::Fade::FADE_TYPE_DIAMOND:
    out_Anim = LA_FADE_FADE_BG_UPPER_WR_OUT;
    out_Pane = PANENAME_FADE_BG_UPPER_PANE_WR_OUT;
    break;
  case gfl2::Fade::FADE_TYPE_CIRCLE:
    out_Anim = LA_FADE_FADE_BG_UPPER_WC_OUT;
    out_Pane = PANENAME_FADE_BG_UPPER_PANE_WC_OUT;
    break;
  case gfl2::Fade::FADE_TYPE_CROSS:
    out_Anim = -1;
    out_Pane = PANENAME_FADE_BG_UPPER_PANE_CROSS_TEXTURE;
    mode = gfl2::Fade::MODE_CROSS;
    break;
  case gfl2::Fade::FADE_TYPE_BALL_TILT:
    out_Anim = LA_FADE_FADE_BG_UPPER_WB2_OUT;
    out_Pane = PANENAME_FADE_BG_UPPER_PANE_WB2_OUT;
    break;
  }

  return mode;
}

/**
 *  @brief  フェードインに必要なパラメータを取得する
 *  @param  in_eType    ロードするフェードの種類
 *  @param  out_Anim    アニメーション番号
 *  @param  out_Pane    ペイン
 *  @retval true        アニメーション
 *  @retval false       カラー
 */
u32 GetFadeParamForFadeIn( gfl2::Fade::FADE_TYPE in_eType, s32 &out_Anim, s32 &out_Pane )
{
  u32 mode = gfl2::Fade::MODE_ANIME;

  switch( in_eType )
  {
  case gfl2::Fade::FADE_TYPE_ALPHA:
    out_Anim = -1;
    out_Pane = PANENAME_FADE_BG_UPPER_PANE_FADE_ANIME_02;
    mode = gfl2::Fade::MODE_ALPHA;
    break;
  case gfl2::Fade::FADE_TYPE_BALL:
    out_Anim = LA_FADE_FADE_BG_UPPER_WB_IN;
    out_Pane = PANENAME_FADE_BG_UPPER_PANE_WB_IN;
    break;
  case gfl2::Fade::FADE_TYPE_MASK:
    out_Anim = LA_FADE_FADE_BG_UPPER_WSK_IN_00;
    out_Pane = PANENAME_FADE_BG_UPPER_PANE_WSK_IN;
    break;
  case gfl2::Fade::FADE_TYPE_DIAMOND:
    out_Anim = LA_FADE_FADE_BG_UPPER_WR_IN;
    out_Pane = PANENAME_FADE_BG_UPPER_PANE_WR_IN;
    break;
  case gfl2::Fade::FADE_TYPE_CIRCLE:
    out_Anim = LA_FADE_FADE_BG_UPPER_WC_IN;
    out_Pane = PANENAME_FADE_BG_UPPER_PANE_WC_IN;
    break;
  case gfl2::Fade::FADE_TYPE_CROSS:
    out_Anim = -1;
    out_Pane = PANENAME_FADE_BG_UPPER_PANE_CROSS_TEXTURE;
    mode = gfl2::Fade::MODE_CROSS;
    break;
  case gfl2::Fade::FADE_TYPE_BALL_TILT:
    out_Anim = LA_FADE_FADE_BG_UPPER_WB2_IN;
    out_Pane = PANENAME_FADE_BG_UPPER_PANE_WB2_IN;
    break;
  }

  return mode;
}

GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(Fade)
GFL_NAMESPACE_END(AppLib)
