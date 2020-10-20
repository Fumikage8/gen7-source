//============================================================================================
/**
 * @file    JoinFestaPersonalDataManager.h
 * @brief    JoinFestaパーソナルデータ管理情報
 *
 * namespace  NetAppLib::JoinFesta::
 */
//============================================================================================
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"

// ライブラリ
#include <gflnet2/include/friends/gflnet2_FriendManager.h>
#include <util/include/gfl2_Map.h>
#include <heap/include/gfl2_heap_manager.h>

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// niji
#include "system/include/HeapDefine.h"
#include "system/include/Date/gfl2_Date.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"

// JoinFesta
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"
#include "NetApp/JoinFesta/include/JoinFestaDefine.h"
// セーブデータ
#include "Savedata/include/JoinFestaVipSave.h"
#include "Savedata/include/JoinFestaGuestSave.h"
#include "Savedata/include/JoinFestaDataSave.h"

/* ----------------------------------------------------------------------------------------- */
/**
*    Singletonインスタンスの実体を定義
*/
/* ----------------------------------------------------------------------------------------- */
SINGLETON_INSTANCE_DEFINITION(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
template class gfl2::base::SingletonAccessor<NetAppLib::JoinFesta::JoinFestaPersonalDataManager>;

#if PM_DEBUG
// ビーコンバージョンの初期値
u8 NetAppLib::JoinFesta::JoinFestaDefine::JOIN_FESTA_BEACON_VERSION = NetAppLib::JoinFesta::JoinFestaDefine::JOIN_FESTA_BEACON_VERSION_SYMBOL; 
#endif

namespace NetAppLib {
namespace JoinFesta {

#if PM_DEBUG
bool JoinFestaPersonalDataManager::m_isDisplayLockDebugMode; //!< パーソナルデータロック表示デバッグモード有効フラグ
#endif

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief    コンストラクタ
 * @param    heap        内部で使用するヒープ RESIDENT_DEVICE
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaPersonalDataManager::JoinFestaPersonalDataManager( gfl2::heap::HeapBase* heap ) :
  m_my()
  ,m_personalList()
  ,m_isSaveLock(false)
  ,m_pListener(NULL)
  ,m_inviteFriendKey()
  ,m_inviteTicketId(0)
  ,m_fieldPersonalList()
  ,m_pScriptTalkPersonal(NULL)
  ,m_scriptSelectPersonal()
  ,m_recruiterPersonal()
  ,m_lastP2pPersonal()
  ,m_isRecruiterVisible(false)
{

  for( u32 i = 0; i < JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; i++ ){
    m_personalList[i].CreateBuffer( heap, LIST_MAX ); 
  }
  m_fieldPersonalList.CreateBuffer( heap, NetApp::JoinFesta::JoinFestaDefine::JFPOINT_MAX );
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief    デストラクタ
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaPersonalDataManager::~JoinFestaPersonalDataManager(void)
{
  Finalize();
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  初期化処理
 *
 * @result 実行結果
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::Initialize( void )
{
  ReleaseAllList();
  ReleaseFieldPersonalList();
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief  終了処理
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::Finalize( void )
{
  ReleaseAllList();
  ReleaseFieldPersonalList();
}

#if PM_DEBUG
//! ジョインフェスタ人物用ヒープの最大値チェック
void JoinFestaPersonalDataManager::DEBUG_CheckMaxHeap()
{
  ReleaseAllList();
  ReleaseFieldPersonalList();

  // 最大人数確保してみる
  const u32 maxNum = LIST_MAX + LIST_MAX + NetApp::JoinFesta::JoinFestaDefine::JFPOINT_MAX;

  JoinFestaPersonalData* tempArray[ maxNum ];
  gfl2::heap::HeapBase*  heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_JOIN_FESTA_PERSONAL );

  ICHI_PRINT("JoinFestaHeapCheck 前\n");
  for( u32 i=0; i<maxNum; i++ )
  {
    tempArray[i] = GFL_NEW(heap) NetAppLib::JoinFesta::JoinFestaPersonalData();
  }
  //heap->Dump();
  ICHI_PRINT("JoinFestaHeapCheck 後\n");
  for( u32 i=0; i<maxNum; i++ )
  {
    GFL_DELETE tempArray[i];
  }
}
#endif

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   JoinFestaパーソナルデータリスナーの設定
 *
  * @param[in]  listener 登録するリスナー
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::SetPersonalListener( JoinFestaPersonalListener* listener )
{
  m_pListener = listener;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief パーソナルデータリストから線形検索を行う
 *
 * @param friendKey 検索対象フレンドキー
 *
 * @return 発見したパーソナルデータ。見つからない場合はNULLを返却
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaPersonalData* JoinFestaPersonalDataManager::FindFromList( const JoinFestaFriendKey& friendKey )
{
  for( u32 i = 0; i < JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; i++ )
  {
    JoinFestaPersonalDataList *list = GetPersonalDataList( static_cast<JoinFestaDefine::E_JOIN_FESTA_RELATION>(i) );

    GFL_ASSERT( list ); //@fix

    if( list != NULL )
    {
      if( list->Size() > 0)
      {
        for( JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it )
        {
          if( (*it)->IsSameFriendKey( friendKey ) ){
            return (*it);
          }
        }
      }
    }
  }
  return NULL;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   全パーソナルデータリストをリセットする
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::ReleaseAllList( void )
{
  for( u32 i = 0; i < JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; i++ )
  {
    ReleaseList( static_cast<JoinFestaDefine::E_JOIN_FESTA_RELATION>(i) );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルデータをリストとセーブから削除する
 * @note    VIPだった場合、フィールドのデータ（フィールドリスト、リクルーター、P2P）に
 *          同一人物がいる場合ゲストに降格させる
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::RemoveListAndSave( const JoinFestaFriendKey& friendKey )
{
  JoinFestaPersonalData* pPersonalData = GetPersonalData( friendKey );
  if( pPersonalData == NULL ){
    return; // 対象者不明
  }

  // フィールドリストにいた場合でVIPだったらゲストにしておく
  {
    NetAppLib::JoinFesta::JoinFestaPersonalData* pFieldPersonal = GetFieldPersonalData( friendKey );
    if( pFieldPersonal && pFieldPersonal->GetRelation() == JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP )
    {
      pFieldPersonal->SetRelation( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );
    }

    // リクルーター
    if( m_recruiterPersonal.IsEnable() )
    {
      if( m_recruiterPersonal.IsSameFriendKey( friendKey ) &&
          m_recruiterPersonal.GetRelation() == JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP )
      {
        m_recruiterPersonal.SetRelation( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );
      }
    }
    // P2Pで遊んだ人物
    if( m_lastP2pPersonal.IsEnable() )
    {
      if( m_lastP2pPersonal.IsSameFriendKey( friendKey ) &&
          m_lastP2pPersonal.GetRelation() == JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP )
      {
        m_lastP2pPersonal.SetRelation( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );
      }
    }
  }

  // セーブから削除
  {
    if( !IsSavedataUpdatingLock() )
    {
      if( pPersonalData->GetRelation() == JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP )
      {
        Savedata::JoinFestaVip::RemovePersonalFromJoinFestaVipSaveData( friendKey ); // セーブデータから除外
      }
      else
      {
        Savedata::JoinFestaGuest::RemovePersonalFromJoinFestaGuestSaveData( friendKey ); // セーブデータから除外
      }
    }
  }

  // リストから削除
  NetAppLib::JoinFesta::JoinFestaPersonalData* deletePersonal = RemovePersonalDataFromList( pPersonalData->GetRelation(), pPersonalData->GetJoinFestaFriendKey() );
  DeleteJoinFestaPersonalData( deletePersonal );

}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   指定のパーソナルデータリストをリセットする
 *
 * @param   target リセット対象となるリスト
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::ReleaseList( JoinFestaDefine::E_JOIN_FESTA_RELATION target )
{
  JoinFestaPersonalDataList *list = GetPersonalDataList( target );

  GFL_ASSERT( list ); //@fix

  if( (list != NULL) && (list->Size() > 0) )
  {
    for( JoinFestaPersonalListIterator it = list->Begin(); it != list->End();  )
    {
      NetAppLib::JoinFesta::JoinFestaPersonalData* targetPersonal = (*it);
      it = list->Erase(it);

      // 登録されているパーソナルデータは解放してしまう
      DeleteJoinFestaPersonalData( targetPersonal );
    }
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   本体フレンドをリストとセーブから削除する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::RemoveCtrFriend()
{
  for( u32 i = 0; i < JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; i++ )
  {
    JoinFestaPersonalDataList *list = GetPersonalDataList( static_cast<JoinFestaDefine::E_JOIN_FESTA_RELATION>(i) );
    if( list != NULL )
    {
      if( list->Size() > 0)
      {
        for( JoinFestaPersonalListIterator it = list->Begin(); it != list->End();  )
        {
          NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);
          if( pPersonalData->IsCtrFriend() )
          {
            it = list->Erase(it);

            JoinFestaFriendKey friendKey = pPersonalData->GetJoinFestaFriendKey(); 

            // セーブから削除
            {
              if( !IsSavedataUpdatingLock() )
              {
                if( pPersonalData->GetRelation() == JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP )
                {
                  Savedata::JoinFestaVip::RemovePersonalFromJoinFestaVipSaveData( friendKey ); // セーブデータから除外
                }
                else
                {
                  Savedata::JoinFestaGuest::RemovePersonalFromJoinFestaGuestSaveData( friendKey ); // セーブデータから除外
                }
              }
            }

            // フィールドリストにいた場合でVIPだったらゲストにしておく
            {
              NetAppLib::JoinFesta::JoinFestaPersonalData* pFieldPersonal = GetFieldPersonalData( friendKey );
              if( pFieldPersonal && pFieldPersonal->GetRelation() == JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP )
              {
                pFieldPersonal->SetRelation( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );
              }
              // リクルーター
              if( m_recruiterPersonal.IsEnable() )
              {
                if( m_recruiterPersonal.IsSameFriendKey( friendKey ) &&
                  m_recruiterPersonal.GetRelation() == JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP )
                {
                  m_recruiterPersonal.SetRelation( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );
                }
              }
              // P2Pで遊んだ人物
              if( m_lastP2pPersonal.IsEnable() )
              {
                if( m_lastP2pPersonal.IsSameFriendKey( friendKey ) &&
                  m_lastP2pPersonal.GetRelation() == JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP )
                {
                  m_lastP2pPersonal.SetRelation( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );
                }
              }
            }

            // メモリ破棄
            DeleteJoinFestaPersonalData( pPersonalData );
          }
          else
          {
            ++it;
          }
        }
      }
    }
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   JoinFestaフレンドキーと関係性をもとに、パーソナルデータを取得する
 *
 * @param   relation  検索対象となるパーソナルデータ種別
 * @param   friendKey 検索対象となるJoinFestaフレンドキー
 *
 * @return  検出したパーソナルデータへのポインタ。データが存在しない場合はNULLを返却
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaPersonalData* JoinFestaPersonalDataManager::GetPersonalData( JoinFestaDefine::E_JOIN_FESTA_RELATION relation, const JoinFestaFriendKey& friendKey )
{
  JoinFestaPersonalDataList* list = GetPersonalDataList( relation );

  GFL_ASSERT( list ); // @fix

  if( list != NULL )
  {
    for( JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it )
    {
      if( (*it)->IsSameFriendKey(friendKey) ){
        return (*it);
      }
    }
  }
  return NULL;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   JoinFestaフレンドキーをもとに、パーソナルデータを取得する
 *
 * @param   friendKey 検索対象となるJoinFestaフレンドキー
 * @param   kind      検索対象となるパーソナルデータ種別
 *
 * @return  検出したパーソナルデータへのポインタ。データが存在しない場合はNULLを返却
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaPersonalData* JoinFestaPersonalDataManager::GetPersonalData( const JoinFestaFriendKey& friendKey )
{
  // @fix MMCat[34]：【複数の本体とROMカードを入れ替えながらプレイしていると、フェスサークルでのマッチングに失敗する状態が発生する】
  //                
  //                  JoinFestaBeaconJoinSequence.cpp|161行目の全メンバーチェックの際に、自分と同じフレンドキーを持つプレイヤーがいた場合、                
  //                  自分の状態の代わりに、そのプレイヤーの情報を参照してしまっていた。
  //                  類似含め根本解決する為に、自分だったらの条件を最優先に変更。
  //                
  // 自分自身を確認
  if( m_my.IsSameFriendKey(friendKey) ){
    return &m_my;
  }

  for( u32 i = 0; i < JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; i++ )
  {
    JoinFestaPersonalData* personal = GetPersonalData( static_cast<JoinFestaDefine::E_JOIN_FESTA_RELATION>(i), friendKey );
    if( personal != NULL ){
      return personal;
    }
  }

  return NULL;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルデータを登録する(通信用）
 *
 * @attention JoinFestaPersonalDataManager::JoinFestaPersonalDataListが更新されます。
 * @caution イテレータループ内でこの関数をコールしないでください。
 *
 * @param[in]   packet         更新判断に使用されるパケット
 * @param[in]   friendKey      JoinFestaフレンドキー
 * @param[in]   relation       自身との関係
 * @param[in]   isOnline       オンライン状態であればtrueを、オフラインであればfalseを指定
 * @param[in]   localFriendCode  ローカルフレンドコード（ビーコン受信時のみ格納される）
 * @param[out]  isNew          新規登録されたこと通知格納するための領域(NULL指定可能)
 * @param[out]  isUpdated      更新(パケットが保持情報と一致しない）されたことを通知格納するための領域(NULL指定可能)
 * @param[out]  isChangeOnline オフラインからオンラインになったことを通知格納するための領域(NULL指定可能）
 *
 * @return  更新・生成されたパーソナルデータ
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaPersonalData* JoinFestaPersonalDataManager::UpdatePersonalDataWithPacket( const NetAppLib::JoinFesta::JoinFestaPacketData& packet,
                                                                       const JoinFestaFriendKey& friendKey,
                                                                       JoinFestaDefine::E_JOIN_FESTA_RELATION relation,
                                                                       bool isOnline, const u64 localFriendCode,
                                                                       bool* isNew, bool* isUpdated, bool* isChangeOnline )
{
  const PacketPersonalInfo*  personalInfo = &packet.personalInfo;
  JoinFestaPersonalData*     findPersonal = GetPersonalData( relation, friendKey );

  //ICHI_PRINT("UpdatePersonalDataWithPacket[%d][%llu]\n",relation,friendKey.localFriendCode);

  // 新規作成フラグ設定
  if( isNew != NULL ){
    *isNew = (findPersonal==NULL);
  }

  if( packet.header.requestCommand == NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_UPDATE_GAMESTAUS_FIELD )
  {//ジョインフェスタ外なのでオフラインにする
    if( NetLib::NijiNetworkSystem::GetNetworkMode() != NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )//WIFIのときは補正しない
    {
      isOnline = false;
    }
    else
    {
      const_cast<NetAppLib::JoinFesta::JoinFestaPacketData&>(packet).header.requestCommand = NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_UPDATE_GAMESTAUS;//WIFIだったら
    }
  }

  // 登録済みなので更新
  if( findPersonal != NULL )
  {
    if( isUpdated != NULL ){
      *isUpdated = !findPersonal->IsSamePacket( packet );
    }
    if( isChangeOnline != NULL ){
      *isChangeOnline = isOnline && !findPersonal->IsOnline();
    }
    bool isOldOnlineStatus = findPersonal->IsOnline();

    findPersonal->SetPacketData( packet );
    findPersonal->SetOnline( isOnline );
    //findPersonal->SetLocalFriendCode( localFriendCode );

    // 不正名前チェック&置換
    findPersonal->ReplaceName();
    // アッパー対応処理
    findPersonal->UpperPacketCope();
    // momiji追加:バトルフェスパケットデータの補間
    findPersonal->BattleFesPacketInterpolation();

    return findPersonal;
  }
  // 新規追加
  else
  {
    gfl2::heap::HeapBase*   heap     = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_JOIN_FESTA_PERSONAL );
    JoinFestaPersonalData*  personal = GFL_NEW(heap) NetAppLib::JoinFesta::JoinFestaPersonalData( packet );

    GFL_ASSERT( (personal != NULL) ); // @fix

    if( personal != NULL )
    {
      if( isUpdated != NULL ){
        *isUpdated = true;
      }
      if( isChangeOnline != NULL ){
        *isChangeOnline = true;
      }

      // @fix GFNMcat[2356]：データ受信の新規追加時は初期値として必ず更新日時を入れておく
      personal->SetUpdateDate();

      personal->SetOnline( isOnline );
      personal->SetLocalFriendCode( localFriendCode );
      personal->SetRelation( relation );

      // 不正名前チェック&置換
      personal->ReplaceName();
      // アッパー対応処理
      personal->UpperPacketCope();
      // momiji追加:バトルフェスパケットデータの補間
      personal->BattleFesPacketInterpolation();

      PushListWithPersonalData( relation, personal );
    }
    return personal;
  }
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   指定のパーソナルデータをもとに、パーソナルデータを登録する（セーブデータ展開用）
 *
 * @param   srcPersonal  [in]生成元となるパーソナルデータ
 * @param   relation       自身との関係
 *
 * @return  更新・生成されたパーソナルデータ
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaPersonalData* JoinFestaPersonalDataManager::UpdatePersonalDataWithPersonal( JoinFestaDefine::E_JOIN_FESTA_RELATION relation, const NetAppLib::JoinFesta::JoinFestaPersonalData& srcPersonal )
{
  if( !NetAppLib::JoinFesta::JoinFestaPacketUtil::IsValidFriendKey( srcPersonal.GetJoinFestaFriendKey() ) ){
    return NULL;
  }

  JoinFestaPersonalData *findPersonal = GetPersonalData( relation, srcPersonal.GetJoinFestaFriendKey() );

  // 登録済みなので更新
  if( findPersonal != NULL )
  {
    findPersonal->Copy( srcPersonal );
    findPersonal->SetOnline( false );

    // 不正名前チェック&置換
    findPersonal->ReplaceName();
    // アッパー対応処理
    findPersonal->UpperPacketCope();

    return findPersonal;
  }
  // 新規追加
  else
  {
    gfl2::heap::HeapBase*   heap        = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_JOIN_FESTA_PERSONAL );
    JoinFestaPersonalData*  newPersonal = GFL_NEW(heap) NetAppLib::JoinFesta::JoinFestaPersonalData();

    GFL_ASSERT( (newPersonal != NULL) ); // @ fix

    if(newPersonal != NULL)
    {
      newPersonal->Copy( srcPersonal );
      newPersonal->SetOnline( false );
      newPersonal->SetRelation( relation );

      // 不正名前チェック&置換
      newPersonal->ReplaceName();

      // アッパー対応処理
      newPersonal->UpperPacketCope();

      PushListWithPersonalData( relation, newPersonal );
    }
    return newPersonal;
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   指定のパーソナルデータをもとに、パーソナルデータを登録する
 *
 * @param[in]   srcPersonal  生成元となるパーソナルデータ
 *
 * @return  更新・生成されたパーソナルデータ
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaPersonalData* JoinFestaPersonalDataManager::UpdatePersonalDataWithPersonal( const NetAppLib::JoinFesta::JoinFestaPersonalData& srcPersonal )
{
  // 自信との関係性を確定する
  JoinFestaDefine::E_JOIN_FESTA_RELATION  relation;       // 自身との関係
  if( Savedata::JoinFestaVip::IsExistJoinFestaVipSaveData( srcPersonal.GetJoinFestaFriendKey() ) )
  {
    relation = JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP; // VIP
  }
  else
  {
    relation = JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST; // ゲスト
  }

  JoinFestaPersonalData* pPersonal = UpdatePersonalDataWithPersonal( relation, srcPersonal );

  // 直接登録の場合は必ずONLINEにする
  if( pPersonal )
  {
    pPersonal->SetOnline( true );
  }

  return pPersonal;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief パーソナルデータの関係性変更に伴うリストの更新処理
 *
 * @param olsRelation 更新前の関係性
 * @param newRelation 更新後の関係性
 * @param personal    対象JoinFestaパーソナルデータ
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::ChangeListWithPersonalData( JoinFestaDefine::E_JOIN_FESTA_RELATION oldRelation,
                                                         JoinFestaDefine::E_JOIN_FESTA_RELATION newRelation,
                                                         JoinFestaPersonalData* personal )
{
  if( personal != NULL )
  {
    ICHI_PRINT( "ChangeListWithPersonalData[%u][%u][%u][%llu]\n", oldRelation, newRelation, \
                                                                personal->GetJoinFestaFriendKey().principalId, \
                                                                personal->GetJoinFestaFriendKey().localFriendCode );

    RemovePersonalDataFromList( oldRelation, personal->GetJoinFestaFriendKey() );
    PushListWithPersonalData( newRelation, personal );
  }
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief パーソナルデータをリストに追加する
 *
 * @param relation 関係性
 * @param personal パーソナルデータへのポインタ
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::PushListWithPersonalData(  JoinFestaDefine::E_JOIN_FESTA_RELATION relation, JoinFestaPersonalData* personal )
{
  JoinFestaPersonalDataManager::JoinFestaPersonalDataList* relationList = GetPersonalDataList( relation );
  if( (relationList == NULL) || (personal==NULL) ){
    GFL_ASSERT(0);  // @check
    return;
  }

  // 定員オーバーの場合はリストから削除する
  DeleteOverPersonalData( relation );

  // 関係性リストに追加
  relationList->PushBack( personal );
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルデータリストを取得する
 *
 * @param   kind 取得するリスト種別
 *
 * @return  パーソナルデータリストへのポインタ
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaPersonalDataManager::JoinFestaPersonalDataList* JoinFestaPersonalDataManager::GetPersonalDataList( JoinFestaDefine::E_JOIN_FESTA_RELATION kind )
{
  GFL_ASSERT( kind < JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX ); // @fix

  if( kind >= JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX ){
    return NULL;
  }

  return &m_personalList[kind];
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   自身のパーソナルデータを取得する
 *
 * @return  自身のパーソナルデータ
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaPersonalData* JoinFestaPersonalDataManager::GetMyData( void )
{
  return &m_my;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルデータの削除を禁止する
 *
 * @param   friendKey 設定対象のJoinFestaフレンドキー
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::LockPersonalData( const JoinFestaFriendKey& friendKey )
{
  for( u32 i = 0; i < JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; i++ )
  {
    JoinFestaPersonalData* target = GetPersonalData( static_cast<JoinFestaDefine::E_JOIN_FESTA_RELATION>(i), friendKey );
    if( target != NULL ){
      target->Lock();
    }
  }
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルデータの削除禁止設定を解除する
 *
 * @param   friendKey 設定対象のJoinFestaフレンドキー
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::UnLockPersonalData( const JoinFestaFriendKey& friendKey )
{
  for( u32 i = 0; i < JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; i++ )
  {
    JoinFestaPersonalData* target = GetPersonalData( static_cast<JoinFestaDefine::E_JOIN_FESTA_RELATION>(i), friendKey );
    if( target != NULL ){
      target->UnLock();
    }
  }
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   全パーソナルデータの削除禁止設定を解除する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::UnLockPersonalData( void )
{
  for( u32 i = 0; i < JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; i++ )
  {
    JoinFestaPersonalDataList *list = GetPersonalDataList( static_cast<JoinFestaDefine::E_JOIN_FESTA_RELATION>(i) );

    GFL_ASSERT( list ); // @fix

    if( list != NULL )
    {
      if( list->Size() > 0)
      {
        for( JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it ){
          (*it)->UnLock();
        }
      }
    }
  }
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルデータリストからあふれるデータを削除する
 *
 * @param   relation  削除対象のリスト種別
 *
 * @return  削除対象者が存在したらtrueを返却する
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalDataManager::DeleteOverPersonalData( JoinFestaDefine::E_JOIN_FESTA_RELATION relation )
{
  JoinFestaPersonalDataList *list = GetPersonalDataList( relation );

  if( list )
  {
    if( list->Size() >= LIST_MAX )
    {
      for( JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it )
      {
        if( !(*it)->IsLock() )
        {
          JoinFestaPersonalData* target = (*it);
          list->Erase( it );

          // リスナー通知
          if( m_pListener != NULL )
          {
            m_pListener->OnDeleteJoinFestaPerson( *target );
          }

          DeleteJoinFestaPersonalData( target );
          return true;
        }
      }
      // ロックの解除漏れで誰も削除できなかった
      GFL_ASSERT(0);  // @check
    }
  }
  return false;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルデータが未登録か判定
 *
 * @param    kind    判定対象のリスト種別
 *
 * @return  1件もパーソナルデータが未登録ならtrueを返却する
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalDataManager::IsEmpty( JoinFestaDefine::E_JOIN_FESTA_RELATION kind ) const
{
  return (m_personalList[kind].Size() == 0);
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルデータが未登録か判定
 *
 * @return  「VIP」「ゲスト」全てが未登録ならtrueを返却する
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalDataManager::IsEmpty( void ) const
{
  return IsEmpty( JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP ) && IsEmpty( JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルデータを最大登録しているか判定
 *
 * @param[in]    relation    判定対象のリスト種別
 *
 * @return  パーソナルデータが登録MAXならtrueを返却する
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalDataManager::IsFull( JoinFestaDefine::E_JOIN_FESTA_RELATION relation ) const
{
  return (m_personalList[relation].Size() == LIST_MAX);
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルデータの登録数を取得
 *
 * @param[in]    relation    判定対象のリスト種別
 *
 * @return  登録数
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaPersonalDataManager::GetCount( JoinFestaDefine::E_JOIN_FESTA_RELATION relation ) const
{
  return m_personalList[relation].Size();
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   オンラインのパーソナルデータが存在するか判定
 *
 * @param    kind    判定対象のリスト種別
 *
 * @return  オンラインユーザーが存在するならtrueを返却する
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalDataManager::IsExistOnlineUser( JoinFestaDefine::E_JOIN_FESTA_RELATION kind )
{
  JoinFestaPersonalDataList *list = GetPersonalDataList( static_cast<JoinFestaDefine::E_JOIN_FESTA_RELATION>(kind) );

  GFL_ASSERT( list ); // @fix

  if( list != NULL )
  {
    if( list->Size() > 0)
    {
      for( JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it )
      {
        if( (*it)->IsOnline() ){
          return true;
        }
      }
    }
  }
  return false;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   オンラインのパーソナルデータが存在するか判定
 *
 * @param    kind    判定対象のリスト種別
 *
 * @return  「VIP」「ゲスト」で１人でもオンラインユーザーが存在するならtrueを返却する
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalDataManager::IsExistOnlineUser( void )
{
  return IsExistOnlineUser( JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP ) ||
         IsExistOnlineUser( JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション開催中のパーソナルデータが存在するか判定
 *
 * @return  「VIP」「Guest」で１人でも開催中ユーザーが存在するならtrueを返却する
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalDataManager::IsExistOpenAttractionUser( void )
{
  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();
  // アトラクション開放済みでないと、アトラクションの情報は全てOFF
  if( pSave->GetRewardState( JoinFestaScript::REWARD_ATTRACTION ) != JoinFestaScript::REWARD_STATE_FINISH )
  {
    return false;
  }

  for( u32 i = 0; i < JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; i++ )
  {
    JoinFestaPersonalDataList *list = GetPersonalDataList( static_cast<JoinFestaDefine::E_JOIN_FESTA_RELATION>(i) );

    GFL_ASSERT( list ); // @fix

    if( list != NULL )
    {
      if( list->Size() > 0)
      {
        for( JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it )
        {
          if( (*it)->IsOnline() == true )
          {
            // アトラクション参加可能かチェック
            if( NetAppLib::JoinFesta::JoinFestaPacketUtil::CheckAttractionJoinEnable( *it ) )
            {
              return true;
            }
          }
        }
      }
    }
  }
  return false;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief ペナルティタイム発動による更新（RTC改ざん検知時）
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::PenaltyTimeUpdate( void )
{
  for( u32 i = 0; i < JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; i++ )
  {
    JoinFestaPersonalDataList *list = GetPersonalDataList( static_cast<JoinFestaDefine::E_JOIN_FESTA_RELATION>(i) );

    GFL_ASSERT( list ); // @fix

    if( list != NULL )
    {
      if( list->Size() > 0)
      {
        for( JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it ){
          (*it)->SetPenaltyTime();
        }
      }
    }
  }

  // @fix NMCat[3321]：セーブデータも更新する
  JoinFestaPacketUtil::UpdateJoinFestaPersonalSaveData();
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief 日付更新におけるリセット処理
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::ResetWithDaytime( void )
{
  for( u32 i = 0; i < JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; i++ )
  {
    JoinFestaPersonalDataList *list = GetPersonalDataList( static_cast<JoinFestaDefine::E_JOIN_FESTA_RELATION>(i) );

    GFL_ASSERT( list ); // @fix

    if( list != NULL )
    {
      if( list->Size() > 0)
      {
        for( JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it ){
          (*it)->ResetWithDaytime();
        }
      }
    }
  }
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルデータ種別の変更
 *
 * @attention JoinFestaPersonalDataManager::JoinFestaPersonalDataListが更新されます。
 * @caution イテレータループ内でこの関数をコールしないでください。
 *
 * @note    セーブデータ領域「友達リスト」「VIPリスト」を更新する
 *
 * @param   targetKind  変更目標種別
 * @param   friendKey   変更対象ユーザーのJoinFestaフレンドキー
 *
 * @return  変更が行われたならtrueを指定
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalDataManager::ChangePersonalRelation( JoinFestaDefine::E_JOIN_FESTA_RELATION targetKind, const JoinFestaFriendKey& friendKey )
{
  GFL_ASSERT( (targetKind<JoinFestaDefine::E_JOIN_FESTA_RELATION_ANOTHER_MAX) ); // @fix

  if( targetKind >= JoinFestaDefine::E_JOIN_FESTA_RELATION_ANOTHER_MAX ){
    return false; // 不正な引数
  }

  if( m_my.IsSameFriendKey( friendKey ) ){
    return false; // 自分は変更できない
  }

  JoinFestaPersonalData* personal = GetPersonalData( friendKey );
  if( personal == NULL ){
    return false; // 対象者不明
  }

  const JoinFestaDefine::E_JOIN_FESTA_RELATION oldKind = personal->GetRelation();
  if( oldKind == targetKind ){
    return false; // 種別に変更なし
  }
  else if( oldKind == JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST )
  {
    if( !IsSavedataUpdatingLock() ){
      Savedata::JoinFestaGuest::RemovePersonalFromJoinFestaGuestSaveData( friendKey ); // セーブデータから除外
    }
  }
  else if( oldKind == JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP )
  {
    if( !IsSavedataUpdatingLock() ){
      Savedata::JoinFestaVip::RemovePersonalFromJoinFestaVipSaveData( friendKey ); // セーブデータから除外
    }
  }

  // 関係情報を更新
  personal->SetRelation( targetKind );

  if( targetKind == JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST )
  {
    if( !IsSavedataUpdatingLock() ){
      Savedata::JoinFestaGuest::AddPersonalToJoinFestaGuestSaveData( friendKey );
    }
  }
  else if( targetKind == JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP )
  {
    if( !IsSavedataUpdatingLock() ){
      Savedata::JoinFestaVip::AddPersonalToJoinFestaVipSaveData( friendKey );
    }
  }

  // 関係性を更新する
  ChangeListWithPersonalData( oldKind, targetKind, personal );

  ICHI_PRINT(">ICHI 関係性更新成功!");

  return true;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルデータリストから指定ユーザーを削除する
 *
 * @param   targetKind  削除対象種別
 * @param   friendKey   削除対象ユーザーのJoinFestaフレンドキー
 *
 * @note    該当ユーザーデータをリストから削除する
 *          ただしヒープメモリからは解放しないので、返却されたポインタを
 *          GFL_DELETEにて解放すること。
 *
 * @return  リストから削除されたパーソナルデータへのポインタ
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaPersonalData* JoinFestaPersonalDataManager::RemovePersonalDataFromList( JoinFestaDefine::E_JOIN_FESTA_RELATION targetKind, const JoinFestaFriendKey& friendKey )
{
  JoinFestaPersonalDataList *list = GetPersonalDataList( targetKind );

  if( list )
  {
    if( list->Size() > 0 )
    {
      for( JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it )
      {
        if( (*it)->IsSameFriendKey( friendKey ) )
        {
          JoinFestaPersonalData* personal = (*it);
          list->Erase( it );
          return personal;
        }
      }
    }
  }

  return NULL;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief JoinFestaPersonalDataの削除
 *
 * @param personal メモリから解放する対象となるJoinFestaPersonalData
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::DeleteJoinFestaPersonalData( NetAppLib::JoinFesta::JoinFestaPersonalData* personal )
{
  if( personal != NULL )
  {
    GFL_SAFE_DELETE( personal );
  }
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief セーブデータ領域の更新をロックする
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::LockSavedataUpdating( void )
{
  m_isSaveLock = true;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief セーブデータ領域の更新をアンロックする
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::UnlockSavedataUpdating( void )
{
  m_isSaveLock = false;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief セーブデータ領域の更新ロック状態を判定
 *
 * @return セーブデータ領域への更新がロックされているならtrueを指定
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalDataManager::IsSavedataUpdatingLock( void ) const
{
  return m_isSaveLock;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   お誘いして来た人のフレンドキーを設定
 *
 * @param[in]   friendKey お誘いして来た人のJoinFestaフレンドキー
 * @param[in]   ticketId  お誘いパケットのチケットID
 *
 * @caution 下画面のお誘いOKボタンからのみ呼び出す事を想定しています。
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::SetInviteFriendKey( const JoinFestaFriendKey& friendKey, const u32 ticketId ) 
{
  GFL_PRINT("SetInviteFriendKey[%u][%llu]\n", friendKey.principalId, friendKey.localFriendCode );
  GFL_PRINT("SetInviteTicketID[%u]\n", ticketId );
  m_inviteFriendKey = friendKey;
  m_inviteTicketId = ticketId;

  JoinFestaPersonalData* personal = GetPersonalData( m_inviteFriendKey );
  if( personal )
  {
    personal->Lock();
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   お誘い情報をリセット
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::ResetInviteFriendKey()
{
  // お誘いを受けた段階でロックしているのでアンロックしておく
  JoinFestaPersonalData* personal = GetPersonalData( m_inviteFriendKey );
  if( personal )
  {
    personal->UnLock();
  }

  m_inviteFriendKey.initialize();
  m_inviteTicketId = 0;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   お誘いして来た人のフレンドキーを取得
 *
 * @return  friendKey お誘いして来た人のJoinFestaフレンドキー
 *
 * @caution ビーコンマッチングアプリからのみ呼び出す事を想定しています。
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaFriendKey JoinFestaPersonalDataManager::GetInviteFriendKey() const 
{
  return m_inviteFriendKey;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   お誘いパケットのチケットIDを取得
 *
 * @return  ticketId お誘いパケットのチケットID
 *
 * @caution ビーコンマッチングアプリからのみ呼び出す事を想定しています。
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaPersonalDataManager::GetInviteTicketId() const 
{
  return m_inviteTicketId;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief 全パーソナルデータでパーソナルイベントのリセット
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::ResetPersonalEvent( void )
{
  for( u32 i = 0; i < JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; i++ )
  {
    ResetPersonalEvent( static_cast<JoinFestaDefine::E_JOIN_FESTA_RELATION>(i) );
  }
}
void JoinFestaPersonalDataManager::ResetPersonalEvent( JoinFestaDefine::E_JOIN_FESTA_RELATION relation )
{
  JoinFestaPersonalDataList *list = GetPersonalDataList( relation );

  GFL_ASSERT( list ); // @fix

  if( list != NULL )
  {
    if( list->Size() > 0)
    {
      for( JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it )
      {
        // 発動しているものだけリセット
        if( (*it)->IsPersonalEventEnable() )
        {
          (*it)->ResetPersonalEvent();
        }
        // スクリプトの選択フラグも同じタイミングでリセットする
        (*it)->SetScriptListSelected( false );
      }
    }
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief 全パーソナルデータでアトラクションデータのリセット
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::ResetAttraction()
{
  for( u32 i = 0; i < JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; i++ )
  {
    JoinFestaPersonalDataList *list = GetPersonalDataList( static_cast<JoinFestaDefine::E_JOIN_FESTA_RELATION>(i) );

    GFL_ASSERT( list ); // @fix

    if( list != NULL )
    {
      if( list->Size() > 0)
      {
        for( JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it )
        {
          // アトラクションデータリセット
          (*it)->ResetAttractionData();
        }
      }
    }
  }

  // フィード出現用もリセットする。（アトラクション中はこちらをいじっている）
  for( JoinFestaPersonalListIterator it = m_fieldPersonalList.Begin(); it != m_fieldPersonalList.End(); ++it )
  {
    // アトラクションデータリセット
    (*it)->ResetAttractionData();
  }
}

//=============================================================================================
//=============================================================================================
// フィールドに出現するパーソナルデータリスト操作系
//=============================================================================================
//=============================================================================================
/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   出現する人物リスト作成
 * @li      呼ぶ箇所は以下２つ
 *          １．フィールドからジョインフェスタに来た際
 *          ２．アトラクション終了時
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::CreateFieldPersonalList()
{
  GFL_PRINT(">JF CreateFieldPersonalList\n");

  //-----------------------
  // 受信が新しい順に32人ピックアップする。
  // 32人に足りない場合は、足りないままで問題無い。
  //-----------------------

  // 既存のリストは解放
  ReleaseFieldPersonalList();


  // 受信からの経過ミリ秒でソートする為のリスト
  JoinFestaPersonalData* timeSortList[ JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX ][ LIST_MAX ];
  u32                    timeSortListCount[ JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX ] = { 0, 0 };

  // 登録されているパーソナルを取得
  for( u32 i = 0; i < JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; i++ )
  {
    JoinFestaPersonalDataList *list = GetPersonalDataList( static_cast<JoinFestaDefine::E_JOIN_FESTA_RELATION>(i) );
    if( list != NULL && list->Size() > 0 )
    {
      for( JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it )
      {
        // ソートリストに登録
        timeSortList[i][ timeSortListCount[i]++ ] = (*it);
      }
    }
  }
  ICHI_PRINT(">ICHI VIP要素数 %d \n", timeSortListCount[0] );
  ICHI_PRINT(">ICHI GUEST要素数 %d \n", timeSortListCount[1] );

  // 更新日時順にソート
  for( u32 relation = 0; relation < JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; relation++ )
  {
    JoinFestaPacketUtil::SortByUpdateDate( timeSortList[relation], timeSortListCount[relation] );
  }

  // 先にVIPを優先して入れる
  for( u32 i=0; i<timeSortListCount[ JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP ]; i++ )
  {
    // パーソナルデータ作成して追加
    AddFieldPersonalList( *(timeSortList[JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP][i]) );

    // 10人入れた時点で終了
    if( m_fieldPersonalList.Size() == 10 )
    {
      break;
    }
  }
  // VIP入れた分だけリストから削除
  if( m_fieldPersonalList.Size() > 0 )
  {
    for( u32 delCount=0; delCount<m_fieldPersonalList.Size(); delCount++ )
    {
      for( u32 i=0; i<timeSortListCount[ JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP ] - 1; i++ )
      {
        timeSortList[JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP][i] = timeSortList[JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP][i+1]; 
      }
      timeSortListCount[ JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP ]--;
    }
  }


  ICHI_PRINT(">ICHI VIPで優先して入れた数 %d \n", m_fieldPersonalList.Size() );

  // リレーション関係なく経過時間昇順で入れる
  {
    JoinFestaPersonalData* lastSortList[ JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX * LIST_MAX ];
    u32                    lastSortListCount = 0;

    // リレーション別のリストを１つにまとめる
    for( u32 relation = 0; relation < JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; relation++ )
    {
      for( u32 i=0; i<timeSortListCount[relation]; i++ )
      {
        lastSortList[ lastSortListCount++ ] = timeSortList[relation][i];
      }
    }

    // 更新日時デソート
    JoinFestaPacketUtil::SortByUpdateDate( lastSortList, lastSortListCount );
    ICHI_PRINT(">ICHI 最終候補リスト数 %d \n", lastSortListCount);

    for( u32 i=0; i<lastSortListCount; i++ )
    {
      // パーソナルデータ作成して追加
      AddFieldPersonalList( *(lastSortList[i]) );

      // 32人入れた時点で終了
      if( m_fieldPersonalList.Size() == m_fieldPersonalList.MaxSize() )
      {
        break;
      }
    }
  }

  // @fix NMCat[1152]：フィールド用リストを作るこのタイミングに移動
  // D要望：ぼっち対策の為にプリセット出現させる
  //    @fix NMCat[2758]：リストではなく、セーブの人数を見る
  if( Savedata::JoinFestaGuest::IsEmptyJoinFestaGuestSaveData() && Savedata::JoinFestaVip::IsEmptyJoinFestaVipSaveData() )
  {
    if( m_fieldPersonalList.Size() <= NetApp::JoinFesta::JoinFestaDefine::PRESET_OPEN_NUM )
    {
      Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

      for( int i=0; i<NetApp::JoinFesta::JoinFestaDefine::PRESET_ID_MAX; i++ )
      {
        // 出現条件にフェスランクがある
        if( i>=8 && pJoinFestaDataSave->GetRank() < 5 )
        {
          continue;
        }
        else if( i>=4 && pJoinFestaDataSave->GetRank() < 3 )
        {
          continue;
        }

        NetAppLib::JoinFesta::JoinFestaPersonalData dummyData;
        // プリセットNPCデータ
        dummyData.SetupPresetNpcData( i );

        // 常駐の出現リストの方に追加
        AddFieldPersonalList( dummyData );
      }
    }
  }


#if PM_DEBUG
  {
    gfl2::str::STRCODE name[ ::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN ];
    GFL_PRINT("--JFFieldList---------------------\n");
    for( JoinFestaPersonalListIterator it = m_fieldPersonalList.Begin(); it != m_fieldPersonalList.End(); ++it )
    {
      gfl2::str::PrintCode( (*it)->GetName(), "name:" );
    }
    GFL_PRINT("----------------------------------\n");
  }
#endif
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   フィールド出現用の人物リスト解放
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::ReleaseFieldPersonalList()
{
  JoinFestaPersonalDataList *list = &m_fieldPersonalList;

  if( list->Size() > 0 )
  {
    for( JoinFestaPersonalListIterator it = list->Begin(); it != list->End();  )
    {
      JoinFestaPersonalData* targetPersonal = (*it);
      it = list->Erase(it);

      // 登録されているパーソナルデータは解放してしまう
      DeleteJoinFestaPersonalData( targetPersonal );
    }
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   フィールド出現用の人物リストに指定したパーソナル追加
 * @param[in]   srcPersonal  生成元となるパーソナルデータ
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaPersonalData* JoinFestaPersonalDataManager::AddFieldPersonalList( const NetAppLib::JoinFesta::JoinFestaPersonalData& srcPersonal )
{
  // リストに空がないと終了
  if( m_fieldPersonalList.Size() == m_fieldPersonalList.MaxSize() )
  {
    GFL_ASSERT(0);
    return NULL;
  }

  // アトラクションで作ったダミーデータの場合はフレンドキーないのでチェックしない
  // プリセットNPCの場合もチェックしない
  if( srcPersonal.IsAttractionDummyData() == false && srcPersonal.IsPresetNpcData() == false )
  {
    // フレンドキーが無効な場合は終了
    if( !NetAppLib::JoinFesta::JoinFestaPacketUtil::IsValidFriendKey( srcPersonal.GetJoinFestaFriendKey() ) ){
      GFL_ASSERT(0);
      return NULL;
    }
  }

  //JoinFestaPersonalData *findPersonal = GetFieldPersonalData( srcPersonal.GetJoinFestaFriendKey() );

  //// 登録済みなので更新
  //if( findPersonal != NULL )
  //{
  //  findPersonal->Copy( srcPersonal );
  //  return findPersonal;
  //}
  // 新規追加
  {
    gfl2::heap::HeapBase*   heap        = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_JOIN_FESTA_PERSONAL );
    JoinFestaPersonalData*  newPersonal = GFL_NEW(heap) NetAppLib::JoinFesta::JoinFestaPersonalData();

    GFL_ASSERT( (newPersonal != NULL) ); // @ fix

    if(newPersonal != NULL)
    {
      newPersonal->Copy( srcPersonal );
      m_fieldPersonalList.PushBack( newPersonal );
      GFL_PRINT(">JF フィールド出現用リスト更新 size%d \n", m_fieldPersonalList.Size());
    }
    return newPersonal;
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   フィールド出現用パーソナルデータリストを取得する
 *
 * @return  パーソナルデータリストへのポインタ
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaPersonalDataManager::JoinFestaPersonalDataList* JoinFestaPersonalDataManager::GetFieldPersonalList()
{
  return &m_fieldPersonalList;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   JoinFestaフレンドキーをもとに、フィールド出現用パーソナルデータを取得する
 *
 * @param[in]  friendKey 検索対象となるJoinFestaフレンドキー
 *
 * @return  検出したパーソナルデータへのポインタ。データが存在しない場合はNULLを返却
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaPersonalData* JoinFestaPersonalDataManager::GetFieldPersonalData( const JoinFestaFriendKey& friendKey )
{
  for( JoinFestaPersonalListIterator it = m_fieldPersonalList.Begin(); it != m_fieldPersonalList.End(); ++it )
  {
    if( (*it)->IsSameFriendKey(friendKey) ){
      return (*it);
    }
  }
  return NULL;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   スクリプトで話しかけた人のフレンドキーを設定
 *
 * @param[in]   friendKey スクリプトで話しかけた人のJoinFestaフレンドキー
 *
 * @attention 話しかけスクリプト中にアプリ遷移などでフィールド破棄しなければいけない時の退避用
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::SetScriptTalkFriendKey( NetAppLib::JoinFesta::JoinFestaPersonalData *pPersonalData )
{
  GFL_PRINT("SetScriptTalkFriendKey\n");
  m_pScriptTalkPersonal = pPersonalData;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   スクリプトで話しかけた人のフレンドキーをリセット
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::ResetScriptTalkFriendKey()
{
  GFL_PRINT("ResetScriptTalkFriendKey\n");
  m_pScriptTalkPersonal = NULL;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   スクリプトで話しかけた人のパーソナルデータを取得する
 * @return  パーソナルデータへのポインタ。データが存在しない場合はNULLを返却
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaPersonalData* JoinFestaPersonalDataManager::GetScriptTalkFieldPersonalData()
{
  return m_pScriptTalkPersonal;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   スクリプトでリストから選んだ人をセット
 * @param[in]   friendKey スクリプトで選んだ人のパーソナルデータ
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::SetScriptSelectPersonal( JoinFestaPersonalData* pJoinFestaPersonalData )
{
  GFL_PRINT("SetScriptSelectPersonal\n");
  GFL_ASSERT( pJoinFestaPersonalData );
  if( pJoinFestaPersonalData != NULL )
  {
    m_scriptSelectPersonal.Copy( *pJoinFestaPersonalData );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   スクリプトでリストから選んだ人をリセット
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::ResetScriptSelectPersonal()
{
  GFL_PRINT("ResetScriptSelectPersonal\n");
  m_scriptSelectPersonal.Reset();
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   スクリプトでリストから選んだ人のパーソナルデータを取得する
 * @return  パーソナルデータへのポインタ。データが存在しない場合はNULLを返却
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaPersonalData* JoinFestaPersonalDataManager::GetScriptSelectPersonal()
{
  return &m_scriptSelectPersonal;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   リクルーターで呼んだ人をセット
 * @param[in]   friendKey スクリプトで選んだ人のJoinFestaフレンドキー
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::SetRecruiterPersonal( JoinFestaPersonalData* pJoinFestaPersonalData )
{
  GFL_PRINT("SetRecruiterPersonal\n");
  GFL_ASSERT( pJoinFestaPersonalData );
  if( pJoinFestaPersonalData != NULL )
  {
    m_recruiterPersonal.Copy( *pJoinFestaPersonalData );
    // セット時はリクルーター非表示
    m_isRecruiterVisible = false;
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   リクルーターで呼んだ人をリセット
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::ResetRecruiterPersonal()
{
  GFL_PRINT("ResetRecruiterPersonal\n");
  m_recruiterPersonal.Reset();
  // リセット時はリクルーター非表示
  m_isRecruiterVisible = false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   リクルーターで呼んだ人のパーソナルデータを取得する
 * @return  パーソナルデータへのポインタ。データが存在しない場合はNULLを返却
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaPersonalData& JoinFestaPersonalDataManager::GetRecruiterPersonal()
{
  return m_recruiterPersonal;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   リクルーターで呼んだ人を表示状態にする
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::SetRecruiterVisibleOn()
{
  ICHI_PRINT(">ICHI SetRecruiterVisibleOn\n");
  m_isRecruiterVisible = true;
}
  
/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   リクルーターで呼んだ人を表示状態を取得する
 * @return  true:表示 false:非表示
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalDataManager::IsRecruiterVisible() const
{
  return m_isRecruiterVisible;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   P2Pで最後に遊んだ人をセット
 * @param[in]   personalData パーソナルデータ
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::SetLastP2pPersonal( NetAppLib::JoinFesta::JoinFestaPersonalData& personalData )
{
  GFL_PRINT("SetLastP2pPersonal\n");

  // @fix GFNMCat[1622]：P2Pで受け取ったPersonalDataだとSubDataがないので、ここで最新データ引っ張ってきてそれを使う。
  JoinFestaPersonalData* pPersonal = GetPersonalData( personalData.GetJoinFestaFriendKey() );
  if( pPersonal != NULL )
  {
    // 関係性を上書き
    personalData.SetRelation( pPersonal->GetRelation() );
  }

  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();
  pSave->SetLastP2pPersonal( personalData );

  // @fix GFNMCat[1637]：リクルーターと同一人物の場合リクルーターは消す
  if( personalData.IsSameFriendKey( GetRecruiterPersonal().GetJoinFestaFriendKey() ) )
  {
    ResetRecruiterPersonal();
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   P2Pで最後に遊んだ人の専用イベント終了
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataManager::FinishLastP2pPersonalEvent()
{
  GFL_PRINT("FinishLastP2pPersonalEvent\n");
  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();
  pSave->FinishLastP2pPersonalEvent();
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   P2Pで最後に遊んだ人のパーソナルデータを取得する
 * @return  パーソナルデータコアデータ
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaPersonalData& JoinFestaPersonalDataManager::GetLastP2pPersonal()
{
  return m_lastP2pPersonal;
}


}    // JoinFesta
}    // NetAppLib



