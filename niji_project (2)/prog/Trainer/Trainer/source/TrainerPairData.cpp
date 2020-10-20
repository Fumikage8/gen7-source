//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		TrainetrPairData.cpp
 *	@brief  視線トレーナーペアデータ管理モジュール
 *	@author Miyuki Iwasawa	
 *	@date		2016.01.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Trainer/Trainer/include/TrainerPairData.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/script_event_data.gaix>

namespace trainer{

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  /**
   *	@brief  コンストラクタ
   */
  //-----------------------------------------------------------------------------
  TrainerPairData::TrainerPairData( const GameSys::GameData* p_gdata )
  {
    m_pGameData = p_gdata;
    m_TrainerPairSize = 0;
    m_seq = 0;
    m_pHeap = NULL;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  デストラクタ
   */
  //-----------------------------------------------------------------------------
  TrainerPairData::~TrainerPairData()
  {
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  ペアデータのサーチ
   */
  //-----------------------------------------------------------------------------
  TrainerPairData::TrainerPairIndex TrainerPairData::SearchPairData( u32 scrid, u32* scrid_pair )
  {
    PairData* pPairTable = reinterpret_cast<PairData*>(m_pTrainerPairBuffer);
    
    int i = 0;
    for( i=0;i < m_TrainerPairSize; i++ ){
      if( pPairTable[i].scrid_trainer01 == scrid ){
        *scrid_pair = pPairTable[i].scrid_trainer02;
        return TRAINER_PAIR_1ST;
      }
      if( pPairTable[i].scrid_trainer02 == scrid ){
        *scrid_pair = pPairTable[i].scrid_trainer01;
        return TRAINER_PAIR_2ND;
      }
    }
    *scrid_pair = 0;
    return TRAINER_PAIR_NONE;
  }


  //----------------------------------------------------------------------------
  /**
   *	@brief  TrainerPairDataデータロード
   */
  //-----------------------------------------------------------------------------
  void TrainerPairData::Initialize( GameSys::GameManager* p_gman, gfl2::heap::HeapBase* p_heap )
  {
    gfl2::fs::AsyncFileManager* pAsyncFileManager = p_gman->GetAsyncFileManager();

    m_seq = 0;
    m_pHeap = p_heap;

    //---------------------------------------------------
    // ARCファイルオープン
    //---------------------------------------------------
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
      openReq.arcId = ARCID_SCRIPT_EVENT_DATA;
      openReq.heapForFile = m_pHeap->GetLowerHandle();
      openReq.heapForArcSrc = m_pHeap->GetLowerHandle();
      openReq.heapForReq = m_pHeap->GetLowerHandle();
      pAsyncFileManager->AddArcFileOpenReq( openReq );
    }
    //---------------------------------------------------
    // ファイル読み込みリクエスト
    //---------------------------------------------------
    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;

      loadReq.arcId = ARCID_SCRIPT_EVENT_DATA;
      loadReq.datId = GARC_script_event_data_pair_trainer_BIN;
      loadReq.ppBuf = &m_pTrainerPairBuffer;
      loadReq.heapForBuf = m_pHeap;
      loadReq.heapForReq = m_pHeap->GetLowerHandle();
      loadReq.heapForCompressed = NULL;
      loadReq.pBufSize = &m_TrainerPairBufferSize;
      loadReq.align = 4;
      pAsyncFileManager->AddArcFileLoadDataReq( loadReq );  
    }
  }
  //----------------------------------------------------------------------------
  /**
   *	@brief  TrainerPairDataデータ待ち
   */
  //-----------------------------------------------------------------------------
  bool TrainerPairData::InitializeWait( GameSys::GameManager* p_gman )
  {
    gfl2::fs::AsyncFileManager* pAsyncFileManager = p_gman->GetAsyncFileManager();
  
    switch( m_seq ){
    case 0:
      // 読み込み完了待ち
      if( !pAsyncFileManager->IsArcFileOpenFinished( ARCID_SCRIPT_EVENT_DATA ) ){
        return false;
      }
      if( !pAsyncFileManager->IsArcFileLoadDataFinished( &m_pTrainerPairBuffer ) ){
        return false;
      }

      //データ格納
      {
        m_TrainerPairSize = m_TrainerPairBufferSize/sizeof(PairData);
        GFL_ASSERT(m_TrainerPairSize==TRAINER_PAIR_NUM_MAX);
      }

      //アーカイブ閉じる
      {
        gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
        closeReq.arcId = ARCID_SCRIPT_EVENT_DATA;
        closeReq.heapForReq = m_pHeap->GetLowerHandle();
        pAsyncFileManager->AddArcFileCloseReq( closeReq );
      }
      m_seq++;
      return false;

    case 1:
      if( !pAsyncFileManager->IsArcFileCloseFinished( ARCID_SCRIPT_EVENT_DATA ) ){
        return false;
      }

      m_seq++;
      break;

    default:
      break;
    }
    return true;
  }
  //----------------------------------------------------------------------------
  /**
   *	@brief  TrainerPairDataデータ破棄
   */
  //-----------------------------------------------------------------------------
  void TrainerPairData::Terminate( void )
  {
    //バッファ開放
    GflHeapSafeFreeMemory(m_pTrainerPairBuffer);
    m_TrainerPairSize = 0;
    m_seq = false;
    m_pHeap = NULL;

    return;
  }


} // trainer 

