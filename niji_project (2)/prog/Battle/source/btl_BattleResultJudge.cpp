//=============================================================================
/**
 * @file   btl_BattleResultJudge.cpp
 * @date   2015/07/30 18:40:14
 * @author obata_toshihiro
 * @brief  バトルの勝敗判定
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./btl_mainmodule.h"
#include "./btl_pokecon.h"
#include "./btl_DeadRec.h"
#include "./btl_RoyalRankingContainer.h"
#include "./btl_BattleResultJudge.h"

GFL_NAMESPACE_BEGIN( btl )


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
BtlResult BattleResultJudge::CheckBattleResult( const MainModule& mainModule, const POKECON& pokecon, const DeadRec& deadRec, const TimeLimit& timeLimit, ResultCause* judgeCause )
{
  BtlSide winSide = BattleResultJudge::JudgeWinSide( mainModule, pokecon, deadRec, timeLimit, judgeCause );
  BTL_PRINT( "[BattleResultJudge] winSide = %d\n", winSide );

  if( winSide != BTL_SIDE_NULL ) 
  {
    u8 playerClientID = mainModule.GetPlayerClientID();
    u8 playerSide = mainModule.GetClientSide( playerClientID );
    return ( winSide == playerSide ) ? ( BTL_RESULT_WIN ) : ( BTL_RESULT_LOSE );
  }

  return BTL_RESULT_DRAW;
}

/**
 * @brief 勝利陣営を判定する
 *
 * @param      mainModule  参照するメインモジュール
 * @param      pokecon     参照するポケモンコンテナ
 * @param      deadRec     参照する死亡記録
 * @param      timeLimit   参照する制限時間情報
 * @param[out] judgeCause  判定理由
 *
 * @return 勝利した陣営
 * @retval BTL_SIDE_NULL  引き分けだった場合
 */
