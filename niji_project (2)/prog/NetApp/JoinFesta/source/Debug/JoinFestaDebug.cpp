//==============================================================================
/**
 * @file	JoinFestaDebug.cpp
 * @brief	ジョインフェスタデバッグ機能
 * @author	ichiraku_katsuhiko
 * @date	2016/03/07 15:59:28
 */
// =============================================================================
#include "NetApp/JoinFesta/include/Debug/JoinFestaDebug.h"

#include <base/include/gfl2_Singleton.h>
#include <debug/include/gfl2_DebugPrint.h>

#include "GameSys/include/GameManager.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "niji_conv_header/field/FieldGimmickTypes.h"


#include "Field/FieldRos/FieldGimmickJoinFesta/include/FieldGimmickJoinFesta.h"
#include "Field/FieldRos/FieldGimmickJoinFestaRoom/include/FieldGimmickJoinFestaRoom.h"

// savedata
#include "Savedata/include/MyStatus.h"
#include "Savedata/include/EventWork.h"
#include "Savedata/include/JoinFestaVipSave.h"
#include "niji_conv_header/field/flagwork/WorkDefine.h"

// JFモジュール
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFDynamicLoadSystem.h"
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFModelManager.h"
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFPointManager.h"
#include "NetApp/JoinFesta/include/JoinFestaResourceManager/JoinFestaResourceManager.h"
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesSystem.h"
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesData.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalEvent/JoinFestaPersonalEvent.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataUtil.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaFacilitiesUtil/JoinFestaFacilitiesUtil.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaSelectWordTable.h"
// スクリプト
#include "niji_reference_files/script/FieldPawnTypes.h"

#include <Debug/DebugWin/include/DebugWinSystem.h>
// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"

// ジョインフェスタ施設テーブル定義
#include "NetApp/JoinFesta/include/JoinFestaDataTable/JoinFestaDataTable.h"

// なりすましイベント
#include "NetStatic/NetEvent/include/Debug/JoinFestaDummySendEvent.h"

#ifdef JF_DEBUG_MENU

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)




