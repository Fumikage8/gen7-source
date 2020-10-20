//============================================================================================
/**
 * @file    JoinFestaPacketUtil.cpp
 *
 * @brief    JoinFestaパケット制御用Utilityクラス
 *          static関数にて機能を提供する。
 */
//============================================================================================
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"

// ライブラリ
#include <gflnet2/include/friends/gflnet2_FriendManager.h>
#include <util/include/gfl2_std_string.h>
#include "system/include/Date/gfl2_Date.h"

// niji
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// JoinFesta
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketEventList.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaInformationMessage.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"

// save
#include "Savedata/include/JoinFestaDataSave.h"
#include "Savedata/include/EventWork.h"

namespace NetAppLib {
namespace JoinFesta {

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief    コンストラクタ(隠蔽化)
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaPacketUtil::JoinFestaPacketUtil( void )
{
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief    デストラクタ(隠蔽化)
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaPacketUtil::~JoinFestaPacketUtil(void)
{
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   通信アクションを履歴情報に登録する
 *
 * @note    savedata::ProfileHistory, 自身のJoinFestaPersonalDataが更新される
 *
 * @param[in]  action 履歴情報に登録する通信アクション
 */
/* ----------------------------------------------------------------------------------------- */
// @static_function
//void JoinFestaPacketUtil::RegistProfileHistory( savedata::ProfileHistory::E_HISTORY_ACTION action )
//{
//  GameSys::GameManager*      gameMgr    = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
//  GameSys::GameData*         gameDat    = gameMgr->GetGameData();
//  savedata::ProfileHistory*     history    = gameDat->GetProfileHistory();
//  NetAppLib::JoinFesta::JoinFestaPersonalData* myPersonal = gameMgr->GetJoinFestaPersonalDataManager()->GetMyData();
//
//  history->PushProfileHistory( action );                   // 履歴登録
//  myPersonal->SetPacketGameStatusWithProfile( gameDat );   // プロフィールステータスを更新
//}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   nn::nex::SubscriptionDataからNetAppLib::JoinFesta::JoinFestaPacketData情報を抜出し格納
 *
 * @param[in]   src 入力元nn::nex::SubscriptionDataへの参照
 * @param[out]  dst 出力先NetAppLib::JoinFesta::JoinFestaPacketDataへのポインタ
 *
 * @return  取得に成功したならtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
// @static_function
#if defined(GF_PLATFORM_CTR)
bool JoinFestaPacketUtil::GetPacketDataFromSubscriptionData( nn::nex::SubscriptionData& src, NetAppLib::JoinFesta::JoinFestaPacketData* dst )
{
  nn::nex::qVector<qByte> buffer;
  src.GetApplicationBuffer( &buffer );

  return GetPacketDataFromSubscriptionData( buffer, dst );
}
#endif

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   nn::nex::SubscriptionDataからNetAppLib::JoinFesta::JoinFestaPacketData情報を抜出し格納
 *
 * @param[in]   src nn::nex::SubscriptionData::GetApplicationBuffer()で取得したバッファ
 * @param[out]  dst 出力先NetAppLib::JoinFesta::JoinFestaPacketDataへのポインタ
 *
 * @return  取得に成功したならtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
// @static_function
#if defined(GF_PLATFORM_CTR)
bool JoinFestaPacketUtil::GetPacketDataFromSubscriptionData( const nn::nex::qVector<nn::nex::qByte>& src, NetAppLib::JoinFesta::JoinFestaPacketData* dst )
{
  size_t packetSize = sizeof(NetAppLib::JoinFesta::JoinFestaPacketData);

  gfl2::std::MemCopy( &src[0], dst, packetSize );

  return true;
}
#endif // GF_PLATFORM_CTR


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   指定のJoinFestaフレンドキーが一致するか判定
 *
 * @param[in]   friendKey1 比較対象となるJoinFestaフレンドキー1
 * @param[in]   friendKey2 比較対象となるJoinFestaフレンドキー2
 *
 * @return  指定JoinFestaフレンドキーが一致するならtrue
 */
/* ----------------------------------------------------------------------------------------- */
// @static_function
bool JoinFestaPacketUtil::IsSameFriendKey( const JoinFestaFriendKey& friendKey1, const JoinFestaFriendKey& friendKey2 )
{
  if( friendKey1.principalId==friendKey2.principalId )
  {
    // 無効なIDの場合は、一致条件として処理せず
    if( friendKey1.principalId != NN_FRIENDS_INVALID_PRINCIPAL_ID ){
      return true;
    }
  }

  if(friendKey1.localFriendCode==friendKey2.localFriendCode)
  {
    // 無効なIDの場合は、一致条件として処理せず
    if( friendKey1.localFriendCode != NN_FRIENDS_INVALID_LOCAL_FRIEND_CODE ){
      return true;
    }
  }
  return false;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   指定のJoinFestaフレンドキーが自身のものか判定
 *
 * @param[in]   friendKey 調査対象となるJoinFestaフレンドキー
 *
 * @return  自分自身のJoinFestaフレンドキーならtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
// @static_function
bool JoinFestaPacketUtil::IsMyFriendKey( const JoinFestaFriendKey& friendKey )
{
  const NetAppLib::JoinFesta::JoinFestaPersonalData* myPersonal= GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetMyData();
  return IsSameFriendKey( myPersonal->GetJoinFestaFriendKey(), friendKey );
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   指定のJoinFestaフレンドキーが有効なものか判定
 *
 * @param[in]   friendKey 調査対象となるJoinFestaフレンドキー
 *
 * @return  有効なJoinFestaフレンドキーならtrue、無効なJoinFestaフレンドキーならfalseを返却
 */
/* ----------------------------------------------------------------------------------------- */
// @static_function
bool JoinFestaPacketUtil::IsValidFriendKey( const JoinFestaFriendKey& friendKey )
{
  return ( (friendKey.principalId != NN_FRIENDS_INVALID_PRINCIPAL_ID) || (friendKey.localFriendCode != NN_FRIENDS_INVALID_LOCAL_FRIEND_CODE) );
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   transferdIDを比較してどちらのJoinFestaフレンドキーが大きな数字か判定
 *
 * @param[in]   friendKey1 調査対象となるJoinFestaフレンドキー1
 * @param[in]   friendKey2 調査対象となるJoinFestaフレンドキー2
 *
 * @note    数値が同じの場合（同一JoinFestaフレンドキーを渡した場合）はtrueを返却
 *
 * @return  JoinFestaフレンドキー1が大きいならtrue、JoinFestaフレンドキー2が大きいならfalseを返却
 */
/* ----------------------------------------------------------------------------------------- */
// @static_function
bool JoinFestaPacketUtil::IsBiggerJoinFestaFriendKey( const JoinFestaFriendKey& friendKey1, const JoinFestaFriendKey& friendKey2 )
{
  return friendKey1.localFriendCode >= friendKey2.localFriendCode;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   指定コマンドが基本ステータス情報を含むコマンドか判定
 *
 * @param[in] command   調査対象となるパケットコマンド
 *
 * @return  基本ステータス情報を含むコマンドであればtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
// @static_function
bool JoinFestaPacketUtil::IsBasicCommand( JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_KIND command )
{
  return (JoinFestaDefine::E_JOIN_FESTA_PACKET_SEPARTOR_BASIC_TOP < command) && (command < JoinFestaDefine::E_JOIN_FESTA_PACKET_SEPARTOR_BASIC_BOTTOM);
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パケットデータのユーザーIDからJoinFestaフレンドキーを生成する
 *
 * @param[in]   isWifi Wifi通信モードのユーザーIDならtrueを指定する
 * @param[in]   userId ユーザーID
 *
 * @return  生成されたJoinFestaフレンドキー
 */
/* ----------------------------------------------------------------------------------------- */
// @static_function
JoinFestaFriendKey JoinFestaPacketUtil::ConvertUserIdToJoinFestaFriendKey( bool isWifi, bit64 userId )
{
  JoinFestaFriendKey friendKey;

#if defined(GF_PLATFORM_CTR)
  if( isWifi ){
    friendKey.initialize( static_cast<gflnet2::friends::PrincipalId>(userId) );
  }
  else{
    friendKey.initialize( static_cast<gflnet2::friends::LocalFriendCode>(userId) );
  }
#endif // GF_PLATFORM_CTR

  return friendKey;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   JoinFestaパーソナル情報の取得
 *
 * @param[in/out]   gameMgr     GameManagerへのポインタ
 * @param[in]       relation    検索するユーザーの種別（通りすがり・VIP・友達）
 * @param[in]       index       releationで指定したユーザー種別に格納される個人情報のインデックス（0～max）
 *
 * @return  パーソナル情報へのポインタ。該当者不在の場合はNULLが返却される。
 */
/* ----------------------------------------------------------------------------------------- */
// @static_function
/*
const NetAppLib::JoinFesta::JoinFestaPersonalData* JoinFestaPacketUtil::GetJoinFestaPersonalData( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION relation, u32 index )
{
  //NetAppLib::JoinFesta::JoinFestaPersonalDataManager*                      JoinFestaPersonalMgr = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetJoinFestaPersonalDataManager();
  //NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* personalList   = JoinFestaPersonalMgr->GetPersonalDataList( relation );
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* personalList   = NULL;

  if( personalList == NULL )
  {
    // 指定種別が誤っている
    return NULL;
  }
  u32 listNum = personalList->Size();

  if( listNum == 0 )
  {
    // リストが空
    return NULL;
  }
  u32 count = 0;

  for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = personalList->Begin(); it != personalList->End(); ++it )
  {
    if( count == index ){
      return (*it);
    }
    count++;
  }
  return NULL;
}
*/

/* ----------------------------------------------------------------------------------------- */
/**
* @brief   自身を除くすべてのユーザーをオフラインに設定する
*/
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPacketUtil::SetOfflineJoinFestaPersonalAll( void )
{
#if defined(GF_PLATFORM_CTR)
  // インフォメーションメッセージクリア
  GFL_SINGLETON_INSTANCE(JoinFestaPacketManager)->GetJoinFestaInformationMessage()->Reset();

  // 通信リクエストをクリア
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketEventList)->Reset();

  // 全種別のユーザーをオフラインに設定
  for( u32 i = 0; i < JoinFestaDefine::E_JOIN_FESTA_RELATION_ANOTHER_MAX; i++ ){
    SetOfflineJoinFestaPersonalWithRelation( static_cast<JoinFestaDefine::E_JOIN_FESTA_RELATION>(i) );
  }
#endif // GF_PLATFORM_CTR
}

/* ----------------------------------------------------------------------------------------- */
/**
* @brief   指定種別のユーザーをオフラインに設定する
*/
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPacketUtil::SetOfflineJoinFestaPersonalWithRelation( JoinFestaDefine::E_JOIN_FESTA_RELATION relation )
{
#if defined(GF_PLATFORM_CTR)
  JoinFestaPersonalDataManager* personalMgr = GFL_SINGLETON_INSTANCE(JoinFestaPersonalDataManager);
  JoinFestaPacketManager*     responder   = GFL_SINGLETON_INSTANCE(JoinFestaPacketManager);

  JoinFestaPersonalDataManager::JoinFestaPersonalDataList* list = personalMgr->GetPersonalDataList( relation );

  if( (list != NULL) && (list->Size() > 0) )
  {
    for( JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = list->Begin(); (it != list->End()) && (list->Size()>0);  )
    {
      //============================================
      // JoinFestaPacketManager::RequestOffline()はJoinFestaPersonalDataListから
      // イテレータを削除する場合がある。
      // 必ず事前にカレントイテレータを次に進めておくこと
      //============================================
      NetAppLib::JoinFesta::JoinFestaPersonalData* personal = (*it);
      it++;
      //============================================

      if( personal->IsOnline() ){
        responder->RequestOffline( personal );
      }
    }
  }
#endif // GF_PLATFORM_CTR
}

//------------------------------------------------------------------
/**
* @brief   JoinFesta用ユーザーセーブデータ領域の更新
*/
//------------------------------------------------------------------
void JoinFestaPacketUtil::UpdateJoinFestaPersonalSaveData()
{
#if defined(GF_PLATFORM_CTR)
  JoinFestaPersonalDataManager* personalMgr = GFL_SINGLETON_INSTANCE(JoinFestaPersonalDataManager);
  JoinFestaPacketManager*     responder   = GFL_SINGLETON_INSTANCE(JoinFestaPacketManager);

  for( u32 i = 0; i < JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; i++ )
  {
    JoinFestaPersonalDataManager::JoinFestaPersonalDataList *list = personalMgr->GetPersonalDataList( static_cast<JoinFestaDefine::E_JOIN_FESTA_RELATION>(i) );

    if( (list != NULL) && (list->Size() > 0) )
    {
      for( JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = list->Begin(); (it != list->End()) && (list->Size()>0);  )
      {

        // @fix NMCat[4004]：ゲストNG設定だが、P2P接続により追加される場合がある。その場合、イテレータが削除されてハングしていた。
        //                   イテレータを事前に進めておくことで対処。（上にある関数と同様の処理）

        //============================================
        // JoinFestaPacketManager::RequestUpdatePacket()はJoinFestaPersonalDataListから
        // イテレータを削除する場合がある。
        // 必ず事前にカレントイテレータを次に進めておくこと
        //============================================
        NetAppLib::JoinFesta::JoinFestaPersonalData* personal = (*it);
        it++;
        //============================================

        responder->RequestUpdatePacket( personal );
      }
    }
  }
#endif // GF_PLATFORM_CTR
}


/* ----------------------------------------------------------------------------------------- */
/**
  * @brief   アトラクションに参加可能かチェックする
  * @retval  true : 参加出来る
  * @retval  false : 参加出来ない
  *
  * @note    条件　①アトラクションご褒美受取済み
  *                ②アトラクション開いているか
  *                ③クリア後解禁アトラクションの場合、クリアしているか
  *                ④制限時間の残りが６０秒以上あるか
  */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPacketUtil::CheckAttractionJoinEnable( JoinFestaPersonalData* pPersonalData )
{
  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();
  // ①アトラクション開放済みでないと、アトラクションの情報は全てOFF
  if( pSave->GetRewardState( JoinFestaScript::REWARD_ATTRACTION ) != JoinFestaScript::REWARD_STATE_FINISH )
  {
    return false;
  }

  if( pPersonalData == NULL )
  {
    return false;
  }

  // ②アトラクション開いているか
  if( pPersonalData->IsOpenAttraction() == false )
  {
    return false;
  }

  // ③クリア後解禁アトラクションかどうか（徐々に開放のやつはチェックしない）
  NetAppLib::JoinFesta::JoinFestaAttractionManager* pAttractionManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager);
  NetAppLib::JoinFesta::JoinFestaAttractionDefine::CoreData coreData = pAttractionManager->GetAttractionData( static_cast<JoinFestaScript::AttractionId>(pPersonalData->GetPacketAttraction().attractionID) );
  Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetEventWork();
  const bool isGameClear = pEventWork->CheckEventFlag( SYS_FLAG_GAME_CLEAR );
  if( coreData.isAfterClear == 1 && isGameClear == false )
  {
    return false;
  }

  // ④制限時間の残りが６０秒以上あるか
  if( pPersonalData->GetPacketAttraction().time < ( NetAppLib::JoinFesta::JoinFestaAttractionDefine::ATTRACTION_ENTRY_TIME_LIMIT * 30 ) )
  {
    return false;
  }

  return true;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   更新日時順にソートする
 * @param   sortList     ソートしたいリスト
 * @param   sortListSize ソートリストサイズ
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPacketUtil::SortByUpdateDate( JoinFestaPersonalData* sortList[], const u32 sortListSize )
{
  for( u32 i=0; i<sortListSize; i++ )
  {
    for( u32 j=i; j<sortListSize; j++ )
    {
      if( i == j )
      {
        continue;
      }
      // 更新日時
      JoinFestaPersonalData::Date date;
      date = sortList[i]->GetUpdateDate();
      gfl2::system::Date date1( (date.year+2000), date.month, date.day, date.hour, date.minute, date.half_sec );
      date = sortList[j]->GetUpdateDate();
      gfl2::system::Date date2( (date.year+2000), date.month, date.day, date.hour, date.minute, date.half_sec );

      if( date1.GetDateTimeToMilliSecond() < date2.GetDateTimeToMilliSecond() )
      {
        JoinFestaPersonalData* tmp;
        tmp = sortList[i];
        sortList[i] = sortList[j];
        sortList[j] = tmp;
      }
    }
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
  * @brief   ファインダースタジオ写真Newフラグチェック
  * @retval  true:１人以上写真Newフラグがたっている   
  * @retval  false:１人も写真Newフラグがたっていない   
  */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPacketUtil::CheckNewPhoto()
{
  JoinFestaPersonalDataManager* personalMgr = GFL_SINGLETON_INSTANCE(JoinFestaPersonalDataManager);

  for( u32 i = 0; i < JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; i++ )
  {
    JoinFestaPersonalDataManager::JoinFestaPersonalDataList *list = personalMgr->GetPersonalDataList( static_cast<JoinFestaDefine::E_JOIN_FESTA_RELATION>(i) );

    if( (list != NULL) && (list->Size() > 0) )
    {
      for( JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = list->Begin(); (it != list->End()) && (list->Size()>0); ++it  )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* personal = (*it);
        // 写真Newフラグチェック
        if( personal->IsNewPhoto() )
        {
          return true;
        }
      }
    }
  }

  return false;
}


}    // JoinFesta
}    // NetAppLib

