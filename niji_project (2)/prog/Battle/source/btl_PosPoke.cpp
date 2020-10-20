//=============================================================================================
/**
 * @file    btl_PosPoke.cpp
 * @brief   ポケモンXYバトルシステム  場にいるポケモンの生存確認用構造体と処理ルーチン
 * @author  taya
 *
 * @date  2011.01.06  作成
 */
//=============================================================================================
#include <util/include/gfl2_std_string.h>

#include "./btl_MainModule.h"
#include "./btl_PosPoke.h"



GFL_NAMESPACE_BEGIN(btl)


// ctor
PosPoke::PosPoke( void )
{
  m_lastPos.Setup( m_lastPosInst, GFL_NELEMS(m_lastPosInst) );
}


//=============================================================================================
/**
 * @brief 初期化
 *
 * @param mainModule  参照するMainModule
 * @param pokeCon     参照するPOKECON
 */
//=============================================================================================
void PosPoke::Init( const MainModule* mainModule, const POKECON* pokeCon )
{
  for( u32 i=0; i<GFL_NELEMS(m_state); ++i )
  {
    m_state[i].fEnable = false;
    m_state[i].existPokeID = BTL_POKEID_NULL;
    m_state[i].clientID = BTL_CLIENTID_NULL;
  }

  for( u32 i=0; i<GFL_NELEMS(m_lastPos); ++i ) 
  {
    m_lastPos[i] = BTL_POS_NULL;
  }

  const u32 endPos = mainModule->GetValidPosMax();

  for( u32 i=0; i<=endPos; ++i ) 
  {
    BtlPokePos pos = static_cast<BtlPokePos>(i);
    this->ExtendPos( *mainModule, pos );
  }

  for( u32 i=0; i<=endPos; ++i ) 
  {
    BtlPokePos pos = static_cast<BtlPokePos>(i);
    setInitialFrontPokemon( mainModule, pokeCon, pos );
  }
}

/**
 * @brief 戦闘開始時に場にいるポケモンを登録する
 * @param mainModule  参照するMainModule
 * @param pokeCon     参照するPOKECON
 * @param pos         登録する立ち位置
 */
void PosPoke::setInitialFrontPokemon( const MainModule* mainModule, const POKECON* pokeCon, BtlPokePos pos )
{
  u8 clientID, memberIdx;

  mainModule->BtlPosToClientID_and_PosIdx( pos, &clientID, &memberIdx );
  if( clientID != BTL_CLIENTID_NULL )
  {
    const BTL_POKEPARAM*bpp = pokeCon->GetClientPokeDataConst( clientID, memberIdx );

    if( bpp != NULL && !(bpp->IsDead()) )
    {
      u8 pokeID = bpp->GetID();
      m_state[ pos ].existPokeID = pokeID;
      m_lastPos[ pokeID ] = pos;
    }
    else
    {
      m_state[ pos ].existPokeID = BTL_POKEID_NULL;
    }
  }
}

//=============================================================================================
/**
 * @brief 管理対象の立ち位置を拡張する
 *
 * @param mainModule  参照するMainModule
 * @param pos         拡張する立ち位置
 */
//=============================================================================================
void PosPoke::ExtendPos( const MainModule& mainModule, BtlPokePos pos )
{
  // 既に拡張されている
  if( m_state[ pos ].fEnable )
  {
    return;
  }

  // 拡張する立ち位置を担当するクライアントが存在しない
  u8 clientId = mainModule.BtlPosToClientID( pos );
  if( clientId == BTL_CLIENTID_NULL )
  {
    GFL_ASSERT(0);
    return;
  }

  m_state[ pos ].fEnable     = true;
  m_state[ pos ].clientID    = clientId;
  m_state[ pos ].existPokeID = BTL_POKEID_NULL;
}

//=============================================================================================
/**
 * ポケモン退場の通知受け取り
 *
 * @param   wk
 * @param   pokeID
 */
//=============================================================================================
void PosPoke::PokeOut( u8 pokeID )
{
  for(u32 i=0; i<GFL_NELEMS(m_state); ++i)
  {
    if( m_state[i].fEnable && (m_state[i].existPokeID == pokeID) ){
      m_state[i].existPokeID = BTL_POKEID_NULL;
      TAYA_PRINT("[pospoke] ポケモン(pos:%d, ID:%d)を退出させる\n", i, pokeID);
      return;
    }
  }

  GFL_ASSERT_MSG(0, "not exist pokeID=%d\n", pokeID);
//  TAYA_Printf("set break point\n");
}
//=============================================================================================
/**
 * ポケモン入場の通知受け取り
 *
 * @param   wk
 * @param   pokeID
 */
//=============================================================================================
void PosPoke::PokeIn( const MainModule* mainModule, BtlPokePos pos,  u8 pokeID, POKECON* pokeCon )
{
  GFL_ASSERT_MSG(m_state[pos].fEnable, "pos=%d\n", pos);
  m_state[pos].existPokeID = pokeID;
  m_lastPos[ pokeID ] = pos;
  BTL_N_Printf( DBGSTR_POSPOKE_In, pokeID, pos );

  checkConfrontRec( mainModule, pos, pokeCon );
}
//----------------------------------------------------------------------------------
/**
 * 対面レコード更新
 *
 * @param   wk
 * @param   pos
 * @param   pokeCon
 */