//==============================================================================
/**
*  @class  
*  @brief  
*  @date   2015/11/18 Wed. 20:05:04
*/
//==============================================================================
JoinFestaDebug::JoinFestaDebug( JFDynamicLoadSystem* pJFDynamicLoadSystem, const JoinFestaDataTable* pDataTable, JoinFestaResourceManager* pResourceManager, JoinFestaFacilitiesSystem* pFacilitiesSystem )
: m_pJFDynamicLoadSystem(pJFDynamicLoadSystem)
, m_pDataTable(pDataTable)
, m_pResourceManager(pResourceManager)
, m_pFacilitiesSystem(pFacilitiesSystem)
, m_debugJfPointIndex(0)
, m_fesRank(0)
, m_attractionScore(0)
, m_debugDeviceManager(NULL)
, m_pDebugButton(NULL)
, m_debugRewardGroup(NULL)
, m_debugAttractionGroup(NULL)
, m_debugPersonalGroup(NULL)
, m_debugPersonalEventGroup(NULL)
, m_debugGlobalAttractionGroup(NULL)
, m_debugStatisticsGroup(NULL)
, m_debugSaveGroup(NULL)
, m_debugUpperGroup(NULL)
, m_eventId(NULL)
{
  GameSys::GameManager*             pGameManager      = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

  m_pFieldmap = pGameManager->GetFieldmap();
  const bool                        isRoom            = ( m_pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;
  ::Debug::DebugWin::DebugWinWork*  devWork           = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  gfl2::str::MsgData*               pMsgData          = devWork->GetMessageData();
  gfl2::debug::DebugWinGroup*       pJFDebugGroupTop  = NULL;

  Savedata::JoinFestaDataSave*  pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();

  gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG);

  m_fesRank = pSave->GetRank();


  // 城外ギミックか城内ギミックか
  Field::FieldGimmickJoinFesta*       pJfGimmick = NULL;
  Field::FieldGimmickJoinFestaRoom*   pJfGimmickRoom = NULL;

  if( isRoom )
  {
    pJfGimmickRoom = static_cast<Field::FieldGimmickJoinFestaRoom*>(m_pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE ));
    pJFDebugGroupTop  = pJfGimmickRoom->GetDebugGroup();
  }
  else
  {
    pJfGimmick = static_cast<Field::FieldGimmickJoinFesta*>(m_pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE ));
    pJFDebugGroupTop  = pJfGimmick->GetDebugGroup();
  }

  /*  deviceManager  */
  m_debugDeviceManager  = pGameManager->GetUiDeviceManager();
  m_pDebugButton        = m_debugDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  /*  JoinFesta直下  */
  {
    // モデル表示位置設定
    //gfl2::debug::DebugWin_AddItemUpdateFunc( pJFDebugGroupTop, pHeap, pMsgData, msg_dmenu_joinfes_point,      this, DebugFunc_ChangeJFPointPosUpdate, DebugFunc_ChangeJFPointPosDraw );  // "JFPoint"
    // モデル表示位置ダンプ
    //gfl2::debug::DebugWin_AddItemUpdateFunc( pJFDebugGroupTop, pHeap, pMsgData, msg_dmenu_joinfes_point_dump, this, DebugFunc_JFPointPosDump ); // "JFPointDump"

    // 「所持フェスコイン」
    gfl2::debug::DebugWin_AddItemUpdateFunc( pJFDebugGroupTop, pHeap, pMsgData, msg_dmenu_jf_coin, this, DebugFunc_JFUpdateFesCoin, DebugFunc_JFDrawFesCoin );
    // 「累計フェスコイン」
    gfl2::debug::DebugWin_AddItemUpdateFunc( pJFDebugGroupTop, pHeap, pMsgData, msg_dmenu_jf_total_coin, this, DebugFunc_JFUpdateTotalFesCoin, DebugFunc_JFDrawTotalFesCoin );
    // 「ランク」
    gfl2::debug::DebugWin_AddItemValueU32( pJFDebugGroupTop, pHeap, pMsgData, msg_dmenu_jf_rank, &m_fesRank, 0, 999, 1);
    // 「ランク決定」
    gfl2::debug::DebugWin_AddItemUpdateFunc( pJFDebugGroupTop, pHeap, pMsgData, msg_dmenu_jf_rank_decide, this, DebugFunc_JFSetFesRank );
  }

  // グループ作成
  // "ごほうび"
  {
    m_debugRewardGroup  = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_jf_reward_top, pJFDebugGroupTop );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugRewardGroup, pHeap, pMsgData, msg_dmenu_jf_reward_item_09, this, DebugFunc_JFRewardPresentShopUpdate, DebugFunc_JFRewardPresentShopDraw );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugRewardGroup, pHeap, pMsgData, msg_dmenu_jf_reward_item_02, this, DebugFunc_JFRewardAttractionUpdate, DebugFunc_JFRewardAttractionDraw );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugRewardGroup, pHeap, pMsgData, msg_dmenu_jf_reward_item_00, this, DebugFunc_JFRewardCommandShopUpdate, DebugFunc_JFRewardCommandShopDraw );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugRewardGroup, pHeap, pMsgData, msg_dmenu_jf_reward_item_01, this, DebugFunc_JFRewardCommandDressupUpdate, DebugFunc_JFRewardCommandDressupDraw );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugRewardGroup, pHeap, pMsgData, msg_dmenu_jf_reward_item_03, this, DebugFunc_JFRewardNamingUpdate, DebugFunc_JFRewardNamingDraw );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugRewardGroup, pHeap, pMsgData, msg_dmenu_jf_reward_item_04, this, DebugFunc_JFRewardPreminumUpdate, DebugFunc_JFRewardPreminumDraw );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugRewardGroup, pHeap, pMsgData, msg_dmenu_jf_reward_item_05, this, DebugFunc_JFRewardBgmUpdate, DebugFunc_JFRewardBgmDraw );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugRewardGroup, pHeap, pMsgData, msg_dmenu_jf_reward_item_06, this, DebugFunc_JFRewardTheme1Update, DebugFunc_JFRewardTheme1Draw );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugRewardGroup, pHeap, pMsgData, msg_dmenu_jf_reward_item_07, this, DebugFunc_JFRewardTheme2Update, DebugFunc_JFRewardTheme2Draw );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugRewardGroup, pHeap, pMsgData, msg_dmenu_jf_reward_item_08, this, DebugFunc_JFRewardTheme3Update, DebugFunc_JFRewardTheme3Draw );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugRewardGroup, pHeap, pMsgData, msg_dmenu_jf_reward_item_10, this, DebugFunc_JFRewardSelectWordUpdate, DebugFunc_JFRewardSelectWordDraw );
  }

  // グループ作成
  // "アトラクション"
  m_debugAttractionGroup  = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_jf_attraction_top, pJFDebugGroupTop );
  {
    gfl2::debug::DebugWinItemValueU32  <u32>* itemU32   = NULL;
    // 「自分のスコア」
    gfl2::debug::DebugWin_AddItemValueU32( m_debugAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_attraction_00, &m_attractionScore, 0, 999, 1);
    // 「自分のスコア決定」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_attraction_01, this, DebugFunc_JFAttractionScoreSet );
    // 「全てのアトラクションが遊べるようになる」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_attraction_02, this, DebugFunc_JFAttractionAllPlay );
    // 「即時リザルト」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_attraction_03, this, DebugFunc_JFAttractionToResult );
    // 「制限時間停止」
    itemU32 = gfl2::debug::DebugWin_AddItemValueU32( m_debugAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_attraction_05, &m_attractionTimeStop, 0, 1, 1);
    itemU32->SetChangedValueFunc( DebugFunc_JFAttractionTimeStop, this );

    // 「なりすましパターン」
#if PM_DEBUG
    gfl2::debug::DebugWin_AddItemValueU32( m_debugAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_attraction_06, &NetEvent::JoinFestaDummySendEvent::m_patternMax, 0, 1000 );
    // 「なりすまし開始(Bでキャンセル）」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_attraction_07, this, DebugFunc_JFAttractionDummySend );
#endif
  }

  // グループ作成
  // "パーソナル系"
  m_debugPersonalGroup = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_jf_personal_top, pJFDebugGroupTop );
  {
    gfl2::debug::DebugWinItemValueU32  <u32>* itemU32   = NULL;
    // 「フィールド」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugPersonalGroup, pHeap, pMsgData, msg_dmenu_jf_personal_item_01, this, DebugFunc_UpdatePersonalField, DebugFunc_DrawPersonalField );
    // 「ゲスト」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugPersonalGroup, pHeap, pMsgData, msg_dmenu_jf_personal_item_02, this, DebugFunc_UpdatePersonalGuest, DebugFunc_DrawPersonalGuest );
    // 「VIP」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugPersonalGroup, pHeap, pMsgData, msg_dmenu_jf_personal_item_03, this, DebugFunc_UpdatePersonalVip, DebugFunc_DrawPersonalVip );
    // 「AllデータDump」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugPersonalGroup, pHeap, pMsgData, msg_dmenu_jf_personal_alldump, this, DebugFunc_JFPersonalAllDump );
    // 「パーソナルイベントリセット」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugPersonalGroup, pHeap, pMsgData, msg_dmenu_jf_event_reset, this, DebugFunc_JFPersonalEventReset );
    // 「ゲストをダミーで埋める」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugPersonalGroup, pHeap, pMsgData, msg_dmenu_jf_guest_dummy_full, this, DebugFunc_JFGuestDummyFull );
    // 「VIPをダミーで埋める」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugPersonalGroup, pHeap, pMsgData, msg_dmenu_jf_vip_dummy_full, this, DebugFunc_JFVipDummyFull );
    // 「お断りをダミーで埋める」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugPersonalGroup, pHeap, pMsgData, msg_dmenu_jf_black_dummy_full, this, DebugFunc_JFBlackListDummyFull );
    // 「着せ替えパターンID」
    gfl2::debug::DebugWin_AddItemValueU32( m_debugPersonalGroup, pHeap, pMsgData, msg_dmenu_jf_kisekaePatternId, &m_kisekaePatternId, 0, NetApp::JoinFesta::KisekaePatternInfoTable::KISEKA_PATTERN_ID_MAX-1 );
    // 「着せ替え出現」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugPersonalGroup, pHeap, pMsgData, msg_dmenu_jf_kisekaeModelAdd, this, DebugFunc_KisekaeModelAdd );
  }

  // グループ作成
  // "パーソナルイベント系"
  m_debugPersonalEventGroup = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_jf_personal_event_top, pJFDebugGroupTop );
  {
    // 「位置」
    gfl2::debug::DebugWin_AddItemValueInt( m_debugPersonalEventGroup, pHeap, pMsgData, msg_dmenu_jf_personal_event_item_00, &m_debugJfPointIndex, 0, 31, 1);
    // 「イベントID」
    gfl2::debug::DebugWin_AddItemValueU32( m_debugPersonalEventGroup, pHeap, pMsgData, msg_dmenu_jf_personal_event_item_01, &m_eventId, 0, JoinFestaScript::PERSONAL_EVENT_MAX, 1 );
    // 「設定」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugPersonalEventGroup, pHeap, pMsgData, msg_dmenu_jf_personal_event_item_02, this, DebugFunc_PersonalEventSet );
  }

  // グループ作成
  // "グローバルアトラクション系"
  m_debugGlobalAttractionGroup = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_jf_ga_top, pJFDebugGroupTop );
  {
    gfl2::debug::DebugWinItemValueU32  <u32>* itemU32   = NULL;
    // 「セーブデータID」
    itemU32 = gfl2::debug::DebugWin_AddItemValueU32( m_debugGlobalAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_ga_item_00, &m_gaSaveIndex, 0, 11, 1 );
    itemU32->SetChangedValueFunc( DebugFunc_GA_SaveIndexChange, this );

    NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData* pGAData = pSave->GetGlobalAttractionData(0);
    m_GAData = *pGAData;

    // 「グローバルアトラクションID」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGlobalAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_personal_event_item_02, &m_GAData.globalAttractionID, 0, 99 );
    // 「集計記録」
    gfl2::debug::DebugWin_AddItemValueU32( m_debugGlobalAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_ga_item_01, &m_GAData.totalRecord, 0, 0xFFFFFFFF );
    // 「開催状態」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGlobalAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_ga_item_02, &m_GAData.openState, 0, NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::OPEN_STATE_TOTAL_FINISH );
    // 「参加状態」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGlobalAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_ga_item_03, &m_GAData.participateState, 0, NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::PARTICIPATE_STATE_OK );
    // 「ローカル記録」
    gfl2::debug::DebugWin_AddItemValueU16( m_debugGlobalAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_ga_item_04, &m_GAData.localRecord, 0, NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::LOCAL_RECORD_MAX );
    // 「アップロード記録」
    gfl2::debug::DebugWin_AddItemValueU16( m_debugGlobalAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_ga_item_05, &m_GAData.uploadRecord, 0, NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::UPLOAD_RECORD_MAX );
    // 「PGL会員状態」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGlobalAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_ga_item_06, (u8*)&m_GAData.isPglMember, 0, 1 );
    // 「集計DL年」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGlobalAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_ga_item_07, &m_GAData.totalDlYear, 0, 99 );
    // 「集計DL月」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGlobalAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_ga_item_08, &m_GAData.totalDlMonth, 1, 12 );
    // 「集計DL日」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGlobalAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_ga_item_09, &m_GAData.totalDlDay, 1, 31 );
    // 「集計DL時」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGlobalAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_ga_item_10, &m_GAData.totalDlHour, 0, 23 );
    // 「集計DL分」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGlobalAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_ga_item_11, &m_GAData.totalDlMinitue, 0, 59 );
    // 「ご褒美授与状態」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGlobalAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_ga_item_12, &m_GAData.rewardState, 0, 1 );
    // 「ご褒美フェスコイン」
    gfl2::debug::DebugWin_AddItemValueU16( m_debugGlobalAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_ga_item_13, &m_GAData.rewardValue, 0, 0xFFFF );
    // 「ご褒美受取年」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGlobalAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_ga_item_14, &m_GAData.rewardYear, 0, 99 );
    // 「ご褒美受取月」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGlobalAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_ga_item_15, &m_GAData.rewardMonth, 1, 12 );
    // 「ご褒美受取日」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGlobalAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_ga_item_16, &m_GAData.rewardDay, 1, 31 );
    // 「ご褒美受取時」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGlobalAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_ga_item_17, &m_GAData.rewardHour, 0, 23 );
    // 「ご褒美受取分」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGlobalAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_ga_item_18, &m_GAData.rewardMinitue, 0, 59 );

    // 「設定」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugGlobalAttractionGroup, pHeap, pMsgData, msg_dmenu_jf_personal_event_item_02, this, DebugFunc_GA_DataSet );
  }

  // グループ作成
  // "統計"
  m_debugStatisticsGroup = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_jf_statistics_top, pJFDebugGroupTop );
  {
    // 「試行回数」
    m_statisticsCount = 1000;
    gfl2::debug::DebugWin_AddItemValueU32( m_debugStatisticsGroup, pHeap, pMsgData, msg_dmenu_jf_statistics_00, &m_statisticsCount, 1, 0xFFFFFFFF );
    // 「ホラーハウス」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugStatisticsGroup, pHeap, pMsgData, msg_dmenu_jf_statistics_01, this, DebugFunc_Statistics_HorrorHouse );
    // 「くじや」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugStatisticsGroup, pHeap, pMsgData, msg_dmenu_jf_statistics_02, this, DebugFunc_Statistics_Lots );
    // 「うらないや」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugStatisticsGroup, pHeap, pMsgData, msg_dmenu_jf_statistics_03, this, DebugFunc_Statistics_Fortune );
    // 「施設抽選」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugStatisticsGroup, pHeap, pMsgData, msg_dmenu_jf_statistics_04, this, DebugFunc_Statistics_FacilitiesLottery );
    // 「パーソナルイベント抽選」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugStatisticsGroup, pHeap, pMsgData, msg_dmenu_jf_statistics_05, this, DebugFunc_Statistics_PersonalEvent );
  }

  // グループ作成
  // "セーブ系"
  m_debugSaveGroup = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_jf_save_top, pJFDebugGroupTop );
  {
    // 「定型文：あいさつ」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugSaveGroup, pHeap, pMsgData, msg_dmenu_jf_save_item_00, this, DebugFunc_PatternHelloUpdate, DebugFunc_PatternHelloDraw );
    // 「定型文：別れ」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugSaveGroup, pHeap, pMsgData, msg_dmenu_jf_save_item_01, this, DebugFunc_PatternPartingUpdate, DebugFunc_PatternPartingDraw );
    // 「定型文：感動」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugSaveGroup, pHeap, pMsgData, msg_dmenu_jf_save_item_02, this, DebugFunc_PatternEmotionUpdate, DebugFunc_PatternEmotionDraw );
    // 「定型文：残念」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugSaveGroup, pHeap, pMsgData, msg_dmenu_jf_save_item_03, this, DebugFunc_PatternRegretUpdate, DebugFunc_PatternRegretDraw );

    // 「占い結果：当たり」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugSaveGroup, pHeap, pMsgData, msg_dmenu_jf_save_item_21, this, DebugFunc_FortuneHitUpdate, DebugFunc_FortuneHitDraw );
    // 「占い結果：行動」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugSaveGroup, pHeap, pMsgData, msg_dmenu_jf_save_item_22, this, DebugFunc_FortuneActionUpdate, DebugFunc_FortuneActionDraw );
    // 「占い結果設定」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugSaveGroup, pHeap, pMsgData, msg_dmenu_jf_save_item_23, this, DebugFunc_FortuneSetupUpdate );

    gfl2::debug::DebugWinItemValueU32  <u32>* itemU32   = NULL;
    // 「アトラクションID」
    itemU32 = gfl2::debug::DebugWin_AddItemValueU32( m_debugSaveGroup, pHeap, pMsgData, msg_dmenu_jf_save_item_16, &m_attractionId, 0, JoinFestaScript::ATTRACTION_NUM-1 );
    itemU32->SetChangedValueFunc( DebugFunc_A_SaveIndexChange, this );

    NetAppLib::JoinFesta::JoinFestaAttractionDefine::SaveData* pAData = pSave->GetAttractionData( 0 );
    m_AttractionData = *pAData;

    // 「プレイ回数」
    gfl2::debug::DebugWin_AddItemValueU16( m_debugSaveGroup, pHeap, pMsgData, msg_dmenu_jf_save_item_17, &m_AttractionData.playCount, 0, 255 );
    // 「最高個人スコア」
    gfl2::debug::DebugWin_AddItemValueU16( m_debugSaveGroup, pHeap, pMsgData, msg_dmenu_jf_save_item_18, &m_AttractionData.manHighScore, 0, 0xFFFF );
    // 「最高トータルスコア」
    gfl2::debug::DebugWin_AddItemValueU32( m_debugSaveGroup, pHeap, pMsgData, msg_dmenu_jf_save_item_19, &m_AttractionData.totalHighScore, 0, 0xFFFFFFFF );
    // 「アトラクションセーブ設定」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugSaveGroup, pHeap, pMsgData, msg_dmenu_jf_save_item_24, this, DebugFunc_A_DataSet );

    // 「定型コメント全開放」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugSaveGroup, pHeap, pMsgData, msg_dmenu_jf_save_item_20, this, DebugFunc_OpenMessageAllOpen );

  }

  // グループ作成
  // "アッパー系"
  m_debugUpperGroup = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_jf_upper_top, pJFDebugGroupTop );
  {
    // 「未知のリクエストコマンド送信」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugUpperGroup, pHeap, pMsgData, msg_dmenu_jf_upper_item_00, this, DebugFunc_UpperCommandSend );
    // 「未知の共通パケット送信」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugUpperGroup, pHeap, pMsgData, msg_dmenu_jf_upper_item_01, this, DebugFunc_UpperUnionPacketSend );
    // 「未知のアトラクション送信」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugUpperGroup, pHeap, pMsgData, msg_dmenu_jf_upper_item_02, this, DebugFunc_UpperAttractionSend );
    // 「未知の定型文」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugUpperGroup, pHeap, pMsgData, msg_dmenu_jf_upper_item_03, this, DebugFunc_UpperPatternHelloSend );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugUpperGroup, pHeap, pMsgData, msg_dmenu_jf_upper_item_04, this, DebugFunc_UpperPatternPartingSend );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugUpperGroup, pHeap, pMsgData, msg_dmenu_jf_upper_item_05, this, DebugFunc_UpperPatternEmotionSend );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugUpperGroup, pHeap, pMsgData, msg_dmenu_jf_upper_item_06, this, DebugFunc_UpperPatternRegretSend );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugUpperGroup, pHeap, pMsgData, msg_dmenu_jf_upper_item_07, this, DebugFunc_UpperPatternSecretSend );
    // 「未知の一押し施設」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugUpperGroup, pHeap, pMsgData, msg_dmenu_jf_upper_item_08, this, DebugFunc_UpperSendRecommendFacilitiesId );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugUpperGroup, pHeap, pMsgData, msg_dmenu_jf_upper_item_09, this, DebugFunc_UpperSendRecommendFacilitiesColor );
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugUpperGroup, pHeap, pMsgData, msg_dmenu_jf_upper_item_10, this, DebugFunc_UpperSendRecommendFacilitiesCharacterId );
    // 「未知のフェスランク」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugUpperGroup, pHeap, pMsgData, msg_dmenu_jf_upper_item_11, this, DebugFunc_UpperSendFesRank );
    // 「未知のきせかえアイテム総数」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugUpperGroup, pHeap, pMsgData, msg_dmenu_jf_upper_item_12, this, DebugFunc_UpperSendHaveDressUpItemNum );
    // 「未知のアンケート回答」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugUpperGroup, pHeap, pMsgData, msg_dmenu_jf_upper_item_13, this, DebugFunc_UpperSendQuestionnaireAnswer );
    // 「未知の所持施設ID」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugUpperGroup, pHeap, pMsgData, msg_dmenu_jf_upper_item_14, this, DebugFunc_UpperSendAllFacilitiesId );
    // 「未知のダイアリーID」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugUpperGroup, pHeap, L"未知のダイアリーID", this, DebugFunc_UpperSendPokeDiaryId, DebugFunc_DrawSendPokeDiaryId );
    // 「不正なファインダースタジオデータ」
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_debugUpperGroup, pHeap, L"不正のFSデータ", this, DebugFunc_NgSendFinderStudio, DebugFunc_DrawNgSendFinderStudio );
  }
}

