//======================================================================
/**
 * @file    FinderStudioBGDataAccessor.cpp
 * @date    2016/12/28 11:54:09
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：背景データアクセサー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioStatic/include/FinderStudioBGDataAccessor.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <debug/include/gfl2_Assert.h>
#include <util/include/gfl2_std_string.h>

#include <niji_conv_header/app/studio/FinderStudioCapture.h>
#include <niji_conv_header/app/data/finder_studio/finder_studio_time_zone_flg_def.h>
#include <niji_conv_header/battle/background/bgsys_normal_ground_def.h>
#include <niji_conv_header/battle/background/bgsys_near_ground_def.h>
#include <niji_conv_header/battle/background/bgsys_far_ground_def.h>
#include <niji_conv_header/message/msg_fs_photo.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )

//////////////////////////////////////////////////////////////////////////
/// @brief    背景データリスト定義ファイルのインクルード
//////////////////////////////////////////////////////////////////////////
#include <niji_conv_header/app/data/finder_studio/finder_studio_bg_data.cdat>

//////////////////////////////////////////////////////////////////////////
/// @brief    世代跨ぎ背景データリスト
//////////////////////////////////////////////////////////////////////////
BGDataAccessor::GenerationBGData BGDataAccessor::m_GenerationBGDataList[BGDataAccessor::GENERATION_BG_DATA_NUM] = {
  { VERSION_SAPPHIRE,     BG_ID_57 },   //!< AGB サファイア       海底洞窟
  { VERSION_RUBY,         BG_ID_57 },   //!< AGB ルビー           海底洞窟
  { VERSION_EMERALD,      BG_ID_57 },   //!< AGB エメラルド       海底洞窟
  { VERSION_RED,          BG_ID_55 },   //!< AGB ファイアーレッド セキエイ高原
  { VERSION_GREEN,        BG_ID_55 },   //!< AGB リーフグリーン   セキエイ高原
  { VERSION_GOLD,         BG_ID_56 },   //!< DS  ハートゴールド   スズの搭
  { VERSION_SILVER,       BG_ID_56 },   //!< DS  ソウルシルバー   スズの搭
  { VERSION_DIAMOND,      BG_ID_58 },   //!< DS  ダイヤモンド     やりのはしら
  { VERSION_PEARL,        BG_ID_58 },   //!< DS  パール           やりのはしら
  { VERSION_PLATINUM,     BG_ID_58 },   //!< DS  プラチナ         やりのはしら
  { VERSION_WHITE,        BG_ID_59 },   //!< DS  ホワイト         スカイアローブリッジ
  { VERSION_BLACK,        BG_ID_59 },   //!< DS  ブラック         スカイアローブリッジ
  { VERSION_W2,           BG_ID_59 },   //!< DS  ホワイト２       スカイアローブリッジ
  { VERSION_B2,           BG_ID_59 },   //!< DS  ブラック２       スカイアローブリッジ
  { VERSION_X,            BG_ID_60 },   //!< 3DS X                ミアレシティ
  { VERSION_Y,            BG_ID_60 },   //!< 3DS Y                ミアレシティ
  { VERSION_CTR_SAPPHIRE, BG_ID_57 },   //!< 3DS アルファサファイア  海底洞窟
  { VERSION_CTR_RUBY,     BG_ID_57 },   //!< 3DS オメガルビー        海底洞窟
  { VERSION_VC_RED,       BG_ID_55 },   //!< VC  赤               セキエイ高原
  { VERSION_VC_GREEN,     BG_ID_55 },   //!< VC  緑               セキエイ高原
  { VERSION_VC_BLUE,      BG_ID_55 },   //!< VC  青               セキエイ高原
  { VERSION_VC_PIKACHU,   BG_ID_55 },   //!< VC  ピカチュウ       セキエイ高原
  { VERSION_VC_GOLD,      BG_ID_56 },   //!< VC  金               スズの搭
  { VERSION_VC_SILVER,    BG_ID_56 },   //!< VC  銀               スズの搭
  { VERSION_VC_CRISTAL,   BG_ID_56 },   //!< VC  クリスタル       スズの搭
};




//////////////////////////////////////////////////////////////////////////
///
/// @brief    データ取得
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   Viewer用のデータを取得
 */
