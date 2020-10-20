//============================================================================================
/**
 * @file		JoinFestaGuestSave.h
 * @brief		JoinFestaゲスト登録データ管理情報
 * @date    2015/10/13 15:02:39
 * @author  ichiraku_katsuhiko
 */
//============================================================================================

#include "Savedata/include/JoinFestaGuestSave.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"

#include "Savedata/include/JoinFestaDataSave.h"

namespace Savedata {
namespace JoinFestaGuest {

  //------------------------------------------------------------------------------------------
  /**
  * @brief  セーブデータ領域へのゲスト情報を登録(グローバル関数)
  *
  * @param[in] friendKey 登録対象となるJoinFestaフレンドキー
  *
  * @return  追加されたならtrueを、空きがない・データがすでに存在する場合はfalseを返却
  */
  //------------------------------------------------------------------------------------------
  bool  AddPersonalToJoinFestaGuestSaveData( const JoinFestaFriendKey& friendKey )
  {
    ICHI_PRINT("AddPersonalToJoinFestaGuestSaveData\n");
    Savedata::JoinFestaPersonalSave*                    guestData        =  GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaGuestData();
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* JoinFestaPersonalMgr = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    const NetAppLib::JoinFesta::JoinFestaPersonalData*  targetPerson   = JoinFestaPersonalMgr->GetPersonalData( friendKey );

    if( targetPerson == NULL ){
      return false;
    }

    return guestData->AddPersonalData( targetPerson );
  }


  //------------------------------------------------------------------------------------------
  /**
  * @brief  セーブデータ領域へのゲスト情報を登録(グローバル関数)
  *
  * @param[in] targetPerson 登録対象となるJoinFestaパーソナルデータ
  *
  * @return  追加されたならtrueを、空きがない・データがすでに存在する場合はfalseを返却
  */
  //------------------------------------------------------------------------------------------
  bool  AddPersonalToJoinFestaGuestSaveData( const NetAppLib::JoinFesta::JoinFestaPersonalData& targetPerson )
  {
    ICHI_PRINT("AddPersonalToJoinFestaGuestSaveData\n");
    GameSys::GameManager*             gameManager    = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GameSys::GameData*                gameData       = gameManager->GetGameData();
    Savedata::JoinFestaPersonalSave*  guestData        = gameData->GetJoinFestaGuestData();

    return guestData->AddPersonalData( &targetPerson );
  }

  //------------------------------------------------------------------------------------------
  /**
  * @brief  セーブデータ領域のゲスト情報を削除(グローバル関数)
  *
  * @param[in] friendKey 削除対象となるJoinFestaフレンドキー
  *
  * @return  削除されたならtrueを、削除失敗の場合はfalseを返却
  */
  //------------------------------------------------------------------------------------------
  bool RemovePersonalFromJoinFestaGuestSaveData( const JoinFestaFriendKey& friendKey )
  {
    ICHI_PRINT("RemovePersonalFromJoinFestaGuestSaveData\n");
    GameSys::GameManager*        gameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GameSys::GameData*           gameData     = gameManager->GetGameData();
    Savedata::JoinFestaPersonalSave*  guestData = gameData->GetJoinFestaGuestData();

    return guestData->RemovePersonalData( friendKey );
  }

  //------------------------------------------------------------------------------------------
  /**
  * @brief  セーブデータ領域のゲスト情報を更新(グローバル関数)
  *
  * @param[in] friendKey 更新対象となるJoinFestaフレンドキー
  *
  * @return  更新されたならtrueを、更新に失敗した場合はfalseを返却
  */
  //------------------------------------------------------------------------------------------
  bool  UpdatePersonalToJoinFestaGuestSaveData( const JoinFestaFriendKey& friendKey )
  {
    //ICHI_PRINT("UpdatePersonalToJoinFestaGuestSaveData\n");
    GameSys::GameManager*             gameManager    = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GameSys::GameData*                gameData       = gameManager->GetGameData();
    Savedata::JoinFestaPersonalSave*  guestData        = gameData->GetJoinFestaGuestData();
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* JoinFestaPersonalMgr = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    NetAppLib::JoinFesta::JoinFestaPersonalData*        targetPerson   = JoinFestaPersonalMgr->GetPersonalData( friendKey );

    if( targetPerson == NULL ){
      return false;
    }

    return guestData->UpdatePersonalData( targetPerson );
  }