JoinFestaDebug::~JoinFestaDebug()
{
  // グループ破棄
  gfl2::debug::DebugWin_RemoveGroup( m_debugRewardGroup );
  gfl2::debug::DebugWin_RemoveGroup( m_debugAttractionGroup );
  gfl2::debug::DebugWin_RemoveGroup( m_debugPersonalGroup );
  gfl2::debug::DebugWin_RemoveGroup( m_debugPersonalEventGroup );
  gfl2::debug::DebugWin_RemoveGroup( m_debugGlobalAttractionGroup );
  gfl2::debug::DebugWin_RemoveGroup( m_debugStatisticsGroup );
  gfl2::debug::DebugWin_RemoveGroup( m_debugSaveGroup );
  gfl2::debug::DebugWin_RemoveGroup( m_debugUpperGroup );
}

// JFポイント設定
void JoinFestaDebug::DebugFunc_ChangeJFPointPosUpdate( void* userWork, gfl2::debug::DebugWinItem *item )
{
  JoinFestaDebug* pWork = (JoinFestaDebug*)userWork;
  gfl2::ui::Button* pButton = pWork->m_pDebugButton;

  // ポイント番号選択
  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT )  ){ pWork->m_debugJfPointIndex--; }
  if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) ){ pWork->m_debugJfPointIndex++; }
  pWork->m_debugJfPointIndex = gfl2::math::Clamp( pWork->m_debugJfPointIndex, 0, (int)JoinFestaDefine::JFPOINT_MAX-1 );

  // 指定したポイントの位置をプレイヤー座標に変更
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    const gfl2::math::Vector3 playerPosition = pWork->m_pFieldmap->GetPlayerPosition();
    JFPoint *jfPoint = pWork->m_pJFDynamicLoadSystem->GetJFPointManager()->GetJFPoint( pWork->m_debugJfPointIndex );
    if( jfPoint ){ jfPoint->SetPointPosition( playerPosition ); }

    item->PlaySystemSound( gfl2::debug::SE_ACCEPT );
  }
}

// モデル表示位置設定　描画
wchar_t* JoinFestaDebug::DebugFunc_ChangeJFPointPosDraw( void* userWork, gfl2::debug::DebugWinItem *item )
{
  JoinFestaDebug*  pWork   = (JoinFestaDebug*)userWork;
  wchar_t*              workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), pWork->m_debugJfPointIndex );
  return workStr;
}


// JFポイントDump
void JoinFestaDebug::DebugFunc_JFPointPosDump( void* userWork, gfl2::debug::DebugWinItem *item )
{
  JoinFestaDebug* pWork = (JoinFestaDebug*)userWork;
  if( pWork->m_pDebugButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pWork->m_pJFDynamicLoadSystem->GetJFPointManager()->Dump();
  }
}


// ご褒美系：共通更新
void JoinFestaDebug::DebugFunc_JFRewardCommonUpdate(void* userWork, gfl2::debug::DebugWinItem* pItem, JoinFestaScript::RewardType rewardType)
{
  Savedata::JoinFestaDataSave*  pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();
  int rewardState = (int)pSave->GetRewardState( rewardType );

  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);

  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_RIGHT))
  {
    if( rewardState < 2 ){
      rewardState++;
      pSave->SetRewardState(rewardType, (JoinFestaScript::RewardStateType)rewardState);
      pItem->PlaySystemSound( gfl2::debug::SE_CHANGE_VALUE );
    }
  }
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_LEFT))
  {
    if( rewardState > 0 ){
      rewardState--;
      pSave->SetRewardState(rewardType, (JoinFestaScript::RewardStateType)rewardState);
      pItem->PlaySystemSound( gfl2::debug::SE_CHANGE_VALUE );
    }
  }
}

  // ご褒美系：共通描画
wchar_t* JoinFestaDebug::DebugFunc_JFRewardCommonDraw( void* userWork, gfl2::debug::DebugWinItem *item, JoinFestaScript::RewardType rewardType )
{
  ::Debug::DebugWin::DebugWinWork*  devWork  = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  gfl2::str::MsgData*               pMsgData = devWork->GetMessageData();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  gfl2::str::StrBuf* workStrBuf = gfl2::debug::DebugWin_GetWorkStrBuf();
  int stateMsg[3] = { msg_dmenu_jf_reward_state_00, msg_dmenu_jf_reward_state_01, msg_dmenu_jf_reward_state_02 };
  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();
  // セーブのご褒美状態を表示する
  int rewardState = (int)pSave->GetRewardState( rewardType );
  pMsgData->GetString( stateMsg[rewardState], *workStrBuf );
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), workStrBuf->GetPtr() );
  return workStr;
}


