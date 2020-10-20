//=============================================================================
/**
 * @file   btl_PartyAllDeadRecorder.cpp
 * @date   2015/09/12 13:42:00
 * @author obata_toshihiro
 * @brief  パーティが全滅した順番の記録
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./btl_PartyAllDeadRecorder.h"

GFL_NAMESPACE_BEGIN( btl )



//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
PartyAllDeadRecorder::PartyAllDeadRecorder( void )
{
  this->Clear();
}

//-----------------------------------------------------------------------------
/**
 * @brief 初期化する
 */
//-----------------------------------------------------------------------------
void PartyAllDeadRecorder::Clear( void )
{
  for( u32 i=0; i<BTL_CLIENT_NUM; ++i )
  {
    m_partyAllDeadOrder[i] = PartyAllDeadRecorder::DEAD_ORDER_NONE;
  }
}
  
//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
PartyAllDeadRecorder::~PartyAllDeadRecorder()
{
}

//-----------------------------------------------------------------------------
/**
 * @brief パーティが全滅したことを記録する
 * @param clientID 全滅したクライアントのID
 */
//-----------------------------------------------------------------------------
void PartyAllDeadRecorder::RecordPartyAllDead( u8 clientID )
{
  if( BTL_CLIENT_NUM <= clientID )
  {
    GFL_ASSERT(0);
    return;
  }

  if( m_partyAllDeadOrder[ clientID ] != PartyAllDeadRecorder::DEAD_ORDER_NONE )
  {
    return;
  }

  s8 maxOrder = -1;
  for( u32 i=0; i<BTL_CLIENT_NUM; ++i )
  {
    if( m_partyAllDeadOrder[ i ] == PartyAllDeadRecorder::DEAD_ORDER_NONE )
    {
      continue;
    }
    if( ( maxOrder < 0 ) ||
        ( maxOrder <= m_partyAllDeadOrder[ i ] ) )
    {
      maxOrder = m_partyAllDeadOrder[ i ];
    }
  }

  m_partyAllDeadOrder[ clientID ] = ( maxOrder + 1 );
}

//-----------------------------------------------------------------------------
/**
 * @brief パーティが全滅した順番を取得する
 * @param clientID 取得対象クライアントのID
 * @retval 0origin, まだ全滅していないなら PartyAllDeadRecorder::DEAD_ORDER_NONE
 */
//-----------------------------------------------------------------------------
u8 PartyAllDeadRecorder::GetAllDeadOrder( u8 clientID ) const
{
  if( BTL_CLIENT_NUM <= clientID )
  {
    GFL_ASSERT(0);
    return PartyAllDeadRecorder::DEAD_ORDER_NONE;
  }

  return m_partyAllDeadOrder[ clientID ];
}



GFL_NAMESPACE_END( btl )