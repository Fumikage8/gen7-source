
#if !defined(BATTLESPOTPROCPARAM_H_INCLUDED)
#define BATTLESPOTPROCPARAM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotProcParam.h
 * @date   2015/10/02 Fri. 11:57:32
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include "NetStatic/NetAppLib/include/UI/NetAppRegulationDrawInfo.h"
// 通信バトルイベント
#include "NetStatic/NetEvent/include/NetworkBattleEvent.h"
#include "NetApp/TeamSelect/include/TeamSelectEventAppParam.h"

namespace NetApp      {
namespace BattleSpot  {
// =============================================================================
/**/
//==============================================================================

enum
{
  NEXT_APP_NONE = 0,
  NEXT_APP_TEAM_SELECT,
  NEXT_APP_BATTLE,
  NEXT_APP_QR,
  NEXT_APP_TEAM_SELECT_CUP,
  NEXT_APP_TEAM_SELECT_FRIEND_CUP,
  NEXT_APP_BATTLEVIDEO_SAVE,
};

//-------------------------------------
/// 共通データ
//=====================================
struct COMMON_DATA{
  u64   disconnect_flag;    ///< 切断フラグ：過去64件の切断状況がビットで保存されている
  u8    disconnect_bit;     ///< 切断管理コード:最後に切断ビットをたてた場所
  u8    first_flag;         ///< 初期フラグ 0:初期 1:一度でも保存した

  u8    pad[6];
};

//-------------------------------------
/// ランキングデータ
//=====================================
struct RANK_DATA{
  u32   rate;         ///< レート値
  u32   win;          ///< 勝利数
  u32   lose;         ///< 敗北数
  u32   draw;         ///< 引き分け数
  u32   disconnect;   ///< 切断数
  u32   pad;
};


//==============================================================================
/**
  *  @brief  
  *  @date   2015/10/02 Fri. 11:58:29
  */
//==============================================================================
typedef struct  APP_PARAM
{

  u32   nextApp;
  u32   nextFrame;
  NetAppLib::UI::RegulationDrawInfo*             m_pRegulationDrawInfo;     //! レギュレーション描画情報
  NetApp::TeamSelect::EVENT_APP_PARAM*           m_pTeamSelectEventParam;   //! バトルチーム選択
  NetEvent::NetworkBattle::PersonalData*         m_pPersonalData[ BTL_CLIENT_NUM ];  //! 参加者データ 


  u32 m_rule;
  bool m_isLegendEnable;
  u32 m_opponentRate;
  u32 m_bgm;

  RANK_DATA m_rankData;
  COMMON_DATA m_commonData;

  bool m_teamSelected;
  bool m_qrDecoded;

  NetEvent::NetworkBattle::EVENT_PARAM*          m_pNetworkBattleParam;     //! 通信対戦イベント

  BATTLE_PGL_RECORD_SET m_pglRecord;

  bool m_autoBattleVideoUpload;

  bool m_isQRTeam;

  bool m_isSessionLeft;

  u32 m_topMenuCurorIndex;
  u32 m_freeRuleCurorIndex;
  u32 m_rateRuleCurorIndex;
  u32 m_freeLegendCurorIndex;

  bool m_isDisconnectBeforeBattle;

#if defined(GF_PLATFORM_CTR)
  nn::nex::DateTime               m_communityEndDateTime[6];
#endif

#if PM_DEBUG
  bool m_IsDebugForcedWinReport; //! 強制勝ちレポート送信
#endif

  APP_PARAM(void)
    : nextApp(0)
    , nextFrame(0)
    , m_pRegulationDrawInfo(NULL)
    , m_pTeamSelectEventParam(NULL)
    , m_rule(0)
    , m_isLegendEnable(false)
    , m_opponentRate(0)
    , m_bgm(0)
    , m_teamSelected(false)
    , m_qrDecoded(false)
    , m_pNetworkBattleParam(NULL)
    , m_autoBattleVideoUpload(false)
    , m_isQRTeam(false)
    , m_isSessionLeft(false)
    , m_topMenuCurorIndex(0)
    , m_freeRuleCurorIndex(0)
    , m_rateRuleCurorIndex(0)
    , m_freeLegendCurorIndex(0)
    , m_isDisconnectBeforeBattle(false)
#if PM_DEBUG
    , m_IsDebugForcedWinReport(false)
#endif
  {
    ::std::memset( m_pPersonalData , 0 , sizeof( m_pPersonalData ) );
    ::std::memset( &m_rankData , 0 , sizeof( m_rankData ) );
    ::std::memset( &m_commonData , 0 , sizeof( m_commonData ) );
    ::std::memset( &m_pglRecord , 0 , sizeof(m_pglRecord) );
  }
}APP_PARAM;



} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
#endif  /*  #if !defined(BATTLESPOTPROCPARAM_H_INCLUDED)  */

