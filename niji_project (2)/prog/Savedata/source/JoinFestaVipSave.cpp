//============================================================================================
/**
 * @file		JoinFestaVipSave.h
 * @brief		JoinFestaVIP登録データ管理情報
 * @date    2015/10/13 15:02:39
 * @author  ichiraku_katsuhiko
 */
//============================================================================================

#include "Savedata/include/JoinFestaVipSave.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"

#include "Savedata/include/JoinFestaDataSave.h"

namespace Savedata {
namespace JoinFestaVip {

  //------------------------------------------------------------------------------------------
  /**
  * @brief  セーブデータ領域へのVIP情報を登録(グローバル関数)
  *
  * @param[in] friendKey 登録対象となるJoinFestaフレンドキー
  *
  * @return  追加されたならtrueを、空きがない・データがすでに存在する場合はfalseを返却
  */
  //------------------------------------------------------------------------------------------
  bool  AddPersonalToJoinFestaVipSaveData( const JoinFestaFriendKey& friendKey )
  {
    ICHI_PRINT("AddPersonalToJoinFestaVipSaveData\n");
    Savedata::JoinFestaPersonalSave*                    vipData        =  GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaVipData();
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* JoinFestaPersonalMgr = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    const NetAppLib::JoinFesta::JoinFestaPersonalData*  targetPerson   = JoinFestaPersonalMgr->GetPersonalData( friendKey );

    if( targetPerson == NULL ){
      return false;
    }

    return vipData->AddPersonalData( targetPerson );
  }


  //------------------------------------------------------------------------------------------
  /**
  * @brief  セーブデータ領域へのVIP情報を登録(グローバル関数)
  *
  * @param[in] targetPerson 登録対象となるJoinFestaパーソナルデータ
  *
  * @return  追加されたならtrueを、空きがない・データがすでに存在する場合はfalseを返却
  */
  //------------------------------------------------------------------------------------------
  bool  AddPersonalToJoinFestaVipSaveData( const NetAppLib::JoinFesta::JoinFestaPersonalData& targetPerson )
  {
    ICHI_PRINT("AddPersonalToJoinFestaVipSaveData\n");
    GameSys::GameManager*             gameManager    = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GameSys::GameData*                gameData       = gameManager->GetGameData();
    Savedata::JoinFestaPersonalSave*  vipData        = gameData->GetJoinFestaVipData();

    return vipData->AddPersonalData( &targetPerson );
  }

  //------------------------------------------------------------------------------------------
  /**
  * @brief  セーブデータ領域のVIP情報を削除(グローバル関数)
  *
  * @param[in] friendKey 削除対象となるJoinFestaフレンドキー
  *
  * @return  削除されたならtrueを、削除失敗の場合はfalseを返却
  */
  //------------------------------------------------------------------------------------------
  bool RemovePersonalFromJoinFestaVipSaveData( const JoinFestaFriendKey& friendKey )
  {
    ICHI_PRINT("RemovePersonalFromJoinFestaVipSaveData\n");
    GameSys::GameManager*        gameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GameSys::GameData*           gameData     = gameManager->GetGameData();
    Savedata::JoinFestaPersonalSave*  vipData = gameData->GetJoinFestaVipData();

    return vipData->RemovePersonalData( friendKey );
  }

  //------------------------------------------------------------------------------------------
  /**
  * @brief  セーブデータ領域のVIP情報を更新(グローバル関数)
  *
  * @param[in] friendKey 更新対象となるJoinFestaフレンドキー
  *
  * @return  更新されたならtrueを、更新に失敗した場合はfalseを返却
  */
  //------------------------------------------------------------------------------------------
  bool  UpdatePersonalToJoinFestaVipSaveData( const JoinFestaFriendKey& friendKey )
  {
    //ICHI_PRINT("UpdatePersonalToJoinFestaVipSaveData\n");
    GameSys::GameManager*             gameManager    = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GameSys::GameData*                gameData       = gameManager->GetGameData();
    Savedata::JoinFestaPersonalSave*  vipData        = gameData->GetJoinFestaVipData();
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* JoinFestaPersonalMgr = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    NetAppLib::JoinFesta::JoinFestaPersonalData*        targetPerson   = JoinFestaPersonalMgr->GetPersonalData( friendKey );

    if( targetPerson == NULL ){
      return false;
    }

    return vipData->UpdatePersonalData( targetPerson );
  }

