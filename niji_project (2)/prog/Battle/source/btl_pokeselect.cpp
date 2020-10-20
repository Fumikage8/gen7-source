//=============================================================================================
/**
 * @file  btl_pokeselect.cpp
 * @brief ポケモンXY バトルシステム ポケモン選択画面用パラメータ
 * @author  taya
 *
 * @date  2011.03.01  WBより移植
 */
//=============================================================================================

#include "./btl_pokeselect.h"

#if defined(GF_PLATFORM_WIN32)
#pragma warning( disable  : 4800 )  //ブール値を 'true' または 'false' に強制的に設定します (警告の処理)
#endif

GFL_NAMESPACE_BEGIN(btl)

//=============================================================================================
/**
 * パラメータ構造体を初期化
 *
 * @param   param       [out]
 * @param   party       パーティデータポインタ
 * @param   numSelect   選択する必要のあるポケモン数
 *
 */
//=============================================================================================
void PokeSelParam::Init( const BTL_PARTY* party, u8 numSelect )
{
  m_party = party;
  m_numSelect = numSelect;
  m_bDisabledPutPosSequence = false;

  for(u32 i=0; i<GFL_NELEMS(m_prohibit); ++i)
  {
    m_prohibit[i] = BTL_POKESEL_CANT_NONE;
  }
}
//=============================================================================================
/**
 * 既に戦闘に出ているポケモンを選べないように設定する
 *
 * @param   param       [out]
 * @param   numCover    戦闘に出ているポケモン数
 *
 */
//=============================================================================================
void PokeSelParam::SetProhibitFighting( u8 numCover )
{
  for(u32 i=0; i<numCover; ++i)
  {
    SetProhibit( BTL_POKESEL_CANT_FIGHTING, i );
  }
}
//=============================================================================================
/**
 * パラメータ構造体に対し、追加情報として個別に選択できない理由を設定する
 *
 * @param   param
 * @param   reason
 * @param   idx
 *
 */
//=============================================================================================
void PokeSelParam::SetProhibit( PokeselReason reason, u8 idx )
{
  GFL_ASSERT(idx<GFL_NELEMS(m_prohibit));
  m_prohibit[ idx ] = reason;
}
//=============================================================================================
/**
 * 選択するべき数を取得
 *
 * @param   param
 *
 * @retval  u8
 */
//=============================================================================================
u8 PokeSelParam::GetNumSelect( void ) const
{
  return m_numSelect;
}

/**
 * 繰り出す位置の選択シーケンスを実行しないように設定する
 */
void PokeSelParam::DisablePutPosSequence( void )
{
  m_bDisabledPutPosSequence = true;
}
/**
 * 繰り出す位置の選択シーケンス無効判定
 * @return
 */
bool PokeSelParam::IsDisabledPutPosSequence( void ) const
{
  return m_bDisabledPutPosSequence;
}


//======================================================================================================
//======================================================================================================

//=============================================================================================
/**
 * リザルト構造体を初期化
 *
 * @param   result    [out]
 *
 */
//=============================================================================================
void PokeSelResult::Init( const PokeSelParam* param )
{
  m_cnt = 0;
  m_max = param->m_numSelect;

  for(uint32_t i=0; i<BTL_POSIDX_MAX; ++i) {
    m_useItem[i] = ITEM_DUMMY_DATA;
    m_wazaIdx[i] = 0;
  }
}
//=============================================================================================
/**
 * リザルト構造体に選択結果を１件追加
 *
 * @param   result    [out]
 * @param   outPokeIdx   入れ替わりに出て行くポケモンのIndex
 * @param   selPokeIdx   選択された（入ってくる）ポケモンのIndex
 *
 */
//=============================================================================================
void PokeSelResult::Push( u8 outPokeIdx, u8 selPokeIdx )
{
  if( m_cnt < m_max )
  {
    m_selIdx[ m_cnt ] = selPokeIdx;
    m_outPokeIdx[ m_cnt ] = outPokeIdx;
    m_useItem[ m_cnt ] = ITEM_DUMMY_DATA;
    m_wazaIdx[ m_cnt ] = 0;
    BTL_PRINT("[PokeSelResult] idx:%d, outPokeIdx:%d, selPokeIdx:%d\n", m_cnt, outPokeIdx, selPokeIdx);
    ++(m_cnt);
  }
  else
  {
    GFL_ASSERT_MSG(0, "cnt=%d, max=%d", m_cnt,m_max);
  }
}
//=============================================================================================
/**
 * リザルト構造体から選択結果を１件削除
 */
