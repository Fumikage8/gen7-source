//==============================================================================
/**
 * @file	JoinFestaPersonalEvent.cpp
 * @brief	ジョインフェスタパーソナルイベント（スクリプトのイベント種類）
 * @author	ichiraku_katsuhiko
 * @date	2015/11/09　12:47
 */
// =============================================================================
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalEvent/JoinFestaPersonalEvent.h"

// ライブラリ
#include <base/include/gfl2_Singleton.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_Map.h>

// niji
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "System/include/HeapDefine.h"
#include "System/include/GflUse.h"

// JoinFesta
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"

// Save
#include "Savedata/include/JoinFestaDataSave.h"

// msgId
#include  <niji_conv_header/message/msg_jf_phrase.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFesta)

//! パーソナルイベントデータテーブル
const JoinFestaPersonalEvent::EventData JoinFestaPersonalEvent::m_eventDataTable[ JoinFestaScript::PERSONAL_EVENT_MAX ] =
{
  { JoinFestaPersonalEventDefine::DIFFICULTY_EASY,    5 }, // PERSONAL_EVENT_NULL
  { JoinFestaPersonalEventDefine::DIFFICULTY_EASY,    5 }, // PERSONAL_EVENT_RECENT 最近起きた事をはなす
  { JoinFestaPersonalEventDefine::DIFFICULTY_EASY,    5 }, // PERSONAL_EVENT_RECORD_BOAST レコード自慢
  { JoinFestaPersonalEventDefine::DIFFICULTY_HARD,   15 }, // PERSONAL_EVENT_RECORD_GAME レコード勝負
  { JoinFestaPersonalEventDefine::DIFFICULTY_HARD,   15 }, // PERSONAL_EVENT_RECORD_INTRODUCTION レコード紹介
  { JoinFestaPersonalEventDefine::DIFFICULTY_HARD,   15 }, // PERSONAL_EVENT_RECORD_GUESS ズバリ当てろ・レコード
  { JoinFestaPersonalEventDefine::DIFFICULTY_EASY,    5 }, // PERSONAL_EVENT_RECOMMENDATION_SHOP0,//! オススメお店案内（おなかがへったよ）
  { JoinFestaPersonalEventDefine::DIFFICULTY_EASY,    5 }, // PERSONAL_EVENT_RECOMMENDATION_SHOP1,//! オススメお店案内（かいものしたい）
  { JoinFestaPersonalEventDefine::DIFFICULTY_EASY,    5 }, // PERSONAL_EVENT_RECOMMENDATION_SHOP2,//! オススメお店案内（ポケモン楽しませたい）
  { JoinFestaPersonalEventDefine::DIFFICULTY_EASY,    5 }, // PERSONAL_EVENT_RECOMMENDATION_SHOP3,//! オススメお店案内（すごいわざを　たいけんしたい）
  { JoinFestaPersonalEventDefine::DIFFICULTY_EASY,    5 }, // PERSONAL_EVENT_RECOMMENDATION_SHOP4,//! オススメお店案内（自分へのごほうび）
  { JoinFestaPersonalEventDefine::DIFFICULTY_EASY,    5 }, // PERSONAL_EVENT_RECOMMENDATION_SHOP5,//! オススメお店案内（ドキドキしたい）
  { JoinFestaPersonalEventDefine::DIFFICULTY_NORMAL, 10 }, // PERSONAL_EVENT_RECOMMENDATION_SHOP6,//! オススメお店案内（わたしのきもちわかる？店員男）
  { JoinFestaPersonalEventDefine::DIFFICULTY_NORMAL, 10 }, // PERSONAL_EVENT_RECOMMENDATION_SHOP7,//! オススメお店案内（わたしのきもちわかる？店員女）
  { JoinFestaPersonalEventDefine::DIFFICULTY_EASY,    5 }, // PERSONAL_EVENT_RECOMMENDATION_SHOP8,//! オススメお店案内（ポケモンを遊ばせたい）
  { JoinFestaPersonalEventDefine::DIFFICULTY_EASY,   20 }, // PERSONAL_EVENT_RECOMMENDATION_PERSON オススメ人案内
  { JoinFestaPersonalEventDefine::DIFFICULTY_HARD,   15 }, // PERSONAL_EVENT_QUESTIONNAIRE_GUESS ズバリ当てろ・アンケート
  { JoinFestaPersonalEventDefine::DIFFICULTY_EASY,   10 }, // PERSONAL_EVENT_QUESTIONNAIRE_Q アンケート質問
  { JoinFestaPersonalEventDefine::DIFFICULTY_EASY,    5 }, // PERSONAL_EVENT_THANKS さっきはありがとう
  { JoinFestaPersonalEventDefine::DIFFICULTY_EASY,    5 }, // PERSONAL_EVENT_DRESSUP_BOAST 着せ替え自慢
  { JoinFestaPersonalEventDefine::DIFFICULTY_EASY,    5 }, // PERSONAL_EVENT_DRESSUP_Q 着せ替え質問
  { JoinFestaPersonalEventDefine::DIFFICULTY_HARD,   15 }, // PERSONAL_EVENT_DRESSUP_GUESS ズバリ当てろ・着せ替え
  { JoinFestaPersonalEventDefine::DIFFICULTY_NORMAL, 20 }, // PERSONAL_EVENT_SECRET 私のヒミツがしりたい？
  { JoinFestaPersonalEventDefine::DIFFICULTY_NORMAL, 20 }, // PERSONAL_EVENT_FES_BOAST フェス自慢
};

