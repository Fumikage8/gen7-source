//==============================================================================
/**
 * @file	JoinFestaDebug.h
 * @brief	ジョインフェスタデバッグ機能
 * @author	ichiraku_katsuhiko
 * @date	2016/03/07 15:59:28
 */
// =============================================================================

#ifndef __JOIN_FESTA_DEBUG_H__
#define __JOIN_FESTA_DEBUG_H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
// DebugWin
#include <Debug/include/gfl2_DebugWinSystem.h>
#include <Debug/include/gfl2_DebugWinItemTypes.h>

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaNetDefine.h"
// JoinFestaDefine
#include "NetApp/JoinFesta/include/JoinFestaDefine.h"
// JoinFestaScript
#include "niji_reference_files/script/JoinFestaPawnDefine.h"
// グローバルアトラクション定義
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaGlobalAttractionDefine.h"
// アトラクション定義
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionDefine.h"

// 前方宣言
namespace NetApp{
  namespace JoinFesta{
    class JFDynamicLoadSystem;
    class JFModelManager; // JFモデルマネージャ
    class JFPoint;        // JFポイント
    class JFPointManager; // JFポイントマネージャ
    class JoinFestaEffectSystem; // エフェクトシステム
    struct JoinFestaDataTable;
    class JoinFestaResourceManager;
    class JoinFestaFacilitiesSystem;
  }
}
namespace NetAppLib {
  namespace JoinFesta {
    class JoinFestaPersonalData;
  }
}
namespace Field{
  class Fieldmap;
}

