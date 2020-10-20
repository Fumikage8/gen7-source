//======================================================================
/**
 * @file    StatusMsgWindow.cpp
 * @date    2015/12/08 19:00:10
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：メッセージウィンドウ(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "StatusMsgWindow.h"

#include <App/Status/source/StatusDrawerBase.h>

#include <AppLib/include/Ui/UIView.h>

// arc
#include <niji_conv_header/app/field_status/field_status_pane.h>
#include <niji_conv_header/app/field_status/field_status_anim_list.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  //! @brief  ボタンID
  static const u32 c_ButtonID[] = {
    BTN_ID_INFO_TOKUSEI,
    BTN_ID_INFO_ITEM,
    BTN_ID_BOX_MARK,
  };
}

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
MsgWindow::MsgWindow( DrawerBase* base )
  : m_pBase(base)
  , m_isOpen(false)
  , m_InputEnable(false)
  , m_pTextMsg(NULL)
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------------------
void MsgWindow::Initialize( void )
{
  // ペインの設定
  {
    m_pPartsMsg = m_pBase->GetParts( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTS_MSG );
    m_pTextMsg = m_pBase->GetTextBox( PANENAME_COMMON_MSG_LOW_001_PANE_TEXTBOX_00, m_pPartsMsg );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   開く
 *
 * @param   msgID         メッセージID
 * @param   inputEnalbe   入力フラグを変更するかどうか(true = 変更、false = 変更しない)
 */
//------------------------------------------------------------------------------
void MsgWindow::Open( const u32 msgID, bool inputEnable )
{
  m_InputEnable = inputEnable;

  if( !m_isOpen )
  {
    // メッセージの設定
    SetMessageID( msgID );

    // 開く
    m_pBase->StartAnim( LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_MSG_IN );

    // 表示
    m_pBase->SetVisiblePane( m_pPartsMsg, true );

    if( m_InputEnable )
    {
      // ボタンの入力を変更
      SetDisableInput();
    }

    m_isOpen = true;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   閉じる
 */
//------------------------------------------------------------------------------
void MsgWindow::Close( void )
{
  if( m_isOpen )
  {
    if( m_InputEnable )
    {
      // ボタンの入力を変更
      SetEnableInput();
    }

    // 開く
    m_pBase->StartAnim( LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_MSG_OUT );

    m_InputEnable = false;
    m_isOpen = false;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   閉じ終わったか？
 *
 * @return  "true  == 閉じ終わった"
 * @return  "false == 継続"
 */
//------------------------------------------------------------------------------
bool MsgWindow::IsClosed( void ) const
{
  return m_pBase->IsAnimEnd( LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_MSG_OUT );
}


//------------------------------------------------------------------------------
/**
 * @brief   ボタンの入力を元に戻す
 */
//------------------------------------------------------------------------------
void MsgWindow::SetEnableInput( void )
{
  app::tool::ButtonManager* btnMan = m_pBase->GetButtonManager();

  for( u32 i=0; i<GFL_NELEMS(c_ButtonID); ++i )
  {
    u32 btnIdx = c_ButtonID[i];

    // 元のボタンフラグをセットする
    btnMan->SetButtonInputEnable( btnIdx, m_BtnEnable[btnIdx] );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンの入力を無効にする
 */
//------------------------------------------------------------------------------
void MsgWindow::SetDisableInput( void )
{
  app::tool::ButtonManager* btnMan = m_pBase->GetButtonManager();

  for( u32 i=0; i<GFL_NELEMS(c_ButtonID); ++i )
  {
    u32 btnIdx = c_ButtonID[i];

    // 現在のボタンの状態を記憶
    m_BtnEnable[btnIdx] = btnMan->IsButtonInputEnable( btnIdx );
    // すべてのボタンを無効に
    btnMan->SetButtonInputEnable( btnIdx, false );
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   メッセージIDの設定
 *
 * @param   msgID   メッセージID
 */
//------------------------------------------------------------------------------
void MsgWindow::SetMessageID( const u32 msgID )
{
  app::ui::UIView* view = m_pBase->GetUIView();

  // メッセージID
  view->SetTextboxPaneMessage( m_pTextMsg, msgID );
}

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)
