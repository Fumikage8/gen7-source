#if PM_DEBUG
//=============================================================================
/**
 * @file    ItemMakeProc.cpp
 * @brief   デバッグ用アイテム作成処理
 * @author  Hiroyuki Nakamura
 * @date    2015.05.26
 */
//=============================================================================

// module
#include "Debug/DebugItemMake/include/ItemMakeProc.h"
#include "ItemMakeFrame.h"
// niji
#include "System/include/HeapDefine.h"
#include "GameSys/include/DllProc.h"
#include "AppLib/include/Frame/AppFrameManager.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include "Print/include/WordSet.h"
#include "Debug/DebugKeyboard/include/DebugKeyboard.h"
// resource
#include "arc_index/message.gaix"
#include "arc_index/debug_message.gaix"
#include "niji_conv_header/message/debug/msg_debug_item_make.h"


GFL_NAMESPACE_BEGIN(Debug)

static const u32 MANAGER_HEAP_SIZE = 0x1000;          //!< マネージャヒープサイズ
static const u32 LOCAL_HEAP_SIZE        = 0x200000;   //!< ローカルヒープサイズ
static const u32 LOCAL_DEVICE_HEAP_SIZE = 0x500000;   //!< ローカルデバイスヒープサイズ

//! メインシーケンス
enum
{
  SEQ_FRAME_INIT = 0,   //!< フレーム初期化
  SEQ_FRAME_CREATE,     //!< フレーム生成
  SEQ_FRAME_MAIN,       //!< フレームメイン
  SEQ_FRAME_END,        //!< フレーム終了

  SEQ_KEYBORD_INIT,     //!< キーボード初期化
  SEQ_KEYBORD_MAIN,     //!< キーボードメイン
  SEQ_KEYBORD_END,      //!< キーボード終了
};


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
ItemMakeProc::ItemMakeProc(void)
  : m_itemID( ITEM_MASUTAABOORU )
  , m_itemNum( 1 )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