  //------------------------------------------------------------------------------------------
  /**
  * @brief  セーブデータ領域のゲスト情報が未登録か判定(グローバル関数)
  *
  * @retval true  未登録
  * @retval false 1件以上登録あり
  */
  //------------------------------------------------------------------------------------------
  bool IsEmptyJoinFestaGuestSaveData( void )
  {
    GameSys::GameManager*        gameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GameSys::GameData*           gameData     = gameManager->GetGameData();
    Savedata::JoinFestaPersonalSave*  guestData = gameData->GetJoinFestaGuestData();

    return guestData->IsEmpty();
  }

  //------------------------------------------------------------------------------------------
  /**
  * @brief  セーブデータ領域のゲスト情報が制限件数に達しているか判定(グローバル関数)
  *
  * @return  制限件数に達しているならtrueを、空きがあるならfalseを返却
  */
  //------------------------------------------------------------------------------------------
  bool IsFullJoinFestaGuestSaveData( void )
  {
    GameSys::GameManager*        gameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GameSys::GameData*           gameData     = gameManager->GetGameData();
    Savedata::JoinFestaPersonalSave*  guestData = gameData->GetJoinFestaGuestData();

    return guestData->IsFull();
  }


  //------------------------------------------------------------------------------------------
  /**
  * @brief  指定JoinFestaフレンドキーのユーザーがセーブデータ領域のゲスト情報に存在するか判定（グローバル関数）
  *
  * @param[in] friendKey 判定対象JoinFestaフレンドキー
  *
  * @return  制限件数に達しているならtrueを、空きがあるならfalseを返却
  */
  //------------------------------------------------------------------------------------------
  bool IsExistJoinFestaGuestSaveData( const JoinFestaFriendKey& friendKey )
  {
    GameSys::GameManager*        gameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GameSys::GameData*           gameData     = gameManager->GetGameData();
    Savedata::JoinFestaPersonalSave*  guestData = gameData->GetJoinFestaGuestData();

    return (guestData->GetPersonalCoreData(friendKey) != NULL);
  }


  //------------------------------------------------------------------------------------------
  /**
  * @brief  ジョインフェスタゲスト用セーブデータの内容をJoinFestaPersonalDataManagerに反映する
  */
  //------------------------------------------------------------------------------------------
  void ApplyJoinFestaGuestSaveData( void )
  {
    ICHI_PRINT("ApplyJoinFestaGuestSaveData\n");
    GameSys::GameManager*             gameManager    = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GameSys::GameData*                gameData       = gameManager->GetGameData();
    Savedata::JoinFestaPersonalSave*  guestData        = gameData->GetJoinFestaGuestData();
    Savedata::JoinFestaDataSave*      pJoinFestaSave = gameData->GetJoinFestaData();
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* JoinFestaPersonalMgr = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    u32                               guestNum       = guestData->GetNum();
    JoinFestaFriendKey                myFriendKey;

    // ゲストとの通信を拒否していたら、リストに何も表示させない為に展開しない
    if( pJoinFestaSave->IsRejectGuest() )
    {
      ICHI_PRINT(">ICHI ゲスト拒否フラグがONな為、セーブ展開しない\n");
      return;
    }

    // JoinFestaPersonalDataManagerリストをクリア
    JoinFestaPersonalMgr->ReleaseList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );

    // セーブデータに登録があるならJoinFestaPersonalDataManagerに登録する
    if( guestNum > 0 )
    {
      const NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData* guestList = guestData->GetPersonalDataList();

      for( u32 i=0; i<guestNum; i++ )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData personal( guestList[i] );

        // フレンドだった場合は、フレンド拒否フラグをチェックする
        if( pJoinFestaSave->IsRejectFriend() && personal.IsCtrFriend() )
        {
          ICHI_PRINT(">ICHI フレンド拒否フラグがONな為、セーブ展開しない\n");
          continue;
        }

        // 常駐リストに追加
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonal = JoinFestaPersonalMgr->UpdatePersonalDataWithPersonal( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST, personal );
      }
    }
  }


} // JoinFestaGuest
} // Savedata

