//==============================================================================
/**
 * @file        ScriptFuncSetJoinFesta.h
 * @brief       ジョインフェスタ用コマンド
 * @author      ichiraku_katsuhiko
 * @date        2015/10/21
 */
//==============================================================================
#include "NetApp/JoinFesta/include/Script/ScriptFuncSetJoinFesta.h"

#include <pml/include/pml_Battle.h>

#include "Print/include/WordSet.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/NijiTime.h"
#include "System/include/HeapDefine.h"

#include "FieldScript/include/ScriptWork.h"
#include "FieldScript/include/ScriptObject.h"
#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptTrialModel.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldRo/include/StaticModel/StaticModelManager.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldActionEventActor.h"
#include "niji_conv_header/field/FieldGimmickTypes.h"
//下画面
#include "Field/FieldRo/include/Subscreen/FieldSubScreenProcManager.h"

// field event
#include "App/AppEvent/include/Shop/ShopBuyEvent.h"
#include "App/AppEvent/include/Kisekae/KisekaeEvent.h"

#include <arc_index/message.gaix>
#include <arc_index/script_message.gaix>

// save
#include "Savedata/include/JoinFestaDataSave.h"
#include "Savedata/include/FashionSave.h"
#include "Savedata/include/MyStatus.h"
#include "Savedata/include/ResortSave.h"
#include "Savedata/include/Record.h"
#include "Savedata/include/PokeDiarySave.h"
#include "Savedata/include/GameSyncSave.h"
#include "savedata/include/ResortSave.h"
#include "Savedata/include/BattleFesSave.h"

//  きせかえデータベース
#include "KisekaeData/include/KisekaeDataBase.h"

// ジョインフェスタギミック
#include "Field/FieldRos/FieldGimmickJoinFesta/include/FieldGimmickJoinFesta.h"
#include "Field/FieldRos/FieldGimmickJoinFestaRoom/include/FieldGimmickJoinFestaRoom.h"

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"

// ジョインフェスタパーソナルデータ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalEvent/JoinFestaPersonalEvent.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"
// ジョインフェスタ施設ユーティリティ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaFacilitiesUtil/JoinFestaFacilitiesUtil.h"
// ジョインフェスタランキングモジュール
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordRankingManager.h"
// ジョインフェスタ動的動作モデルロードシステム
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFModel.h"
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFModelManager.h"
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFDynamicLoadSystem.h"
// ジョインフェスタ施設システム
#include "NetApp/JoinFesta/include/JoinFestaDefine.h"
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesSystem.h"
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesData.h"
#include "NetApp/JoinFesta/include/JoinFestaDataTable/JoinFestaDataTable.h"
// アトラクションマネージャ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionUtil.h"
// ジョインフェスタエフェクト
#include "NetApp/JoinFesta/include/JoinFestaEffectSystem/JoinFestaEffectSystem.h"

// ジョインフェスタリソースマネージャ
#include "NetApp/JoinFesta/include/JoinFestaResourceManager/JoinFestaResourceManager.h"

// JoinFestaScript
#include "niji_reference_files/script/JoinFestaPawnDefine.h"

// 施設更新イベント
#include "NetApp/JoinFesta/include/Event/JoinFestaFacilitiesModelUpdateEvent.h"
// リクルーターイベント
#include "NetApp/JoinFesta/include/Event/JoinFestaRecruiterEvent.h"
// チュートリアルイベント
#include "NetApp/JoinFesta/include/Event/JoinFestaTutorialEvent.h"

// ゲームシンクコード変換用
#include "NetStatic/NetAppLib/include/Util/NetAppConvertUtility.h"

// message
#include  <niji_conv_header/message/msg_jf_phrase.h>
#include  <niji_conv_header/message/msg_jf_shop_name.h>
#include  <niji_conv_header/message/msg_jf_questiondata.h>
#include  <niji_conv_header/message/msg_jf_playerlist.h>
#include  <niji_conv_header/message/msg_net_app_name.h>
#include  <niji_conv_header/message/msg_ride_poke_name.h>
#include  <niji_conv_header/message/msg_place_name.h>
#include  <niji_conv_header/message/msg_jf_attraction.h>
#include  <niji_conv_header/message/msg_wazaname.h>
#include  <niji_conv_header/message/script/msg_z_f0601jf0101.h>
#include  <niji_conv_header/message/script/msg_cafe_event.h>
#include  <niji_conv_header/message/script/msg_shop_malasada.h>

// バトルフェス
// ポケモン抽選管理
#include "Field/FieldStatic/include/BattleFes/BattleFesPokeLotterySystem.h"
#include <niji_conv_header/poke_lib/item/monsball_id.h>
#include "Field/FieldStatic/include/BattleFes/BattleFesManager.h"
#include "Field/FieldStatic/include/BattleFes/BattleFesTool.h"

// =============================================================================
/**/
//==============================================================================
/// @brief フィールド
GFL_NAMESPACE_BEGIN(Field)
/// @brief スクリプト
GFL_NAMESPACE_BEGIN(FieldScript)


// ダイアリーデータの範囲外チェック
// @fix NMCat[4210]：マーシャドーに関わるデータは破棄する
bool IsOutOfRangeMonsNo( u16 monsNo )
{
  // momiji追加：ゼラオラも追加対応する
  if( monsNo == MONSNO_NULL || monsNo > MONSNO_MAX || monsNo == MONSNO_MABOROSI17 || monsNo == MONSNO_MABOROSI18 )
  {
    GFL_RELEASE_PRINT("ダイアリーデータ ポケモンNo範囲外[%d]\n", monsNo);
    return true;
  }
  else
  {
    return false;
  }
}
bool IsOutOfRangeItemNo( u16 itemNo )
{
  if( itemNo == ITEM_DUMMY_DATA || itemNo > ITEM_DATA_MAX ||
      itemNo == ITEM_HAISINBIIZU_1 || itemNo == ITEM_HAISINBIIZU )
  {
    GFL_RELEASE_PRINT("ダイアリーデータ アイテムNo範囲外[%d]\n", itemNo);
    return true;
  }
  else
  {
    return false;
  }
}
bool IsOutOfRangeWazaNo( u16 wazaNo )
{
  // momiji追加：ゼラオラ専用技も追加対応する
  if( wazaNo >= WAZANO_MAX ||
      wazaNo == WAZANO_SYADOOSUTIIRU || wazaNo == WAZANO_HAISINSENYOUZENRYOKU ||
      wazaNo == WAZANO_MABOROSIWAZA )
  {
    GFL_RELEASE_PRINT("ダイアリーデータ 技No範囲外[%d]\n", wazaNo);
    return true;
  }
  else
  {
    return false;
  }
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
cell AMX_NATIVE_CALL nJFInitializeGlobal(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*           pScriptWork = pScript->GetWork();
  GameSys::GameManager*               pGameManager= pScript->GetGameManager();
  Field::Fieldmap*                    pFieldmap   = pGameManager->GetFieldmap();
  const bool                          isRoom      = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Savedata::JoinFestaDataSave*        pSave       = pGameManager->GetGameData()->GetJoinFestaData();
  Savedata::GameSyncSave*             pGameSyncSave = pGameManager->GetGameData()->GetSaveData()->GetGameSync();
  NetAppLib::JoinFesta::JoinFestaAttractionManager* pAttractionManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager);

  // 城外ギミックか城内ギミックか
  Field::FieldGimmickJoinFesta*       pJfGimmick = NULL;
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = NULL;

  if( isRoom )
  {
    pJfGimmickRoom = static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE ));
  }
  else
  {
    pJfGimmick = static_cast<Field::FieldGimmickJoinFesta*>(pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE ));
  }


  // 動作モデルのイベントID
  u32 eventId = 0;
  // 話しかけターゲット存在チェック
  if( pScriptWork->IsTargetObjidNone() == false )
  {
    eventId = pScriptWork->GetTempWorkValue( SCWK_TARGET_OBJID );
  }


#define JFPAWN_SET_I(__pawnGlobalSymbol__, __value__) pScript->SetGlobalParameterInteger(#__pawnGlobalSymbol__, (__value__))
#define JFPAWN_SET_F(__pawnGlobalSymbol__, __value__) pScript->SetGlobalParameterFloat(#__pawnGlobalSymbol__, (__value__))

  //------------------------------------------------------------------------------
  // 全体
  //------------------------------------------------------------------------------
  JFPAWN_SET_I(gJF_IsRoom, isRoom);

  bool isSecretPatternSet = false;
  {
    NetAppLib::JoinFesta::JoinFestaPersonalData* myPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetMyData();

    if( ( myPersonal->GetSex() == PM_MALE && myPersonal->GetPattern( JoinFestaScript::PATTERN_SECRET ) != msg_jf_phrase_m_518 ) ||
        ( myPersonal->GetSex() == PM_FEMALE && myPersonal->GetPattern( JoinFestaScript::PATTERN_SECRET ) != msg_jf_phrase_f_518 )
      )
    {
      isSecretPatternSet = true;
    }
  }


  JFPAWN_SET_I(gJF_IsSecretPatternSet, isSecretPatternSet);
  JFPAWN_SET_I(gJF_IsGameSync, pGameSyncSave->GetIsCreateGameSyncSaveID() );
  JFPAWN_SET_I(gJF_FortuneResultGts, pSave->GetFortuneResultHitByAction( JoinFestaScript::FORTUNE_RESULT_ACTION_GTS ) );
  JFPAWN_SET_I(gJF_FortuneResultMiracle, pSave->GetFortuneResultHitByAction( JoinFestaScript::FORTUNE_RESULT_ACTION_MIRACLE ) );
  JFPAWN_SET_I(gJF_FortuneResultBtlspot, pSave->GetFortuneResultHitByAction( JoinFestaScript::FORTUNE_RESULT_ACTION_BTLSPOT ) );
  JFPAWN_SET_I(gJF_FortuneResultPersonalEvent, pSave->GetFortuneResultHitByAction( JoinFestaScript::FORTUNE_RESULT_ACTION_FESCIRCLE ) );
  JFPAWN_SET_I(gJF_FortuneResultAttraction, pSave->GetFortuneResultHitByAction( JoinFestaScript::FORTUNE_RESULT_ACTION_ATTRACTION ) );


  //------------------------------------------------------------------------------
  // アトラクション系
  //------------------------------------------------------------------------------
  JFPAWN_SET_I(gJF_IsAttractionPlay,              pAttractionManager->IsPlaying());               /**< アトラクション中フラグ */
  JFPAWN_SET_I(gJF_AttractionID,                  pAttractionManager->GetPlayAttractionId());     /**< アトラクション中だった場合アトラクションID */
  JFPAWN_SET_I(gJF_AttractionGetCoin,             pAttractionManager->GetResultData().getFesCoin );     /**< アトラクション結果で入手するフェスコイン値 */

  u8 attractionPokeTypeSearch = 0;
  if( pAttractionManager->IsPlaying() )
  {
    JoinFestaScript::AttractionId attractionId = pAttractionManager->GetPlayAttractionId();
    attractionPokeTypeSearch = NetAppLib::JoinFesta::JoinFestaAttractionUtil::GetPokeTypeFromAttractionId(attractionId);
  }
  JFPAWN_SET_I(gJF_AttractionSearchPokeType,      attractionPokeTypeSearch );      /**< タイプサーチアトラクションで探すタイプ */

  //------------------------------------------------------------------------------
  // 施設系
  //------------------------------------------------------------------------------

  /*  servicetype  */
  JFPAWN_SET_I(gJF_SERVICETYPE_Familiarity,       NetApp::JoinFesta::ShopServiceInfo::SERVICETYPE_Familiarity);        /**<  なつき          */
  JFPAWN_SET_I(gJF_SERVICETYPE_Familiarity_HP,    NetApp::JoinFesta::ShopServiceInfo::SERVICETYPE_Familiarity_HP);     /**<  なつき  HP      */
  JFPAWN_SET_I(gJF_SERVICETYPE_Familiarity_Atk,   NetApp::JoinFesta::ShopServiceInfo::SERVICETYPE_Familiarity_Atk);    /**<  なつき  攻撃    */
  JFPAWN_SET_I(gJF_SERVICETYPE_Familiarity_Def,   NetApp::JoinFesta::ShopServiceInfo::SERVICETYPE_Familiarity_Def);    /**<  なつき  防御    */
  JFPAWN_SET_I(gJF_SERVICETYPE_Familiarity_SpAtk, NetApp::JoinFesta::ShopServiceInfo::SERVICETYPE_Familiarity_SpAtk);  /**<  なつき  特攻    */
  JFPAWN_SET_I(gJF_SERVICETYPE_Familiarity_SpDef, NetApp::JoinFesta::ShopServiceInfo::SERVICETYPE_Familiarity_SpDef);  /**<  なつき  特防    */
  JFPAWN_SET_I(gJF_SERVICETYPE_Familiarity_Agi,   NetApp::JoinFesta::ShopServiceInfo::SERVICETYPE_Familiarity_Agi);    /**<  なつき  素早    */
  JFPAWN_SET_I(gJF_SERVICETYPE_LV,                NetApp::JoinFesta::ShopServiceInfo::SERVICETYPE_LV);                 /**<  LV              */
  JFPAWN_SET_I(gJF_SERVICETYPE_EffortPowerHP,     NetApp::JoinFesta::ShopServiceInfo::SERVICETYPE_EffortPowerHP);      /**<  努力値HP        */
  JFPAWN_SET_I(gJF_SERVICETYPE_EffortPowerAtk,    NetApp::JoinFesta::ShopServiceInfo::SERVICETYPE_EffortPowerAtk);     /**<  努力値攻撃      */
  JFPAWN_SET_I(gJF_SERVICETYPE_EffortPowerDef,    NetApp::JoinFesta::ShopServiceInfo::SERVICETYPE_EffortPowerDef);     /**<  努力値防御      */
  JFPAWN_SET_I(gJF_SERVICETYPE_EffortPowerSpAtk,  NetApp::JoinFesta::ShopServiceInfo::SERVICETYPE_EffortPowerSpAtk);   /**<  努力値特攻      */
  JFPAWN_SET_I(gJF_SERVICETYPE_EffortPowerSpDef,  NetApp::JoinFesta::ShopServiceInfo::SERVICETYPE_EffortPowerSpDef);   /**<  努力値特防      */
  JFPAWN_SET_I(gJF_SERVICETYPE_EffortPowerAgi,    NetApp::JoinFesta::ShopServiceInfo::SERVICETYPE_EffortPowerAgi);     /**<  努力値素早      */
  JFPAWN_SET_I(gJF_SERVICETYPE_Item,              NetApp::JoinFesta::ShopServiceInfo::SERVICETYPE_Item);               /**<  道具            */
  JFPAWN_SET_I(gJF_SERVICETYPE_Happening,         NetApp::JoinFesta::ShopServiceInfo::SERVICETYPE_Happening);          /**<  ビックリハウス  */
  JFPAWN_SET_I(gJF_SERVICETYPE_Lottery,           NetApp::JoinFesta::ShopServiceInfo::SERVICETYPE_Lottery);            /**<  くじ抽選        */

  
  /** 現在話しかけている店の情報をセットする  */
  if( pJfGimmick != NULL )
  {
    NetApp::JoinFesta::JoinFestaFacilitiesSystem*   pJfFacilitiesSystem = pJfGimmick->GetJoinFestaFacilitiesSystem();

    // 施設Index
    JoinFestaScript::FacilitiesIndex facilitiesIndex = JoinFestaScript::FACILITIES_INDEX_0;
    // 施設ID
    u8 facilitiesId = 0;
    // 施設種類
    JoinFestaScript::FacilitiesType facilitiesType = JoinFestaScript::FACILITIES_TYPE_LOTTERY;
    // 店員が男か
    bool isMale = false;
    // 自分の施設かどうか
    bool isOwned = true;
    // 占い結果：当たり取得
    u8 fortuneResultHit = 0;
    // 占い結果：行動取得
    u8 fortuneResultAction = 0;

    // 話しかけた動作モデルが店員かチェック
    if( eventId >= NetApp::JoinFesta::JoinFestaDefine::ASSISTANT_EVENT_ID_OFFSET )
    {
      facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>( eventId - NetApp::JoinFesta::JoinFestaDefine::ASSISTANT_EVENT_ID_OFFSET);
      GFL_PRINT(">施設インデックス %d \n", facilitiesIndex);

      const NetApp::JoinFesta::JoinFestaFacilitiesData* pFacilitiesData = pJfFacilitiesSystem->GetFacilitiesData( facilitiesIndex );

      // 施設のデータから欲しい情報引っ張ってくる
      if( pFacilitiesData != NULL )
      {
        facilitiesId = pFacilitiesData->GetFacilitiesId();
        facilitiesType = pFacilitiesData->GetFacilitiesType();
        isMale = pFacilitiesData->GetAssistantSex() == PM_MALE ? true : false;
        fortuneResultHit = pFacilitiesData->GetFortuneResultHit();
        fortuneResultAction = pFacilitiesData->GetFortuneResultAction();
        isOwned = !pFacilitiesData->IsPlayerFlag();
      }
    }

    // グローバルフラグセット
    JFPAWN_SET_I(gJF_FacilitiesIndex,             facilitiesIndex);
    JFPAWN_SET_I(gJF_FacilitiesId,                facilitiesId);
    JFPAWN_SET_I(gJF_Shop_ShopType,               facilitiesType);
    JFPAWN_SET_I(gJF_Shop_IsOwned,                isOwned);
    JFPAWN_SET_I(gJF_Staff_IsMale,                isMale);
    JFPAWN_SET_I(gJF_FortuneResultHit,            fortuneResultHit);
    JFPAWN_SET_I(gJF_FortuneResultAction,         fortuneResultAction);
  }


  //------------------------------------------------------------------------------
  // 人物系
  //------------------------------------------------------------------------------

  /* 現在話しかけている人のパーソナル情報をセットする   */
  {
    NetApp::JoinFesta::JFModelManager*  pJfModelManager = isRoom ? pJfGimmickRoom->GetJFDynamicLoadSystem()->GetJFModelManager() : pJfGimmick->GetJFDynamicLoadSystem()->GetJFModelManager();

    // JFパーソナルデータ
    NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = pJfModelManager->GetJoinFestaPersonalData( eventId );

    // 人物に話しかけている時のみ
    if( jfPersonal != NULL )
    {
      u32 eventBaseCoin = NetAppLib::JoinFesta::JoinFestaPersonalEvent::GetBaseFesCoin( jfPersonal->GetPersonalEventId() );
      // @fix GFNMcat[1453]：P2P人物イベントの基礎値を通信ルートで決める
      if( jfPersonal->GetPersonalEventId() == JoinFestaScript::PERSONAL_EVENT_THANKS )
      {
        switch( jfPersonal->GetCommRoute() ){
        case JoinFestaScript::COMM_ROUTE_GTS:           eventBaseCoin = 4; break;
        case JoinFestaScript::COMM_ROUTE_MIRACLE_TRADE: eventBaseCoin = 2; break;
        case JoinFestaScript::COMM_ROUTE_RANDOM_BATTLE: eventBaseCoin = 2; break;
        case JoinFestaScript::COMM_ROUTE_P2P_TRADE:     eventBaseCoin = 1; break;
        case JoinFestaScript::COMM_ROUTE_P2P_BATTLE:    eventBaseCoin = 1; break;
        case JoinFestaScript::COMM_ROUTE_QUICK:         eventBaseCoin = 1; break;//!< クイック通信（GFNMCat[1628]対処）
        default:
          GFL_ASSERT_MSG( 0,"%d",jfPersonal->GetCommRoute());
          eventBaseCoin = 1;
          break;
        }
        ICHI_PRINT(">ICHI P2Pイベントコイン %d \n", eventBaseCoin );
      }

      // 人物話しかけかどうか
      JFPAWN_SET_I(gJF_JFPersonalTalk, true );

      // GF#4310対応：おこわりリストの人はVIPコマンド出さないようにする 
      bool isRejectVip = false;
      if( pSave->IsRejectVip() || pSave->IsBlackList(jfPersonal->GetJoinFestaFriendKey()) )
      {
        isRejectVip = true;
      }
      JFPAWN_SET_I(gJF_IsRejectVip, isRejectVip );

      // カメラパターン
      JFPAWN_SET_I(gJF_PersonalCameraPattern, pSave->GetPersonalTalkCameraCount() );
      // 性別
      JFPAWN_SET_I(gJF_PersonalSex, jfPersonal->GetSex() );
      // VIPかどうか
      JFPAWN_SET_I(gJF_IsVip, jfPersonal->IsRelatonVip() );
      // プリセットかどうか
      JFPAWN_SET_I(gJF_IsPreset, jfPersonal->IsPresetNpcData() );
      // プリセットNPCID
      JFPAWN_SET_I(gJF_PresetId, jfPersonal->GetPresetNpcId() );
      // 言語ID
      // @fix NMCat[4278]：フェスサークルのアトラクション異文化交流で繁体字と簡体字で遊んでいる人物のメッセージが逆になる
      //    原因：スクリプト側の言語コード定義が、繁体字・簡体字逆になっていた為。(script_def.inc)
      //    修正方法：スクリプト側を修正する方が正しいが、影響範囲を考慮して、プログラム側で簡体字／繁体字を逆に渡すことで対処する。
      //              
      {
        u8 languageId = jfPersonal->GetLanguageId();
        if( languageId == POKEMON_LANG_CHINA ){ languageId = POKEMON_LANG_TAIWAN; }
        else if( languageId == POKEMON_LANG_TAIWAN ){ languageId = POKEMON_LANG_CHINA; }

        JFPAWN_SET_I(gJF_LanguageId, languageId );
      }
      // 通信経路
      JFPAWN_SET_I(gJF_CommRouteType, jfPersonal->GetCommRoute() );
      // 一押し施設ID
      JFPAWN_SET_I(gJF_RecommendFacilitiesId, jfPersonal->GetRecommendFacilitiesId() );
      JFPAWN_SET_I(gJF_RecommendFacilitiesColor, jfPersonal->GetRecommendFacilitiesColor() );
      JFPAWN_SET_I(gJF_RecommendFacilitiesCharacterId, jfPersonal->GetRecommendFacilitiesCharacterId() );
      // パーソナルイベント発動フラグ
      JFPAWN_SET_I(gJF_IsPersonalEvent, jfPersonal->IsPersonalEventEnable() );
      // パーソナルイベントID
      JFPAWN_SET_I(gJF_PersonalEventId, jfPersonal->GetPersonalEventId() );
      JFPAWN_SET_I(gJF_PersonalEventBaseCoin, eventBaseCoin );
      JFPAWN_SET_I(gJF_PersonalEventRecordIndex1, jfPersonal->GetPersonalEventRecordID1() );
      JFPAWN_SET_I(gJF_PersonalEventRecordIndex2, jfPersonal->GetPersonalEventRecordID2() );
      JFPAWN_SET_I(gJF_PersonalEventRecordIndex3, jfPersonal->GetPersonalEventRecordID3() );
      JFPAWN_SET_I(gJF_PersonalEventRecordCategory1, jfPersonal->GetPersonalEventRecordCategory1() );
      JFPAWN_SET_I(gJF_PersonalEventRecordCategory2, jfPersonal->GetPersonalEventRecordCategory2() );
      JFPAWN_SET_I(gJF_PersonalEventRecordCategory3, jfPersonal->GetPersonalEventRecordCategory3() );
      JFPAWN_SET_I(gJF_PersonalEventQuestionId, jfPersonal->GetPersonalEventQuestionId() );
      JFPAWN_SET_I(gJF_PersonalCommTime, jfPersonal->GetUpdateElapsedDate() ); // 受信からの経過時間(0～73)
      // アトラクション中のNPCダミーデータか
      JFPAWN_SET_I(gJF_IsAttractionDummy, jfPersonal->IsAttractionDummyData() );
      // アトラクション中のパーソナルイベント発動フラグ
      JFPAWN_SET_I(gJF_IsAttractionPersonalEvent, jfPersonal->IsAttractionPersonalEventEnable() );
      // アトラクション中パーソナルイベントID
      JFPAWN_SET_I(gJF_AttractionPersonalEventId, jfPersonal->GetAttractionPersonalEventFake() );

      u8 pokeType1, pokeType2;
      jfPersonal->GetAttractionPokeType( pokeType1, pokeType2 );
      JFPAWN_SET_I(gJF_AttractionPokeType1,           pokeType1 ); 			/**< アトラクションで使用するポケモンタイプ1 */
      JFPAWN_SET_I(gJF_AttractionPokeType2,           pokeType2 ); 			/**< アトラクションで使用するポケモンタイプ2 */
      JFPAWN_SET_I(gJF_AttractionMonsNo,              jfPersonal->GetAttractionMonsNo() ); 			/**< アトラクションで使用するポケモン番号 */
      JFPAWN_SET_I(gJF_AttractionIsAddPoint,          jfPersonal->IsAttractionAddPoint() ); 		/**< アトラクションで加点対象か */

      // 城内のリクルーターかどうか
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData& recruiterPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetRecruiterPersonal();

        if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey(recruiterPersonal.GetJoinFestaFriendKey(),jfPersonal->GetJoinFestaFriendKey()) )
        {
          JFPAWN_SET_I(gJF_IsRecruiterPersonal, true ); 		/**< リクルーター呼び出しかどうか */
        }
      }

      // @fix GFNMCat[1669]：通信拒否設定で「ようすをみる」をさせない
      {
        bool isCommandProfileEnable = true;
        if( pSave->IsRejectVip() == true && jfPersonal->IsRelatonVip() == true )
        {
          isCommandProfileEnable = false;
        }
        else if( pSave->IsRejectGuest() == true && jfPersonal->IsRelatonVip() == false )
        {
          isCommandProfileEnable = false;
        }
        else if( pSave->IsRejectFriend() == true && jfPersonal->IsCtrFriend() == true )
        {
          isCommandProfileEnable = false;
        }

        JFPAWN_SET_I(gJF_IsCommandProfileEnable, isCommandProfileEnable ); 		/**< ようすを見るコマンド有効化フラグ */
      }

      // @fix GFNMCat[4580]：関係性の人数を渡す
      int relationCount = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetCount( jfPersonal->GetRelation() );
      JFPAWN_SET_I(gJF_RelationCount, relationCount ); 		/**< 関係性の人数 */

      /*  写真  */
      {
        App::FinderStudio::Static::PhotoData  data; /*  dummy  */
        bool                                  bPhotoDataAvailable = false;

        bPhotoDataAvailable = jfPersonal->GetPacketPhotoData(&data);
        JFPAWN_SET_I(gJF_PhotoDataAvailable, bPhotoDataAvailable);
      }


      /*  UGCガイドラインメッセージフラグ  */
      {
        JFPAWN_SET_I(gJF_UGCMessageFlag_PhotoView_Talk, pSave->GetUGCGuidelineMessageFlag_PhotoView_FromScriptTalk());
        JFPAWN_SET_I(gJF_UGCMessageFlag_PhotoView_List, pSave->GetUGCGuidelineMessageFlag_PhotoView_FromTrainerSelect());
      }
    }
  }

