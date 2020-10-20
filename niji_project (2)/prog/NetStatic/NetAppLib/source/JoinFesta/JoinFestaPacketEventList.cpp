//============================================================================================
/**
 * @file  JoinFestaPacketEventList.cpp
 * @brief  ジョインフェスタの通信をトリガとして発生するイベントを管理するクラス
 *
 * @author ichiraku_katsuhiko
 */
//============================================================================================
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketEventList.h"

// ライブラリ
#include <util/include/gfl2_std_string.h>

// JoinFesta
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"

/* ----------------------------------------------------------------------------------------- */
/**
*    Singletonインスタンスの実体を定義
*/
/* ----------------------------------------------------------------------------------------- */
SINGLETON_INSTANCE_DEFINITION(NetAppLib::JoinFesta::JoinFestaPacketEventList);
template class gfl2::base::SingletonAccessor<NetAppLib::JoinFesta::JoinFestaPacketEventList>;


namespace NetAppLib {
namespace JoinFesta {

/* ----------------------------------------------------------------------------------------- */
/**
* @brief  コンストラクタ
*
* @param  personalMgr パーソナルマネージャ
* @param  heap        内部で使用するヒープ
*/
/* ----------------------------------------------------------------------------------------- */
JoinFestaPacketEventList::JoinFestaPacketEventList( JoinFestaPersonalDataManager* personalMgr, gfl2::heap::HeapBase* heap ) :
  m_infoList()
{
  m_personalMgr = personalMgr;

  m_infoList.CreateBuffer( heap, EVENT_BUFF_SIZE );
}


/* ----------------------------------------------------------------------------------------- */
/**
* @brief  デストラクタ
*/
/* ----------------------------------------------------------------------------------------- */
JoinFestaPacketEventList::~JoinFestaPacketEventList( void )
{
  m_infoList.Clear();
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief イベントを登録する
 *
 * @attention バッファオーバーした場合は、登録されない
 *
 * @param kind      パケットコマンド
 * @param ticketId  チケットID
 * @param friendKey イベント発行者のJoinFestaフレンドキー
 *
 * @return 登録に成功したならtrueを返却する
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPacketEventList::PushEvent( JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_KIND kind, u32 ticketId, const JoinFestaFriendKey& friendKey )
{
  // 登録済みイベントをキャンセル
  EraseEvent( friendKey );

  // バッファオーバー確認
  if( m_infoList.Size() < m_infoList.MaxSize() )
  {
    PacketEventInfo info; 
    info.initialize( kind, ticketId, friendKey );
    m_infoList.PushBack( info );
    ICHI_PRINT(">ICHI パケットイベント登録 kind:%d ticketId:%d pid:%d \n", kind, ticketId,friendKey.principalId );
    return true;
  }
  return false;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  イベントを取得する
 *
 * @attention イベント発行者のパケット情報が更新されている場合は
 *            イベント情報から除外される。
 *
 * @param[in] isErase 取り出したイベントをリストから削除するならtrueを指定
 *
 * @return 取り出したイベント情報へのポインタ。データが存在しない場合はNULLを返す
 */
/* ----------------------------------------------------------------------------------------- */
const JoinFestaPacketEventList::PacketEventInfo* JoinFestaPacketEventList::PopEvent( bool isErase )
{
  if( m_infoList.Size() > 0 )
  {
    for( gfl2::util::ListIterator<PacketEventInfo> it = m_infoList.Begin(); it != m_infoList.End(); )
    {
      PacketEventInfo* info     = &(*it);
      JoinFestaPersonalData* personal = m_personalMgr->GetPersonalData( info->m_friendKey );

      bool isValidEvent = IsValidEventInfo( personal, info );

      // イベントを削除
      if( isErase == true )
      {
        it = m_infoList.Erase( it );
        ICHI_PRINT(">ICHI パケットイベント削除 kind:%d ticketId:%d pid:%d \n", info->m_kind, info->m_ticketId, info->m_friendKey.principalId );
      }

      if( isValidEvent ){
        ICHI_PRINT(">ICHI パケットイベント取得 kind:%d ticketId:%d pid:%d \n", info->m_kind, info->m_ticketId, info->m_friendKey.principalId );
        return info;
      }
    }
  }

  return NULL;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   メンバ変数のリセット
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPacketEventList::Reset()
{
  // リストのクリア
  if( m_infoList.Size() > 0 )
  {
    m_infoList.Erase();
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  指定イベントが有効なイベントか判定
 *
 * @attention イベント発行者のパケット情報が更新されている場合はイベント情報から除外される。
 *
 * @param[in] personal イベント発行者のパーソナルデータ
 * @param[in] eventInf 判定対象イベント情報
 *
 * @return 指定イベントが有効値ならtrue、無効値ならfalseを返却
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPacketEventList::IsValidEventInfo( const NetAppLib::JoinFesta::JoinFestaPersonalData* personal, JoinFestaPacketEventList::PacketEventInfo* eventInf ) const
{
  if( (personal != NULL) && (eventInf->m_kind == personal->GetRequestCommand()) )
  {
    // 対戦
    if( JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_INVITE == eventInf->m_kind )
    {
      if( personal->IsEnablePacketBattleInvite() )
      {
        const PacketBattleInvite& invite = personal->GetPacketBattleInvite();
        return invite.ticketID == eventInf->m_ticketId;
      }
    }
    // 交換
    else if( JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_INVITE == eventInf->m_kind )
    {
      if( personal->IsEnablePacketTradeInvite() )
      {
        const PacketTradeInvite& invite = personal->GetPacketTradeInvite();
        return invite.ticketID == eventInf->m_ticketId;
      }
    }
  }
  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  指定JoinFestaフレンドキーのイベント情報を削除
 *
 * @param[in] friendKey 削除対象のJoinFestaフレンドキー
 *
 * @return 該当イベントが存在していたならtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPacketEventList::EraseEvent( const JoinFestaFriendKey& friendKey )
{
  bool isErase = false;

  if( m_infoList.Size() > 0 )
  {
    for( gfl2::util::ListIterator<PacketEventInfo> it = m_infoList.Begin(); it != m_infoList.End(); )
    {
      if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey( (*it).m_friendKey, friendKey) )
      {
        it = m_infoList.Erase( it );
        isErase = true;
        continue;
      }
      it++;
    }
  }
  return isErase;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   イベント情報が存在するか判定
 *
 * @param   isSimpleCheck 簡易チェックを行うならtrueを指定。
 *                        簡易チェックでは、登録されているイベントが現在有効であるかのチェックを
 *                        行わず、単純にリストに要素が登録されているかのみを判定する。
 *
 * @return  イベント情報が存在するならtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPacketEventList::IsExistEvent( bool isSimpleCheck )
{
  if( isSimpleCheck )
  {
    // 簡易チェック（高速）
    return m_infoList.Size() != 0;
  }
  else
  {
    // 厳密なチェック（低速）
    for( gfl2::util::ListIterator<PacketEventInfo> it = m_infoList.Begin(); it != m_infoList.End(); ++it )
    {
      PacketEventInfo* info = &(*it);
      JoinFestaPersonalData* personal = m_personalMgr->GetPersonalData( info->m_friendKey );

      if( IsValidEventInfo( personal, info ) ){
        return true;
      }
    }
    return false;
  }
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   イベント情報が存在するか判定
 *
 * @param   friendKey イベント発信者のJoinFestaフレンドキー
 *
 * @return  イベント情報が存在するならtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPacketEventList::IsExistEvent( const JoinFestaFriendKey& friendKey )
{
  for( gfl2::util::ListIterator<PacketEventInfo> it = m_infoList.Begin(); it != m_infoList.End(); ++it )
  {
    PacketEventInfo* info = &(*it);
    if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey( friendKey, info->m_friendKey ) )
    {
      JoinFestaPersonalData* personal = m_personalMgr->GetPersonalData( info->m_friendKey );

      return IsValidEventInfo( personal, info );
    }
  }
  return false;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   イベント情報が存在するか判定
 *
 * @return  イベント情報が存在するならtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPacketEventList::IsExistEvent( JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_KIND type )
{
  for( gfl2::util::ListIterator<PacketEventInfo> it = m_infoList.Begin(); it != m_infoList.End(); ++it )
  {
    PacketEventInfo* info = &(*it);
    if( info->m_kind == type )
    {
      JoinFestaPersonalData* personal = m_personalMgr->GetPersonalData( info->m_friendKey );

      if( IsValidEventInfo( personal, info) )
      {
        return true;
      }
    }
  }
  return false;
}




}		// JoinFesta
}		// NetAppLib


