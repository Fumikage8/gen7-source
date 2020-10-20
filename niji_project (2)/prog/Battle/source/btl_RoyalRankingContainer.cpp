//=============================================================================
/**
 * @file   btl_RoyalRankingContainer.cpp
 * @date   2015/09/11 17:38:56
 * @author obata_toshihiro
 * @brief  バトルロイヤルの現在順位を保持するコンテナ
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./btl_RoyalRankingContainer.h"


GFL_NAMESPACE_BEGIN( btl )


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
RoyalRankingContainer::RoyalRankingContainer( void )
{
  this->Clear();
}

//-----------------------------------------------------------------------------
/**
 * @brief 初期化する
 */
//-----------------------------------------------------------------------------
void RoyalRankingContainer::Clear( void )
{
  for( u32 i=0; i<BTL_CLIENT_NUM; ++i )
  {
    m_clientRanking[i] = 0;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
RoyalRankingContainer::~RoyalRankingContainer()
{
}

//-----------------------------------------------------------------------------
/**
 * @brief 指定クライアントの現在順位を取得する
 * @para clientId  取得対象クライアントのID
 * @return 順位( 0 origin )
 */
//-----------------------------------------------------------------------------
u8 RoyalRankingContainer::GetClientRanking( u8 clientId ) const
{
  if( BTL_CLIENT_NUM <= clientId )
  {
    GFL_ASSERT(0);
    return 0;
  }

  return m_clientRanking[ clientId ];
}

//-----------------------------------------------------------------------------
/**
 * @brief 指定クライアントの現在順位を設定する
 * @param clientId  設定対象クライアントのID
 * @param ranking   設定する順位( 0 origin )
 */
//-----------------------------------------------------------------------------
void RoyalRankingContainer::SetClientRanking( u8 clientId, u8 ranking )
{
  if( ( BTL_CLIENT_NUM <= clientId ) ||
      ( BTL_CLIENT_NUM <= ranking  ) )
  {
    GFL_ASSERT(0);
    return;
  }

  m_clientRanking[ clientId ] = ranking;
}



GFL_NAMESPACE_END( btl )