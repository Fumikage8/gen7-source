#if PM_DEBUG
//=============================================================================
/**
 * @file    LauncherProc.cpp
 * @brief   デバッグ用ランチャー処理
 * @author  Hiroyuki Nakamura
 * @date    2015.02.05
 */
//=============================================================================
#include <Debug/Launcher/include/LauncherProc.h>
#include <Debug/Launcher/include/LauncherListData.h>

#include <thread/include/gfl2_ThreadStatic.h>

#include <util/include/gfl2_std_string.h>

#include <GameSys/include/GameProcManager.h>
#include <Fade/include/gfl2_FadeManager.h>

#include <System/include/HeapDefine.h>
#include "LauncherSub.h"

//#include <debug/include/gfl2_DebugwinTypes.h>
//#include <debug/include/gfl2_DebugWinItem.h>
#include "Debug/DebugWin/include/DebugWinSystem.h"


GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(launcher)

//=============================================================================
//  定数定義
//=============================================================================
static const u32 SYSTEM_HEAP_SIZE = 0x1000;         // システムヒープサイズ
static const u32 APP_HEAP_SIZE = 0x50000;           // アプリ用ローカルヒープサイズ
static const u32 APP_DEVICE_HEAP_SIZE = 0x15000;   // アプリ用ローカルデバイスヒープサイズ

#if 0
0x3FEBC
0x65F4
#endif

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   prm   セットアップパラメータ
 */
