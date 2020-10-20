//=============================================================================================
/**
 * @file    btl_PokeSet.cpp
 * @brief   ポケモンXY バトルシステム 特定目的のポケモンパラメータセット管理
 * @author  taya
 *
 * @date  2011.01.07  作成
 */
//=============================================================================================

#include "btl_calc.h"
#include "btl_MainModule.h"
#include "btl_ServerFlow.h"
#include "btl_PokeParam.h"

#include "btl_PokeSet.h"



GFL_NAMESPACE_BEGIN(btl)

/**
 *  初期化
 */
void PokeSet::Clear( void )
{
  for(u32 i=0; i<BTL_POS_NUM; ++i)
  {
    m_bpp[ i ]    = NULL;
    m_damage[ i ] = 0;
    m_migawariDamage[ i ] = 0;
    m_damageType[ i ] = 0;
    m_sortWork[ i ] = 0;
  }

  m_count = 0;
  m_countMax = 0;
  m_getIdx = 0;
  m_targetPosCount = 0;
}

/**
 *  ポケモン１体登録
 */
void PokeSet::Add( BTL_POKEPARAM* bpp )
{
  if( bpp == NULL )
  {
    return;
  }

  if( m_count < GFL_NELEMS(m_bpp) )
  {
    for(u32 i=0; i<m_count; ++i)
    {
      if( m_bpp[i] == bpp ){
        return;
      }
    }
    m_bpp[ m_count ] = bpp;
    m_damageType[ m_count ] = DMGTYPE_NONE;
    m_damage[ m_count ] = 0;
    m_migawariDamage[ m_count ] = 0;
    m_count++;

    if( m_count > m_countMax ){
      m_countMax = m_count;
    }
  }
  else
  {
    GFL_ASSERT(0);
  }
}
/**
 *  ポケモン１体登録（ダメージ記録）
 */
void PokeSet::AddWithDamage( BTL_POKEPARAM* bpp, u16 damage, bool fMigawariDamage )
{
  Add( bpp );
  {
    for(u32 i=0; i<m_count; ++i)
    {
      if( m_bpp[i] == bpp )
      {
        if( fMigawariDamage ){
          m_migawariDamage[ i ] += damage;
          m_damageType[ i ] = DMGTYPE_MIGAWARI;
        }
        else{
          m_damage[ i ] += damage;
          m_damageType[ i ] = DMGTYPE_REAL;
        }
      }
    }
  }
}

/**
 *  ポケモン１体除外
 */
void PokeSet::Remove( BTL_POKEPARAM* bpp )
{
  u32 i;
  for(i=0; i<m_count; ++i)
  {
    if( m_bpp[i] == bpp )
    {
      u32 j;
      for(j=i+1; j<m_count; ++j)
      {
        m_bpp[j-1] = m_bpp[j];
      }
      m_count--;
      m_getIdx--;
      break;
    }
  }
}
/**
 *  ポケモンデータ取得
 */
BTL_POKEPARAM* PokeSet::Get( u32 idx ) const
{
  if( idx < m_count )
  {
    return (BTL_POKEPARAM*)(m_bpp[ idx ]);
  }
  return NULL;
}
/**
 *  順番アクセス開始
 */
void PokeSet::SeekStart( void )
{
  m_getIdx = 0;
}
/**
 *  順番アクセス（NULLが返ったら修了）
 */
BTL_POKEPARAM* PokeSet::SeekNext( void )
{
  if( m_getIdx < m_count )
  {
    return m_bpp[ m_getIdx++ ];
  }
  else
  {
    return NULL;
  }
}
/**
 *  ダメージ記録取得（実体・みがわりとも）
 */
bool PokeSet::GetDamage( const BTL_POKEPARAM* bpp, u32* damage ) const
{
  for(u32 i=0; i<m_count; ++i)
  {
    if( m_bpp[i] == bpp )
    {
      if( m_damageType[i] != DMGTYPE_NONE )
      {
        *damage = (m_damage[i] + m_migawariDamage[i]);
        return true;
      }
    }
  }
  GFL_ASSERT(0); // ポケモン見つからない
  return false;
}
/**
 *  ダメージ記録取得（実体のみ）
 */
bool PokeSet::GetDamageReal( const BTL_POKEPARAM* bpp, u32* damage ) const
{
  for(u32 i=0; i<m_count; ++i)
  {
    if( (m_bpp[i] == bpp)
    ){
      if( m_damageType[i] == DMGTYPE_REAL )
      {
        *damage = m_damage[i];
        return true;
      }
    }
  }
  return false;
}
/**
 * 受けたダメージタイプを取得
 */
PokeSet::DamageType  PokeSet::GetDamageType( const BTL_POKEPARAM* bpp ) const
{
  for(u32 i=0; i<m_count; ++i)
  {
    if( m_bpp[i] == bpp )
    {
      return (DamageType)(m_damageType[i]);
    }
  }
  GFL_ASSERT(0); // ポケモン見つからない
  return DMGTYPE_NONE;;
}

