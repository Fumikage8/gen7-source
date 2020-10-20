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


#include "../../System/include/HeapDefine.h"
#include "GameSystem/include/GameEventManager.h"
#include "GameSystem/include/GameEvent.h""


namespace GameSystem{

#if PM_DEBUG
  const u32 GameEventManager::m_scChkHeapTbl[GameEventManager::CHK_HEAP_MAX] = {
    gfl2::heap::HEAPID_SYSTEM,
    HEAPID_RESIDENT,
    HEAPID_RESIDENT_DEVICE,
    HEAPID_APP,
    HEAPID_APP_DEVICE,
    HEAPID_EVENT,
    HEAPID_EVENT_DEVICE,
  };

  const char  * GameEventManager::m_scChkHeapNameTbl[GameEventManager::CHK_HEAP_MAX] = {
    "HEAPID_SYSTEM",
    "HEAPID_RESIDENT",
    "HEAPID_RESIDENT_DEVICE",
    "HEAPID_APP",
    "HEAPID_APP_DEVICE",
    "HEAPID_EVENT",
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

    // 1～255までを使用する。
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
        //p_heap->SetDebugUserID( m_ChkHeapDataId );todo
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
          p_heap->DumpId( m_ChkHeapDataId );

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

#if PM_DEBUG
    m_ChkHeapDataFlag   = false;
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
      nn::ro::Module* pModule = mpEvent->GetRoModule();
      GFL_DELETE mpEvent;
      if( pModule ){
        gfl::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl::ro::RoManager );
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

    GFL_ASSERT( mpChangeEvent == NULL ); //@check

    // 親が必ず必要。
    if(mpEvent){
      next_event->CopyParentEventPointer( mpEvent );//親の受け渡しを行う
    }else{
      GFL_ASSERT( mpEvent ); //@check
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
    GameEvent * event;
    
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

#if 0 //todo niji 未実装 
#if PM_DEBUG
  /**
   * @brief イベントリストの描画
   */
  void GameEventManager::DEBUG_EventListDraw( gfl::grp::GraphicsSystem *grpSys )
  {
    {
      static const gfl::grp::DisplayType sc_Disp[ 2 ] = {
        gfl::grp::DISPLAY_UPPER,
        gfl::grp::DISPLAY_UPPER_RIGHT
      };
      for( u32 i=0; i<2; ++i ){
        gfl::grp::RenderTarget * renderTarget = grpSys->GetCurrentFrameBuffer(sc_Disp[i]);

        if (grpSys->BindCurrentCommandList(renderTarget)) 
        {
          gfl::grp::util::DrawUtil::BeginRender(renderTarget);
          gfl::grp::util::DrawUtil::SetTextScale(0.7f, 0.7f);
        
          // 階層数を検索
          GameEvent* pDrawEvent = mpEvent;
          s32 list_num = 0;
          while( pDrawEvent )
          {
            pDrawEvent = pDrawEvent->GetParentEventPointer();
            list_num++;
          }

          // 親イベント上に来るように描画していく
          pDrawEvent = mpEvent;
          while( pDrawEvent )
          {
            list_num--;

            bool is_call_same_event = false;
            GameEvent* pListEvent = mpEvent;
            while( pListEvent )
            {
              if( pListEvent != pDrawEvent )
              {
                const std::type_info& list_type_info = typeid(*pListEvent);
                const std::type_info& draw_type_info = typeid(*pDrawEvent);
                if( list_type_info == draw_type_info )
                {
                  is_call_same_event = true;
                  break;
                }
              }
              pListEvent = pListEvent->GetParentEventPointer();
            }

            // 同じイベントが呼ばれている時は色を変える
            if( is_call_same_event )
            {
              gfl::grp::util::DrawUtil::SetTextColor(gfl::math::VEC4(1.0f,0.0f,0.0f,1.0f));
            }else{
              gfl::grp::util::DrawUtil::SetTextColor(gfl::math::VEC4(0.5f,0.8f,1.0f,1.0f));
            }

            const std::type_info& type_info = typeid(*pDrawEvent);
            gfl::grp::util::DrawUtil::DrawText(1+list_num*15, 1 + list_num*15, L"%s\n" ,type_info.name());
            pDrawEvent = pDrawEvent->GetParentEventPointer();
          }

          gfl::grp::util::DrawUtil::EndRender();
        }
      }
    }

  }

#endif // PM_DEBUG
#endif






} //namespace GameSys








