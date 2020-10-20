//=============================================================================
/**
 * @file   btl_RoyalRankingCalculator.cpp
 * @date   2015/09/11 17:38:56
 * @author obata_toshihiro
 * @brief  バトルロイヤルの現在順位を計算するクラス
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./btl_RoyalRankingCalculator.h"

#include "./btl_mainmodule.h"
#include "./btl_pokecon.h"
#include "./btl_party.h"
#include "./btl_PartyAllDeadRecorder.h"
#include "./btl_RoyalRankingContainer.h"


GFL_NAMESPACE_BEGIN( btl )


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
void RoyalRankingCalculator::UpdateRanking( 
  RoyalRankingContainer*      container,
  const MainModule&           mainModule,
  const POKECON&              pokecon,
  const PartyAllDeadRecorder& deadRecord,
  const TimeLimit&            timeLimit )
{
  ClientData clientData[ BTL_CLIENT_NUM ];

  // クライアントごとのデータを収集
  for( u32 clientId=0; clientId<BTL_CLIENT_NUM; ++clientId )
  {
    clientData[ clientId ].clientId     = clientId;
    clientData[ clientId ].allDeadOrder = deadRecord.GetAllDeadOrder( clientId );
    clientData[ clientId ].isTimeOver   = ( mainModule.IsClientLimitTimeExist() && ( timeLimit.clientLimitTime[ clientId ] == 0 ) );

    const BTL_PARTY* party = pokecon.GetPartyDataConst( clientId );
    RoyalRankingCalculator::CalcClientData( &clientData[ clientId ], *party );

    BTL_PRINT( "[RoyalRankingCalclator] timeLimit.clientLimitTime[ %d ] = %d\n", clientId, timeLimit.clientLimitTime[ clientId ] );
    BTL_PRINT( "[RoyalRankingCalclator] clientData[ %d ].clientId       = %d\n", clientId, clientData[ clientId ].clientId );
    BTL_PRINT( "[RoyalRankingCalclator] clientData[ %d ].allDeadOrder   = %d\n", clientId, clientData[ clientId ].allDeadOrder );
    BTL_PRINT( "[RoyalRankingCalclator] clientData[ %d ].isTimeOver     = %d\n", clientId, clientData[ clientId ].isTimeOver );
    BTL_PRINT( "[RoyalRankingCalclator] clientData[ %d ].killCount      = %d\n", clientId, clientData[ clientId ].killCount );
    BTL_PRINT( "[RoyalRankingCalclator] clientData[ %d ].remainPokeNum  = %d\n", clientId, clientData[ clientId ].remainPokeNum );
    BTL_PRINT( "[RoyalRankingCalclator] clientData[ %d ].remainHpSum    = %d\n", clientId, clientData[ clientId ].remainHpSum );
    BTL_PRINT( "[RoyalRankingCalclator] clientData[ %d ].remainHpRatio  = %d\n", clientId, clientData[ clientId ].remainHpRatio );
  }

  // 順位の降順にソートし、ランキング付け
  {
    const bool timeOverEnable = RoyalRankingCalculator::CheckCompaireEnableByTimeOver( clientData, GFL_NELEMS( clientData ) );
    RoyalRankingCalculator::SortClientDataByRankingLowerOrder( clientData, GFL_NELEMS( clientData ), timeOverEnable );
    RoyalRankingCalculator::SetRanking( clientData, GFL_NELEMS( clientData ), timeOverEnable );
  }

  // 計算結果を返す
  for( u32 i=0; i<GFL_NELEMS( clientData ); ++i )
  {
    container->SetClientRanking( clientData[i].clientId, clientData[i].ranking );

    BTL_PRINT( "[RoyalRankingCalclator] clientId=%d, ranking=%d\n", clientData[i].clientId, clientData[i].ranking );
  }
}

/**
 * @brief パーティの残りポケモン数・残りHPの合計値・割合[%]を計算する
 * @param[out] clientData   計算したパーティデータの格納先
 * @param      party        計算対象のパーティ
 */
void RoyalRankingCalculator::CalcClientData( RoyalRankingCalculator::ClientData* clientData, const BTL_PARTY& party )
{
  u16 pokeNum   = 0;
  u32 hpSum     = 0;
  u32 maxHpSum  = 0;

  const u8 memberCount = party.GetMemberCount();
  for( u8 memberIndex=0; memberIndex<memberCount; ++memberIndex )
  {
    const BTL_POKEPARAM* pokeParam = party.GetMemberDataConst( memberIndex );
    if( !( pokeParam->IsDead() ) ) 
    {
      ++pokeNum;
    }
    hpSum     += pokeParam->GetValue( BTL_POKEPARAM::BPP_HP );
    maxHpSum  += pokeParam->GetValue( BTL_POKEPARAM::BPP_MAX_HP );
  }

  clientData->killCount     = party.GetTotalKillCount();
  clientData->remainPokeNum = pokeNum;
  clientData->remainHpSum   = hpSum;
  clientData->remainHpRatio = ( hpSum * 100 ) / maxHpSum;
}