BtlSide BattleResultJudge::JudgeWinSide( const MainModule& mainModule, const POKECON& pokecon, const DeadRec& deadRec, const TimeLimit& timeLimit, ResultCause* judgeCause )
{
  *judgeCause = RESULT_CAUSE_OTHER;

  BtlSide winSide = BTL_SIDE_NULL;

  // 両陣営の生き残りポケ数＆総ポケ数＆持ち時間切れ数カウント
  u8 alivePokeCount[ BTL_SIDE_NUM ];
  u8 totalPokeCount[ BTL_SIDE_NUM ];
  u8 timeOverCount[ BTL_SIDE_NUM ];
  BattleResultJudge::GetPokeCountBySide( alivePokeCount, totalPokeCount, timeOverCount, mainModule, pokecon, timeLimit );
  BTL_PRINT( "[BattleResultJudge] alivePokeCount = %d, %d, %d, %d\n", alivePokeCount[0], alivePokeCount[1], alivePokeCount[2], alivePokeCount[3] );
  BTL_PRINT( "[BattleResultJudge] totalPokeCount = %d, %d, %d, %d\n", totalPokeCount[0], totalPokeCount[1], totalPokeCount[2], totalPokeCount[3] );
  BTL_PRINT( "[BattleResultJudge] timeOverCount  = %d, %d, %d, %d\n", timeOverCount[0], timeOverCount[1], timeOverCount[2], timeOverCount[3] );


  // どちらか全滅、もう一方は生き残りのケース
  winSide = BattleResultJudge::JudgeWinSide_UniqueAlive( mainModule, alivePokeCount );
  if( winSide != BTL_SIDE_NULL ) 
  {
    *judgeCause = RESULT_CAUSE_ALL_DEAD;
    return winSide;
  }

  // どちらかが持ち時間超過のケース
  if( mainModule.IsClientLimitTimeExist() )
  {
    winSide = BattleResultJudge::JudgeWinSide_UniqueTimeOver( timeOverCount, mainModule );
    if( winSide != BTL_SIDE_NULL ) 
    {
      *judgeCause = RESULT_CAUSE_TIMEOVER;
      return winSide;
    }
  }

  // 厳密判定が不要なら、この時点で引き分け
  if( !( mainModule.IsResultStrictMode() ) ) {
    return BTL_SIDE_NULL;
  }


  // 以下、バトル施設・通信対戦用の厳密判定


  // 全陣営全滅 -> 最後に倒れた側の勝ち
  winSide = BattleResultJudge::JudgeWinSide_DeadBoth( alivePokeCount, totalPokeCount, mainModule, deadRec );
  if( winSide != BTL_SIDE_NULL ) 
  {
    *judgeCause = RESULT_CAUSE_ALL_DEAD;
    return winSide;
  }


  // 以下、複数陣営が生き残っている( タイムアウト等 )
  *judgeCause = RESULT_CAUSE_TIMEOVER;

  // 倒されたポケ数で優勢な方が勝ち
  winSide = BattleResultJudge::JudgeWinSide_ByDeadPokeCount( mainModule, alivePokeCount , totalPokeCount );
  if( winSide != BTL_SIDE_NULL ) 
  {
    return winSide;
  }

  // …それでも引き分けなら残りHP割合で優勢な方の勝ち
  
  int HP_total[ BTL_SIDE_NUM ];
  int HP_ratio[ BTL_SIDE_NUM ];
  for( u32 side=0; side<BTL_SIDE_NUM; ++side ) {
    BattleResultJudge::CalcHPRatio( &HP_total[ side ], &HP_ratio[ side ], mainModule, pokecon, static_cast<BtlSide>(side) );
  }
  BTL_PRINT( "[BattleResultJudge] HP_ratio = %d, %d, %d, %d\n", HP_ratio[0], HP_ratio[1], HP_ratio[2], HP_ratio[3] );
  BTL_PRINT( "[BattleResultJudge] HP_total = %d, %d, %d, %d\n", HP_total[0], HP_total[1], HP_total[2], HP_total[3] );

  winSide = BattleResultJudge::JudgeWinSide_ByRemainedHpRatio( mainModule, HP_ratio );
  if( winSide != BTL_SIDE_NULL ) 
  {
    return winSide;
  }

  // …まだ引き分けなら残りHP総和で優勢な方の勝ち

  winSide = BattleResultJudge::JudgeWinSide_ByRemainedHpTotal( mainModule, HP_total );
  if( winSide != BTL_SIDE_NULL ) 
  {
    return winSide;
  }

  return BTL_SIDE_NULL;
  
}

 /**
  * @brief 陣営ごとのポケモン数を取得する
  * @param[out] alivePokeCount  陣営ごとの生き残ったポケモン数
  * @param[out] totalPokeCount  陣営ごとの総ポケモン数
  * @param[out] timeOverCount   陣営ごとの持ち時間超過クライアント数
  * @param      mainModule      参照するメインモジュール
  * @param      pokecon         参照するポケモンコンテナ
  * @param      timeLimit       参照する制限時間情報
  */
void BattleResultJudge::GetPokeCountBySide( 
  u8* alivePokeCount, 
  u8* totalPokeCount,
  u8* timeOverCount,
  const MainModule& mainModule,
  const POKECON& pokecon,
  const TimeLimit& timeLimit )
{
  for( u32 side=0; side<BTL_SIDE_NUM; ++side ) 
  {
    alivePokeCount[ side ] = 0;
    totalPokeCount[ side ] = 0;
    timeOverCount[ side ] = 0;
  }

  for( u32 clientId=0; clientId<BTL_CLIENT_MAX; ++clientId )
  {
    if( mainModule.IsExistClient(clientId) )
    {
      const BtlSide    side  = mainModule.GetClientSide( clientId );
      const BTL_PARTY* party = pokecon.GetPartyDataConst( clientId );

      if( party->GetMemberCount() )
      {
        alivePokeCount[ side ] += party->GetAliveMemberCount();
        totalPokeCount[ side ] += party->GetMemberCount();
      }

      if( ( mainModule.IsClientLimitTimeExist() ) &&
          ( timeLimit.clientLimitTime[ clientId ] == 0 ) )
      {
        timeOverCount[ side ]++;
      }
    }
  }
}

