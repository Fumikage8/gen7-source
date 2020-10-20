// ============================================================================
/*
 * @file JoinFestaSubScreenFrameBase.cpp
 * @brief ジョインフェスタ下画面のフレームベース
 * @date 2015.09.01
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameBase.h"

// FrameResultID
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameResult.h"

// ジョインフェスタ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaNetDefine.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketEventList.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaSubScreenFrameBase::JoinFestaSubScreenFrameBase( ApplicationSystem::JoinFestaSubScreenWork* pJoinFestaSubScreenWork,
                                                          View::JoinFestaSubScreenParentView* pJoinFestaSubScreenParentView ) :
  NetAppLib::System::NetAppFrameBase( pJoinFestaSubScreenWork ),
  m_pJoinFestaSubScreenWork( pJoinFestaSubScreenWork ),
  m_pParentView( pJoinFestaSubScreenParentView ),
  m_pHeap( pJoinFestaSubScreenWork->GetAppHeap()->GetDeviceHeap() )
{
}


JoinFestaSubScreenFrameBase::~JoinFestaSubScreenFrameBase()
{
}

/**
 *  @brief  受信パケットのイベントチェック更新
 */
void JoinFestaSubScreenFrameBase::UpdatePacketEventCheck()
{
  // @fix GFNMcat[916] チュートリアルでもお誘いがくる
  if( m_pJoinFestaSubScreenWork->IsTutorial() )
  {
    return;
  }

  // 受信パケットのイベント通知チェック
  const Define::PacketEventType packetEventType = GetPacketEventType();

  switch( packetEventType ){
  case Define::PACKET_EVENT_TYPE_BATTLE_S: //! バトル（シングル）招待
  case Define::PACKET_EVENT_TYPE_BATTLE_D: //! バトル（ダブル）招待
  case Define::PACKET_EVENT_TYPE_BATTLE_M: //! バトル（マルチ）招待
  case Define::PACKET_EVENT_TYPE_BATTLE_R: //! バトル（ロイヤル）招待
  case Define::PACKET_EVENT_TYPE_TRADE:    //! 交換招待
    // お誘い画面フレームへ
    exitFrame( COMMON_RESULT_INVITATION );
    break;
  }
}

/**
 *  @brief  受信パケットのイベント取得
 *  @return PacketEventType
 */
Define::PacketEventType JoinFestaSubScreenFrameBase::GetPacketEventType()
{
  // 通信パケット受信によるイベント起動チェック
  NetAppLib::JoinFesta::JoinFestaPacketEventList* jfPacketEventList = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketEventList);

  // イベント取得
  const NetAppLib::JoinFesta::JoinFestaPacketEventList::PacketEventInfo* pPacketEvent = jfPacketEventList->PopEvent(true); 

  // イベント存在チェック
  if( pPacketEvent == NULL )
  {
    return Define::PACKET_EVENT_TYPE_NONE;
  }

  //--------------------------------------------
  // パーソナルチェック
  NetAppLib::JoinFesta::JoinFestaPersonalData* personal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetPersonalData( pPacketEvent->m_friendKey );

  if( personal == NULL )
  {
    return Define::PACKET_EVENT_TYPE_NONE; // ユーザーは存在しない
  }

  //--------------------------------------------
  Define::PacketEventType result = Define::PACKET_EVENT_TYPE_NONE;

  // バトル招待
  if( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_INVITE == pPacketEvent->m_kind &&
      personal->IsEnablePacketBattleInvite() )
  {
    NetAppLib::JoinFesta::PacketBattleInvite packet = personal->GetPacketBattleInvite();
    Regulation::BATTLE_RULE battleRule = static_cast<Regulation::BATTLE_RULE>(packet.battle_rule);

    // バトルルール
    switch( battleRule ){
    case Regulation::BATTLE_RULE_SINGLE: result = Define::PACKET_EVENT_TYPE_BATTLE_S; break;
    case Regulation::BATTLE_RULE_DOUBLE: result = Define::PACKET_EVENT_TYPE_BATTLE_D; break;
    case Regulation::BATTLE_RULE_ROYAL:  result = Define::PACKET_EVENT_TYPE_BATTLE_R; break;
    case Regulation::BATTLE_RULE_MULTI:  result = Define::PACKET_EVENT_TYPE_BATTLE_M; break;
    default:
      GFL_ASSERT(0);
      break;
    }
  }
  // 交換招待
  else if( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_INVITE == pPacketEvent->m_kind )
  {
    result = Define::PACKET_EVENT_TYPE_TRADE;
  }

  if( result != Define::PACKET_EVENT_TYPE_NONE )
  {
    // ワークにフレンドキーセット
    m_pJoinFestaSubScreenWork->SetPacketEventFriendKey( personal->GetJoinFestaFriendKey(), pPacketEvent->m_ticketId );

    // ワークに値保存
    m_pJoinFestaSubScreenWork->SetPacketEventType( result );
  }
  return result;
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)
