#if !defined( __FINDER_STUDIO_MESSAGE_MENU_VIEW_H_INCLUDED__ )
#define __FINDER_STUDIO_MESSAGE_MENU_VIEW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioMessageMenuView.h
 * @date    2016/11/30 15:22:48
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：メッセージメニューView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <AppLib/include/Tool/app_tool_MessageMenuView.h>

#include <Str/include/gfl2_Str.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class G2DUtil;
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Tool )
class MessageMenuView;
GFL_NAMESPACE_END( Tool )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( print )
class WordSet;
GFL_NAMESPACE_END( print )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )

//////////////////////////////////////////////////////////////////////////
/// ---[ 型定義 begin ]---
typedef app::util::G2DUtil MessageMenuMsgManager;
/// ---[ 型定義 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///
/// @brief    メッセージメニューViewListener
///
//////////////////////////////////////////////////////////////////////////
class MessageMenuViewListener
{
public:
  //-------------------------------------------------------
  // @brief   リスナー関数
  //-------------------------------------------------------
  virtual void OnEvent( const App::Tool::MessageMenuView::IEventHandler::EventCode event ) { ; }
  virtual void OnSelectMenu( const u32 index ) { ; }
};




//////////////////////////////////////////////////////////////////////////
///
/// @brief    メッセージメニューView
///
//////////////////////////////////////////////////////////////////////////
class MessageMenuView
  : public App::Tool::MessageMenuView::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( MessageMenuView ); //コピーコンストラクタ＋代入禁止

public:
  enum MenuType
  {
    MENU_TYPE_YESNO_CONFIRM,
    MENU_TYPE_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  MessageMenuView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan, gfl2::str::MsgData* msgData, void* cursorBuf );
  virtual ~MessageMenuView( void );

public:
  //-------------------------------------------------------
  // @brief   初期化・終了
  //-------------------------------------------------------
  bool InitFunc( void );
  bool EndFunc( void );

private:
  //-------------------------------------------------------
  // @brief   生成・破棄：非公開
  //-------------------------------------------------------
  void _CreateMessageMenuView( void );
  void _DeleteMessageMenuView( void );

  void _CreateMsgManager( void );
  void _DeleteMsgManager( void );

  //-------------------------------------------------------
  // @brief   チェック関数：非公開
  //-------------------------------------------------------
  bool _IsSetup( void );
  bool _IsCreateMessageMenuView( void );

  //-------------------------------------------------------
  // @brief   レイアウト 読込み・開放：非公開
  //-------------------------------------------------------
  bool _LoadLayout( void );
  void _ReleaseLayout( void );

public:
  //-------------------------------------------------------
  // @brief   メッセージ 表示・非表示
  //-------------------------------------------------------
  void ShowMessage_Id( u32 msgID, bool isImmediateMessage = false, bool isShowTimerIcon = false, bool isShowBG = true );
  void ShowMessage_Str( const gfl2::str::StrBuf* strBuf, bool isImmediateMessage = false, bool isShowTimerIcon = false, bool isShowBG = true );

  void ShowMessageWithListMenu( MenuType type, u32 msgID, bool isImmediateMessage = false );
  void ShowMessageWithListMenu( u32 msgID, const u32 * item, u32 item_max, u32 cancel, bool cancel_mark, bool isImmediateMessage = false );
  void HideMessage( void );

  //-------------------------------------------------------
  // @brief   リスナー 設定・解除
  //-------------------------------------------------------
  void SetListener( MessageMenuViewListener* listener ) { m_pListener = listener; }
  void RemoveListener( void ) { m_pListener = NULL; }

  //-------------------------------------------------------
  // @brief   メッセージデータ 関連
  //-------------------------------------------------------
  void SetMessageManagerMsgDataAndWordSet( gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  void RemoveMessageManagerMsgDataAndWordSet( void );

  //-------------------------------------------------------
  // @brief   取得関数
  //-------------------------------------------------------
  App::Tool::MessageMenuView* GetView( void ) const { return m_pMsgMenuView; }
  MessageMenuMsgManager*      GetMsgManager( void ) const { return m_pMsgManager; }
  gfl2::str::StrBuf*          GetStrBuf( void ) const { return m_pStrBuf; }

private:
  //-------------------------------------------------------
  // @brief   メッセージ表示：非公開
  // @note    追加する場合は列挙体MenuTypeとShowMessageWithListMenuに関数ポインタを追加
  //-------------------------------------------------------
  void _ShowMessageWithListMenu_YesNoConfirm( u32 msgID, bool isImmediateMessage );

  //-------------------------------------------------------
  // @brief   イベント：非公開
  //-------------------------------------------------------
  virtual void MessageMenuView_OnEvent( const App::Tool::MessageMenuView::IEventHandler::EventCode event );

private:
  app::util::Heap*                  m_pHeap;
  app::util::AppRenderingManager*   m_pAppRenderingManager;
  App::Tool::MessageMenuView*       m_pMsgMenuView;
  
  gfl2::str::MsgData*               m_pMsgData;
  gfl2::str::StrBuf*                m_pStrBuf;

  MessageMenuViewListener*          m_pListener;
  MessageMenuMsgManager*            m_pMsgManager;

  void*   m_pLytDatBuff;
  void*   m_pCursorBuff;

  u8    m_MainSeq;
  u8    m_LoadSeq;
};

GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_MESSAGE_MENU_VIEW_H_INCLUDED__