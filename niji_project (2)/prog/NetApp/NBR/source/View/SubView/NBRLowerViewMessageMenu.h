// ============================================================================
/*
 * @file NBRLowerViewMessageMenu.h
 * @brief 通信対戦受付アプリ　下画面ビューメッセージメニュ
 * @date 2015.07.28
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBRLOWERVIEW_MESSAGEMENU_H_INCLUDE )
#define NBRLOWERVIEW_MESSAGEMENU_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

#include "NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h"

#include "GameSys/include/GameData.h"
#include "NetApp/NBR/source/NBRSoundDefine.h"
#include "NetApp/NBR/source/View/NBRViewDefine.h"
// ビューベース 
#include "NetApp/NBR/source/View/NBRLowerViewBase.h"
// リスナー
#include "NetApp/NBR/source/View/SubView/NBRLowerViewMessageMenuListener.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(System)
class ApplicationWorkBase;
GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)

class NBRLowerViewMessageMenu :
  public NBRLowerViewBase,
  public NetAppLib::UI::NetAppMessageMenu::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRLowerViewMessageMenu );
public:
  // コンストラクタ
  NBRLowerViewMessageMenu( NetAppLib::System::ApplicationWorkBase* pWorkBase,
                       app::util::G2DUtil* pG2dUtil,
                       app::util::G2DUtil::LAYOUT_WORK* pLayoutWork );
  // デストラクタ
  virtual~NBRLowerViewMessageMenu();

  // NetAppMessageMenu::IEventHandler
  virtual void MessageMenuView_OnEvent( const NetAppLib::UI::NetAppMessageMenu::IEventHandler::EventCode eEventCode );

  // 準備完了したか
  bool IsInitialized();

  // リスナー
  void SetListener( NBRLowerViewMessageMenuListener* pListener ) { m_pListener = pListener; }
  void RemoveListener() { m_pListener = NULL; }

public:
  //=========================================================
  //
  //  メッセージウィンドウ系
  //
  //=========================================================
  // メッセージウィンドウ（一括）
  void ShowMessage( const u32 msgId, const bool isShowTimeIcon );
  void ShowMessageStrBuf( gfl2::str::StrBuf* buf, const bool isShowTimeIcon );
  // メッセージウィンドウ（ストリーム）
  void StartMessage( const u32 msgId, const MessageFinishType finishType );
  void StartMessageStrBuf( gfl2::str::StrBuf* buf, const MessageFinishType finishType );
  bool IsEndMessage( void ) const;

  //! アプリ終了時の待機メッセージ
  void ShowAppEndMessage();

  /**
   * @breif  ルール選択者決定でのチュートリアルメッセージ(ストリーミング)
   * @param  connectNum  接続人数
   * @param  isMutli     マルチ
   */
  void StartMessageRuleSelectorTutorial( int connectNum, bool isMulti );

  // 名前埋め込みでメッセージウィンドウ（一括）
  void ShowMessageMyStatus( const u32 msgId, Savedata::MyStatus* pMyStatus, bool isShowTimeIcon );
  // 名前埋め込みでメッセージウィンドウ（ストリーム）
  void StartMessageMyStatus( const u32 msgId, const MessageFinishType finishType, Savedata::MyStatus* pMyStatus );

  //=========================================================
  //
  //  選択肢系
  //
  //=========================================================
  //! 対戦をやめますか選択肢
  void ShowAppEndList();
  //! ルールを決めるのを他の人にお任せするか選択肢
  void ShowRuleOtherList();
  //! 別の対戦ルールにするかの選択肢
  void ShowCannotJoinList();

  //! マルチで立ち位置にプレイヤーを選ぶ選択肢
  void ShowMutliPlayerList( const u8 pos, Savedata::MyStatus* pMyStatus[4], bool isRandomEnable, int disableNo );

  //! 選択肢が表示されているか
  bool IsShowList(){ return m_bShowList; }


  //! 共通：非表示
  void HideMessage();

private:
  // 名前埋め込みでメッセージ
  void ShowMessageMyStatusCore( const u32 msgId, const MessageFinishType finishType, Savedata::MyStatus* pMyStatus, const bool isImmediateMessage, const bool isShowTimerIcon );

  // 選択肢表示
  void ShowList( u32 msgId );

private:
  NetAppLib::UI::NetAppMessageMenu                      m_NetAppMessageMenu;  //! 共通ウィンドウ・リスト
  bool                                                  m_bEndMsgStream;      //! ストリームメッセージの表示終了フラグ
  NBRLowerViewMessageMenuListener*                      m_pListener;
  bool                                                  m_bShowList;          //! 選択肢コマンド表示中か
  gfl2::str::StrBuf*                                    m_strbuf;             //!< 汎用文字列バッファ
};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBRLOWERVIEW_MESSAGEMENU_H_INCLUDE
