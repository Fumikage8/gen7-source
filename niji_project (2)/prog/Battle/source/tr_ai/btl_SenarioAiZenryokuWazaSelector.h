//=============================================================================
/**
 * @file   btl_SenarioAiZenryokuWazaSelector.h
 * @date   2016/01/13 15:21:30
 * @author obata_toshihiro
 * @brief  シナリオ中のゲーム内トレーナーが全力技を選択するためのAI
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_SENARIOAIZENRYOKUWAZASELECTOR_H_INCLUDED
#define BTL_SENARIOAIZENRYOKUWAZASELECTOR_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "../btl_common.h"

GFL_NAMESPACE_BEGIN( btl )

  class MainModule;
  class ServerFlow;
  class POKECON;
  class BTL_POKEPARAM;
  class ZenryokuWazaStatus;


  /**
   * @class SenarioAiZenryokuWazaSelector
   * @brief シナリオ中のゲーム内トレーナー用、全力技選択AI
   */
  class SenarioAiZenryokuWazaSelector
  {
    GFL_FORBID_COPY_AND_ASSIGN( SenarioAiZenryokuWazaSelector );

  public:

    /**
     * @brief セットアップパラメータ
     */
    struct SetupParam
    {
      const MainModule* mainModule;   // 関連付ける MainModule
      const POKECON*    pokecon;      // 参照する POKECON
      ServerFlow*       serverFlow;   // 使用する ServerFlow
      AIRandSys*        randSys;      // 使用する乱数
    };

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param setupParam  セットアップパラメータ
     */
    //-------------------------------------------------------------------------
    SenarioAiZenryokuWazaSelector( const SetupParam& setupParam );

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    virtual ~SenarioAiZenryokuWazaSelector();



    //-------------------------------------------------------------------------
    /**
     * @brief ワザ決定後、そのワザを全力技として使うかどうかを判定する
     * @param[in]  attacker   ワザを撃つポケモンのパラメータ
     * @param[in]  wazaIdx    撃とうとしているワザのインデックス
     * @param[in]  targetPos  ワザ対象の位置ID
     * @return     全力ワザを使うなら true
     */
    //-------------------------------------------------------------------------
    bool ShouldUseZenryokuWaza( const BTL_POKEPARAM* attacker, u8 wazaIdx, BtlPokePos targetPos ) const;



  private:


    const ZenryokuWazaStatus* getClientZenryokuWazaStatus( const BTL_POKEPARAM* attacker ) const;
    bool  isZenryokuWazaEnable( const BTL_POKEPARAM* attacker, u8 wazaIdx, const ZenryokuWazaStatus* zenryokuWazaStatus ) const;
    bool  shouldCancelByTypeAffinity( const BTL_POKEPARAM* attacker, u8 wazaIdx, BtlPokePos targetPos ) const;
    bool  decideZenryokuWazaUseAtRandom( BtlTypeAff typeAffinity ) const;

    static bool isSingleTargetWaza( WazaNo waza_no );


    const MainModule* m_mainModule;
    const POKECON* m_pokecon;
    ServerFlow* m_serverFlow;
    AIRandSys* m_randSys;

  };



GFL_NAMESPACE_END( btl )

#endif // BTL_SENARIOAIZENRYOKUWAZASELECTOR_H_INCLUDED