#undef  JFPAWN_SET_F
#undef  JFPAWN_SET_I
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief       自分の定型文をレジスト
 * @param       第1引数　バッファＩＤ
 * @param       第2引数　定型種類
 *                      0:あいさつ
 *                      1:別れ
 *                      2:感動
 *                      3:残念
 *                      4:ないしょ
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetMyPattern(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork       = pScript->GetWork();
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();
  print::WordSet*                               pWordSet          = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaResourceManager*  pJfResourceManager = isRoom ? pJfGimmickRoom->GetJoinFestaResourceManager() : pJfGimmick->GetJoinFestaResourceManager();


  u32 bufID       = ptr[1]; // バッファID
  u32 patternType = ptr[2]; // 定型種類

  // 自分のパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData* personal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetMyData();

  // 定型用のmstxt使う
  if( personal != NULL )
  {
    gfl2::str::MsgData* pMsgData = pJfResourceManager->GetMsgData( NetApp::JoinFesta::JoinFestaResourceManager::RESOURCE_Message_JFPhrase );

    // 定型文のラベル求める
    u32 msgId = personal->GetPattern( static_cast<JoinFestaScript::PatternType>(patternType) );

    pWordSet->RegisterWord( bufID, pMsgData, msgId );
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       相手のプレイヤー名をレジスト
 * @param       第1引数　バッファＩＤ
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetTargetPlayerName(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript         = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager    = pScript->GetGameManager();
  print::WordSet*                     pWordSet        = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  // 話しかけた相手のJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetScriptTalkFieldPersonalData(); 

  u32 bufID       = ptr[1]; // バッファID

  if( jfPersonal != NULL )
  {
    gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

    // パーソナルデータから名前と性別取ってくる
    gfl2::str::StrBuf name( jfPersonal->GetName(), p_heap );
    pWordSet->RegisterWord( bufID, name, pWordSet->GrammerFromSex( jfPersonal->GetSex() ) );
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       相手の定型文をレジスト
 * @param       第1引数　バッファＩＤ
 * @param       第2引数　定型種類
 *                      0:あいさつ
 *                      1:別れ
 *                      2:感動
 *                      3:残念
 *                      4:ないしょ
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetTargetPattern(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();
  print::WordSet*                               pWordSet          = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaResourceManager*  pJfResourceManager = isRoom ? pJfGimmickRoom->GetJoinFestaResourceManager() : pJfGimmick->GetJoinFestaResourceManager();



  u32 bufID       = ptr[1]; // バッファID
  u32 patternType = ptr[2]; // 定型種類

  // 話しかけた相手のJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetScriptTalkFieldPersonalData(); 

  // 定型用のmstxt使う
  if( jfPersonal != NULL )
  {
    gfl2::str::MsgData* pMsgData = pJfResourceManager->GetMsgData( NetApp::JoinFesta::JoinFestaResourceManager::RESOURCE_Message_JFPhrase );

    // 定型文のラベル求める
    u32 msgId = jfPersonal->GetPattern( static_cast<JoinFestaScript::PatternType>(patternType) );

    pWordSet->RegisterWord( bufID, pMsgData, msgId );
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       施設種類をレジスト
 * @param       第1引数　バッファＩＤ
 * @param       第2引数　施設種類 FacilitiesType定義
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetFacilitiesType(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork       = pScript->GetWork();
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();
  print::WordSet*                               pWordSet          = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaResourceManager*  pJfResourceManager = isRoom ? pJfGimmickRoom->GetJoinFestaResourceManager() : pJfGimmick->GetJoinFestaResourceManager();


  u32 bufID       = ptr[1]; // バッファID
  JoinFestaScript::FacilitiesType facilitiesType = static_cast<JoinFestaScript::FacilitiesType>( ptr[2] ); // 施設種類

  // 施設名用のmstxt使う
  {
    gfl2::str::MsgData* pMsgData = pJfResourceManager->GetMsgData( NetApp::JoinFesta::JoinFestaResourceManager::RESOURCE_Message_JFShopName );
    u32 msgId = 0;
    // 施設IDから施設名のラベル求める
    switch( facilitiesType ){
    case JoinFestaScript::FACILITIES_TYPE_LOTTERY:    //! くじ引き屋
      msgId = jfs_type_000;
      break;
    case JoinFestaScript::FACILITIES_TYPE_AIR_TOY:    //! エア遊具
      msgId = jfs_type_001;
      break;
    case JoinFestaScript::FACILITIES_TYPE_FUN_HOUSE:  //! ビックリハウス
      msgId = jfs_type_002;
      break;
    case JoinFestaScript::FACILITIES_TYPE_STALL:      //! 屋台
      msgId = jfs_type_003;
      break;
    case JoinFestaScript::FACILITIES_TYPE_GIFT_SHOP:  //! ギフトショップ
      msgId = jfs_type_004;
      break;
    case JoinFestaScript::FACILITIES_TYPE_FORTUNE:    //! 占い
      msgId = jfs_type_005;
      break;
    case JoinFestaScript::FACILITIES_TYPE_DYEING:     //! 染物屋
      msgId = jfs_type_006;
      break;
    case JoinFestaScript::FACILITIES_TYPE_TRADE:      //! こうかんじょ(momiji追加)
      msgId = jfs_type_007;
      break;
    }
    pWordSet->RegisterWord( bufID, pMsgData, msgId );
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       施設種類をレジスト
 * @param       第1引数　バッファＩＤ
 * @param       第2引数　施設ID（全施設でユニークなID）
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetFacilitiesType2(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork       = pScript->GetWork();
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();
  print::WordSet*                               pWordSet          = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaResourceManager*  pJfResourceManager = isRoom ? pJfGimmickRoom->GetJoinFestaResourceManager() : pJfGimmick->GetJoinFestaResourceManager();
  const NetApp::JoinFesta::JoinFestaDataTable*  p_jfDataTable      = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem()->GetDataTable() : pJfGimmick->GetJoinFestaFacilitiesSystem()->GetDataTable();


  u32 bufID       = ptr[1]; // バッファID
  u32 facilitiesId= ptr[2]; // 施設ID

  // 施設名用のmstxt使う
  {
    gfl2::str::MsgData* pMsgData = pJfResourceManager->GetMsgData( NetApp::JoinFesta::JoinFestaResourceManager::RESOURCE_Message_JFShopName );
    u32 msgId = 0;

    const NetApp::JoinFesta::ShopItemInfo* pShop = p_jfDataTable->GetShopItemInfo( facilitiesId );
    GFL_ASSERT_MSG(pShop, "%d", facilitiesId);
    if( pShop != NULL )
    {
      // 施設種類から施設名のラベル求める
      switch( pShop->shopType ){
      case JoinFestaScript::FACILITIES_TYPE_LOTTERY:    //! くじ引き屋
        msgId = jfs_type_000;
        break;
      case JoinFestaScript::FACILITIES_TYPE_AIR_TOY:    //! エア遊具
        msgId = jfs_type_001;
        break;
      case JoinFestaScript::FACILITIES_TYPE_FUN_HOUSE:  //! ビックリハウス
        msgId = jfs_type_002;
        break;
      case JoinFestaScript::FACILITIES_TYPE_STALL:      //! 屋台
        msgId = jfs_type_003;
        break;
      case JoinFestaScript::FACILITIES_TYPE_GIFT_SHOP:  //! ギフトショップ
        msgId = jfs_type_004;
        break;
      case JoinFestaScript::FACILITIES_TYPE_FORTUNE:    //! 占い
        msgId = jfs_type_005;
        break;
      case JoinFestaScript::FACILITIES_TYPE_DYEING:     //! 染物屋
        msgId = jfs_type_006;
        break;
      case JoinFestaScript::FACILITIES_TYPE_TRADE:      //! こうかんじょ(momiji追加)
        msgId = jfs_type_007;
        break;
      }
      pWordSet->RegisterWord( bufID, pMsgData, msgId );
    }
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       施設名をレジスト
 * @param       第1引数　バッファＩＤ
 * @param       第2引数　施設ID（全施設でユニークなID）
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetFacilitiesName(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork       = pScript->GetWork();
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();
  print::WordSet*                               pWordSet          = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaResourceManager*  pJfResourceManager = isRoom ? pJfGimmickRoom->GetJoinFestaResourceManager() : pJfGimmick->GetJoinFestaResourceManager();
  const NetApp::JoinFesta::JoinFestaDataTable*  p_jfDataTable      = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem()->GetDataTable() : pJfGimmick->GetJoinFestaFacilitiesSystem()->GetDataTable();


  u32 bufID       = ptr[1]; // バッファID
  u32 facilitiesId= ptr[2]; // 施設ID

  // 施設名用のmstxt使う
  {
    gfl2::str::MsgData* pMsgData = pJfResourceManager->GetMsgData( NetApp::JoinFesta::JoinFestaResourceManager::RESOURCE_Message_JFShopName );
    u32 msgId = 0;
    const NetApp::JoinFesta::ShopItemInfo* pShop = p_jfDataTable->GetShopItemInfo( facilitiesId );
    GFL_ASSERT_MSG(pShop, "%d", facilitiesId);
    if( pShop != NULL )
    {
      msgId = pShop->messageID_ShopName;
    }
    pWordSet->RegisterWord( bufID, pMsgData, msgId );
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       施設紹介者のプレイヤー名をレジスト
 * @param       第1引数　バッファＩＤ
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetFacilitiesPlayerName(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*               pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                       pScriptWork         = pScript->GetWork();
  GameSys::GameManager*                           pGameManager        = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();
  print::WordSet*                                 pWordSet            = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();


  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaFacilitiesSystem*   pJfFacilitiesSystem = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem() : pJfGimmick->GetJoinFestaFacilitiesSystem();


  u32 bufID   = ptr[1]; // バッファID
  

  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  // 話しかけた動作モデルが店員かチェック
  u32 eventId = pScriptWork->GetTempWorkValue( SCWK_TARGET_OBJID );
  if( eventId >= NetApp::JoinFesta::JoinFestaDefine::ASSISTANT_EVENT_ID_OFFSET )
  {
    JoinFestaScript::FacilitiesIndex facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>( eventId - NetApp::JoinFesta::JoinFestaDefine::ASSISTANT_EVENT_ID_OFFSET);
    GFL_PRINT(">施設インデックス %d \n", facilitiesIndex);

    const NetApp::JoinFesta::JoinFestaFacilitiesData* pFacilitiesData = pJfFacilitiesSystem->GetFacilitiesData( facilitiesIndex );

    // 施設のデータから欲しい情報引っ張ってくる
    if( pFacilitiesData != NULL && pFacilitiesData->IsPlayerFlag() )
    {
      gfl2::str::StrBuf name( pFacilitiesData->GetPlayerName(), p_heap );
      pWordSet->RegisterWord( bufID, name, pWordSet->GrammerFromSex(pFacilitiesData->GetPlayerSex()) );
    }
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       施設紹介者の定型文をレジスト
 * @param       第1引数　バッファＩＤ
 * @param       第2引数　定型種類
 *                      0:あいさつ
 *                      1:別れ
 *                      2:感動
 *                      3:残念
 *                      4:ないしょ
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetFacilitiesPattern(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*               pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                       pScriptWork         = pScript->GetWork();
  GameSys::GameManager*                           pGameManager        = pScript->GetGameManager();
  Field::Fieldmap*                                pFieldmap           = pGameManager->GetFieldmap();
  print::WordSet*                                 pWordSet            = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaResourceManager*  pJfResourceManager = isRoom ? pJfGimmickRoom->GetJoinFestaResourceManager() : pJfGimmick->GetJoinFestaResourceManager();
  NetApp::JoinFesta::JoinFestaFacilitiesSystem* pJfFacilitiesSystem = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem() : pJfGimmick->GetJoinFestaFacilitiesSystem();


  u32 bufID       = ptr[1]; // バッファID
  u32 patternType = ptr[2]; // 定型種類

  // 話しかけた動作モデルが店員かチェック
  u32 eventId = pScriptWork->GetTempWorkValue( SCWK_TARGET_OBJID );
  if( eventId >= NetApp::JoinFesta::JoinFestaDefine::ASSISTANT_EVENT_ID_OFFSET )
  {
    JoinFestaScript::FacilitiesIndex facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>( eventId - NetApp::JoinFesta::JoinFestaDefine::ASSISTANT_EVENT_ID_OFFSET);
    GFL_PRINT(">施設インデックス %d \n", facilitiesIndex);

    const NetApp::JoinFesta::JoinFestaFacilitiesData* pFacilitiesData = pJfFacilitiesSystem->GetFacilitiesData( facilitiesIndex );

    // 施設のデータから欲しい情報引っ張ってくる
    if( pFacilitiesData != NULL && pFacilitiesData->IsPlayerFlag() )
    {
      gfl2::str::MsgData* pMsgData = pJfResourceManager->GetMsgData( NetApp::JoinFesta::JoinFestaResourceManager::RESOURCE_Message_JFPhrase );

      const u32 msgID = pFacilitiesData->GetPlayerPattern( static_cast<JoinFestaScript::PatternType>(patternType) );
      pWordSet->RegisterWord( bufID, pMsgData, msgID );
    }
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       施設店員のトレーナータイプをレジスト
 * @param       第1引数　バッファＩＤ
 * @param       第2引数　店員のキャラクターID
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetFacilitiesTrType(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*               pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                           pGameManager        = pScript->GetGameManager();
  print::WordSet*                                 pWordSet            = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  u32 bufID       = ptr[1]; // バッファID
  u32 characterId = ptr[2]; // キャラクターID

  u32 trtype = NetAppLib::JoinFesta::JoinFestaFacilitiesUtil::GetTrTypeFromCharacterId( characterId );

  pWordSet->RegisterTrTypeName( bufID, trtype );

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       フェスサークル名をレジスト
 * @param       第1引数　バッファＩＤ
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetFesCircleName(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject* pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*             pGameManager        = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*      pSave               = pGameManager->GetGameData()->GetJoinFestaData();
  print::WordSet*                   pWordSet            = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  u32 bufID       = ptr[1]; // バッファID

  {
    gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

    // セーブデータからフェスサークル名取ってくる
    gfl2::str::StrBuf fesName( NetApp::JoinFesta::JoinFestaDefine::STRLEN_JOIN_FESTA_NAME + ::System::EOM_LEN, p_heap );
    pSave->GetJoinFestaNameString( &fesName );
    pWordSet->RegisterWord( bufID, fesName );
  }

  return NULL;
}


//------------------------------------------------------------------------------
/**
 * @brief       アトラクション名をレジスト
 * @param       第1引数　バッファＩＤ
 * @param       第2引数　アトラクションID
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetAttractionName(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*               pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                           pGameManager        = pScript->GetGameManager();
  print::WordSet*                                 pWordSet            = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();
  NetAppLib::JoinFesta::JoinFestaAttractionManager* pAttractionManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager);

  u32 bufID         = ptr[1]; // バッファID
  u32 attractionId  = ptr[2]; // アトラクションID

  {
    // アトラクションデータ
    NetAppLib::JoinFesta::JoinFestaAttractionDefine::CoreData coreData = pAttractionManager->GetAttractionData( static_cast<JoinFestaScript::AttractionId>(attractionId) );
    pWordSet->RegisterAttractionName( bufID, coreData.attractionName );
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       アンケートアトラクションの回答をレジスト（人物・施設両対応）
 * @param       第1引数　バッファＩＤ
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetQuestionAnswer(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*               pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                       pScriptWork         = pScript->GetWork();
  GameSys::GameManager*                           pGameManager        = pScript->GetGameManager();
  Field::Fieldmap*                                pFieldmap           = pGameManager->GetFieldmap();
  print::WordSet*                                 pWordSet            = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();
  NetAppLib::JoinFesta::JoinFestaAttractionManager* pAttractionManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager);


  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaFacilitiesSystem* pJfFacilitiesSystem = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem() : pJfGimmick->GetJoinFestaFacilitiesSystem();


  u32 bufID       = ptr[1]; // バッファID
  u32 eventId = pScriptWork->GetTempWorkValue( SCWK_TARGET_OBJID );
  JoinFestaScript::AttractionId attractionId = pAttractionManager->GetPlayAttractionId();

  // アンケートアトラクションに対応したアンケートID取得
  const u8 questionnaireId = NetAppLib::JoinFesta::JoinFestaAttractionUtil::GetQuestionnaireIdFromAttractionId( attractionId );

  {
    gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
    gfl2::str::MsgData* pMsgData = GFL_NEW( pHeap ) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_jf_questiondata_DAT, pHeap );

    u32 msgId=0;
    u8 sex = PM_MALE;
    
    // 話しかけた相手のJFパーソナルデータ
    NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetScriptTalkFieldPersonalData(); 
    if( jfPersonal != NULL )
    {
      NetAppLib::JoinFesta::PacketGameStatus1 packet;
      if( jfPersonal->GetPacketGameStatus1( &packet ) )
      {
        msgId = pAttractionManager->GetQuestionAnswerMsgId( attractionId, packet.questionnaireAnswer[ questionnaireId ] );
        sex = jfPersonal->GetSex();
      }
    }
    // 話しかけた動作モデルが店員かチェック
    else if( eventId >= NetApp::JoinFesta::JoinFestaDefine::ASSISTANT_EVENT_ID_OFFSET )
    {
      JoinFestaScript::FacilitiesIndex facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>( eventId - NetApp::JoinFesta::JoinFestaDefine::ASSISTANT_EVENT_ID_OFFSET);
      GFL_PRINT(">施設インデックス %d \n", facilitiesIndex);

      const NetApp::JoinFesta::JoinFestaFacilitiesData* pFacilitiesData = pJfFacilitiesSystem->GetFacilitiesData( facilitiesIndex );

      // 施設のデータから欲しい情報引っ張ってくる
      if( pFacilitiesData != NULL  )
      {
        // 回答数
        const u8 answerMax = NetAppLib::JoinFesta::JoinFestaAttractionUtil::GetAnswerMaxFromQuestionnaireId( questionnaireId );

        // 施設IDと店員キャラクターIDと施設インデックスからオフセット求める
        u32 msgIdOffset = pFacilitiesData->GetFacilitiesId() + pFacilitiesData->GetCharacterId() + facilitiesIndex;
        msgIdOffset = msgIdOffset % answerMax;

        switch( questionnaireId ){
        case 0: msgId = jf_question_sel_q_00_ans_01; break;
        case 1: msgId = jf_question_sel_q_01_ans_01; break;
        case 2: msgId = jf_question_sel_q_02_ans_01; break;
        case 3: msgId = jf_question_sel_q_03_ans_01; break;
        case 4: msgId = jf_question_sel_q_04_ans_01; break;
        case 5: msgId = jf_question_sel_q_05_ans_01; break;
        case 6: msgId = jf_question_sel_q_06_ans_01; break;
        case 7: msgId = jf_question_sel_q_07_ans_01; break;
        case 8: msgId = jf_question_sel_q_08_ans_01; break;
        case 9: msgId = jf_question_sel_q_09_ans_01; break;
        case 10:msgId = jf_question_sel_q_10_ans_01; break;
        case 11:msgId = jf_question_sel_q_11_ans_01; break;
        case 12:msgId = jf_question_sel_q_12_ans_01; break;
        case 13:msgId = jf_question_sel_q_13_ans_01; break;
        case 14:msgId = jf_question_sel_q_14_ans_01; break;
        case 15:msgId = jf_question_sel_q_15_ans_01; break;
        default:
          GFL_ASSERT_MSG(0, "questionId:%d\n", questionnaireId );
          break;
        }

        msgId += msgIdOffset;
        sex = pFacilitiesData->GetAssistantSex();
      }
    }

    // ローカライズ対応。男女メッセージ分け
    if( msgId != jf_question_sel_noans )
    {
      const int offsetSex = ( sex == PM_MALE ) ? 0 : 12 ;
      msgId += offsetSex;
    }

    pWordSet->RegisterWord( bufID, pMsgData, msgId );
    GFL_SAFE_DELETE( pMsgData );
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       アンケート回答をレジスト
 * @param       第1引数　バッファＩＤ
 * @param       第2引数　アンケートID
 * @param       第3引数　アンケート回答ID(ナイショが0)
 * @param       第4引数　性別
 * @param       第5引数　0:一人称 1:二人称 通常は一人称
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetQuestionAnswer2(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*               pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                           pGameManager        = pScript->GetGameManager();
  print::WordSet*                                 pWordSet            = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  u32 bufID       = ptr[1]; // バッファID
  u32 questionId  = ptr[2]; // アンケートID
  u32 questionAnswerId = ptr[3]; // アンケート回答ID
  u32 sex         = ptr[4]; // 性別
  u32 person      = ptr[5]; // 人称

  {
    u32 msgId=0;
    gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
    gfl2::str::MsgData* pMsgData = GFL_NEW( pHeap ) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_jf_questiondata_DAT, pHeap );

    // アンケートID
    // 一人称と二人称で分ける
    if( person == 0 )
    {
      // 一人称
      switch( questionId ){
      case 0: msgId = jf_question_sel_q_00_ans_01; break;
      case 1: msgId = jf_question_sel_q_01_ans_01; break;
      case 2: msgId = jf_question_sel_q_02_ans_01; break;
      case 3: msgId = jf_question_sel_q_03_ans_01; break;
      case 4: msgId = jf_question_sel_q_04_ans_01; break;
      case 5: msgId = jf_question_sel_q_05_ans_01; break;
      case 6: msgId = jf_question_sel_q_06_ans_01; break;
      case 7: msgId = jf_question_sel_q_07_ans_01; break;
      case 8: msgId = jf_question_sel_q_08_ans_01; break;
      case 9: msgId = jf_question_sel_q_09_ans_01; break;
      case 10:msgId = jf_question_sel_q_10_ans_01; break;
      case 11:msgId = jf_question_sel_q_11_ans_01; break;
      case 12:msgId = jf_question_sel_q_12_ans_01; break;
      case 13:msgId = jf_question_sel_q_13_ans_01; break;
      case 14:msgId = jf_question_sel_q_14_ans_01; break;
      case 15:msgId = jf_question_sel_q_15_ans_01; break;
      default:
        GFL_ASSERT_MSG(0, "questionId:%d\n", questionId );
        break;
      }
    }
    else
    {
      // 二人称
      switch( questionId ){
      case 0: msgId = jf_question_sel_q_00_ans_01_2p; break;
      case 1: msgId = jf_question_sel_q_01_ans_01_2p; break;
      case 2: msgId = jf_question_sel_q_02_ans_01_2p; break;
      case 3: msgId = jf_question_sel_q_03_ans_01_2p; break;
      case 4: msgId = jf_question_sel_q_04_ans_01_2p; break;
      case 5: msgId = jf_question_sel_q_05_ans_01_2p; break;
      case 6: msgId = jf_question_sel_q_06_ans_01_2p; break;
      case 7: msgId = jf_question_sel_q_07_ans_01_2p; break;
      case 8: msgId = jf_question_sel_q_08_ans_01_2p; break;
      case 9: msgId = jf_question_sel_q_09_ans_01_2p; break;
      case 10:msgId = jf_question_sel_q_10_ans_01_2p; break;
      case 11:msgId = jf_question_sel_q_11_ans_01_2p; break;
      case 12:msgId = jf_question_sel_q_12_ans_01_2p; break;
      case 13:msgId = jf_question_sel_q_13_ans_01_2p; break;
      case 14:msgId = jf_question_sel_q_14_ans_01_2p; break;
      case 15:msgId = jf_question_sel_q_15_ans_01_2p; break;
      default:
        GFL_ASSERT_MSG(0, "questionId:%d\n", questionId );
        break;
      }
    }
    

    // アンケート回答分と性別を足す
    const int offsetSex = ( sex == PM_MALE ) ? 0 : 12 ;
    msgId += questionAnswerId + offsetSex;

    pWordSet->RegisterWord( bufID, pMsgData, msgId );
    GFL_SAFE_DELETE( pMsgData );
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       アンケートの質問をレジスト
 * @param       第1引数　バッファＩＤ
 * @param       第2引数　アンケートのmsgId
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetQuestion(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*               pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                           pGameManager        = pScript->GetGameManager();
  print::WordSet*                                 pWordSet            = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  u32 bufID       = ptr[1]; // バッファID
  u32 msgId       = ptr[2]; // MSGID
  {
    gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
    gfl2::str::MsgData* pMsgData = GFL_NEW( pHeap ) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_jf_questiondata_DAT, pHeap );
    pWordSet->RegisterWord( bufID, pMsgData, msgId );
    GFL_SAFE_DELETE( pMsgData );
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       染める色名をレジスト
 * @param       第1引数　バッファＩＤ
 * @param       第2引数　色ID DRESSUP_COLOR_ID_CLOTH_XXX
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetDyeColor(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*               pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                           pGameManager        = pScript->GetGameManager();
  print::WordSet*                                 pWordSet            = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  u32 bufID       = ptr[1]; // バッファID
  u32 colorId     = ptr[2]; // 色ID DRESSUP_COLOR_ID_CLOTH_XXX
  {
    const u32 colorMessageId = app::util::Kisekae::KisekaeDataBase::GetColorMsgIdForColorId( colorId );
    gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
    gfl2::str::MsgData* pMsgData = GFL_NEW( pHeap ) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_kisekae_color_DAT, pHeap );
    pWordSet->RegisterWord( bufID, pMsgData, colorMessageId );
    GFL_SAFE_DELETE( pMsgData );
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       定型文をレジスト
 * @param       第1引数　バッファＩＤ
 * @param       第2引数　定型文のメッセージID(男用を入れる。内部で男女処理している）
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetSelectWord(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork       = pScript->GetWork();
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();
  print::WordSet*                               pWordSet          = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();
  Savedata::MyStatus*                           pMyStatus         = pGameManager->GetGameData()->GetPlayerStatus();   

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaResourceManager*  pJfResourceManager = isRoom ? pJfGimmickRoom->GetJoinFestaResourceManager() : pJfGimmick->GetJoinFestaResourceManager();


  u32 bufID       = ptr[1]; // バッファID
  u32 patternMessageId = ptr[2]; // 定型文メッセージID

  // 女性の場合は、女性用メッセージに合わせる
  if( pMyStatus->GetSex() == PM_FEMALE )
  {
    patternMessageId += (msg_jf_phrase_max/2);
  }

  if( patternMessageId >= msg_jf_phrase_max )
  {
    GFL_ASSERT(0);
    patternMessageId = 0;
  }

  gfl2::str::MsgData* pMsgData = pJfResourceManager->GetMsgData( NetApp::JoinFesta::JoinFestaResourceManager::RESOURCE_Message_JFPhrase );
  pWordSet->RegisterWord( bufID, pMsgData, patternMessageId );

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       通信アプリ名をレジスト
 * @param       第1引数　バッファＩＤ
 * @param       第2引数　CommRouteType
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetCommRoute(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*               pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                           pGameManager        = pScript->GetGameManager();
  print::WordSet*                                 pWordSet            = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  u32 bufID       = ptr[1]; // バッファID
  JoinFestaScript::CommRouteType commRoute = static_cast<JoinFestaScript::CommRouteType>(ptr[2]);
  {
    u32 messageId = 0;

    // 占い結果の行動によりメッセージ決める
    // ローカライズ要望で、過去形のアプリ名単語になる。
    switch( commRoute ){
    case JoinFestaScript::COMM_ROUTE_GTS:           messageId = net_app_name_10; break;
    case JoinFestaScript::COMM_ROUTE_MIRACLE_TRADE: messageId = net_app_name_11; break;
    case JoinFestaScript::COMM_ROUTE_RANDOM_BATTLE: messageId = net_app_name_12; break;
    case JoinFestaScript::COMM_ROUTE_P2P_TRADE:     messageId = net_app_name_13; break;
    case JoinFestaScript::COMM_ROUTE_P2P_BATTLE:    messageId = net_app_name_14; break;
    case JoinFestaScript::COMM_ROUTE_QUICK:         messageId = net_app_name_19; break;  //!< クイック通信（GFNMCat[1628]対処）
    default:
      GFL_ASSERT_MSG( 0,"%d",commRoute);
      break;
    }

    gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
    gfl2::str::MsgData* pMsgData = GFL_NEW( pHeap ) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_net_app_name_DAT, pHeap );
    pWordSet->RegisterWord( bufID, pMsgData, messageId );
    GFL_SAFE_DELETE( pMsgData );
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       占い結果の行動をレジスト
 * @param       第1引数　バッファＩＤ
 * @param       第2引数　FortuneResultAction
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetFortuneAction(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*               pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                           pGameManager        = pScript->GetGameManager();
  print::WordSet*                                 pWordSet            = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  u32 bufID       = ptr[1]; // バッファID
  JoinFestaScript::FortuneResultAction action = static_cast<JoinFestaScript::FortuneResultAction>(ptr[2]);
  {
    u32 messageId = 0;

    // 占い結果の行動によりメッセージ決める
    switch( action ){
    case JoinFestaScript::FORTUNE_RESULT_ACTION_GTS:        messageId = net_app_name_01; break;
    case JoinFestaScript::FORTUNE_RESULT_ACTION_MIRACLE:    messageId = net_app_name_02; break;
    case JoinFestaScript::FORTUNE_RESULT_ACTION_BTLSPOT:    messageId = net_app_name_03; break;
    case JoinFestaScript::FORTUNE_RESULT_ACTION_FESCIRCLE:  messageId = net_app_name_06; break;
    case JoinFestaScript::FORTUNE_RESULT_ACTION_ATTRACTION: messageId = net_app_name_07; break;
    case JoinFestaScript::FORTUNE_RESULT_ACTION_LOTTERY:    messageId = net_app_name_08; break;
    case JoinFestaScript::FORTUNE_RESULT_ACTION_FUN_HOUSE:  messageId = net_app_name_09; break;
    default:
      GFL_ASSERT_MSG( 0,"%d",action);
      break;
    }

    gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
    gfl2::str::MsgData* pMsgData = GFL_NEW( pHeap ) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_net_app_name_DAT, pHeap );
    pWordSet->RegisterWord( bufID, pMsgData, messageId );
    GFL_SAFE_DELETE( pMsgData );
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       着せ替えアイテム名をレジスト
 * @param       第1引数　バッファＩＤ
 * @param       第2引数　sex
 * @param       第3引数　category   (Savedata::Fashion::Category)
 * @param       第4引数　itemId
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetKisekaeItemName(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*               pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                           pGameManager        = pScript->GetGameManager();
  print::WordSet*                                 pWordSet            = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  u32                                 bufID     = ptr[1]; // バッファID
  poke_3d::model::DressUpParam::Sex   sex       = static_cast<poke_3d::model::DressUpParam::Sex>( ptr[2] ); // 性別
  Savedata::Fashion::Category         category  = static_cast<Savedata::Fashion::Category>(ptr[3]); // カテゴリ
  u32                                 itemId    = ptr[4]; // 着せ替えアイテムID
  {
    const table_data_GoodsData* pGoodsData = app::util::Kisekae::KisekaeDataBase::GetGoodsDataSearchItemId( sex, category, itemId );

    if( pGoodsData )
    {
      pWordSet->RegisterKisekaeItemName( bufID, pGoodsData->goods_txt_id, 1 );
    }
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       リクルーターで呼んだ人物名をレジスト
 * @param       第1引数　バッファＩＤ
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetRecruiterName(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript         = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager    = pScript->GetGameManager();
  print::WordSet*                     pWordSet        = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  // リクルーターのJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData& jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetRecruiterPersonal();

  u32 bufID       = ptr[1]; // バッファID

  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  // パーソナルデータから名前と性別取ってくる
  gfl2::str::StrBuf name( jfPersonal.GetName(), p_heap );
  pWordSet->RegisterWord( bufID, name, pWordSet->GrammerFromSex( jfPersonal.GetSex() ) );

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       最後にP2Pで遊んだ人物名をレジスト
 * @param       第1引数　バッファＩＤ
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetLastP2pName(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript         = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager    = pScript->GetGameManager();
  print::WordSet*                     pWordSet        = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  // LastP2pのJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData& jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetLastP2pPersonal();

  u32 bufID       = ptr[1]; // バッファID

  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  // パーソナルデータから名前と性別取ってくる
  gfl2::str::StrBuf name( jfPersonal.GetName(), p_heap );
  pWordSet->RegisterWord( bufID, name, pWordSet->GrammerFromSex( jfPersonal.GetSex() ) );

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       リクルーターで呼んだ人物の定型文をレジスト
 * @param       第1引数　バッファＩＤ
 * @param       第2引数　定型種類
 * @return      リクルーターで呼んだ人物の性別 
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetRecruiterPattern(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();
  print::WordSet*                               pWordSet          = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaResourceManager*  pJfResourceManager = isRoom ? pJfGimmickRoom->GetJoinFestaResourceManager() : pJfGimmick->GetJoinFestaResourceManager();



  u32 bufID       = ptr[1]; // バッファID
  u32 patternType = ptr[2]; // 定型種類

  // リクルーターのJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData& jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetRecruiterPersonal();

  // 定型用のmstxt使う
  gfl2::str::MsgData* pMsgData = pJfResourceManager->GetMsgData( NetApp::JoinFesta::JoinFestaResourceManager::RESOURCE_Message_JFPhrase );

  // 定型文のラベル求める
  u32 msgId = jfPersonal.GetPattern( static_cast<JoinFestaScript::PatternType>(patternType) );

  pWordSet->RegisterWord( bufID, pMsgData, msgId );
  return jfPersonal.GetSex();
}

//------------------------------------------------------------------------------
/**
 * @brief       レコード名をレジスト
 * @param       第1引数　バッファＩＤ
 * @param       第2引数　レコードカテゴリ
 * @param       第3引数　カテゴリ内インデックス
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetRecordName2(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript         = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager    = pScript->GetGameManager();
  print::WordSet*                     pWordSet        = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  u32 bufID       = ptr[1];     // バッファID
  u32 recordCategory = ptr[2];  // レコードカテゴリ
  u32 index = ptr[3];           // カテゴリ内インデックス
  
  u32 messageId = 0;

  switch( recordCategory ){
  case JoinFestaScript::RECORD_CATEGORY_BATTLE: messageId = jf_playerlist_cap_rec_01_01 + index; break;
  case JoinFestaScript::RECORD_CATEGORY_TRADE:  messageId = jf_playerlist_cap_rec_02_01 + index; break;
  case JoinFestaScript::RECORD_CATEGORY_GREED:  messageId = jf_playerlist_cap_rec_03_01 + index; break;
  case JoinFestaScript::RECORD_CATEGORY_POKEMON:messageId = jf_playerlist_cap_rec_04_01 + index; break;
  case JoinFestaScript::RECORD_CATEGORY_DAILY:  messageId = jf_playerlist_cap_rec_05_01 + index; break;
  case JoinFestaScript::RECORD_CATEGORY_CIRCLE: messageId = jf_playerlist_cap_rec_06_01 + index; break;
  case JoinFestaScript::RECORD_CATEGORY_VARIETY:messageId = jf_playerlist_cap_rec_07_01 + index; break;
  default:
    GFL_ASSERT(0);
    messageId = jf_playerlist_cap_rec_01_01;
    break;
  }

  gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  gfl2::str::MsgData* pMsgData = GFL_NEW( pHeap ) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_jf_playerlist_DAT, pHeap );
  pWordSet->RegisterWord( bufID, pMsgData, messageId );
  GFL_SAFE_DELETE( pMsgData );

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       レコードカテゴリ名をレジスト（XXXきろくの方）
 * @param       第1引数　バッファＩＤ
 * @param       第2引数　レコードカテゴリ
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetRecordCategoryName(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript         = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager    = pScript->GetGameManager();
  print::WordSet*                     pWordSet        = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  u32 bufID       = ptr[1]; // バッファID
  JoinFestaScript::RecordCategory recordCategory = static_cast<JoinFestaScript::RecordCategory>(ptr[2]); // レコードカテゴリ

  u32 messageId = jf_playerlist_cap_29;

  switch( recordCategory ){
  case JoinFestaScript::RECORD_CATEGORY_BATTLE:   messageId = jf_playerlist_cap_29; break;
  case JoinFestaScript::RECORD_CATEGORY_TRADE:    messageId = jf_playerlist_cap_30; break;
  case JoinFestaScript::RECORD_CATEGORY_GREED:    messageId = jf_playerlist_cap_31; break;
  case JoinFestaScript::RECORD_CATEGORY_POKEMON:  messageId = jf_playerlist_cap_32; break;
  case JoinFestaScript::RECORD_CATEGORY_DAILY:    messageId = jf_playerlist_cap_33; break;
  case JoinFestaScript::RECORD_CATEGORY_CIRCLE:   messageId = jf_playerlist_cap_34; break;
  case JoinFestaScript::RECORD_CATEGORY_VARIETY:  messageId = jf_playerlist_cap_35; break;
  }

  gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  gfl2::str::MsgData* pMsgData = GFL_NEW( pHeap ) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_jf_playerlist_DAT, pHeap );
  pWordSet->RegisterWord( bufID, pMsgData, messageId );
  GFL_SAFE_DELETE( pMsgData );

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       レコードカテゴリ名をレジスト（XXXランキングの方）
 * @param       第1引数　バッファＩＤ
 * @param       第2引数　レコードカテゴリ
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetRecordCategoryName2(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript         = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager    = pScript->GetGameManager();
  print::WordSet*                     pWordSet        = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  u32 bufID       = ptr[1]; // バッファID
  JoinFestaScript::RecordCategory recordCategory = static_cast<JoinFestaScript::RecordCategory>(ptr[2]); // レコードカテゴリ

  u32 messageId = jf_playerlist_cap_21;

  switch( recordCategory ){
  case JoinFestaScript::RECORD_CATEGORY_BATTLE:   messageId = jf_playerlist_cap_21; break;
  case JoinFestaScript::RECORD_CATEGORY_TRADE:    messageId = jf_playerlist_cap_22; break;
  case JoinFestaScript::RECORD_CATEGORY_GREED:    messageId = jf_playerlist_cap_23; break;
  case JoinFestaScript::RECORD_CATEGORY_POKEMON:  messageId = jf_playerlist_cap_24; break;
  case JoinFestaScript::RECORD_CATEGORY_DAILY:    messageId = jf_playerlist_cap_25; break;
  case JoinFestaScript::RECORD_CATEGORY_CIRCLE:   messageId = jf_playerlist_cap_26; break;
  case JoinFestaScript::RECORD_CATEGORY_VARIETY:  messageId = jf_playerlist_cap_27; break;
  }

  gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  gfl2::str::MsgData* pMsgData = GFL_NEW( pHeap ) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_jf_playerlist_DAT, pHeap );
  pWordSet->RegisterWord( bufID, pMsgData, messageId );
  GFL_SAFE_DELETE( pMsgData );

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       スクリプトで選んだ人物名をレジスト
 * @param       第1引数　バッファＩＤ
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetScriptSelectName(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript         = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager    = pScript->GetGameManager();
  print::WordSet*                     pWordSet        = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  // スクリプトで選んだ人のJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetScriptSelectPersonal();

  u32 bufID       = ptr[1]; // バッファID

  if( jfPersonal != NULL )
  {
    gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

    // パーソナルデータから名前と性別取ってくる
    gfl2::str::StrBuf name( jfPersonal->GetName(), p_heap );
    pWordSet->RegisterWord( bufID, name, pWordSet->GrammerFromSex( jfPersonal->GetSex() ) );
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       スクリプトで選んだ人物の定型文をレジスト
 * @param       第1引数　バッファＩＤ
 * @param       第2引数　定型種類
 * @return      スクリプトで選んだ人物の性別 
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetScriptSelectPattern(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();
  print::WordSet*                               pWordSet          = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaResourceManager*  pJfResourceManager = isRoom ? pJfGimmickRoom->GetJoinFestaResourceManager() : pJfGimmick->GetJoinFestaResourceManager();


  u32 bufID       = ptr[1]; // バッファID
  u32 patternType = ptr[2]; // 定型種類

  // スクリプトで選んだ人のJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetScriptSelectPersonal();

  // 定型用のmstxt使う
  if( jfPersonal != NULL )
  {
    gfl2::str::MsgData* pMsgData = pJfResourceManager->GetMsgData( NetApp::JoinFesta::JoinFestaResourceManager::RESOURCE_Message_JFPhrase );

    // 定型文のラベル求める
    u32 msgId = jfPersonal->GetPattern( static_cast<JoinFestaScript::PatternType>(patternType) );

    pWordSet->RegisterWord( bufID, pMsgData, msgId );

    return jfPersonal->GetSex();
  }

  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief       ゲームシンクIDコードをレジスト
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetGameSyncIdCode(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  print::WordSet*                               pWordSet          = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();
  Savedata::MyStatus*                           pMyStatus         = pGameManager->GetGameData()->GetPlayerStatus();   


  {
    gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
    gfl2::str::StrBuf strCode( NetApp::Util::ConvertUtility::STR_MAX+System::EOM_LEN, p_heap );
    NetApp::Util::ConvertUtility::U64ToStr( pMyStatus->GetNexUniqueID(), &strCode, false );

    const gfl2::str::STRCODE* pGameSyncID = strCode.GetPtr();

    gfl2::str::STRCODE gameSyncID1[5];
    gfl2::str::STRCODE gameSyncID2[5];
    gfl2::str::STRCODE gameSyncID3[5];
    gfl2::str::STRCODE gameSyncID4[5];

    gfl2::std::MemClear( gameSyncID1, sizeof( gameSyncID1 ) );
    gfl2::std::MemClear( gameSyncID2, sizeof( gameSyncID2 ) );
    gfl2::std::MemClear( gameSyncID3, sizeof( gameSyncID3 ) );
    gfl2::std::MemClear( gameSyncID4, sizeof( gameSyncID4 ) );

    gfl2::std::MemCopy( &pGameSyncID[0], &gameSyncID1[0], sizeof( gfl2::str::STRCODE ) * 4 );
    gfl2::std::MemCopy( &pGameSyncID[4], &gameSyncID2[0], sizeof( gfl2::str::STRCODE ) * 4 );
    gfl2::std::MemCopy( &pGameSyncID[8], &gameSyncID3[0], sizeof( gfl2::str::STRCODE ) * 4 );
    gfl2::std::MemCopy( &pGameSyncID[12], &gameSyncID4[0], sizeof( gfl2::str::STRCODE ) * 4 );
    gameSyncID1[4] = gfl2::str::EOM_CODE;
    gameSyncID2[4] = gfl2::str::EOM_CODE;
    gameSyncID3[4] = gfl2::str::EOM_CODE;
    gameSyncID4[4] = gfl2::str::EOM_CODE;


    strCode.Clear();
    strCode.SetStr(gameSyncID1);
    pWordSet->RegisterWord( 0, strCode );
    strCode.Clear();
    strCode.SetStr(gameSyncID2);
    pWordSet->RegisterWord( 1, strCode );
    strCode.Clear();
    strCode.SetStr(gameSyncID3);
    pWordSet->RegisterWord( 2, strCode );
    strCode.Clear();
    strCode.SetStr(gameSyncID4);
    pWordSet->RegisterWord( 3, strCode );
  }

  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        AddFesCoin
 * @brief       フェスコイン追加
 * @param       第1引数　追加したいコイン数
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL AddFesCoin(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject* pScript       = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*             pGameManager  = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*      pSave         = pGameManager->GetGameData()->GetJoinFestaData();

  u32 addCoin = ptr[1];
  pSave->AddCoin( addCoin ); 

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        SubFesCoin
 * @brief       フェスコイン減らす
 * @param       第1引数　減らすしたいコイン数
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL SubFesCoin(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject* pScript       = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*             pGameManager  = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*      pSave         = pGameManager->GetGameData()->GetJoinFestaData();

  u32 subCoin = ptr[1];
  pSave->SubCoin( subCoin ); 

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        GetFestaCoin
 * @brief       獲得フェスコインの取得
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetFesCoin(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject* pScript       = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*             pGameManager  = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*      pSave         = pGameManager->GetGameData()->GetJoinFestaData();

  return pSave->GetCoin();
}

//------------------------------------------------------------------------------
/**
 * @func        GetTotalFesCoin
 * @brief       累計獲得フェスコインの取得
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetTotalFesCoin(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject* pScript       = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*             pGameManager  = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*      pSave         = pGameManager->GetGameData()->GetJoinFestaData();

  return pSave->GetTotalCoin();
}

//------------------------------------------------------------------------------
/**
 * @func        SetFesRank
 * @brief       フェスランク設定
 * @param       第1引数　セットするランク値
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL SetFesRank(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject* pScript       = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*             pGameManager  = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*      pSave         = pGameManager->GetGameData()->GetJoinFestaData();

  u16 rank = ptr[1];
  pSave->SetRank( rank );

  return NULL;
}
//------------------------------------------------------------------------------
/**
 * @func        GetFesRank
 * @brief       フェスランク取得
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetFesRank(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject* pScript       = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*             pGameManager  = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*      pSave         = pGameManager->GetGameData()->GetJoinFestaData();

  return pSave->GetRank();
}

//------------------------------------------------------------------------------
/**
 * @func        CheckFesRankUp
 * @brief       フェスランクがアップしてよいかチェック
 * @return      trueでランクアップ可
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL CheckFesRankUp(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject* pScript       = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*             pGameManager  = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*      pSave         = pGameManager->GetGameData()->GetJoinFestaData();

  return ( pSave->GetCoinNecessaryForNextRank() == 0 );
}

//------------------------------------------------------------------------------
/**
 * @func        SetFesReward
 * @brief       ご褒美イベントセット
 * @param       第1引数　ご褒美イベントセットするインデックス(0-9)
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL SetFesReward(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject* pScript       = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*             pGameManager  = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*      pSave         = pGameManager->GetGameData()->GetJoinFestaData();

  JoinFestaScript::RewardType index = static_cast<JoinFestaScript::RewardType>(ptr[1]);

  // 初期状態の場合、発動状態にする
  if( pSave->GetRewardState( index ) == JoinFestaScript::REWARD_STATE_NULL )
  {
    pSave->SetRewardState( index, JoinFestaScript::REWARD_STATE_SATISFY_CONDITION );
  }

  // お店授与のイベントは何度も起こり、上書きされる仕様
  if( index == JoinFestaScript::REWARD_PRESENT_SHOP )
  {
    pSave->SetRewardState( index, JoinFestaScript::REWARD_STATE_SATISFY_CONDITION );
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        GetFesRewardState
 * @brief       ご褒美イベント状態取得
 * @param       第1引数　ご褒美イベント取得するインデックス(0-9)
 * @reutrn      状態(0:初期 1:条件満たした 2:ご褒美イベントこなした )
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetFesRewardState(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject* pScript       = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*             pGameManager  = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*      pSave         = pGameManager->GetGameData()->GetJoinFestaData();

  JoinFestaScript::RewardType index = static_cast<JoinFestaScript::RewardType>(ptr[1]);
  return pSave->GetRewardState( index );
}

//------------------------------------------------------------------------------
/**
 * @func        FesRewardFinish
 * @brief       ご褒美イベント終了
 * @param       第1引数　ご褒美イベント数量するインデックス(0-9)
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FesRewardFinish(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject* pScript       = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*             pGameManager  = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*      pSave         = pGameManager->GetGameData()->GetJoinFestaData();
  Field::Fieldmap*                  pFieldmap     = pGameManager->GetFieldmap();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  const NetApp::JoinFesta::JoinFestaDataTable*  p_jfDataTable      = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem()->GetDataTable() : pJfGimmick->GetJoinFestaFacilitiesSystem()->GetDataTable();


  JoinFestaScript::RewardType index = static_cast<JoinFestaScript::RewardType>(ptr[1]);
  
  // 発動状態の場合、終了させる
  if( pSave->GetRewardState( index ) == JoinFestaScript::REWARD_STATE_SATISFY_CONDITION )
  {
    pSave->SetRewardState( index, JoinFestaScript::REWARD_STATE_FINISH );

    // 定型文授与の場合は、定型文を開放する
    if( index == JoinFestaScript::REWARD_SELECT_WORD )
    {
      pSave->SetUseSelectWord( msg_jf_phrase_m_250 );
    }
    // @fix GFNMcat[3593]：プレミアムサービス追加時は購入状態をリセット
    // @fix GFNMCat[3763]：ペナルティフラグON時はリセットしない
    else if( index == JoinFestaScript::REWARD_PREMIUM && !pGameManager->GetGameData()->GetGameTime()->IsPenaltyMode() )
    {
      for( int i=0; i<JoinFestaScript::FACILITIES_INDEX_MAX; i++ )
      {
        NetApp::JoinFesta::JoinFestaFacilitiesData::CoreData* pCoreData = pSave->GetFacilitiesData( static_cast<JoinFestaScript::FacilitiesIndex>(i) );
        const NetApp::JoinFesta::ShopItemInfo* pShop = p_jfDataTable->GetShopItemInfo( pCoreData->facilitiesId );
        GFL_ASSERT_MSG(pShop, "%d", pCoreData->facilitiesId);

        if( pShop != NULL )
        {
          // 屋台、ギフトショップ、エアバルーンのみ
          switch( pShop->shopType ){
          case JoinFestaScript::FACILITIES_TYPE_AIR_TOY:    //! エア遊具
          case JoinFestaScript::FACILITIES_TYPE_STALL:      //! 屋台
          case JoinFestaScript::FACILITIES_TYPE_GIFT_SHOP:  //! ギフトショップ
            pCoreData->itemBuyBit = 0;
            break;
          }
        }
      }
    }
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        SetFacilitiesBuy
 * @brief       施設の商品購入
 * @param       第1引数　施設インデックス FacilitiesIndex
 * @param       第2引数　購入する商品ID（リストの順）
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL SetFacilitiesBuy(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject* pScript       = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*             pGameManager  = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*      pSave         = pGameManager->GetGameData()->GetJoinFestaData();

  JoinFestaScript::FacilitiesIndex facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>(ptr[1]);
  u32 itemNo = ptr[2];

  pSave->SetFacilitiesIndexOfScriptCall( facilitiesIndex );
  pSave->SetFacilitiesBuy( itemNo );
  // 無効値設定
  pSave->SetFacilitiesIndexOfScriptCall( JoinFestaScript::FACILITIES_INDEX_MAX );

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        IsFacilitiesBuy
 * @brief       施設の商品購入してあるかチェック
 * @param       第1引数　施設インデックス FacilitiesIndex
 * @param       第2引数　購入する商品ID（リストの順）
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL IsFacilitiesBuy(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject* pScript       = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*             pGameManager  = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*      pSave         = pGameManager->GetGameData()->GetJoinFestaData();

  JoinFestaScript::FacilitiesIndex facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>(ptr[1]);
  u32 itemNo = ptr[2];

  pSave->SetFacilitiesIndexOfScriptCall( facilitiesIndex );
  bool ret = pSave->IsFacilitiesBuy( itemNo );
  
  // 無効値設定
  pSave->SetFacilitiesIndexOfScriptCall( JoinFestaScript::FACILITIES_INDEX_MAX );

  return ret;
}

//------------------------------------------------------------------------------
/**
 * @func        SetFesTheme
 * @brief       フェスのテーマを設定する
 * @param       第1引数　テーマ(0-3)
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL SetFesTheme(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject* pScript       = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*             pGameManager  = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*      pSave         = pGameManager->GetGameData()->GetJoinFestaData();

  u32 theme = ptr[1];
  pSave->SetTheme( theme );
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        GetFesTheme
 * @brief       フェスのテーマを取得する
 * @return      テーマ(0-3)
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetFesTheme(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject* pScript       = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*             pGameManager  = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*      pSave         = pGameManager->GetGameData()->GetJoinFestaData();

  return pSave->GetTheme();
}

//------------------------------------------------------------------------------
/**
 * @func        SetTempSaveFesTheme
 * @brief       一時保存用テーマを設定する
 * @param       第1引数　テーマ(0-3)
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL SetTempSaveFesTheme(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject* pScript       = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*             pGameManager  = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*      pSave         = pGameManager->GetGameData()->GetJoinFestaData();

  u32 theme = ptr[1];
  pSave->SetTempSaveTheme( theme );
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        GetTempSaveFesTheme
 * @brief       一時保存用テーマを取得する
 * @return      テーマ(0-3)
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetTempSaveFesTheme(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject* pScript       = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*             pGameManager  = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*      pSave         = pGameManager->GetGameData()->GetJoinFestaData();

  return pSave->GetTempSaveTheme();
}

//------------------------------------------------------------------------------
/**
 * @func   SetQuestionnaireAnswer
 * @brief  アンケート回答を設定する
 * @param  第1引数   questionId 質問ID
 * @param  第2引数   answer     答え
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL SetQuestionnaireAnswer(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject* pScript       = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*             pGameManager  = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*      pSave         = pGameManager->GetGameData()->GetJoinFestaData();

  u32 questionId  = ptr[1];
  u32 answer      = ptr[2];

  pSave->SetQuestionnaireAnswer( questionId, answer );

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief  ポケパラ系施設の注意喚起メッセージのスキップフラグを設定
 * @param  第1引数   0:エアバルーン 1:屋台 2:ホラーハウス
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL SetPokeParaSkipMessage(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject* pScript       = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*             pGameManager  = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*      pSave         = pGameManager->GetGameData()->GetJoinFestaData();

  u32 facilities = ptr[1];

  if( facilities == 0 )
  {
    pSave->SetIsSkipMessageAirToy( true );
  }
  else if( facilities == 1 )
  {
    pSave->SetIsSkipMessageStall( true );
  }
  else
  {
    pSave->SetIsSkipMessageHorrorHouse( true );
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief  ポケパラ系施設の注意喚起メッセージのスキップフラグを取得
 * @param  第1引数   0:エアバルーン 1:屋台 2:ホラーハウス
 * @return true:スキップする false：スキップしない
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetPokeParaSkipMessage(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject* pScript       = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*             pGameManager  = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*      pSave         = pGameManager->GetGameData()->GetJoinFestaData();

  u32 facilities = ptr[1];

  if( facilities == 0 )
  {
    return pSave->GetIsSkipMessageAirToy();
  }
  else if( facilities == 1 )
  {
    return pSave->GetIsSkipMessageStall();
  }
  else
  {
    return pSave->GetIsSkipMessageHorrorHouse();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief       JFPersonalへ話しかけ開始
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL PersonalTalkStart(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript         = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*           pScriptWork     = pScript->GetWork();
  GameSys::GameManager*               pGameManager    = pScript->GetGameManager();
  Field::Fieldmap*                    pFieldmap       = pGameManager->GetFieldmap();
  Savedata::JoinFestaDataSave*        pSave           = pGameManager->GetGameData()->GetJoinFestaData();


  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JFModelManager*  pJfModelManager = isRoom ? pJfGimmickRoom->GetJFDynamicLoadSystem()->GetJFModelManager() : pJfGimmick->GetJFDynamicLoadSystem()->GetJFModelManager();


  // 動作モデルのイベントID
  u32 eventId = pScriptWork->GetTempWorkValue( SCWK_TARGET_OBJID );
  // JFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = pJfModelManager->GetJoinFestaPersonalData( eventId ); 

  if( jfPersonal != NULL )
  {
    // 話しかけた相手のフレンドキーを保存
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->SetScriptTalkFriendKey( jfPersonal );
  }

  // 話しかけ状態ビーコンにする
  {
    NetAppLib::JoinFesta::JoinFestaPersonalData* pMyData = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetMyData();

    // アトラクション中の場合は、書き換えてはいけない
    if( pMyData->IsOpenAttraction() == false )
    {
#if defined(GF_PLATFORM_CTR)

    // NCLさん要望：サブスクリプションのAPI呼び出しによる負荷が大きいため削除
    if( NetLib::NijiNetworkSystem::GetNetworkMode() != NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )
    {
      pMyData->SetRequestJoinFestaPersonal();
      GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();
    }

#endif
    }
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       JFPersonalへ話しかけ終了
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL PersonalTalkEnd(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript         = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager    = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*        pSave           = pGameManager->GetGameData()->GetJoinFestaData();

  // 話しかけた相手のフレンドキーをリセット
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->ResetScriptTalkFriendKey();

  // 話しかけ状態ビーコンからゲーム中に戻す
  {
    NetAppLib::JoinFesta::JoinFestaPersonalData* pMyData = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetMyData();
    // アトラクション中の場合は、書き換えてはいけない
    if( pMyData->IsOpenAttraction() == false )
    {
#if defined(GF_PLATFORM_CTR)

    // NCLさん要望：サブスクリプションのAPI呼び出しによる負荷が大きいため削除
    if( NetLib::NijiNetworkSystem::GetNetworkMode() != NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )
    {
      GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SetPacketGameStatusWithMe();
      GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();
    }

#endif
    }
  }

  // 話しかけたとみなしてのカメラカウンタ増加
  pSave->AddPersonalTalkCameraCount();

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       話しかけ相手の関係性をVIPに変更
 * @retval      0:変更出来た
 * @retva       1:変更出来なかった
 * @retval      2:VIPが50人いて登録出来なかった
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL ChangeVipRelation(AMX* amx, const cell * ptr)
{
  if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->IsFull( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP ) )
  {
    ICHI_PRINT(">ICHI VIPが50人登録されていた\n");
    return 2;
  }

  // 話しかけた相手のJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetScriptTalkFieldPersonalData(); 

  if( jfPersonal != NULL )
  {
    // 常駐リストを更新
    bool ret = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->ChangePersonalRelation( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP, jfPersonal->GetJoinFestaFriendKey() );

    if( ret == false )
    {
      return 1;
    }

    // フィールドリストも更新
    {
      NetAppLib::JoinFesta::JoinFestaPersonalData* pFieldPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetFieldPersonalData( jfPersonal->GetJoinFestaFriendKey() );
      if( pFieldPersonal )
      {
        // 話書け中ののデータも更新
        pFieldPersonal->SetRelation( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP );
      }
    }

    // 話書け中ののデータも更新
    jfPersonal->SetRelation( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP );
  }

  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief       パーソナルイベント終了
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL PersonalEventFinish(AMX* amx, const cell * ptr)
{
  // 話しかけた相手のJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetScriptTalkFieldPersonalData(); 

  if( jfPersonal != NULL )
  {
    jfPersonal->FinishPersonalEvent();

    // Manager側も終了させる
    NetAppLib::JoinFesta::JoinFestaPersonalData* personal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetPersonalData( jfPersonal->GetJoinFestaFriendKey() );
    if( personal != NULL )
    {
      personal->FinishPersonalEvent();
    }

    // ジョインフェスタで交流イベントをした回数
    Savedata::IncRecord( Savedata::Record::RECID_JF_GEST_TALK_CNT );
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       パーソナルイベント終了（チュートリアルの人物専用）
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL PersonalEventFinishTutorial(AMX* amx, const cell * ptr)
{
  // 話しかけた相手のJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetScriptTalkFieldPersonalData(); 

  if( jfPersonal != NULL )
  {
    jfPersonal->ResetPersonalEvent();
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       パーソナルのアトラクションイベント実行済みか
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL IsPersonalAttractionFinish(AMX* amx, const cell * ptr)
{
  // 話しかけた相手のJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetScriptTalkFieldPersonalData(); 

  if( jfPersonal != NULL )
  {
    // 話しかけ済みかどうか
    return (jfPersonal->GetAttractionAppearanceState() == NetAppLib::JoinFesta::JoinFestaAttractionDefine::APPEARANCE_STATE_TALK );
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief       パーソナルのアトラクションイベント実行済みにさせる
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL PersonalAttractionFinish(AMX* amx, const cell * ptr)
{
  // 話しかけた相手のJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetScriptTalkFieldPersonalData(); 

  if( jfPersonal != NULL )
  {
    // 話しかけ済みとする
    jfPersonal->SetAttractionAppearanceState( NetAppLib::JoinFesta::JoinFestaAttractionDefine::APPEARANCE_STATE_TALK );
    // Manager側も終了させる
    NetAppLib::JoinFesta::JoinFestaPersonalData* personal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetPersonalData( jfPersonal->GetJoinFestaFriendKey() );
    if( personal != NULL && !personal->IsRelatonMe() )
    {
      personal->SetAttractionAppearanceState( NetAppLib::JoinFesta::JoinFestaAttractionDefine::APPEARANCE_STATE_TALK );
    }
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケダイアリートーク
 * @param   第1引数   index(0～2) 履歴何番目のダイアリーか
 * @return  ダイアリーに対応したメッセージID　ダイアリーがなかった場合、-1が返る
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL PokeDiaryTalk(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork       = pScript->GetWork();
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();
  print::WordSet*                               pWordSet          = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaResourceManager*  pJfResourceManager = isRoom ? pJfGimmickRoom->GetJoinFestaResourceManager() : pJfGimmick->GetJoinFestaResourceManager();
  const NetApp::JoinFesta::JoinFestaDataTable*  p_jfDataTable      = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem()->GetDataTable() : pJfGimmick->GetJoinFestaFacilitiesSystem()->GetDataTable();

  u32 diaryIndex = ptr[1];


  // 話しかけた相手のJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetScriptTalkFieldPersonalData(); 

  // PokeDiaryのパケットデータ
  NetAppLib::JoinFesta::PacketGameStatus5 packet;

  // 受信していない場合はない
  if( jfPersonal == NULL || jfPersonal->GetPacketGameStatus5( &packet ) == false )
  {
    return -1;
  }
  if( diaryIndex >= 3 )
  {
    GFL_ASSERT(0);
    return -1;
  }

  const bool isMale = ( jfPersonal->GetSex() == PM_MALE );
  const Savedata::PokeDiarySave::DIARY_DATA pokeDiary = packet.pokeDiary[ diaryIndex ];
  // 今回はカウントのダイアリーしかない
  const PDID pdid = static_cast<PDID>(pokeDiary.diary_id + PDID_COUNT_NULL);

  // ダイアリーが入っているかチェック
  if( pdid == PDID_NULL || pdid == PDID_LIVE_NULL || pdid == PDID_COUNT_NULL )
  {
    return -1;
  }
  // アッパー対応
  if( pdid >= PDID_COUNT_NUM )
  {
    return -1;
  }


  // 日時は共通
  {
    u32 year    = pokeDiary.date.year;
    u32 month   = pokeDiary.date.month;
    u32 day     = pokeDiary.date.day;

    pWordSet->RegisterNumber( 0, year, 2, print::NUM_DISP_ZERO, print::NUM_CODE_HANKAKU );
    pWordSet->RegisterNumber( 1, month, 2, print::NUM_DISP_LEFT, print::NUM_CODE_HANKAKU );
    pWordSet->RegisterNumber( 2, day, 2, print::NUM_DISP_LEFT, print::NUM_CODE_HANKAKU );
  }

  // 固有データ
  const u16 data1 = pokeDiary.data[0];
  const u16 data2 = pokeDiary.data[1];
  const u16 data3 = pokeDiary.data[2];

  // ダイアリーのメッセージに対応したタグを入れる
  switch( pdid ){
  case PDID_COUNT_FRIENDLY_SHOP:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeItemNo( data1 ) ) { return -1; }
      if( data2 > 999 ) { return -1; }

      // buf:3 = 道具名  data1
      // buf:4 = 個数3桁 data2
      pWordSet->RegisterItemName( 3, data1, data2 );
      pWordSet->RegisterNumber( 4, data2, 3, print::NUM_DISP_LEFT, print::NUM_CODE_HANKAKU );
      return ( isMale ? msg_jf_player_history_m_01 : msg_jf_player_history_f_01 );
    }
  case PDID_COUNT_KISEKAE:
    {
      // アッパー対応：範囲外チェック
      if( data1 > 999 ) { return -1; }
      // buf:4 = 個数3桁 data1
      pWordSet->RegisterNumber( 4, data1, 3, print::NUM_DISP_LEFT, print::NUM_CODE_HANKAKU );
      return ( isMale ? msg_jf_player_history_m_02 : msg_jf_player_history_f_02 );
    }
  case PDID_COUNT_GANBA_RIBBON:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeMonsNo( data1 ) ) { return -1; }

      // buf:3 = 種族名 data1
      pWordSet->RegisterPokeMonsNameNo( 3, (MonsNo)data1 );
      return ( isMale ? msg_jf_player_history_m_03 : msg_jf_player_history_f_03 );
    }
  case PDID_COUNT_NAME_CHANGE:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeMonsNo( data1 ) ) { return -1; }

      // buf:3 = 種族名 data1
      pWordSet->RegisterPokeMonsNameNo( 3, (MonsNo)data1 );
      return ( isMale ? msg_jf_player_history_m_04 : msg_jf_player_history_f_04 );
    }
  case PDID_COUNT_SODATEYA_TAMAGO:
    {
      return ( isMale ? msg_jf_player_history_m_05 : msg_jf_player_history_f_05 );
    }
  case PDID_COUNT_ID_PRESENT:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeItemNo( data1 ) ) { return -1; }

      // buf:3 = 道具名  data1
      pWordSet->RegisterItemName( 3, data1, 1 );
      return ( isMale ? msg_jf_player_history_m_06 : msg_jf_player_history_f_06 );
    }
  case PDID_COUNT_CAMERA_OYAJI:
    {
      // アッパー対応：範囲外チェック
      if( data1 >= msg_place_name_max ) { return -1; }
      if( data2 > 9999 ) { return -1; }

      // buf:3 = 地名  data1
      // buf:4 = 個数4桁  data2
      pWordSet->RegisterPlaceName( 3, data1 );
      pWordSet->RegisterNumber( 4, data2, 4, print::NUM_DISP_LEFT, print::NUM_CODE_HANKAKU );
      return ( isMale ? msg_jf_player_history_m_07 : msg_jf_player_history_f_07 );
    }
  case PDID_COUNT_FOSSIL:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeMonsNo( data1 ) ) { return -1; }

      // buf:3 = 種族名 data1
      pWordSet->RegisterPokeMonsNameNo( 3, (MonsNo)data1 );
      return ( isMale ? msg_jf_player_history_m_08 : msg_jf_player_history_f_08 );
    }
  case PDID_COUNT_CHECK_NATSUKI:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeMonsNo( data1 ) ) { return -1; }
      if( data2 > 255 ) { return -1; }

      // buf:3 = 種族名 data1
      pWordSet->RegisterPokeMonsNameNo( 3, (MonsNo)data1 );

      if( data2 == 255 )
      { // なつき度255
        return ( isMale ? msg_jf_player_history_m_09_01 : msg_jf_player_history_f_09_01 );
      }
      else if( data2 >= 200 )
      { // なつき度200-254
        return ( isMale ? msg_jf_player_history_m_09_02 : msg_jf_player_history_f_09_02 );
      }
      else if( data2 >= 150 )
      { // なつき度150-199
        return ( isMale ? msg_jf_player_history_m_09_03 : msg_jf_player_history_f_09_03 );
      }
      else if( data2 >= 100 )
      { // なつき度100-149
        return ( isMale ? msg_jf_player_history_m_09_04 : msg_jf_player_history_f_09_04 );
      }
      else if( data2 >= 50 )
      { // なつき度50-99
        return ( isMale ? msg_jf_player_history_m_09_05 : msg_jf_player_history_f_09_05 );
      }
      else if( data2 >= 1 )
      { // なつき度1-49
        return ( isMale ? msg_jf_player_history_m_09_06 : msg_jf_player_history_f_09_06 );
      }
      else
      { // なつき度0
        return ( isMale ? msg_jf_player_history_m_09_07 : msg_jf_player_history_f_09_07 );
      }
    }
  case PDID_COUNT_PLANT_SEED:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeItemNo( data1 ) ) { return -1; }

      // buf:3 = 道具名  data1
      pWordSet->RegisterItemName( 3, data1, 1 );
      return ( isMale ? msg_jf_player_history_m_10 : msg_jf_player_history_f_10 );
    }
  case PDID_COUNT_GET_SEED:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeItemNo( data1 ) ) { return -1; }
      if( data2 > 999 ) { return -1; }

      // buf:3 = 道具名  data1
      // buf:4 = 個数3桁 data2
      pWordSet->RegisterItemName( 3, data1, data2 );
      pWordSet->RegisterNumber( 4, data2, 3, print::NUM_DISP_LEFT, print::NUM_CODE_HANKAKU );
      return ( isMale ? msg_jf_player_history_m_11 : msg_jf_player_history_f_11 );
    }
  case PDID_COUNT_BP_SHOP:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeItemNo( data1 ) ) { return -1; }
      if( data2 > 999 ) { return -1; }

      // buf:3 = 道具名  data1
      // buf:4 = 個数3桁 data2
      pWordSet->RegisterItemName( 3, data1, data2 );
      pWordSet->RegisterNumber( 4, data2, 3, print::NUM_DISP_LEFT, print::NUM_CODE_HANKAKU );
      return ( isMale ? msg_jf_player_history_m_12 : msg_jf_player_history_f_12 );
    }
  case PDID_COUNT_WIN_BH_CHAMP:
    {
      // アッパー対応：範囲外チェック
      if( data2 > 2 ) { return -1; }
      if( data3 > 9999 ) { return -1; }

      // data1 ノーマル:0 スーパー:1
      // data2 シングル:0 ダブル:1 マルチ:2
      // buf:4 = 個数3桁 data3
      pWordSet->RegisterNumber( 4, data3, 4, print::NUM_DISP_LEFT, print::NUM_CODE_HANKAKU );

      u32 msgId;

      if( data1 == 0 ){
        msgId = msg_jf_player_history_m_13_01;
      }else if( data1 == 1){
        msgId = msg_jf_player_history_m_13_04;
      } else {
        return -1;
      }
      // ルール分ずらす
      msgId += (data2*2);
      // 性別分ずらす
      if( isMale == false )
      {
        msgId += 1;
      }

      return msgId;
    }
  case PDID_COUNT_DENDOU:
    {
      if( data1 > 9999 ) { return -1; }
      // buf:4 = 個数4桁 data1
      pWordSet->RegisterNumber( 4, data1, 4, print::NUM_DISP_LEFT, print::NUM_CODE_HANKAKU );
      return ( isMale ? msg_jf_player_history_m_14 : msg_jf_player_history_f_14 );
    }
  case PDID_COUNT_GTS_POKEPUT:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeMonsNo( data1 ) ) { return -1; }

      // buf:3 = 種族名 data1
      pWordSet->RegisterPokeMonsNameNo( 3, (MonsNo)data1 );
      return ( isMale ? msg_jf_player_history_m_15 : msg_jf_player_history_f_15 );
    }
  case PDID_COUNT_GTS_POKETRADE:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeMonsNo( data1 ) ) { return -1; }
      if( IsOutOfRangeMonsNo( data2 ) ) { return -1; }

      // buf:3 = 種族名 data1
      // buf:4 = 種族名 data2
      pWordSet->RegisterPokeMonsNameNo( 3, (MonsNo)data1 );
      pWordSet->RegisterPokeMonsNameNo( 4, (MonsNo)data2 );
      return ( isMale ? msg_jf_player_history_m_16 : msg_jf_player_history_f_16 );
    }
  case PDID_COUNT_MTRADE:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeMonsNo( data1 ) ) { return -1; }
      if( IsOutOfRangeMonsNo( data2 ) ) { return -1; }

      // buf:3 = 種族名 data1
      // buf:4 = 種族名 data2
      pWordSet->RegisterPokeMonsNameNo( 3, (MonsNo)data1 );
      pWordSet->RegisterPokeMonsNameNo( 4, (MonsNo)data2 );
      return ( isMale ? msg_jf_player_history_m_17 : msg_jf_player_history_f_17 );
    }
  case PDID_COUNT_P2PTRADE:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeMonsNo( data1 ) ) { return -1; }
      if( IsOutOfRangeMonsNo( data2 ) ) { return -1; }

      // buf:3 = 種族名 data1
      // buf:4 = 種族名 data2
      pWordSet->RegisterPokeMonsNameNo( 3, (MonsNo)data1 );
      pWordSet->RegisterPokeMonsNameNo( 4, (MonsNo)data2 );
      return ( isMale ? msg_jf_player_history_m_18 : msg_jf_player_history_f_18 );
    }
  case PDID_COUNT_BSPOT:
    {
      // アッパー対応：範囲外チェック
      if( data2 > 2 ) { return -1; }
      if( data1 > 9999 ) { return -1; }

      // buf:4 = 個数4桁 data1
      // data2 シングル:0 ダブル:1 バトルロイヤル:2
      // data3 フリー:0 レート:1 大会:2 仲間:3

      pWordSet->RegisterNumber( 4, data1, 4, print::NUM_DISP_LEFT, print::NUM_CODE_HANKAKU );

      u32 msgId;

      // バトル種別
      switch( data3 ){
      case 0: msgId = msg_jf_player_history_m_19_01; break;
      case 1: msgId = msg_jf_player_history_m_19_04; break;
      case 2: msgId = msg_jf_player_history_m_19_06; break;
      case 3: msgId = msg_jf_player_history_m_19_08; break;
      default: return -1;
      }

      // ルール分ずらす
      msgId += (data2*2);
      // 性別分ずらす
      if( isMale == false )
      {
        msgId += 1;
      }

      return msgId;
    }
  case PDID_COUNT_JF_ATTLACTION:
    {
      // アッパー対応：範囲外チェック
      if( data1 >= msg_jf_attraction_max ){ return -1; }
      if( data2 > 9999 ) { return -1; }

      // buf:3 = アトラクション名 data1
      // buf:4 = 数値4桁 data2
      pWordSet->RegisterAttractionName( 3, data1 );
      pWordSet->RegisterNumber( 4, data2, 4, print::NUM_DISP_LEFT, print::NUM_CODE_HANKAKU );
      return ( isMale ? msg_jf_player_history_m_20 : msg_jf_player_history_f_20 );
    }
  case PDID_COUNT_P2PBATTLE:
    {
      // アッパー対応：範囲外チェック
      if( data1 > 3 ){ return -1; }

      // data1 シングル:0 ダブル:1 ロイヤル:2 マルチ:3 
      u32 msgId = msg_jf_player_history_m_21_01;
      switch(data1){
      case 0: msgId = msg_jf_player_history_m_21_01; break; // シングル
      case 1: msgId = msg_jf_player_history_m_21_02; break; // ダブル
      case 2: msgId = msg_jf_player_history_m_21_04; break; // ロイヤル
      case 3: msgId = msg_jf_player_history_m_21_03; break; // マルチ
      }

      // 性別分ずらす
      if( isMale == false )
      {
        msgId += 1;
      }
      return msgId;
    }
  case PDID_COUNT_JF_SHOP:
    {
      // buf:3 = 施設名 data1
      // 施設名用のmstxt使う
      {
        gfl2::str::MsgData* pMsgData = pJfResourceManager->GetMsgData( NetApp::JoinFesta::JoinFestaResourceManager::RESOURCE_Message_JFShopName );
        u32 msgId = 0;
        const NetApp::JoinFesta::ShopItemInfo* pShop = p_jfDataTable->GetShopItemInfo( data1 );
        if( pShop != NULL )
        {
          msgId = pShop->messageID_ShopName;
        }
        else
        {
          // アッパー対応
          return -1;
        }
        pWordSet->RegisterWord( 3, pMsgData, msgId );
      }
      return ( isMale ? msg_jf_player_history_m_22 : msg_jf_player_history_f_22 );
    }
  case PDID_COUNT_RIDE:
    {
      // アッパー対応：範囲外チェック
      if( data1 > 6 ) { return -1; }
      if( data2 >= msg_place_name_max ) { return -1; }

      // buf:3 = 地名 data2
      // buf:4 = ライド名 data1
      pWordSet->RegisterPlaceName( 3, data2 );

      // ライド名入っているメッセージデータ
      {
        MonsNo monsNo = MONSNO_KENTAROSU;
        switch(data1){
        case 0: monsNo=MONSNO_KENTAROSU; break; // ケンタロス
        case 1: monsNo=MONSNO_SAMEHADAA; break; // サメハダー
        case 2: monsNo=MONSNO_MUURANDO; break; // ムーランド
        case 3: monsNo=MONSNO_RAPURASU; break; // ラプラス
        case 4: monsNo=MONSNO_KAIRIKII; break; // カイリキー
        case 5: monsNo=MONSNO_RIZAADON; break; // リザードン
        case 6: monsNo=MONSNO_ROBA2; break; // バンバドロ
        default: return -1;
        }

        pWordSet->RegisterPokeMonsNameNo( 4, monsNo );
      }
      return ( isMale ? msg_jf_player_history_m_23 : msg_jf_player_history_f_23 );
    }
  case PDID_COUNT_EFFORT:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeMonsNo( data1 ) ) { return -1; }

      // buf:3 = 種族名 data1
      pWordSet->RegisterPokeMonsNameNo( 3, (MonsNo)data1 );
      return ( isMale ? msg_jf_player_history_m_24 : msg_jf_player_history_f_24 );
    }
  case PDID_COUNT_PARLE:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeMonsNo( data1 ) ) { return -1; }

      // buf:3 = 種族名 data1
      pWordSet->RegisterPokeMonsNameNo( 3, (MonsNo)data1 );
      return ( isMale ? msg_jf_player_history_m_25 : msg_jf_player_history_f_25 );
    }
  case PDID_COUNT_CAFE:
    {
      // アッパー対応：範囲外チェック
      if( data2 >= msg_place_name_max ) { return -1; }

      // buf:3 = 地名 data2
      // buf:4 = 飲み物名 data1
      pWordSet->RegisterPlaceName( 3, data2 );

      // 飲み物名入っているメッセージデータ
      {
        u32 msgId = msg_pokecen_cafe_select_01;
        switch( data1 ){
        case 3: msgId = msg_pokecen_cafe_select_01; break; // ミックスオレ
        case 4: msgId = msg_pokecen_cafe_select_02; break; // エネココア
        case 5: msgId = msg_pokecen_cafe_select_03; break; // モーモーミルク
        case 6: msgId = msg_pokecen_cafe_select_04; break; // パイルジュース
        case 7: msgId = msg_pokecen_cafe_select_05; break; // ロズレイティー
        case 8: msgId = msg_pokecen_cafe_select_06; break; // グランブルマウンテン
        default: return -1;
        }

        gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
        gfl2::str::MsgData* pMsgData = GFL_NEW( pHeap ) gfl2::str::MsgData( print::GetMessageArcId_Script(), GARC_script_message_cafe_event_DAT, pHeap );
        pWordSet->RegisterWord( 4, pMsgData, msgId );
        GFL_SAFE_DELETE( pMsgData );
      }
      return ( isMale ? msg_jf_player_history_m_26 : msg_jf_player_history_f_26 );
    }
  case PDID_COUNT_BATTLEROYAL:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeMonsNo( data1 ) ) { return -1; }
      if( data2 > 3 ) { return -1; }

      // buf:3 = 種族名 data1
      pWordSet->RegisterPokeMonsNameNo( 3, (MonsNo)data1 );

      // ランク data2

      u32 msgId = msg_jf_player_history_m_27_01;

      // ランク分ずらす
      msgId += (data2*2);
      // 性別分ずらす
      if( isMale == false )
      {
        msgId += 1;
      }

      return msgId;
    }
  case PDID_COUNT_LVUP:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeMonsNo( data1 ) ) { return -1; }

      // buf:3 = 種族名 data1
      pWordSet->RegisterPokeMonsNameNo( 3, (MonsNo)data1 );
      return ( isMale ? msg_jf_player_history_m_28 : msg_jf_player_history_f_28 );
    }
  case PDID_COUNT_MARASADA:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeMonsNo( data2 ) ) { return -1; }

      // buf:4 = マサラダ名 data1
      // buf:3 = 種族名 data2
      pWordSet->RegisterPokeMonsNameNo( 3, (MonsNo)data2 );

      // マサラダ名入っているメッセージデータ
      {
        u32 msgId = msg_malasada_select1_01;
        switch( data1 ){
        case 0: msgId = msg_malasada_select1_06; break; // マボサダ
        case 1: msgId = msg_malasada_select1_01; break; // アマサダ
        case 2: msgId = msg_malasada_select1_02; break; // スッパサダ
        case 3: msgId = msg_malasada_select1_03; break; // シブサダ
        case 4: msgId = msg_malasada_select1_04; break; // カラサダ
        case 5: msgId = msg_malasada_select1_05; break; // ニガサダ
        default: return -1;
        }

        gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
        gfl2::str::MsgData* pMsgData = GFL_NEW( pHeap ) gfl2::str::MsgData( print::GetMessageArcId_Script(), GARC_script_message_shop_malasada_DAT, pHeap );
        pWordSet->RegisterWord( 4, pMsgData, msgId );
        GFL_SAFE_DELETE( pMsgData );
      }
      return ( isMale ? msg_jf_player_history_m_29 : msg_jf_player_history_f_29 );
    }
  case PDID_COUNT_MONOHIROI:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeMonsNo( data1 ) ) { return -1; }
      if( IsOutOfRangeItemNo( data2 ) ) { return -1; }

      // buf:3 = 種族名 data1
      // buf:4 = 道具名 data2
      pWordSet->RegisterPokeMonsNameNo( 3, (MonsNo)data1 );
      pWordSet->RegisterItemName( 4, data2, 1 );
      return ( isMale ? msg_jf_player_history_m_30 : msg_jf_player_history_f_30 );
    }
  case PDID_COUNT_NAKAYOSHI:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeMonsNo( data1 ) ) { return -1; }

      // buf:3 = 種族名 data1
      pWordSet->RegisterPokeMonsNameNo( 3, (MonsNo)data1 );
      return ( isMale ? msg_jf_player_history_m_31 : msg_jf_player_history_f_31 );
    }
  case PDID_COUNT_LOMILOMI:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeMonsNo( data1 ) ) { return -1; }

      // buf:3 = 種族名 data1
      pWordSet->RegisterPokeMonsNameNo( 3, (MonsNo)data1 );
      return ( isMale ? msg_jf_player_history_m_32 : msg_jf_player_history_f_32 );
    }
  case PDID_COUNT_TRAINING:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeMonsNo( data1 ) ) { return -1; }

      // buf:3 = 種族名 data1
      pWordSet->RegisterPokeMonsNameNo( 3, (MonsNo)data1 );
      return ( isMale ? msg_jf_player_history_m_33 : msg_jf_player_history_f_33 );
    }
  case PDID_COUNT_WAZAMASHIN:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeMonsNo( data1 ) ) { return -1; }
      if( IsOutOfRangeWazaNo( data2 ) ) { return -1; }

      // buf:3 = 種族名 data1
      // buf:4 = 技名 data2
      pWordSet->RegisterPokeMonsNameNo( 3, (MonsNo)data1 );
      pWordSet->RegisterWazaName( 4, data2 );
      return ( isMale ? msg_jf_player_history_m_34 : msg_jf_player_history_f_34 );
    }
  case PDID_COUNT_BATTLEBIKING:
    {
      // buf:3 = 料理名 data1 →ラベルないので入れれない
      return ( isMale ? msg_jf_player_history_m_35 : msg_jf_player_history_f_35 );
    }
  case PDID_COUNT_POKRESORT:
    {
      // アッパー対応：範囲外チェック
      if( data1 >= Savedata::ResortSave::RESORT_KIND_MAX ){ return -1; }

      // リゾートの場所ID data1
      u32 msgId = msg_jf_player_history_m_36_01;

      // ランク分ずらす
      msgId += (data1*2);
      // 性別分ずらす
      if( isMale == false )
      {
        msgId += 1;
      }

      return msgId;
    }
  case PDID_COUNT_QR:
    {
      return ( isMale ? msg_jf_player_history_m_37 : msg_jf_player_history_f_37 );
    }
  case PDID_COUNT_FIELDSCAN:
    {
      // アッパー対応：範囲外チェック
      if( IsOutOfRangeMonsNo( data1 ) ) { return -1; }

      // buf:3 = 種族名 data1
      pWordSet->RegisterPokeMonsNameNo( 3, (MonsNo)data1 );
      return ( isMale ? msg_jf_player_history_m_38 : msg_jf_player_history_f_38 );
    }
  case PDID_COUNT_BATTLEMOVIE:
    {
      // data1 はバトルカテゴリだが、メッセージが対応していないので見ない。
      // アッパー対応：範囲外チェック
      if( data2 > 3 ){ return -1; }

      // data2 シングル:0 ダブル:1 ロイヤル:2 マルチ:3 
      u32 msgId = msg_jf_player_history_m_39_01;

      switch( data2 ){
      case 0: msgId=msg_jf_player_history_m_39_01; break; // シングル
      case 1: msgId=msg_jf_player_history_m_39_02; break; // ダブル
      case 2: msgId=msg_jf_player_history_m_39_03; break; // ロイヤル
      case 3: msgId=msg_jf_player_history_m_39_04; break; // マルチ
      }

      // 性別分ずらす
      if( isMale == false )
      {
        msgId += 1;
      }

      return msgId;
    }
  }


  GFL_ASSERT(0);
  return -1;
}

//------------------------------------------------------------------------------
/**
 * @brief   そのふくステキ！
 * @retval  SCWK_RET0   イチオシかどうか(0:イチオシでない 1:イチオシ)
 * @retval  SCWK_RET1   着せ替えカテゴリ(Savedata::Fashion::Category)
 * @retval  SCWK_RET2   着せ替えアイテムID
 * @retval  SCWK_RET3   必要なフェスコイン
 * @retrun  オススメされたきせかえアイテムを所持しているかどうか(trueでしている）
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL ThatKisekaeNice(AMX* amx, const cell * ptr)
{
  // イチオシ着せ替えを実際に着ていたら、イチオシアイテムを紹介する
  // それ以外は、着ている中から抽選する

  Field::FieldScript::ScriptObject*             pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork         = pScript->GetWork();
  GameSys::GameManager*                         pGameManager        = pScript->GetGameManager();
  Savedata::Fashion*                            pFashionSave        = pGameManager->GetGameData()->GetFashion();
  Savedata::MyStatus*                           pMyStatus           = pGameManager->GetGameData()->GetPlayerStatus();   


  // 話しかけた相手のJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetScriptTalkFieldPersonalData(); 

  if( jfPersonal == NULL )
  {
    GFL_ASSERT(0);
    return true;
  }

  // 相手の現在着ている着せ替え
  poke_3d::model::DressUpParam dressUpParam;
  System::DressUpParamSaveData::ConvertToDressUpParam(&dressUpParam, *jfPersonal->GetDressUpParam() );

  s16   retDressUpItem = -1;         //!< 着せ替えItem   
  Savedata::Fashion::Category retDressUpItemCategory = Savedata::Fashion::CATEGORY_TOPS;  //!< 着せ替えItemCategory

  // 所持着せ替えからおすすめする
  {
    s16 selectItemPrams[ poke_3d::model::DressUpParam::ITEM_CATEGORY_COUNT ] = {0};
    u32 selectItemCategory[ poke_3d::model::DressUpParam::ITEM_CATEGORY_COUNT ] = {0};
    u8 selectItemPramsCount = 0;

    // 有効な着せ替えアイテムのみ抽出
    for (u32 i = 0; i < poke_3d::model::DressUpParam::ITEM_CATEGORY_COUNT; ++i)
    {
      // @fix GFNMCat[3468]：不要なカテゴリはSkipi
      poke_3d::model::DressUpParam::ItemCategory itemCategory = static_cast<poke_3d::model::DressUpParam::ItemCategory>(i);
      switch( itemCategory ){
      case poke_3d::model::DressUpParam::ITEM_CATEGORY_FACE:                   // 顔
      case poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE:             // 髪型
      case poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCBADGE:               // バッジアクセサリ
      case poke_3d::model::DressUpParam::ITEM_CATEGORY_BNGL:                   // バングル
      case poke_3d::model::DressUpParam::ITEM_CATEGORY_BODY:                   // ボディ
      case poke_3d::model::DressUpParam::ITEM_CATEGORY_SITU:                   // シチュエーション着替え
        continue;
      case poke_3d::model::DressUpParam::ITEM_CATEGORY_LEGS:                   // レッグ
        // 靴下無しは除く
        if( dressUpParam.itemParams[ i ] == 0 )
        {
          continue;
        }
        break;
      }

      if( dressUpParam.itemParams[ i ] != -1 )
      {
        selectItemCategory[ selectItemPramsCount ]  = i; // poke_3d::model::DressUpParam::ItemCategory
        selectItemPrams[ selectItemPramsCount ]     = dressUpParam.itemParams[ i ];
        selectItemPramsCount++;
      }
    }

    // その人のその日の乱数で決める
    gfl2::math::Random random;
    u32 seed = pGameManager->GetGameData()->GetSaveData()->GetRandomGroup()->GetTodayRandSeed();
    // TIDの下1桁を引く
    seed -= (jfPersonal->GetTransferedID() % 10 );
    random.Initialize( seed );


    u32 randomValue = random.Next( selectItemPramsCount );

    retDressUpItemCategory  = Savedata::Fashion::ConvSavedataCategoryFromDressUpParamItemCategory( (poke_3d::model::DressUpParam::ItemCategory)selectItemCategory[ randomValue ] );  //!< 着せ替えItemCategory
    retDressUpItem          = selectItemPrams[ randomValue ];          //!< 着せ替えItem   
    ICHI_PRINT(">ICHI 所持着せ替えをオススメ %d %d\n", retDressUpItemCategory, retDressUpItem);
  }

  // 結果入れる
  {
    if( retDressUpItem == -1 )
    {
      GFL_ASSERT(0);
      retDressUpItem = 0; // safety
    }

    // イチオシかどうか(0:イチオシでない 1:イチオシ) 一押しの仕様は削除された 2016/03/30
    pScriptWork->SetWorkValue( SCWK_RET0, 0 );
    // 着せ替えカテゴリ(Savedata::Fashion::Category)
    pScriptWork->SetWorkValue( SCWK_RET1, retDressUpItemCategory );
    // 着せ替えアイテムID
    pScriptWork->SetWorkValue( SCWK_RET2, retDressUpItem );
    // 必要なフェスコイン
    const table_data_GoodsData* pGoodsData = app::util::Kisekae::KisekaeDataBase::GetGoodsDataSearchItemId( 
                                                              static_cast<poke_3d::model::DressUpParam::Sex>(jfPersonal->GetSex()),
                                                              retDressUpItemCategory,
                                                              static_cast<u32>(retDressUpItem) );

    if( pGoodsData )
    {
      pScriptWork->SetWorkValue( SCWK_RET3, pGoodsData->fes_coin_num );
      ICHI_PRINT(">ICHI 必要なフェスコイン %d \n", pGoodsData->fes_coin_num);
    }
  }

  return pFashionSave->IsOwned( retDressUpItemCategory, retDressUpItem );
}

//------------------------------------------------------------------------------
/**
 * @brief       レコード紹介のパーソナルイベント
 * @param       第1引数　レコードカテゴリ
 * @return      反応結果 0:大成功 1:成功 2:残念
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL PersonalEventRecordIntroduction(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript         = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager    = pScript->GetGameManager();
  print::WordSet*                     pWordSet        = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);

  u32 recordCategory = ptr[1];

  // スクリプトで選んだ人のJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = pJoinFestaPersonalDataManager->GetScriptSelectPersonal();

  if( jfPersonal != NULL )
  {
    gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

    NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager* pRankingManager = GFL_NEW( p_heap->GetLowerHandle() ) NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager();

    // リストにいなかった場合のみ、選んだ人を含めたランキングである必要がある
    const bool isExistList = ( pJoinFestaPersonalDataManager->GetPersonalData( jfPersonal->GetJoinFestaFriendKey() ) == NULL ) ? false : true;
    NetAppLib::JoinFesta::JoinFestaPersonalData* addPerson = isExistList==false ? jfPersonal : NULL;
    pRankingManager->UpdateRankingList( addPerson );

    u32 rank;
    u32 rankLength;

    if( jfPersonal->GetRelation() == NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST )
    {
      rank = pRankingManager->GetGuestRank( static_cast<NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager::RecordCategory>(recordCategory), jfPersonal->GetJoinFestaFriendKey() );
      rankLength = pRankingManager->GetGuestRankLength();
    }
    else
    {
      rank = pRankingManager->GetVipRank( static_cast<NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager::RecordCategory>(recordCategory), jfPersonal->GetJoinFestaFriendKey() );
      rankLength = pRankingManager->GetVipRankLength();
    }



    // 結果を返す
    u8 ret = 2;
    {
      const u8 bigSuccessRank = static_cast<f32>(rankLength) * 0.2f; // 上位20%
      const u8 successRank = static_cast<f32>(rankLength) * 0.4f;    // 上位40%
      ICHI_PRINT("rank:%d rankLength:%d bigSuccessRank:%d successRank:%d \n", rank, rankLength, bigSuccessRank, successRank );
      if( rank <= bigSuccessRank )
      {
        ret = 0;
      }
      else if( rank <= successRank )
      {
        ret = 1;
      }
      else
      {
        ret = 2;
      }
    }


    // スクリプトで選んだ人をリセット
    pJoinFestaPersonalDataManager->ResetScriptSelectPersonal();

    GFL_SAFE_DELETE( pRankingManager );

    return ret;
  }


  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       ズバリ当てろレコードのパーソナルイベント
 * @param       第1引数　レコードカテゴリ
 * @retval      SCWK_RET0 自分の順位
 * @retval      SCWK_RET1 人数
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL PersonalEventRecordGuess(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript         = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*           pScriptWork     = pScript->GetWork();
  GameSys::GameManager*               pGameManager    = pScript->GetGameManager();
  print::WordSet*                     pWordSet        = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();

  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);

  u32 recordCategory = ptr[1];

  // 話しかけた相手のJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = pJoinFestaPersonalDataManager->GetScriptTalkFieldPersonalData(); 

  if( jfPersonal != NULL )
  {
    gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

    NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager* pRankingManager = GFL_NEW( p_heap->GetLowerHandle() ) NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager();

    // リストにいなかった場合のみ、選んだ人を含めたランキングである必要がある
    const bool isExistList = ( pJoinFestaPersonalDataManager->GetPersonalData( jfPersonal->GetJoinFestaFriendKey() ) == NULL ) ? false : true;
    NetAppLib::JoinFesta::JoinFestaPersonalData* addPerson = isExistList==false ? jfPersonal : NULL;
    pRankingManager->UpdateRankingList( addPerson );

    u32 rank;
    u32 rankLength;

    if( jfPersonal->GetRelation() == NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST )
    {
      rank = pRankingManager->GetGuestRank( static_cast<NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager::RecordCategory>(recordCategory), jfPersonal->GetJoinFestaFriendKey() );
      rankLength = pRankingManager->GetGuestRankLength();
    }
    else
    {
      rank = pRankingManager->GetVipRank( static_cast<NetAppLib::JoinFestaUI::JoinFestaRecordRankingManager::RecordCategory>(recordCategory), jfPersonal->GetJoinFestaFriendKey() );
      rankLength = pRankingManager->GetVipRankLength();
    }

    ICHI_PRINT("rank:%d rankLength:%d rankAverage:%d \n", rank, rankLength, rankLength/2 );

    pScriptWork->SetWorkValue( SCWK_RET0, rank );
    pScriptWork->SetWorkValue( SCWK_RET1, rankLength );

    GFL_SAFE_DELETE( pRankingManager );
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       アンケート質問パーソナルイベント用にどのアンケートをすればよいか取得
 * @return      アンケートID（0～15）
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetQuestionnaireIdForPersonalEvent(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject* pScript       = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*             pGameManager  = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*      pSave         = pGameManager->GetGameData()->GetJoinFestaData();

  // 答えていないのを優先
  for( u8 i=0; i<16; i++ )
  {
    if( pSave->GetQuestionnaireAnswer( i ) == 0 )
    {
      return i;
    }
  }
  // 以降は、ID順にローテーション
  return pSave->GetQuestionnaireIdForPersonalEvent();
}

//------------------------------------------------------------------------------
/**
 * @brief       ズバリ当てろアンケートパーソナルイベント用にアンケート答えを取得
 * @param       アンケートID（0～15）
 * @return      アンケート回答ID
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetQuestionnaireAnswerForPersonalEvent(AMX* amx, const cell * ptr)
{
  // 話しかけた相手のJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetScriptTalkFieldPersonalData(); 

  u32 questionId = ptr[1];

  if( jfPersonal )
  {
    NetAppLib::JoinFesta::PacketGameStatus1 packet;
    if( jfPersonal->GetPacketGameStatus1( &packet ) )
    {
      if( questionId < 16 )
      {
        return packet.questionnaireAnswer[ questionId ];
      }
    }
  }

  GFL_ASSERT_MSG(0,"questionId:%d\n",questionId);
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief       自分のレコード取得
 * @param       第1引数　レコードカテゴリ
 * @param       第2引数　レコードカテゴリ内のインデックス
 *
 * @return      データ(-1でデータなし）
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetMyPersonalRecord(AMX* amx, const cell * ptr)
{
  NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetMyData();

  u32 recordCategory = ptr[1];
  u32 recordIndex = ptr[2];

  if( jfPersonal )
  {
    return jfPersonal->GetRecordData( recordCategory, recordIndex );
  }

  // データなし
  return -1;
}

//------------------------------------------------------------------------------
/**
 * @brief       話しかけ相手のレコード取得
 * @param       第1引数　レコードカテゴリ
 * @param       第2引数　レコードカテゴリ内のインデックス
 *
 * @return      データ(-1でデータなし）
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetPersonalRecord(AMX* amx, const cell * ptr)
{
  // 話しかけた相手のJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetScriptTalkFieldPersonalData(); 

  u32 recordCategory = ptr[1];
  u32 recordIndex = ptr[2];

  if( jfPersonal )
  {
    return jfPersonal->GetRecordData( recordCategory, recordIndex );
  }

  // データなし
  return -1;
}

//------------------------------------------------------------------------------
/**
 * @brief       レコードカテゴリに含まれるレコードの数を取得
 * @param       第1引数　レコードカテゴリ
 * @return      総数
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetRecordCategoryNum(AMX* amx, const cell * ptr)
{
  u32 recordCategory = ptr[1];
  return NetAppLib::JoinFesta::JoinFestaPersonalEvent::GetRecordCategoryNum( recordCategory );
}

//------------------------------------------------------------------------------
/**
 * @brief       相手のダイアリーにネタバレがあるかチェック
 * @return      trueでネタバレ有り
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL IsDiaryNetabare(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject* pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*             pGameManager        = pScript->GetGameManager();
  Savedata::PokeDiarySave*          pDiary              = pGameManager->GetGameData()->GetPokeDiarySave();

  // 話しかけた相手のJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetScriptTalkFieldPersonalData(); 

  // PokeDiaryのパケットデータ
  NetAppLib::JoinFesta::PacketGameStatus5 packet;

  // 受信していない場合はなし
  if( jfPersonal == NULL || jfPersonal->GetPacketGameStatus5( &packet ) == false )
  {
    return false;
  }

  for( u32 i=0; i<3; i++ )
  {
    // 相手のダイアリーID
    const Savedata::PokeDiarySave::DIARY_DATA pokeDiary = packet.pokeDiary[ i ];
    const PDID otherPdid = static_cast<PDID>(pokeDiary.diary_id + PDID_COUNT_NULL);

    // @fix GFNMCat[4931]：ダイアリーIDのNULLチェック漏れ
    if( otherPdid != PDID_COUNT_NULL )
    {
      // 自分のダイアリーの中に存在するかチェック
      Savedata::PokeDiarySave::DIARY_DATA tmp;
      if( pDiary->Get( otherPdid, false, &tmp ) == false )
      {
        return true; // 存在しない時点でネタバレとする
      }
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief       アトラクションの制限時間カウントダウン開始
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL AttractionTimeCountDownStart(AMX* amx, const cell * ptr)
{
  NetAppLib::JoinFesta::JoinFestaAttractionManager* pAttractionManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager);
  pAttractionManager->TimeCountDownStart();
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       アトラクションのスコアアップ
 * @reutrn      trueで正解
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL AttractionScoreUp(AMX* amx, const cell * ptr)
{
  NetAppLib::JoinFesta::JoinFestaAttractionManager* pAttractionManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager);
  pAttractionManager->AddScore();
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       アンケートアトラクションで回答が正解かチェック
 * @reutrn      trueで正解
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL AttractionQuestionIsOk(AMX* amx, const cell * ptr)
{
  NetAppLib::JoinFesta::JoinFestaAttractionManager* pAttractionManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager);

  JoinFestaScript::AttractionId attractionId = pAttractionManager->GetPlayAttractionId();

  // 話しかけた相手のJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetScriptTalkFieldPersonalData(); 

  if( jfPersonal != NULL )
  {
    NetAppLib::JoinFesta::PacketGameStatus1 packet;
    if( jfPersonal->GetPacketGameStatus1( &packet ) )
    {
      ICHI_PRINT(">ICHI m_core.m_gameStatus1.attraction_questionnaire[15] %d \n", packet.questionnaireAnswer[15]);
      // アンケートアトラクションに対応したアンケートID取得
      const u8 questionnaireId = NetAppLib::JoinFesta::JoinFestaAttractionUtil::GetQuestionnaireIdFromAttractionId( attractionId );
      return pAttractionManager->IsQuestionAnswerScoreUp( attractionId, packet.questionnaireAnswer[ questionnaireId ] );
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief       カイリキーライドアトラクションで加点対象かチェック
 * @reutrn      trueで正解
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL AttractionKairikyRideIsOk(AMX* amx, const cell * ptr)
{
  NetAppLib::JoinFesta::JoinFestaAttractionManager* pAttractionManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager);

  // 話しかけた相手のJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetScriptTalkFieldPersonalData(); 

  if( jfPersonal != NULL )
  {
    // 加点対象チェック
    return pAttractionManager->IsKairikyRideScoreUp( jfPersonal );
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   ケンタロスライドで使用する壊せる岩のセット（表示）
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL AttractionKentarosuRideSetRock(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*               pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                       pScriptWork         = pScript->GetWork();
  GameSys::GameManager*                           pGameManager        = pScript->GetGameManager();
  Field::FieldResident*                           pFieldResident      = pGameManager->GetFieldResident();
  Field::EventWork*                               pEventWork          = pGameManager->GetGameData()->GetEventWork();
  Field::StaticModel::StaticModelManager*         pStaticModelManager = pGameManager->GetFieldmap()->GetStaticModelManager();

  if( pFieldResident == NULL ) return false;

  Field::PlacementDataManager* pPlacementDataManager = pFieldResident->GetPlacementDataManager();

  // 今置いている岩を削除する
  for( u32 flag=SYS_FLAG_JF_ROCKBREAKER_00; flag<=SYS_FLAG_JF_ROCKBREAKER_15; flag++ )
  {
    // @fix GFNMCat[2280] ジョインフェスタ用のUnRegist関数に置き換え
    // Kick処理の中でActionEventActorは自身のUnRegistを行うため、UnRegistZoneActionEventで配置モデルのUnRegistまで処理が走らないため
    pPlacementDataManager->UnRegistZoneActionEventForJoinFesta( flag );
  }

  // 壊せる岩に紐付けされているフラグを設定
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->SetKentarosuRideRockBreaker();

  // フラグ設定された岩を置く
  for( u32 flag=SYS_FLAG_JF_ROCKBREAKER_00; flag<=SYS_FLAG_JF_ROCKBREAKER_15; flag++ )
  {
    if( Field::EventWork::CheckIDFlagRenge( flag ) )
    {
      // 置く岩はフラグが0になっている
      if( pEventWork->CheckEventFlag( flag ) == 0 )
      {
        pPlacementDataManager->RegistZoneActionEvent( flag );
        // アニメもリセット（壊れた岩を戻す）
        {
          Field::IFieldActorBase* pActorBase = pPlacementDataManager->SearchZoneActionEvent( flag );
          GFL_ASSERT(pActorBase);

          if( pActorBase )
          {
            Field::FieldActionEventActor *pActor = reinterpret_cast<Field::FieldActionEventActor*>( pActorBase );
            const Field::FieldActionEventAccessor::ActionEventData* pData = pActor->GetData();

            // 配置モデル検索
            Field::StaticModel::StaticModel* pStaticModel = pStaticModelManager->GetEventIDStaticModel( pData->eventID );
            // 岩のアニメ状態を初期フレームで固定する
            Field::StaticModel::StaticModel::Handle_SetFrameEventAnimation handle1(0,0);
            pStaticModel->Set( &handle1 );
            pStaticModel->Update();
            Field::StaticModel::StaticModel::Handle_StopEventAnimation handle2(0);
            pStaticModel->Set( &handle2 );
            pStaticModel->Update();
          }
        }
      }
    }
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief   タイプ相性を計算する
 * @param    第1引数　わざのタイプ
 * @param    第2引数　技を受けるポケモンのタイプ1
 * @param    第3引数　技を受けるポケモンのタイプ2
 * @param    第4引数　さかさバトルかどうか
 *
 * @return   AboutAffinityIDの定義
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL AttractionCalcAffinityAboud(AMX* amx, const cell * ptr)
{
  pml::PokeType wazaType  = static_cast<pml::PokeType>(ptr[1]);
  pml::PokeType pokeType1 = static_cast<pml::PokeType>(ptr[2]);
  pml::PokeType pokeType2 = static_cast<pml::PokeType>(ptr[3]);
  bool isSakasa = GFL_BOOL_CAST(ptr[4]);

  return pml::battle::TypeAffinity::CalcAffinityAbout( wazaType, pokeType1, pokeType2, isSakasa );
}

//------------------------------------------------------------------------------
/**
 * @brief   アトラクションを通信エラーでリタイアしたかフラグ取得
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL AttractionIsErrorRetire(AMX* amx, const cell * ptr)
{
  return GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->IsErrorRetire();
}

//------------------------------------------------------------------------------
/**
 * @brief       施設へ話しかけ開始
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FacilitiesTalkStart(AMX* amx, const cell * ptr)
{
  // 施設話しかけビーコンにする
  {
    NetAppLib::JoinFesta::JoinFestaPersonalData* pMyData = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetMyData();

    // アトラクション中の場合は、書き換えてはいけない
    if( pMyData->IsOpenAttraction() == false )
    {
#if defined(GF_PLATFORM_CTR)

      // NCLさん要望：サブスクリプションのAPI呼び出しによる負荷が大きいため削除
      if( NetLib::NijiNetworkSystem::GetNetworkMode() != NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )
      {
        pMyData->SetRequestJoinFestaShop();
        GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();
      }
#endif
    }
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       施設へ話しかけ終了
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FacilitiesTalkEnd(AMX* amx, const cell * ptr)
{
  NetAppLib::JoinFesta::JoinFestaPersonalData* pMyData = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetMyData();
  // アトラクション中の場合は、書き換えてはいけない
  if( pMyData->IsOpenAttraction() == false )
  {
    // 話しかけ状態ビーコンからゲーム中に戻す
#if defined(GF_PLATFORM_CTR)

    // NCLさん要望：サブスクリプションのAPI呼び出しによる負荷が大きいため削除
    if( NetLib::NijiNetworkSystem::GetNetworkMode() != NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )
    {
      GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SetPacketGameStatusWithMe();
      GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();
    }
#endif
  }
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       施設インデックスから施設ユニークID取得
 * @param       第1引数　施設インデックス FacilitiesIndex
 * @reutrn      FacilitiesId
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetFacilitiesId(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager        = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaFacilitiesSystem* pJfFacilitiesSystem = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem() : pJfGimmick->GetJoinFestaFacilitiesSystem();



  JoinFestaScript::FacilitiesIndex facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>(ptr[1]);

  const NetApp::JoinFesta::JoinFestaFacilitiesData* pData = pJfFacilitiesSystem->GetFacilitiesData( facilitiesIndex ); 

  if( pData != NULL )
  {
    return pData->GetFacilitiesId();
  }

  // 落ちないように
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief       施設インデックスから施設の種類取得
 * @param       第1引数　施設インデックス FacilitiesIndex
 * @reutrn      FacilitiesType
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetFacilitiesType(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager        = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaFacilitiesSystem* pJfFacilitiesSystem = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem() : pJfGimmick->GetJoinFestaFacilitiesSystem();


  JoinFestaScript::FacilitiesIndex facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>(ptr[1]);

  const NetApp::JoinFesta::JoinFestaFacilitiesData* pData = pJfFacilitiesSystem->GetFacilitiesData( facilitiesIndex ); 

  if( pData != NULL )
  {
    return pData->GetFacilitiesType();
  }

  // 落ちないように
  return JoinFestaScript::FACILITIES_TYPE_LOTTERY;
}

//------------------------------------------------------------------------------
/**
 * @brief       施設IDから施設の種類取得
 * @param       第1引数　施設ID（全施設でユニークなID）
 * @reutrn      FacilitiesType
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetFacilitiesType2(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork       = pScript->GetWork();
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  const NetApp::JoinFesta::JoinFestaDataTable*  p_jfDataTable      = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem()->GetDataTable() : pJfGimmick->GetJoinFestaFacilitiesSystem()->GetDataTable();


  u32 facilitiesId= ptr[1]; // 施設ID

  const NetApp::JoinFesta::ShopItemInfo* pShop = p_jfDataTable->GetShopItemInfo( facilitiesId );
  GFL_ASSERT_MSG(pShop, "%d", facilitiesId);
  if( pShop != NULL )
  {
    return pShop->shopType;
  }

  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief       施設インデックスから施設の店員ID取得
 * @param       第1引数　施設インデックス FacilitiesIndex
 * @reutrn      characterId
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetFacilitiesCharacterId(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager        = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaFacilitiesSystem* pJfFacilitiesSystem = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem() : pJfGimmick->GetJoinFestaFacilitiesSystem();


  JoinFestaScript::FacilitiesIndex facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>(ptr[1]);

  const NetApp::JoinFesta::JoinFestaFacilitiesData* pData = pJfFacilitiesSystem->GetFacilitiesData( facilitiesIndex ); 

  if( pData != NULL )
  {
    return pData->GetCharacterId();
  }

  // 落ちないように
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief       施設インデックスから施設店員の性別取得
 * @param       第1引数　施設インデックス FacilitiesIndex
 * @reutrn      性別
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetFacilitiesAssistantSex(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager        = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaFacilitiesSystem* pJfFacilitiesSystem = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem() : pJfGimmick->GetJoinFestaFacilitiesSystem();


  JoinFestaScript::FacilitiesIndex facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>(ptr[1]);

  const NetApp::JoinFesta::JoinFestaFacilitiesData* pData = pJfFacilitiesSystem->GetFacilitiesData( facilitiesIndex ); 

  if( pData != NULL )
  {
    return pData->GetAssistantSex();
  }

  // 落ちないように
  return PM_MALE;
}

//------------------------------------------------------------------------------
/**
 * @brief       施設インデックスから施設の名前MSGID取得
 * @param       第1引数　施設インデックス FacilitiesIndex
 * @reutrn      施設名のメッセージID
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetFacilitiesShopName(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager        = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaFacilitiesSystem* pJfFacilitiesSystem = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem() : pJfGimmick->GetJoinFestaFacilitiesSystem();


  JoinFestaScript::FacilitiesIndex facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>(ptr[1]);

  const NetApp::JoinFesta::JoinFestaFacilitiesData* pData = pJfFacilitiesSystem->GetFacilitiesData( facilitiesIndex ); 

  if( pData != NULL )
  {
    return pData->GetShopNameMsgId();
  }

  // 落ちないように
  return 0;
}


//------------------------------------------------------------------------------
/**
 * @brief       施設インデックスから施設の説明MSGID取得
 * @param       第1引数　施設インデックス FacilitiesIndex
 * @reutrn      施設説明のメッセージID
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetFacilitiesDescription(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager        = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();
  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaFacilitiesSystem* pJfFacilitiesSystem = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem() : pJfGimmick->GetJoinFestaFacilitiesSystem();


  JoinFestaScript::FacilitiesIndex facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>(ptr[1]);

  const NetApp::JoinFesta::JoinFestaFacilitiesData* pData = pJfFacilitiesSystem->GetFacilitiesData( facilitiesIndex ); 

  if( pData != NULL )
  {
    return pData->GetShopDescriptionMsgId();
  }

  // 落ちないように
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief       施設IDから施設のランク取得
 * @param       第1引数　施設ID（全施設でユニークなID）
 * @return      施設のランク(1～5）
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetFacilitiesRank(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork       = pScript->GetWork();
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaResourceManager*  pJfResourceManager = isRoom ? pJfGimmickRoom->GetJoinFestaResourceManager() : pJfGimmick->GetJoinFestaResourceManager();
  const NetApp::JoinFesta::JoinFestaDataTable*  p_jfDataTable      = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem()->GetDataTable() : pJfGimmick->GetJoinFestaFacilitiesSystem()->GetDataTable();

  u32 facilitiesId= ptr[1]; // 施設ID

  {
    const NetApp::JoinFesta::ShopItemInfo* pShop = p_jfDataTable->GetShopItemInfo( facilitiesId );
    GFL_ASSERT_MSG(pShop, "%d", facilitiesId);
    if( pShop != NULL )
    {
      return pShop->rank;
    }
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       ご褒美イベントでの施設抽選
 * @retval      WK_SYS_JF_FACILITIES_ID     抽選した施設ID
 * @retval      WK_SYS_JF_FACILITIES_CHARA  ランダムな店員のキャラクターID
 * @retval      WK_SYS_JF_FACILITIES_TYPE   抽選した施設の施設タイプ
 * @retval      WK_SYS_JF_FACILITIES_COLOR  抽選した施設の色
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FacilitiesLottery(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork       = pScript->GetWork();
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();
  print::WordSet*                               pWordSet          = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();
  Savedata::JoinFestaDataSave*                  pSave             = pGameManager->GetGameData()->GetJoinFestaData();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  const NetApp::JoinFesta::JoinFestaDataTable*  p_jfDataTable      = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem()->GetDataTable() : pJfGimmick->GetJoinFestaFacilitiesSystem()->GetDataTable();

  //施設抽選用乱数作成
  gfl2::math::Random random;
  u32                  randomSeed = pSave->GetFacilitiesLotterySeed();
  random.Initialize( randomSeed );

  // 施設の抽選
  u32 hitFacilitiesId = NetAppLib::JoinFesta::JoinFestaFacilitiesUtil::LotteryFacilitiesId( &random, p_jfDataTable );

  // 抽選した施設ID
  pScriptWork->SetWorkValue( WK_SYS_JF_FACILITIES_ID, hitFacilitiesId );
  // ランダムな店員のキャラクターID
  pScriptWork->SetWorkValue( WK_SYS_JF_FACILITIES_CHARA, random.Next( NetAppLib::JoinFesta::JoinFestaFacilitiesUtil::CHARACTER_TYPE_MAX ) );

  // 抽選した施設の施設タイプ
  {
    const NetApp::JoinFesta::ShopItemInfo* pShop = p_jfDataTable->GetShopItemInfo( hitFacilitiesId );
    pScriptWork->SetWorkValue( WK_SYS_JF_FACILITIES_TYPE, pShop->shopType );
  }
  // 抽選した施設の色
  {
    const NetApp::JoinFesta::ShopItemInfo* pShop = p_jfDataTable->GetShopItemInfo( hitFacilitiesId );
    const JoinFestaScript::FacilitiesColor color = NetAppLib::JoinFesta::JoinFestaFacilitiesUtil::GetRandomColor( &random, static_cast<JoinFestaScript::FacilitiesType>(pShop->shopType), hitFacilitiesId );
    ICHI_PRINT(">ICHI 抽選カラー:%d \n", color );
    pScriptWork->SetWorkValue( WK_SYS_JF_FACILITIES_COLOR, color );
  }

  // 抽選した結果で確定したので、シード値をリセット
  pSave->ResetFacilitiesLotterySeed();

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       施設を変更する
 * @param       第1引数　変更する施設インデックス
 * @param       第2引数　新しい施設のID（全施設のユニークID）
 * @param       第3引数  新しい施設の店員
 * @param       第4引数  新しい施設の色
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FacilitiesChange(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork       = pScript->GetWork();
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*                  pSave               = pGameManager->GetGameData()->GetJoinFestaData();

  JoinFestaScript::FacilitiesIndex facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>( ptr[1] ); // 施設インデックス
  u8 newFacilitiesId = ptr[2]; // 施設ID
  u32 newFacilitiesCharacterId = ptr[3];
  u8 newFacilitiesColor = ptr[4];

  pSave->FacilitiesChange( facilitiesIndex, newFacilitiesId, newFacilitiesCharacterId, newFacilitiesColor );

  // ジョインフェスタでお店を建てた回数
  Savedata::IncRecord( Savedata::Record::RECID_JF_SHOP_GET_CNT );

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       施設の場所を入れ替える
 * @param       第1引数　変更する施設インデックス1
 * @param       第2引数　変更する施設インデックス2
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FacilitiesChangeIndex(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork       = pScript->GetWork();
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*                  pSave               = pGameManager->GetGameData()->GetJoinFestaData();

  JoinFestaScript::FacilitiesIndex facilitiesIndex1 = static_cast<JoinFestaScript::FacilitiesIndex>( ptr[1] ); // 施設インデックス
  JoinFestaScript::FacilitiesIndex facilitiesIndex2 = static_cast<JoinFestaScript::FacilitiesIndex>( ptr[2] ); // 施設インデックス

  pSave->FacilitiesChangeIndex( facilitiesIndex1, facilitiesIndex2 );

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       施設のモデルを最新情報で更新する
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FacilitiesModelUpdate(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::FieldScript::FieldScriptSystem*        pScriptSystem     = pGameManager->GetFieldScriptSystem();

  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, NetApp::JoinFesta::JoinFestaFacilitiesModelUpdateEvent>( pGameManager->GetGameEventManager() );
  pScriptSystem->Suspend();

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       施設を変えたふりをする
 * @param       第1引数　変更する施設インデックス
 * @param       第2引数　新しい施設のID（全施設のユニークID）
 * @param       第3引数  新しい施設の店員
 * @param       第4引数  新しい施設の色
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FacilitiesFakeChangeStart(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork       = pScript->GetWork();
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*                  pSave             = pGameManager->GetGameData()->GetJoinFestaData();

  JoinFestaScript::FacilitiesIndex facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>( ptr[1] ); // 施設インデックス
  u8 facilitiesId = ptr[2]; // 施設ID
  u32 characterId = ptr[3];
  u8 color = ptr[4];

  pSave->FacilitiesFakeChangeStart( facilitiesIndex, facilitiesId, characterId, color );

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       施設を変えたふりを終了する
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FacilitiesFakeChangeEnd(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork       = pScript->GetWork();
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*                  pSave             = pGameManager->GetGameData()->GetJoinFestaData();

  pSave->FacilitiesFakeChangeEnd();

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       施設の紹介者情報をセット
 * @param       第1引数　セットする施設インデックス
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL SetFacilitiesIntroducer(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager        = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaFacilitiesSystem* pJfFacilitiesSystem = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem() : pJfGimmick->GetJoinFestaFacilitiesSystem();


  // 話しかけた相手のJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetScriptTalkFieldPersonalData(); 
  GFL_ASSERT(jfPersonal);

  JoinFestaScript::FacilitiesIndex facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>(ptr[1]);

  NetApp::JoinFesta::JoinFestaFacilitiesData* pData = pJfFacilitiesSystem->GetFacilitiesData( facilitiesIndex ); 
  GFL_ASSERT(pData);

  if( jfPersonal != NULL && pData != NULL )
  {
    pData->SetIntroducer( jfPersonal );
  }

  return NULL;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  占いやの抽選
 *  @param  第1引数　施設インデックス FacilitiesIndex
 *  @retval SCWK_RET0 占いの当たり結果
 *  @retval SCWK_RET1 占いの行動結果
 *  @retval SCWK_RET2 当たった定型文メッセージID（男性用のメッセージID）
 *  @retval SCWK_RET3 当たった定型文メッセージが初Getかどうか(trueで初Get）
 */
/* ----------------------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FortuneLottery(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork         = pScript->GetWork();
  GameSys::GameManager*                         pGameManager        = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap           = pGameManager->GetFieldmap();
  Savedata::JoinFestaDataSave*                  pSave               = pGameManager->GetGameData()->GetJoinFestaData();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaFacilitiesSystem* pJfFacilitiesSystem = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem() : pJfGimmick->GetJoinFestaFacilitiesSystem();


  JoinFestaScript::FacilitiesIndex facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>(ptr[1]);

  NetApp::JoinFesta::JoinFestaFacilitiesData* pData = pJfFacilitiesSystem->GetFacilitiesData( facilitiesIndex ); 

  if( pData != NULL )
  {
    // 抽選用乱数作成
    gfl2::math::Random random;
    random.Initialize( pData->GetRandomSeed() );


    // [当たり・中あたり・大当たり]の抽選
    JoinFestaScript::FortuneResultHit hitResult = pData->LotteryFortuneResultHit( &random );

    // 当たり行動の抽選
    JoinFestaScript::FortuneResultAction actionResult = pData->LotteryFortuneResultAction( &random );

    // 当たる定型文の抽選
    u32 selectWordMessageId = pData->LotteryExecute( &random );
    const bool isNewGet = ( pSave->IsUseSelectWord( selectWordMessageId ) == false ) ? true : false;

    // 定型文の開放
    if( isNewGet == true )
    {
      pSave->SetUseSelectWord( selectWordMessageId );
    }

    // 乱数シードリセット
    pData->ResetRandomSeed();

    // 施設セーブデータに保存
    pSave->SetFortuneResult( facilitiesIndex, hitResult, actionResult );  

    // スクリプトの返す戻り値
    pScriptWork->SetWorkValue( SCWK_RET0, hitResult );
    pScriptWork->SetWorkValue( SCWK_RET1, actionResult );
    pScriptWork->SetWorkValue( SCWK_RET2, selectWordMessageId );
    pScriptWork->SetWorkValue( SCWK_RET3, isNewGet );
    
    return NULL;
  }

  return NULL;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  占いやの結果取得（当たり）
 *  @param  第1引数　結果を取得したい行動
 *  @return 占いの当たり結果
 *  @li     占いやが複数ある場合、結果として1番良いのを返す
 */
/* ----------------------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL GetFortuneResultHitByAction(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*                  pSave             = pGameManager->GetGameData()->GetJoinFestaData();

  JoinFestaScript::FortuneResultAction fortuneResultAction = static_cast<JoinFestaScript::FortuneResultAction>(ptr[1]);

  return pSave->GetFortuneResultHitByAction( fortuneResultAction );
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  くじ屋でくじを引く
 *  @param  第1引数　施設インデックス FacilitiesIndex
 *  @retval SCWK_RET0 当たったアイテム
 *  @retval SCWK_RET1 何等か（1オリジン）
 *  @retval SCWK_RET2 再挑戦出来るかどうか
 */
/* ----------------------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL DrawLots(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork         = pScript->GetWork();
  GameSys::GameManager*                         pGameManager        = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap           = pGameManager->GetFieldmap();
  Savedata::JoinFestaDataSave*                  pSave             = pGameManager->GetGameData()->GetJoinFestaData();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaFacilitiesSystem* pJfFacilitiesSystem = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem() : pJfGimmick->GetJoinFestaFacilitiesSystem();


  JoinFestaScript::FacilitiesIndex facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>(ptr[1]);

  const NetApp::JoinFesta::JoinFestaFacilitiesData* pData = pJfFacilitiesSystem->GetFacilitiesData( facilitiesIndex ); 

  if( pData != NULL )
  {
    // 抽選用乱数作成
    gfl2::math::Random random;
    random.Initialize( pData->GetRandomSeed() );

    // 当たるアイテムの抽選
    u32 hitItemId = pData->LotteryExecute( &random );

    // なん等か調べる
    u32 hitRank = pData->GetLotInfoIndexByItemId(hitItemId) + 1; // 1オリジンにする

    // 再挑戦出来るかどうか
    bool isRetry = pData->IsRetryDrawLot( &random );

    /**
      2017/04/21 Fri. Yu Muto
      RECID_GLOBAL_ATTRACTION_LUCK
        ->RECID_POKE_EVOLUTION
      となったため削除
    */
/*
    if( hitRank == 1 )
    {
      // グローバルアトラクション用
      Savedata::IncRecord( Savedata::Record::RECID_GLOBAL_ATTRACTION_LUCK );
    }
*/

    // スクリプトの返す戻り値
    pScriptWork->SetWorkValue( SCWK_RET0, hitItemId );
    pScriptWork->SetWorkValue( SCWK_RET1, hitRank );
    pScriptWork->SetWorkValue( SCWK_RET2, isRetry );
  }

  return NULL;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  くじ屋でくじを引くのを終了
 *  @param  第1引数　施設インデックス FacilitiesIndex
 */
/* ----------------------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL DrawLotsEnd(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager        = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap           = pGameManager->GetFieldmap();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaFacilitiesSystem* pJfFacilitiesSystem = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem() : pJfGimmick->GetJoinFestaFacilitiesSystem();


  JoinFestaScript::FacilitiesIndex facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>(ptr[1]);

  NetApp::JoinFesta::JoinFestaFacilitiesData* pData = pJfFacilitiesSystem->GetFacilitiesData( facilitiesIndex ); 

  if( pData != NULL )
  {
    // 乱数シードリセット
    pData->ResetRandomSeed();
  }

  return NULL;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  ビックリハウスの抽選
 *  @param  第1引数　施設インデックス FacilitiesIndex
 *  @retval SCWK_RET0 抽選したアイテムID1
 *  @retval SCWK_RET1 抽選したアイテムID2（無い場合は0)
 *  @retval SCWK_RET2 抽選したアイテムID3（無い場合は0)
 */
/* ----------------------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FunHouseLottery(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork         = pScript->GetWork();
  GameSys::GameManager*                         pGameManager        = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap           = pGameManager->GetFieldmap();
  Savedata::JoinFestaDataSave*                  pSave               = pGameManager->GetGameData()->GetJoinFestaData();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaFacilitiesSystem* pJfFacilitiesSystem = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem() : pJfGimmick->GetJoinFestaFacilitiesSystem();


  JoinFestaScript::FacilitiesIndex facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>(ptr[1]);

  NetApp::JoinFesta::JoinFestaFacilitiesData* pData = pJfFacilitiesSystem->GetFacilitiesData( facilitiesIndex ); 

  if( pData != NULL )
  {
    // アイテム抽選
    u32 hitItemId[3] = {0, 0, 0};
    pData->LotteryFunHouse( hitItemId );

    // ポケマメだった場合、直接追加する
    for( u32 itemCount=0; itemCount<3; itemCount++ )
    {
      if( hitItemId[ itemCount ] == JoinFestaScript::POKEMAME_ITEM_ID )
      {
        // 青色のポケマメを１つ増やす
        pGameManager->GetGameData()->GetResortSave()->SetPokeBeansNum( Savedata::ResortSave::POKEBEANS_PATTERN_BLUE, 1 );

        // レコード追加
        // 不要になった
        // Savedata::IncRecord( Savedata::Record::RECID_MAME_GET_CNT );
      }
    }

    // スクリプトの返す戻り値
    pScriptWork->SetWorkValue( SCWK_RET0, hitItemId[0] );
    pScriptWork->SetWorkValue( SCWK_RET1, hitItemId[1] );
    pScriptWork->SetWorkValue( SCWK_RET2, hitItemId[2] );
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       一押し施設の紹介料を取得
 * @param       第1引数　紹介する施設ID（全施設でユニークなID）
 * @return      紹介に必要なフェスコイン
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetRecommendShopPrice(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork       = pScript->GetWork();
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();
  Savedata::JoinFestaDataSave*                  pSave             = pGameManager->GetGameData()->GetJoinFestaData();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  const NetApp::JoinFesta::JoinFestaDataTable*  p_jfDataTable      = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem()->GetDataTable() : pJfGimmick->GetJoinFestaFacilitiesSystem()->GetDataTable();

  u32 facilitiesId= ptr[1]; // 施設ID

  const NetApp::JoinFesta::ShopItemInfo* pShop = p_jfDataTable->GetShopItemInfo( facilitiesId );
  GFL_ASSERT_MSG(pShop, "%d", facilitiesId);
  if( pShop != NULL )
  {
    // 基本値 + 同じ施設IDがある数x2
    u8 sameCount = 1;
    for( u8 i=0; i<JoinFestaScript::FACILITIES_INDEX_MAX; i++ )
    {

      NetApp::JoinFesta::JoinFestaFacilitiesData::CoreData* pFacilitiesData = pSave->GetFacilitiesData( static_cast<JoinFestaScript::FacilitiesIndex>(i) );
      if( facilitiesId == pFacilitiesData->facilitiesId )
      {
        sameCount++;
      }
    }

    // 8店舗目は7店舗目と同じ
    if( sameCount > JoinFestaScript::FACILITIES_INDEX_MAX )
    {
      sameCount = JoinFestaScript::FACILITIES_INDEX_MAX;
    }

    f32 price = static_cast<f32>( pShop->price * sameCount );

    // @fix NMCat[2083]：倍率高すぎるので削除
    //// 2店舗目以降は1.5倍
    //if( sameCount > 1 )
    //{
    //  price *= 1.5f;
    //}

    return price;
  }

  return 999;
}

//------------------------------------------------------------------------------
/**
 * @brief       話し相手の施設IDを取得
 * @param       第1引数　施設インデックス FacilitiesIndex
 * @reutrn      FacilitiesId
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetTargetFacilitiesId(AMX* amx, const cell * ptr)
{
  // 話しかけた相手のJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetScriptTalkFieldPersonalData(); 

  u32 facilitiesIndex = ptr[1];

  if( jfPersonal )
  {
    NetAppLib::JoinFesta::PacketGameStatus1 packet;
    if( jfPersonal->GetPacketGameStatus1( &packet ) )
    {
      return packet.facilitiesId[ facilitiesIndex ];
    }
  }

  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief       こうかんじょの残り回数を取得
 * @param       第1引数　施設インデックス FacilitiesIndex
 * @reutrn      残り回数
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetTradeRemainCount( AMX* amx, const cell * ptr )
{
  Field::FieldScript::ScriptObject*             pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager        = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap           = pGameManager->GetFieldmap();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaFacilitiesSystem* pJfFacilitiesSystem = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem() : pJfGimmick->GetJoinFestaFacilitiesSystem();


  JoinFestaScript::FacilitiesIndex facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>(ptr[1]);

  NetApp::JoinFesta::JoinFestaFacilitiesData* pData = pJfFacilitiesSystem->GetFacilitiesData( facilitiesIndex ); 

  if( pData != NULL )
  {
    // こうかんじょの残り回数取得
    return pData->GetTradeRemainCount();
  }

  GFL_ASSERT_MSG( 0, "%d", facilitiesIndex );
  return 0;
}
//------------------------------------------------------------------------------
/**
 * @brief       こうかんじょの残り回数をデクリメント
 * @param       第1引数　施設インデックス FacilitiesIndex
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL SubTradeRemainCount( AMX* amx, const cell * ptr )
{
  Field::FieldScript::ScriptObject*             pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager        = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap           = pGameManager->GetFieldmap();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaFacilitiesSystem* pJfFacilitiesSystem = isRoom ? pJfGimmickRoom->GetJoinFestaFacilitiesSystem() : pJfGimmick->GetJoinFestaFacilitiesSystem();


  JoinFestaScript::FacilitiesIndex facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>(ptr[1]);

  NetApp::JoinFesta::JoinFestaFacilitiesData* pData = pJfFacilitiesSystem->GetFacilitiesData( facilitiesIndex ); 

  if( pData != NULL )
  {
    // こうかんじょの残り回数取得
    pData->SubTradeRemainCount();
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       ランクアップエフェクト開始
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL StartRankupEffect(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork       = pScript->GetWork();
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaEffectSystem*  pJoinFestaEffectSystem = isRoom ? pJfGimmickRoom->GetJoinFestaEffectSystem() : pJfGimmick->GetJoinFestaEffectSystem();

  pJoinFestaEffectSystem->Start_Rankup();

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       ランクアップエフェクト停止
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL StopRankupEffect(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork       = pScript->GetWork();
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaEffectSystem*  pJoinFestaEffectSystem = isRoom ? pJfGimmickRoom->GetJoinFestaEffectSystem() : pJfGimmick->GetJoinFestaEffectSystem();

  pJoinFestaEffectSystem->Stop_Rankup();

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       ショップ獲得エフェクト開始
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL StartNewShopEffect(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork       = pScript->GetWork();
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaEffectSystem*  pJoinFestaEffectSystem = isRoom ? pJfGimmickRoom->GetJoinFestaEffectSystem() : pJfGimmick->GetJoinFestaEffectSystem();

  pJoinFestaEffectSystem->Start_NewShop();

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       ショップ獲得エフェクト停止
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL StopNewShopEffect(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork       = pScript->GetWork();
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaEffectSystem*  pJoinFestaEffectSystem = isRoom ? pJfGimmickRoom->GetJoinFestaEffectSystem() : pJfGimmick->GetJoinFestaEffectSystem();

  pJoinFestaEffectSystem->Stop_NewShop();

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       アトラクション開始エフェクト開始
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL StartAttractionEffect(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork       = pScript->GetWork();
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaEffectSystem*  pJoinFestaEffectSystem = isRoom ? pJfGimmickRoom->GetJoinFestaEffectSystem() : pJfGimmick->GetJoinFestaEffectSystem();

  pJoinFestaEffectSystem->Start_Attraction();

  return NULL;
}


//------------------------------------------------------------------
/**
  *  @brief    施設座標系からWorldへ変換
  *  @param    第1引数　施設インデックス
  *  @return   桁上げに使用したScale
  */
//------------------------------------------------------------------
cell AMX_NATIVE_CALL nJFFacilityCoordToWorld(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager        = pScript->GetGameManager();
  Field::FieldGimmickJoinFesta*                 pJfGimmick          = static_cast<Field::FieldGimmickJoinFesta*>(pGameManager->GetFieldmap()->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE ));
  NetApp::JoinFesta::JoinFestaFacilitiesSystem* pJfFacilitiesSystem = pJfGimmick->GetJoinFestaFacilitiesSystem();
  Field::FieldScript::Work*                     pScriptWork         = pScript->GetWork();
  JoinFestaScript::FacilitiesIndex              facilitiesIndex     = static_cast<JoinFestaScript::FacilitiesIndex>( ptr[1] ); // 施設インデックス
  NetApp::JoinFesta::JoinFestaFacilitiesModel*  pFacilityModel      = pJfFacilitiesSystem->GetFacilityModel(facilitiesIndex);
  const gfl2::math::Vector3                     localPos(amx_ctof(ptr[2]), amx_ctof(ptr[3]), amx_ctof(ptr[4]));
  gfl2::math::Vector3                           worldPos;
  const gfl2::math::Matrix34*                   pWorldMatrix        = pFacilityModel->GetWorldMatrix();


  worldPos  = pWorldMatrix->TransformCoord(localPos);

  pScriptWork->SetWorkValue(SCWK_RET0, amx_ftoc(worldPos.x));
  pScriptWork->SetWorkValue(SCWK_RET1, amx_ftoc(worldPos.y));
  pScriptWork->SetWorkValue(SCWK_RET2, amx_ftoc(worldPos.z));

#if PM_DEBUG
#if 0
  {
    /*  カメラ調整用  */
    const gfl2::math::Vector3                     __worldPos( 5661.682617f, 231.504715f, 4191.612793f);
    const gfl2::math::Vector3                     __worldAt(  4423.280762f, 8.961059f, 4203.804199f);
    const JoinFestaScript::FacilitiesIndex        __targetFacilityIndex     = static_cast<JoinFestaScript::FacilitiesIndex>(1);   /**<  上記座標を取るときに基準とした施設のID  */
    NetApp::JoinFesta::JoinFestaFacilitiesModel*  __pFacilityModel          = pJfFacilitiesSystem->GetFacilityModel(__targetFacilityIndex);
    gfl2::math::Vector3                           __localPos;
    gfl2::math::Vector3                           __localAt;
    gfl2::math::Matrix34                          __InvWorldMtx;


    __InvWorldMtx = __pFacilityModel->GetWorldMatrix()->Inverse();
    __localPos    = __InvWorldMtx.TransformCoord(__worldPos);
    __localAt     = __InvWorldMtx.TransformCoord(__worldAt);

    GFL_PRINT("JF_FacilityLocal_Pos   : %.5f, %.5f, %.5f\n", __localPos.x, __localPos.y, __localPos.z);
    GFL_PRINT("JF_FacilityLocal_LookAt: %.5f, %.5f, %.5f\n", __localAt.x,  __localAt.y,  __localAt.z);
  }
#endif
#endif
  return 0;
}

//------------------------------------------------------------------
/**
  *  @brief    話しかけ人物座標系からWorldへ変換
  *  @return   桁上げに使用したScale
  */
//------------------------------------------------------------------
cell AMX_NATIVE_CALL nJFPersonalCoordToWorld(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::FieldScript::Work*                     pScriptWork       = pScript->GetWork();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();

  // 城外ギミックか城内ギミックか
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JFModelManager*  pJfModelManager = isRoom ? pJfGimmickRoom->GetJFDynamicLoadSystem()->GetJFModelManager() : pJfGimmick->GetJFDynamicLoadSystem()->GetJFModelManager();

  // 動作モデルのイベントID
  u32 eventId = pScriptWork->GetTempWorkValue( SCWK_TARGET_OBJID );

  gfl2::math::Matrix34 worldMatrix;
  {
    const gfl2::math::Vector3* pWorldPos = pJfModelManager->GetWorldPosition( eventId );
    if( pWorldPos == NULL )
    {
      return NULL;
    }
    ICHI_PRINT(">ICHI pWorldPos  : %.5f, %.5f, %.5f\n", pWorldPos->x, pWorldPos->y, pWorldPos->z);

    // 外側向かす
    const gfl2::math::Vector3 centerPosition( 4200.0f, 0.0f, 4200.0f ); // マップの真ん中
    // 中心点からのベクトル
    gfl2::math::Vector3 vec = (*pWorldPos - centerPosition).Normalize();
    ICHI_PRINT(">ICHI vec  : %.5f, %.5f, %.5f\n", vec.x, vec.y, vec.z);
    f32 radian = gfl2::math::Atan2Rad(vec.x,vec.z);
    ICHI_PRINT(">ICHI 動作モデルの角度 %f \n", gfl2::math::ConvRadToDeg(radian) );

    gfl2::math::Matrix34 rotMat( gfl2::math::Matrix34::GetRotationY( radian ) );
    //gfl2::math::Matrix34 transMat( gfl2::math::Matrix34::GetTranslation( *pWorldPos ) );
    rotMat.SetTranslationVector( *pWorldPos );
    worldMatrix = rotMat;// * transMat;
  }

  const gfl2::math::Vector3                     localPos(amx_ctof(ptr[1]), amx_ctof(ptr[2]), amx_ctof(ptr[3]));
  gfl2::math::Vector3                           worldPos;
  //const gfl2::math::Matrix34*                   pWorldMatrix = &pFieldmap->GetPlayerCharacter()->GetCharaDrawInstance()->GetWorldMatrix();

  //if( pWorldMatrix != NULL )
  {
    //worldPos  = pWorldMatrix->TransformCoord(localPos);
    worldPos  = worldMatrix.TransformCoord(localPos);

    pScriptWork->SetWorkValue(SCWK_RET0, amx_ftoc(worldPos.x));
    pScriptWork->SetWorkValue(SCWK_RET1, amx_ftoc(worldPos.y));
    pScriptWork->SetWorkValue(SCWK_RET2, amx_ftoc(worldPos.z));
    ICHI_PRINT(">ICHI localPos  : %.5f, %.5f, %.5f\n", localPos.x, localPos.y, localPos.z);
    ICHI_PRINT(">ICHI WorldPos  : %.5f, %.5f, %.5f\n", worldPos.x, worldPos.y, worldPos.z);
  }

#if PM_DEBUG
#if 0
  {
    gfl2::math::Vector3 targetPos[7];
    gfl2::math::Vector3 cameraPos[7];

    targetPos[0] = gfl2::math::Vector3( 4418.176758f, 73.009140f, 4670.442871f );
    cameraPos[0] = gfl2::math::Vector3( 4904.291016f, 196.642960f, 5760.985352f );
    targetPos[1] = gfl2::math::Vector3( 4134.681152f, -133.993729f, 4908.948242f );
    cameraPos[1] = gfl2::math::Vector3( 5042.470703f, 244.510849f, 5597.081543f );
    targetPos[2] = gfl2::math::Vector3( 4289.295898f, -235.393463f, 4505.512207f );
    cameraPos[2] = gfl2::math::Vector3( 4788.889160f, 256.128387f, 5480.219727f );
    targetPos[3] = gfl2::math::Vector3( 4365.578125f, 31.708086f, 4567.752441f );
    cameraPos[3] = gfl2::math::Vector3( 4851.692383f, 155.341904f, 5658.294922f );
    targetPos[4] = gfl2::math::Vector3( 4778.508301f, -113.006783f, 4655.614258f );
    cameraPos[4] = gfl2::math::Vector3( 4703.831055f, 269.563873f, 5790.928711f );
    targetPos[5] = gfl2::math::Vector3( 4539.368652f, -136.050629f, 4458.790527f );
    cameraPos[5] = gfl2::math::Vector3( 4767.257324f, 195.499695f, 5589.727051f );
    targetPos[6] = gfl2::math::Vector3( 4402.009277f, -233.280060f, 4667.504883f );
    cameraPos[6] = gfl2::math::Vector3( 4861.183105f, 270.037445f, 5655.730957f );

    for( int i=0; i<7; i++ )
    {
      /*  カメラ調整用  */
      gfl2::math::Vector3                           __localPos;
      gfl2::math::Vector3                           __localAt;
      gfl2::math::Matrix34                          __InvWorldMtx;

      __InvWorldMtx = worldMatrix.Inverse();
      __localPos    = __InvWorldMtx.TransformCoord(cameraPos[i]);
      __localAt     = __InvWorldMtx.TransformCoord(targetPos[i]);

      GFL_PRINT("-- %d --\n", i);
      GFL_PRINT("JF_ParsonalLocal_Pos   : %.5f, %.5f, %.5f\n", __localPos.x, __localPos.y, __localPos.z);
      GFL_PRINT("JF_ParsonalLocal_LookAt: %.5f, %.5f, %.5f\n", __localAt.x,  __localAt.y,  __localAt.z);
    }
  }
#endif
#endif

  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief       染められる着せ替えを所持しているか
 * @reutrn      trueで所持している
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL IsHaveColorDressup(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager        = pScript->GetGameManager();
  Savedata::Fashion*                            pFashionSave        = pGameManager->GetGameData()->GetFashion();
  Savedata::MyStatus*                           pMyStatus           = pGameManager->GetGameData()->GetPlayerStatus();   

  // 所持している着せ替えアイテムを全てチェック
  for( u32 categoryIdx=0; categoryIdx<Savedata::Fashion::CATEGORY_MAX; categoryIdx++ )
  {
    // アイテムカテゴリ
    const Savedata::Fashion::Category saveCategoryIdx = static_cast<Savedata::Fashion::Category>(categoryIdx);
    // グッズ数
    const u32 goodsNum = app::util::Kisekae::KisekaeDataBase::GetGoodsDataNum( saveCategoryIdx );
    for( u32 goodsIdx = 0; goodsIdx < goodsNum; ++goodsIdx )
    {
      const table_data_GoodsData* pGoodsData = app::util::Kisekae::KisekaeDataBase::GetGoodsData( saveCategoryIdx, goodsIdx );
      if( pGoodsData != NULL )
      {
        // 性別と合っているグッズかどうか
        if( pMyStatus->GetSex() == pGoodsData->sex )
        {
          s16 itemId = (s16)pGoodsData->item_id;

          // 所持チェック
          if( pFashionSave->IsOwned( categoryIdx, itemId ) == 0 )
          {
            continue;// 未所持
          }

          // 染められるアイテムかチェック
          if( pGoodsData->color_change == 1 )
          { 
            ICHI_PRINT(">ICHI 染アイテムだった ItemCategory:%d ItemIdx:%d \n", categoryIdx, itemId );
            return true;
          }
        }
      }
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief       MenuのItemに染める色リストを追加する
 *              選択IDにはカラーIDが入る
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL ListMenuAddDyeColor(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork         = pScript->GetWork();
  GameSys::GameManager*                         pGameManager        = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap           = pGameManager->GetFieldmap();
  Field::FieldGimmickJoinFesta*                 pJfGimmick          = static_cast<Field::FieldGimmickJoinFesta*>(pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE ));
  NetApp::JoinFesta::JoinFestaFacilitiesSystem* pJfFacilitiesSystem = pJfGimmick->GetJoinFestaFacilitiesSystem();
  Field::FieldScript::SystemSingletones*        pScriptSystemSingletones = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();

  GFL_ASSERT( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA );
  GFL_ASSERT( pScriptSystemSingletones->m_pListMenu );
  if( !pScriptSystemSingletones->m_pListMenu ) return true;


  // 話しかけた動作モデルが店員かチェック
  u32 eventId = pScriptWork->GetTempWorkValue( SCWK_TARGET_OBJID );
  if( eventId >= NetApp::JoinFesta::JoinFestaDefine::ASSISTANT_EVENT_ID_OFFSET )
  {
    JoinFestaScript::FacilitiesIndex facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>( eventId - NetApp::JoinFesta::JoinFestaDefine::ASSISTANT_EVENT_ID_OFFSET);
    GFL_PRINT(">施設インデックス %d \n", facilitiesIndex);

    const NetApp::JoinFesta::JoinFestaFacilitiesData* pFacilitiesData = pJfFacilitiesSystem->GetFacilitiesData( facilitiesIndex );

    // 施設のデータから欲しい情報引っ張ってくる
    if( pFacilitiesData != NULL  )
    {
      // サービス０番目にショップIDを入れている
      const u32 shopId = pFacilitiesData->GetServiceID(0);

      const u32 shopDataNum = app::util::Kisekae::KisekaeDataBase::GetColorShopDataNum( shopId );

      for( u32 i=0; i<shopDataNum; i++ )
      {
        const table_data_DyeingData* pDyeingData = app::util::Kisekae::KisekaeDataBase::GetColorShopData( shopId, i );

        // きのみコインにデータ入っている = きのみで染める色
        if(  pDyeingData->kinomi_coin > 0 )
        {
          u32 colorMessageId = app::util::Kisekae::KisekaeDataBase::GetColorMsgIdForColorId( pDyeingData->color_id );
          pScriptSystemSingletones->m_pListMenu->AddItem( colorMessageId, pDyeingData->color_id ); 
        }
      }
    }
  }


  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       染め用の着せ替えショップID取得
 * @param       第1引数　isKinomi   きのみ染めかどうか
 * @param       第2引数　colorId    きのみ染めの場合色ID
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetKisekaeShopId(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript             = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork         = pScript->GetWork();
  GameSys::GameManager*                         pGameManager        = pScript->GetGameManager();
  Field::FieldGimmickJoinFesta*                 pJfGimmick          = static_cast<Field::FieldGimmickJoinFesta*>(pGameManager->GetFieldmap()->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE ));
  NetApp::JoinFesta::JoinFestaFacilitiesSystem* pJfFacilitiesSystem = pJfGimmick->GetJoinFestaFacilitiesSystem();
  Field::FieldScript::FieldScriptSystem*        pScriptSystem       = pGameManager->GetFieldScriptSystem();
  Field::FieldScript::SystemSingletones*        pScriptSystemSingletones = pScriptSystem->GetSystemWork()->GetSingletones();


  bool isKinomi = GFL_BOOL_CAST(ptr[1]);
  s32  colorId  = ptr[2];

  // 話しかけた動作モデルが店員かチェック
  u32 eventId = pScriptWork->GetTempWorkValue( SCWK_TARGET_OBJID );
  if( eventId >= NetApp::JoinFesta::JoinFestaDefine::ASSISTANT_EVENT_ID_OFFSET )
  {
    JoinFestaScript::FacilitiesIndex facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>( eventId - NetApp::JoinFesta::JoinFestaDefine::ASSISTANT_EVENT_ID_OFFSET);
    GFL_PRINT(">施設インデックス %d \n", facilitiesIndex);

    const NetApp::JoinFesta::JoinFestaFacilitiesData* pFacilitiesData = pJfFacilitiesSystem->GetFacilitiesData( facilitiesIndex );

    // 施設のデータから欲しい情報引っ張ってくる
    if( pFacilitiesData != NULL  )
    {
      // サービス０番目にショップIDを入れている
      const u32 shopId = pFacilitiesData->GetServiceID(0);
      GFL_PRINT(">ショップID %d \n", shopId );
      return shopId;
    }
  }

  GFL_ASSERT(0);
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief       リクルーターで選んだ人物呼び出し
 * @return      呼び出した動作モデルのID
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL RecruiterPersonalLoad(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::FieldScript::FieldScriptSystem*        pScriptSystem     = pGameManager->GetFieldScriptSystem();

  NetApp::JoinFesta::JoinFestaRecruiterEvent::StartEvent( pGameManager, NetApp::JoinFesta::JoinFestaRecruiterEvent::BOOT_MODE_LOAD );

  pScriptSystem->Suspend();

  // 必ず1番目
  return NetApp::JoinFesta::JoinFestaDefine::MODEL_EVENT_ID_OFFSET;
}

//------------------------------------------------------------------------------
/**
 * @brief       リクルーターで選んだ人物破棄
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL RecruiterPersonalUnload(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::FieldScript::FieldScriptSystem*        pScriptSystem     = pGameManager->GetFieldScriptSystem();

  NetApp::JoinFesta::JoinFestaRecruiterEvent::StartEvent( pGameManager, NetApp::JoinFesta::JoinFestaRecruiterEvent::BOOT_MODE_UNLOAD );

  pScriptSystem->Suspend();

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       チュートリアルNPC呼び出し
 * @param       第1引数　bootMode 0:最初の１人 1:次の２人
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL TutorialPersonalLoad(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::FieldScript::FieldScriptSystem*        pScriptSystem     = pGameManager->GetFieldScriptSystem();

  NetApp::JoinFesta::JoinFestaTutorialEvent::BootMode bootMode = static_cast<NetApp::JoinFesta::JoinFestaTutorialEvent::BootMode>(ptr[1]);

  NetApp::JoinFesta::JoinFestaTutorialEvent::StartEvent( pGameManager, bootMode );

  pScriptSystem->Suspend();

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       チュートリアル下画面解放
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL TutorialSubScreenOpen(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript           = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork       = pScript->GetWork();
  GameSys::GameManager*                         pGameManager      = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap         = pGameManager->GetFieldmap();

  Field::SubScreen::FieldSubScreenProcManager*  pSubScreenManager = pFieldmap->GetFieldSubScreenManager();

  // 一旦Suspendを解除してUpdateを回すことでフレームが変わる
  pSubScreenManager->Resume();
  pSubScreenManager->Update();
  pSubScreenManager->Suspend();

  return NULL;
}


//------------------------------------------------------------------------------
/**
 * @brief       動的出現システムの無効化 必ずDynamicLoadEnableを呼んで有効化すること
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL DynamickLoadDisable(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript         = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*           pScriptWork     = pScript->GetWork();
  GameSys::GameManager*               pGameManager    = pScript->GetGameManager();
  Field::Fieldmap*                    pFieldmap       = pGameManager->GetFieldmap();

  // 城内は処理しない
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  if( isRoom ){ return NULL; }

  Field::FieldGimmickJoinFesta* pJfGimmick = static_cast<Field::FieldGimmickJoinFesta*>(pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE ));
  NetApp::JoinFesta::JFDynamicLoadSystem*  pJFDynamicLoadSystem = pJfGimmick->GetJFDynamicLoadSystem();

  // 動的出現システム無効化
  pJFDynamicLoadSystem->DynamickLoadDisable();

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       動的出現システムの無効化
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL DynamickLoadEnable(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript         = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*           pScriptWork     = pScript->GetWork();
  GameSys::GameManager*               pGameManager    = pScript->GetGameManager();
  Field::Fieldmap*                    pFieldmap       = pGameManager->GetFieldmap();

  // 城内は処理しない
  const bool isRoom = ( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  if( isRoom ){ return NULL; }

  Field::FieldGimmickJoinFesta* pJfGimmick = static_cast<Field::FieldGimmickJoinFesta*>(pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE ));
  NetApp::JoinFesta::JFDynamicLoadSystem*  pJFDynamicLoadSystem = pJfGimmick->GetJFDynamicLoadSystem();

  // 動的出現システム有効化
  pJFDynamicLoadSystem->DynamickLoadEnable();

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       無線スイッチがONかどうか
 * @return      falseでOFF
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL IsWLANSwitchEnable(AMX* amx, const cell * ptr)
{
  return NetLib::NijiNetworkSystem::IsWLANSwitchEnable();
}

//------------------------------------------------------------------------------
/**
 * @brief       グローバルアトラクションが参加中か？
 * @return      trueで参加中
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL IsGAttractionParticipate(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript         = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager    = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*                  pSave           = pGameManager->GetGameData()->GetJoinFestaData();

  // どれか1つでも参加中ならば参加中とする
  for( u8 i=0; i<NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::RECORD_MAX; i++ )
  {
    NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData* pGlobalAttractionData = pSave->GetGlobalAttractionData( i );
    if( pGlobalAttractionData->participateState == NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::PARTICIPATE_STATE_OK )
    {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief       グローバルアトラクションの結果取得
 * @return      0の場合はコイン取得無し。0以外はその分コイン取得
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetGAttractionResult(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript         = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager    = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*                  pSave           = pGameManager->GetGameData()->GetJoinFestaData();

  // 終了中かつご褒美未授与をチェックする
  for( u8 i=0; i<NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::RECORD_MAX; i++ )
  {
    NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData* pGlobalAttractionData = pSave->GetGlobalAttractionData( i );
    if( pGlobalAttractionData->openState == NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::OPEN_STATE_TOTAL_FINISH &&
        pGlobalAttractionData->participateState == NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::PARTICIPATE_STATE_OK &&
        pGlobalAttractionData->rewardState == 0 &&
        pGlobalAttractionData->rewardValue > 0 ) // @fix GFNMCat[5629] 受け取る報酬の値が設定されてある時という条件を追加
    {
      // 授与した時間を保存
      GameSys::DeviceDate date;
      gfl2::system::DateParameters  parameters = date.GetDateConst()->GetParameters();

      pGlobalAttractionData->rewardYear   = parameters.year - 2000;
      pGlobalAttractionData->rewardMonth  = parameters.month;              //!< ご褒美受取月
      pGlobalAttractionData->rewardDay    = parameters.day;                //!< ご褒美受取日
      pGlobalAttractionData->rewardHour   = parameters.hour;               //!< ご褒美受取時
      pGlobalAttractionData->rewardMinitue= parameters.minute;             //!< ご褒美受取分

      pGlobalAttractionData->rewardState = 1;
      return pGlobalAttractionData->rewardValue;
    }
  }

  return 0;
}

// バトルフェスコマンド

//------------------------------------------------------------------------------
/**
* @func        InitializeBattleFes
* @brief       バトルフェスの初期化
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL InitializeBattleFes(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  BattleFes::BattleFesManager*        pBattleFesManager = pGameManager->GetGameData()->GetBattleFesManager();
  pBattleFesManager->Create(pGameManager);
  return NULL;
}

//------------------------------------------------------------------------------
/**
* @func        TerminateBattleFes
* @brief       バトルフェスの終了処理
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL TerminateBattleFes(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  BattleFes::BattleFesManager*        pBattleFesManager = pGameManager->GetGameData()->GetBattleFesManager();
  pBattleFesManager->Delete();

  Field::FieldScript::FieldScriptSystem*        pScriptSystem = pGameManager->GetFieldScriptSystem();
  pScriptSystem->Suspend();

  return NULL;
}

//-----------------------------------------------------------------------------
/**
* @func        WaitModelLoad
* @brief       モデル読み込み待ち
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL WaitModelLoad(AMX *amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  BattleFes::BattleFes* pBattleFes = pGameManager->GetGameData()->GetBattleFesManager()->GetBattleFes();
  return pBattleFes->WaitModelLoad();
}

//-----------------------------------------------------------------------------
/**
* @func        ClearModelData
* @brief       モデル情報削除
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL ClearModelData(AMX *amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  BattleFes::BattleFes* pBattleFes = pGameManager->GetGameData()->GetBattleFesManager()->GetBattleFes();
  pBattleFes->ClearModelData(Field::BATTLE_FES_POS_INDEX_ENEMY);
  return NULL;
}

//-----------------------------------------------------------------------------
/**
* @func        GetCharacterID
* @brief       トレーナーのキャラクターＩＤ取得
* @param       第1引数　tr_count
* @return      キャラクターＩＤ
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetCharacterID(AMX *amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  BattleFes::BattleFes* pBattleFes = pGameManager->GetGameData()->GetBattleFesManager()->GetBattleFes();
  u16 tr_count = ptr[1];
  return pBattleFes->GetCharacterID(tr_count);
}

//-----------------------------------------------------------------------------
/**
* @func        AddTrainerObj
* @brief       トレーナーOBJ追加
* @param       第1引数　eventID
* @param       第2引数　chara_id
* @param       第3引数　x
* @param       第4引数　y
* @param       第5引数　z
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL AddTrainerObj(AMX *amx, const cell * ptr)
{
  u16 index = ptr[1];
  u16 objid = ptr[2];
  float x = amx_ctof(ptr[3]);
  float y = amx_ctof(ptr[4]);
  float z = amx_ctof(ptr[5]);
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  BattleFes::BattleFes* pBattleFes = pGameManager->GetGameData()->GetBattleFesManager()->GetBattleFes();
  pBattleFes->AddTrainerObj(static_cast<Field::BATTLE_FES_POS_INDEX>(index), objid, x, y, z);
  return NULL;
}

//-----------------------------------------------------------------------------
/**
* @func        DeleteObj
* @brief       トレーナーOBJ削除
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL DeleteObj(AMX *amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  BattleFes::BattleFes* pBattleFes = pGameManager->GetGameData()->GetBattleFesManager()->GetBattleFes();
  pBattleFes->DeleteObj();
  return NULL;
}

//-----------------------------------------------------------------------------
/**
* @func        WaitModelDelete
* @brief       トレーナーOBJ削除待ち
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL WaitModelDelete(AMX *amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  BattleFes::BattleFes* pBattleFes = pGameManager->GetGameData()->GetBattleFesManager()->GetBattleFes();
  return pBattleFes->WaitModelDelete();
}

//-----------------------------------------------------------------------------
/**
* @func        AddDressupTrainerObj
* @brief       着せ替えトレーナーOBJ追加
* @param       第1引数　eventID
* @param       第2引数　x
* @param       第3引数　y
* @param       第4引数　z
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL AddDressupTrainerObj(AMX *amx, const cell * ptr)
{
  u16 index = ptr[1];
  float x = amx_ctof(ptr[2]);
  float y = amx_ctof(ptr[3]);
  float z = amx_ctof(ptr[4]);
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  BattleFes::BattleFes* pBattleFes = pGameManager->GetGameData()->GetBattleFesManager()->GetBattleFes();
  pBattleFes->AddDressupTrainerObj(static_cast<Field::BATTLE_FES_POS_INDEX>(index), x, y, z);
  return NULL;
}

//------------------------------------------------------------------------------
/**
* @func        WordSetBattleFesFriendName
* @brief       バトルフェスの仲間名をレジスト
* @param       第1引数　仲間ＩＤ
* @param       第2引数　バッファＩＤ
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetBattleFesFriendName(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  print::WordSet*                     pWordSet = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();
  GameSys::GameData*                  pGameData = pGameManager->GetGameData();
  Field::EventWork*                   ev = pGameData->GetEventWork();
  bool                                isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave*            pBattleFesSave = pGameData->GetBattleFesSave(isEvent);

  u32 friendID = ptr[1]; // 仲間ＩＤ
  u32 bufID = ptr[2]; // バッファID

  if (pBattleFesSave->IsPartner(friendID))
  {
    gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);

    // データから名前と性別取ってくる
    gfl2::str::STRCODE name[System::STRLEN_PLAYER_NAME + System::EOM_LEN];
    pBattleFesSave->GetPartnerName(friendID, name);
    gfl2::str::StrBuf* pWordSetName = GFL_NEW(pHeap->GetLowerHandle()) gfl2::str::StrBuf(name, pHeap->GetLowerHandle());
    pWordSet->RegisterWord(bufID, *pWordSetName, pWordSet->GrammerFromSex(pBattleFesSave->GetPartnerSex(friendID)));
    GFL_SAFE_DELETE(pWordSetName);
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
* @brief       仲間の挨拶定型文をレジスト
* @param       第1引数　バッファＩＤ
* @param       第2引数　仲間ＩＤ
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetPartnerHello(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*                         pGameManager = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap = pGameManager->GetFieldmap();
  print::WordSet*                               pWordSet = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();
  GameSys::GameData*                            pGameData = pGameManager->GetGameData();
  Field::EventWork*                             ev = pGameData->GetEventWork();
  bool                                          isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave*                      pBattleFesSave = pGameData->GetBattleFesSave(isEvent);

  u32 friendID = ptr[1]; // 仲間ＩＤ
  u32 bufID = ptr[2]; // バッファID

  gfl2::str::MsgData* pMsgData = pGameManager->GetGameData()->GetBattleFesManager()->GetMsgData();

  // 定型文のラベル求める
  u32 msgId = pBattleFesSave->GetPartnerHello(friendID);

  pWordSet->RegisterWord(bufID, pMsgData, msgId);

  return NULL;
}

//------------------------------------------------------------------------------
/**
* @func        GetLotteryMonsNo
* @brief       バトルフェスの抽選したポケモンのモンスター番号を取得
* @param       第1引数　ポケモンＩＤ（０～２）
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetLotteryMonsNo(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();

  u32 pokeID = ptr[1]; // 仲間ＩＤ
  BattleFes::BattleFesPokeLotterySystem* pBattleFesPokeLotterySystem = pGameManager->GetGameData()->GetBattleFesManager()->GetBattleFesPokeLotterySystem();
  GFL_ASSERT(pBattleFesPokeLotterySystem != NULL);
  pml::pokepara::PokemonParam* p_pokemonParam = pBattleFesPokeLotterySystem->GetPokemon(pokeID);
  return p_pokemonParam->GetMonsNo();
}

//------------------------------------------------------------------------------
/**
* @func        GetRentalMonsNo
* @brief       バトルフェスの自分のレンタルポケモンのモンスター番号を取得
* @return      モンスター番号
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetRentalMonsNo(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  GameSys::GameData*                  pGameData = pGameManager->GetGameData();
  Field::EventWork*                   ev = pGameData->GetEventWork();
  bool                                isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave*            pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
  gfl2::heap::HeapBase*               p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
  pml::pokepara::CoreParam*           p_coreParam = GFL_NEW(p_heap) pml::pokepara::CoreParam(p_heap, MONSNO_NULL, 0, 0);

  if (pBattleFesSave->GetRentalPokemonData(p_coreParam))
  {
    u16 monsno = p_coreParam->GetMonsNo();
    GFL_DELETE(p_coreParam);
    return monsno;
  }

  GFL_DELETE(p_coreParam);
  return MONSNO_NULL;
}

//------------------------------------------------------------------------------
/**
* @func        SaveRentalPokemon
* @brief       バトルフェスの自分のレンタルポケモンを保存
* @param       第1引数　ポケモンＩＤ（０～２）
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL SaveRentalPokemon(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  GameSys::GameData*                  pGameData = pGameManager->GetGameData();
  Field::EventWork*                   ev = pGameData->GetEventWork();
  bool                                isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave*            pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
  
  u32 pokeID = ptr[1]; // 仲間ＩＤ

  BattleFes::BattleFesPokeLotterySystem* pBattleFesPokeLotterySystem = pGameManager->GetGameData()->GetBattleFesManager()->GetBattleFesPokeLotterySystem();
  GFL_ASSERT(pBattleFesPokeLotterySystem != NULL);
  pml::pokepara::PokemonParam* p_pokemonParam = pBattleFesPokeLotterySystem->GetPokemon(pokeID);
  BattleFes::BATTLE_TREE_DATA battleTreeData = pBattleFesPokeLotterySystem->GetBattleTreeData(pokeID);
  // バトルで必要な項目を設定
  // 自分のデータ
  Savedata::MyStatus* pMyStatus = pGameManager->GetGameData()->GetPlayerStatus();

  // ID 親の名前と性別　ポケモンの親判定に使用されるため
  // IDがパーソナルデータにない＝自分のＩＤを設定する
  p_pokemonParam->SetID(pMyStatus->GetID());

  // データから名前と性別取ってくる
  gfl2::str::STRCODE name[System::STRLEN_PLAYER_NAME + System::EOM_LEN];
  pMyStatus->GetNameString(name);
  // 親の名前と性別登録
  p_pokemonParam->SetParentName(name);
  p_pokemonParam->SetParentSex(static_cast<pml::Sex>(pMyStatus->GetSex()));

  // 捕まえたボール設定
  p_pokemonParam->SetGetBall(item::BALLID_MONSUTAABOORU);
  // 選択ポケモン保存
  pBattleFesSave->SetRentalPokemonData(p_pokemonParam);
  // バトルツリーデータの保存
  pBattleFesSave->SetBattleTreeData(battleTreeData);
  return true;
}

//------------------------------------------------------------------------------
/**
* @func        IsRentalPokemon
* @brief       バトルフェスの借りているポケモンがいるか否か
* @param[out] bool
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL IsRentalPokemon(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  GameSys::GameData*                  pGameData = pGameManager->GetGameData();
  Field::EventWork*                   ev = pGameData->GetEventWork();
  bool                                isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave*            pBattleFesSave = pGameData->GetBattleFesSave(isEvent);

  return pBattleFesSave->IsExistRentalPokemonData();
}

//------------------------------------------------------------------------------
/**
* @func        CreatePokemonLotterySystem
* @brief       バトルフェスのポケモン抽選システム作成
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL CreatePokemonLotterySystem(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  gfl2::heap::HeapBase*               pHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
  pGameManager->GetGameData()->GetBattleFesManager()->CreateBattleFesPokeLotterySystem(pHeap);

  return NULL;
}

//------------------------------------------------------------------------------
/**
* @func        DeletePokemonLotterySystem
* @brief       バトルフェスのポケモン抽選システム破棄
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL DeletePokemonLotterySystem(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  pGameManager->GetGameData()->GetBattleFesManager()->DeleteBattleFesPokeLotterySystem();

  return NULL;
}

//------------------------------------------------------------------------------
/**
* @func        PokemonLottery
* @brief       バトルフェスのポケモン抽選
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL PokemonLottery(AMX * amx, const cell * ptr)
{
  bool isShop = GFL_BOOL_CAST(ptr[1]);
  Field::FieldScript::ScriptObject* pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  // 抽選ポケモン更新
  GameSys::GameData* pGameData = pScript->GetGameManager()->GetGameData();
  gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
  pGameData->GetBattleFesManager()->GetBattleFesPokeLotterySystem()->PokemonLottery(pGameData->GetSaveData()->GetRandomGroup()->GetTodayRandSeed(), pHeap, isShop);
  return NULL;
}

//------------------------------------------------------------------------------
/**
* @func        IsFullPartner
* @brief       バトルフェスの仲間が満員か空きがあるか
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL IsFullPartner(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  GameSys::GameData*                  pGameData = pGameManager->GetGameData();
  Field::EventWork*                   ev = pGameData->GetEventWork();
  bool                                isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave*            pBattleFesSave = pGameData->GetBattleFesSave(isEvent);

  return pBattleFesSave->IsFullPartner();
}

//------------------------------------------------------------------------------
/**
* @func        IsPartner
* @brief       バトルフェスの仲間がいるか否か
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL IsPartner(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  GameSys::GameData*                  pGameData = pGameManager->GetGameData();
  Field::EventWork*                   ev = pGameData->GetEventWork();
  bool                                isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave*            pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
  for (u32 index(0); index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX; ++index)
  {
    if (pBattleFesSave->IsPartner(index))
    {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
* @func        AddFesFunPartner
* @brief       バトルフェス　仲間を補充
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL AddFesFunPartner(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  GameSys::GameData*                  pGameData = pGameManager->GetGameData();
  Field::EventWork*                   ev = pGameData->GetEventWork();
  bool                                isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave*            pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
  s32 monsno = 0;
  gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);

  for (u32 index(0); index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX; ++index)
  {
    if (!pBattleFesSave->IsPartner(index))
    {
      const u32 LOOP_MAX = 100;
      u32 loop = 0;
      pml::pokepara::PokemonParam* pp = NULL;
      do{
        GFL_SAFE_DELETE(pp);
        pp = BattleFesTool::CreateRandomBattleTreePokemon(pHeap->GetLowerHandle(), NULL);
        ++loop;
      } while (monsno == pp->GetMonsNo() && loop < LOOP_MAX);

      monsno = pp->GetMonsNo();

      poke_3d::model::DressUpParam dressUpParam;
      u32 sex = System::GflUse::GetPublicRand(PM_NEUTRAL);
      BattleFesTool::GetDressUpParamRandom(sex, pHeap, &dressUpParam);
      pBattleFesSave->AddFesFunPartner(index, sex, pp, &dressUpParam);
      GFL_SAFE_DELETE(pp);
    }
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
* @func        IsBoss
* @brief       バトルフェスの対戦相手がボスか
* @return      ボス？否
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL IsBoss(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  GameSys::GameData*                  pGameData = pGameManager->GetGameData();
  Field::EventWork*                   ev = pGameData->GetEventWork();
  bool                                isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave*            pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
  u32 tr_count = ptr[1]; // トレーナー番号
  return pBattleFesSave->IsBoss(tr_count);
}

//------------------------------------------------------------------------------
/**
* @func        IsBossed
* @brief       バトルフェスの対戦相手がボスだったか
* @return      ボス？否
* @fix 【フェスサークル】バトルエージェントにてボスが1セット目から出てくる
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL IsBossed(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  GameSys::GameData*                  pGameData = pGameManager->GetGameData();
  Field::EventWork*                   ev = pGameData->GetEventWork();
  bool                                isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave*            pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
  u32 tr_count = BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX - 1; // 最後のトレーナー番号
  u32 tr_id = pBattleFesSave->GetTrainerData(tr_count).trID;
  return (tr_id == BattleFes::BattleFesDefine::TR_ID_MAMANE || tr_id == BattleFes::BattleFesDefine::TR_ID_SAKAKI);
}

//------------------------------------------------------------------------------
/**
* @func        IsRankUp
* @brief       バトルフェスのランクアップするか否か
* @return      する・しない
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL IsRankUp(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  GameSys::GameData*                  pGameData = pGameManager->GetGameData();
  Field::EventWork*                   ev = pGameData->GetEventWork();
  bool                                isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave*            pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
  return (pBattleFesSave->GetNowBattleSetCount() >= pBattleFesSave->GetRankUpBattleSetCount());
}

//------------------------------------------------------------------------------
/**
* @func        IsFirstBattle
* @brief       バトルフェスの今のランクで初戦か否か
* @return      yes/no
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL IsFirstBattle(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  GameSys::GameData*                  pGameData = pGameManager->GetGameData();
  Field::EventWork*                   ev = pGameData->GetEventWork();
  bool                                isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave*            pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
  return (pBattleFesSave->GetNowBattleSetCount() == 0 && pBattleFesSave->GetWinCount() == 0);
}

//------------------------------------------------------------------------------
/**
* @func        WordSetBattleFesTrainerName
* @brief       バトルフェスのトレーナ名設定
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetBattleFesTrainerName(AMX * amx, const cell * ptr)
{
  u16 buf_id = ptr[1];
  u16 tr_id = ptr[2];
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  BattleFes::BattleFes* pBattleFes = pGameManager->GetGameData()->GetBattleFesManager()->GetBattleFes();
  pBattleFes->WordSetTrainerNameFromTrainerNo(buf_id, tr_id);
  return NULL;
}

//------------------------------------------------------------------------------
/**
* @func        WordSetBattleFesTrainerTypeName
* @brief       バトルフェスのトレーナタイプ設定
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL WordSetBattleFesTrainerTypeName(AMX * amx, const cell * ptr)
{
  u16 buf_id = ptr[1];
  u16 tr_id = ptr[2];
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  BattleFes::BattleFes* pBattleFes = pGameManager->GetGameData()->GetBattleFesManager()->GetBattleFes();
  pBattleFes->WordSetTrainerTypeNameFromTrainerNo(buf_id, tr_id);
  return NULL;
}

//------------------------------------------------------------------------------
/**
* @func        GetWinCount
* @brief       バトルフェスの連勝数取得
* @return      連勝数
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetWinCount(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  GameSys::GameData*                  pGameData = pGameManager->GetGameData();
  Field::EventWork*                   ev = pGameData->GetEventWork();
  bool                                isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave*            pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
  return pBattleFesSave->GetWinCount();
}

//------------------------------------------------------------------------------
/**
* @func        ClearWinCount
* @brief       バトルフェスの連勝数クリア
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL ClearWinCount(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  GameSys::GameData*                  pGameData = pGameManager->GetGameData();
  Field::EventWork*                   ev = pGameData->GetEventWork();
  bool                                isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave*            pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
  pBattleFesSave->ClearWinCount();
  return NULL;
}

//------------------------------------------------------------------------------
/**
* @func        AddWinCount
* @brief       バトルフェスの連勝数増加
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL AddWinCount(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  GameSys::GameData*                  pGameData = pGameManager->GetGameData();
  Field::EventWork*                   ev = pGameData->GetEventWork();
  bool                                isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave*            pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
  pBattleFesSave->AddWinCount();
  return NULL;
}

//------------------------------------------------------------------------------
/**
* @func        BattleFesClearProcess
* @brief       バトルフェスのクリア後処理
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL BattleFesClearProcess(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  GameSys::GameData*                  pGameData = pGameManager->GetGameData();
  Field::EventWork*                   ev = pGameData->GetEventWork();
  bool                                isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave*            pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
  gfl2::heap::HeapBase*               pHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
  // バトルに必要なデータ削除
  pBattleFesSave->ClearBattleData(pHeap);
  // 連勝数０
  pBattleFesSave->ZeroClearWinCount();
  return true;
}

//------------------------------------------------------------------------------
/**
* @func        BattleFesWinProcess
* @brief       バトルフェスの３連勝クリア後処理
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL BattleFesWinProcess(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  GameSys::GameData*                  pGameData = pGameManager->GetGameData();
  Field::EventWork*                   ev = pGameData->GetEventWork();
  bool                                isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave*            pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
  // トレーナーデータ削除
  pBattleFesSave->ClearTrainerData();
  return true;
}

//------------------------------------------------------------------------------
/**
* @func        RankUp
* @brief       バトルフェスのランクアップ
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL RankUp(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  GameSys::GameData*                  pGameData = pGameManager->GetGameData();
  Field::EventWork*                   ev = pGameData->GetEventWork();
  bool                                isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave*            pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
  pBattleFesSave->AddRank();
  // @fix MMCat[289] 【フェスサークル】バトルエージェントのグレードアップ後、続けて再挑戦すると、レンタルしなおしたポケモンがグレードアップ前のレベルとなっている
  gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
  pGameData->GetBattleFesManager()->GetBattleFesPokeLotterySystem()->UpdatePokemonData(pHeap);
  return NULL;
}

//------------------------------------------------------------------------------
/**
* @func        GetBattleFesRank
* @brief       バトルフェスのランク取得
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetBattleFesRank(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  GameSys::GameData*                  pGameData = pGameManager->GetGameData();
  Field::EventWork*                   ev = pGameData->GetEventWork();
  bool                                isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave*            pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
  return pBattleFesSave->GetRank();
}

//------------------------------------------------------------------------------
/**
* @func        GetRankUpBattleSetCount
* @brief       バトルフェスのランクアップまでの挑戦数取得
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetRankUpBattleSetCount(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  GameSys::GameData*                  pGameData = pGameManager->GetGameData();
  Field::EventWork*                   ev = pGameData->GetEventWork();
  bool                                isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave*            pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
  return (pBattleFesSave->GetRankUpBattleSetCount() - pBattleFesSave->GetNowBattleSetCount());
}

//------------------------------------------------------------------------------
/**
* @func        CountProcess
* @brief       バトルフェスのバトル後カウント処理
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL CountProcess(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  GameSys::GameData*                  pGameData = pGameManager->GetGameData();
  Field::EventWork*                   ev = pGameData->GetEventWork();
  bool                                isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
  Savedata::BattleFesSave*            pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
  // カウント更新
  pBattleFesSave->UpdateTotalRandCount();
  // お店利用フラグＯＦＦ
  pBattleFesSave->ClearUseShop();
  return NULL;
}

//-----------------------------------------------------------------------------
/**
* @func   StartBattle
*	@brief  戦闘開始処理
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL StartBattle(AMX *amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  BattleFes::BattleFes*               pBattleFes = pGameManager->GetGameData()->GetBattleFesManager()->GetBattleFes();
  pBattleFes->StartBattle();
  Field::FieldScript::FieldScriptSystem*        pScriptSystem = pGameManager->GetFieldScriptSystem();
  pScriptSystem->Suspend();
  return NULL;
}

//-----------------------------------------------------------------------------
/**
* @func   EndBattle
*	@brief  戦闘終了処理
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL EndBattle(AMX *amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  BattleFes::BattleFes*               pBattleFes = pGameManager->GetGameData()->GetBattleFesManager()->GetBattleFes();
  pBattleFes->EndBattle();
  return NULL;
}

//-----------------------------------------------------------------------------
/**
* @func   IsWin
*	@brief  戦闘結果
* @return 勝ち？負け
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL IsWin(AMX *amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*   pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*               pGameManager = pScript->GetGameManager();
  BattleFes::BattleFes*               pBattleFes = pGameManager->GetGameData()->GetBattleFesManager()->GetBattleFes();
  return pBattleFes->IsWin();
}

//-----------------------------------------------------------------------------
/**
* @func   UpdatePacketData
*	@brief  バトルフェスの通信データ更新
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL UpdatePacketData(AMX *amx, const cell * ptr)
{
#if defined( GF_PLATFORM_CTR )

  NetAppLib::JoinFesta::JoinFestaPacketManager* pJoinFestaPacketManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager);

  pJoinFestaPacketManager->SetPacketGameStatusWithMe();

  pJoinFestaPacketManager->SendMyData();

#endif // defined( GF_PLATFORM_CTR 
  return NULL;
}

//-----------------------------------------------------------------------------
/**
* @func        GetBattleFesValidPatherCount
* @brief       通信人物の中から、バトルフェスに誘える人数を取得する
* @return      誘う事が出来る人数
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetBattleFesValidPatherCount(AMX *amx, const cell * ptr)
{
  return BattleFesTool::GetBattleFesValidPatherCount();
}

//------------------------------------------------------------------------------
/**
* @brief       ワープエフェクト開始
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL StartWarpEffect(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork = pScript->GetWork();
  GameSys::GameManager*                         pGameManager = pScript->GetGameManager();
  Field::Fieldmap*                              pFieldmap = pGameManager->GetFieldmap();

  // 城外ギミックか城内ギミックか
  const bool isRoom = (pFieldmap->GetFieldGimmickID(Field::Fieldmap::GIMMICK_IDX_ZONE) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM) ? true : false;
  Field::FieldGimmick* pFieldGimmick = pFieldmap->GetFieldGimmick(Field::Fieldmap::GIMMICK_IDX_ZONE);
  Field::FieldGimmickJoinFesta*       pJfGimmick = isRoom ? NULL : static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = isRoom ? static_cast<Field::FieldGimmickJoinFestaRoom*>(pFieldGimmick) : NULL;

  NetApp::JoinFesta::JoinFestaEffectSystem*  pJoinFestaEffectSystem = isRoom ? pJfGimmickRoom->GetJoinFestaEffectSystem() : pJfGimmick->GetJoinFestaEffectSystem();

  const gfl2::math::Vector3                     waro_pos(amx_ctof(ptr[1]), amx_ctof(ptr[2]), amx_ctof(ptr[3]));
  pJoinFestaEffectSystem->SetEventHandler(NULL);
  pJoinFestaEffectSystem->Start_Warp(waro_pos);

  return NULL;
}

//------------------------------------------------------------------------------
/**
* @brief       バトルフェス今日のフィールド効果取得
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL GetBtlFieldType(AMX* amx, const cell * ptr)
{
  u16 rank = ptr[1];
  return static_cast<u32>(BattleFesTool::GetBtlFieldType(rank));
}

//------------------------------------------------------------------------------
/**
* @brief       バトルフェスモニターと今日の乱数シードに違いがあるか？取得
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL IsRandUpdate(AMX* amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject*             pScript = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  Field::FieldScript::Work*                     pScriptWork = pScript->GetWork();
  GameSys::GameManager*                         pGameManager = pScript->GetGameManager();
  return (pGameManager->GetGameData()->GetBattleFesManager()->GetMonitorRandSeed() != pGameManager->GetGameData()->GetSaveData()->GetRandomGroup()->GetTodayRandSeed());
}

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
cell AMX_NATIVE_CALL nJFUGCGuidelineMessageFlagOn_PhotoView_Talk(AMX * amx, const cell * ptr)
{
  Field::FieldScript::ScriptObject* pScript       = Field::FieldScript::ScriptObject::GetClass<Field::FieldScript::ScriptObject*>(amx);
  GameSys::GameManager*             pGameManager  = pScript->GetGameManager();
  Savedata::JoinFestaDataSave*      pSave         = pGameManager->GetGameData()->GetJoinFestaData();

  pSave->SetUGCGuidelineMessageFlag_PhotoView_FromScriptTalk(true);
  return 0;
}


/// 未実装中のものが warning に含まれるので
#define UNDER_CONSTRUCTION  PM_DEBUG

//class FieldScriptSystem;

//------------------------------------------------------------------------------
/**
 * @func        GetTable
 * @brief       命令テーブルセットを取得.
 * @author      N.Takeda
 * @date        2015/04/13(月) 20:57:50
 *
 * @return      const GFL_PAWN_FUNC_TABLE*  
 */
//------------------------------------------------------------------------------
const GFL_PAWN_FUNC_TABLE   *FuncSetJoinFesta::GetTable( void )
{
  static const GFL_PAWN_FUNC_TABLE s_FuncTable[] = {

    GFL_PAWN_FUNC( nJFInitializeGlobal )
    GFL_PAWN_FUNC( nJFFacilityCoordToWorld )
    GFL_PAWN_FUNC( nJFPersonalCoordToWorld )

    // WordSet系
    GFL_PAWN_FUNC( WordSetMyPattern )
    GFL_PAWN_FUNC( WordSetTargetPlayerName )
    GFL_PAWN_FUNC( WordSetTargetPattern )
    GFL_PAWN_FUNC( WordSetFacilitiesType )
    GFL_PAWN_FUNC( WordSetFacilitiesType2 )
    GFL_PAWN_FUNC( WordSetFacilitiesName )
    GFL_PAWN_FUNC( WordSetFacilitiesPlayerName )
    GFL_PAWN_FUNC( WordSetFacilitiesPattern )
    GFL_PAWN_FUNC( WordSetFacilitiesTrType )
    GFL_PAWN_FUNC( WordSetFesCircleName )
    GFL_PAWN_FUNC( WordSetAttractionName )
    GFL_PAWN_FUNC( WordSetQuestionAnswer )
    GFL_PAWN_FUNC( WordSetQuestionAnswer2 )
    GFL_PAWN_FUNC( WordSetQuestion )
    GFL_PAWN_FUNC( WordSetDyeColor )
    GFL_PAWN_FUNC( WordSetSelectWord )
    GFL_PAWN_FUNC( WordSetCommRoute )
    GFL_PAWN_FUNC( WordSetFortuneAction )
    GFL_PAWN_FUNC( WordSetKisekaeItemName )
    GFL_PAWN_FUNC( WordSetRecruiterName )
    GFL_PAWN_FUNC( WordSetLastP2pName )
    GFL_PAWN_FUNC( WordSetRecruiterPattern )
    GFL_PAWN_FUNC( WordSetRecordName2 )
    GFL_PAWN_FUNC( WordSetRecordCategoryName )
    GFL_PAWN_FUNC( WordSetRecordCategoryName2 )
    GFL_PAWN_FUNC( WordSetScriptSelectName )
    GFL_PAWN_FUNC( WordSetScriptSelectPattern )
    GFL_PAWN_FUNC( WordSetGameSyncIdCode )

    // ジョインフェスタセーブ系
    GFL_PAWN_FUNC( AddFesCoin )
    GFL_PAWN_FUNC( SubFesCoin )
    GFL_PAWN_FUNC( GetFesCoin )
    GFL_PAWN_FUNC( GetTotalFesCoin )
    GFL_PAWN_FUNC( SetFesRank )
    GFL_PAWN_FUNC( GetFesRank )
    GFL_PAWN_FUNC( CheckFesRankUp )
    GFL_PAWN_FUNC( SetFesReward )
    GFL_PAWN_FUNC( GetFesRewardState )
    GFL_PAWN_FUNC( FesRewardFinish )
    GFL_PAWN_FUNC( SetFacilitiesBuy )
    GFL_PAWN_FUNC( IsFacilitiesBuy )
    GFL_PAWN_FUNC( SetFesTheme )
    GFL_PAWN_FUNC( GetFesTheme )
    GFL_PAWN_FUNC( SetTempSaveFesTheme )
    GFL_PAWN_FUNC( GetTempSaveFesTheme )
    GFL_PAWN_FUNC( SetQuestionnaireAnswer )
    GFL_PAWN_FUNC( SetPokeParaSkipMessage )
    GFL_PAWN_FUNC( GetPokeParaSkipMessage )

    // パーソナル系
    GFL_PAWN_FUNC( PersonalTalkStart )
    GFL_PAWN_FUNC( PersonalTalkEnd )
    GFL_PAWN_FUNC( ChangeVipRelation )
    GFL_PAWN_FUNC( PersonalEventFinish )
    GFL_PAWN_FUNC( PersonalEventFinishTutorial )
    GFL_PAWN_FUNC( IsPersonalAttractionFinish )
    GFL_PAWN_FUNC( PersonalAttractionFinish )
    GFL_PAWN_FUNC( PokeDiaryTalk )
    GFL_PAWN_FUNC( ThatKisekaeNice )
    GFL_PAWN_FUNC( PersonalEventRecordIntroduction )
    GFL_PAWN_FUNC( PersonalEventRecordGuess )
    GFL_PAWN_FUNC( GetQuestionnaireIdForPersonalEvent )
    GFL_PAWN_FUNC( GetQuestionnaireAnswerForPersonalEvent )
    GFL_PAWN_FUNC( GetMyPersonalRecord )
    GFL_PAWN_FUNC( GetPersonalRecord )
    GFL_PAWN_FUNC( GetRecordCategoryNum )
    GFL_PAWN_FUNC( IsDiaryNetabare )

    // アトラクション系
    GFL_PAWN_FUNC( AttractionTimeCountDownStart )
    GFL_PAWN_FUNC( AttractionScoreUp )
    GFL_PAWN_FUNC( AttractionQuestionIsOk )
    GFL_PAWN_FUNC( AttractionKairikyRideIsOk )
    GFL_PAWN_FUNC( AttractionKentarosuRideSetRock )
    GFL_PAWN_FUNC( AttractionCalcAffinityAboud )
    GFL_PAWN_FUNC( AttractionIsErrorRetire )

    // 施設系
    GFL_PAWN_FUNC( FacilitiesTalkStart )
    GFL_PAWN_FUNC( FacilitiesTalkEnd )
    GFL_PAWN_FUNC( GetFacilitiesId )
    GFL_PAWN_FUNC( GetFacilitiesType )
    GFL_PAWN_FUNC( GetFacilitiesType2 )
    GFL_PAWN_FUNC( GetFacilitiesCharacterId )
    GFL_PAWN_FUNC( GetFacilitiesAssistantSex )
    GFL_PAWN_FUNC( GetFacilitiesShopName )
    GFL_PAWN_FUNC( GetFacilitiesDescription )
    GFL_PAWN_FUNC( GetFacilitiesRank )
    GFL_PAWN_FUNC( FacilitiesLottery )
    GFL_PAWN_FUNC( FacilitiesChange )
    GFL_PAWN_FUNC( FacilitiesChangeIndex )
    GFL_PAWN_FUNC( FacilitiesModelUpdate )
    GFL_PAWN_FUNC( FacilitiesFakeChangeStart )
    GFL_PAWN_FUNC( FacilitiesFakeChangeEnd )
    GFL_PAWN_FUNC( SetFacilitiesIntroducer )
    GFL_PAWN_FUNC( FortuneLottery )
    GFL_PAWN_FUNC( GetFortuneResultHitByAction )
    GFL_PAWN_FUNC( DrawLots )
    GFL_PAWN_FUNC( DrawLotsEnd )
    GFL_PAWN_FUNC( FunHouseLottery )
    GFL_PAWN_FUNC( GetRecommendShopPrice )
    GFL_PAWN_FUNC( GetTargetFacilitiesId )
    GFL_PAWN_FUNC( GetTradeRemainCount )
    GFL_PAWN_FUNC( SubTradeRemainCount )

    // エフェクト系
    GFL_PAWN_FUNC( StartRankupEffect )
    GFL_PAWN_FUNC( StopRankupEffect )
    GFL_PAWN_FUNC( StartNewShopEffect )
    GFL_PAWN_FUNC( StopNewShopEffect )
    GFL_PAWN_FUNC( StartAttractionEffect )

    // その他
    GFL_PAWN_FUNC( IsHaveColorDressup )
    GFL_PAWN_FUNC( ListMenuAddDyeColor )
    GFL_PAWN_FUNC( GetKisekaeShopId )
    GFL_PAWN_FUNC( RecruiterPersonalLoad )
    GFL_PAWN_FUNC( RecruiterPersonalUnload )
    GFL_PAWN_FUNC( TutorialPersonalLoad )
    GFL_PAWN_FUNC( TutorialSubScreenOpen )
    GFL_PAWN_FUNC( DynamickLoadDisable )
    GFL_PAWN_FUNC( DynamickLoadEnable )
    GFL_PAWN_FUNC( IsWLANSwitchEnable )

    // グローバルアトラクション系
    GFL_PAWN_FUNC( IsGAttractionParticipate )
    GFL_PAWN_FUNC( GetGAttractionResult )
    

    // アプリ呼び出し系
    GFL_PAWN_FUNC( CallJoinFestaShop_ )

    // バトルフェスサークル

    // バトルフェスマネージャーが必要な関数群
    // 初期化
    GFL_PAWN_FUNC( InitializeBattleFes )
    GFL_PAWN_FUNC( TerminateBattleFes )

    // モデル系
    GFL_PAWN_FUNC( WaitModelLoad )
    GFL_PAWN_FUNC( ClearModelData )
    GFL_PAWN_FUNC( GetCharacterID )
    GFL_PAWN_FUNC( AddTrainerObj )
    GFL_PAWN_FUNC( DeleteObj )
    GFL_PAWN_FUNC( WaitModelDelete )
    GFL_PAWN_FUNC( AddDressupTrainerObj )

    // WordSet系
    GFL_PAWN_FUNC( WordSetBattleFesFriendName )
    GFL_PAWN_FUNC( WordSetPartnerHello )

    // ポケモン系
    GFL_PAWN_FUNC( GetLotteryMonsNo )
    GFL_PAWN_FUNC( GetRentalMonsNo )
    GFL_PAWN_FUNC( SaveRentalPokemon )
    GFL_PAWN_FUNC( IsRentalPokemon )
    GFL_PAWN_FUNC( CreatePokemonLotterySystem )
    GFL_PAWN_FUNC( DeletePokemonLotterySystem )
    GFL_PAWN_FUNC( PokemonLottery )

    // 仲間系
    GFL_PAWN_FUNC( IsFullPartner )
    GFL_PAWN_FUNC( IsPartner )
    GFL_PAWN_FUNC( AddFesFunPartner )

    // トレーナー系
    GFL_PAWN_FUNC( IsBoss )
    GFL_PAWN_FUNC( IsBossed )
    GFL_PAWN_FUNC( IsRankUp )
    GFL_PAWN_FUNC( IsFirstBattle )
    GFL_PAWN_FUNC( WordSetBattleFesTrainerName )
    GFL_PAWN_FUNC( WordSetBattleFesTrainerTypeName )

    // セーブデータのみアクセスする関数群
    // バトルフェス記録系
    GFL_PAWN_FUNC( GetWinCount )
    GFL_PAWN_FUNC( ClearWinCount )
    GFL_PAWN_FUNC( AddWinCount )
    GFL_PAWN_FUNC( BattleFesClearProcess )
    GFL_PAWN_FUNC( BattleFesWinProcess )
    GFL_PAWN_FUNC( RankUp )
    GFL_PAWN_FUNC( GetBattleFesRank )
    GFL_PAWN_FUNC( GetRankUpBattleSetCount )
    GFL_PAWN_FUNC( CountProcess )

    // バトル系
    GFL_PAWN_FUNC( StartBattle )
    GFL_PAWN_FUNC( EndBattle )
    GFL_PAWN_FUNC( IsWin )

    // 通信
    GFL_PAWN_FUNC( UpdatePacketData )
    GFL_PAWN_FUNC( GetBattleFesValidPatherCount )

    // エフェクト
    GFL_PAWN_FUNC( StartWarpEffect )

    GFL_PAWN_FUNC( GetBtlFieldType )
    GFL_PAWN_FUNC( IsRandUpdate )

    GFL_PAWN_FUNC( nJFUGCGuidelineMessageFlagOn_PhotoView_Talk )

    GFL_PAWN_FUNC_END,
  };
  
  return s_FuncTable;
}



//------------------------------------------------------------------------------
/**
 * @func        CallJoinFestaShop_
 * @brief       フェスコイン使用のショップ呼び出し.
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetJoinFesta::CallJoinFestaShop_(AMX * amx, const cell * ptr)
{
  ScriptObject*                                   pScript           = ScriptObject::GetClass<ScriptObject*>( amx );
  FieldScript::Work*                              pScriptWork       = pScript->GetWork();
  GameSys::GameManager*                           pGameManager      = pScript->GetGameManager();
  FieldScriptSystem*                              p_sys             = pGameManager->GetFieldScriptSystem();
  Field::FieldGimmickJoinFesta*                   pJfGimmick        = static_cast<Field::FieldGimmickJoinFesta*>(pGameManager->GetFieldmap()->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE ));
  NetApp::JoinFesta::JoinFestaFacilitiesSystem*   pJfFacilitiesSystem  = pJfGimmick->GetJoinFestaFacilitiesSystem();
  Savedata::JoinFestaDataSave*                    pSave             = pGameManager->GetGameData()->GetJoinFestaData();


  // 話しかけた施設のIndexを保存しておく
  {
    // 念の為先に無効値設定
    pSave->SetFacilitiesIndexOfScriptCall( JoinFestaScript::FACILITIES_INDEX_MAX );

    // 動作モデルのイベントID
    u32 eventId = pScriptWork->GetTempWorkValue( SCWK_TARGET_OBJID );
    if( eventId >= NetApp::JoinFesta::JoinFestaDefine::ASSISTANT_EVENT_ID_OFFSET )
    {
      JoinFestaScript::FacilitiesIndex facilitiesIndex = static_cast<JoinFestaScript::FacilitiesIndex>( eventId - NetApp::JoinFesta::JoinFestaDefine::ASSISTANT_EVENT_ID_OFFSET);
      GFL_PRINT(">施設インデックス %d \n", facilitiesIndex);

      pSave->SetFacilitiesIndexOfScriptCall( facilitiesIndex );
    }
  }

  
  bool isItem = ptr[1];           // アイテムショップか
  u16 shop_id = ptr[2];           // ショップID
  u16 howmany_msg = ptr[3];       // メッセージ：いくつ買う？ッセージ：いくつ買う
  u16 really_msg = ptr[4];        // メッセージ：xxコでxx円です
  u16 really_msg2 = ptr[5];       // メッセージ：xxコでxx円です（複数形）
  u16 hereitis_msg = ptr[6];      // メッセージ：まいど
  u16 youcanttake_msg = ptr[7];   // メッセージ：持ちきれない
  u16 nomoney_msg = ptr[8];       // メッセージ：お金が足りない
  u16 selected_msg = ptr[9];      // メッセージ：今日は終了しました（フェスショップ用）

  // ショップタイプ
  App::Shop::ShopType shop_type = ( isItem == true ? App::Shop::TYPE_FC_ITEM : App::Shop::TYPE_FC_ACTION );

  App::Event::ShopBuyEvent * ev = App::Event::ShopBuyEvent::StartEvent( pGameManager );
  ev->SetupHeap( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE) );
  ev->SetupParamForFC(
    shop_type,          // ショップタイプ
    shop_id,            // ショップID
    howmany_msg,        // howmany_msg       メッセージ：いくつ買う？
    really_msg,         // really_msg        メッセージ：xxコでxx円です
    really_msg2,        // really_msg2       メッセージ：xxコでxx円です（複数形）
    hereitis_msg,       // hereitis_msg      メッセージ：まいど
    youcanttake_msg,    // youcanttake_msg   メッセージ：持ちきれない
    nomoney_msg,        // nomoney_msg       メッセージ：お金が足りない
    selected_msg,       // selected_msg      メッセージ：今日は終了しました（フェスショップ用）
    pJfFacilitiesSystem
  );
  
  p_sys->Suspend();

  return 0;
}




GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )

