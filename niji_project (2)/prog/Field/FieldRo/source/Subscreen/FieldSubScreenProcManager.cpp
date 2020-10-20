//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		 FieldSubScreenProcManager.cpp
 *	@brief   下画面管理マネージャー
 *	@author  k.ohno
 *	@date		 2015.06.05
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


// niji
#include "GameSys/include/GameManager.h"
#include "proc/include/gfl2_ProcManager.h"

#include "Field\FieldRo\include\Subscreen\FieldSubScreenProcManager.h"

#include "Field\FieldRo\include\Subscreen\FieldSubScreenProc.h"

#include "Field/FieldStatic/include/Subscreen/FieldSubScreen.h"

// 各画面のSubProc
#include "App/FieldMenu/include/FieldMenuSubWindowProc.h"
#include "App/Finder/include/FinderProc.h"
#include "NetApp/JoinFestaSubScreen/include/JoinFestaSubScreenProc.h"


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( SubScreen );


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

/**
 *	@brief  コンストラクタ
 */
FieldSubScreenProcManager::FieldSubScreenProcManager( gfl2::heap::HeapBase* pSys ,gfl2::heap::HeapBase* pDev ) :
m_pManager(NULL),
m_pHeap(pSys),
m_bSubScreenProcEndRequest(false),
m_pRoModule(NULL),
m_isSuspend(false)
{
  m_pDevHeap=pDev;
  mEndSeq = 0;
  m_pFieldMenuProc=NULL;
  m_pFinderProc=NULL;
  m_pJoinFestaSubScreenProc = NULL;
}

/**
 *	@brief  デストラクタ
 */
FieldSubScreenProcManager::~FieldSubScreenProcManager( void )
{
  GFL_ASSERT(m_pManager==NULL);
}


//----------------------------------------------------------------------------
/**
 *	@brief    初期化
 *
 *	@param	p_fieldmap    フィールドマップ
 */
//-----------------------------------------------------------------------------
void FieldSubScreenProcManager::Initialize( Field::SubScreen::FieldSubScreen* pFieldSub)
{
  mEndSeq = 0;
  m_pFieldMenuProc=NULL;
  m_pFinderProc=NULL;
  m_pJoinFestaSubScreenProc = NULL;
  m_bSubScreenProcEndRequest = false;
  m_isSuspend = false;
  m_callWindowId = WIN_NONE; 

  gfl2::heap::HeapBase* p_heap = m_pHeap;

  mpSubScreen = pFieldSub;

  // CALL用のヒープを生成
  m_pHeapCall = GFL_CREATE_LOCAL_HEAP_NAME( p_heap, SUBWINDOW_CALL_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "FieldSubWindowCall" );

  m_pManager = GFL_NEW( p_heap ) gfl2::proc::Manager( p_heap );
}

//----------------------------------------------------------------------------
/**
 *	@brief    破棄
 * @return   破棄できたらtrue
 */