//! イベント発動抽選テーブル
const JoinFestaPersonalEvent::EventActuate   JoinFestaPersonalEvent::m_eventActuateTable[ JoinFestaPersonalEvent::EVENT_ACTUATE_TABLE_MAX ] =
{
  // JFランク（指定したランク以上）,  難易度低のイベント数、難易度中のイベント数、難易度高のイベント数
  { 100, 2, 2, 6 },
  {  91, 2, 3, 5 }, 
  {  81, 3, 2, 5 }, 
  {  71, 3, 3, 4 }, 
  {  61, 3, 4, 3 }, 
  {  51, 4, 3, 3 }, 
  {  41, 4, 3, 2 }, 
  {  31, 4, 2, 2 }, 
  {  21, 4, 2, 1 }, 
  {  11, 4, 1, 1 }, 
  {   1, 4, 1, 0 }, 
};

//! 一言ウィンドウメッセージIDテーブル
const u32  JoinFestaPersonalEvent::m_wordWindowMessageId[ JoinFestaScript::PERSONAL_EVENT_MAX ] =
{
  msg_jf_phrase_m_001, // PERSONAL_EVENT_NULL
  msg_jf_phrase_m_001, // PERSONAL_EVENT_RECENT 最近起きた事をはなす
  msg_jf_phrase_m_001, // PERSONAL_EVENT_RECORD_BOAST レコード自慢
  msg_jf_phrase_m_003, // PERSONAL_EVENT_RECORD_GAME レコード勝負
  msg_jf_phrase_m_004, // PERSONAL_EVENT_RECORD_INTRODUCTION レコード紹介
  msg_jf_phrase_m_005, // PERSONAL_EVENT_RECORD_GUESS ズバリ当てろ・レコード
  msg_jf_phrase_m_011, // PERSONAL_EVENT_RECOMMENDATION_SHOP0,//! オススメお店案内（おなかがへったよ）
  msg_jf_phrase_m_012, // PERSONAL_EVENT_RECOMMENDATION_SHOP1,//! オススメお店案内（かいものしたい）
  msg_jf_phrase_m_013, // PERSONAL_EVENT_RECOMMENDATION_SHOP2,//! オススメお店案内（ポケモン楽しませたい）
  msg_jf_phrase_m_014, // PERSONAL_EVENT_RECOMMENDATION_SHOP3,//! オススメお店案内（すごいわざを　たいけんしたい）
  msg_jf_phrase_m_015, // PERSONAL_EVENT_RECOMMENDATION_SHOP4,//! オススメお店案内（自分へのごほうび）
  msg_jf_phrase_m_016, // PERSONAL_EVENT_RECOMMENDATION_SHOP5,//! オススメお店案内（ドキドキしたい）
  msg_jf_phrase_m_017, // PERSONAL_EVENT_RECOMMENDATION_SHOP6,//! オススメお店案内（わたしのきもちわかる？店員男）
  msg_jf_phrase_m_017, // PERSONAL_EVENT_RECOMMENDATION_SHOP7,//! オススメお店案内（わたしのきもちわかる？店員女）
  msg_jf_phrase_m_019, // PERSONAL_EVENT_RECOMMENDATION_SHOP8,//! オススメお店案内（ポケモンを遊ばせたい）
  msg_jf_phrase_m_018, // PERSONAL_EVENT_RECOMMENDATION_PERSON オススメ人案内
  msg_jf_phrase_m_005, // PERSONAL_EVENT_QUESTIONNAIRE_GUESS ズバリ当てろ・アンケート
  msg_jf_phrase_m_018, // PERSONAL_EVENT_QUESTIONNAIRE_Q アンケート質問
  msg_jf_phrase_m_006, // PERSONAL_EVENT_THANKS さっきはありがとう
  msg_jf_phrase_m_002, // PERSONAL_EVENT_DRESSUP_BOAST 着せ替え自慢
  msg_jf_phrase_m_018, // PERSONAL_EVENT_DRESSUP_Q 着せ替え質問
  msg_jf_phrase_m_005, // PERSONAL_EVENT_DRESSUP_GUESS ズバリ当てろ・着せ替え
  msg_jf_phrase_m_009, // PERSONAL_EVENT_SECRET 私のヒミツがしりたい？
  msg_jf_phrase_m_001, // PERSONAL_EVENT_FES_BOAST フェス自慢
};