/* ----------------------------------------------------------------------------------------- */
/**
* @brief   デバッグ用：自分のスコア設定
*/
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_JFAttractionScoreSet(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->DEBUG_SetScore( pThis->m_attractionScore );
    pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：全てのアトラクションが選べるようになる
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_JFAttractionAllPlay(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetEventWork();
    // ゲームクリアフラグを立てる
    pEventWork->SetEventFlag( SYS_FLAG_GAME_CLEAR );
    // アトラクション参加レコードを増やす
    Savedata::AddRecord( Savedata::Record::RECID_JF_ATT_CNT, 50 );
    pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：アトラクション即時リザルト
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_JFAttractionToResult(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->DEBUG_SetTimeLimit( 0 );
    pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：アトラクション制限時間停止
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_JFAttractionTimeStop(void* userWork)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);

  if( pThis->m_attractionTimeStop == 1 )
  {
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->DEBUG_SetTimeLimitEnable( false );
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->DEBUG_SetOtherTimeLimitEnable( false );
  }
  else
  {
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->DEBUG_SetTimeLimitEnable( true );
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->DEBUG_SetOtherTimeLimitEnable( true );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：アトラクションなりすまし開始
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_JFAttractionDummySend(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    // 起動フラグはON
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->DEBUG_SetIsDummySendEvent( true );

    pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
//! 更新:フェスコイン
void JoinFestaDebug::DebugFunc_JFUpdateFesCoin(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  Savedata::JoinFestaDataSave*  pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();

  int step = 1;

  if( pThis->m_pDebugButton->IsHold( gfl2::ui::BUTTON_R ) ) { step *= 10; }
  if( pThis->m_pDebugButton->IsHold( gfl2::ui::BUTTON_L ) ) { step *= 1000; }

  if(pThis->m_pDebugButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
  {
    pSave->AddCoin( step );
    pItem->PlaySystemSound( gfl2::debug::SE_CHANGE_VALUE );
  }
  if(pThis->m_pDebugButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
  {
    pSave->SubCoin( step );
    pItem->PlaySystemSound( gfl2::debug::SE_CHANGE_VALUE );
  }
}
//! 描画：フェスコイン
wchar_t* JoinFestaDebug::DebugFunc_JFDrawFesCoin( void* userWork, gfl2::debug::DebugWinItem *item )
{
  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), pSave->GetCoin() );
  return workStr;
}
//------------------------------------------------------------------
//------------------------------------------------------------------

//------------------------------------------------------------------
//------------------------------------------------------------------
//! 更新:累計フェスコイン
void JoinFestaDebug::DebugFunc_JFUpdateTotalFesCoin(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  Savedata::JoinFestaDataSave*  pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();

  int step = 1;
  u32 nowTotalCoin = pSave->GetTotalCoin();

  if( pThis->m_pDebugButton->IsHold( gfl2::ui::BUTTON_R ) ) { step *= 10; }
  if( pThis->m_pDebugButton->IsHold( gfl2::ui::BUTTON_L ) ) { step *= 100; }

  if(pThis->m_pDebugButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
  {
    pSave->SetTotalCoin( nowTotalCoin + step );
    pItem->PlaySystemSound( gfl2::debug::SE_CHANGE_VALUE );
  }
  if(pThis->m_pDebugButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
  {
    pSave->SetTotalCoin( nowTotalCoin - step );
    pItem->PlaySystemSound( gfl2::debug::SE_CHANGE_VALUE );
  }
}
//! 描画：累計フェスコイン
wchar_t* JoinFestaDebug::DebugFunc_JFDrawTotalFesCoin( void* userWork, gfl2::debug::DebugWinItem *item )
{
  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), pSave->GetTotalCoin() );
  return workStr;
}
//------------------------------------------------------------------
//------------------------------------------------------------------

//------------------------------------------------------------------
/**
 *  フェスランク決定
*/
//------------------------------------------------------------------
void JoinFestaDebug::DebugFunc_JFSetFesRank(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    Savedata::JoinFestaDataSave*  pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();
    GFL_PRINT("<%s>\n", __FUNCTION__);
    pSave->SetRank( pThis->m_fesRank );
    pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );

    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }
}

//------------------------------------------------------------------
/**
 *  パーソナルAllDump
 */
//------------------------------------------------------------------
void JoinFestaDebug::DebugFunc_JFPersonalAllDump(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList *list;

    // モードで、リストと探索Indexが違う
    for( int mode=0; mode<3; mode++ )
    {
      switch( mode ){
      case 0: list = manager->GetPersonalDataList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST ); break;
      case 1: list = manager->GetPersonalDataList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP ); break;
      default: list = manager->GetFieldPersonalList(); break;
      }

      if( list != NULL && list->Size() > 0 )
      {
        int cnt = 0;
        for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it )
        {
          if( (*it) != NULL )
          {
            (*it)->DEBUG_Dump();
          }
        }
      }
    }
  }
}

//------------------------------------------------------------------
/**
 *  パーソナルイベントリセット
 */
//------------------------------------------------------------------
void JoinFestaDebug::DebugFunc_JFPersonalEventReset(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    Savedata::JoinFestaDataSave*  pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();
    // 抽選時間リセット（発動フラグも中で初期化される）
    pSave->SetPersonalEventLotteryTimeNow();

    // パーソナルデータのパーソナルイベントもリセットする
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->ResetPersonalEvent();

    pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }
}

//------------------------------------------------------------------
/**
 *  パーソナルデータリストをダミーで埋める
 */
//------------------------------------------------------------------
void JoinFestaDebug::DebugFunc_JFDummyFull(void* userWork, gfl2::debug::DebugWinItem* pItem, NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION relation )
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    Savedata::MyStatus*                                 pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();   

    // 適当な人を追加
    const int addNum = ( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::LIST_MAX - manager->GetPersonalDataList(relation)->Size() );
    for( int i=0; i<addNum; i++)
    {
      u32 pid = NN_FRIENDS_INVALID_LOCAL_FRIEND_CODE;
      u64 tid = System::GflUse::GetPublicRand64();

      JoinFestaFriendKey friendKey( pid, tid );
      NetAppLib::JoinFesta::JoinFestaPacketData packet;
      manager->GetMyData()->OutputPacketData( &packet );
      // フレンドキーだけ上書き
      packet.personalInfo.myPrincipalId = pid;
      packet.personalInfo.transferdId = tid;

      if( relation == NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST ) {
        swprintf( packet.personalInfo.name, 13, L"D-G-%d", i );
      } else {
        swprintf( packet.personalInfo.name, 13, L"D-V-%d", i );
      }
      // 日本言語は全角にする
      if( pMyStatus->GetPokeLanguageId() == POKEMON_LANG_JAPAN )
      {
        for( u32 index = 0 ; index < System::STRLEN_PLAYER_NAME+System::EOM_LEN ; ++index )
        {
          if( packet.personalInfo.name[index] >= L'0' && packet.personalInfo.name[index] <= L'9' )
          {
            packet.personalInfo.name[index] = packet.personalInfo.name[index] - L'0' + L'０';
          }
          else if( packet.personalInfo.name[index] >= L'A' && packet.personalInfo.name[index] <= L'Z' )
          {
            packet.personalInfo.name[index] = packet.personalInfo.name[index] - L'A' + L'Ａ';
          }
          else if( packet.personalInfo.name[index] == L'-' )
          {
            packet.personalInfo.name[index] = L'－';
          }
        }
      }


#if defined(GF_PLATFORM_CTR)
      NetAppLib::JoinFesta::JoinFestaPacketManager* pJoinFestaPacketManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPacketManager );
      NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = pJoinFestaPacketManager->UpdateWithJoinFestaPacketData( &packet, true );
      //NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = manager->UpdatePersonalDataWithPacket( packet, friendKey, relation, true, 0 );

      // 全情報を出来るだけ入れる
      if( pPersonalData ) { pPersonalData->DEBUG_SetFullData(); }

      // VIPの場合は関係性更新
      if( relation == NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP )
      {
        manager->ChangePersonalRelation( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP, pPersonalData->GetJoinFestaFriendKey() );
      }
#else 
      NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = manager->UpdatePersonalDataWithPacket( packet, friendKey, relation, true, 0 );
      // 全情報を出来るだけ入れる
      if( pPersonalData ) { pPersonalData->DEBUG_SetFullData(); }
#endif // GF_PLATFORM_CTR
    }

    pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
    gfl2::debug::DebugWin_CloseWindow();
  }
}

//------------------------------------------------------------------
/**
 *  お断りをダミーで埋める
 */
//------------------------------------------------------------------
void JoinFestaDebug::DebugFunc_JFBlackListDummyFull(void* userWork, gfl2::debug::DebugWinItem* pItem )
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

    {
      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList *list = manager->GetPersonalDataList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP );

      if( list != NULL && list->Size() > 0 )
      {
        // ブラックリスト追加
        for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it )
        {
          NetAppLib::JoinFesta::JoinFestaPersonalData* targetPersonal = (*it);
          pSave->AddBlackList( *targetPersonal );
        }
        // セーブからも削除
        for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it )
        {
          NetAppLib::JoinFesta::JoinFestaPersonalData* targetPersonal = (*it);
          Savedata::JoinFestaVip::RemovePersonalFromJoinFestaVipSaveData( targetPersonal->GetJoinFestaFriendKey() ); // セーブデータから除外
        }
        // 追加した人全員削除
        manager->ReleaseList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP );
      }
    }

    pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
    gfl2::debug::DebugWin_CloseWindow();
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：共通Index更新
 * @param   mode 0:ゲスト 1:VIP 2:フィールド
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_UpdatePersonalCommon(void* userWork, gfl2::debug::DebugWinItem* pItem, u8 mode )
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);

  int* pValue;

  switch(mode){
  case 0: pValue = &pThis->m_guestIndex; break;
  case 1: pValue = &pThis->m_vipIndex; break;
  default:pValue = &pThis->m_fieldIndex; break;
  }

  const int VALUE_MAX = (mode==2) ? 31 : 49;

  int step = 1;
  if( pThis->m_pDebugButton->IsHold( gfl2::ui::BUTTON_R ) ) { step *= 10; }
  if( pThis->m_pDebugButton->IsHold( gfl2::ui::BUTTON_L ) ) { step *= 100; }

  if(pThis->m_pDebugButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
  {
    *pValue += step;
    if( *pValue > VALUE_MAX ) { *pValue = VALUE_MAX; }
    pItem->PlaySystemSound( gfl2::debug::SE_CHANGE_VALUE );
  }
  if(pThis->m_pDebugButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
  {
    *pValue -= step;
    if( *pValue < 0 ){ *pValue=0; }
    pItem->PlaySystemSound( gfl2::debug::SE_CHANGE_VALUE );
  }
  

  // AボタンでDump
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList *list;
    NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = NULL;

    // モードで、リストと探索Indexが違う
    switch( mode ){
    case 0:
      list = manager->GetPersonalDataList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );
      break;
    case 1:
      list = manager->GetPersonalDataList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP );
      break;
    default:
      list = manager->GetFieldPersonalList();
      break;
    }

    if( list != NULL && list->Size() > 0 )
    {
      int cnt = 0;
      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it )
      {
        // 探しているIndexと同じもの
        if( cnt++ == *pValue )
        {
          pPersonalData = (*it);
          break;
        }
      }
    }
    if( pPersonalData != NULL )
    {
      pPersonalData->DEBUG_Dump();
      pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
    }
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：共通Index描画
 * @param   mode 0:ゲスト 1:VIP 2:フィールド
 */
