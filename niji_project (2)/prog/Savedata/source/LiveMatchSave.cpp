//==============================================================================
/**
 * @file   LiveMatchSave.cpp
 * @date   2015/11/26 Thu. 13:18:40
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <debug/include/gfl2_Assert.h>
#include  <debug/include/gfl2_DebugPrint.h>
#include  <util/include/gfl2_std_string.h>
#include  "../include/LiveMatchSave.h"


namespace Savedata  {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @brief    ctor
  */
//------------------------------------------------------------------
LiveMatchData::LiveMatchData(void)
  : m_LiveMatchSaveData()
{
  STATIC_ASSERT( sizeof(m_LiveMatchSaveData) == 66856 ); /*  想定外のサイズ変更を検知するため  */
  Clear(NULL);
}

//------------------------------------------------------------------
/**
  *  @brief    dtor
  */
//------------------------------------------------------------------
LiveMatchData::~LiveMatchData()
{
  /*  nop  */
}


//------------------------------------------------------------------
/**
  *  @brief    
  *  @note  implement SaveDataInterface I/F
  */
//------------------------------------------------------------------
void*   LiveMatchData::GetData(void)
{
  return &m_LiveMatchSaveData;
}


//------------------------------------------------------------------
/**
  *  @brief    
  *  @note  implement SaveDataInterface I/F
  */
//------------------------------------------------------------------
size_t  LiveMatchData::GetDataSize(void)
{
  return LIVEMATCH_SAVEDATA_SIZE;
}


//------------------------------------------------------------------
/**
  *  @brief    
  *  @note  implement SaveDataInterface I/F
  */
//------------------------------------------------------------------
void  LiveMatchData::Clear(gfl2::heap::HeapBase* pTempHeap)
{
  gfl2::std::MemClear(&m_LiveMatchSaveData, sizeof(m_LiveMatchSaveData));

  SetLiveEntryWork_Initial();
  InvalidatePartyIndex();
  m_LiveMatchSaveData.live_results.Clear();
}


//------------------------------------------------------------------
/**
  * @breif   ライブ大会成績設定
  * @param   results 成績
  */
//------------------------------------------------------------------
void LiveMatchData::SetLiveResults( LIVE_RESULTS* results )
{
  m_LiveMatchSaveData.live_results  = *results;
}


//------------------------------------------------------------------
/**
  *  @brief  ライブ大会対戦数カウントアップ
  *  @param  is_draw 引き分けかどうか
  *  @param  is_win  true:勝ち false:負け
  */
//------------------------------------------------------------------
void LiveMatchData::AddLiveResultCountUp( bool is_draw, bool is_win )
{
  if( is_draw )     m_LiveMatchSaveData.live_results.ApplyResult_Draw();
  else if( is_win ) m_LiveMatchSaveData.live_results.ApplyResult_Win();
  else              m_LiveMatchSaveData.live_results.ApplyResult_Lose();
/*
  ICHI_PRINT("ライブ大会対戦数カウントアップ count %d win %d lose %d \n",
             m_regulation.live_results.battle_num,
             m_regulation.live_results.win_num,
             m_regulation.live_results.lose_num
      );
*/
}


//------------------------------------------------------------------
/**
  * @breif   ライブ大会成績取得
  * @return  results 成績
  */
//------------------------------------------------------------------
const LiveMatchData::LIVE_RESULTS*  LiveMatchData::GetLiveResults( void ) const
{
  return &m_LiveMatchSaveData.live_results;
}


//------------------------------------------------------------------
/**
  * @brief   ライブ大会対戦履歴設定
  * @param   record  ライブ大会履歴構造体
  */
//------------------------------------------------------------------
void LiveMatchData::SetLiveRecord( LIVE_RECORD* record )
{
  m_LiveMatchSaveData.live_record_last  = (m_LiveMatchSaveData.live_record_last + 1) % LIVE_RECORD_MAX;

  m_LiveMatchSaveData.live_record[m_LiveMatchSaveData.live_record_last] = *record;

  GFL_PRINT("savedata:ライブ大会対戦履歴 %d\n", m_LiveMatchSaveData.live_record_last );
}


//------------------------------------------------------------------
/**
  * @brief   ライブ大会対戦履歴取得
  * @param   idx 取得したいデータのインデックス
  * 
  * @return  LIVE_RECORD   ライブ大会履歴構造体
  */
//------------------------------------------------------------------
LiveMatchData::LIVE_RECORD*  LiveMatchData::GetLiveRecord( int idx )
{
  GFL_ASSERT( idx < LIVE_RECORD_MAX );

  return &m_LiveMatchSaveData.live_record[ idx ];

}


//------------------------------------------------------------------
/**
  * @brief   ライブ大会対戦の最終履歴を取得
  * @return  LIVE_RECORD   ライブ大会履歴構造体
  */
//------------------------------------------------------------------
LiveMatchData::LIVE_RECORD*  LiveMatchData::GetLastLiveRecord( void )
{
  return &m_LiveMatchSaveData.live_record[ m_LiveMatchSaveData.live_record_last ];
}


//------------------------------------------------------------------
/**
  * @brief   ライブ大会参加ワーク設定
  */
//------------------------------------------------------------------
void LiveMatchData::SetLiveEntryWork( const u8 work )
{
  m_LiveMatchSaveData.live_entry_work = work;
}


//------------------------------------------------------------------
/**
  *  @brief    ライブ大会参加ワーク取得
  */
//------------------------------------------------------------------
u8   LiveMatchData::GetLiveEntryWork( void ) const
{
  return m_LiveMatchSaveData.live_entry_work;
}





} /*  namespace Savedata  */