BGDataAccessor::ViewerData BGDataAccessor::GetViewerData( u32 bgID )
{
  return _GetViewerData( bgID );
}

/**
 * @brief   UI用のデータを取得
 */
BGDataAccessor::UIData BGDataAccessor::GetUIData( u32 bgID )
{
  return _GetUIData( bgID );
}

/**
 * @brief   Viewer用のデータを取得：実体
 */
BGDataAccessor::ViewerData BGDataAccessor::_GetViewerData( u32 bgID )
{
  ViewerData data;
  gfl2::std::MemClear( &data, sizeof( ViewerData ) );

  if( bgID >= BG_ID_NUM )
  {
    GFL_ASSERT( 0 );

    return data;
  }

  const BGData bgData = c_BGDataList[bgID];
  {
    data.groundID = bgData.ground_id;
    data.nearID = bgData.near_id;
    data.farID = bgData.far_id;
  }

  return data;
}

/**
 * @brief   UI用のデータを取得：実体
 */
BGDataAccessor::UIData BGDataAccessor::_GetUIData( u32 bgID )
{
  UIData data;
  gfl2::std::MemClear( &data, sizeof( UIData ) );

  if( bgID >= BG_ID_NUM )
  {
    GFL_ASSERT( 0 );

    return data;
  }

  const BGData bgData = c_BGDataList[bgID];
  {
    data.msgID = bgData.msg_id;
    data.texID = bgData.tex_id;
    data.timeZoneFlg = bgData.time_zone_flg;
    data.unlockFlg = bgData.flg_id;
  }

  return data;
}

/**
 * @brief   取得：デフォルトの時間帯
 *
 * @param[in]   bgID    背景ID
 *
 * @retval  時間帯
 */
