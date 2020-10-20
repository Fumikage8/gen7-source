// ============================================================================
/*
 * @file TeamSelectResponseListener.h
 * @brief バトルで使用するチームを検査するリクエストのレスポンスリスナーです。
 * @date 2015.06.23
 */
// ============================================================================
#if !defined( TEAMSELECTRESPONSELISTENER_H_INCLUDE )
#define TEAMSELECTRESPONSELISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
// pmlib
#include <pml/include/pmlib.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)


//-------------------------------------
/// レギュレーション検査種別
//=====================================
enum REGULATION_NG_TYPE
{
  REGULATION_NG_TYPE_NONE    = 0,   ///< エラー無し
  REGULATION_NG_TYPE_EGG        ,   ///< タマゴチェック						
  REGULATION_NG_TYPE_NUM_LOW    ,   ///< ポケモン数下限チェック				
  REGULATION_NG_TYPE_NUM_HI     ,   ///< ポケモン数上限チェック				
  REGULATION_NG_TYPE_MAST_POKE  ,   ///< 必須ポケモン（全国図鑑＃）チェック	
  REGULATION_NG_TYPE_VETO_WAZA  ,   ///< 禁止わざチェック					
  REGULATION_NG_TYPE_VETO_POKE  ,   ///< 禁止ポケモンチェック				
  REGULATION_NG_TYPE_VETO_ITEM  ,   ///< 禁止どうぐチェック					
  REGULATION_NG_TYPE_LEVEL      ,   ///< 参加レベル指定チェック				
  REGULATION_NG_TYPE_BOTHPOKE   ,   ///< 同種ポケモンチェック				
  REGULATION_NG_TYPE_BOTHITEM   ,   ///< 同じどうぐチェック					
  REGULATION_NG_TYPE_NUM_LEGEND ,   ///< 伝説ポケモン数チェック				
  REGULATION_NG_TYPE_QR         ,   ///< QRバトルチーム使用チェック			
  REGULATION_NG_TYPE_SPECIAL    ,   ///< 特別な条件を満たすポケモンチェック
  REGULATION_NG_TYPE_COMPATIBLE ,   ///< niji通信互換チェック

  REGULATION_NG_TYPE_MAX,
};

//-------------------------------------
/// チェック結果
//=====================================
struct REGULATION_CHECK_RESULT
{
  REGULATION_NG_TYPE NGType;                  //NGになった検査種別
  bool isPokeNG[pml::PokeParty::MAX_MEMBERS]; //個々のポケモンがNGかどうか（PokePartyのIndexと対応している）
};

//--------------------------------------------------------------------------------------------
/**
  * @brief   チームを検査するリクエストのレスポンスリスナー
  */
//--------------------------------------------------------------------------------------------
class TeamSelectResponseListener
{



public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   チェック成功
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnCheckSuccess( void ) = 0;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   レギュレーション違反
   * @param   NGTypeNum            NGになった検査種別の数（＝違反表示画面のページ数）
   * @param   checkPokeResultArray 検査結果の配列(NGTypeNum - 1までアクセス可能)
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnRegulationNG( u32 NGTypeNum , REGULATION_CHECK_RESULT* checkResultArray ) = 0;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ポケモンが不正
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnValidationNG( void ) = 0;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   通信エラー
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnError( void ) = 0;
};


GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)

#endif // TEAMSELECTRESPONSELISTENER_H_INCLUDE