/**
 *  現在登録されているポケモン総数を取得
 */
u32 PokeSet::GetCount( void ) const
{
  return m_count;
}
/**
 *  初期化後、記録されたポケモン総数を取得
 */
u32 PokeSet::GetCountMax( void ) const
{
  return m_countMax;
}

void PokeSet::SetDefaultTargetCount( u8 cnt )
{
  m_targetPosCount = cnt;
}

/**
 *  本来は１体以上いたハズのターゲットが現在は0になってしまったケースをチェック
 */
bool PokeSet::IsRemovedAll( void ) const
{
  if( (m_countMax > 0) || ( m_targetPosCount > 0) )
  {
    return (m_count == 0);
  }
  return false;
}

/**
 *  生きてるポケモンのみコピー
 */
u32 PokeSet::CopyAlive( PokeSet* dst ) const
{
  u32 max = GetCount();

  dst->Clear();

  BTL_PRINT("[PokeSet] ターゲット最大 %d のウチ、生きているのだけコピーする\n", max);

  for(u32 i=0; i<max; ++i)
  {
    BTL_POKEPARAM* member = Get( i );
    if( !(member->IsDead()) )
    {
      BTL_PRINT("  %d は生きてるからコピー\n", member->GetID());
      dst->Add( member );
    }
    else{
      BTL_PRINT("  %d は生きてないから無視\n", member->GetID());
    }
  }
  return dst->GetCount();

}
/**
 *  指定ポケモンと同陣営のデータのみをコピー
 *
 *  @return u32 コピーしたポケモン数
 */
u32 PokeSet::CopyFriends( const MainModule& mainModule, const BTL_POKEPARAM* bpp, PokeSet* dst ) const
{
  u32 max = GetCount();
  u8  ID1 = bpp->GetID();
  u8  ID2;

  dst->Clear();

  for(u32 i=0; i<max; ++i)
  {
    BTL_POKEPARAM* member = Get( i );
    ID2 = member->GetID();
    if( mainModule.IsFriendPokeID(ID1, ID2) ){
      dst->Add( member );
    }
  }
  return dst->GetCount();
}
/**
 *  指定ポケモンと敵チームのポケモンのみを別ワークにコピー
 *
 *  @return u32 コピーしたポケモン数
 */
u32 PokeSet::CopyEnemys( const MainModule& mainModule, const BTL_POKEPARAM* bpp, PokeSet* dst ) const
{
  BTL_POKEPARAM* member;
  u32 max, i;
  u8 ID1, ID2;

  dst->Clear();
  ID1 = bpp->GetID();

  max = GetCount();
  for(i=0; i<max; ++i)
  {
    member = Get( i );
    ID2 = member->GetID();
    if( !mainModule.IsFriendPokeID(ID1, ID2) ){
      dst->Add( member );
    }
  }
  return dst->GetCount();
}
/**
 *  死んでる＆場に居ないポケモンを削除
 */
void PokeSet::RemoveDisablePoke( const PosPoke& posPoke )
{
  BTL_POKEPARAM* bpp;

  SeekStart();
  while( (bpp = SeekNext()) != NULL )
  {
    if( bpp->IsDead() )
    {
      BTL_PRINT(" [PokeSet] poke(%d) は死んでるから削除する\n", bpp->GetID());
      Remove( bpp );
      continue;
    }

    if( posPoke.GetPokeExistPos(bpp->GetID()) == BTL_POS_NULL ){
      BTL_PRINT(" [PokeSet] poke(%d) は場に居ないから削除する\n", bpp->GetID());
      Remove( bpp );
      continue;
    }
  }
}

/**
 *  当ターンの計算後素早さ順でソート（当ターンに行動していないポケは素の素早さ）
 */
void PokeSet::SortByAgility( ServerFlow* flowWk )
{
  if( m_count > 1 )
  {
    u32 i, j;

    for(i=0; i<m_count; ++i){
      m_sortWork[i] = flowWk->Hnd_CalcAgility( m_bpp[i], true );
    }

    for(i=0; i<m_count; ++i)
    {
      for(j=i+1; j<m_count; ++j)
      {
        if( (m_sortWork[j] > m_sortWork[i])
        ||  ((m_sortWork[j] == m_sortWork[i]) && calc::GetRand(2)) // 素早さ一致ならランダム
        ){
          BTL_POKEPARAM* tmpBpp;
          u16            tmpDmg;
          u16            tmpAgi;

          tmpBpp = m_bpp[i];
          tmpDmg = m_damage[i];
          tmpAgi = m_sortWork[i];
          m_bpp[i]      = m_bpp[j];
          m_damage[i]   = m_damage[j];
          m_sortWork[i] = m_sortWork[j];
          m_bpp[j]      = tmpBpp;
          m_damage[j]   = tmpDmg;
          m_sortWork[j] = tmpAgi;
        }
      }
    }
  }
}
GFL_NAMESPACE_END(btl)

