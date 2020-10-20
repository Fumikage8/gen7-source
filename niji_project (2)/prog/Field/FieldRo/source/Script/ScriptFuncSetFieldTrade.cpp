//==============================================================================
/**
 * @file        ScriptFuncSetFieldTrade.cpp
 * @brief       ScriptFuncSetCommon系 フィールドポケモン交換系統
    
 * @author      Sho Ohhira 
 * @date        2015/12/23
 */
//==============================================================================

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "Savedata/include/SaveData.h"
#include <arc_def_index/arc_def.h>

#include "FieldScript/include/ScriptWork.h"
#include "FieldScript/include/ScriptObject.h"
#include "FieldScript/include/FieldScriptSystem.h"
#include "System/include/HeapDefine.h"

#include "Field/FieldRo/include/Fieldmap.h"

//FieldTrade
#include <arc_index/script_event_data.gaix>
#include "App/AppEvent/include/PokeTrade/PokeTradeEvent.h"

#include "Field/FieldRo/include/Script/ScriptFuncSetCommon.h"
#include "niji_reference_files/script/FieldPawnTypes.h"

#include <arc_index/message.gaix>
#include <arc_index/script_message.gaix>

#include "Field/FieldStatic/include/Script/ScriptFuncSetAppCall.h"

// debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

namespace{
  #include <niji_conv_header/field/field_trade/FieldTradeData.h>//エクセル上でのID
}

/// @brief フィールド
GFL_NAMESPACE_BEGIN( Field )
/// @brief スクリプト
GFL_NAMESPACE_BEGIN( FieldScript )
//------------------------------------------------------------------------------
/**
 * @func        NPCとのポケモン交換：エクセルからのデータ取得開始
 * @brief       エクセルの読み込み。読み込み待ちと破棄処理も呼ぶ事
 * @author      ohhira_syo
 * @date        2015/11/27(金) 11:29:00
 */
//------------------------------------------------------------------------------
//エクセルからのデータ取得
cell AMX_NATIVE_CALL FuncSetCommon::FieldTradeExcelLoad( AMX * amx, const cell * ptr )
{
  //Alias
  GameSys::GameManager* p_gman = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::fs::AsyncFileManager* pAsyncFileManager = p_gman->GetAsyncFileManager();
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE );

  //データの格納先
  void** ppDataBuf = &p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp;

  //---------------------------------------------------
  // ARCファイルオープン
  //---------------------------------------------------
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = ARCID_SCRIPT_EVENT_DATA;
    openReq.heapForFile = p_heap->GetLowerHandle();
    openReq.heapForArcSrc = p_heap->GetLowerHandle();
    openReq.heapForReq = p_heap->GetLowerHandle();
    pAsyncFileManager->AddArcFileOpenReq( openReq );
  }
  //---------------------------------------------------
  // ファイル読み込みリクエスト
  //---------------------------------------------------
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;

    loadReq.arcId = ARCID_SCRIPT_EVENT_DATA;
    loadReq.datId = GARC_script_event_data_field_trade_BIN;
    loadReq.ppBuf = ppDataBuf;
    loadReq.heapForBuf = p_heap;
    loadReq.heapForReq = p_heap->GetLowerHandle();
    loadReq.heapForCompressed = NULL;
    //loadReq.pBufSize = &m_BufferSize;
    loadReq.align = 4;
    pAsyncFileManager->AddArcFileLoadDataReq( loadReq );  
  }

  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        NPCとのポケモン交換：エクセルからのデータ取得待ち
 * @brief       エクセルの読み込み待ち
 * @author      ohhira_syo
 * @date        2015/11/27(金) 11:29:00
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::FieldTradeExcelIsLoaded( AMX * amx, const cell * ptr )
{
  //Alias
  GameSys::GameManager* p_gman = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::fs::AsyncFileManager* pAsyncFileManager = p_gman->GetAsyncFileManager();
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE );

  //データの格納先
  void** ppDataBuf = &p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp;

  // 読み込み完了待ち
  if( !pAsyncFileManager->IsArcFileOpenFinished( ARCID_SCRIPT_EVENT_DATA ) ){
    return false;
  }
  if( !pAsyncFileManager->IsArcFileLoadDataFinished( ppDataBuf ) ){
    return false;
  }

  //アーカイブ閉じる
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
    closeReq.arcId = ARCID_SCRIPT_EVENT_DATA;
    closeReq.heapForReq = p_heap->GetLowerHandle();
    pAsyncFileManager->SyncArcFileClose( closeReq );
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        NPCとのポケモン交換：エクセルからのデータ取得
 * @brief       エクセルの実際の値の取得
 * @author      ohhira_syo
 * @date        2015/11/27(金) 11:29:00
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::FieldTradeExcelGetParam( AMX * amx, const cell * ptr )
{
  //FieldTradeConverter.rbによって構造体と配列風アクセスは自動提供されるのでそれを利用する

  //Param
  u32 data_index  = ptr[1];
  u32 param_index = ptr[2];

  //Alias
  GameSys::GameManager* p_gman = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

  //データの格納先
  void* pDataBuf = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp;

  //指定されたIndexのデータ
  FieldTradeData* pData = &static_cast<FieldTradeData*>(pDataBuf)[data_index];

  //指定されたパラメータ
  return FieldTradeData_GetParam(pData, param_index);
}

//------------------------------------------------------------------------------
/**
 * @func        NPCとのポケモン交換：エクセルからのデータ破棄
 * @brief       読み込んだデータの破棄
 * @author      ohhira_syo
 * @date        2015/11/27(金) 11:29:00
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::FieldTradeExcelEnd( AMX * amx, const cell * ptr )
{
  //Alias
  GameSys::GameManager* p_gman = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

  //データの格納先
  void* pDataBuf = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp;

  //読み込んだデータは不要になるので削除
  GflHeapSafeFreeMemory(pDataBuf);

  //一応NULLに戻しておく
  p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp = NULL;

  return 0;
}


GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )
