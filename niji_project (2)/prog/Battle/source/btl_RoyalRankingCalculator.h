//=============================================================================
/**
 * @file   btl_RoyalRankingCalculator.h
 * @date   2015/09/11 17:38:56
 * @author obata_toshihiro
 * @brief  バトルロイヤルの現在順位を計算するクラス
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_ROYALRANKINGCALCULATOR_H_INCLUDED
#define BTL_ROYALRANKINGCALCULATOR_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "./btl_common.h"
#include "./btl_server_TimeLimit.h"


GFL_NAMESPACE_BEGIN( btl )

  class MainModule;
  class POKECON;
  class BTL_PARTY;
  class PartyAllDeadRecorder;
  class RoyalRankingContainer;


  /**
   * @class RoyalRankingContainer
   * @brief バトルロイヤルの現在順位を計算するクラス
   */
  class RoyalRankingCalculator
  {
    GFL_FORBID_COPY_AND_ASSIGN( RoyalRankingCalculator );

  public:

    //-------------------------------------------------------------------------
    /**
     * @brief 現在順位を更新する
     * @param[out] container   計算結果の格納先
     * @param      mainModule  参照するメインモジュール
     * @param      pokecon     参照する全クライアントのパーティ
     * @param      deadRecord  参照するパーティの全滅記録
     * @param      timeLimit   参照する制限時間情報
     */
    //-------------------------------------------------------------------------
    static void UpdateRanking( 
      RoyalRankingContainer*      container,
      const MainModule&           mainModule,
      const POKECON&              pokecon,
      const PartyAllDeadRecorder& deadRecord,
      const TimeLimit&            timeLimit );


  private:

    struct ClientData
    {
      u8   clientId;       // クライアントID
      u16  killCount;      // 倒したポケモンの数
      u16  remainPokeNum;  // 残りポケモンの数
      u16  remainHpRatio;  // 残りHPの割合
      u16  remainHpSum;    // 残りHPの合計値
      u16  allDeadOrder;   // 全滅した順番( 0origin, 全滅していないなら PartyAllDeadRecorder::DEAD_ORDER_NONE )
      u8   ranking;        // 順位( 0origin )
      bool isTimeOver;     // 持ち時間を超過したか？
    };

    static void CalcClientData( ClientData* clientData, const BTL_PARTY& party );
    static void SortClientDataByRankingLowerOrder( ClientData* clientDataArray, u32 arraySize, bool timeOverEnable );
    static bool CheckCompaireEnableByTimeOver( const ClientData* clientDataArray, u32 arraySize );
    static s32 CompareRanking( const ClientData& party0, const ClientData& party1, bool timeOverEnable );
    static void SetRanking( ClientData* clientDataArray, u32 arraySize, bool timeOverEnable );

  };


GFL_NAMESPACE_END( btl )

#endif // BTL_ROYALRANKINGCALCULATOR_H_INCLUDED