/* ----------------------------------------------------------------------------------------- */
wchar_t* JoinFestaDebug::DebugFunc_DrawPersonalCommon( void* userWork, gfl2::debug::DebugWinItem *item, u8 mode )
{
  JoinFestaDebug* pThis = static_cast<JoinFestaDebug*>(userWork);

  // 名前
  gfl2::str::STRCODE name[ ::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN ] = L"---";

  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList *list;


  // モードで、リストと探索Indexが違う
  int searchIndex;
  switch( mode ){
  case 0:
    {
      list = manager->GetPersonalDataList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );
      searchIndex = pThis->m_guestIndex;
    }
    break;
  case 1:
    {
      list = manager->GetPersonalDataList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP );
      searchIndex = pThis->m_vipIndex;
    }
    break;
  default:
    {
      list = manager->GetFieldPersonalList();
      searchIndex = pThis->m_fieldIndex;
    }
    break;
  }

  NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = NULL;
  if( list != NULL && list->Size() > 0 )
  {
    int cnt = 0;
    for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it )
    {
      // 探しているIndexと同じもの
      if( cnt++ == searchIndex )
      {
        pPersonalData = (*it);
        pPersonalData->GetName( name );
        break;
      }
    }
  }

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d:%ls]", item->GetName(), searchIndex, name );
  return workStr;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：着せ替えモデル出現
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_KisekaeModelAdd(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    JFPoint *jfPoint = pThis->m_pJFDynamicLoadSystem->GetJFPointManager()->GetJFPoint( 0 );

    bool isSuccess = false;
    if( jfPoint )
    {
      NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = jfPoint->GetPersonalData();

      // プリセットNPCでないと着せ替え変えない
      if( pPersonalData && pPersonalData->IsPresetNpcData() )
      {
        // 着せ替えを上書きする
        NetAppLib::JoinFesta::JoinFestaPersonalDataUtil::SetDressUpPattern( *pPersonalData, pThis->m_kisekaePatternId, pThis->m_pDataTable );

        // 削除リクエストだけ発行する
        if( pThis->m_pJFDynamicLoadSystem->DEBUG_UnloadRequest( 0 ) )
        {
          isSuccess = true;
        }
      }
    }

    if( isSuccess ) {
      pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
    } else {
      pItem->PlaySystemSound( gfl2::debug::SE_ERROR );
    }
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：パーソナルイベント設定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_PersonalEventSet(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    JFPoint *jfPoint = pThis->m_pJFDynamicLoadSystem->GetJFPointManager()->GetJFPoint( pThis->m_debugJfPointIndex );

    bool isSuccess = false;
    if( jfPoint )
    {
      JoinFestaScript::PersonalEventType personalEventType = static_cast<JoinFestaScript::PersonalEventType>(pThis->m_eventId);
      NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = jfPoint->GetPersonalData();

      if( pPersonalData )
      {
        if( NetAppLib::JoinFesta::JoinFestaPersonalEvent::IsCanEventFromPersonal( personalEventType, *pPersonalData ) )
        {
          // パーソナルイベントセーブの0スロット目を必ず発動出来るようにする
          Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();
          pSave->GetPersonalEventSlotData( 0 )->state = static_cast<u8>(NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::STATE_LOTTERY_OK);

          pPersonalData->SetPersonalEvent( 0, personalEventType );
          isSuccess = true;
        }
      }
    }

    if( isSuccess ) {
      pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
    } else {
      pItem->PlaySystemSound( gfl2::debug::SE_ERROR );
    }
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：グローバルアトラクションセーブデータID変更
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_GA_SaveIndexChange(void* userWork)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  Savedata::JoinFestaDataSave*  pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();

  NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData* pGAData = pSave->GetGlobalAttractionData( pThis->m_gaSaveIndex );
  pThis->m_GAData = *pGAData;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：グローバルアトラクションセーブデータ設定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_GA_DataSet(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    Savedata::JoinFestaDataSave*  pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();
    pSave->SetGlobalAttractionData( pThis->m_gaSaveIndex, &pThis->m_GAData );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：ホラーハウス統計
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_Statistics_HorrorHouse(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    Savedata::JoinFestaDataSave*  pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();

    GFL_PRINT("□□□□□□□□□□□□□□□□□□\n");
    GFL_PRINT("□□□□□□□□□□□□□□□□□□\n");
    GFL_PRINT("ホラーハウス統計\n");
    // 占い屋の結果
    GFL_PRINT("占いの補正→");
    switch( pSave->GetFortuneResultHitByAction( JoinFestaScript::FORTUNE_RESULT_ACTION_FUN_HOUSE ) ){
    case JoinFestaScript::FORTUNE_RESULT_HIT_SMALL: GFL_PRINT("当たり\n"); break;
    case JoinFestaScript::FORTUNE_RESULT_HIT_MIDDLE:GFL_PRINT("中当たり\n"); break;
    case JoinFestaScript::FORTUNE_RESULT_HIT_BIG:   GFL_PRINT("大当たり\n"); break;
    default: GFL_PRINT("なし\n"); break;
    }
    GFL_PRINT("□□□□□□□□□□□□□□□□□□\n");
    GFL_PRINT("□□□□□□□□□□□□□□□□□□\n");

    // 全施設回す
    for( u32 facilitiesId=0; facilitiesId<ShopItemInfoTable::SHOPITEM_MAX; facilitiesId++ )
    {
      const NetApp::JoinFesta::ShopItemInfo* pShop = pThis->m_pDataTable->GetShopItemInfo( facilitiesId );


      if( pShop->shopType == JoinFestaScript::FACILITIES_TYPE_FUN_HOUSE )
      {
        GFL_PRINT("================================================================\n");
        // 施設用のMsg
        // 施設名用のmstxt使う
        gfl2::str::StrBuf* workStrBuf = gfl2::debug::DebugWin_GetWorkStrBuf();
        gfl2::str::MsgData* pMsgData = pThis->m_pResourceManager->GetMsgData( NetApp::JoinFesta::JoinFestaResourceManager::RESOURCE_Message_JFShopName );
        pMsgData->GetString( pShop->messageID_ShopName, *workStrBuf );
        gfl2::str::PrintCode( workStrBuf->GetPtr(), "施設名：" );

        // 実際に建てる
        pSave->FacilitiesChange( JoinFestaScript::FACILITIES_INDEX_0, facilitiesId, 0, 0 );

        JoinFestaFacilitiesData* pData = pThis->m_pFacilitiesSystem->GetFacilitiesData( JoinFestaScript::FACILITIES_INDEX_0 ); 
        // 施設をデータのみ強制変更
        pData->DEBUG_ForceInitialize( JoinFestaScript::FACILITIES_INDEX_0, pThis->m_pDataTable );

        // 結果格納用
        u32 hitItemCount[10] = {0};
        u32 hit2ItemCount = 0;
        u32 hit3ItemCount = 0;
        u32 hitItemNum = 0;

        // 試行回数分回す
        for( u32 i=0; i<pThis->m_statisticsCount; i++ )
        {
          u32 hitItemId[3];
          pData->LotteryFunHouse( hitItemId );

          for( u32 j=0; j<3; j++ )
          {
            if( hitItemId[j] > 0 )
            {
              // 当たったスロットのカウントインクリメント
              const u32 slot = pData->GetLotInfoIndexByItemId( hitItemId[j] );
              hitItemCount[ slot ]++;
              hitItemNum++;
            }
          }
          // 3つアイテム当たったか
          if( hitItemId[1]>0 && hitItemId[2]>0 ) hit3ItemCount++;
          // 2つアイテム当たったか
          else if( hitItemId[1]>0  ) hit2ItemCount++;
        }

        // 結果
        GFL_PRINT("2個当たった回数:%d回\n", hit2ItemCount );
        GFL_PRINT("3個当たった回数:%d回\n", hit3ItemCount );
        GFL_PRINT("sum,%d,slot,", hitItemNum );
        for( u32 i=0; i<LotInfo::SLOT_MAX; i++ )
        {
          GFL_PRINT("%d,", hitItemCount[i] );
        }
        GFL_PRINT("\n");
      }
    }

    
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：くじや統計
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_Statistics_Lots(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    Savedata::JoinFestaDataSave*  pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();

    GFL_PRINT("□□□□□□□□□□□□□□□□□□\n");
    GFL_PRINT("□□□□□□□□□□□□□□□□□□\n");
    GFL_PRINT("くじや統計\n");
    // 占い屋の結果
    GFL_PRINT("占いの補正→");
    switch( pSave->GetFortuneResultHitByAction( JoinFestaScript::FORTUNE_RESULT_ACTION_LOTTERY ) ){
    case JoinFestaScript::FORTUNE_RESULT_HIT_SMALL: GFL_PRINT("当たり\n"); break;
    case JoinFestaScript::FORTUNE_RESULT_HIT_MIDDLE:GFL_PRINT("中当たり\n"); break;
    case JoinFestaScript::FORTUNE_RESULT_HIT_BIG:   GFL_PRINT("大当たり\n"); break;
    default: GFL_PRINT("なし\n"); break;
    }
    GFL_PRINT("□□□□□□□□□□□□□□□□□□\n");
    GFL_PRINT("□□□□□□□□□□□□□□□□□□\n");

    // 全施設回す
    for( u32 facilitiesId=0; facilitiesId<ShopItemInfoTable::SHOPITEM_MAX; facilitiesId++ )
    {
      const NetApp::JoinFesta::ShopItemInfo* pShop = pThis->m_pDataTable->GetShopItemInfo( facilitiesId );

      if( pShop->shopType == JoinFestaScript::FACILITIES_TYPE_LOTTERY )
      {
        GFL_PRINT("================================================================\n");
        // 施設用のMsg
        // 施設名用のmstxt使う
        gfl2::str::StrBuf* workStrBuf = gfl2::debug::DebugWin_GetWorkStrBuf();
        gfl2::str::MsgData* pMsgData = pThis->m_pResourceManager->GetMsgData( NetApp::JoinFesta::JoinFestaResourceManager::RESOURCE_Message_JFShopName );
        pMsgData->GetString( pShop->messageID_ShopName, *workStrBuf );
        gfl2::str::PrintCode( workStrBuf->GetPtr(), "施設名：" );

        // 実際に建てる
        pSave->FacilitiesChange( JoinFestaScript::FACILITIES_INDEX_0, facilitiesId, 0, 0 );

        JoinFestaFacilitiesData* pData = pThis->m_pFacilitiesSystem->GetFacilitiesData( JoinFestaScript::FACILITIES_INDEX_0 ); 
        // 施設をデータのみ強制変更
        pData->DEBUG_ForceInitialize( JoinFestaScript::FACILITIES_INDEX_0, pThis->m_pDataTable );

        // 結果格納用
        u32 hitItemCount[10] = {0};
        u32 hitItemNum = 0;
        u32 retryCount = 0;

        // 試行回数分回す
        for( u32 i=0; i<pThis->m_statisticsCount; i++ )
        {
          // 抽選用乱数作成
          gfl2::math::Random random;
          random.Initialize( pData->GetRandomSeed() );

          u32 hitItemId = pData->LotteryExecute( &random );

          // 当たったスロットのカウントインクリメント
          const u32 slot = pData->GetLotInfoIndexByItemId( hitItemId );
          hitItemCount[ slot ]++;
          hitItemNum++;

          if( pData->IsRetryDrawLot( &random ) )
          {
            retryCount++;
          }

          pData->ResetRandomSeed();
        }

        // 結果
        GFL_PRINT("再挑戦回数:%d回\n", retryCount );
        GFL_PRINT("sum,%d,slot,", hitItemNum );
        for( u32 i=0; i<LotInfo::SLOT_MAX; i++ )
        {
          GFL_PRINT("%d,", hitItemCount[i] );
        }
        GFL_PRINT("\n");
      }
    }
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：うらないや統計
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_Statistics_Fortune(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    Savedata::JoinFestaDataSave*  pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();

    GFL_PRINT("□□□□□□□□□□□□□□□□□□\n");
    GFL_PRINT("□□□□□□□□□□□□□□□□□□\n");
    GFL_PRINT("うらないや統計\n");
    GFL_PRINT("□□□□□□□□□□□□□□□□□□\n");
    GFL_PRINT("□□□□□□□□□□□□□□□□□□\n");

    // 全施設回す
    for( u32 facilitiesId=0; facilitiesId<ShopItemInfoTable::SHOPITEM_MAX; facilitiesId++ )
    {
      const NetApp::JoinFesta::ShopItemInfo* pShop = pThis->m_pDataTable->GetShopItemInfo( facilitiesId );

      if( pShop->shopType == JoinFestaScript::FACILITIES_TYPE_FORTUNE )
      {
        GFL_PRINT("================================================================\n");
        // 施設用のMsg
        // 施設名用のmstxt使う
        gfl2::str::StrBuf* workStrBuf = gfl2::debug::DebugWin_GetWorkStrBuf();
        gfl2::str::MsgData* pMsgData = pThis->m_pResourceManager->GetMsgData( NetApp::JoinFesta::JoinFestaResourceManager::RESOURCE_Message_JFShopName );
        pMsgData->GetString( pShop->messageID_ShopName, *workStrBuf );
        gfl2::str::PrintCode( workStrBuf->GetPtr(), "施設名：" );

        // 実際に建てる
        pSave->FacilitiesChange( JoinFestaScript::FACILITIES_INDEX_0, facilitiesId, 0, 0 );

        JoinFestaFacilitiesData* pData = pThis->m_pFacilitiesSystem->GetFacilitiesData( JoinFestaScript::FACILITIES_INDEX_0 ); 
        // 施設をデータのみ強制変更
        pData->DEBUG_ForceInitialize( JoinFestaScript::FACILITIES_INDEX_0, pThis->m_pDataTable );

        // 結果格納用
        u32 hitItemCount[10] = {0};
        u32 hitItemNum = 0;

        u32 hitResult[3] = {0}; // 当たり結果
        u32 hitAction[7] = {0}; // 行動結果

        // 試行回数分回す
        for( u32 i=0; i<pThis->m_statisticsCount; i++ )
        {
          // 抽選用乱数作成
          gfl2::math::Random random;
          random.Initialize( pData->GetRandomSeed() );

          u32 hitItemId = pData->LotteryExecute( &random );

          // 当たったスロットのカウントインクリメント
          const u32 slot = pData->GetLotInfoIndexByItemId( hitItemId );
          hitItemCount[ slot ]++;
          hitItemNum++;

          // [当たり・中あたり・大当たり]の抽選
          switch( pData->LotteryFortuneResultHit( &random ) ){
          case JoinFestaScript::FORTUNE_RESULT_HIT_SMALL:  hitResult[0]++; break;
          case JoinFestaScript::FORTUNE_RESULT_HIT_MIDDLE: hitResult[1]++; break;
          case JoinFestaScript::FORTUNE_RESULT_HIT_BIG:    hitResult[2]++; break;
          }

          // 当たり行動の抽選
          switch( pData->LotteryFortuneResultAction( &random ) ){
          case JoinFestaScript::FORTUNE_RESULT_ACTION_GTS:        hitAction[0]++; break;
          case JoinFestaScript::FORTUNE_RESULT_ACTION_MIRACLE:    hitAction[1]++; break;
          case JoinFestaScript::FORTUNE_RESULT_ACTION_BTLSPOT:    hitAction[2]++; break;
          case JoinFestaScript::FORTUNE_RESULT_ACTION_FESCIRCLE:  hitAction[3]++; break;
          case JoinFestaScript::FORTUNE_RESULT_ACTION_ATTRACTION: hitAction[4]++; break;
          case JoinFestaScript::FORTUNE_RESULT_ACTION_LOTTERY:    hitAction[5]++; break;
          case JoinFestaScript::FORTUNE_RESULT_ACTION_FUN_HOUSE:  hitAction[6]++; break;
          }

          pData->ResetRandomSeed();
        }

        // 結果
        GFL_PRINT("大当たり[%d],中あたり[%d],当たり[%d]\n", hitResult[2], hitResult[1], hitResult[0] );
        GFL_PRINT("GTS[%d],ミラクル[%d],スポット[%d],フェス[%d],アトラクション[%d],くじや[%d],ホラーハウス[%d]\n", hitAction[0], hitAction[1], hitAction[2], hitAction[3], hitAction[4], hitAction[5], hitAction[6]);
        GFL_PRINT("sum,%d,slot,", hitItemNum );
        for( u32 i=0; i<LotInfo::SLOT_MAX; i++ )
        {
          GFL_PRINT("%d,", hitItemCount[i] );
        }
        GFL_PRINT("\n");
      }
    }
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：施設抽選統計
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_Statistics_FacilitiesLottery(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    Savedata::JoinFestaDataSave*  pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();

    GFL_PRINT("□□□□□□□□□□□□□□□□□□\n");
    GFL_PRINT("□□□□□□□□□□□□□□□□□□\n");
    GFL_PRINT("施設抽選統計\n");
    GFL_PRINT("□□□□□□□□□□□□□□□□□□\n");
    GFL_PRINT("□□□□□□□□□□□□□□□□□□\n");

    // 結果格納用
    u32 hitShopRankCount[20][5] = {0};
    u32 hitColorCount[20][4] = {0};
    u32 hitFacilitiesIdCount[20][130] = {0};

    const u16 fesRankTable[20] = {1,2,3,4,5,6,7,8,9,10,11,21,31,41,51,61,71,81,91,100};

    for( u32 fesRankTableIndex=0; fesRankTableIndex<20; fesRankTableIndex++ )
    {
      pSave->SetRank( fesRankTable[ fesRankTableIndex ] );

      // 試行回数分回す
      for( u32 i=0; i<pThis->m_statisticsCount; i++ )
      {
        //施設抽選用乱数作成
        gfl2::math::Random random;
        u32                  randomSeed = pSave->GetFacilitiesLotterySeed();
        random.Initialize( randomSeed );

        // 施設の抽選
        u32 hitFacilitiesId = NetAppLib::JoinFesta::JoinFestaFacilitiesUtil::LotteryFacilitiesId( &random, pThis->m_pDataTable );

        hitFacilitiesIdCount[fesRankTableIndex][hitFacilitiesId]++;

        // 染物屋以外は色抽選
        const NetApp::JoinFesta::ShopItemInfo* pShop = pThis->m_pDataTable->GetShopItemInfo( hitFacilitiesId );
        if( pShop->shopType != JoinFestaScript::FACILITIES_TYPE_DYEING )
        {
          const JoinFestaScript::FacilitiesColor color = NetAppLib::JoinFesta::JoinFestaFacilitiesUtil::GetRandomColor( &random, static_cast<JoinFestaScript::FacilitiesType>(pShop->shopType), hitFacilitiesId );
          hitColorCount[ fesRankTableIndex ][ color ]++; // ランクは1オリジン
        }

        hitShopRankCount[ fesRankTableIndex ][ pShop->rank-1 ]++; // ランクは1オリジン

        // 抽選した結果で確定したので、シード値をリセット
        pSave->ResetFacilitiesLotterySeed();
      }
    }


    // 結果
    for( u32 fesRankTableIndex=0; fesRankTableIndex<20; fesRankTableIndex++ )
    {
      GFL_PRINT("抽選テーブル%02d[rank1,2,3,4,5],", fesRankTableIndex);
      for( u32 i=0; i<5; i++ )
      {
        GFL_PRINT("%d,", hitShopRankCount[ fesRankTableIndex ][ i ] );
      }
      GFL_PRINT(" color[1,2,3,4]:");
      for( u32 i=0; i<4; i++ )
      {
        GFL_PRINT("%d,", hitColorCount[ fesRankTableIndex ][ i ] );
      }
      GFL_PRINT(" 施設:");

      u32 sum = 0;
      for( u32 i = 0; i < 130; i++ )
      {
        GFL_PRINT( "ID%d[%d],", i, hitFacilitiesIdCount[ fesRankTableIndex ][ i ] );
        sum += hitFacilitiesIdCount[fesRankTableIndex][i];
      }
      GFL_PRINT(" SUM=%d ", sum);

      GFL_PRINT("\n");
    }
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：パーソナルイベント抽選統計
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_Statistics_PersonalEvent(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    Savedata::JoinFestaDataSave*  pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);

    GFL_PRINT("□□□□□□□□□□□□□□□□□□\n");
    GFL_PRINT("□□□□□□□□□□□□□□□□□□\n");
    GFL_PRINT("パーソナルイベント抽選統計\n");
    GFL_PRINT("□□□□□□□□□□□□□□□□□□\n");
    GFL_PRINT("□□□□□□□□□□□□□□□□□□\n");

    // 結果格納用
    u32 hitEventIdCount[11][JoinFestaScript::PERSONAL_EVENT_MAX] = {0};
    u32 hitDifficultyCount[11][3] = {0}; // 難易度

    const u16 fesRankTable[11] = {1,11,21,31,41,51,61,71,81,91,100};

    for( u32 fesRankTableIndex=0; fesRankTableIndex<11; fesRankTableIndex++ )
    {
      pSave->SetRank( fesRankTable[ fesRankTableIndex ] );

      // 試行回数分回す
      for( u32 i=0; i<pThis->m_statisticsCount; i++ )
      {
        // 抽選時間リセット（発動フラグも中で初期化される）
        pSave->SetPersonalEventLotteryTimeNow();
        // パーソナルデータのパーソナルイベントもリセットする
        {
          NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetFieldPersonalList();
          for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = pPersonalList->Begin(); it != pPersonalList->End(); ++it )
          {
            (*it)->ResetPersonalEvent();
          }
        }
        // 人物イベント抽選もリスト構築時に行う
        NetAppLib::JoinFesta::JoinFestaPersonalEvent::PersonalEventActuate();

        // 抽選した結果からデータ取り出す
        for( u32 slot=0; slot<NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::ACTUATE_PERSONAL_EVENT_MAX; slot++)
        {
          NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::SlotData* pSlotData =  pSave->GetPersonalEventSlotData( slot );

          if( pSlotData->state == NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::STATE_LOTTERY_OK )
          {
            NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = manager->GetPersonalData( pSlotData->friendKey );

            // 難易度
            hitDifficultyCount[ fesRankTableIndex ][ pSlotData->difficulty ]++;

            // パーソナルイベントID
            hitEventIdCount[ fesRankTableIndex ][ pPersonalData->GetPersonalEventId() ]++;
          }
        }
      }
    }


    // 結果
    for( u32 fesRankTableIndex=0; fesRankTableIndex<11; fesRankTableIndex++ )
    {
      GFL_PRINT("抽選テーブル%d[難易度]:", fesRankTableIndex);
      for( u32 i=0; i<3; i++ )
      {
        GFL_PRINT("%d,", hitDifficultyCount[ fesRankTableIndex ][ i ] );
      }
      GFL_PRINT(" EventId:");
      for( u32 i=1; i<JoinFestaScript::PERSONAL_EVENT_MAX; i++ )
      {
        GFL_PRINT("[ID%d,%d],", i, hitEventIdCount[ fesRankTableIndex ][ i ] );
      }
      GFL_PRINT("\n");
    }
  }
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   定型文共通
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_PatternCommonUpdate( void* userWork, gfl2::debug::DebugWinItem* pItem, JoinFestaScript::PatternType patternType )
{
  Savedata::JoinFestaDataSave*  pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();
  JoinFestaDebug* pThis = static_cast<JoinFestaDebug*>(userWork);

  NetAppLib::JoinFesta::JoinFestaSelectWordTable* pSelectWordTable = pThis->m_pResourceManager->GetSelectWordTable();

  u32 msgId = pSave->GetPattern( patternType );

  // 女性なら男性のに合わせる
  if( PM_FEMALE == GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatus()->GetSex() )
  {
    msgId -= msg_jf_phrase_max/2;
  }

  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_RIGHT))
  {
    if( msgId < msg_jf_phrase_max/2 ){
      msgId++;

      // カテゴリ取得してmsgIdに変換
      NetAppLib::JoinFesta::JoinFestaSelectWordTable::Category category = pSelectWordTable->GetCategory( msgId );
      NetAppLib::JoinFesta::JoinFestaSelectWordTable::Message* message = pSelectWordTable->GetCategoryTable();
      u32 categoryMsgId = message[ category ].messageID;

      // 女性なら戻す
      if( PM_FEMALE == GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatus()->GetSex() )
      {
        msgId += msg_jf_phrase_max/2;
      }
      pSave->SetPattern( patternType, msgId, categoryMsgId );
      pItem->PlaySystemSound( gfl2::debug::SE_CHANGE_VALUE );
    }
  }
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_LEFT))
  {
    if( msgId > 0 ){
      msgId--;
      // カテゴリ取得してmsgIdに変換
      NetAppLib::JoinFesta::JoinFestaSelectWordTable::Category category = pSelectWordTable->GetCategory( msgId );
      NetAppLib::JoinFesta::JoinFestaSelectWordTable::Message* message = pSelectWordTable->GetCategoryTable();
      u32 categoryMsgId = message[ category ].messageID;

      // 女性なら戻す
      if( PM_FEMALE == GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatus()->GetSex() )
      {
        msgId += msg_jf_phrase_max/2;
      }
      pSave->SetPattern( patternType, msgId, categoryMsgId );
      pItem->PlaySystemSound( gfl2::debug::SE_CHANGE_VALUE );
    }
  }
}

wchar_t* JoinFestaDebug::DebugFunc_PatternCommonDraw( void* userWork, gfl2::debug::DebugWinItem *item, JoinFestaScript::PatternType patternType )
{
  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();
  JoinFestaDebug* pThis = static_cast<JoinFestaDebug*>(userWork);

  // 定型文のMsgData
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  gfl2::str::StrBuf* workStrBuf = gfl2::debug::DebugWin_GetWorkStrBuf();
  gfl2::str::MsgData* pMsgData = pThis->m_pResourceManager->GetMsgData( NetApp::JoinFesta::JoinFestaResourceManager::RESOURCE_Message_JFPhrase );

  u32 msgId = pSave->GetPattern( patternType );
  pMsgData->GetString( msgId, *workStrBuf );

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), workStrBuf->GetPtr() );
  return workStr;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   占い結果：当たり
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_FortuneHitUpdate( void* userWork, gfl2::debug::DebugWinItem* pItem )
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);

  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_RIGHT))
  {
    if( pThis->m_fortuneHit < 3 ){
      pThis->m_fortuneHit++;
      pItem->PlaySystemSound( gfl2::debug::SE_CHANGE_VALUE );
    }
  }
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_LEFT))
  {
    if( pThis->m_fortuneHit > 0 ){
      pThis->m_fortuneHit--;
      pItem->PlaySystemSound( gfl2::debug::SE_CHANGE_VALUE );
    }
  }
}
wchar_t* JoinFestaDebug::DebugFunc_FortuneHitDraw( void* userWork, gfl2::debug::DebugWinItem *item )
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  ::Debug::DebugWin::DebugWinWork*  devWork  = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  gfl2::str::MsgData*               pMsgData = devWork->GetMessageData();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  gfl2::str::StrBuf* workStrBuf = gfl2::debug::DebugWin_GetWorkStrBuf();
  int stateMsg[4] = { msg_dmenu_jf_save_item_08, msg_dmenu_jf_save_item_07, msg_dmenu_jf_save_item_06, msg_dmenu_jf_save_item_05 };
  pMsgData->GetString( stateMsg[pThis->m_fortuneHit], *workStrBuf );
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), workStrBuf->GetPtr() );
  return workStr;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   占い結果：行動
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_FortuneActionUpdate( void* userWork, gfl2::debug::DebugWinItem* pItem )
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);

  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_RIGHT))
  {
    if( pThis->m_fortuneAction < 7 ){
      pThis->m_fortuneAction++;
      pItem->PlaySystemSound( gfl2::debug::SE_CHANGE_VALUE );
    }
  }
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_LEFT))
  {
    if( pThis->m_fortuneAction > 0 ){
      pThis->m_fortuneAction--;
      pItem->PlaySystemSound( gfl2::debug::SE_CHANGE_VALUE );
    }
  }
}
wchar_t* JoinFestaDebug::DebugFunc_FortuneActionDraw( void* userWork, gfl2::debug::DebugWinItem *item )
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  ::Debug::DebugWin::DebugWinWork*  devWork  = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  gfl2::str::MsgData*               pMsgData = devWork->GetMessageData();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  gfl2::str::StrBuf* workStrBuf = gfl2::debug::DebugWin_GetWorkStrBuf();
  int stateMsg[8] = { msg_dmenu_jf_save_item_08, msg_dmenu_jf_save_item_09, msg_dmenu_jf_save_item_10, msg_dmenu_jf_save_item_11, msg_dmenu_jf_save_item_12, msg_dmenu_jf_save_item_13, msg_dmenu_jf_save_item_14, msg_dmenu_jf_save_item_15 };
  pMsgData->GetString( stateMsg[pThis->m_fortuneAction], *workStrBuf );
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), workStrBuf->GetPtr() );
  return workStr;
}

