// ============================================================================
/*
 * @file JoinFestaSubScreenWork.h
 * @brief ジョインフェスタ下画面のワーククラスです。
 * @date 2015.06.10
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTASUBSCREEN_WORK_H_INCLUDE )
#define JOINFESTASUBSCREEN_WORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetStatic/NetAppLib/include/Message/NetAppMessageDataUtility.h"

// リクエスト定義
#include "Field/FieldStatic/include/Subscreen/SubscreenRequestDef.h"

// ジョインフェスタフレンドキー
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaNetDefine.h"
// ジョインフェスタ下画面定義
#include "NetApp/JoinFestaSubScreen/include/JoinFestaSubScreenDefine.h"

#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameID.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


class JoinFestaSubScreenWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSubScreenWork );

private:

  // どちらもHEAPID_FIELD_SUBSCREEN_DEVICEEからとられる
  static const u32  APP_HEAP_SIZE             = 200 * 1024;    //  200k
  static const u32  APP_DEVICE_HEAP_SIZE      = (1456 * 1024);   // 1.4MB

  // 起動リクエスト
  Field::SubScreen::FieldMenuOutRequestID m_requestId;

  // 受信パケットのイベントチケットID
  u32                     m_packetEventTicketId;
  // 受信パケットのイベント通知
  Define::PacketEventType m_packetEventType;
  // 受信パケットのイベント通知相手のフレンドキー
  JoinFestaFriendKey      m_packetEventFriendKey;

  u32                                                     m_alertMessageID;
  NetApp::JoinFestaSubScreen::JoinFestaSubScreenFrameID   m_alertBootFrameID;

public:
  JoinFestaSubScreenWork();
  virtual~JoinFestaSubScreenWork();

  // 起動リクエストアクセサー
  void SetRequestID( Field::SubScreen::FieldMenuOutRequestID requestId );
  void ResetRequestID();
  Field::SubScreen::FieldMenuOutRequestID GetRequestID() const;

  // 受信パケットのイベント通知アクセサー
  void SetPacketEventType( const Define::PacketEventType packetEventType ); 
  Define::PacketEventType GetPacketEventType() const;
  // 受信パケットのイベント通知相手のフレンドキーアクセサー
  void SetPacketEventFriendKey( JoinFestaFriendKey friendKey, u32 ticketId ); 
  JoinFestaFriendKey  GetPacketEventFriendKey() const;
  u32 GetPacketEventTicketId() const;
  // 受信パケットのリセット（キャンセル時）
  void ResetPacketEvent();

  void SetAlertMessageIDAndBootFrameID( const u32 alertMessageID, const NetApp::JoinFestaSubScreen::JoinFestaSubScreenFrameID alertBootFrameID );
  u32 GetAlertMessageID() const;
  NetApp::JoinFestaSubScreen::JoinFestaSubScreenFrameID GetAlertBootFrameID() const;

  //! チュートリアル中かどうか
  bool IsTutorial() const;

  //! 城内かどうか
  bool IsRoom() const;

  //! BGのみの表示にするかどうか
  bool IsBgOnly() const;

};



GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASUBSCREENWORK_H_INCLUDE
