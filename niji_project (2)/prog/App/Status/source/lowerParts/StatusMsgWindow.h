#if !defined( __APP_STATUS_MESSAGE_WINDOW_H_INCLUDED__ )
#define __APP_STATUS_MESSAGE_WINDOW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    StatusMsgWindow.h
 * @date    2015/12/08 18:51:16
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：メッセージウィンドウ(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "StatusWazaExchange.h"

#include <App/Status/source/StatusLowerDraw.h>

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <Layout/include/gfl2_Layout.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)
class DrawerBase;
GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   メッセージウィンドウ(下)
//------------------------------------------------------------------------------
class MsgWindow
  : public WazaExchangeMsgListener
{
  GFL_FORBID_COPY_AND_ASSIGN( MsgWindow );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  MsgWindow( DrawerBase* base );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~MsgWindow( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //------------------------------------------------------------------
  void Initialize( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   開く
   *
   * @param   msgID         メッセージID
   * @param   inputEnalbe   入力フラグを変更するかどうか(true = 変更、false = 変更しない)
   */
  //------------------------------------------------------------------
  void Open( const u32 msgID, bool inputEnable = true );

  //------------------------------------------------------------------
  /**
   * @brief   閉じる
   */
  //------------------------------------------------------------------
  void Close( void );

  //------------------------------------------------------------------
  /**
   * @brief   閉じ終わったか？
   *
   * @return  "true  == 閉じ終わった"
   * @return  "false == 継続"
   */
  //------------------------------------------------------------------
  bool IsClosed( void ) const;

private:
  //------------------------------------------------------------------
  /**
   * @brief   ボタンの入力を元に戻す
   */
  //------------------------------------------------------------------
  void SetEnableInput( void );

  //------------------------------------------------------------------
  /**
   * @brief   ボタンの入力を無効にする
   */
  //------------------------------------------------------------------
  void SetDisableInput( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   メッセージIDの設定
   *
   * @param   msgID   メッセージID
   */
  //------------------------------------------------------------------
  virtual void SetMessageID( const u32 msgID );

private:
  static const u32 BUTTON_NUM = BTN_ID_NUM;

private:
  DrawerBase*     m_pBase;

private:
  bool    m_isOpen;

  bool    m_InputEnable;

  bool    m_BtnEnable[BUTTON_NUM];

private:
  gfl2::lyt::LytParts*        m_pPartsMsg;

  gfl2::lyt::LytTextBox*      m_pTextMsg;
};

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

#endif // __APP_STATUS_MESSAGE_WINDOW_H_INCLUDED__