//-----------------------------------------------------------------------------
bool FieldSubScreenProcManager::Finalize( void )
{
  enum {
    SEQ_CALL_WIN_NONE,
    SEQ_CALL_WIN_NONE_WAIT,
    SEQ_END,
  };

  switch(mEndSeq){
  case SEQ_CALL_WIN_NONE:
    DeleteWindowRequest();
    mEndSeq = SEQ_CALL_WIN_NONE_WAIT;
    //break throw
  case SEQ_CALL_WIN_NONE_WAIT:
    if( IsDeleteWindowComplete() )
    {
      GFL_SAFE_DELETE( m_pManager );
      gfl2::heap::Manager::DeleteHeap( m_pHeapCall );
      m_pHeapCall = NULL;
      mEndSeq = SEQ_END;
    }
    break;
  case SEQ_END:
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  一時停止
 */
//-----------------------------------------------------------------------------
void FieldSubScreenProcManager::Suspend( void )
{
  // 現在実行中のProcessに通知
  if( m_pManager ){
    FieldSubScreenProc* p_proc = static_cast<FieldSubScreenProc*>(m_pManager->GetBaseProcess());
    if( p_proc ){
      p_proc->Suspend();
    }
  }
  m_isSuspend = true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  下画面再開
 */
//-----------------------------------------------------------------------------
void FieldSubScreenProcManager::Resume( void )
{
  // Suspendコール後のみ処理する
  if( m_isSuspend == true )
  {
    // 現在実行中のProcessに通知
    if( m_pManager ){
      FieldSubScreenProc* p_proc = static_cast<FieldSubScreenProc*>(m_pManager->GetBaseProcess());
      if( p_proc ){
        p_proc->Resume();
      }
    }
    m_isSuspend = false;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief    更新
 */
//-----------------------------------------------------------------------------
void FieldSubScreenProcManager::Update( void )
{
  if(m_pManager){

    // プロセス動作更新
    gfl2::proc::MainStatus status = m_pManager->Main();

    // プロセス切り替えリクエスト中
    if( m_bSubScreenProcEndRequest == true )
    {
      // 現在のプロセス動作終了検知（無い場合は即時）
      if( status == gfl2::proc::MAINSTATUS_CHANGE || status == gfl2::proc::MAINSTATUS_NULL )
      {
        // DLL読みこんでいた場合は解放
        if( m_pRoModule != NULL )
        {
          gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
          roMgr->DisposeModule( m_pRoModule );
          m_pRoModule = NULL;
        }

        // リクエストのあった下画面をコール
        CreateSubScreenProc();

        m_bSubScreenProcEndRequest = false;
      }
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief    描画
 */
//-----------------------------------------------------------------------------
void FieldSubScreenProcManager::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if(m_pManager){
    m_pManager->Draw(displayNo);
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  下画面呼び出し
 *  @param  [in]windowId  SubWindowId定義
 */
//-----------------------------------------------------------------------------
void FieldSubScreenProcManager::CallWindow( SubWindowId windowId )
{
  GFL_ASSERT( m_bSubScreenProcEndRequest == false ); // 下画面変更中はおかしい。開発中に気づくアサート

  // 既存下画面の終了リクエスト
  if(m_callWindowId == windowId ){
    return;
  }

  m_callWindowId=windowId;
  ICHI_PRINT(">SubScreen CallWindow %d \n", m_callWindowId);
  
  // 今のプロセス終了リクエスト
  SubScreenProcEndReq();
}

//----------------------------------------------------------------------------
/**
 *	@brief  ウィンドウ切り替えの完了まち
 *
 *	@retval true    ウィンドウ切り替え完了
 *	@retval false   ウィンドウ切り替え途中
 */
//-----------------------------------------------------------------------------
bool FieldSubScreenProcManager::IsChangeComplate( void ) const
{
  if( m_bSubScreenProcEndRequest == false )
  {
    switch( m_callWindowId ){
    case WIN_NONE:
      // 下画面なしは何もしなくても切り替え完了
      return true;
    case WIN_MENU_MAIN:
      {
        if( m_pFieldMenuProc )
        {
          return m_pFieldMenuProc->IsCreate();
        }
      }
      break;
    case WIN_JOIN_FESTA:
    case WIN_FINDER:
      {
        // なおかつMainFuncになっているか？
        if( m_pManager->GetBaseProcessInitialized() )
        {
          return true;
        }
      }
      break;
    default:
      GFL_ASSERT(0);
      break;
    }
  }
  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *	@brief  起動中の下画面の破棄リクエスト
 */
/* ----------------------------------------------------------------------------------------- */
void FieldSubScreenProcManager::DeleteWindowRequest()
{
  GFL_ASSERT( m_bSubScreenProcEndRequest == false ); // 下画面変更中はおかしい。開発中に気づくアサート

  if( m_pManager )
  {
    // 下画面なしにすると全て破棄済みになる
    CallWindow( WIN_NONE );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 *	@brief  起動中の下画面が破棄完了かどうか
 *  @return trueで破棄完了
 */
/* ----------------------------------------------------------------------------------------- */
bool FieldSubScreenProcManager::IsDeleteWindowComplete()
{
  if( m_pManager )
  {
    this->Update();
    return IsChangeComplate();
  }
  return true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  起動中の下画面種類のID 取得
 *  @return subWindowId  SubWindowId定義
 */
//-----------------------------------------------------------------------------
SubWindowId FieldSubScreenProcManager::GetSubWindowId()
{
  // 現在実行中の下画面IDを取得
  if( m_pManager ){
    FieldSubScreenProc* p_proc = static_cast<FieldSubScreenProc*>(m_pManager->GetBaseProcess());
    if( p_proc ){
      return p_proc->GetSubWindowId();
    }
  }

  return WIN_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  下画面イベントリクエストチェック
 *  @retval true:リクエスト有り
 *  @retval false:リクエスト無し
 */
//-----------------------------------------------------------------------------
bool FieldSubScreenProcManager::IsEventRequest() const
{
  if( GetEventRequestID() != OUT_REQ_ID_NONE )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 *  @brief 下画面イベントリクエスト取得
 *  @return リクエストID
 */
//-----------------------------------------------------------------------------
FieldMenuOutRequestID FieldSubScreenProcManager::GetEventRequestID() const
{
  FieldSubScreenProc* p_proc = static_cast<FieldSubScreenProc*>(m_pManager->GetBaseProcess());
  if( p_proc )
  {
    return p_proc->GetEventRequestID();
  }
  return OUT_REQ_ID_NONE;
}


//-----------------------------------------------------------------------------
/**
 * @func    RemakeFrame
 * @brief   フレーム再生成リクエスト
 * @date    2015.09.10
 *
 * @retval  true  = リクエスト成功
 * @retval  false = リクエスト失敗
 *
   */
//-----------------------------------------------------------------------------
bool FieldSubScreenProcManager::RemakeFrame(void)
{
  if(m_pFieldMenuProc){
    return m_pFieldMenuProc->RemakeFrame();
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsFrameRemake
 * @brief   フレーム再生成中か？
 * @date    2016.05.17
 *
 * @retval  true  = 再生成中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldSubScreenProcManager::IsFieldMenuFrameRemake(void)
{
  if( m_pFieldMenuProc )
  {
    return m_pFieldMenuProc->IsFrameRemake();
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsFieldMenuCreate
 * @brief   フィールドメニューの生成が終了したか
 * @date    2015.11.04
 *
 * @retval  true  = した
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldSubScreenProcManager::IsFieldMenuCreate(void)
{
  if( m_pFieldMenuProc )
  {
    return m_pFieldMenuProc->IsCreate();
  }
  return false;
}


//-----------------------------------------------------------------------------
/**
 * @func    SetRotomEvent
 * @brief   ロトムメッセージ表示イベントを開始
 * @date    2015.11.11
 *
 * @param   msg_id  メッセージID
 *
 * @retval  true  = 成功
 * @retval  false = 失敗
 */
//-----------------------------------------------------------------------------
bool FieldSubScreenProcManager::SetRotomEvent( u32 msg_id )
{
  if( m_pFieldMenuProc )
  {
    return m_pFieldMenuProc->SetRotomEvent( msg_id );
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsRotomEvent
 * @brief   ロトムメッセージ表示イベント中か
 * @date    2015.11.11
 *
 * @retval  true  = イベント中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool FieldSubScreenProcManager::IsRotomEvent(void)
{
  if( m_pFieldMenuProc )
  {
    return m_pFieldMenuProc->IsRotomEvent();
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomPosEvent
 * @brief   ロトムメッセージ表示リクエスト（POSイベント用）
 * @date    2016.01.15
 *
 * @param   msg_id  メッセージID
 */
//-----------------------------------------------------------------------------
void FieldSubScreenProcManager::SetRotomPosEvent( u32 msg_id )
{
  if( m_pFieldMenuProc )
  {
    m_pFieldMenuProc->SetRotomPosEvent( msg_id );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsCanCreateFieldStaffList
 * @brief   FieldStaffListクラスを生成可能か
 * @date    2017.04.05
 *
 * @retval  true  = 可
 * @retval  false = 不可
 */
//-----------------------------------------------------------------------------
bool FieldSubScreenProcManager::IsCanCreateFieldStaffList(void) const
{
  if( m_pFieldMenuProc )
  {
    return m_pFieldMenuProc->IsMenuDrawCreate();
  }
  return true;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   下画面プロセス作成
 */
/* ----------------------------------------------------------------------------------------- */
void FieldSubScreenProcManager::CreateSubScreenProc()
{
  switch( m_callWindowId ){
  case WIN_NONE:
    // 下画面なし
    break;
  case WIN_MENU_MAIN:
    {
      // DLL読み込み
      {
        GFL_ASSERT( m_pRoModule==NULL );
        gfl2::fs::AsyncFileManager* pAsyncFileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
        gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
        m_pRoModule = roMgr->LoadModule(pAsyncFileManager, "FieldMenu.cro");
        roMgr->StartModule(m_pRoModule, true);
      }

      m_pFieldMenuProc = CallSubWindow< App::FieldMenu::FieldMenuSubWindowProc >();
      if(mpSubScreen){
        mpSubScreen->SetFieldMenuProc(m_pFieldMenuProc);
      }
    }
    break;
  case WIN_JOIN_FESTA:
    {
      // DLL読み込み
      {
        GFL_ASSERT( m_pRoModule==NULL );
        gfl2::fs::AsyncFileManager* pAsyncFileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
        gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
        m_pRoModule = roMgr->LoadModule(pAsyncFileManager, "JoinFestaSubScreen.cro");
        roMgr->StartModule(m_pRoModule, true);
      }

      m_pJoinFestaSubScreenProc = CallSubWindow< NetApp::JoinFestaSubScreen::JoinFestaSubScreenProc >();
    }
    break;
  case WIN_FINDER:
    {
      m_pFinderProc = CallSubWindow< App::Finder::FinderProc >();
    }
    break;
  default:
    GFL_ASSERT(0);
    break;
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 *    今のPROCの終了リクエスト
 */
/* ----------------------------------------------------------------------------------------- */
void FieldSubScreenProcManager::SubScreenProcEndReq(void)
{
  if( m_pManager ){
    FieldSubScreenProc* p_proc = static_cast<FieldSubScreenProc*>(m_pManager->GetBaseProcess());
    if( p_proc ){
      p_proc->ReqEnd();
      m_pFieldMenuProc=NULL;
      m_pFinderProc=NULL;
      m_pJoinFestaSubScreenProc = NULL;
    }
    m_bSubScreenProcEndRequest = true;
  }
}


GFL_NAMESPACE_END( SubScreen );
GFL_NAMESPACE_END( Field );
