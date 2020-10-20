#if !defined( __APP_BOX_SEARCH_APP_PARAM_H_INCLUDED__ )
#define __APP_BOX_SEARCH_APP_PARAM_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    BoxSearchAppParam.h
 * @date    2015/07/27 12:57:04
 * @author  fukushima_yuya
 * @brief   ボックス検索：外部設定パラメータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// pml
#include <pml/include/pmlib.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( Fade )
class FadeManager;
GFL_NAMESPACE_END( Fade )
GFL_NAMESPACE_END( gfl2 )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( BoxSearch )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
static const pml::Sex BOX_SEARCH_GENDER_NULL = pml::SEX_NUM;    //!< 性別項目無効
static const pml::pokepara::Seikaku BOX_SEARCH_SEIKAKU_NULL = pml::pokepara::SEIKAKU_NUM;   //!< 性格項目無効

//! @brief  終了モード
enum EndMode
{
  END_MODE_CANCEL,    //!< キャンセル
  END_MODE_SEARCH,    //!< 検索開始
  END_MODE_NUM,
};

//! @brief  条件項目
enum
{
  CONDITIONAL_NULL = 0,   //!< 条件なし
  CONDITIONAL_TRUE,       //!< 正
  CONDITIONAL_FALSE,      //!< 否
  CONDITIONAL_NUM,
};

//------------------------------------------------------------------------------
/**
 * @brief   外部設定パラメータ
 */
//------------------------------------------------------------------------------
struct APP_PARAM
{
  // [in]
  gfl2::Fade::FadeManager*  pFadeManager;   //!< フェードマネージャ

  // [in/out]
  MonsNo monsNo;    //!< ポケモン番号
  WazaNo wazaNo;    //!< わざ番号

  u8 type1;         //!< タイプ１
  u8 type2;         //!< タイプ２
  u8 item;          //!< 持ち物をもっているか
  u8 wazaMachine;   //!< 技マシン番号

  pml::Sex sex;                         //!< 性別
  pml::pokepara::BoxMarkContainer mark; //!< マーキング
  pml::pokepara::Seikaku seikaku;       //!< 性格
  TokuseiNo tokusei;                    //!< 特性

  u8 entry;     //!< チーム登録をしているか
  u8 teamNo;    //!< チーム番号

  // [out]
  EndMode   endMode;

  APP_PARAM( void )
    : pFadeManager( NULL )
    , monsNo( MONSNO_NULL )
    , wazaNo( WAZANO_NULL )
    , type1( POKETYPE_NULL )
    , type2( POKETYPE_NULL )
    , item( CONDITIONAL_NULL )
    , wazaMachine( CONDITIONAL_NULL )
    , sex( BOX_SEARCH_GENDER_NULL )
    , seikaku( BOX_SEARCH_SEIKAKU_NULL )
    , tokusei( TOKUSEI_NULL )
    , entry( CONDITIONAL_NULL )
    , teamNo( CONDITIONAL_NULL )
    , endMode( END_MODE_NUM )
  { 
    for( u32 i=0; i<pml::pokepara::BOX_MARK_NUM; ++i )
    {
      mark.markColor[i] = pml::pokepara::BOX_MARK_COLOR_NONE;
    }
  }
};

GFL_NAMESPACE_END( BoxSearch )
GFL_NAMESPACE_END( App )

#endif // __APP_BOX_SEARCH_APP_PARAM_H_INCLUDED__