//! レコードカテゴリ最大数テーブル
const u8   JoinFestaPersonalEvent::m_recordCategoryNumTable[ JoinFestaPersonalEvent::RECORD_CATEGORY_NUM_TABLE_MAX ] =
{
  7, //RECORD_CATEGORY_BATTLE
  4, //RECORD_CATEGORY_TRADE
  5, //RECORD_CATEGORY_GREED
  6, //RECORD_CATEGORY_POKEMON
  5, //RECORD_CATEGORY_DAILY
  5, //RECORD_CATEGORY_CIRCLE
  6, //RECORD_CATEGORY_VARIETY
};

// コンストラクタ
JoinFestaPersonalEvent::JoinFestaPersonalEvent()
{
}

// デストラクタ
JoinFestaPersonalEvent::~JoinFestaPersonalEvent()
{
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルイベントが存在するか（フィールドメニュー用）
 *
 * @retval  true:存在する  
 * @retval  false:存在しない 
 */
/* ----------------------------------------------------------------------------------------- */
// @static_function
bool JoinFestaPersonalEvent::IsEventExist()
{
  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);

  // パーソナルデータが1つもない場合は存在しない事にする
  /*if( pJoinFestaPersonalDataManager->IsEmpty() )
  {
    ICHI_PRINT(">ICHI PersonalDataEmpty\n");
    return false;
  }*/
  // ↑フェスファンがいる為、人数0でもチェックする

  // 毎フレームチェックされるので、抽選時間が経過していた時だけ抽選処理する

  // @fix NMCat[303]：イベント発動は本当に抽選しないと分からないので回す。
  UpdateLotteryTime();
  pJoinFestaPersonalDataManager->CreateFieldPersonalList();
  PersonalEventActuate();

  // 抽選したので、フィールドリストでイベント発動している人物がいればOK
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = pJoinFestaPersonalDataManager->GetFieldPersonalList();
  for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = pPersonalList->Begin(); it != pPersonalList->End(); ++it )
  {
    // パーソナルイベント発動チェック
    if( (*it)->IsPersonalEventEnable() == true )
    {
      return true;
    }
  }

  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルイベント抽選時間更新
 *          一定時間経過していた場合は、リセット処理も走る
 */