/**
 * @brief 唯一生き残った陣営を取得する
 * @param mainModule      参照するメインモジュール
 * @param alivePokeCount  各陣営の生き残ったポケモン数
 * @retval BTL_SIDE_NULL  生き残った陣営が複数ある場合
 */
BtlSide BattleResultJudge::JudgeWinSide_UniqueAlive( const MainModule& mainModule, const u8* alivePokeCount )
{
  BtlSide winSide = BTL_SIDE_NULL;

  for( u32 i=0; i<BTL_SIDE_NUM; ++i )
  {
    BtlSide side = static_cast<BtlSide>( i );

    if( !mainModule.IsSideExist( side ) )
    {
      continue;
    }

    if( alivePokeCount[ side ] <= 0 ) {
      continue;
    }

    if( winSide == BTL_SIDE_NULL ) 
    {
      winSide = side;
    }
    else
    {
      return BTL_SIDE_NULL;
    }
  }

  return winSide;
}

/**
 * @brief 持ち時間が残っているクライアントが最も多い、唯一の陣営を取得する
 * @param timeOverCount  各陣営の持ち時間を超過したクライアントの数
 * @param mainModule     参照するメインモジュール
 * @retval BTL_SIDE_NULL  持ち時間が残っている陣営が複数ある場合
 */
BtlSide BattleResultJudge::JudgeWinSide_UniqueTimeOver( const u8* timeOverCount, const MainModule& mainModule )
{
  BtlSide winSide              = BTL_SIDE_NULL;
  u32     winSideTimeOverCount = 0;
  u32     winSideNum           = 0;

  for( u32 i=0; i<BTL_SIDE_NUM; ++i )
  {
    const BtlSide side              = static_cast<BtlSide>( i );
    const u32     sideTimeOverCount = timeOverCount[ side ];

    if( !mainModule.IsSideExist( side ) )
    {
      continue;
    }

    // 単独1位を発見
    if( ( winSide == BTL_SIDE_NULL ) ||
        ( sideTimeOverCount < winSideTimeOverCount ) )
    {
      winSide              = side;
      winSideTimeOverCount = sideTimeOverCount;
      winSideNum           = 1;
      continue;
    }

    // １位タイを発見
    if( sideTimeOverCount == winSideTimeOverCount )
    {
      winSideNum++;
    }
  }

  // 持ち時間が残っているクライアントが最も多い陣営が複数存在するなら、勝利陣営を特定できない
  if( 1 < winSideNum )
  {
    return BTL_SIDE_NULL;
  }

  return winSide;
}

/**
 * @brief 全陣営全滅時の勝敗判定
 *
 * @param alivePokeCount  各陣営の生き残ったポケモン数
 * @param totalPokeCount  各陣営の総ポケモン数
 * @param mainModule      参照するメインモジュール
 * @param deadRec         参照する死亡記録
 *
 * @return  勝利した陣営
 * @retval  BTL_SIDE_NULL  勝敗が決定できなかった場合
 *
 * @note 全陣営全滅なら、最後に倒れたポケモンの陣営が勝ち
 */
BtlSide BattleResultJudge::JudgeWinSide_DeadBoth( 
  const u8*         alivePokeCount, 
  const u8*         totalPokeCount,
  const MainModule& mainModule,
  const DeadRec&    deadRec )
{
  // 全陣営が全滅したかチェック
  for( u32 side=0; side<BTL_SIDE_NUM; ++side )
  {
    if( !mainModule.IsSideExist( static_cast<BtlSide>(side) ) )
    {
      continue;
    }

    if( 0 < alivePokeCount[ side ] ) 
    {
      return BTL_SIDE_NULL;
    }
  }

  const u8 lastPokeID = deadRec.GetLastDeadPokeID();
  if( lastPokeID == BTL_POKEID_NULL ) 
  {
    GFL_ASSERT(0);
    return BTL_SIDE_NULL;  // 有り得ないけどねんのため
  }

  const u8 lastPokeSide = mainModule.PokeIDtoSide( lastPokeID );
  BTL_N_Printf( DBGSTR_SVFS_LastPokeID, lastPokeID );
  return lastPokeSide;
}