/**
 * @brief 順位の比較に持ち時間オーバーを考慮すべきかどうかをチェックする
 * @param[out] clientDataArray  パーティデータ配列
 * @param      arraySize        パーティデータ配列の要素数
 * @retval true   考慮する
 * @retval false  考慮しない
 */
 bool RoyalRankingCalculator::CheckCompaireEnableByTimeOver( const ClientData* clientDataArray, u32 arraySize )
{
  // 全滅したパーティが存在するなら、持ち時間オーバーを考慮しない
  for( u32 i=0; i<arraySize; ++i )
  {
    if( clientDataArray[i].remainPokeNum == 0 )
    {
      return false;
    }
  }

  return true;
}

/**
 * @brief クライアントデータを順位の降順にソートする
 * @param[out] clientDataArray  ソート対象のパーティデータ配列
 * @param      arraySize        パーティデータ配列の要素数
 * @param      timeOverEnable   持ち時間オーバーを考慮するか？
 */
void RoyalRankingCalculator::SortClientDataByRankingLowerOrder( ClientData* clientDataArray, u32 arraySize, bool timeOverEnable )
{
  for( u32 i=0; i<(arraySize-1); ++i )
  {
    for( u32 j=i; j<arraySize; ++j )
    {
      u32 index0 = i;
      u32 index1 = j;
      if( RoyalRankingCalculator::CompareRanking( clientDataArray[ index0 ], clientDataArray[ index1 ], timeOverEnable ) < 0 )
      {
        ClientData temp           = clientDataArray[ index0 ];
        clientDataArray[ index0 ] = clientDataArray[ index1 ];
        clientDataArray[ index1 ] = temp;
      }
    }
  }
}

/**
 * @brief 順位を比較する
 * @param client0         比較するパーティ0
 * @param client1         比較するパーティ0
 * @param timeOverEnable  持ち時間オーバーを考慮するか？
 * @retval  1  client0 が勝っている
 * @retval  0  同順位
 * @retval -1  client0 が負けている
 */
s32 RoyalRankingCalculator::CompareRanking( const ClientData& party0, const ClientData& party1, bool timeOverEnable )
{
  // 全滅で比較
  if( ( 0 < party0.remainPokeNum ) && ( party1.remainPokeNum == 0 ) )
  {
    return 1;
  }
  if( ( 0 < party1.remainPokeNum ) && ( party0.remainPokeNum == 0 ) )
  {
    return -1;
  }

  // 手持ち時間オーバーで比較
  if( timeOverEnable )
  {
    if( !party0.isTimeOver && party1.isTimeOver )
    {
      return 1;
    }
    if( party0.isTimeOver && !party1.isTimeOver )
    {
      return -1;
    }
  }

  // ( 倒したポケモンの数＋残りポケモン数 )で比較
  {
    u32 client0_point = party0.killCount + party0.remainPokeNum;
    u32 client1_point = party1.killCount + party1.remainPokeNum;

    if( client0_point > client1_point )
    {
      return 1;
    }
    if( client0_point < client1_point ) 
    {
      return -1;
    }
  }

  // 倒したポケモンの数で比較
  if( party0.killCount > party1.killCount )
  {
    return 1;
  }
  if( party0.killCount < party1.killCount ) 
  {
    return -1;
  }
  
  // 残りHP割合で比較
  if( party0.remainHpRatio > party1.remainHpRatio ) 
  {
    return 1;
  }
  if( party0.remainHpRatio < party1.remainHpRatio ) 
  {
    return -1;
  }

  // 残りHP合計値で比較
  if( party0.remainHpSum > party1.remainHpSum ) 
  {
    return 1;
  }
  if( party0.remainHpSum < party1.remainHpSum ) 
  {
    return -1;
  }

  // 全滅した順番で比較
  if( party0.allDeadOrder > party1.allDeadOrder ) 
  {
    return 1;
  }
  if( party0.allDeadOrder < party1.allDeadOrder ) 
  {
    return -1;
  }

  return 0;
}

/**
 * @brief パーティデータに現在順位を設定する
 * @param[out] clientDataArray  順位の降順に並んだパーティデータの配列
 * @param      arraySize        パーティデータ配列の要素数
 * @param      timeOverEnable   持ち時間オーバーを考慮するか？
 *
 * @note 同順位のクライアントが存在する場合 [ 0, 1, 1, 3 ] のように順位付けます。
 */
void RoyalRankingCalculator::SetRanking( ClientData* clientDataArray, u32 arraySize, bool timeOverEnable )
{
  u32 rank = 0;
  u32 same_rank_count = 0;
  clientDataArray[0].ranking = rank;

  for( u32 i=1; i<arraySize; ++i )
  {
    s32 compareResult = RoyalRankingCalculator::CompareRanking( clientDataArray[ i - 1 ], clientDataArray[ i ], timeOverEnable );

    if( compareResult < 0 ) 
    {
      GFL_ASSERT(0); // 順位の降順にソートされていない
    }

    // 同順位なら、同じ値をセットする
    if( compareResult == 0 )
    {
      same_rank_count++;
    }
    else
    {
      rank += same_rank_count + 1;
      same_rank_count = 0;
    }

    clientDataArray[i].ranking = rank;
  }
}


GFL_NAMESPACE_END( btl )