  //------------------------------------------------------------------------------------------
  /**
  * @brief  セーブデータ領域のVIP情報が未登録か判定(グローバル関数)
  *
  * @retval true  未登録
  * @retval false 1件以上登録あり
  */
  //------------------------------------------------------------------------------------------
  bool IsEmptyJoinFestaVipSaveData( void )
  {
    GameSys::GameManager*        gameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GameSys::GameData*           gameData     = gameManager->GetGameData();
    Savedata::JoinFestaPersonalSave*  vipData = gameData->GetJoinFestaVipData();

    return vipData->IsEmpty();
  }

  //------------------------------------------------------------------------------------------
  /**
  * @brief  セーブデータ領域のVIP情報が制限件数に達しているか判定(グローバル関数)
  *
  * @return  制限件数に達しているならtrueを、空きがあるならfalseを返却
  */
  //------------------------------------------------------------------------------------------
  bool IsFullJoinFestaVipSaveData( void )
  {
    GameSys::GameManager*        gameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GameSys::GameData*           gameData     = gameManager->GetGameData();
    Savedata::JoinFestaPersonalSave*  vipData = gameData->GetJoinFestaVipData();

    return vipData->IsFull();
  }


  //------------------------------------------------------------------------------------------
  /**
  * @brief  指定JoinFestaフレンドキーのユーザーがセーブデータ領域のVIP情報に存在するか判定（グローバル関数）
  *
  * @param[in] friendKey 判定対象JoinFestaフレンドキー
  *
  * @return  制限件数に達しているならtrueを、空きがあるならfalseを返却
  */
  //------------------------------------------------------------------------------------------
  bool IsExistJoinFestaVipSaveData( const JoinFestaFriendKey& friendKey )
  {
    GameSys::GameManager*        gameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GameSys::GameData*           gameData     = gameManager->GetGameData();
    Savedata::JoinFestaPersonalSave*  vipData = gameData->GetJoinFestaVipData();

    return (vipData->GetPersonalCoreData(friendKey) != NULL);
  }


  //------------------------------------------------------------------------------------------
  /**
  * @brief  ジョインフェスタVIP用セーブデータの内容をJoinFestaPersonalDataManagerに反映する
  */
  //------------------------------------------------------------------------------------------
  void ApplyJoinFestaVipSaveData( void )
  {
    ICHI_PRINT("ApplyJoinFestaVipSaveData\n");
    GameSys::GameManager*             gameManager    = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GameSys::GameData*                gameData       = gameManager->GetGameData();
    Savedata::JoinFestaPersonalSave*  vipData        = gameData->GetJoinFestaVipData();
    Savedata::JoinFestaDataSave*      pJoinFestaSave = gameData->GetJoinFestaData();
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* JoinFestaPersonalMgr = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    u32                               vipNum       = vipData->GetNum();
    JoinFestaFriendKey                myFriendKey;

    // VIPとの通信を拒否していたら、リストに何も表示させない為に展開しない
    if( pJoinFestaSave->IsRejectVip() )
    {
      ICHI_PRINT(">ICHI VIP拒否フラグがONな為、セーブ展開しない\n");
      return;
    }

    // JoinFestaPersonalDataManagerリストをクリア
    JoinFestaPersonalMgr->ReleaseList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP );

    // セーブデータに登録があるならJoinFestaPersonalDataManagerに登録する
    if( vipNum > 0 )
    {
      const NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData* vipList = vipData->GetPersonalDataList();

      for( u32 i=0; i<vipNum; i++ )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData personal( vipList[i] );

        // フレンドだった場合は、フレンド拒否フラグをチェックする
        if( pJoinFestaSave->IsRejectFriend() && personal.IsCtrFriend() )
        {
          ICHI_PRINT(">ICHI フレンド拒否フラグがONな為、セーブ展開しない\n");
          continue;
        }

        // 常駐リストに追加
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonal = JoinFestaPersonalMgr->UpdatePersonalDataWithPersonal( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP, personal );
      }
    }
  }

} // JoinFestaVip
} // Savedata

