#if !defined( __APP_RESORT_LOWER_KINOMI_GET_EFFECT_DRAW_H_INCLUDED__ )
#define __APP_RESORT_LOWER_KINOMI_GET_EFFECT_DRAW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    ResortLowerKinomiGetEffectDraw.h
 * @date    2016/01/22 20:38:05
 * @author  fukushima_yuya
 * @brief   ポケリゾート：きのみ取得エフェクト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortLowerBaseDraw.h"

// gflib2
#include <Layout/include/gfl2_Layout.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
// @brief   ポケリゾート：きのみ取得エフェクト
//------------------------------------------------------------------------------
class LowerKinomiGetEffectDraw
  : public LowerBaseDraw
{
  GFL_FORBID_COPY_AND_ASSIGN(LowerKinomiGetEffectDraw);

public:
  enum ItemType
  {
    ITEM_TYPE_KINOMI,
    ITEM_TYPE_MAME,
    ITEM_TYPE_NUM,
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   生成パラメータ
   */
  //------------------------------------------------------------------
  LowerKinomiGetEffectDraw( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~LowerKinomiGetEffectDraw( void ) { ; }

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  virtual void Initialize2D( InitParam& param );
  
  //------------------------------------------------------------------
  /**
   * @brief   クラスの初期化
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  virtual void InitializeClass( InitParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   ペインの初期化
   */
  //------------------------------------------------------------------
  virtual void InitializePane( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   エフェクト再生
   *
   * @param   type    取得するアイテムの種類
   * @param   index   きのみのインデックス
   * @param   x       X座標
   * @param   y       Y座標
   */
  //------------------------------------------------------------------
  void PlayGetEffect( ItemType type, int index, int x, int y );

  //------------------------------------------------------------------
  /**
   * @brief   エフェクト再生の終了チェック
   *
   * @return  "true  == 終了"
   * @return  "false == 再生中"
   */
  //------------------------------------------------------------------
  bool IsAnimEndGetEffect( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   エフェクト再生：きのみ取得
   *
   * @param   index   きのみのインデックス
   * @param   x       X座標
   * @param   y       Y座標
   */
  //------------------------------------------------------------------
  void PlayGetEffect_Kinomi( int index, int x, int y );

  //------------------------------------------------------------------
  /**
   * @brief   エフェクト再生：マメ取得
   *
   * @param   x       X座標
   * @param   y       Y座標
   */
  //------------------------------------------------------------------
  void PlayGetEffect_Mame( int x, int y );

private:
  bool  m_DiaryFlag;

private:
  gfl2::lyt::LytPane*     m_pCenterPane;

  gfl2::lyt::LytPicture*  m_pKinomiIconPane;

  gfl2::lyt::LytPicture*  m_pMameIconPane;

  gfl2::lyt::LytTextBox*  m_pTextGetNum;
};

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)

#endif // __APP_RESORT_LOWER_KINOMI_GET_EFFECT_DRAW_H_INCLUDED__