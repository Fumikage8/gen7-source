//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   EventSymbolEncount.cpp
 *  @brief  シンボルエンカウトイベント
 *  @author miyuki iwasawa 
 *  @date   2015.09.03
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <str/include/gfl2_StrBuf.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// pml
#include <pml/include/pmlib.h>

#include "Savedata/include/MyStatus.h"
#include "Savedata/include/ZukanSave.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "System/include/HeapDefine.h"

#include "Field/FieldRo/include/Fieldmap.h"


#include "Field/FieldStatic/include/Event/EventSymbolEncount.h"
#include "Field/FieldStatic/include/Event/EventProcessCall.h"

#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"
#include "Field/FieldStatic/include/Event/EventBattleCall.h"
#include "Field/FieldStatic/include/Encount/FieldEncountWork.h"
#include "Field/FieldStatic/include/Encount/FieldEncountPokeSet.h"

#include "FieldScript/include/EventScriptCall.h" // for CallScript
#include "FieldScript/include/FieldScriptSystem.h"

#include "App/GameOver/include/GameOverProc.h"
#include "App/AppEvent/include/GameOver/GameOverEvent.h"

#include "App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/script_event_data.gaix>

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Encount )
  
//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          実装
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *    イベントの実装
 */
//-----------------------------------------------------------------------------
//--------------------------------------------------------------
/**
 * @brief   パラメータ通知 
 * @param   p_gman       ゲームマネージャークラス
 * @retval  true  起動してよい
 * @retval  false 起動しない
 */
//--------------------------------------------------------------
void EventSymbolEncount::SetParam( PokeSet* p_pokeset, u32 symbol_id, u32 call_option /* = EVBTL_CALLOPT_NON*/, u32 ret_bgm_id /*=xy_snd::SOUND_ITEM_ID_NONE*/)
{
  m_SymbolID = symbol_id;
  m_pPokeSet = p_pokeset;
  m_CallOption = call_option;
  m_RetBGMID = ret_bgm_id;

  return;
} 


//--------------------------------------------------------------
/**
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 * @retval  true  起動してよい
 * @retval  false 起動しない
 */
//--------------------------------------------------------------
bool EventSymbolEncount::BootChk(GameSys::GameManager* /*p_gman*/)
{
  return true;
} 

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventSymbolEncount::InitFunc(GameSys::GameManager* p_gman)
{
  m_pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  this->Initialize( p_gman );
} 

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 * @return  GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventSymbolEncount::MainFunc(GameSys::GameManager* p_gman)
{
  enum{
    SEQ_INITALIZE_WAIT,
    SEQ_END,
  };
  switch( m_Seq ){
  case SEQ_INITALIZE_WAIT:
    if( !this->InitializeWait(p_gman) ) break;

    CallBattle(p_gman);
    m_Seq++;
    break;
  case SEQ_END:
    return GameSys::GMEVENT_RES_FINISH;
  }
  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventSymbolEncount::EndFunc(GameSys::GameManager* p_gman)
{
  GFL_SAFE_DELETE(m_pPokeParam);
}

//--------------------------------------------------------------
/**
 * @brief データ初期化 
 * @param p_gman  ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventSymbolEncount::Initialize(GameSys::GameManager* p_gman)
{
  gfl2::fs::AsyncFileManager* pAsyncFileManager = p_gman->GetAsyncFileManager();

  m_InitSeq = 0;

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
    loadReq.datId = GARC_script_event_data_symbol_encount_BIN;
    loadReq.ppBuf = &m_pBuffer;
    loadReq.heapForBuf = m_pHeap;
    loadReq.heapForReq = m_pHeap->GetLowerHandle();
    loadReq.heapForCompressed = NULL;
    loadReq.pBufSize = &m_BufferSize;
    loadReq.align = 4;
    pAsyncFileManager->AddArcFileLoadDataReq( loadReq );  
  }
  
}

//--------------------------------------------------------------
/**
 * @brief データ初期化 
 * @param p_gman  ゲームマネージャークラス
 */
//--------------------------------------------------------------
bool EventSymbolEncount::InitializeWait(GameSys::GameManager* p_gman)
{
  gfl2::fs::AsyncFileManager* pAsyncFileManager = p_gman->GetAsyncFileManager();
  
  switch( m_InitSeq ){
  case 0:
    // 読み込み完了待ち
    if( !pAsyncFileManager->IsArcFileOpenFinished( ARCID_SCRIPT_EVENT_DATA ) ){
      return false;
    }
    if( !pAsyncFileManager->IsArcFileLoadDataFinished( &m_pBuffer ) ){
      return false;
    }

    //データ格納
    {
      SymbolEncountParam* pParam = reinterpret_cast<SymbolEncountParam *>(m_pBuffer);
      SymbolEncountParam* pTarget = NULL;

      u32 param_num = m_BufferSize/sizeof(SymbolEncountParam);
      if( m_SymbolID >= param_num ){
        GFL_ASSERT_MSG(0,"%d>=%d",m_SymbolID,param_num);
        m_SymbolID = 0;
      }
      pTarget = &pParam[m_SymbolID];
      m_pPokeSet->SetSymbolEncountData(pTarget);
      GFL_ASSERT_STOP((pTarget->monsNo != MONSNO_NULL));
      // @fix MMCat[497] UB世界のシングルエンカウントのポケモン抽選に関して >> 必ずレアになるフラグをリセットする場所をここへ移動。ＵＢのシンボルじゃなくてもリセットして大丈夫
      {
        GameSys::GameData* p_gdata = p_gman->GetGameData();
        Savedata::Misc* p_misc = p_gdata->GetMisc();
        //抽選終了したら、誤動作防止のためレアトリガーをリセット
        p_misc->ResetMustRareEncTrigger();
      }

      //援軍データ格納
      for( int i=0; i<pTarget->reinforce_num;i++){
        SymbolReinforeParam* srp = &pTarget->reinforce[i];
        m_pPokeSet->SetReinforcePokeParam( &pParam[srp->index-1], static_cast<ReinforcePokeType>(srp->type), srp->count ); //indexは0が無効で1から
      }
    }

    //バッファは開放
    GflHeapSafeFreeMemory(m_pBuffer);  

    //アーカイブ閉じる
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId = ARCID_SCRIPT_EVENT_DATA;
      closeReq.heapForReq = m_pHeap->GetLowerHandle();
      pAsyncFileManager->AddArcFileCloseReq( closeReq );
    }
    m_InitSeq++;
    return false;

  case 1:
    if( !pAsyncFileManager->IsArcFileCloseFinished( ARCID_SCRIPT_EVENT_DATA ) ){
      return false;
    }
    m_InitSeq++;
    break;

  default:
    break;
  }

  
  return true;
}

//--------------------------------------------------------------
/**
 * @brief 図鑑登録アプリコール
 * @param p_gman  ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventSymbolEncount::CallBattle(GameSys::GameManager* p_gman)
{
  GameSys::GameEventManager* p_eventman = p_gman->GetGameEventManager();
  EventBattleCall::CallWildEx( p_eventman, p_gman, *m_pPokeSet, m_CallOption, m_RetBGMID );
}

GFL_NAMESPACE_END( Encount )
GFL_NAMESPACE_END( Field )