//-----------------------------------------------------------------------------
LauncherProc::LauncherProc( CallParam * prm )
  : m_pSysHeap( NULL )
  , m_createSysHeap( false )
  , m_pListData( NULL )
  , m_pTmpListData( NULL )
  , m_isListCreateLow( false )
  , m_pHeap( NULL )
  , m_pDevHeap( NULL )
  , m_pHeapAllocator( NULL )
  , m_pDevHeapAllocator( NULL )
  , m_pLauncherSub( NULL )
  , m_page( 0 )
  , m_pos( 0 )
  , m_pCallParam( prm )
{
  gfl2::std::MemCopy( prm, &m_CallParamOrg ,sizeof(CallParam) );

  m_pCallParam = &m_CallParamOrg;

  CreateSystemHeap();
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
LauncherProc::~LauncherProc()
{
  DeleteSystemHeap();
}

//-----------------------------------------------------------------------------
/**
 * @func    InitCallParam
 * @brief   外部設定パラメータを初期化
 * @date    2015.02.05
 *
 * @param   prm   外部設定パラメータ
 */
//-----------------------------------------------------------------------------
void LauncherProc::InitCallParam( CallParam * prm )
{
  gfl2::std::MemClear( prm, sizeof(CallParam) );

  prm->heapID    = HEAPID_NULL;
  prm->devHeapID = HEAPID_NULL;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   LauncherProc初期化処理
 * @date    2015.02.05
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result LauncherProc::InitFunc( gfl2::proc::Manager * /*pManager*/ )
{
//  CreateSystemHeap();
  CreateCore();
	return gfl2::proc::RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   LauncherProc終了処理
 * @date    2015.02.05
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result LauncherProc::EndFunc( gfl2::proc::Manager * /*pManager*/ )
{
  if( m_pLauncherSub->IsLayoutDrawing() != false )
  {
    return gfl2::proc::RES_CONTINUE;
  }
  ReleaseCore();
//  DeleteSystemHeap();
	return gfl2::proc::RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateFunc
 * @brief   LauncherProcアップデート処理
 * @date    2015.02.05
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result LauncherProc::UpdateFunc( gfl2::proc::Manager * /*pManager*/ )
{
  if( m_pLauncherSub != NULL )
  {
    m_pLauncherSub->UpdateLayout();
  }

  switch( GetSubSeq() ){
  case SEQID_SELECT:
    if( SequenceSelect() != false )
    {
      return gfl2::proc::RES_CONTINUE;
    }
    break;

  case SEQID_SELECT_WAIT:
    if( SequenceSelectWait() != false )
    {
      return gfl2::proc::RES_CONTINUE;
    }
    break;

  case SEQID_SCROLL_MAIN:
    SequenceScrollMain();
    return gfl2::proc::RES_CONTINUE;

  case SEQID_SCROLL_END:
    SequenceScrollEnd();
    return gfl2::proc::RES_CONTINUE;

  case SEQID_WAIT:
    // Process自分自身　＆　Eventも無い状態
    if( !this->GetGameManager()->GetGameEventManager()->IsExists() )
    {
      CreateCore();
      SetSubSeq( SEQID_SELECT );
    }
    return gfl2::proc::RES_CONTINUE;

	default:
		GFL_ASSERT( 0 );
		break;
	}

  return gfl2::proc::RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 * @func    PreDrawFunc
 * @brief   LauncherProc描画処理(下準備)
 * @date    2015.02.19
 *
 * @param   pManager    プロセス管理マネージャー
 */
//-----------------------------------------------------------------------------
void LauncherProc::PreDrawFunc( gfl2::proc::Manager * /*pManager*/ )
{
  if( GetSubSeq() != SEQID_SELECT_WAIT && m_pLauncherSub != NULL )
  {
    m_pLauncherSub->PreDraw();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    DrawFunc
 * @brief   LauncherProc描画処理
 * @date    2015.02.05
 *
 * @param   pManager    プロセス管理マネージャー
 * @param   displayNo   描画対象画面
 */
//-----------------------------------------------------------------------------
void LauncherProc::DrawFunc( gfl2::proc::Manager * pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  if( GetSubSeq() != SEQID_SELECT_WAIT && m_pLauncherSub != NULL )
  {
    m_pLauncherSub->DrawLayout( displayNo );
  }
}

//------------------------------------------------------------------
/**
  * @brief   PROCのイベント起動検知処理
  *
  *  以下のタイミングで行うべき、イベント起動処理
  *  　GameProc->EventCheckFunc
  *  　EventManager->Main
  *  　GameProc->Main
  */
//------------------------------------------------------------------
void LauncherProc::EventCheckFunc(GameSys::GameManager* pManager)
{
  gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );

  pFadeManager->Reset( gfl2::Fade::DISP_UPPER );
  pFadeManager->Reset( gfl2::Fade::DISP_LOWER );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateSystemHeap
 * @brief   システムヒープを作成
 * @date    2015.02.18
 */
//-----------------------------------------------------------------------------
void LauncherProc::CreateSystemHeap(void)
{
  if( gfl2::heap::Manager::IsExistHeapId( HEAPID_DEBUG_LAUNCHER_SYSTEM ) == false )
  {
    m_pSysHeap = GFL_CREATE_HEAP(
                  gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG)->GetLowerHandle(),
                  HEAPID_DEBUG_LAUNCHER_SYSTEM,
                  SYSTEM_HEAP_SIZE,
                  gfl2::heap::HEAP_TYPE_EXP,
                  false );
    GFL_ASSERT_STOP( m_pSysHeap );
    m_createSysHeap = true;
  }
  else
  {
    m_pSysHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG_LAUNCHER_SYSTEM);
    m_createSysHeap = false;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteSystemHeap
 * @brief   システムヒープを削除
 * @date    2015.02.18
 */
//-----------------------------------------------------------------------------
void LauncherProc::DeleteSystemHeap(void)
{
  DeleteListData();
  if( m_createSysHeap == true )
  {
    GFL_DELETE_HEAP( m_pSysHeap );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteListData
 * @brief   リストデータを削除
 * @date    2015.02.18
 */
//-----------------------------------------------------------------------------
void LauncherProc::DeleteListData(void)
{
  GFL_SAFE_DELETE( m_pListData );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateCore
 * @brief   コアデータ生成
 * @date    2015.02.05
 */
//-----------------------------------------------------------------------------
void LauncherProc::CreateCore(void)
{
  CreateHeap();

  m_pLauncherSub = GFL_NEW( m_pHeap ) LauncherSub( m_pHeap, m_pDevHeap, m_pHeapAllocator, m_pDevHeapAllocator );
  m_pLauncherSub->CreateGraphicsSystem();
  m_pLauncherSub->CreateLayout();
  m_pLauncherSub->CreateStrBuf();
  m_pLauncherSub->CreateMessage( m_pListData->GetArcDataID(), m_pListData->GetListTable(), m_pListData->GetListTableSize() );

  m_pLauncherSub->InitPrintItem();

  m_pLauncherSub->SetCursorAnime( m_pos, true );

  m_pLauncherSub->SetupUI();
  m_pLauncherSub->SetEventListner( this );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateCore
 * @brief   コアデータ削除
 * @date    2015.02.05
 */
//-----------------------------------------------------------------------------
void LauncherProc::ReleaseCore(void)
{
  m_pLauncherSub->DeleteMessage();
  m_pLauncherSub->DeleteStrBuf();
  m_pLauncherSub->DeleteLayout();
  m_pLauncherSub->DeleteGraphicsSystem();
  GFL_SAFE_DELETE( m_pLauncherSub );

  DeleteHeap();
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateHeap
 * @brief   ヒープ作成
 * @date    2015.02.05
 */
//-----------------------------------------------------------------------------
void LauncherProc::CreateHeap(void)
{
  if( m_pCallParam->heap != NULL )
  {
    m_pHeap = m_pCallParam->heap;
  }
  else
  {
    gfl2::heap::HEAPID heapID = m_pCallParam->heapID;
    if( heapID == HEAPID_NULL )
    {
      heapID = HEAPID_DEBUG_LAUNCHER;
    }
    m_pHeap = GFL_CREATE_HEAP(
                gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG ),
                heapID,
                APP_HEAP_SIZE,
                gfl2::heap::HEAP_TYPE_EXP,
                false );
  }

  if( m_pCallParam->devHeap != NULL )
  {
    m_pDevHeap = m_pCallParam->devHeap;
  }
  else
  {
    gfl2::heap::HEAPID devHeapID = m_pCallParam->devHeapID;
    if( devHeapID == HEAPID_NULL )
    {
      devHeapID = HEAPID_DEBUG_LAUNCHER_DEVICE;
    }
    m_pDevHeap = GFL_CREATE_HEAP(
                  gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG ),
                  devHeapID,
                  APP_DEVICE_HEAP_SIZE,
                  gfl2::heap::HEAP_TYPE_EXP,
                  false );
  }

  if( m_pCallParam->allocator != NULL )
  {
    m_pHeapAllocator = m_pCallParam->allocator;
  }
  else
  {
    m_pHeapAllocator = GFL_NEW( m_pHeap ) gfl2::heap::NwAllocator( m_pHeap );
  }

  if( m_pCallParam->devAllocator != NULL )
  {
    m_pDevHeapAllocator = m_pCallParam->devAllocator;
  }
  else
  {
    m_pDevHeapAllocator = GFL_NEW( m_pDevHeap ) gfl2::heap::NwAllocator( m_pDevHeap );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteHeap
 * @brief   ヒープ削除
 * @date    2015.02.05
 */
//-----------------------------------------------------------------------------
void LauncherProc::DeleteHeap(void)
{
  if( m_pCallParam->devAllocator == NULL )
  {
    GFL_SAFE_DELETE( m_pDevHeapAllocator );
  }
  if( m_pCallParam->allocator == NULL )
  {
    GFL_SAFE_DELETE( m_pHeapAllocator );
  }
  if( m_pCallParam->devHeap == NULL && m_pCallParam->devHeapID == HEAPID_NULL )
  {
    GFL_DELETE_HEAP( m_pDevHeap );
  }
  if( m_pCallParam->heap == NULL && m_pCallParam->heapID == HEAPID_NULL )
  {
    GFL_DELETE_HEAP( m_pHeap );
  }
}


//-----------------------------------------------------------------------------
/**
 * @func    SeaquenceSelect
 * @brief   シーケンス：項目選択
 * @date    2015.02.05
 *
 * @retval  true  = 継続
 * @retval  false = 終了へ
 */
//-----------------------------------------------------------------------------
bool LauncherProc::SequenceSelect(void)
{

#if GFL_DEBUGWIN_USE
  //このデバッグウインドウが動いている時はUIViewを停止
  if( gfl2::debug::DebugWin_IsOpen()  ){
    return true;
  }
#endif


  // キャンセル
  if( m_pLauncherSub->IsButtonTrigger( gfl2::ui::BUTTON_B ) != false )
  {
//    return SelectPos( CURSOR_POS_CANCEL );
    m_selectPos = CURSOR_POS_CANCEL;
    SetSubSeq( SEQID_SELECT_WAIT );
    return true;
  }

  // 決定
  if( m_pLauncherSub->IsButtonTrigger( gfl2::ui::BUTTON_A ) != false )
  {
//    return SelectPos( m_pLauncherSub->CalcCursorPos(m_page,m_pos) );
    m_selectPos = m_pLauncherSub->CalcCursorPos(m_page,m_pos);
    SetSubSeq( SEQID_SELECT_WAIT );
    return true;
  }

  // カーソル移動
  if( m_pLauncherSub->MoveCursor( &m_page, &m_pos, m_pListData->GetListTableSize() ) != false )
  {
    SetSubSeq( SEQID_SCROLL_MAIN );
  }


#if defined( DEBUG_ONLY_FOR_obata_toshihiro )
  // 小幡の環境では、Lボタンが押されたらデバッグバトルを開始する
  if( m_pLauncherSub->IsButtonTrigger( gfl2::ui::BUTTON_L ) != false )
  {
    m_selectPos = 3;
    SetSubSeq( SEQID_SELECT_WAIT );
    return true;
  }
#endif

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    SeaquenceSelectWait
 * @brief   シーケンス：項目選択（１フレ遅れ解放対策）
 * @date    2015.08.21
 *
 * @retval  true  = 継続
 * @retval  false = 終了へ
 */
//-----------------------------------------------------------------------------
bool LauncherProc::SequenceSelectWait(void)
{
#if GFL_DEBUGWIN_USE
  //このデバッグウインドウが動いている時はUIViewを停止
  if( gfl2::debug::DebugWin_IsOpen()  ){
    return true;
  }
#endif

  if( m_pLauncherSub->IsLayoutDrawing() != false )
  {
    return true;
  }

  bool ret = SelectPos( m_selectPos );

  if( GetSubSeq() == SEQID_SELECT_WAIT )
  {
    SetSubSeq( SEQID_SELECT );
  }

  return ret;
}

//-----------------------------------------------------------------------------
/**
 * @func    SeaquenceScrollMain
 * @brief   シーケンス：ページスクロール処理
 * @date    2015.02.05
 */
//-----------------------------------------------------------------------------
void LauncherProc::SequenceScrollMain(void)
{
  if( m_pLauncherSub->IsEndScrollAnime() != false )
  {
    SetSubSeq( SEQID_SCROLL_END );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SeaquenceScrollEnd
 * @brief   シーケンス：ページスクロール終了処理
 * @date    2015.02.05
 */
//-----------------------------------------------------------------------------
void LauncherProc::SequenceScrollEnd(void)
{
  m_pLauncherSub->ResetListPos( m_pos );
  SetSubSeq( SEQID_SELECT );
}

//-----------------------------------------------------------------------------
/**
 * @func    SelectPos
 * @brief   項目決定処理
 * @date    2015.02.05
 *
 * @param   pos   項目位置 ( キャンセル = CURSOR_POS_CANCEL )
 *
 * @retval  true  = 継続
 * @retval  false = 終了へ
 */
//-----------------------------------------------------------------------------
bool LauncherProc::SelectPos( u32 pos )
{
  if( m_pListData != NULL )
  {
    ListData::RequestID req = m_pListData->SelectListener( pos, this );
    if( req == ListData::REQID_CALL )
    {
      ReleaseCore();
      SetSubSeq( SEQID_WAIT );
    }
    else if( req == ListData::REQID_RESET )
    {
      // リストデータ変更
      GFL_ASSERT_STOP( m_pTmpListData );
      DeleteListData();
      m_pListData    = m_pTmpListData;
      m_pTmpListData = NULL;
      // リスト表示更新
      m_pLauncherSub->SetCursorAnime( m_pos, false );
      m_pLauncherSub->DeleteMessage();
      m_pLauncherSub->CreateMessage( m_pListData->GetArcDataID(), m_pListData->GetListTable(), m_pListData->GetListTableSize() );
      m_pLauncherSub->InitPrintItem();
      m_pos  = 0;
      m_page = 0;
      m_pLauncherSub->SetCursorAnime( m_pos, true );
    }
    else if( req == ListData::REQID_FINISH )
    {
      m_pCallParam->cursorPos = pos;
      return false;
    }
  }
  else if( pos == CURSOR_POS_CANCEL )
  {
    m_pCallParam->cursorPos = pos;
    return false;
  }

  return true;
}


//-----------------------------------------------------------------------------
/**
* @func    OnPrintString
* @brief   SubのOnPrintString と ListData をブリッジ
*/
//-----------------------------------------------------------------------------
void LauncherProc::OnPrintString( u32 pos, gfl2::str::StrBuf * strBuf )
{
  if( m_pListData )
  {
    m_pListData->OnPrintString(pos,strBuf);
  }
}


GFL_NAMESPACE_END(launcher)
GFL_NAMESPACE_END(debug)

#endif  // PM_DEBUG