/**
 * @brief 倒されたポケモンの数から、勝利した陣営を判断する
 * @param   alivePokeCount  各陣営の生き残ったポケモン数
 * @param   totalPokeCount  各陣営の総ポケモン数
 * @retval BTL_SIDE_NULL  勝利陣営が複数存在する場合
 */
BtlSide BattleResultJudge::JudgeWinSide_ByDeadPokeCount( const MainModule& mainModule, const u8* alivePokeCount, const u8* totalPokeCount )
{
  // 倒されたポケモンの数を算出
  int deadPokeCount[ BTL_SIDE_NUM ];
  for( u32 side=0; side<BTL_SIDE_NUM; ++side ) {
    deadPokeCount[ side ] = totalPokeCount[ side ] - alivePokeCount[ side ];
  }
  BTL_N_Printf( DBGSTR_SVFL_DeadPokeCount, deadPokeCount[ BTL_SIDE_1ST ], deadPokeCount[ BTL_SIDE_2ND ], deadPokeCount[ BTL_SIDE_3RD ], deadPokeCount[ BTL_SIDE_4TH ] );

  // 倒されたポケモンが一番少ない陣営が勝ち
  BtlSide winSide              = BTL_SIDE_NULL;
  int     winSideDeadPokeCount = 0;
  u32     winSideCount         = 0;
  for( u32 side=0; side<BTL_SIDE_NUM; ++side ) 
  {
    if( !mainModule.IsSideExist( static_cast<BtlSide>(side) ) )
    {
      continue;
    }

    // 単独１位を発見
    if( ( winSide == BTL_SIDE_NULL ) ||
        ( deadPokeCount[ side ] < winSideDeadPokeCount ) )
    {
      winSide              = static_cast<BtlSide>( side );
      winSideDeadPokeCount = deadPokeCount[ side ];
      winSideCount         = 1;
      continue;
    }

    // １位タイを発見
    if( deadPokeCount[ side ] == winSideDeadPokeCount )
    {
      ++winSideCount;
    }
  }

  // 勝利陣営が複数あるなら、倒されたポケモンの数からは判断できない
  if( 1 < winSideCount ) {
    return BTL_SIDE_NULL;
  }

  return winSide;
}

/**
 * @brief 残りHPの割合から、勝利した陣営を判断する
 * @param mainModule       参照するメインモジュール
 * @param remainedHpRatio  各陣営の残りHPの割合
 * @retval BTL_SIDE_NULL  勝利陣営が複数存在する場合
 */
BtlSide BattleResultJudge::JudgeWinSide_ByRemainedHpRatio( const MainModule& mainModule, const int* remainedHpRatio )
{
  // 残りHPの割合が最も高い陣営の勝ち
  BtlSide winSide        = BTL_SIDE_NULL;
  int     winSideHpRatio = 0;
  u32     winSideCount   = 0;
  for( u32 side=0; side<BTL_SIDE_NUM; ++side ) 
  {
    if( !mainModule.IsSideExist( static_cast<BtlSide>(side) ) )
    {
      continue;
    }

    // 単独１位を発見
    if( ( winSide == BTL_SIDE_NULL ) ||
        ( winSideHpRatio < remainedHpRatio[ side ] ) )
    {
      winSide        = static_cast<BtlSide>( side );
      winSideHpRatio = remainedHpRatio[ side ];
      winSideCount   = 1;
      continue;
    }

    // １位タイを発見
    if( remainedHpRatio[ side ] == winSideHpRatio )
    {
      ++winSideCount;
    }
  }

  // 勝利陣営が複数あるなら、残りHPの割合からは判断できない
  if( 1 < winSideCount ) {
    return BTL_SIDE_NULL;
  }

  return winSide;
}