//------------------------------------------------------------------
/**
 * @brief   占い結果設定
 */
//------------------------------------------------------------------
void JoinFestaDebug::DebugFunc_FortuneSetupUpdate(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    Savedata::JoinFestaDataSave*  pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();

    pSave->SetFortuneResult( JoinFestaScript::FACILITIES_INDEX_0, 
      static_cast<JoinFestaScript::FortuneResultHit>(pThis->m_fortuneHit),
      static_cast<JoinFestaScript::FortuneResultAction>(pThis->m_fortuneAction) );

    pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：アトラクションセーブデータID変更
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_A_SaveIndexChange(void* userWork)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  Savedata::JoinFestaDataSave*  pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();

  NetAppLib::JoinFesta::JoinFestaAttractionDefine::SaveData* pGAData = pSave->GetAttractionData( pThis->m_attractionId );
  pThis->m_AttractionData = *pGAData;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：アトラクションセーブデータ設定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_A_DataSet(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    Savedata::JoinFestaDataSave*  pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();
    pSave->SetAttractionData( pThis->m_attractionId, &pThis->m_AttractionData );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：定型コメント全開放
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_OpenMessageAllOpen(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    Savedata::JoinFestaDataSave*  pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();
    const u32* openMessageIdTbl = NetAppLib::JoinFesta::JoinFestaSelectWordTable::GetOpenMessageIdTbl();

    for (int i = 0; i < NetAppLib::JoinFesta::JoinFestaSelectWordTable::OPEN_MESSAGE_NUM; i++)
    {
      pSave->SetUseSelectWord( openMessageIdTbl[i] ); 
    }
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：アッパーリクエストコマンド送信
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_UpperCommandSend(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);

#if PM_DEBUG
#if defined(GF_PLATFORM_CTR)
    manager->GetMyData()->SetRequestFutureCommand();
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();
#endif
#endif

    pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：アッパー共通パケット送信
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_UpperUnionPacketSend(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);

#if PM_DEBUG
#if defined(GF_PLATFORM_CTR)
    manager->GetMyData()->SetRequestFutureUnion();
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();
#endif
#endif

    pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：アッパーアトラクション送信
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_UpperAttractionSend(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    NetAppLib::JoinFesta::JoinFestaPersonalData* pMyData = manager->GetMyData();

    {
      NetAppLib::JoinFesta::PacketAttraction attractionInfo = {0};

      attractionInfo.ownerTransferdID   = pMyData->GetTransferedID();
      attractionInfo.totalScore         = 0;
      attractionInfo.time               = 600;
      attractionInfo.myScore            = 0;
      ::std::memcpy( attractionInfo.name , pMyData->GetName(), sizeof( attractionInfo.name ) - 2 );
      attractionInfo.attractionID       = 999;
      attractionInfo.ticketID           = 0;

#if PM_DEBUG
#if defined(GF_PLATFORM_CTR)
      manager->GetMyData()->SetRequestAttraction( attractionInfo );
      GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->DEBUG_SetPacketAttraction( attractionInfo );
      GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();
#endif
#endif
    }

    pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：アッパー定型文送信
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_UpperPatternSendCommon(void* userWork, gfl2::debug::DebugWinItem* pItem, JoinFestaScript::PatternType patternType )
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    NetAppLib::JoinFesta::JoinFestaPersonalData* pMyData = manager->GetMyData();

#if PM_DEBUG
#if defined(GF_PLATFORM_CTR)
    manager->GetMyData()->SetRequestFuturePattern( patternType );
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();
#endif
#endif

    pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：アッパー一押し施設送信
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_UpperSendRecommendFacilitiesId(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
#if PM_DEBUG
#if defined(GF_PLATFORM_CTR)
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    manager->GetMyData()->SetRequestFutureRecommendFacilitiesId();
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();
#endif
#endif

    pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }
}
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_UpperSendRecommendFacilitiesColor(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
#if PM_DEBUG
#if defined(GF_PLATFORM_CTR)
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    manager->GetMyData()->SetRequestFutureRecommendFacilitiesColor();
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();
#endif
#endif

    pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }
}
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_UpperSendRecommendFacilitiesCharacterId(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
#if PM_DEBUG
#if defined(GF_PLATFORM_CTR)
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    manager->GetMyData()->SetRequestFutureRecommendFacilitiesCharacterId();
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();
#endif
#endif

    pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：アッパーフェスランク送信
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_UpperSendFesRank(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
#if PM_DEBUG
#if defined(GF_PLATFORM_CTR)
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    manager->GetMyData()->SetRequestFutureFesRank();
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();
#endif
#endif

    pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：アッパーきせかえアイテム総数送信
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_UpperSendHaveDressUpItemNum(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
#if PM_DEBUG
#if defined(GF_PLATFORM_CTR)
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    manager->GetMyData()->SetRequestFutureHaveDressUpItemNum();
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();
#endif
#endif

    pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：アッパーアンケート回答送信
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_UpperSendQuestionnaireAnswer(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
#if PM_DEBUG
#if defined(GF_PLATFORM_CTR)
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    manager->GetMyData()->SetRequestFutureQuestionnaireAnswer();
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();
#endif
#endif

    pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：アッパー所持施設ID送信
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_UpperSendAllFacilitiesId(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
#if PM_DEBUG
#if defined(GF_PLATFORM_CTR)
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    manager->GetMyData()->SetRequestFutureAllFacilitiesId();
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();
#endif
#endif

    pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：アッパーポケダイアリーID送信
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_UpperSendPokeDiaryId(void* userWork, gfl2::debug::DebugWinItem* pItem)
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_RIGHT))
  {
    if( pThis->m_pokeDiarySendPattern < 6 ){
      pThis->m_pokeDiarySendPattern++;
      pItem->PlaySystemSound( gfl2::debug::SE_CHANGE_VALUE );
    }
  }
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_LEFT))
  {
    if( pThis->m_pokeDiarySendPattern > 0 ){
      pThis->m_pokeDiarySendPattern--;
      pItem->PlaySystemSound( gfl2::debug::SE_CHANGE_VALUE );
    }
  }

  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
#if PM_DEBUG
#if defined(GF_PLATFORM_CTR)
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    manager->GetMyData()->SetRequestFuturePokeDiaryId( pThis->m_pokeDiarySendPattern );
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();
#endif
#endif

    pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }
}
wchar_t* JoinFestaDebug::DebugFunc_DrawSendPokeDiaryId( void* userWork, gfl2::debug::DebugWinItem *item )
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);

  const wchar_t *diaryPattern[7] = {
    L"1:NG,2:OK,3:OK", // 0
    L"1:OK,2:NG,3:OK", // 1
    L"1:OK,2:OK,3:NG", // 2
    L"1:NG,2:NG,3:OK", // 3
    L"1:NG,2:OK,3:NG", // 4
    L"1:OK,2:NG,3:NG", // 5
    L"1:NG,2:NG,3:NG", // 6
  };

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), diaryPattern[ pThis->m_pokeDiarySendPattern ] );
  return workStr;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：不正なファインダースタジオデータ送信
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDebug::DebugFunc_NgSendFinderStudio( void* userWork, gfl2::debug::DebugWinItem* pItem )
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_RIGHT))
  {
    if( pThis->m_finderStudioSendPattern < 19 ){
      pThis->m_finderStudioSendPattern++;
      pItem->PlaySystemSound( gfl2::debug::SE_CHANGE_VALUE );
    }
  }
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_LEFT))
  {
    if( pThis->m_finderStudioSendPattern > 0 ){
      pThis->m_finderStudioSendPattern--;
      pItem->PlaySystemSound( gfl2::debug::SE_CHANGE_VALUE );
    }
  }

  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
