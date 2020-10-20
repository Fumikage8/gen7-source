// ============================================================================
/*
 * @file JoinFestaSubScreenWork.cpp
 * @brief ジョインフェスタ下画面のワーククラスです。
 * @date 2015.06.10
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameResult.h"
#include "System/include/HeapDefine.h"

#include "JoinFestaSubScreenWork.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameID.h"


// niji
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "Field/FieldRo/include/Fieldmap.h"
#include "niji_conv_header/field/FieldGimmickTypes.h"

// savedata
#include "Savedata/include/EventWork.h"
#include "niji_conv_header/field/flagwork/WorkDefine.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


JoinFestaSubScreenWork::JoinFestaSubScreenWork() :
ApplicationWorkBase( HEAPID_FIELD_SUBSCREEN_DEVICE, APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE ),
m_requestId(Field::SubScreen::OUT_REQ_ID_NONE)
,m_packetEventTicketId( 0 )
,m_packetEventType( Define::PACKET_EVENT_TYPE_NONE )
,m_packetEventFriendKey()
,m_alertMessageID( 0 )
,m_alertBootFrameID( NetApp::JoinFestaSubScreen::JOINFESTASUBSCREEN_FRAME_ID_MAIN )
{
}


JoinFestaSubScreenWork::~JoinFestaSubScreenWork()
{
}


//----------------------------------------------------------------------------
/**
 *	@brief  起動リクエスト設定
 */
//-----------------------------------------------------------------------------
void JoinFestaSubScreenWork::SetRequestID( Field::SubScreen::FieldMenuOutRequestID requestId )
{
  GFL_PRINT("JoinFestaSubScreenWork::SetRequestID %d \n", requestId );
  m_requestId = requestId;
}

//----------------------------------------------------------------------------
/**
 *	@brief  起動リクエストリセット
 */
//-----------------------------------------------------------------------------
void JoinFestaSubScreenWork::ResetRequestID()
{
  m_requestId = Field::SubScreen::OUT_REQ_ID_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  起動リクエスト取得
 */
//-----------------------------------------------------------------------------
Field::SubScreen::FieldMenuOutRequestID JoinFestaSubScreenWork::GetRequestID() const
{
  return m_requestId;
}

//----------------------------------------------------------------------------
/**
 *	@brief  受信パケットのイベント通知設定
 */
//----------------------------------------------------------------------------
void JoinFestaSubScreenWork::SetPacketEventType( const Define::PacketEventType packetEventType ) 
{
  GFL_PRINT("JoinFestaSubScreenWork::SetPacketEventType %d \n", packetEventType );
  m_packetEventType = packetEventType;
}

//----------------------------------------------------------------------------
/**
 *	@brief  受信パケットのイベント通知取得
 */
//----------------------------------------------------------------------------
Define::PacketEventType JoinFestaSubScreenWork::GetPacketEventType() const
{
  return m_packetEventType;
}

//----------------------------------------------------------------------------
/**
 *	@brief  受信パケットのイベント通知のフレンドキー設定
 */
//----------------------------------------------------------------------------
void JoinFestaSubScreenWork::SetPacketEventFriendKey( JoinFestaFriendKey friendKey, u32 ticketId )
{
  m_packetEventFriendKey = friendKey;
  m_packetEventTicketId = ticketId;
}

//----------------------------------------------------------------------------
/**
 *	@brief  受信パケットのイベント通知のフレンドキー取得
 */
//----------------------------------------------------------------------------
JoinFestaFriendKey  JoinFestaSubScreenWork::GetPacketEventFriendKey() const
{
  return m_packetEventFriendKey;
}

//----------------------------------------------------------------------------
/**
 *	@brief  受信パケットのイベント通知のチケットID取得
 */
//----------------------------------------------------------------------------
u32 JoinFestaSubScreenWork::GetPacketEventTicketId() const
{
  return m_packetEventTicketId;
}

//----------------------------------------------------------------------------
/**
 *	@brief  受信パケットのリセット（キャンセル時）
 */
//----------------------------------------------------------------------------
void JoinFestaSubScreenWork::ResetPacketEvent()
{
  m_packetEventType = Define::PACKET_EVENT_TYPE_NONE;
  m_packetEventFriendKey.initialize();
  m_packetEventTicketId = 0;
}

void JoinFestaSubScreenWork::SetAlertMessageIDAndBootFrameID( const u32 alertMessageID, const NetApp::JoinFestaSubScreen::JoinFestaSubScreenFrameID alertBootFrameID )
{
  m_alertMessageID = alertMessageID;
  m_alertBootFrameID = alertBootFrameID;
}

u32 JoinFestaSubScreenWork::GetAlertMessageID() const
{
  return m_alertMessageID;
}

NetApp::JoinFestaSubScreen::JoinFestaSubScreenFrameID JoinFestaSubScreenWork::GetAlertBootFrameID() const
{
  return m_alertBootFrameID;
}


//! チュートリアル中かどうか
bool JoinFestaSubScreenWork::IsTutorial() const
{
  Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
  if( *(pEventWork->GetEventWorkAdrs( WK_SYS_JF_TUTORIAL )) < 5 )
  {
    return true;
  }
  return false;
}

//! 城内かどうか
bool JoinFestaSubScreenWork::IsRoom() const
{
  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Field::Fieldmap* pFieldmap          = pGameManager->GetFieldmap();
  if( pFieldmap != NULL )
  {
    if( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM )
    {
      return true;
    }
  }
  return false;
}

//! BGのみの表示にするかどうか
bool JoinFestaSubScreenWork::IsBgOnly() const
{
  Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
  if( pEventWork->CheckEventFlag( SYS_FLAG_BFES_ROCKET_NOTTORI ) )
  {
    return true;
  }
  return false;
}


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)
