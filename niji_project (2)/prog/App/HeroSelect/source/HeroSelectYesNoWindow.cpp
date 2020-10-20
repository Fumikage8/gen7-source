//======================================================================
/**
 * @file HeroSelectYesNoWindow.cpp
 * @date 2015/08/27 10:59:02
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "HeroSelectYesNoWindow.h"
#include "HeroSelectDrawerBase.h"
#include "HeroSelectConst.h"

#include <AppLib/include/Tool/app_tool_MenuWindow.h>

// arc
#include <niji_conv_header/message/msg_introdemo.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( HeroSelect )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  enum {
    SEQ_READY,
    SEQ_WAIT,
    SEQ_OPEN,
    SEQ_MAIN,
    SEQ_CLOSE,
    SEQ_NUM,
  };
}


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
YesNoWindow::YesNoWindow( DrawerBase* pBase, MenuCursor* pCursor, app::util::AppRenderingManager* pRenderingManager )
  : m_pBase( pBase )
  , m_pCursor( pCursor )
  , m_pAppRenderingManager( pRenderingManager )
  , m_pMenuWindow( NULL )
  , m_pMessage( NULL )
  , m_Seq( SEQ_READY )
  , m_Result( WND_RES_NONE )
  , m_isOpenWindow( false )
{
  Init();
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
YesNoWindow::~YesNoWindow( void )
{
  if( m_pMenuWindow )
  {
    m_pMenuWindow->RemoveSubView();

    GFL_SAFE_DELETE( m_pMenuWindow );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------------------
void YesNoWindow::Init( void )
{
  gfl2::heap::HeapBase* devHeap = m_pBase->GetDeviceHeap();

  // メニューウィンドウの生成
  {
    m_pMenuWindow = GFL_NEW(devHeap) app::tool::MenuWindow(
      m_pBase->GetHeap(), m_pAppRenderingManager, m_pCursor->GetCursorBuffer(), app::util::G2DUtil::SETUP_UPPER );
    m_pMenuWindow->SetListener( this );

    // はい
    m_pMenuWindow->AddItem( m_pBase->GetMsgData(), intro_yes );
    // いいえ
    m_pMenuWindow->AddItem( m_pBase->GetMsgData(), intro_no, true, false );

    // UIViewへの登録
    m_pBase->GetUIView()->AddSubView( m_pMenuWindow );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void YesNoWindow::Update( void )
{
  switch( m_Seq )
  {
  case SEQ_READY:
    {
      if( IsReady() )
      {
        // 入力をキーのみに変更
        m_pMenuWindow->SetInputMode( app::tool::ButtonManager::INPUT_MODE_KEY_ONLY );

        m_Seq = SEQ_WAIT;
      }
    } break;

  case SEQ_OPEN:
    {
      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_CLOSE:
    {
      m_Seq = SEQ_WAIT;
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   準備完了か？
 * @return  "true" == 完了
 */
//------------------------------------------------------------------------------
bool YesNoWindow::IsReady( void )
{
  GFL_ASSERT( m_pMenuWindow );

  return m_pMenuWindow->IsCreate();
}

//------------------------------------------------------------------------------
/**
 * @brief   開く
 * @param   msgID     メッセージID
 * @param   isYesNo   YESNOウィンドウの表示フラグ
 */
//------------------------------------------------------------------------------
void YesNoWindow::OpenWindow( void )
{
  if( m_Seq != SEQ_WAIT ) return;

  m_pBase->GetUIView()->SetInputEnabled( false );

  // メニューウィンドウを開く
  m_pMenuWindow->StartMenuDisplay( 0, app::tool::MenuWindow::PUT_RD, app::tool::MenuWindow::DEFAULT_PUT_X_UPPER, app::tool::MenuWindow::DEFAULT_PUT_Y_UPPER );

  m_Result = WND_RES_NONE;
  m_isOpenWindow = true;
  m_Seq = SEQ_OPEN;
}

//------------------------------------------------------------------------------
/**
 * @brief   閉じる
 */
//------------------------------------------------------------------------------
void YesNoWindow::CloseWindow( void )
{
  if( !m_isOpenWindow ) return;

  // メニューウィンドウを閉じる
  m_pMenuWindow->EndMenu( false );

  m_pBase->GetUIView()->SetInputEnabled( true );

  m_isOpenWindow = false;
  m_Seq = SEQ_CLOSE;
}

//------------------------------------------------------------------------------
/**
 * @brief   結果の取得
 */
//------------------------------------------------------------------------------
u8 YesNoWindow::GetResult( void ) const
{
  return m_Result;
}

//------------------------------------------------------------------------------
/**
 * @brief   メニューウィンドウの選択
 * @param   選択された項目ID
 */
//------------------------------------------------------------------------------
void YesNoWindow::SelectMenu( u32 id )
{
  m_Result = (id == 0) ? WND_RES_YES : WND_RES_NO;

  CloseWindow();
}

GFL_NAMESPACE_END( HeroSelect )
GFL_NAMESPACE_END( App )
