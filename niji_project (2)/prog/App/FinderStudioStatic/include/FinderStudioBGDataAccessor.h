#if !defined(__FINDER_STUDIO_BG_DATA_ACCESSOR_H_INCLUDED__)
#define __FINDER_STUDIO_BG_DATA_ACCESSOR_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioBGDataAccessor.h
 * @date    2016/12/28 11:31:40
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：背景データアクセサー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioStatic/include/FinderStudioViewerDefine.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <pml/include/pmlib.h>

#include <niji_conv_header/app/data/finder_studio/finder_studio_bg_def.h>
#include <niji_conv_header/app/data/finder_studio/finder_studio_time_zone_flg_def.h>
#include <niji_conv_header/battle/background/bgsys_normal_ground_def.h>
#include <niji_conv_header/battle/background/bgsys_near_ground_def.h>
#include <niji_conv_header/battle/background/bgsys_far_ground_def.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)

//////////////////////////////////////////////////////////////////////////
///
/// @brief    背景データアクセサー
///
//////////////////////////////////////////////////////////////////////////
class BGDataAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( BGDataAccessor ); //コピーコンストラクタ＋代入禁止

public:
  /// Viewer用データ構造体
  struct ViewerData
  {
    u32 groundID;
    u32 nearID;
    u32 farID;
  };
  /// UI用データ構造体
  struct UIData
  {
    u32 msgID;
    u32 texID;
    u8  timeZoneFlg;
    u32 unlockFlg;
  };
  /// 世代跨ぎ背景構造体
  struct GenerationBGData
  {
    u32 version;
    u32 bgID;
  };

public:
  //-------------------------------------------------------
  // @brief   データ取得関数
  //-------------------------------------------------------
  static ViewerData GetViewerData( u32 bgID );
  static UIData     GetUIData( u32 bgID );

  static Viewer::TimeZone GetDefaultTimeZone( u32 bgID );

  //-------------------------------------------------------
  // @brief   チェック関数
  //-------------------------------------------------------
  static bool IsUnlock( u32 bgID );
  static bool IsOnlyTimeZone( u32 bgID );

private:
  //-------------------------------------------------------
  // @brief   データ取得関数：実体
  //-------------------------------------------------------
  static ViewerData _GetViewerData( u32 bgID );
  static UIData     _GetUIData( u32 bgID );


public:
  //-------------------------------------------------------
  // @brief   世代跨ぎ背景関連
  //-------------------------------------------------------
  static u32  GetPokemonVersion( const pml::pokepara::PokemonParam* pp ) { return _GetPokemonVersion( pp ); }
  static u32  GetGenerationBackGroundID( u32 version ) { return _GetGenerationBackGroundID( version ); }
  static bool IsGenerationPokemon( const pml::pokepara::PokemonParam* pp ) { return _IsGenerationPokemon( pp ); }
  static bool IsGenerationPokemon( u32 version ) { return _IsGenerationPokemon( version ); }
  static bool IsGenerationBackGround( u32 bgID ) { return _IsGenerationBackGround( bgID ); }
  static bool IsCompareGenrationBackGround( u32 version, u32 bgID ) { return _IsCompareGenrationBackGround( version, bgID ); }

private:
  //-------------------------------------------------------
  // @brief   世代跨ぎ背景関連：実体
  //-------------------------------------------------------
  static u32  _GetPokemonVersion( const pml::pokepara::PokemonParam* pp );
  static u32  _GetGenerationBackGroundID( u32 version );
  static bool _IsGenerationPokemon( const pml::pokepara::PokemonParam* pp );
  static bool _IsGenerationPokemon( u32 version );
  static bool _IsGenerationBackGround( u32 bgID );
  static bool _IsCompareGenrationBackGround( u32 version, u32 bgID );

private:
  static const u32 GENERATION_BG_DATA_NUM = 25;

  static GenerationBGData   m_GenerationBGDataList[GENERATION_BG_DATA_NUM];
};

GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END(App)

#endif // __FINDER_STUDIO_BG_DATA_ACCESSOR_H_INCLUDED__