//=============================================================================================
void PokeSelResult::Pop( void )
{
  if( m_cnt )
  {
    m_cnt--;
  }
}
//=============================================================================================
/**
 * キャンセルステートの設定
 *
 * @param   bool flg
 *
 */
//=============================================================================================
void PokeSelResult::SetCancel( bool flg )
{
  m_fCancel = flg;
}
//=============================================================================================
/**
 * キャンセルされたか判定
 *
 * @param   result
 *
 * @retval  BOOL
 */
//=============================================================================================
bool PokeSelResult::IsCancel( void ) const
{
  return m_fCancel;
}
//=============================================================================================
/**
 * 選択し終わったか判定
 *
 * @param   result    リザルト構造体ポインタ
 *
 * @retval  BOOL
 */
//=============================================================================================
bool PokeSelResult::IsDone( void ) const
{
  return m_cnt == m_max;
}
//=============================================================================================
/**
 * 選んだ数を取得
 *
 * @param   result
 *
 * @retval  u8
 */
//=============================================================================================
u8 PokeSelResult::GetCount( void ) const
{
  return m_cnt;
}
//=============================================================================================
/**
 * 最後に選んだポケモンの並び順を返す
 *
 * @param   result
 *
 * @retval  u8
 */
//=============================================================================================
u8 PokeSelResult::GetLast( void ) const
{
  if( m_cnt > 0 )
  {
    return m_selIdx[ m_cnt - 1 ];
  }
  else
  {
    return BTL_PARTY_MEMBER_MAX;
  }
}
//=============================================================================================
/**
 * 選んだポケモンの並び順を返す
 *
 * @param   result
 * @param   idx       何番目に選んだ？
 *
 * @retval  u8
 */
//=============================================================================================
u8 PokeSelResult::Get( u8 idx ) const
{
  if( idx < m_cnt ){
    return m_selIdx[ idx ];
  }
  return BTL_PARTY_MEMBER_MAX;
}

//=============================================================================================
/**
 * アイテム使用モードとしての結果を格納する
 *
 * @param   itemNo
 */
//=============================================================================================
void  PokeSelResult::SetItemUse( u8 pokeIdx, u16 itemNo, u8 wazaIdx )
{
  if( m_cnt < m_max )
  {
    m_selIdx[ m_cnt ] = pokeIdx;
    m_outPokeIdx[ m_cnt ] = 0;
    m_useItem[ m_cnt ] = itemNo;
    m_wazaIdx[ m_cnt ] = wazaIdx;
    ++m_cnt;
  }
  else
  {
    GFL_ASSERT_MSG(0, "cnt=%d, max=%d", m_cnt,m_max);
  }
}
//=============================================================================================
/**
 * 最後に選んだポケモンについて、プレイヤーがアイテム使用する決定をしたかどうか判定
 *
 * @retval  bool
 */
//=============================================================================================
bool  PokeSelResult::IsItemUse( u8* pokeIdx, u16* itemNo, u8* wazaIdx ) const
{
  if( m_cnt == 0 ){
    return false;
  }

//  uint32_t idx = this->GetLast();
  uint32_t idx = m_cnt - 1;
  if( m_useItem[idx] != ITEM_DUMMY_DATA )
  {
    *pokeIdx = m_selIdx[ idx ];
    *wazaIdx = m_wazaIdx[ idx ];
    *itemNo  = m_useItem[ idx ];
    return true;
  }
  *pokeIdx = 0;
  *wazaIdx = 0;
  *itemNo = ITEM_DUMMY_DATA;
  return false;
}

GFL_NAMESPACE_END(btl)

#if defined(GF_PLATFORM_WIN32)
#pragma warning( default  : 4800 )
#endif