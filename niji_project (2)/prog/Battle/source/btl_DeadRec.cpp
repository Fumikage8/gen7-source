//=============================================================================================
/**
 * @file    btl_DeadRec.cpp
 * @brief   ポケモンXY 死亡ポケ記録
 * @author  taya
 *
 * @date  2011.01.06 作成
 */
//=============================================================================================

#include "./btl_DeadRec.h"
#include "./btl_mainmodule.h"

#if defined(GF_PLATFORM_WIN32)
#pragma warning( disable  : 4800 )  //ブール値を 'true' または 'false' に強制的に設定します (警告の処理)
#endif

GFL_NAMESPACE_BEGIN(btl)
//--------------------------------------------------
/**
 *  バトル開始時の初期化
 */
//--------------------------------------------------
void DeadRec::Init( const POKECON* pokeCon )
{
  for(u32 i=0; i<TURN_MAX; ++i){
    clearTurnRecord( &m_turnRecord[i] );
  }

  for(u32 i=0; i<BTL_POKEID_MAX; ++i)
  {
    if( pokeCon->IsExistPokemon(i) )
    {
      const BTL_POKEPARAM* bpp = pokeCon->GetPokeParamConst( i );
      m_currentDeadFlag[i] = bpp->IsDead();
    }
    else{
      m_currentDeadFlag[i] = 0;
    }

    m_deadCount[i] = 0;
  }

  clearReliveRecord();
}
//--------------------------------------------------
/**
 *  ターン開始処理
 */
//--------------------------------------------------
void  DeadRec::StartTurn( void )
{
  for(int i=TURN_MAX-1; i>0; --i)
  {
    m_turnRecord[i] = m_turnRecord[i-1];
  }
  clearTurnRecord( &m_turnRecord[0] );

  clearReliveRecord();
}
//--------------------------------------------------
/**
 *  死亡ポケID追加
 */
//--------------------------------------------------
void  DeadRec::Add( u8 pokeID )
{
  u8 cnt = m_turnRecord[0].cnt;
  if( cnt < BTL_POKEID_MAX )
  {
    m_turnRecord[0].deadPokeID[ cnt ] = pokeID;
    m_turnRecord[0].fExpChecked[ cnt ] = false;
    m_turnRecord[0].cnt++;
  }
  else{
    GFL_ASSERT(0);
  }


  if( pokeID < BTL_POKEID_MAX )
  {
    m_currentDeadFlag[ pokeID ] = true;
    if( m_deadCount[ pokeID ] < DEAD_COUNT_MAX ){
      ++(m_deadCount[ pokeID ]);
    }
  }else{
    GFL_ASSERT(0);
  }
}
//------------------------------------------------
/**
 * ポケモンの生き返り通知
 * @param pokeID
 */
//------------------------------------------------
void  DeadRec::Relive( u8 pokeID )
{
  if( pokeID < BTL_POKEID_MAX )
  {
    if( m_currentDeadFlag[ pokeID ] )
    {
      m_currentDeadFlag[ pokeID ] = false;

      if( !isRelivedThisTurn(pokeID) )
      {
        if( m_relivePokeCount < GFL_NELEMS(m_relivePokeID) ){
          m_relivePokeID[ m_relivePokeCount++ ] = pokeID;
        }
        else{
          GFL_ASSERT(0);
        }
      }
    }
  }
  else{
    GFL_ASSERT(0);
  }
}

//------------------------------------------------
/**
 * 指定ポケモンが現在死んでいるかどうか判定
 * @param pokeID
 * @return  死んでいたら true 
 */
//------------------------------------------------
bool  DeadRec::IsDeadNow( u8 pokeID ) const
{
  if( pokeID < BTL_POKEID_MAX ){
    return m_currentDeadFlag[ pokeID ];
  }else{
    GFL_ASSERT(0);
    return false;
  }
}
//------------------------------------------------
/**
 * 指定クライアントIDのポケモンが、このバトル中に「ひん死」になった回数を返す
 * @param clientID
 * @return  指定クライアントIDのポケモンが、このバトル中に「ひん死」になった回数
 */
