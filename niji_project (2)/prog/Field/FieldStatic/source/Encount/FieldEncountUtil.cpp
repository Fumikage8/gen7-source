//=============================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file FieldEncountUtil.cpp
 *  @brief エンカウント関連の便利関数
 *  @author saita_kazuki
 *  @date 2015.1.14
 */
//=============================================================================

#include "Field/FieldStatic/include/Encount/FieldEncountUtil.h"

// gflib2
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <util/include/gfl2_std_string.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/script_event_data.gaix>


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Encount )

//----------------------------------------------------------------------------
/**
 * @brief シンボルエンカウントデータの同期読み込み
 * @param pHeap garc読み込みに使用するヒープ
 * @param pAsyncFileManager ファイル読み込み管理クラス
 * @param tableID シンボルエンカウントテーブルID
 * @param pOut パラメータの格納先
 */
//-----------------------------------------------------------------------------
void EncountUtil::SyncLoadSymbolEncountParam( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager, u32 tableID, SymbolEncountParam* pOut )
{
  size_t  allSize = 0;
  void*   pBuffer = NULL;

  // シンボルエンカウントデータarcオープン
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId           = ARCID_SCRIPT_EVENT_DATA;
    openReq.heapForFile     = pHeap->GetLowerHandle();
    openReq.heapForArcSrc   = pHeap->GetLowerHandle();
    openReq.heapForReq      = pHeap->GetLowerHandle();
    pAsyncFileManager->SyncArcFileOpen( openReq );
  }
  // シンボルエンカウントデータ読み込み
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
    loadReq.arcId       = ARCID_SCRIPT_EVENT_DATA;
    loadReq.datId       = GARC_script_event_data_symbol_encount_BIN;
    loadReq.ppBuf       = &pBuffer;
    loadReq.pBufSize    = &allSize;
    loadReq.heapForBuf  = pHeap;
    loadReq.align       = 4;
    pAsyncFileManager->SyncArcFileLoadData( loadReq );
  }

  // パラメーター取得
  Encount::SymbolEncountParam* pSymbolEncountParam = reinterpret_cast<Encount::SymbolEncountParam*>( pBuffer );
  size_t oneSize = sizeof( Encount::SymbolEncountParam );
  u32 paramNum = allSize / oneSize;
  if( tableID >= paramNum )
  {
    GFL_ASSERT_MSG( 0,"SymbolEncount table index is over. : %d >= %d", tableID, paramNum );
    tableID = 0;
  }
  gfl2::std::MemCopy( &pSymbolEncountParam[tableID], pOut, oneSize );

  // バッファ解放
  GflHeapSafeFreeMemory( pBuffer );

  // シンボルエンカウントデータarcクローズ
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
    closeReq.arcId      = ARCID_SCRIPT_EVENT_DATA;
    closeReq.heapForReq = pHeap;
    pAsyncFileManager->SyncArcFileClose( closeReq );
  }
}

GFL_NAMESPACE_END( Encount )
GFL_NAMESPACE_END( Field )