/* ----------------------------------------------------------------------------------------- */
// @static_function
void JoinFestaPersonalEvent::UpdateLotteryTime()
{
  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

  // イベント抽選時間が経過していたら、イベント抽選時間をリセットする
  if( pSave->IsPassedPersonalEventLotteryTime() )
  {
    // 発動フラグも中で初期化される
    pSave->SetPersonalEventLotteryTimeNow();

    // パーソナルデータのパーソナルイベントもリセットする
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->ResetPersonalEvent();
  }
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルイベント発動抽選処理
 */
/* ----------------------------------------------------------------------------------------- */
// @static_function
void JoinFestaPersonalEvent::PersonalEventActuate()
{
  //ICHI_PRINT(">ICHI ------------------------------- \n");
  //ICHI_PRINT(">ICHI イベント発動抽選処理開始 \n");
  //ICHI_PRINT(">ICHI ------------------------------- \n");
  // ざっと仕様
  // 1.パーソナルイベントなめて、対象難易度のイベントが出来るパーソナルを探す
  // 2.パーソナル見つかった場合はイベント発動。見つからなかった場合は何もしない。
  // 3.最後に、PersonalManagerの方に適用
  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

  NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetFieldPersonalList();

  // 1.パーソナルイベントなめて、対象難易度のイベントが出来るパーソナルを探す
  for( u32 i=0; i<JoinFestaPersonalEventDefine::ACTUATE_PERSONAL_EVENT_MAX; i++ )
  {
    //ICHI_PRINT(">ICHI ------------------------------- \n");
    //ICHI_PRINT(">ICHI %dつ目 \n", i);
    JoinFestaPersonalEventDefine::SlotData* pSlotData = pSave->GetPersonalEventSlotData(i);

    // 設定されている難易度のものをイベント発動
    JoinFestaPersonalEventDefine::Difficulty targetDifficaluty = static_cast<JoinFestaPersonalEventDefine::Difficulty>(pSlotData->difficulty);

    // イベント状態をチェック
    switch( pSlotData->state ){
    case JoinFestaPersonalEventDefine::STATE_DISABLE:        //! イベント無効（そもそもイベントをしない）
      //ICHI_PRINT(">ICHI 無効イベント %d \n", i);
      break;
    case JoinFestaPersonalEventDefine::STATE_LOTTERY_NO:     //! 抽選していない
      {
        //ICHI_PRINT(">ICHI イベント未抽選 %d \n", i);
        // 2.パーソナル見つかった場合はイベント発動。見つからなかった場合は何もしない。
        SetPersonalEventForUnderDifficulty( i, targetDifficaluty, pPersonalList ); 
      }
      break;
    case JoinFestaPersonalEventDefine::STATE_LOTTERY_OK:     //! 抽選済み
      {
        //ICHI_PRINT(">ICHI イベント抽選済み %d \n", i);
        // フレンドキーで検索し、いた場合は何もしない。いなかったら再抽選。
        bool isFound =false;
        for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = pPersonalList->Begin(); it != pPersonalList->End(); ++it )
        {
          if( (*it)->IsSameFriendKey( pSlotData->friendKey ) )
          {
            isFound = true;
            break;
          }
        }
        if( isFound == false )
        {
          //ICHI_PRINT(">ICHI リストにいなかったので再抽選 \n");
          // いない場合は再抽選
          SetPersonalEventForUnderDifficulty( i, targetDifficaluty, pPersonalList ); 
        }
      }
      break;
    case JoinFestaPersonalEventDefine::STATE_FINISH:         //! イベント終了した
      //ICHI_PRINT(">ICHI イベント終了済み %d \n", i);
      break;
    }
  }

  // 3.最後に、PersonalManagerの方に適用。元データに書き込まないと、次に来た時にイベント発動情報が消える。
  {
    // 今回イベント発動していたデータをマネージャーリストの方に適用
    for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = pPersonalList->Begin(); it != pPersonalList->End(); ++it )
    {
      NetAppLib::JoinFesta::JoinFestaPersonalData* src = (*it);
      JoinFestaFriendKey friendKey = src->GetJoinFestaFriendKey();

      // プリセットは無視
      if( src->IsPresetNpcData() )
      {
        continue;
      }

      // パーソナルイベント発動していたらそのデータをコピー
      if( src->IsPersonalEventEnable() == true )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* dst = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetPersonalData( friendKey );
        if( dst != NULL )
        {
          // パーソナルイベント情報をコピー
          dst->Copy( *src );
        }
      }
    }
  }
  // @fix NMCat[3321]：セーブデータも更新する
  JoinFestaPacketUtil::UpdateJoinFestaPersonalSaveData();

}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   イベント発動データからパーソナルイベントセーブを設定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalEvent::SetupPersonalEventSaveForEventActuate( u16 jfRank, JoinFestaPersonalEventDefine::SlotData* pSlotDataArray )
{
  // JFランクに応じたイベント発動データ取得
  EventActuate eventActuate = m_eventActuateTable[0];

  for( int i=0; i<EVENT_ACTUATE_TABLE_MAX; i++ )
  {
    if( jfRank >= m_eventActuateTable[i].m_jfRank )
    {
      eventActuate = m_eventActuateTable[i];
      break;
    }
  }

  // 難易度の値を入れる
  {
    //ICHI_PRINT(">ICHI パーソナルイベントの難易度値セット \n");
    u32 index=0;
    //難易度高 
    for( u8 i=0; i<eventActuate.m_hardEventNum; i++ )
    {
      pSlotDataArray[ index ].difficulty = static_cast<u8>(NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::DIFFICULTY_HARD);
      pSlotDataArray[ index ].state = static_cast<u8>(JoinFestaPersonalEventDefine::STATE_LOTTERY_NO);
      //ICHI_PRINT(">ICHI %d:Hard \n", index );
      index++;
    }
    //難易度中
    for( u8 i=0; i<eventActuate.m_normalEventNum; i++ )
    {
      pSlotDataArray[ index ].difficulty = static_cast<u8>(NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::DIFFICULTY_NORMAL);
      pSlotDataArray[ index ].state = static_cast<u8>(JoinFestaPersonalEventDefine::STATE_LOTTERY_NO);
      //ICHI_PRINT(">ICHI %d:Normal \n", index );
      index++;
    }
    //難易度低
    for( u8 i=0; i<eventActuate.m_easyEventNum; i++ )
    {
      pSlotDataArray[ index ].difficulty = static_cast<u8>(NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::DIFFICULTY_EASY);
      pSlotDataArray[ index ].state = static_cast<u8>(JoinFestaPersonalEventDefine::STATE_LOTTERY_NO);
      //ICHI_PRINT(">ICHI %d:Easy \n", index );
      index++;
    }
  }
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルイベントから一言ウィンドウに出すメッセージのID取得
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaPersonalEvent::GetWordWindowMessageIdFromPersonalEvent( u32 personalEventType, u32 sex )
{
  if( personalEventType < JoinFestaScript::PERSONAL_EVENT_MAX )
  {
    const int offsetSex = ( sex == PM_MALE ) ? 0 : msg_jf_phrase_max/2 ;
    return m_wordWindowMessageId[ personalEventType ] + offsetSex;
  }
  GFL_ASSERT(0);
  return m_wordWindowMessageId[ 0 ];
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルイベントから基礎値取得
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaPersonalEvent::GetBaseFesCoin( u32 personalEventType )
{
  if( personalEventType < JoinFestaScript::PERSONAL_EVENT_MAX )
  {
    return m_eventDataTable[ personalEventType ].m_baseCoin;
  }
  GFL_ASSERT(0);
  return 1;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルからレコード値が大きいレコードを取得
 * @param[out]   pOutCategory1  レコードカテゴリー1
 * @param[out]   pOutCategory2  レコードカテゴリー2
 * @param[out]   pOutCategory3  レコードカテゴリー3
 * @param[out]   pOutRecord1    レコードINDEX1
 * @param[out]   pOutRecord2    レコードINDEX2
 * @param[out]   pOutRecord3    レコードINDEX3
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalEvent::GetHighRecord( const NetAppLib::JoinFesta::JoinFestaPersonalData* personal,
                                            u8* pOutCategory1, u8* pOutCategory2, u8* pOutCategory3,
                                            u8* pOutRecord1, u8* pOutRecord2, u8* pOutRecord3 )
{
  gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  gfl2::util::FixedSizeContainer<u8, 100> recordList;

  for( u32 recordCategory=0; recordCategory< RECORD_CATEGORY_NUM_TABLE_MAX; recordCategory++ )
  {
    for( u32 recordIndex=0; recordIndex<GetRecordCategoryNum(recordCategory); recordIndex++ )
    {
      const s32 recordValue = personal->GetRecordData( recordCategory, recordIndex );

      // 有効値
      s32 validValue = 1; 

      // ※フェスランクと集めた服は初期値１なので有効値を変える
      if( recordCategory == JoinFestaScript::RECORD_CATEGORY_GREED && recordIndex == 4 ) { validValue = 20; }
      if( recordCategory == JoinFestaScript::RECORD_CATEGORY_CIRCLE && recordIndex == 0 ) { validValue = 10; }

      if( recordValue >= validValue )
      {
        u8 id = (recordCategory*20) + recordIndex;
        recordList.push_back( id );
      }
    }
  }

  //ICHI_PRINT(">ICHI レコード数 %d \n", recordList.size() );
  GFL_ASSERT( recordList.size() );

  // 中身シャッフル
  {
    if( recordList.size() > 1 )
    {
      // とりあえず15回くらい
      for( int i=0; i<15; i++ )
      {
        u32 changeIndex = System::GflUse::GetPublicRand( recordList.size() );
        u8 tmp;
        tmp = recordList[ 0 ];
        recordList[ 0 ] = recordList[ changeIndex ];
        recordList[ changeIndex ] = tmp;
      }
    }
  }

  // ランダムに抽選
  for( u8 count=0; count<recordList.size(); count++ )
  {
    u8 hitId = recordList[ count ];

    u8 recordCategory = hitId/20;
    u8 recordIndex    = hitId%20;
    //ICHI_PRINT("%d: recordCategory[%d] recordIndex[%d]\n", count, recordCategory, recordIndex );

    switch( count ){
    case 0: *pOutCategory1 = recordCategory; *pOutRecord1 = recordIndex; break;
    case 1: *pOutCategory2 = recordCategory; *pOutRecord2 = recordIndex; break;
    case 2: *pOutCategory3 = recordCategory; *pOutRecord3 = recordIndex; break;
    }

    if( count >= 2 ) { break; }
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   レコードカテゴリの総数を取得
 * @param[in]   categoryId    レコードカテゴリ
 * @return  レコードカテゴリ総数
 */
/* ----------------------------------------------------------------------------------------- */
u8 JoinFestaPersonalEvent::GetRecordCategoryNum( u8 categoryId )
{
  if( categoryId < RECORD_CATEGORY_NUM_TABLE_MAX )
  {
    return m_recordCategoryNumTable[ categoryId ];
  }
  GFL_ASSERT(0);
  return 0;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   指定したパーソナルデータで、指定したパーソナルイベントが行えるかチェック
 *
 * @param[in]   personalEventType チェックしたいパーソナルイベント種類
 * @param[in]   personalData      チェックしたいパーソナルデータ
 *
 * @return  trueで行える
 */
/* ----------------------------------------------------------------------------------------- */
// @static_function
bool JoinFestaPersonalEvent::IsCanEventFromPersonal( JoinFestaScript::PersonalEventType personalEventType, const NetAppLib::JoinFesta::JoinFestaPersonalData& personalData )
{
  switch( personalEventType ){
  case JoinFestaScript::PERSONAL_EVENT_RECENT:              //! 最近起きた事をはなす
    {
      NetAppLib::JoinFesta::PacketGameStatus5 data5;
      bool bResult5 = personalData.GetPacketGameStatus5( &data5 );

      // 拡張情報のダイアリーが１件以上あればOK
      if( bResult5 == true )
      {
        if( data5.pokeDiary[0].diary_id != PDID_NULL )
        {
          return true;
        }
      }
    }
    break;

  case JoinFestaScript::PERSONAL_EVENT_RECORD_BOAST:        //! レコード自慢
    {
      // 拡張情報のレコードデータに数字が１以上のものが1件以上あれば選出
      for( u32 recordCategory=0; recordCategory< RECORD_CATEGORY_NUM_TABLE_MAX; recordCategory++ )
      {
        for( u32 recordIndex=0; recordIndex<GetRecordCategoryNum(recordCategory); recordIndex++ )
        {
          // ※フェスランクと集めた服は選出条件から除外する
          if( recordCategory == JoinFestaScript::RECORD_CATEGORY_GREED && recordIndex == 4 ) { continue; }
          if( recordCategory == JoinFestaScript::RECORD_CATEGORY_CIRCLE && recordIndex == 0 ) { continue; }

          const s32 recordValue = personalData.GetRecordData( recordCategory, recordIndex );
          if( recordValue >= 1 )
          {
            return true;
          }
        }
      }
    }
    break;
  case JoinFestaScript::PERSONAL_EVENT_RECORD_GAME:         //! レコード勝負
    {
      // 拡張情報のレコードデータに数字が1以上あるレコードがジャンル別に3件以上あれば選出
      u8 count = 0;

      for( u32 recordCategory=0; recordCategory< RECORD_CATEGORY_NUM_TABLE_MAX; recordCategory++ )
      {
        for( u32 recordIndex=0; recordIndex<GetRecordCategoryNum(recordCategory); recordIndex++ )
        {
          // ※フェスランクと集めた服は選出条件から除外する
          if( recordCategory == JoinFestaScript::RECORD_CATEGORY_GREED && recordIndex == 4 ) { continue; }
          if( recordCategory == JoinFestaScript::RECORD_CATEGORY_CIRCLE && recordIndex == 0 ) { continue; }

          const s32 recordValue = personalData.GetRecordData( recordCategory, recordIndex );
          if( recordValue >= 1 )
          {
            count++;
            break;
          }
        }
      }

      if( count >= 3 )
      {
        return true;
      }
    }
    break;
  case JoinFestaScript::PERSONAL_EVENT_QUESTIONNAIRE_GUESS: //! ズバリ当てろ・アンケート
    {
      NetAppLib::JoinFesta::PacketGameStatus1 data1;
      bool bResult1 = personalData.GetPacketGameStatus1( &data1 );

      // 拡張情報のアンケートデータに1個以上有効な回答があれば選出
      if( bResult1 == true )
      {
        for( u32 i=0; i<16; i++ )
        { 
          // アンケート回答が無回答(=0)、ナイショ(=1)以外であればOK
          if( data1.questionnaireAnswer[ i ] >= 2 )
          {
            return true;
          }
        }
      }
      return false;
    }
  case JoinFestaScript::PERSONAL_EVENT_DRESSUP_GUESS:       //! ズバリ当てろ・着せ替え
    {
      // 拡張情報に一押しアイテムがあり、現在着ている
      return false;
    }
  case JoinFestaScript::PERSONAL_EVENT_SECRET:              //! 私のヒミツがしりたい？
    {
      // 定型コメントヒミツに、518以外が入っている
      u16 patternSecret = personalData.GetPattern( JoinFestaScript::PATTERN_SECRET );
      if( !(patternSecret == msg_jf_phrase_m_518 || patternSecret == msg_jf_phrase_f_518) )
      {
        return true;
      }
    }
    break;
  case JoinFestaScript::PERSONAL_EVENT_FES_BOAST:           //! フェス自慢
    {
      NetAppLib::JoinFesta::PacketGameStatus1 data1;
      bool bResult1 = personalData.GetPacketGameStatus1( &data1 );

      // 拡張情報のフェスのランクが3以上
      if( bResult1 == true )
      {
        if( data1.jfRank >= 3 )
        {
          return true;
        }
      }
    }
    break;
  case JoinFestaScript::PERSONAL_EVENT_RECORD_GUESS:        //! ズバリ当てろ・レコード
    {
      // 関係性の人数が10人以上
      if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetCount( personalData.GetRelation() ) >= 10 )
      {
        return true;
      }
    }
    break;
  case JoinFestaScript::PERSONAL_EVENT_RECORD_INTRODUCTION: //! レコード紹介
  case JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP0://! オススメお店案内（おなかがへったよ）
  case JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP1://! オススメお店案内（かいものしたい）
  case JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP2://! オススメお店案内（ポケモン楽しませたい）
  case JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP3://! オススメお店案内（すごいわざを　たいけんしたい）
  case JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP4://! オススメお店案内（自分へのごほうび）
  case JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP5://! オススメお店案内（ドキドキしたい）
  case JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP6://! オススメお店案内（わたしのきもちわかる？店員男）
  case JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP7://! オススメお店案内（わたしのきもちわかる？店員女）
  case JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP8://! オススメお店案内（ポケモンを遊ばせたい）
  case JoinFestaScript::PERSONAL_EVENT_QUESTIONNAIRE_Q:     //! アンケート質問
    // 無条件でOK
    return true;
  case JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_PERSON://! オススメ人案内
  case JoinFestaScript::PERSONAL_EVENT_DRESSUP_BOAST:       //! 着せ替え自慢
  case JoinFestaScript::PERSONAL_EVENT_DRESSUP_Q:           //! 着せ替え質問
    // 未実装
    return false;
  case JoinFestaScript::PERSONAL_EVENT_THANKS:              //! さっきはありがとう
    // P2Pのみ
    return false;
  }

  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   指定した難易度以下でパーソナルイベント発動
 *
 * @param[in]   targetDifficulty  パーソナルイベント難易度
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalEvent::SetPersonalEventForUnderDifficulty( const u32 personalEventSaveIndex, JoinFestaPersonalEventDefine::Difficulty targetDifficulty, NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList )
{
  bool isBreak = false;
  while( !isBreak ) {
    // 指定された難易度でイベント出来るかチェック
    NetAppLib::JoinFesta::JoinFestaPersonalData* personal = GetPersonalDataFromDifficulty( targetDifficulty, pPersonalList );

    if( personal != NULL )
    {
      // パーソナルイベント抽選
      JoinFestaScript::PersonalEventType personalEventType = LotteryPersonalEvent( targetDifficulty, *personal );

      // パーソナルイベント発動
      personal->SetPersonalEvent( personalEventSaveIndex, personalEventType );
      isBreak = true;
      break;
    }

    // 難易度１つ下げる
    if( targetDifficulty == JoinFestaPersonalEventDefine::DIFFICULTY_HARD )
    {
      targetDifficulty = JoinFestaPersonalEventDefine::DIFFICULTY_NORMAL;
      //ICHI_PRINT("難易度中に下げる\n");
    }
    else if( targetDifficulty == JoinFestaPersonalEventDefine::DIFFICULTY_NORMAL )
    {
      targetDifficulty = JoinFestaPersonalEventDefine::DIFFICULTY_EASY;
      //ICHI_PRINT("難易度低に下げる\n");
    }
    else
    {
      //ICHI_PRINT("どの難易度も出来なかった\n");
      isBreak = true;
    }
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   指定した難易度でイベントが行えるパーソナルデータを取得
 *
 * @param[in]   targetDifficulty  パーソナルイベント難易度
 *
 * @return      パーソナルデータ　見つからなければNULL
 */
/* ----------------------------------------------------------------------------------------- */
NetAppLib::JoinFesta::JoinFestaPersonalData* JoinFestaPersonalEvent::GetPersonalDataFromDifficulty( JoinFestaPersonalEventDefine::Difficulty targetDifficulty, NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList )
{
  // イベント行えるパーソナル保存用
  gfl2::util::FixedSizeContainer<NetAppLib::JoinFesta::JoinFestaPersonalData*, NetApp::JoinFesta::JoinFestaDefine::JFPOINT_MAX> list;

  // パーソナルリスト分まわす
  for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = pPersonalList->Begin(); it != pPersonalList->End(); ++it )
  {
    NetAppLib::JoinFesta::JoinFestaPersonalData* personal = (*it);
    JoinFestaFriendKey friendKey = personal->GetJoinFestaFriendKey();

    // プリセットは含めない
    if( personal->IsPresetNpcData() )
    {
      continue;
    }

    // 既にイベント発動していたパーソナルデータならば抽選リストに含めない
    if( personal->IsPersonalEventEnable() == true )
    {
      //ICHI_PRINT(">ICHI 既にイベントは発動済み1  friendKey:(%u,%llu)\n", friendKey.principalId, friendKey.localFriendCode);
      continue;
    }

    // 既にイベント終了している場合も含めない
    if( IsSamePersonalDataFromPersonalEvent( *personal ) )
    {
      //ICHI_PRINT(">ICHI 既にイベントは発動済み2  friendKey:(%u,%llu)\n", friendKey.principalId, friendKey.localFriendCode);
      continue;
    }

    // 指定した難易度のイベントが行えるかチェック
    if( IsCanDifficultyFromPersonal( targetDifficulty, *personal ) )
    {
      // リストに追加
      list.push_back( personal );
    }
  }
  //ICHI_PRINT(">ICHI listCount %d \n", list.size() );

  if( list.size() == 1 )
  {
    return list[ 0 ];
  }
  else if( list.size() > 1 )
  {
    // ランダムに人を抽出
    u32 randomValue = System::GflUse::GetPublicRand( (list.size()-1) );
    return list[ randomValue ];
  }

  return NULL;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルイベントからパーソナルデータが一致するものがあるかチェック
 *
 * @param[in]   personal  チェックするパーソナルデータ
 *
 * @return      trueで一致するものがあった
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalEvent::IsSamePersonalDataFromPersonalEvent( const NetAppLib::JoinFesta::JoinFestaPersonalData& personal )
{
  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();
  for( u32 i=0; i<JoinFestaPersonalEventDefine::ACTUATE_PERSONAL_EVENT_MAX; i++ )
  {
    JoinFestaPersonalEventDefine::SlotData* pSlotData = pSave->GetPersonalEventSlotData(i);

    // イベント状態が発動してよい状態かチェック
    if( pSlotData->state == JoinFestaPersonalEventDefine::STATE_DISABLE )
    {
      continue;
    }

    // フレンドキーが一致しているかチェック
    if( personal.IsSameFriendKey( pSlotData->friendKey ) )
    {
      return true;
    }
  }
  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   指定したパーソナルデータで、指定した難易度のイベントが行えるかチェック
 *
 * @param[in]   targetDifficulty  チェックしたいパーソナルイベント難易度
 * @param[in]   personalData      チェックしたいパーソナルデータ
 *
 * @return  trueで行える
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaPersonalEvent::IsCanDifficultyFromPersonal( JoinFestaPersonalEventDefine::Difficulty targetDifficulty, const NetAppLib::JoinFesta::JoinFestaPersonalData& personalData )
{
  // 全パーソナルイベントをチェック
  for( u8 eventId = 0; eventId<JoinFestaScript::PERSONAL_EVENT_MAX; eventId++ )
  {
    if( m_eventDataTable[ eventId ].m_difficulty != targetDifficulty )
    {
      // 指定難易度以外はチェックする必要ないのでスキップ
      continue;
    }

    // イベント実行可能チェック
    if( IsCanEventFromPersonal( static_cast<JoinFestaScript::PersonalEventType>(eventId), personalData ) )
    {
      return true;
    }
  }

  return false;
}



/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   指定した難易度でイベント抽選する
 *
 * @param[in]   targetDifficulty  イベント難易度
 * @param[in]   personalData      パーソナルデータ
 *
 * @return  抽選したイベント
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaScript::PersonalEventType JoinFestaPersonalEvent::LotteryPersonalEvent( JoinFestaPersonalEventDefine::Difficulty targetDifficulty, const NetAppLib::JoinFesta::JoinFestaPersonalData& personalData )
{
  gfl2::util::FixedSizeContainer<u8, JoinFestaScript::PERSONAL_EVENT_MAX> canEventList;

  // 全パーソナルイベントをチェック
  for( u8 eventId = 0; eventId<JoinFestaScript::PERSONAL_EVENT_MAX; eventId++ )
  {
    if( m_eventDataTable[ eventId ].m_difficulty != targetDifficulty )
    {
      // 指定難易度以外はチェックする必要ないのでスキップ
      continue;
    }

    // 施設案内が9つあって当たりやすいので、
    // 施設案内を1つとしてカウントし施設案内が当たった時に、どの施設案内にするか決める
    // 
    switch( eventId ){
    // case JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP0://! オススメお店案内（おなかがへったよ）
    // case JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP1://! オススメお店案内（かいものしたい）
    // ↑の２つだけ抽選させる
    case JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP2://! オススメお店案内（ポケモン楽しませたい）
    case JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP3://! オススメお店案内（すごいわざを　たいけんしたい）
    case JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP4://! オススメお店案内（自分へのごほうび）
    case JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP5://! オススメお店案内（ドキドキしたい）
    // →無条件 case JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP6://! オススメお店案内（わたしのきもちわかる？店員男）
    // →無条件 case JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP7://! オススメお店案内（わたしのきもちわかる？店員女）
    case JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP8://! オススメお店案内（ポケモンを遊ばせたい）
      continue;
    }

    // イベント実行可能チェック
    if( IsCanEventFromPersonal( static_cast<JoinFestaScript::PersonalEventType>(eventId), personalData ) )
    {
      canEventList.push_back( eventId );
    }
  }

  //ICHI_PRINT(">ICHI 可能イベント数 %d \n", canEventList.size() );
  GFL_ASSERT( canEventList.size() );

  // ランダムに抽選
  u8 hitEvent = 0;

  if( canEventList.size() == 1 )
  {
    // 1つしかないので固定
    hitEvent = canEventList[ 0 ];
  }
  else if( canEventList.size() > 1 )
  {
    u32 randomValue = System::GflUse::GetPublicRand( (canEventList.size()) );
    hitEvent = canEventList[ randomValue ];
  }

  // 施設案内だった場合、どの施設案内かランダムで決める
  if( hitEvent == JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP0 || hitEvent == JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP1 )
  {
    // 難易度低の施設案内イベントテーブル
    const u8 easyRecommendationShopTable[] = {
      JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP0,
      JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP1,
      JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP2,
      JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP3,
      JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP4,
      JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP5,
      JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP8,
    };
    const u8 easyRecommendationShopTableMax = GFL_NELEMS( easyRecommendationShopTable );
    hitEvent = easyRecommendationShopTable[ System::GflUse::GetPublicRand( easyRecommendationShopTableMax ) ];
    ICHI_PRINT(">ICHI 施設案内イベントだったので再度抽選した %d \n", hitEvent );
  }

  return static_cast<JoinFestaScript::PersonalEventType>(hitEvent);
}




GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetAppLib)