//----------------------------------------------------------------------------------
void PosPoke::checkConfrontRec( const MainModule* mainModule, BtlPokePos pos, POKECON* pokeCon )
{
  u8 pokeID = m_state[pos].existPokeID;

  if( pokeID != BTL_POKEID_NULL )
  {
    for(u32 i=0; i<GFL_NELEMS(m_state); ++i)
    {
      if( (m_state[i].fEnable)
      &&  (m_state[i].existPokeID != BTL_POKEID_NULL)
      &&  (m_state[i].existPokeID != pokeID )
      ){
        if( !(mainModule->IsFriendPokeID(m_state[i].existPokeID, pokeID)) )
        {
          BTL_Printf("%d と%d が対面\n", pokeID, m_state[i].existPokeID);

          BTL_POKEPARAM* bpp = pokeCon->GetPokeParam( m_state[i].existPokeID );
          bpp->Confront_Set( pokeID );

          bpp = pokeCon->GetPokeParam( pokeID );
          bpp->Confront_Set( m_state[i].existPokeID );
        }
      }
    }
  }
}

//=============================================================================================
/**
 * ポケモン位置交換
 *
 * @param   wk
 * @param   pos1
 * @param   pos2
 */
//=============================================================================================
void PosPoke::Swap( BtlPokePos pos1, BtlPokePos pos2 )
{
  State tmp = m_state[ pos1 ];
  m_state[ pos1 ] = m_state[ pos2 ];
  m_state[ pos2 ] = tmp;

  updateLastPos( pos1 );
  updateLastPos( pos2 );
}
//----------------------------------------------------------------------------------
/**
 * ポケモン最終位置ワークを更新
 *
 * @param   pos
 */
//----------------------------------------------------------------------------------
void PosPoke::updateLastPos( BtlPokePos pos )
{
  u8 pokeID = m_state[ pos ].existPokeID;

  if( pokeID != BTL_POKEID_NULL )
  {
    m_lastPos[ pokeID ] = pos;
  }
}


//=============================================================================================
/**
 * 指定クライアントの担当している「位置」の内、空きになっている位置IDを取得
 *
 * @param   wk
 * @param   clientID
 * @param   pos         [out] 空き位置ID（BtlPokePos）
 *
 * @retval  u8    空き位置数
 */
//=============================================================================================
u8 PosPoke::GetClientEmptyPos( u8 clientID, u8* pos ) const
{
  u32 cnt = 0;

  for(u32 i=0; i<GFL_NELEMS(m_state); ++i)
  {
    if( m_state[i].fEnable && (m_state[i].clientID == clientID) )
    {
      if( m_state[i].existPokeID == BTL_POKEID_NULL ){
        pos[ cnt++ ] = i;
      }
    }
  }
  return cnt;
}
//=============================================================================================
/**
 * 指定クライアントの担当している「位置」の内、空きになっている位置数を取得
 *
 * @param   clientID
 *
 * @retval  u8    空き位置数
 */
//=============================================================================================
u8 PosPoke::GetClientEmptyPosCount( u8 clientID ) const
{
  u32 cnt = 0;

  for(u32 i=0; i<GFL_NELEMS(m_state); ++i)
  {
    if( m_state[i].fEnable && (m_state[i].clientID == clientID) )
    {
      if( m_state[i].existPokeID == BTL_POKEID_NULL ){
        ++cnt;
      }
    }
  }
  return cnt;
}
//=============================================================================================
/**
 * 指定ポケモンが戦闘の場に出ているか判定
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  bool
 */
//=============================================================================================
bool PosPoke::IsExist( u8 pokeID ) const
{
  return ( GetPokeExistPos(pokeID) != BTL_POS_NULL );
}
//=============================================================================================
/**
 * 指定ポケモンが戦闘の場に出ているか判定
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  bool
 */
//=============================================================================================
bool PosPoke::IsExistFrontPos( const MainModule* mainModule, u8 pokeID ) const
{
  BtlPokePos pos = GetPokeExistPos( pokeID );
  if( pos != BTL_POS_NULL )
  {
    return mainModule->IsFrontPos( pos );
  }
  return false;
}
//=============================================================================================
/**
 * 指定ポケモンがいる場所IDを取得
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  BtlPokePos    出ている場合は位置ID、出ていない場合は BTL_POS_NULL
 */
//=============================================================================================
BtlPokePos PosPoke::GetPokeExistPos( u8 pokeID ) const
{
  for(u32 i=0; i<GFL_NELEMS(m_state); ++i)
  {
    if( m_state[i].fEnable && (m_state[i].existPokeID == pokeID) ){
      return i;
    }
  }
  return BTL_POS_NULL;
}
//=============================================================================================
/**
 * 指定ポケモンが最後に居た位置を返す（死んでいる場合にも有効な値が返る）
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  BtlPokePos    最後に居た位置／一度も場に出ていないポケモンの場合 BTL_POS_NULL
 */
//=============================================================================================
BtlPokePos PosPoke::GetPokeLastPos( u8 pokeID ) const
{
  return m_lastPos[ pokeID ];
}

//=============================================================================================
/**
 * 指定位置にいるポケモンIDを返す（誰もいなければ BTL_POKEID_NULL）
 *
 * @param   wk
 * @param   pos
 *
 * @retval  u8
 */
//=============================================================================================
u8 PosPoke::GetExistPokeID( BtlPokePos pos ) const
{
  GFL_ASSERT(pos<BTL_POS_NUM);
  GFL_ASSERT(m_state[pos].fEnable);

  return m_state[ pos ].existPokeID;
}
GFL_NAMESPACE_END(btl)



