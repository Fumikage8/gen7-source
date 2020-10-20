//=============================================================================
/**
 * @file   btl_BattleResultJudge.h
 * @date   2015/07/30 18:40:14
 * @author obata_toshihiro
 * @brief  バトルの勝敗判定
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_BATTLERESULTJUDGE_H_INCLUDED
#define BTL_BATTLERESULTJUDGE_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <Battle/include/battle_ResultData.h>
#include "./btl_server_TimeLimit.h"
#include "./btl_PokemonPosition.h"


GFL_NAMESPACE_BEGIN( btl )

  class MainModule;
  class POKECON;
  class DeadRec;
  class RoyalRankingContainer;


  /**
   * @class BattleResultJudge
   * @brief バトルの勝敗を判定するクラス
   */
  class BattleResultJudge
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleResultJudge );

  public:

    //-----------------------------------------------------------------------------
    /**
     * @brief 勝敗判定
     *
     * @param      mainModule  参照するメインモジュール
     * @param      pokecon     参照するポケモンコンテナ
     * @param      deadRec     参照する死亡記録
     * @param      timeLimit   参照する制限時間情報
     * @param[out] judgeCause  判定理由
     *
     * @retval  勝敗コード( win, lose, draw )
     */
    //-----------------------------------------------------------------------------
    static BtlResult CheckBattleResult( const MainModule& mainModule, const POKECON& pokecon, const DeadRec& deadRec, const TimeLimit& timeLimit, ResultCause* judgeCause );

    //-----------------------------------------------------------------------------
    /**
     * @brief 勝敗判定( バトルロイヤル用 )
     *
     * @param      mainModule             参照するメインモジュール
     * @param      royalRankingContainer  バトルロイヤルの順位コンテナ
     * @param[out] judgeCause             判定理由
     *
     * @retval  勝敗コード( win, lose )
     */
    //-----------------------------------------------------------------------------
    static BtlResult CheckBattleResult_Royal( const MainModule& mainModule, const RoyalRankingContainer& royalRankingContainer, ResultCause* judgeCause );



  private:

    static void GetPokeCountBySide( u8* alivePokeCount, u8* totalPokeCount, u8* timeOverCount, const MainModule& mainModule, const POKECON& pokecon, const TimeLimit& timeLimit );
    static BtlSide JudgeWinSide( const MainModule& mainModule, const POKECON& pokecon, const DeadRec& deadRec, const TimeLimit& timeLimit, ResultCause* judgeCause );
    static BtlSide JudgeWinSide_UniqueAlive( const MainModule& mainModule, const u8* alivePokeCount );
    static BtlSide JudgeWinSide_UniqueTimeOver( const u8* timeOverCount, const MainModule& mainModule );
    static BtlSide JudgeWinSide_DeadBoth( const u8* alivePokeCount, const u8* totalPokeCount, const MainModule& mainModule, const DeadRec& deadRec );
    static BtlSide JudgeWinSide_ByDeadPokeCount( const MainModule& mainModule, const u8* alivePokeCount, const u8* totalPokeCount );
    static BtlSide JudgeWinSide_ByRemainedHpRatio( const MainModule& mainModule, const int* remainedHpRatio );
    static BtlSide JudgeWinSide_ByRemainedHpTotal( const MainModule& mainModule, const int* remainedHpTotal );
    static void CalcHPRatio( int* HP_sum, int* HP_ratio, const MainModule& mainModule, const POKECON& pokecon, const BtlSide targetSide );

  };


GFL_NAMESPACE_END( btl )

#endif // BTL_BATTLERESULTJUDGE_H_INCLUDED