//======================================================================
/**
 * @file btl_PokeIDRec.cpp
 * @date 2016/06/17 13:28:45
 * @author taya
 * @brief 汎用ポケモンID履歴管理クラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>

#include "./btl_PokeIDRec.h"

GFL_NAMESPACE_BEGIN(btl)

// ctor / dtor
PokeIDRec::PokeIDRec()
{
  this->Clear();
}
PokeIDRec::~PokeIDRec()
{

}

/**
 * @brief 記録領域の初期化
 */
void PokeIDRec::Clear( void )
{
  for(u32 i=0; i<BTL_CLIENT_NUM; ++i)
  {
    for(u32 j=0; j<CAPACITY_PER_CLIENT; ++j)
    {
      m_registeredID[i][j] = BTL_POKEID_NULL;
    }
  }
}

/**
 * @brief ID１件登録
 * @param[in]  pokeID   登録するポケモンID
 */
void PokeIDRec::Register( u8 pokeID )
{
  u8 clientID = PokeID::PokeIdToClientId( pokeID );
  if( clientID >= BTL_CLIENT_NUM ){
    GFL_ASSERT(0);
    return;
  }

  if( findIndex(clientID, pokeID) >= 0 )
  {
    removeID( clientID, pokeID );
  }
  addID( clientID, pokeID );
}

/**
 * @brief 指定IDが登録されているか確認する
 * @param[in]  pokeID  確認したいポケモンID
 * @return  登録されていたら true
 */
bool PokeIDRec::IsRegistered( u8 pokeID ) const
{
  u8 clientID = PokeID::PokeIdToClientId( pokeID );
  if( clientID >= BTL_CLIENT_NUM ){
    GFL_ASSERT(0);
    return false;
  }

  return findIndex(clientID, pokeID) >= 0;
}

/**
 * @brief 指定IDが、直近 n 件以内に登録されているか確認する（※ n 件 = クライアントごとにカウント）
 * @param[in]  pokeID      確認したいポケモンID
 * @param[in]  checkCount  さかのぼって調べる件数（n）
 * @return  登録されていたら true
 */
bool PokeIDRec::IsRegisteredRecent( u8 pokeID, u8 checkCount ) const
{
  u8 clientID = PokeID::PokeIdToClientId( pokeID );
  if( clientID >= BTL_CLIENT_NUM ){
    GFL_ASSERT(0);
    return false;
  }

  int index = findIndex( clientID, pokeID );
  if( index < 0 ){
    return false;
  }
  return ( (int)checkCount > index );
}

//--------------------------------------

/**
 * @brief  登録されているIDを削除
 * @param[in]  clientID   クライアントID
 * @param[in]  pokeID     ポケモンID
 */
void PokeIDRec::removeID( u8 clientID, u8 pokeID )
{
  GFL_ASSERT_STOP(clientID < BTL_CLIENT_MAX);

  for(u32 i=0; i<CAPACITY_PER_CLIENT; ++i)
  {
    if( m_registeredID[ clientID ][ i ] != pokeID ){
      continue;
    }

    for(u32 j=i; j<(CAPACITY_PER_CLIENT-1); ++j){
      m_registeredID[clientID][j] = m_registeredID[clientID][j+1];
    }

    m_registeredID[clientID][ CAPACITY_PER_CLIENT-1 ] = BTL_POKEID_NULL;

    break;
  }
}

/**
 * @brief  登録されているIDを削除
 * @param[in]  clientID   クライアントID
 * @param[in]  pokeID     ポケモンID
 */
void PokeIDRec::addID( u8 clientID, u8 pokeID )
{
  GFL_ASSERT_STOP( clientID < BTL_CLIENT_MAX );

  for(int i=CAPACITY_PER_CLIENT-1; i>0; --i)
  {
    m_registeredID[ clientID ][ i ] = m_registeredID[ clientID ][ i-1 ];
  }
  m_registeredID[ clientID ][0] = pokeID;
}

/**
 * @brief   IDが登録されていればそのインデックス値を返す（クライアントごとに調べ、直近なら0, 以後1, 2, ...）
 * @param[in]  clientID   クライアントID
 * @param[in]  pokeID     ポケモンID
 * @return  pokeIDが登録されていればその登録インデックス値（0以上）／登録がなければ -1
 */
int PokeIDRec::findIndex( u8 clientID, u8 pokeID ) const
{
  GFL_ASSERT_STOP( clientID < BTL_CLIENT_MAX );

  for(int i=0; i<CAPACITY_PER_CLIENT; ++i)
  {
    if( m_registeredID[ clientID ][i] == pokeID ){
      return i;
    }
  }
  return -1;
}


GFL_NAMESPACE_END(btl)