//------------------------------------------------
u32    DeadRec::GetDeadCountByClientID( u8 clientID ) const
{
  u32 count = 0;

  for(uint32_t i=0; i<BTL_POKEID_MAX; ++i)
  {
    if( MainModule::PokeIDtoClientID(i) == clientID )
    {
      count += m_deadCount[i];
    }
  }

  return count;
}

//--------------------------------------------------
/**
 *  経験値取得済みフラグセット
 */
//--------------------------------------------------
void  DeadRec::SetExpCheckedFlag( u8 turn, u8 idx )
{
  if( turn < TURN_MAX )
  {
    if( idx < m_turnRecord[turn].cnt )
    {
      m_turnRecord[turn].fExpChecked[ idx ] = true;
    }
  }
}
//--------------------------------------------------
/**
 *  指定ターン内に死んだポケ数を取得
 */
//--------------------------------------------------
u8    DeadRec::GetCount( u8 turn ) const
{
  if( turn < TURN_MAX ){
    return m_turnRecord[ turn ].cnt;
  }else{
    GFL_ASSERT(0);
    return 0;
  }
}
//--------------------------------------------------
/**
 *  指定ターン内に死んだポケIDを取得
 */
//--------------------------------------------------
u8    DeadRec::GetPokeID( u8 turn, u8 idx ) const
{
  if( turn < TURN_MAX )
  {
    if( idx < m_turnRecord[turn].cnt )
    {
      return m_turnRecord[turn].deadPokeID[ idx ];
    }
  }
  return BTL_POKEID_NULL;
}
//--------------------------------------------------
/**
 *  経験値取得済みフラグチェック
 */
//--------------------------------------------------
bool  DeadRec::GetExpCheckedFlag( u8 turn, u8 idx ) const
{
  if( turn < TURN_MAX )
  {
    if( idx < m_turnRecord[turn].cnt )
    {
      return m_turnRecord[turn].fExpChecked[ idx ];
    }
  }
  return false;
}
//--------------------------------------------------
/**
 * 最後に死亡したポケモンIDを取得
 */
//--------------------------------------------------
u8    DeadRec::GetLastDeadPokeID( void ) const
{
  for(u32 i=0; i<TURN_MAX; ++i)
  {
    if( m_turnRecord[i].cnt )
    {
      u32 n = m_turnRecord[i].cnt - 1;
      return m_turnRecord[i].deadPokeID[n];
    }
  }
  return BTL_POKEID_NULL;
}
/**
 * 現ターンに生き返ったポケモンを、繰り出せる空き位置が存在するかどうか判定
 * @param posPoke
 * @return 繰り出せる空き位置があれば true
 */
bool   DeadRec::IsRelivedPokePuttableEmptyPos( const PosPoke& posPoke ) const
{
  for (u32 i=0; i<m_relivePokeCount; ++i)
  {
    u8 clientID = MainModule::PokeIDtoClientID( m_relivePokeID[i] );
    if( posPoke.GetClientEmptyPosCount(clientID) ){
      return true;
    }
  }
  return false;
}

/**
 *  ターン記録単位の初期化
 */
void DeadRec::clearTurnRecord( Unit* unit )
{
  unit->cnt = 0;

  for(u32 i=0; i<BTL_POKEID_MAX; ++i)
  {
    unit->deadPokeID[ i ]  = BTL_POKEID_NULL;
    unit->fExpChecked[ i ] = false;
  }
}

/**
 * 現ターン生き返り記録の初期化
 */
void DeadRec::clearReliveRecord( void )
{
  for (uint32_t i=0; i<GFL_NELEMS(m_relivePokeID); ++i) {
    m_relivePokeID[i] = BTL_POKEID_NULL;
  }
  m_relivePokeCount = 0;
}

/**
 * 現ターンに生き返ったことがあるポケモンかどうか判定
 */
bool DeadRec::isRelivedThisTurn( u8 pokeID ) const
{
  for (uint32_t i=0; i<GFL_NELEMS(m_relivePokeID); ++i)
  {
    if( m_relivePokeID[i] == pokeID ){
      return true;
    }
  }
  return false;
}

GFL_NAMESPACE_END(btl)


#if defined(GF_PLATFORM_WIN32)
#pragma warning( default  : 4800 )
#endif

