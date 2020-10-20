//======================================================================
/**
 * @file    TitleMenuYesNoWindow.cpp
 * @date    2015/09/07 18:13:21
 * @author  fukushima_yuya
 * @brief   タイトルメニューYesNoウィンドウ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "TitleMenuYesNoWindow.h"
#include "TitleMenuDrawerBase.h"
#include "TitleMenuConst.h"

#include <AppLib/include/Tool/app_tool_MenuWindow.h>

// arc
#include <niji_conv_header/message/msg_title_menu.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( TitleMenu )

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
YesNoWindow::YesNoWindow( DrawerBase* pBase, MenuCursor* pCursor, app::util::AppRenderingManager* pRenderingManager, app::tool::MenuWindowListener* pListener )
  : m_pBase( pBase )
  , m_pCursor( pCursor )
  , m_pAppRenderingManager( pRenderingManager )
  , m_pMenuWindow( NULL )
  , m_pMessage( NULL )
  , m_Seq( SEQ_READY )
  , m_isOpenWindow( false )
{
  Init( pListener );
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
    m_pMenuWindow->RemoveFromSuperView();

    GFL_SAFE_DELETE( m_pMenuWindow );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------------------
void YesNoWindow::Init( app::tool::MenuWindowListener* pListener )
{
  gfl2::heap::HeapBase* devHeap = m_pBase->GetDeviceHeap();

  // メニューウィンドウの生成
  {
    m_pMenuWindow = GFL_NEW(devHeap) app::tool::MenuWindow(
      m_pBase->GetHeap(), m_pAppRenderingManager, m_pCursor->GetCursorBuffer(), app::util::G2DUtil::SETUP_UPPER );
    m_pMenuWindow->SetListener( pListener );

    // はい
    m_pMenuWindow->AddItem( m_pBase->GetMsgData(), title_menu_12_03 );
    // いいえ
    m_pMenuWindow->AddItem( m_pBase->GetMsgData(), title_menu_12_04, true, false );

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
 * @brief   破棄完了か
 * @return  "true" == 完了
 */
//------------------------------------------------------------------------------
bool YesNoWindow::IsDelete( void )
{
  if( m_pMenuWindow )
  {
    return m_pMenuWindow->IsModuleFree();
  }

  return true;
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
  m_pMenuWindow->StartMenuDisplay( 1, app::tool::MenuWindow::PUT_RD, app::tool::MenuWindow::DEFAULT_PUT_X_UPPER, app::tool::MenuWindow::DEFAULT_PUT_Y_UPPER );

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

GFL_NAMESPACE_END( TitleMenu )
GFL_NAMESPACE_END( App )
