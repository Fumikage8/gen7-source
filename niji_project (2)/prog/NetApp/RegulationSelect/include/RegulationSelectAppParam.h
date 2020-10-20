// ============================================================================
/*
 * @file RegulationSelectAppParam.h
 * @brief レギュレーション選択アプリを起動する為のパラメータです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATIONSELECT_APPPARAM_H_INCLUDE )
#define REGULATIONSELECT_APPPARAM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetApp/RegulationSelect/source/RegulationSelectDefine.h"
#include "Battle/Regulation/include/Regulation.h"
#include "NetStatic/NetEvent/include/NetEventBase.h"
#include "NetStatic/NetAppLib/include/UI/NetAppRegulationDrawInfo.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)


//! アプリの起動パラメータ
struct APP_PARAM
{
  u32                                     bootMode;                         //! 起動モード( RegulationSelectDefine.h の BootMode を参照 )
  u32                                     battleRule;                       //! 選択モード[P2P通信対戦用]で起動する時に表示するバトルルール( Regulation.h の BATTLE_RULE を参照 )
  NetAppLib::UI::RegulationDrawInfo*      pRegulationDrawInfo;              //! 詳細表示モードで起動する時に表示するレギュレーションの情報
  bool                                    bSingleNormalClearFlag;           //! 選択モード[バトル施設用]で起動する時に指定する
  bool                                    bDoubleNormalClearFlag;           //! 選択モード[バトル施設用]で起動する時に指定する
  bool                                    bMultiNormalClearFlag;            //! 選択モード[バトル施設用]で起動する時に指定する
  bool                                    bBattleRoyalNormalClearFlag;      //! 選択モード[バトルロイヤル用]で起動する時に指定する
  bool                                    bBattleRoyalSuperClearFlag;       //! 選択モード[バトルロイヤル用]で起動する時に指定する
  bool                                    bBattleRoyalHyperClearFlag;       //! 選択モード[バトルロイヤル用]で起動する時に指定する
  bool                                    bQRPartyFlag;                     //! 詳細表示モードで起動する時に指定する
};


//! アプリの結果
struct APP_RESULT
{
  NetAppLib::UI::RegulationDrawInfo*      pSelectRegulationDrawInfo;        //! 選択したレギュレーション
  bool                                    bIsSelectedRegulation;            //! レギュレーション選択フラグ[ true : レギュレーションを選択した, false : レギュレーションを選択しないで戻った ]
};


struct APP_RESULT_LOCAL
{
  bool                                    bIsFatalError;
};

GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONSELECT_APPPARAM_H_INCLUDE
