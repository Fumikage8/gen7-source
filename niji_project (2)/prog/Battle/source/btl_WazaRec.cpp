//=============================================================================================
/**
 * @file  btl_WazaRec.cpp
 * @brief 出たワザ記録機構
 * @author  taya
 *
 * @date  2011.01.06  作成
 */
//=============================================================================================

// #include "waza_tool\wazano_def.h"  // for WAZANO_NULL

#include "btl_WazaRec.h"


GFL_NAMESPACE_BEGIN(btl)

//=============================================================================================
/**
 * 初期化
 */
//=============================================================================================
void WazaRec::Init( void )
{
  m_ptr = 0;
  for(u32 i=0; i<GFL_NELEMS(m_record); ++i){
    m_record[i].wazaID = WAZANO_NULL;
  }
}
//=============================================================================================
/**
 * 出たワザ１件記録
 *
 * @param   rec
 * @param   waza
 * @param   turn
 * @param   pokeID
 */
//=============================================================================================
void WazaRec::Add( WazaID waza, u32 turn, u8 pokeID )
{
  m_record[ m_ptr ].wazaID     = waza;
  m_record[ m_ptr ].turn       = turn;
  m_record[ m_ptr ].pokeID     = pokeID;
  m_record[ m_ptr ].fEffective = false;
  m_ptr++;
  BTL_Printf( "ワザ[%d]記録、turn=%d, ptr=%d\n", waza, turn, m_ptr );
  if( m_ptr >= GFL_NELEMS(m_record) ){
    m_ptr = 0;
  }
}
//=============================================================================================
/**
 * 最後に記録したワザが効果があったことを記録
 */
//=============================================================================================
void WazaRec::SetEffectiveLast( void )
{
  int ptr = m_ptr - 1;
  if( ptr < 0 ){
    ptr = GFL_NELEMS(m_record) - 1;
  }
  m_record[ ptr ].fEffective = true;
}

//=============================================================================================
/**
 * 指定ターンにワザが使われていたかチェック
 *
 * @param   waza
 * @param   turn
 *
 * @retval  bool
 */
//=============================================================================================
bool WazaRec::IsUsedWaza( WazaNo waza, u32 turn ) const
{
  int p = m_ptr;

  BTL_Printf("WAZAREC:check Waza=%d, turn=%d\n", waza, turn);

  for ever
  {
    if( --p < 0 ){
      p = GFL_NELEMS(m_record) - 1;
    }
    if( ( static_cast<u32>(p) == m_ptr) || (m_record[p].wazaID == WAZANO_NULL) ){
      BTL_Printf("  seek p=%d, rec_ptr=%d, ... break;\n", p, m_ptr);
      break;
    }
    BTL_Printf("  ..HIT:Ptr=%d, turn=%d, waza=%d\n", p, m_record[p].turn, m_record[p].wazaID);
    if( (m_record[p].wazaID == waza) && (m_record[p].turn == turn) ){
      return true;
    }
  }
  return false;
}
//=============================================================================================
/**
 * 指定ターンにワザが使われた回数チェック
 *
 * @param   waza
 * @param   turn
 *
 * @retval  bool
 */
//=============================================================================================
u32 WazaRec::GetUsedWazaCount( WazaID waza, u32 turn ) const
{
  int p = m_ptr;
  u32 count = 0;

  for ever
  {
    if( --p < 0 ){
      p = GFL_NELEMS(m_record) - 1;
    }
    if( ( static_cast<u32>(p) == m_ptr) || (m_record[p].wazaID == WAZANO_NULL) ){
      break;
    }
    if( (m_record[p].wazaID == waza) && (m_record[p].turn == turn) ){
      ++count;
    }
  }
  return count;
}

//=============================================================================================
/**
 * 指定ターン、直前に有効だったワザのIDを返す
 *
 * @param   turn
 *
 * @retval  WazaID
 */
//=============================================================================================
WazaID WazaRec::GetPrevEffectiveWaza( u32 turn ) const
{
  int p = m_ptr;

  for ever
  {
    if( --p < 0 ){
      p = GFL_NELEMS(m_record) - 1;
    }
    if( ( static_cast<u32>(p) == m_ptr) || (m_record[p].wazaID == WAZANO_NULL) ){
      break;
    }
    if( (m_record[p].fEffective) && (m_record[p].turn == turn) ){
      return m_record[p].wazaID;
    }
  }
  return WAZANO_NULL;
}
GFL_NAMESPACE_END(btl)