ItemMakeProc::~ItemMakeProc()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   Proc初期化処理
 * @date    2015.05.26
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result ItemMakeProc::InitFunc( gfl2::proc::Manager * pManager )
{
  gfl2::heap::HeapBase * heap     = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
	gfl2::heap::HeapBase * dev_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  // マネージャーヒープ生成
  m_pManagerHeap = GFL_CREATE_LOCAL_HEAP( dev_heap, MANAGER_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
  // ローカルヒープ生成
  m_heap.LocalHeapCreate( heap, dev_heap, LOCAL_HEAP_SIZE, LOCAL_DEVICE_HEAP_SIZE );

  // 道具名メッセージデータ作成
  // @note itemname_wordsetだと検索できない
//  m_pItemName = GFL_NEW(m_heap.GetDeviceHeap()) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_itemname_wordset_DAT, m_heap.GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );
  m_pItemName = GFL_NEW(m_heap.GetDeviceHeap()) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_itemname_DAT, m_heap.GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );

	return gfl2::proc::RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   Proc終了処理
 * @date    2015.05.26
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result ItemMakeProc::EndFunc( gfl2::proc::Manager * pManager )
{
  // 道具名メッセージデータ削除
  GFL_SAFE_DELETE( m_pItemName );
  // ローカルヒープ削除
  m_heap.Delete();
  // マネージャヒープ削除
  GFL_DELETE_HEAP( m_pManagerHeap );

	return gfl2::proc::RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateFunc
 * @brief   Procアップデート処理
 * @date    2015.05.26
 *
 * @param   pManager    プロセス管理マネージャー
 *
 * @return  gfl2::proc::Result
 */
//-----------------------------------------------------------------------------
gfl2::proc::Result ItemMakeProc::UpdateFunc( gfl2::proc::Manager * pManager )
{
  switch( GetSubSeq() )
  {
  case SEQ_FRAME_INIT:
    CreateRenderManager();
    SetSubSeq( SEQ_FRAME_CREATE );
    break;
  case SEQ_FRAME_CREATE:
    m_pRenderingManager->Update();
    if( m_pRenderingManager->IsCreated() == false )
    {
      break;
    }
    CreateMainFrame();
    SetSubSeq( SEQ_FRAME_MAIN );
    break;
  case SEQ_FRAME_MAIN:
    if( m_pFrameManager->Main() == applib::frame::MAINSTATUS_NULL )
    {
      SetSubSeq( SEQ_FRAME_END );
    }
    m_pRenderingManager->Update();
    break;
  case SEQ_FRAME_END:
    if( DeleteMainFrame() == false )
    {
      break;
    }
    if( DeleteRenderManager() == false )
    {
      break;
    }
    // キーボード
    if( m_exitID == ItemMakeFrame::EXIT_CALL_KEYBORD )
    {
      SetSubSeq( SEQ_KEYBORD_INIT );
    }
    // 終了へ
    else
    {
      return gfl2::proc::RES_FINISH;
    }
    break;

  case SEQ_KEYBORD_INIT:
    CreateKeybord();
    SetSubSeq( SEQ_KEYBORD_MAIN );
    break;
  case SEQ_KEYBORD_MAIN:
    SetSubSeq( SEQ_KEYBORD_END );
    break;
  case SEQ_KEYBORD_END:
    DeleteKeybord();
    SetSubSeq( SEQ_FRAME_INIT );
    break;
  }

	return gfl2::proc::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    PreDrawFunc
 * @brief   Proc描画下準備
 * @date    2015.07.09
 *
 * @param   pManager    プロセス管理マネージャー
 */
//-----------------------------------------------------------------------------
void ItemMakeProc::PreDrawFunc( gfl2::proc::Manager * pManager )
{
  switch( GetSubSeq() )
  {
  case SEQ_FRAME_MAIN:
    m_pRenderingManager->PreDraw();
    break;

  default:
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    DrawFunc
 * @brief   Proc描画処理
 * @date    2015.05.26
 *
 * @param   pManager    プロセス管理マネージャー
 * @param   displayNo   描画対象画面
 */
//-----------------------------------------------------------------------------
void ItemMakeProc::DrawFunc( gfl2::proc::Manager * pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  switch( GetSubSeq() )
  {
  case SEQ_FRAME_MAIN:
    m_pFrameManager->Draw( displayNo );
    m_pRenderingManager->Draw( displayNo );
    break;

  default:
    break;
  }
}



//-----------------------------------------------------------------------------
/**
 * @func    CreateRenderManager
 * @brief   描画マネージャ生成
 * @date    2015.07.09
 */
//-----------------------------------------------------------------------------
void ItemMakeProc::CreateRenderManager(void)
{
  m_pRenderingManager = GFL_NEW(m_pManagerHeap) app::util::AppRenderingManager();

  app::util::AppRenderingManager::Description desc;
  for( int i=0; i<app::util::AppRenderingManager::RenderPlaceType::NUM; i++ )
  {
    desc.renderDesc[i].renderOrder     = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
    desc.renderDesc[i].pokeModelNumMax = 0;
    desc.renderDesc[i].modelNumMax     = 0;
  }

  m_pRenderingManager->StartAsyncCreate(
    desc,
    &m_heap,
    NULL,
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteRenderManager
 * @brief   描画マネージャ削除
 * @date    2015.07.09
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ItemMakeProc::DeleteRenderManager(void)
{
  if( m_pRenderingManager->CanDestroy() == false )
  {
    return false;
  }
  GFL_DELETE m_pRenderingManager;
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateMainFrame
 * @brief   フレーム生成
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void ItemMakeProc::CreateMainFrame(void)
{
  // フレーム生成
  m_pFrameManager = GFL_NEW(m_pManagerHeap) applib::frame::Manager( m_pManagerHeap );
  m_pItemMakeFrame = MENU_FRAME_CALL_MODAL< applib::frame::Manager, ItemMakeFrame >( m_pFrameManager );
  m_pItemMakeFrame->Setup( &m_heap, m_pRenderingManager, &m_exitID, &m_itemID, &m_itemNum, m_pItemName );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMainFrame
 * @brief   フレーム削除
 * @date    2015.05.26
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ItemMakeProc::DeleteMainFrame(void)
{
  if( m_pFrameManager == NULL )
  {
    return true;
  }

  if( m_pFrameManager->End() != false )
  {
    return false;
  }
  GFL_SAFE_DELETE( m_pFrameManager );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateKeybord
 * @brief   キーボード生成
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void ItemMakeProc::CreateKeybord(void)
{
  m_pStrBuf = GFL_NEW(m_heap.GetDeviceHeap()) gfl2::str::StrBuf( 256, m_heap.GetDeviceHeap() );

  debug::KeyboardProc * proc = GameSys::CallDebugKeyboardProc();
  proc->SetupParam( m_heap.GetSystemHeap(), m_heap.GetDeviceHeap(), m_pStrBuf, 32 );
  proc->SetSearchWords( m_pItemName, &m_keyboardIndex );
  proc->SetDefaultInputMode( debug::KeyboardProc::INPUT_HIRAGANA );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteKeybord
 * @brief   キーボード削除
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void ItemMakeProc::DeleteKeybord(void)
{
  if( m_keyboardIndex != debug::KeyboardProc::MATCH_NONE )
  {
    m_itemID = m_keyboardIndex;
  }
  GFL_SAFE_DELETE( m_pStrBuf );
}


GFL_NAMESPACE_END(Debug)

#endif  // PM_DEBUG