#ifdef JF_DEBUG_MENU

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class JoinFestaDebug
{
  GFL_FORBID_COPY_AND_ASSIGN(JoinFestaDebug);
public:

public:
  // コンストラクタ
  JoinFestaDebug( JFDynamicLoadSystem* pJFDynamicLoadSystem, const JoinFestaDataTable* pDataTable, JoinFestaResourceManager* pResourceManager, JoinFestaFacilitiesSystem* pFacilitiesSystem );
  // デストラクタ
  virtual ~JoinFestaDebug();

private:
  // モデル表示位置設定
  static void DebugFunc_ChangeJFPointPosUpdate( void* userWork, gfl2::debug::DebugWinItem *item );
  static wchar_t* DebugFunc_ChangeJFPointPosDraw( void* userWork, gfl2::debug::DebugWinItem *item );

  // JFポイントDump
  static void DebugFunc_JFPointPosDump( void* userWork, gfl2::debug::DebugWinItem *item );

  // ご褒美系：共通更新
  static void DebugFunc_JFRewardCommonUpdate(void* userWork, gfl2::debug::DebugWinItem* pItem, JoinFestaScript::RewardType rewardType);

  // ご褒美系：お店授与
  static void DebugFunc_JFRewardPresentShopUpdate(void* userWork, gfl2::debug::DebugWinItem* pItem) { DebugFunc_JFRewardCommonUpdate( userWork, pItem, JoinFestaScript::REWARD_PRESENT_SHOP ); }
  // ご褒美系：いいみせない？
  static void DebugFunc_JFRewardCommandShopUpdate(void* userWork, gfl2::debug::DebugWinItem* pItem) { DebugFunc_JFRewardCommonUpdate( userWork, pItem, JoinFestaScript::REWARD_COMMAND_SHOP ); }
  // ご褒美系：そのふくステキ
  static void DebugFunc_JFRewardCommandDressupUpdate(void* userWork, gfl2::debug::DebugWinItem* pItem) { DebugFunc_JFRewardCommonUpdate( userWork, pItem, JoinFestaScript::REWARD_COMMAND_DRESSUP ); }
  // ご褒美系：アトラクション開放
  static void DebugFunc_JFRewardAttractionUpdate(void* userWork, gfl2::debug::DebugWinItem* pItem) { DebugFunc_JFRewardCommonUpdate( userWork, pItem, JoinFestaScript::REWARD_ATTRACTION ); }
  // ご褒美系：ネーミングライツ
  static void DebugFunc_JFRewardNamingUpdate(void* userWork, gfl2::debug::DebugWinItem* pItem) { DebugFunc_JFRewardCommonUpdate( userWork, pItem, JoinFestaScript::REWARD_NAMING ); }
  // ご褒美系：プレミアムサービス追加
  static void DebugFunc_JFRewardPreminumUpdate(void* userWork, gfl2::debug::DebugWinItem* pItem) { DebugFunc_JFRewardCommonUpdate( userWork, pItem, JoinFestaScript::REWARD_PREMIUM ); }
  // ご褒美系：BGM変更機能
  static void DebugFunc_JFRewardBgmUpdate(void* userWork, gfl2::debug::DebugWinItem* pItem) { DebugFunc_JFRewardCommonUpdate( userWork, pItem, JoinFestaScript::REWARD_BGM ); }
  // ご褒美系：テーマ1
  static void DebugFunc_JFRewardTheme1Update(void* userWork, gfl2::debug::DebugWinItem* pItem) { DebugFunc_JFRewardCommonUpdate( userWork, pItem, JoinFestaScript::REWARD_THEME1 ); }
  // ご褒美系：テーマ2
  static void DebugFunc_JFRewardTheme2Update(void* userWork, gfl2::debug::DebugWinItem* pItem) { DebugFunc_JFRewardCommonUpdate( userWork, pItem, JoinFestaScript::REWARD_THEME2 ); }
  // ご褒美系：テーマ3
  static void DebugFunc_JFRewardTheme3Update(void* userWork, gfl2::debug::DebugWinItem* pItem) { DebugFunc_JFRewardCommonUpdate( userWork, pItem, JoinFestaScript::REWARD_THEME3 ); }
  // ご褒美系：定型文授与
  static void DebugFunc_JFRewardSelectWordUpdate(void* userWork, gfl2::debug::DebugWinItem* pItem) { DebugFunc_JFRewardCommonUpdate( userWork, pItem, JoinFestaScript::REWARD_SELECT_WORD ); }


  // ご褒美系：共通描画
  static wchar_t* DebugFunc_JFRewardCommonDraw( void* userWork, gfl2::debug::DebugWinItem *item, JoinFestaScript::RewardType rewardType );
  // ご褒美系：お店授与
  static wchar_t* DebugFunc_JFRewardPresentShopDraw(void* userWork, gfl2::debug::DebugWinItem* pItem) { return DebugFunc_JFRewardCommonDraw( userWork, pItem, JoinFestaScript::REWARD_PRESENT_SHOP ); }
  // ご褒美系：いいみせない？
  static wchar_t* DebugFunc_JFRewardCommandShopDraw( void* userWork, gfl2::debug::DebugWinItem *item ) { return DebugFunc_JFRewardCommonDraw( userWork, item, JoinFestaScript::REWARD_COMMAND_SHOP ); }
  // ご褒美系：そのふくステキ
  static wchar_t* DebugFunc_JFRewardCommandDressupDraw(void* userWork, gfl2::debug::DebugWinItem* pItem) { return DebugFunc_JFRewardCommonDraw( userWork, pItem, JoinFestaScript::REWARD_COMMAND_DRESSUP ); }
  // ご褒美系：アトラクション開放
  static wchar_t* DebugFunc_JFRewardAttractionDraw(void* userWork, gfl2::debug::DebugWinItem* pItem) { return DebugFunc_JFRewardCommonDraw( userWork, pItem, JoinFestaScript::REWARD_ATTRACTION ); }
  // ご褒美系：ネーミングライツ
  static wchar_t* DebugFunc_JFRewardNamingDraw(void* userWork, gfl2::debug::DebugWinItem* pItem) { return DebugFunc_JFRewardCommonDraw( userWork, pItem, JoinFestaScript::REWARD_NAMING ); }
  // ご褒美系：プレミアムサービス追加
  static wchar_t* DebugFunc_JFRewardPreminumDraw(void* userWork, gfl2::debug::DebugWinItem* pItem) { return DebugFunc_JFRewardCommonDraw( userWork, pItem, JoinFestaScript::REWARD_PREMIUM ); }
  // ご褒美系：BGM変更機能
  static wchar_t* DebugFunc_JFRewardBgmDraw(void* userWork, gfl2::debug::DebugWinItem* pItem) { return DebugFunc_JFRewardCommonDraw( userWork, pItem, JoinFestaScript::REWARD_BGM ); }
  // ご褒美系：テーマ1
  static wchar_t* DebugFunc_JFRewardTheme1Draw(void* userWork, gfl2::debug::DebugWinItem* pItem) { return DebugFunc_JFRewardCommonDraw( userWork, pItem, JoinFestaScript::REWARD_THEME1 ); }
  // ご褒美系：テーマ2
  static wchar_t* DebugFunc_JFRewardTheme2Draw(void* userWork, gfl2::debug::DebugWinItem* pItem) { return DebugFunc_JFRewardCommonDraw( userWork, pItem, JoinFestaScript::REWARD_THEME2 ); }
  // ご褒美系：テーマ3
  static wchar_t* DebugFunc_JFRewardTheme3Draw(void* userWork, gfl2::debug::DebugWinItem* pItem) { return DebugFunc_JFRewardCommonDraw( userWork, pItem, JoinFestaScript::REWARD_THEME3 ); }
  // ご褒美系：定型文授与
  static wchar_t* DebugFunc_JFRewardSelectWordDraw(void* userWork, gfl2::debug::DebugWinItem* pItem) { return DebugFunc_JFRewardCommonDraw( userWork, pItem, JoinFestaScript::REWARD_SELECT_WORD ); }


  // 自分のスコア設定
  static void DebugFunc_JFAttractionScoreSet(void* userWork, gfl2::debug::DebugWinItem* pItem);
  // 全てのアトラクション選べるようになる
  static void DebugFunc_JFAttractionAllPlay(void* userWork, gfl2::debug::DebugWinItem* pItem);
  // 即時リザルト
  static void DebugFunc_JFAttractionToResult(void* userWork, gfl2::debug::DebugWinItem* pItem);
  // 制限時間STOP
  static void DebugFunc_JFAttractionTimeStop(void* userWork);
  // なりすまし開始
  static void DebugFunc_JFAttractionDummySend(void* userWork, gfl2::debug::DebugWinItem* pItem);

  //! 更新:フェスコイン
  static void DebugFunc_JFUpdateFesCoin(void* userWork, gfl2::debug::DebugWinItem* pItem);
  //! 描画：フェスコイン
  static wchar_t* DebugFunc_JFDrawFesCoin( void* userWork, gfl2::debug::DebugWinItem *item );

  //! 更新:累計フェスコイン
  static void DebugFunc_JFUpdateTotalFesCoin(void* userWork, gfl2::debug::DebugWinItem* pItem);
  //! 描画：累計フェスコイン
  static wchar_t* DebugFunc_JFDrawTotalFesCoin( void* userWork, gfl2::debug::DebugWinItem *item );

  // フェスランク決定
  static void DebugFunc_JFSetFesRank(void* userWork, gfl2::debug::DebugWinItem* pItem);

  // パーソナルAllDump
  static void DebugFunc_JFPersonalAllDump(void* userWork, gfl2::debug::DebugWinItem* pItem);

  // パーソナルイベントリセット
  static void DebugFunc_JFPersonalEventReset(void* userWork, gfl2::debug::DebugWinItem* pItem);

  // パーソナルリストをダミーで埋める
  static void DebugFunc_JFDummyFull(void* userWork, gfl2::debug::DebugWinItem* pItem, NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION relation );

  // ゲスト埋める
  static void DebugFunc_JFGuestDummyFull(void* userWork, gfl2::debug::DebugWinItem* pItem )
  {
    DebugFunc_JFDummyFull( userWork, pItem, NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );
  }
  // VIP埋める
  static void DebugFunc_JFVipDummyFull(void* userWork, gfl2::debug::DebugWinItem* pItem )
  {
    DebugFunc_JFDummyFull( userWork, pItem, NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP );
  }

  // お断り埋める
  static void DebugFunc_JFBlackListDummyFull(void* userWork, gfl2::debug::DebugWinItem* pItem );

  // パーソナル：共通Index
  static void DebugFunc_UpdatePersonalCommon(void* userWork, gfl2::debug::DebugWinItem* pItem, u8 mode );
  static wchar_t* DebugFunc_DrawPersonalCommon( void* userWork, gfl2::debug::DebugWinItem *item, u8 mode );

  // パーソナル：ゲストIndex
  static void DebugFunc_UpdatePersonalGuest(void* userWork, gfl2::debug::DebugWinItem* pItem )
  {
    DebugFunc_UpdatePersonalCommon( userWork, pItem, 0 );
  }
  static wchar_t* DebugFunc_DrawPersonalGuest( void* userWork, gfl2::debug::DebugWinItem *item )
  {
    return DebugFunc_DrawPersonalCommon( userWork, item, 0 );
  }

  // パーソナル：VIPIndex
  static void DebugFunc_UpdatePersonalVip(void* userWork, gfl2::debug::DebugWinItem* pItem )
  {
    DebugFunc_UpdatePersonalCommon( userWork, pItem, 1 );
  }
  static wchar_t* DebugFunc_DrawPersonalVip( void* userWork, gfl2::debug::DebugWinItem *item )
  {
    return DebugFunc_DrawPersonalCommon( userWork, item, 1 );
  }

  // パーソナル：フィールドIndex
  static void DebugFunc_UpdatePersonalField(void* userWork, gfl2::debug::DebugWinItem* pItem )
  {
    DebugFunc_UpdatePersonalCommon( userWork, pItem, 2 );
  }
  static wchar_t* DebugFunc_DrawPersonalField( void* userWork, gfl2::debug::DebugWinItem *item )
  {
    return DebugFunc_DrawPersonalCommon( userWork, item, 2 );
  }

  // 着せ替えモデル出現
  static void DebugFunc_KisekaeModelAdd(void* userWork, gfl2::debug::DebugWinItem* pItem);

  // パーソナルイベント設定
  static void DebugFunc_PersonalEventSet(void* userWork, gfl2::debug::DebugWinItem* pItem);

  // グローバルアトラクションセーブデータID変更
  static void DebugFunc_GA_SaveIndexChange(void* userWork);
  // グローバルアトラクションセーブデータ設定
  static void DebugFunc_GA_DataSet(void* userWork, gfl2::debug::DebugWinItem* pItem);

  // ホラーハウス統計
  static void DebugFunc_Statistics_HorrorHouse(void* userWork, gfl2::debug::DebugWinItem* pItem);
  // くじや統計
  static void DebugFunc_Statistics_Lots(void* userWork, gfl2::debug::DebugWinItem* pItem);
  // うらないや統計
  static void DebugFunc_Statistics_Fortune(void* userWork, gfl2::debug::DebugWinItem* pItem);
  // 施設抽選統計
  static void DebugFunc_Statistics_FacilitiesLottery(void* userWork, gfl2::debug::DebugWinItem* pItem);
  // パーソナルイベント抽選統計
  static void DebugFunc_Statistics_PersonalEvent(void* userWork, gfl2::debug::DebugWinItem* pItem);

  // 定型文共通
  static void DebugFunc_PatternCommonUpdate( void* userWork, gfl2::debug::DebugWinItem* pItem, JoinFestaScript::PatternType patternType );
  static wchar_t* DebugFunc_PatternCommonDraw( void* userWork, gfl2::debug::DebugWinItem *item, JoinFestaScript::PatternType patternType );
  // 定型文：あいさつ
  static void DebugFunc_PatternHelloUpdate( void* userWork, gfl2::debug::DebugWinItem* pItem )
  {
    DebugFunc_PatternCommonUpdate( userWork, pItem, JoinFestaScript::PATTERN_HELLO );
  }
  static wchar_t* DebugFunc_PatternHelloDraw( void* userWork, gfl2::debug::DebugWinItem *item )
  {
    return DebugFunc_PatternCommonDraw( userWork, item, JoinFestaScript::PATTERN_HELLO );
  }
  // 定型文：わかれ
  static void DebugFunc_PatternPartingUpdate( void* userWork, gfl2::debug::DebugWinItem* pItem )
  {
    DebugFunc_PatternCommonUpdate( userWork, pItem, JoinFestaScript::PATTERN_PARTING );
  }
  static wchar_t* DebugFunc_PatternPartingDraw( void* userWork, gfl2::debug::DebugWinItem *item )
  {
    return DebugFunc_PatternCommonDraw( userWork, item, JoinFestaScript::PATTERN_PARTING );
  }
  // 定型文：感動
  static void DebugFunc_PatternEmotionUpdate( void* userWork, gfl2::debug::DebugWinItem* pItem )
  {
    DebugFunc_PatternCommonUpdate( userWork, pItem, JoinFestaScript::PATTERN_EMOTION );
  }
  static wchar_t* DebugFunc_PatternEmotionDraw( void* userWork, gfl2::debug::DebugWinItem *item )
  {
    return DebugFunc_PatternCommonDraw( userWork, item, JoinFestaScript::PATTERN_EMOTION );
  }
  // 定型文：残念
  static void DebugFunc_PatternRegretUpdate( void* userWork, gfl2::debug::DebugWinItem* pItem )
  {
    DebugFunc_PatternCommonUpdate( userWork, pItem, JoinFestaScript::PATTERN_REGRET );
  }
  static wchar_t* DebugFunc_PatternRegretDraw( void* userWork, gfl2::debug::DebugWinItem *item )
  {
    return DebugFunc_PatternCommonDraw( userWork, item, JoinFestaScript::PATTERN_REGRET );
  }

  // 占い結果：当たり
  static void DebugFunc_FortuneHitUpdate( void* userWork, gfl2::debug::DebugWinItem* pItem );
  static wchar_t* DebugFunc_FortuneHitDraw( void* userWork, gfl2::debug::DebugWinItem *item );
  // 占い結果：行動
  static void DebugFunc_FortuneActionUpdate( void* userWork, gfl2::debug::DebugWinItem* pItem );
  static wchar_t* DebugFunc_FortuneActionDraw( void* userWork, gfl2::debug::DebugWinItem *item );
  // 占い結果設定
  static void DebugFunc_FortuneSetupUpdate( void* userWork, gfl2::debug::DebugWinItem* pItem );

  // アトラクションセーブデータID変更
  static void DebugFunc_A_SaveIndexChange(void* userWork);
  // アトラクションセーブデータ設定
  static void DebugFunc_A_DataSet(void* userWork, gfl2::debug::DebugWinItem* pItem);

  // 定型コメント全開放
  static void DebugFunc_OpenMessageAllOpen(void* userWork, gfl2::debug::DebugWinItem* pItem);

  // アッパー対応
  static void DebugFunc_UpperCommandSend(void* userWork, gfl2::debug::DebugWinItem* pItem);
  static void DebugFunc_UpperUnionPacketSend(void* userWork, gfl2::debug::DebugWinItem* pItem);
  static void DebugFunc_UpperAttractionSend(void* userWork, gfl2::debug::DebugWinItem* pItem);
  static void DebugFunc_UpperPatternSendCommon(void* userWork, gfl2::debug::DebugWinItem* pItem, JoinFestaScript::PatternType patternType );
  static void DebugFunc_UpperPatternHelloSend(void* userWork, gfl2::debug::DebugWinItem* pItem )
  {
    DebugFunc_UpperPatternSendCommon( userWork, pItem, JoinFestaScript::PATTERN_HELLO );
  }
  static void DebugFunc_UpperPatternPartingSend(void* userWork, gfl2::debug::DebugWinItem* pItem )
  {
    DebugFunc_UpperPatternSendCommon( userWork, pItem, JoinFestaScript::PATTERN_PARTING );
  }
  static void DebugFunc_UpperPatternEmotionSend(void* userWork, gfl2::debug::DebugWinItem* pItem )
  {
    DebugFunc_UpperPatternSendCommon( userWork, pItem, JoinFestaScript::PATTERN_EMOTION );
  }
  static void DebugFunc_UpperPatternRegretSend(void* userWork, gfl2::debug::DebugWinItem* pItem )
  {
    DebugFunc_UpperPatternSendCommon( userWork, pItem, JoinFestaScript::PATTERN_REGRET );
  }
  static void DebugFunc_UpperPatternSecretSend(void* userWork, gfl2::debug::DebugWinItem* pItem )
  {
    DebugFunc_UpperPatternSendCommon( userWork, pItem, JoinFestaScript::PATTERN_SECRET );
  }
  static void DebugFunc_UpperSendRecommendFacilitiesId(void* userWork, gfl2::debug::DebugWinItem* pItem);
  static void DebugFunc_UpperSendRecommendFacilitiesColor(void* userWork, gfl2::debug::DebugWinItem* pItem);
  static void DebugFunc_UpperSendRecommendFacilitiesCharacterId(void* userWork, gfl2::debug::DebugWinItem* pItem);
  static void DebugFunc_UpperSendFesRank(void* userWork, gfl2::debug::DebugWinItem* pItem);
  static void DebugFunc_UpperSendHaveDressUpItemNum(void* userWork, gfl2::debug::DebugWinItem* pItem);
  static void DebugFunc_UpperSendQuestionnaireAnswer(void* userWork, gfl2::debug::DebugWinItem* pItem);
  static void DebugFunc_UpperSendAllFacilitiesId(void* userWork, gfl2::debug::DebugWinItem* pItem);

  // 未知のダイアリーID
  static void DebugFunc_UpperSendPokeDiaryId(void* userWork, gfl2::debug::DebugWinItem* pItem);
  static wchar_t* DebugFunc_DrawSendPokeDiaryId( void* userWork, gfl2::debug::DebugWinItem *item );

  // 不正なファインダースタジオデータ
  static void DebugFunc_NgSendFinderStudio(void* userWork, gfl2::debug::DebugWinItem* pItem);
  static wchar_t* DebugFunc_DrawNgSendFinderStudio( void* userWork, gfl2::debug::DebugWinItem *item );


private:

  Field::Fieldmap*            m_pFieldmap;
  JFDynamicLoadSystem*        m_pJFDynamicLoadSystem;
  const JoinFestaDataTable*   m_pDataTable;
  JoinFestaResourceManager*   m_pResourceManager;     ///< ジョインフェスタリソースマネージャ
  JoinFestaFacilitiesSystem*  m_pFacilitiesSystem;
  int                         m_debugJfPointIndex;
  u32                         m_fesRank;
  u32                         m_attractionScore;
  u32                         m_attractionTimeStop;
  u32                         m_attractionDummySend;
  u32                         m_personalId;
  u32                         m_eventId;
  u32                         m_kisekaePatternId;
  u32                         m_statisticsCount;
  int                         m_guestIndex;
  int                         m_vipIndex;
  int                         m_fieldIndex;
  u32                         m_gaSaveIndex;
  int                         m_fortuneHit;
  int                         m_fortuneAction;
  u32                         m_attractionId;
  int                         m_pokeDiarySendPattern;
  int                         m_finderStudioSendPattern;
  NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData m_GAData;
  NetAppLib::JoinFesta::JoinFestaAttractionDefine::SaveData m_AttractionData;
  gfl2::ui::DeviceManager*    m_debugDeviceManager;
  gfl2::ui::Button*           m_pDebugButton;
  gfl2::debug::DebugWinGroup* m_debugRewardGroup;
  gfl2::debug::DebugWinGroup* m_debugAttractionGroup;
  gfl2::debug::DebugWinGroup* m_debugPersonalGroup;
  gfl2::debug::DebugWinGroup* m_debugPersonalEventGroup;
  gfl2::debug::DebugWinGroup* m_debugGlobalAttractionGroup;
  gfl2::debug::DebugWinGroup* m_debugStatisticsGroup; // 統計
  gfl2::debug::DebugWinGroup* m_debugSaveGroup; // セーブ系
  gfl2::debug::DebugWinGroup* m_debugUpperGroup;

};

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

#endif // JF_DEBUG_MENU

#endif // __JOIN_FESTA_DEBUG_H__