/**
 * @brief 残りHPの総和から、勝利した陣営を判断する
 * @param remainedHpTotal  各陣営の残りHPの総和
 * @retval BTL_SIDE_NULL  勝利陣営が複数存在する場合
 */
BtlSide BattleResultJudge::JudgeWinSide_ByRemainedHpTotal( const MainModule& mainModule, const int* remainedHpTotal )
{
  // 残りHPの総和が最も高い陣営の勝ち
  BtlSide winSide        = BTL_SIDE_NULL;
  int     winSideHpTotal = 0;
  u32     winSideCount   = 0;
  for( u32 side=0; side<BTL_SIDE_NUM; ++side ) 
  {
    if( !mainModule.IsSideExist( static_cast<BtlSide>(side) ) )
    {
      continue;
    }

    // 単独１位を発見
    if( ( winSide == BTL_SIDE_NULL ) ||
        ( winSideHpTotal < remainedHpTotal[ side ] ) )
    {
      winSide        = static_cast<BtlSide>( side );
      winSideHpTotal = remainedHpTotal[ side ];
      winSideCount   = 1;
      continue;
    }

    // １位タイを発見
    if( remainedHpTotal[ side ] == winSideHpTotal )
    {
      ++winSideCount;
    }
  }

  // 勝利陣営が複数あるなら、残りHPの総和からは判断できない
  if( 1 < winSideCount ) {
    return BTL_SIDE_NULL;
  }

  return winSide;
}

/**
 * @brief 指定陣営側パーティのHP残総和＆割合計算
 *
 * @param[out] HP_sum      総和を格納
 * @param[out] HP_ratio    割合（%）を格納
 * @param      mainModule  参照するメインモジュール
 * @param      pokecon     参照するポケモンコンテナ
 * @param      targetSide  計算対象の陣営
 */
void BattleResultJudge::CalcHPRatio(
  int*              HP_sum, 
  int*              HP_ratio, 
  const MainModule& mainModule, 
  const POKECON&    pokecon, 
  const BtlSide     targetSide )
{
  *HP_sum = 0;
  *HP_ratio = 0;

  if( !mainModule.IsSideExist( targetSide ) )
  {
    return;
  }

  u32 HPMax_sum = 0;

  for( u32 clientId=0; clientId<BTL_CLIENT_MAX; ++clientId )
  {
    if( ( mainModule.IsExistClient( clientId ) ) &&
        ( mainModule.GetClientSide( clientId ) == targetSide ) )
    {
      const BTL_PARTY* party = pokecon.GetPartyDataConst( clientId );
      const u32 memberCount = party->GetMemberCount();
      for( u32 memberIndex=0; memberIndex<memberCount; ++memberIndex )
      {
        const BTL_POKEPARAM* pokePara = party->GetMemberDataConst( memberIndex );
        HPMax_sum += pokePara->GetValue( BTL_POKEPARAM::BPP_MAX_HP );
        *HP_sum   += pokePara->GetValue( BTL_POKEPARAM::BPP_HP );
      }
    }
  }

  *HP_ratio = ( ( *HP_sum ) * 100 ) / HPMax_sum;
}

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
BtlResult BattleResultJudge::CheckBattleResult_Royal( const MainModule& mainModule, const RoyalRankingContainer& royalRankingContainer, ResultCause* judgeCause )
{
  *judgeCause = RESULT_CAUSE_ALL_DEAD;

  u8 playerClientID = mainModule.GetPlayerClientID();
  u8 ranking = royalRankingContainer.GetClientRanking( playerClientID );
  return ( ranking == 0 ) ? ( BTL_RESULT_WIN ) : ( BTL_RESULT_LOSE );
}




GFL_NAMESPACE_END( btl )