#if PM_DEBUG
#if defined(GF_PLATFORM_CTR)
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    manager->GetMyData()->SetRequestNgFinderStudio( pThis->m_finderStudioSendPattern );
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();
#endif
#endif

    pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );
    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }
}
wchar_t* JoinFestaDebug::DebugFunc_DrawNgSendFinderStudio( void* userWork, gfl2::debug::DebugWinItem *item )
{
  JoinFestaDebug*  pThis = static_cast<JoinFestaDebug*>(userWork);

  const wchar_t *diaryPattern[20] = {
    L"1:monsNo", // 0
    L"2:monsSex", // 0
    L"3:bgIndex", // 0
    L"4:bgTimeAnime", // 0
    L"5:timeZone", // 0
    L"6:trMotion", // 0
    L"7:pokePosX", // 0
    L"8:pokePosZ", // 0
    L"9:pokeDegY", // 0
    L"10:trDegY", // 0
    L"11:cameraH", // 0
    L"12:cameraDegY", // 0
    L"13:cameraDegX", // 0
    L"14:cameraFov", // 0
    L"15:cameraFocus", // 0
    L"16:stampId1", // 0
    L"17:stampId10", // 0
    L"18:stampPosX", // 0
    L"19:stampPosY", // 0
    L"20:frameId", // 0
  };

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), diaryPattern[ pThis->m_finderStudioSendPattern ] );
  return workStr;
}

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)


#endif // JF_DEBUG_MENU
