//===================================================================
/**
 * @file    GameEventManager.cpp
 * @brief   イベントの制御クラス
 * @author  k.ohno
 * @date    2011.3.6   Syachiから移植
*/
//===================================================================


#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>


#include "System/include/HeapDefine.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameEvent.h"
#include "FieldScript/include/FieldScriptSystem.h"


namespace GameSys{

#if PM_DEBUG
  const u32 GameEventManager::m_scChkHeapTbl[GameEventManager::CHK_HEAP_MAX] = {
    gfl2::heap::HEAPID_SYSTEM,
    HEAPID_RESIDENT,
    HEAPID_RESIDENT_DEVICE,
    HEAPID_APP,
    HEAPID_APP_DEVICE,
    HEAPID_EVENT_DEVICE,
  };

  const char  * GameEventManager::m_scChkHeapNameTbl[GameEventManager::CHK_HEAP_MAX] = {
    "HEAPID_SYSTEM",
    "HEAPID_RESIDENT",
    "HEAPID_RESIDENT_DEVICE",
    "HEAPID_APP",
    "HEAPID_APP_DEVICE",
    "HEAPID_EVENT_DEVICE",
  };

//------------------------------------------------------------------
/**
 * @brief	  今のヒープ情報を保持
 */
//------------------------------------------------------------------
  void GameEventManager::SetChkHeapData( void )    // 
  {
    if( m_ChkHeapDataFlag == false ){
      return ;
    }

    // 一番先頭のイベントのときだけ
    if( mpEvent != NULL ){
      return ;
    }

    // 1〜255までを使用する。
    if( m_ChkHeapDataId < 255 ){
      m_ChkHeapDataId ++;
    }else{
      m_ChkHeapDataId = 1;
    }

    gfl2::heap::HeapBase * p_heap = NULL;
    for( u32 i=0; i<GFL_NELEMS(m_scChkHeapTbl); ++i )
    {
      p_heap = gfl2::heap::Manager::GetHeapByHeapId( m_scChkHeapTbl[i] );
      if(p_heap){
        m_ChkHeapData[i].free         = p_heap->GetTotalFreeSize();
        m_ChkHeapData[i].allocatable  = p_heap->GetTotalAllocatableSize();
      }
      else{
        m_ChkHeapData[i].free = 0;
        m_ChkHeapData[i].allocatable = 0;
      }
    }
  }

//------------------------------------------------------------------
/**
 * @brief	  保持ヒープ情報と今のヒープ情報を比較
 */
//------------------------------------------------------------------
  void GameEventManager::ChkHeapDataFunc( void ) // 
  {
    if( m_ChkHeapDataFlag == false ){
      return ;
    }
    bool is_err = false;

    gfl2::heap::HeapBase * p_heap = NULL;
    for( u32 i=0; i<GFL_NELEMS(m_scChkHeapTbl); ++i )
    {
      u32 free;
      u32 allocatable;
      p_heap = gfl2::heap::Manager::GetHeapByHeapId( m_scChkHeapTbl[i] );

      if(p_heap){
        free         = p_heap->GetTotalFreeSize();
        allocatable  = p_heap->GetTotalAllocatableSize();

        // 減った時にエラーを表示する。
        if( (free < m_ChkHeapData[i].free) || (allocatable < m_ChkHeapData[i].allocatable) )
        {
          GFL_PRINT( "//======================================\n" );
          GFL_PRINT( "// HeapID[%d][%s] EventHeapCheckError\n", m_scChkHeapTbl[i], m_scChkHeapNameTbl[i] );
          GFL_PRINT( "// Before free[0x%x] allocatable[0x%x]\n", m_ChkHeapData[i].free, m_ChkHeapData[i].allocatable );
          GFL_PRINT( "// After free[0x%x] allocatable[0x%x]\n", free, allocatable );
          GFL_PRINT( "//======================================\n" );
          p_heap->Dump();

          // 最後のものを取っておく。
          m_ChkHeapDataErrorHeapID = m_scChkHeapTbl[i];

          is_err = true;
        }
      }
    }

    if( is_err ){
      //---------------------------
      // エラー信号発信
      //---------------------------
      m_ChkHeapDataError  = true;

      if(m_ChkHeapDataAssertFlag){
        if( Field::FieldScript::FieldScriptSystem::DebugHioEnableFlag ){
          if( Field::FieldScript::FieldScriptSystem::DebugRegdentHeapCheckDisableFlag == true ){
            Field::FieldScript::FieldScriptSystem::DebugRegdentHeapCheckDisableFlag = false;
          }else{
            GFL_ASSERT(0);
          }
        }else{
          GFL_ASSERT(0);
        }
      }
    }
  }
#endif // PM_DEBUG

  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  GameEventManager::GameEventManager(GameManager* gmgr)
  {
    mpChangeEvent = NULL;
    mpGameMgr = gmgr;
    mDoEvent = false;
    mpEvent = NULL;

#if PM_DEBUG
    m_ChkHeapDataFlag   = false;
    m_ChkHeapDataAssertFlag = true;
    m_ChkHeapDataError  = false;
    m_ChkHeapDataId = 1;
#endif // PM_DEBUG
  }


//------------------------------------------------------------------
/**
 * @brief	  デストラクタ
 */
//------------------------------------------------------------------