Viewer::TimeZone BGDataAccessor::GetDefaultTimeZone( u32 bgID )
{
  // UIデータの取得
  UIData data = _GetUIData( bgID );

  // 朝かどうか
  if( ( data.timeZoneFlg & TIME_ZONE_FLG_NOON ) )
  {
    return Viewer::TIME_ZONE_NOON;
  }
  // 夕かどうか
  else if( ( data.timeZoneFlg & TIME_ZONE_FLG_EVENING ) )
  {
    return Viewer::TIME_ZONE_EVENING;
  }
  // 夜かどうか
  else if( ( data.timeZoneFlg & TIME_ZONE_FLG_NIGHT ) )
  {
    return Viewer::TIME_ZONE_NIGHT;
  }

  GFL_ASSERT(0);    //!< ここへはこないはず

  return Viewer::TIME_ZONE_NOON;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    チェック関数
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   背景が開放されているか
 *
 * @param   bgID    背景ID
 *
 * @retval  "true"なら開放済み
 * @retval  "false"なら未開放
 */
bool BGDataAccessor::IsUnlock( u32 bgID )
{
  // 指定背景のUIデータを取得
  UIData data = _GetUIData( bgID );

  // イベントワークの取得
  GameSys::GameData* gameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  Field::EventWork* eventWork = gameData->GetEventWork();

  // イベントワークのチェック
  bool result = eventWork->CheckEventFlag( data.unlockFlg );

  FUKUSHIMA_PRINT( "IsUnlock : bgID=[%d] flagID=[%d] result=[%d]\n", bgID, data.unlockFlg, result );

  return result;
}

/**
 * @brief   選択できる時間帯が１つのみかどうか
 *
 * @param[in]   bgID      背景インデックス(0～BG_ID_NUM)
 *
 * @retval  "true"なら１つのみ
 * @retval  "false"なら２つ以上
 */
bool BGDataAccessor::IsOnlyTimeZone( u32 bgID )
{
  // 指定背景のUIデータを取得
  UIData data = _GetUIData( bgID );

  if( data.timeZoneFlg == TIME_ZONE_FLG_NOON    ||
      data.timeZoneFlg == TIME_ZONE_FLG_EVENING ||
      data.timeZoneFlg == TIME_ZONE_FLG_NIGHT )
  {
    return true;
  }

  return false;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    世代跨ぎ関連
///
/// @note     カセットバージョンに応じて使用できる背景の仕様
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   捕獲したカセットバージョンを取得
 *
 * @param[in]   pp    ポケモンパラメータ
 *
 * @retval  カセットバージョン
 */
u32 BGDataAccessor::_GetPokemonVersion( const pml::pokepara::PokemonParam* pp )
{
  u32 version = PM_VERSION;

  if( pp == NULL )
  {
    GFL_ASSERT( 0 );
    return version;
  }

  bool fastmode = pp->StartFastMode();
  {
    /// カセットバージョンを取得
    version = pp->GetCassetteVersion();
  }
  pp->EndFastMode( fastmode );

  return version;
}

/**
 * @brief   世代ごとの背景を取得
 *
 * @param[in]   version   カセットバージョン
 *
 * @retval  背景ID
 *          リストにないバージョンの時はBG_ID_NUMを返す
 */
u32 BGDataAccessor::_GetGenerationBackGroundID( u32 version )
{
  for( u32 i = 0; i < GENERATION_BG_DATA_NUM; ++i )
  {
    GenerationBGData data = m_GenerationBGDataList[i];

    /// 世代跨ぎバージョン比較チェック
    if( version == data.version )
    {
      return data.bgID;
    }
  }

  return BG_ID_NUM;
}

/**
 * @brief   指定のポケモンが世代跨ぎかどうか
 *
 * @param[in]   pp    ポケモンパラメータ
 *
 * @retval  "true"なら世代跨ぎ
 * @retval  "false"ならそれ以外
 */
bool BGDataAccessor::_IsGenerationPokemon( const pml::pokepara::PokemonParam* pp )
{
  /// ポケモンを捕獲したカセットバージョンを取得
  u32 version = _GetPokemonVersion( pp );

  return _IsGenerationPokemon( version );
}

/**
 * @brief   指定のポケモンが世代跨ぎかどうか
 *
 * @param[in]   version   ポケモンを捕獲したカセットバージョン
 *
 * @retval  "true"なら世代跨ぎ
 * @retval  "false"ならそれ以外
 */
bool BGDataAccessor::_IsGenerationPokemon( u32 version )
{
  for( u32 i = 0; i < GENERATION_BG_DATA_NUM; ++i )
  {
    GenerationBGData data = m_GenerationBGDataList[i];

    /// 世代跨ぎバージョン比較チェック
    if( version == data.version )
    {
      return true;
    }
  }

  return false;
}

/**
 * @brief   世代跨ぎ背景かどうか
 *
 * @param[in]   bgID    背景ID
 *
 * @retval  "true"なら世代跨ぎ背景
 * @retval  "false"ならそれ以外
 */
bool BGDataAccessor::_IsGenerationBackGround( u32 bgID )
{
  /// 世代跨ぎ背景リスト
  const u32 c_GenerationBGList[] = {
    BG_ID_55,   //!< セキエイ高原
    BG_ID_56,   //!< スズの塔
    BG_ID_57,   //!< 海底
    BG_ID_58,   //!< やりのはしら
    BG_ID_59,   //!< スカイアローブリッジ
    BG_ID_60,   //!< ミアレシティ
  };

  for( u32 i = 0; i < GFL_NELEMS( c_GenerationBGList ); ++i )
  {
    /// 世代跨ぎ背景かどうか
    if( bgID == c_GenerationBGList[i] )
    {
      return true;
    }
  }

  return false;
}

/**
 * @brief   世代にあった背景かどうか
 *
 * @param[in]   version   選択中のポケモンのバージョン
 * @param[in]   bgID      対象の背景ID
 *
 * @retval  "true"なら世代にあっている
 * @retval  "false"ならそれ以外
 */
bool BGDataAccessor::_IsCompareGenrationBackGround( u32 version, u32 bgID )
{
  for( u32 i = 0; i < GENERATION_BG_DATA_NUM; ++i )
  {
    GenerationBGData data = m_GenerationBGDataList[i];

    /// 世代跨ぎバージョン比較チェック
    if( version == data.version )
    {
      return ( bgID == data.bgID );
    }
  }

  return false;
}

GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
