//=============================================================================
/**
 * @file   btl_PokeID.cpp
 * @date   2015/10/15 11:36:48
 * @author obata_toshihiro
 * @brief  ポケIDの定義
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./btl_PokeID.h"
#include "./btl_const.h"


GFL_NAMESPACE_BEGIN( btl )

namespace
{

  // 各クライアントのポケID開始値
  static const u8 ClientBasePokeID[ BTL_CLIENT_MAX ] = 
  {
    0,                        // Client 0 のポケモンID 開始値
    BTL_PARTY_MEMBER_MAX*2,   // Client 1 のポケモンID 開始値
    BTL_PARTY_MEMBER_MAX*1,   // Client 2 のポケモンID 開始値
    BTL_PARTY_MEMBER_MAX*3,   // Client 3 のポケモンID 開始値
  };

}


//-----------------------------------------------------------------------------
/**
 * @brief 指定クライアントのポケIDの最小値を取得する
 * @param clientID  クライアントID
 */
 //----------------------------------------------------------------------------
u8 PokeID::GetClientBasePokeId( BTL_CLIENT_ID clientID )
{
  GFL_ASSERT_STOP( clientID < GFL_NELEMS(ClientBasePokeID) );
  return ClientBasePokeID[ clientID ];
}

//----------------------------------------------------------------------------
/**
 * @brief 指定クライアントのポケIDを取得する
 * @param clientID   クライアントID
 * @param pokeIndex  ポケモンのインデックス
 */
//----------------------------------------------------------------------------
u8 PokeID::GetClientPokeId( BTL_CLIENT_ID clientID, u8 pokeIndex )
{
  const u8 basePokeId = PokeID::GetClientBasePokeId( clientID );

  if( BTL_PARTY_MEMBER_MAX <= pokeIndex )
  {
    GFL_ASSERT(0);
    return basePokeId;
  }

  return ( basePokeId + pokeIndex );
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケID　→　戦闘開始時のパーティ内インデックス
 */
//-----------------------------------------------------------------------------
u8 PokeID::PokeIdToStartMemberIndex( u8 pokeID )
{
  const BTL_CLIENT_ID clientId = PokeID::PokeIdToClientId( pokeID );
  const u8 basePokeId = PokeID::GetClientBasePokeId( clientId );
  if( ( pokeID < basePokeId ) || ( ( basePokeId + BTL_PARTY_MEMBER_MAX ) <= pokeID ) )
  {
    GFL_ASSERT(0);
    return 0;
  }
  return ( pokeID - basePokeId );
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケID　→　クライアントID
 */
//-----------------------------------------------------------------------------
BTL_CLIENT_ID PokeID::PokeIdToClientId( u8 pokeID )
{
  for( u32 i=0; i<GFL_NELEMS(ClientBasePokeID); ++i )
  {
    BTL_CLIENT_ID clientId = static_cast<BTL_CLIENT_ID>( i );
    u32 min = PokeID::GetClientBasePokeId( clientId );
    u32 max = min + BTL_PARTY_MEMBER_MAX - 1;
    if( ( pokeID >= min ) && ( pokeID <= max ) )
    {
      return clientId;
    }
  }

  GFL_PRINT( "Illegal PokeID[%d]", pokeID );
  GFL_ASSERT(0);
  return BTL_CLIENT_PLAYER;
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケID　→　ショートポケID( 3bit )
 */
//-----------------------------------------------------------------------------
u8 PokeID::PokeIdToShortId( u8 pokeID )
{
  BTL_CLIENT_ID clientID   = PokeID::PokeIdToClientId( pokeID );
  u8            basePokeID = PokeID::GetClientBasePokeId( clientID );

  int diff = pokeID - basePokeID;
  if( ( 0 <= diff ) && ( diff < BTL_PARTY_MEMBER_MAX ) )
  {
    return static_cast<u8>( diff );
  }

  GFL_PRINT( "Illegal PokeID[%d]", pokeID );
  GFL_ASSERT(0);
  return 0;
}

//-----------------------------------------------------------------------------
/**
 * @brief クライアントID & ショートポケID( 3bit )　→　ポケID
 */
//-----------------------------------------------------------------------------
u8 PokeID::ShortIdToPokeId( BTL_CLIENT_ID clientID, u8 shortID )
{
  if( ( clientID < GFL_NELEMS(ClientBasePokeID) ) &&
      ( shortID < BTL_PARTY_MEMBER_MAX ) )
  {
    return PokeID::GetClientBasePokeId( clientID ) + shortID;
  }
  else
  {
    GFL_PRINT( "clientID:%d, shortID:%d", clientID, shortID );
    GFL_ASSERT(0);
    return BTL_POKEID_NULL;
  }
}



GFL_NAMESPACE_END( btl )