  GameEventManager::~GameEventManager()
  {
  }

  //------------------------------------------------------------------
  /**
   * @brief	イベント切替
   *
   * @param	event	  	  次のイベント制御ワークへのポインタ
   */
  //------------------------------------------------------------------
  void GameEventManager::Change(GameEvent * event)
  {
#ifdef RO_VALID
    if(mpEvent){
      gfl2::ro::Module* pModule = mpEvent->GetRoModule();
      GFL_DELETE mpEvent;
      if( pModule ){
        gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
        roMgr->DisposeModule( pModule );
      }
    }
#else
    if(mpEvent){
      GFL_DELETE mpEvent;
    }
#endif

    mpEvent = event;
  }

  //------------------------------------------------------------------
  /**
   * @brief	イベント起動中チェック
   * @retval	true	イベント起動中
   * @retval	false	イベントなし
   */
  //------------------------------------------------------------------
  bool GameEventManager::isExists(void) const
  {
    if ( mpEvent )
    {
      return true;
    }
    return false;
  }

  //------------------------------------------------------------------
  //------------------------------------------------------------------
  void GameEventManager::ChangeEvent(GameEvent * next_event)
  {
  //	GAMESYS_WORK * gsys = now_event->gsys;

    GFL_ASSERT( mpChangeEvent == NULL );

    // 親が必ず必要。
    if(mpEvent){
      next_event->CopyParentEventPointer( mpEvent );//親の受け渡しを行う
    }else{
      GFL_ASSERT( mpEvent );
    }

    mpChangeEvent = next_event;

    //Change(next_event);
  }

  //------------------------------------------------------------------
  //------------------------------------------------------------------
  void GameEventManager::CallEvent(GameEvent * child)
  {

    child->SetParentEventPointer(mpEvent);
    mpEvent = child;
  }

  //=============================================================================
  //=============================================================================
  //------------------------------------------------------------------
  /**
   * @brief	  イベント制御
   * @retval	GMEVENT_RESULT
   */
  //------------------------------------------------------------------
  GMEVENT_RESULT GameEventManager::Main(void)
  {
    GMEVENT_RESULT result;
    GameEvent * parent;
    
    // このフレーム中、一度でもイベントが呼ばれたかチェックするため
    mDoEvent = this->isExists();

    for(;;) {
      if (mpEvent == NULL) {
        return GMEVENT_RES_OFF;    ///※イベントが動いてない場合はずっとTRUEを返すに変更
      }
      result = mpEvent->UpdateFunc(mpGameMgr);

      switch ( result ) {
      case GMEVENT_RES_CONTINUE:
        return GMEVENT_RES_CONTINUE;
      case GMEVENT_RES_FINISH:
        if(mpChangeEvent == NULL){

          //
          parent = mpEvent->GetParentEventPointer();
          Change( parent );
          if (parent == NULL) 
          {  //動くイベントがない状態
            // 常駐メモリ状態のチェック
  #if PM_DEBUG
            ChkHeapDataFunc();
  #endif // PM_DEBUG
            return GMEVENT_RES_FINISH;
          }
        }else{

          // チェンジリクエストの反映
          Change( mpChangeEvent );
          mpChangeEvent = NULL;
        }
        return GMEVENT_RES_CONTINUE;
      case GMEVENT_RES_CONTINUE_DIRECT:
        /* do nothing */
        break;
      default:
        GFL_ASSERT( 0 );
        return GMEVENT_RES_CONTINUE;
      }
    }

    //return GMEVENT_RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
   * @brief	イベント起動中チェック
   * @retval	true	イベント起動中
   * @retval	false	イベントなし
   */
  //------------------------------------------------------------------
  bool GameEventManager::IsExists(void) const
  {
    if ( mpEvent )
    {
      return true;
    }
    return this->mDoEvent;
  }


  //----------------------------------------------------------------------------
  /**
   *  @brief  イベント用のヒープの取得
   */
  //-----------------------------------------------------------------------------
  gfl2::heap::HeapBase* GameEventManager::GetHeap(void)
  {
    return gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT ); 
  }

} //namespace GameSys








