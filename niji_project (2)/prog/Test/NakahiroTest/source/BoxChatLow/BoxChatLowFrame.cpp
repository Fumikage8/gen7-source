#if PM_DEBUG

// module
#include "BoxChatLowFrame.h"
// niji
#include "GameSys/include/GameData.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Util/app_util_Heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/boxLyt.gaix"
#include "arc_index/MenuCursor.gaix"
#include "arc_index/message.gaix"


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Nakahiro)

//! メインシーケンス
enum MainSeq
{
  SEQ_MAIN = 0,       //!< メイン
  SEQ_END
};


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
BoxChatLowFrame::BoxChatLowFrame(void)
  : m_heap( NULL )
  , m_renderingManager( NULL )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
BoxChatLowFrame::~BoxChatLowFrame()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    Setup
 * @brief   パラメータセット
 * @date    2015.05.26
 *
 * @parma   heap        ヒープ
 * @param   render_man  描画マネージャー
 */
//-----------------------------------------------------------------------------
void BoxChatLowFrame::Setup( app::util::Heap * heap, app::util::AppRenderingManager * render_man )
{
  m_heap             = heap;
  m_renderingManager = render_man;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   フレーム初期化
 * @date    2015.05.26
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result BoxChatLowFrame::InitFunc(void)
{
  LoadLayoutResource();
  LoadMenuCursorResource();
  CreateMsgData();
  CreateDraw();
  return applib::frame::RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   フレーム終了
 * @date    2015.05.26
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result BoxChatLowFrame::EndFunc(void)
{
  if( DeleteDraw() == false )
  {
    return applib::frame::RES_CONTINUE;
  }
  DeleteMsgData();
  UnloadMenuCursorResource();
  UnloadLayoutResource();
  return applib::frame::RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateFunc
 * @brief   フレーム更新
 * @date    2015.05.26
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result BoxChatLowFrame::UpdateFunc(void)
{
  m_pDraw->UpdateTree();

  switch( GetSubSeq() )
  {
  case SEQ_MAIN:
    break;
  case SEQ_END:
    return applib::frame::RES_FINISH;
  }
  return applib::frame::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    DrawFunc
 * @brief   フレーム描画
 * @date    2015.05.26
 *
 * @param   no  描画先のディスプレイ番号
 */
//-----------------------------------------------------------------------------
void BoxChatLowFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  m_pDraw->DrawTree( no );
}


//-----------------------------------------------------------------------------
/**
 * @func    CreateDraw
 * @brief   描画クラス生成
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void BoxChatLowFrame::CreateDraw(void)
{
  m_pDraw = GFL_NEW(m_heap->GetDeviceHeap()) App::Box::BoxLowChtDraw( m_heap, m_renderingManager );
  m_pDraw->Init( m_pLytResBuff, m_pMsgData, m_pMenuCursorResBuff );
  m_pDraw->SetUIListener( this );

/*
  m_pDraw->SetDrawListener( this );
  m_pDraw->Init( m_pLytResBuff );
  m_pDraw->InitSmallList( m_pItemIcon, m_pMenuCursor[0] );
  m_pDraw->InitMiddleList( m_pItemIcon, m_pMenuCursor[1] );
  m_pDraw->InitLargeList( m_pItemIcon, m_pMenuCursor[2] );
  m_pDraw->ChangeInput();
*/
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteDraw
 * @brief   描画クラス削除
 * @date    2015.05.26
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BoxChatLowFrame::DeleteDraw(void)
{
  if( m_pDraw != NULL )
  {
    if( m_pDraw->End() == false )
    {
      return false;
    }
    GFL_SAFE_DELETE( m_pDraw );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadLayoutResource
 * @brief   レイアウトリソース読み込み
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void BoxChatLowFrame::LoadLayoutResource(void)
{
  app::util::FileAccessor::FileOpenSync( ARCID_BOX, m_heap->GetDeviceHeap() );
  app::util::FileAccessor::FileLoadSync(
    ARCID_BOX, GARC_boxLyt_chtlow_applyt_COMP, &m_pLytResBuff, m_heap->GetDeviceHeap(), true, 128 );
  app::util::FileAccessor::FileCloseSync( ARCID_BOX );
}

//-----------------------------------------------------------------------------
/**
 * @func    UnloadLayourResource
 * @brief   レイアウトリソース削除
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void BoxChatLowFrame::UnloadLayoutResource(void)
{
  GflHeapFreeMemory( m_pLytResBuff );
}

void BoxChatLowFrame::LoadMenuCursorResource(void)
{
  app::util::FileAccessor::FileOpenSync( ARCID_MENU_CURSOR, m_heap->GetDeviceHeap() );
  app::util::FileAccessor::FileLoadSync(
    ARCID_MENU_CURSOR, GARC_MenuCursor_MenuCursor_applyt_COMP, &m_pMenuCursorResBuff, m_heap->GetDeviceHeap(), true, 128 );
  app::util::FileAccessor::FileCloseSync( ARCID_MENU_CURSOR );
}

void BoxChatLowFrame::UnloadMenuCursorResource(void)
{
  GflHeapFreeMemory( m_pMenuCursorResBuff );
}

void BoxChatLowFrame::CreateMsgData(void)
{
  m_pMsgData = GFL_NEW(m_heap->GetDeviceHeap()) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_box_DAT, m_heap->GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );
}

void BoxChatLowFrame::DeleteMsgData(void)
{
  GFL_SAFE_DELETE( m_pMsgData );
}




void BoxChatLowFrame::OnButtonAction( int button_no )
{
  if( button_no == App::Box::BoxLowChtDrawListener::LOWCHT_ACTION_CANCEL )
  {
    SetSubSeq( SEQ_END );
  }
  else
  {
    GFL_PRINT( "★%d\n", button_no );
  }
}


GFL_NAMESPACE_END(Nakahiro)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG
