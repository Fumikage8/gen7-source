//============================================================================================
/**
 * @file  JoinFestaInformationMessage.cpp
 *
 * @brief JoinFestaストリーミングメッセージ
 *
 *        JoinFesta通信で受信したタプレイヤー要求をテキスト表示するためのクラス
 */
//============================================================================================

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaInformationMessage.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"

// システム
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// save
#include "Savedata/include/JoinFestaDataSave.h"
#include "Savedata/include/EventWork.h"
#include "Savedata/include/Record.h" // レコードID

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"

#include <niji_conv_header/message/msg_jf_menu.h>

namespace NetAppLib {
namespace JoinFesta {


InformationMessageData::InformationMessageData() :
  m_messageID( 0xffffffff ),
  m_personalData()
{
}

InformationMessageData::InformationMessageData( const InformationMessageData& data ) :
  m_messageID( data.m_messageID )
{
  m_personalData.Copy( data.m_personalData );
}

InformationMessageData::~InformationMessageData()
{
}

InformationMessageData& InformationMessageData::operator = ( const InformationMessageData& data )
{
  m_messageID = data.m_messageID;
  m_personalData.Copy( data.m_personalData );
  return *this;
}

/* ----------------------------------------------------------------------------------------- */
/**
* @brief  コンストラクタ
*
* @param  heap        内部で使用するヒープ
*/
/* ----------------------------------------------------------------------------------------- */
JoinFestaInformationMessage::JoinFestaInformationMessage( gfl2::heap::HeapBase* heap ) :
  m_infoList( heap, MESSAGE_BUFF_SIZE )
{
}


/* ----------------------------------------------------------------------------------------- */
/**
* @brief  デストラクタ
*/
/* ----------------------------------------------------------------------------------------- */
JoinFestaInformationMessage::~JoinFestaInformationMessage( void )
{
  m_infoList.Clear();
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  メッセージの登録
 *
 * @attention バッファオーバーした場合は、登録されない
 *
 * @param  personal 通信で受信したパーソナル情報
 *
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaInformationMessage::PushMessage( JoinFestaPersonalData& personal )
{
  eraseMessage( personal );

  const JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_KIND command = personal.GetRequestCommand();  // 受信対象者のコマンド

  u32 messageID = jf_menu_str_01;

  switch( command )
  {
    //// -----------------------------------------------------------------
    //// 対戦に招待された
    //case  JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_INVITE:
    //{
    //  const PacketBattleInvite& packetBattleInvite = personal.GetPacketBattleInvite();
    //  Regulation::BATTLE_RULE battleRule = static_cast<Regulation::BATTLE_RULE>( packetBattleInvite.battle_rule );
    //  switch( battleRule )
    //  {
    //    case Regulation::BATTLE_RULE_SINGLE: messageID = jf_menu_str_01; break; // シングルバトル
    //    case Regulation::BATTLE_RULE_DOUBLE: messageID = jf_menu_str_02; break; // ダブルバトル
    //    case Regulation::BATTLE_RULE_ROYAL:  messageID = jf_menu_str_03; break; // マルチバトル
    //    case Regulation::BATTLE_RULE_MULTI:  messageID = jf_menu_str_04; break; // バトルロイヤルバトル
    //  }
    //}
    //break;

    //// -----------------------------------------------------------------
    //// 交換に招待された
    //case  JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_INVITE:
    //{
    //  messageID = jf_menu_str_05;
    //}
    //break;

    // -----------------------------------------------------------------
    // アトラクションが開催されていた
    case  JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_ATTRACTION:
    {
      if( !NetAppLib::JoinFesta::JoinFestaPacketUtil::CheckAttractionJoinEnable( &personal ) )
      {
        return;
      }

      // @fix NMCat[2664]：1つのアトラクションに大人数が参加すると同じメッセージが来る
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* pMyData = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPersonalDataManager )->GetMyData();
        if( pMyData->IsOpenAttraction() == true )
        {
          const NetAppLib::JoinFesta::PacketAttraction& packetAttraction = pMyData->GetPacketAttraction();
          if( pMyData->IsSameAttraction( packetAttraction ) )
          {
            ICHI_PRINT(">ICHI 同じアトラクションなので無視\n");
            return;
          }
        }
      }

      messageID = jf_menu_str_06;
    }
    break;
    default:
      // ↑以外は追加しない
      return;
  }

  InformationMessageData data;
  data.SetMessageID( messageID );
  data.SetPersonalData( personal );

#if PM_DEBUG
  gfl2::str::PrintCode( personal.GetName(), "AttractionStreamMsgPush：" );
#endif

  if( m_infoList.Size() < m_infoList.MaxSize() )
  {
    m_infoList.PushBack( data );
  }
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   メッセージを取得する
 *
 * @param[out]  messageData 取得したメッセージデータ
 *
 * @return      取得できたらtrue
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaInformationMessage::PopMessage( InformationMessageData& messageData )
{
  bool bResult = false;

  if( m_infoList.Size() > 0 )
  {
    gfl2::util::List<InformationMessageData>::Iterator it = m_infoList.Begin();

    messageData = (*it);

    m_infoList.PopFront();


    // @fix NMCat[2664]：1つのアトラクションに大人数が参加すると同じメッセージが来る
    NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPersonalDataManager )->GetPersonalData( messageData.GetPersonalData().GetJoinFestaFriendKey() );
    if( NetAppLib::JoinFesta::JoinFestaPacketUtil::CheckAttractionJoinEnable( pJoinFestaPersonalData ) )
    {
      bResult = true;
    }
  }

  return bResult;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   メッセージが存在するか
 * @return  存在する場合true
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaInformationMessage::IsMessageExist() const
{
  return (m_infoList.Size() > 0 );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   メンバ変数のリセット
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaInformationMessage::Reset()
{
  // リストのクリア
  if( m_infoList.Size() > 0 )
  {
    m_infoList.Erase();
  }
}



void JoinFestaInformationMessage::eraseMessage( const JoinFestaPersonalData& personal )
{
  if( m_infoList.Size() > 0 )
  {
    gfl2::util::List<InformationMessageData>::Iterator it = m_infoList.Begin();

    while( it != m_infoList.End() )
    {
      if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey( it->GetPersonalData().GetJoinFestaFriendKey(), personal.GetJoinFestaFriendKey() ) )
      {
        it = m_infoList.Erase( it );
      }
      else
      {
        ++it;
      }
    }
  }
}


}		// JoinFesta
}		// NetAppLib

