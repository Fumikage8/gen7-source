//===================================================================
/**
 * @file    JoinFestaDataSave.h
 * @brief   JoinFestaデータ管理
 * @author  ichiraku_katsuhiko
 * @date    2015/10/16
*/
//===================================================================
#include "../include/JoinFestaDataSave.h"

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_std_string.h>
// system
#include "System/include/GflUse.h"
#include "System/include/HeapDefine.h"
#include "GameSys/include/NijiTime.h"
#include "Print/include/WordSet.h"
#include "System/include/CTR/CTR_Config.h" 
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Savedata/include/Record.h"
#include "Savedata/include/JoinFestaVipSave.h"
#include "Savedata/include/JoinFestaGuestSave.h"
#include "Savedata/include/GameTime.h"

// モデルのインデックス
#include <niji_conv_header/field/chara/chara_model_id.h>

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalEvent/JoinFestaPersonalEvent.h"
#include "NetApp/JoinFesta/include/JoinFestaDataTable/JoinFestaDataTable.h"
// ジョインフェスタ施設ユーティリティ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaFacilitiesUtil/JoinFestaFacilitiesUtil.h"
// ジョインフェスタパーソナルデータユーティリティ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataUtil.h"

// フェスサークル初期名用
#include <arc_index/message.gaix>
// 定型文用msgId
#include  <niji_conv_header/message/msg_jf_phrase.h>
#include  <niji_conv_header/message/msg_setphrase.h>

GFL_NAMESPACE_BEGIN(Savedata)

//--------------------------------------------------------------------------------------------
/**
 * @brief    コンストラクタ
 */
//--------------------------------------------------------------------------------------------
JoinFestaDataSave::JoinFestaDataSave( void ) :
  m_facilitiesIndex(JoinFestaScript::FACILITIES_INDEX_MAX)
  ,m_fakeFacilitiesIndex(JoinFestaScript::FACILITIES_INDEX_NULL)
  ,m_personalTalkCameraCount(0)
  ,m_isSkipMessageAirToy(0)
  ,m_isSkipMessageStall(0)
  ,m_isSkipMessageHorrorHouse(0)
{
  STATIC_ASSERT( sizeof(SaveBody) == 14744 );
  gfl2::std::MemClear( &m_fakeFacilitiesCoreData, sizeof(NetApp::JoinFesta::JoinFestaFacilitiesData::CoreData) );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief    デストラクタ
 */
//--------------------------------------------------------------------------------------------
JoinFestaDataSave::~JoinFestaDataSave(void)
{
}

//------------------------------------------------------------------------------------------
/**
* @brief    データの先頭ポインタを得る関数
*
* @return  先頭のポインタ
*/
//------------------------------------------------------------------------------------------
// @override
void* JoinFestaDataSave::GetData(void)
{
  return &m_body;
}


//------------------------------------------------------------------------------------------
/**
* @brief    データサイズ
*
* @return  バイト
*/
//------------------------------------------------------------------------------------------
// @override
size_t JoinFestaDataSave::GetDataSize(void)
{
  return sizeof(SaveBody);
}


//-----------------------------------------------------------------------------
/**
 * @brief セーブデータのクリア
 * @param heap クリアに使用するテンポラリヒープ
 */
//-----------------------------------------------------------------------------
// @override
void JoinFestaDataSave::Clear( gfl2::heap::HeapBase * heap )
{
  gfl2::std::MemClear( &m_body, sizeof(SaveBody) );

  // 初期ランクは1
  m_body.m_rank = 1;

  // パーソナルイベントの抽選時間更新
  //  ※ランク使う
  SetPersonalEventLotteryTimeNow();

  // 施設抽選ランダムシード初期化
  ResetFacilitiesLotterySeed();

  // 初期施設ID
  InitializeDefaultFacilities();

  // フェスサークル初期名
  {
    gfl2::str::MsgData* pMsgData = GFL_NEW( heap ) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_jf_window_DAT, heap );
    gfl2::str::StrBuf tempStrBuf( 256, heap );
    pMsgData->GetString( 0, tempStrBuf );
    tempStrBuf.PutStr( m_body.m_joinFestaName, NetApp::JoinFesta::JoinFestaDefine::STRLEN_JOIN_FESTA_NAME + ::System::EOM_LEN );

    // プリセットNPCの名前男女分
    pMsgData->GetString( 1, tempStrBuf );
    tempStrBuf.PutStr( m_body.m_presetNpcName[0], ::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN );
    pMsgData->GetString( 2, tempStrBuf );
    tempStrBuf.PutStr( m_body.m_presetNpcName[1], ::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN );
    GFL_SAFE_DELETE(pMsgData);
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief 自分の定型文の初期値を設定
 *
 * @note  性別が決まらないと決めれない為、関数にして性別決まった後で呼ぶ
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::InitializeDefaultPattern( const u8 sex )
{
  // 定型文の初期データ
  {
#if defined(GF_PLATFORM_CTR)
    u64 transferdId = nn::cfg::CTR::GetTransferableId( GameSys::GameManager::APLICATION_UNIQUE_ID );
#else
    u64 transferdId = System::GflUse::GetPublicRand();
#endif

    // トランスファードIDを14で割った余りで決まる
    const u32 tblIndex = transferdId % NetAppLib::JoinFesta::JoinFestaPersonalDataUtil::INIT_PATTENR_TABLE_NUM; 
    ICHI_PRINT(">ICHI 初期定型文のテーブル %d \n", tblIndex);

    // 性別がFEMALEの場合は、オフセットずらす
    const int offsetSex = ( sex == PM_MALE ) ? 0 : msg_jf_phrase_max/2 ;

    // 定型文
    m_body.patternHello           = NetAppLib::JoinFesta::JoinFestaPersonalDataUtil::GetInitPatternMsg( tblIndex, 0 ) + offsetSex;
    m_body.patternParting         = NetAppLib::JoinFesta::JoinFestaPersonalDataUtil::GetInitPatternMsg( tblIndex, 1 ) + offsetSex;
    m_body.patternEmotion         = NetAppLib::JoinFesta::JoinFestaPersonalDataUtil::GetInitPatternMsg( tblIndex, 2 ) + offsetSex;
    m_body.patternRegret          = NetAppLib::JoinFesta::JoinFestaPersonalDataUtil::GetInitPatternMsg( tblIndex, 3 ) + offsetSex;
    m_body.patternSecret          = ( sex == PM_MALE ) ? msg_jf_phrase_m_518 : msg_jf_phrase_f_518 ;
    // 定型文のカテゴリ
    m_body.patternCategoryHello   = NetAppLib::JoinFesta::JoinFestaPersonalDataUtil::GetInitCategoryMsg( tblIndex, 0 );
    m_body.patternCategoryParting = NetAppLib::JoinFesta::JoinFestaPersonalDataUtil::GetInitCategoryMsg( tblIndex, 1 );
    m_body.patternCategoryEmotion = NetAppLib::JoinFesta::JoinFestaPersonalDataUtil::GetInitCategoryMsg( tblIndex, 2 );
    m_body.patternCategoryRegret  = NetAppLib::JoinFesta::JoinFestaPersonalDataUtil::GetInitCategoryMsg( tblIndex, 3 );
    m_body.patternCategorySecret  = msg_setphrase_categ_20;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief 日付更新におけるリセット処理
 *
 * @note  ペナルティタイムの時は呼ばれない
 */
//-----------------------------------------------------------------------------
void JoinFestaDataSave::ResetWithDaytime( void )
{
  GFL_PRINT(">JFSAVE ResetWithDaytime \n");
  // 施設の購入フラグをリセット
  for( u8 i=0; i<JoinFestaScript::FACILITIES_INDEX_MAX; i++ )
  {
    m_body.m_facilities[i].itemBuyBit = 0;
    m_body.m_facilities[i].fortuneResultHit     = static_cast<u8>(JoinFestaScript::FORTUNE_RESULT_HIT_NULL);
    m_body.m_facilities[i].fortuneResultAction  = static_cast<u8>(JoinFestaScript::FORTUNE_RESULT_ACTION_NULL);

    // こうかんじょの残り回数をリセット
    m_body.m_facilities[i].tradeRemainCount = NetAppLib::JoinFesta::JoinFestaFacilitiesUtil::GetTradeRemainCountMax( m_body.m_facilities[i].facilitiesId );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief ペナルティタイム発動による更新
 */
//-----------------------------------------------------------------------------
void JoinFestaDataSave::PenaltyTimeUpdate()
{
  GFL_PRINT(">JFSAVE PenaltyTimeUpdate \n");
  // 施設の購入フラグを全てON
  for( u8 i=0; i<JoinFestaScript::FACILITIES_INDEX_MAX; i++ )
  {
    m_body.m_facilities[i].itemBuyBit = 0xFFFFFFFF;

    // @fix GFNMCat[3745]　占い結果もクリアする
    m_body.m_facilities[i].fortuneResultHit     = static_cast<u8>(JoinFestaScript::FORTUNE_RESULT_HIT_NULL);
    m_body.m_facilities[i].fortuneResultAction  = static_cast<u8>(JoinFestaScript::FORTUNE_RESULT_ACTION_NULL);

    // こうかんじょの残り回数は0にする
    m_body.m_facilities[i].tradeRemainCount = 0;
  }
  // パーソナルイベントの抽選時間を固定でMAX時間に。
  GameSys::DeviceDate date;
  m_body.m_personalEventLotteryTime = date.GetDateConst()->GetParameters();
  // ↑イベントフラグに関してはそのままとする。
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief １分毎に更新される処理（フィールド中更新）
 * @param  diff_minute   変わった分数
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::UpdateMinuteTimeAtField( s32 diff_minute )
{
  // 交換ペナルティカウント減算
  if( m_body.m_tradePenaltyCount > 0 )
  {
    if( m_body.m_tradePenaltyCount < static_cast<u32>(diff_minute) )
    {
      m_body.m_tradePenaltyCount = 0;
    }
    else
    {
      m_body.m_tradePenaltyCount -= diff_minute;
    }
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  定型文設定
 *  @param  patternType   定型文種類
 *  @param  patternMsgId  定型文のメッセージID（msg_jf_phrase.h）
 *  @param  patternCategoryMsgId  定型カテゴリのメッセージID（msg_setphrase.h）
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetPattern( const JoinFestaScript::PatternType patternType, const u16 patternMsgId, const u16 patternCategoryMsgId )
{
  switch( patternType ){
  case JoinFestaScript::PATTERN_HELLO:
    m_body.patternHello         = patternMsgId;
    m_body.patternCategoryHello = patternCategoryMsgId;
    break;
  case JoinFestaScript::PATTERN_PARTING:
    m_body.patternParting         = patternMsgId;
    m_body.patternCategoryParting = patternCategoryMsgId;
    break;
  case JoinFestaScript::PATTERN_EMOTION:
    m_body.patternEmotion         = patternMsgId;
    m_body.patternCategoryEmotion = patternCategoryMsgId;
    break;
  case JoinFestaScript::PATTERN_REGRET:
    m_body.patternRegret          = patternMsgId;
    m_body.patternCategoryRegret  = patternCategoryMsgId;
    break;
  case JoinFestaScript::PATTERN_SECRET:
    m_body.patternSecret          = patternMsgId;
    m_body.patternCategorySecret  = patternCategoryMsgId;
    break;
  default: GFL_ASSERT_MSG(0,"%d",patternType); break;
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  定型文取得
 *  @param  patternType   定型文種類
 *  @return 定型カテゴリのメッセージID（msg_setphrase.h）
 */
/* ----------------------------------------------------------------------------------------- */
u16 JoinFestaDataSave::GetPatternCateogry( const JoinFestaScript::PatternType patternType ) const
{
  switch( patternType ){
  case JoinFestaScript::PATTERN_HELLO:    //!< 定型：あいさつ
    return m_body.patternCategoryHello;
  case JoinFestaScript::PATTERN_PARTING:  //!< 定型：別れ
    return m_body.patternCategoryParting;
  case JoinFestaScript::PATTERN_EMOTION:  //!< 定型：感動
    return m_body.patternCategoryEmotion;
  case JoinFestaScript::PATTERN_REGRET:   //!< 定型：残念
    return m_body.patternCategoryRegret;
  case JoinFestaScript::PATTERN_SECRET:   //!< 定型：ナイショ
    return m_body.patternCategorySecret;
  }
  GFL_ASSERT_MSG(0,"%d",patternType);
  return 0;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  定型文取得
 *  @param  patternType   定型文種類
 *  @return 定型文のメッセージID（msg_jf_phrase.h）
 */
/* ----------------------------------------------------------------------------------------- */
u16 JoinFestaDataSave::GetPattern( const JoinFestaScript::PatternType patternType ) const
{
  switch( patternType ){
  case JoinFestaScript::PATTERN_HELLO:    //!< 定型：あいさつ
    return m_body.patternHello;
  case JoinFestaScript::PATTERN_PARTING:  //!< 定型：別れ
    return m_body.patternParting;
  case JoinFestaScript::PATTERN_EMOTION:  //!< 定型：感動
    return m_body.patternEmotion;
  case JoinFestaScript::PATTERN_REGRET:   //!< 定型：残念
    return m_body.patternRegret;
  case JoinFestaScript::PATTERN_SECRET:   //!< 定型：ナイショ
    return m_body.patternSecret;
  }
  GFL_ASSERT_MSG(0,"%d",patternType);
  return 0;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  友達との通信拒否フラグ設定
 *  @param  isReject   通信拒否をONにするならtrueを指定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetRejectFriend( const bool isReject )
{
  m_body.isRejectFriend = isReject;

  // @fix NMcat[1269]：拒否設定解除でセーブから復帰
  //if( isReject == false )
  //{
  //  // VIPもゲストも友達がいるかもしれないのでどちらも復帰
  //  Savedata::JoinFestaVip::ApplyJoinFestaVipSaveData();
  //  Savedata::JoinFestaGuest::ApplyJoinFestaGuestSaveData();
  //}
  // @fix GFNMCat[1635]：フレンドを拒否した場合は消す
  // ※削除した為、↑の復帰は意味なくなったので削除
  if( isReject == true )
  {
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->RemoveCtrFriend();
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  友達との通信拒否フラグ取得
 *  @return  通信拒否がONであればtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaDataSave::IsRejectFriend() const
{
  return m_body.isRejectFriend;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  VIPとの通信拒否フラグ設定
 *  @param  isReject   通信拒否をONにするならtrueを指定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetRejectVip( const bool isReject )
{
  bool isResetPersonalEvent = false;
  if( m_body.isRejectVip == true && isReject == false )
  {
    isResetPersonalEvent = true;// 拒否NGからOKにした時のみ
  }

  m_body.isRejectVip = isReject;

  // @fix NMcat[1269]：拒否設定解除でセーブから復帰
  if( isReject == false )
  {
    Savedata::JoinFestaVip::ApplyJoinFestaVipSaveData();

    // @fix NMCat[3954]：セーブにはイベント状態が残っている為、展開直後にリセットする。
    //                   セーブには残る事になるが、イベント抽選時(=フィールドで新しい人物セットアップ時）にセーブ更新が走る為、
    //                   展開した人物が表示されるまでにはセーブからもクリアされる。
    if( isResetPersonalEvent == true )
    {
      GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->ResetPersonalEvent( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP );
    }
  }
  else
  { // リストからVIP削除
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->ReleaseList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  VIPとの通信拒否フラグ取得
 *  @return  通信拒否がONであればtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaDataSave::IsRejectVip() const
{
  return m_body.isRejectVip;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  Guestとの通信拒否フラグ設定
 *  @param  isReject   通信拒否をONにするならtrueを指定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetRejectGuest( const bool isReject )
{
  bool isResetPersonalEvent = false;
  if( m_body.isRejectGuest == true && isReject == false )
  {
    isResetPersonalEvent = true;// 拒否NGからOKにした時のみ
  }

  m_body.isRejectGuest = isReject;

  // @fix NMcat[1269]：拒否設定解除でセーブから復帰
  if( isReject == false )
  {
    Savedata::JoinFestaGuest::ApplyJoinFestaGuestSaveData();

    // @fix NMCat[3954]：セーブにはイベント状態が残っている為、展開直後にリセットする。
    //                   セーブには残る事になるが、イベント抽選時(=フィールドで新しい人物セットアップ時）にセーブ更新が走る為、
    //                   展開した人物が表示されるまでにはセーブからもクリアされる。
    if( isResetPersonalEvent == true )
    {
      GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->ResetPersonalEvent( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );
    }
  }
  else
  { // リストからGuest削除
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->ReleaseList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  Guestとの通信拒否フラグ取得
 *  @return  通信拒否がONであればtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaDataSave::IsRejectGuest() const
{
  return m_body.isRejectGuest;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  通信対戦いつでもOKフラグ設定
 *  @param  isOk   通信対戦いつでもONにするならtrueを指定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetAlwaysBattleOk( const bool isOk )
{
  m_body.isAlwaysBattleOk = isOk;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  通信対戦いつでもOkフラグ取得
 *  @param  通信対戦いつでもOKがONであればtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaDataSave::IsAlwaysBattleOk() const
{
  return m_body.isAlwaysBattleOk;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  通信交換いつでもOKフラグ設定
 *  @param  isOk   通信交換いつでもONにするならtrueを指定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetAlwaysTradeOk( const bool isOk )
{
  m_body.isAlwaysTradeOk = isOk;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  通信交換いつでもOkフラグ取得
 *  @param  通信交換いつでもOKがONであればtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaDataSave::IsAlwaysTradeOk() const
{
  return m_body.isAlwaysTradeOk;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  アトラクションいつでもOKフラグ設定
 *  @param  isOk   アトラクションいつでもONにするならtrueを指定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetAlwaysAttractionOk( const bool isOk )
{
  m_body.isAlwaysAttractionOk = isOk;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  アトラクションいつでもOkフラグ取得
 *  @param  アトラクションいつでもOKがONであればtrueを返却
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaDataSave::IsAlwaysAttractionOk() const
{
  return m_body.isAlwaysAttractionOk;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  現在のテーマを設定する
 *  @param  theme   フェスサークルのテーマ
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetTheme( const u8 theme )
{
  GFL_ASSERT( theme < JoinFestaScript::FES_THEME_MAX );
  if( theme < JoinFestaScript::FES_THEME_MAX )
  {
    m_body.theme = theme;
    GFL_PRINT(">JFSAVE テーマ%dに設定\n", theme );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  現在のテーマを取得する
 *  @return フェスサークルのテーマ
 */
/* ----------------------------------------------------------------------------------------- */
u8 JoinFestaDataSave::GetTheme() const
{
  return m_body.theme;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  施設抽選用のランダムシード取得
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaDataSave::GetFacilitiesLotterySeed()
{
  return m_body.facilitiesLotterySeed;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  施設抽選用のランダムシードをリセットする
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::ResetFacilitiesLotterySeed()
{
  m_body.facilitiesLotterySeed = System::GflUse::GetPublicRand();
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  アンケート回答を設定する
 *  @param  questionId 質問ID
 *  @param  answer     答え
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetQuestionnaireAnswer( const u32 questionId, const u32 answer )
{
  if( questionId < 16 )
  {
    m_body.questionnaireAnswer[ questionId ] = answer;
    GFL_PRINT(">JFSAVE SetQuestionnaireAnswer [%d]= %d \n", questionId, answer );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  アンケート回答を取得する
 *  @param  questionId 質問ID
 *  @return answer     答え
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaDataSave::GetQuestionnaireAnswer( const u32 questionId ) const
{
  if( questionId < 16 )
  {
    return m_body.questionnaireAnswer[ questionId ];
  }
  GFL_ASSERT(0);
  return 0;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  パーソナルイベントで質問するアンケートID取得
 *  @return アンケートID
 */
/* ----------------------------------------------------------------------------------------- */
u8 JoinFestaDataSave::GetQuestionnaireIdForPersonalEvent()
{
  const u8 beforeQuestionnaireId = m_body.beforeQuestionnaireId;

  // 加算しておく
  m_body.beforeQuestionnaireId++;
  if( m_body.beforeQuestionnaireId >= 16 )
  {
    m_body.beforeQuestionnaireId = 0;
  }

  return beforeQuestionnaireId;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief パーソナルイベント抽選完了
 *  @param  slotIndex   インデックス（最大値 ACTUATE_PERSONAL_EVENT_MAX )
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetPersonalEventLotteryOk( const u32 slotIndex, const JoinFestaFriendKey& friendKey )
{
  GFL_ASSERT( slotIndex < NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::ACTUATE_PERSONAL_EVENT_MAX );
  if( slotIndex < NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::ACTUATE_PERSONAL_EVENT_MAX )
  {
    switch( m_body.m_personalEventSlotData[ slotIndex ].state ){
    case NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::STATE_LOTTERY_NO:  //! 抽選していない
    case NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::STATE_LOTTERY_OK:  //! 抽選済み
      m_body.m_personalEventSlotData[ slotIndex ].state = static_cast<u8>(NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::STATE_LOTTERY_OK);
      m_body.m_personalEventSlotData[ slotIndex ].friendKey = friendKey;
      //ICHI_PRINT(">ICHI JoinFestaDataSave::SetPersonalEventLotteryOk %d \n", slotIndex );
      break;
    default:
      GFL_ASSERT(0);
      break;
    }
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief パーソナルイベント終了
 *  @param  slotIndex   インデックス（最大値 ACTUATE_PERSONAL_EVENT_MAX )
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetPersonalEventFinish( const u32 slotIndex )
{
  GFL_ASSERT( slotIndex < NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::ACTUATE_PERSONAL_EVENT_MAX );
  if( slotIndex < NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::ACTUATE_PERSONAL_EVENT_MAX )
  {
    m_body.m_personalEventSlotData[ slotIndex ].state = static_cast<u8>(NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::STATE_FINISH);
    //ICHI_PRINT(">ICHI JoinFestaDataSave::SetPersonalEventFinish %d \n", slotIndex );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  パーソナルイベント取得
 *  @param  slotIndex   インデックス（最大値 ACTUATE_PERSONAL_EVENT_MAX )
 *  @return イベントデータ
 */
/* ----------------------------------------------------------------------------------------- */
NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::SlotData* JoinFestaDataSave::GetPersonalEventSlotData( const u32 slotIndex )
{
  GFL_ASSERT( slotIndex < NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::ACTUATE_PERSONAL_EVENT_MAX );
  if( slotIndex < NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::ACTUATE_PERSONAL_EVENT_MAX )
  {
    return &m_body.m_personalEventSlotData[ slotIndex ];
  }
  return NULL;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  パーソナルイベント抽選時間を今の時間にする
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetPersonalEventLotteryTimeNow()
{
  //ICHI_PRINT(">ICHI JoinFestaDataSave::SetPersonalEventLotteryTimeNow\n");
  GameSys::DeviceDate date;
  m_body.m_personalEventLotteryTime = date.GetDateConst()->GetParameters();
  
  // イベントフラグもリセットする
  for( u8 i=0; i<NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::ACTUATE_PERSONAL_EVENT_MAX; i++ )
  {
    m_body.m_personalEventSlotData[i].state = static_cast<u8>(NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::STATE_DISABLE );
    m_body.m_personalEventSlotData[i].friendKey.initialize();
  }

  // プリセットNPCのイベント発動も同タイミングでリセット
  for( u8 i=0; i<NetApp::JoinFesta::JoinFestaDefine::PRESET_ID_MAX; i++ )
  {
    m_body.m_presetNpcEventState[ i ] = static_cast<u8>(NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::STATE_DISABLE );
    m_body.m_presetNpcEventId[ i ]    = static_cast<u8>(JoinFestaScript::PERSONAL_EVENT_NULL);
  }

  // JFランクに応じたイベント発動データ設定
  NetAppLib::JoinFesta::JoinFestaPersonalEvent::SetupPersonalEventSaveForEventActuate( m_body.m_rank, m_body.m_personalEventSlotData ); 
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  パーソナルイベント抽選時間が経過したか
 *  @return trueで経過した
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaDataSave::IsPassedPersonalEventLotteryTime()
{
  GameSys::DeviceDate date;

  gfl2::system::Date lottery( m_body.m_personalEventLotteryTime );

  // 経過秒数を計算
  s64 passSecond = date.GetDateConst()->GetDateTimeToSecond() - lottery.GetDateTimeToSecond();

  if( passSecond > NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::PERSONAL_EVENT_LOTTERY_TIME )
  {
    ICHI_PRINT(">ICHI 経過秒数:%lld \n", passSecond );
    return true;
  }
  return false;
}

/************************************************************************/
/*  施設用                                                              */
/************************************************************************/
/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  施設データ設定
 *  @param[in] 施設データ
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetFacilitiesData( const JoinFestaScript::FacilitiesIndex facilitiesIndex, NetApp::JoinFesta::JoinFestaFacilitiesData::CoreData* pData )
{
  GFL_ASSERT( facilitiesIndex < JoinFestaScript::FACILITIES_INDEX_MAX );
  if( facilitiesIndex < JoinFestaScript::FACILITIES_INDEX_MAX )
  {
    gfl2::std::MemCopy( pData, &m_body.m_facilities[ facilitiesIndex ], sizeof(NetApp::JoinFesta::JoinFestaFacilitiesData::CoreData) );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  施設データ取得
 *  @return 施設データ
 */
/* ----------------------------------------------------------------------------------------- */
NetApp::JoinFesta::JoinFestaFacilitiesData::CoreData* JoinFestaDataSave::GetFacilitiesData( const JoinFestaScript::FacilitiesIndex facilitiesIndex )
{
  if( facilitiesIndex < JoinFestaScript::FACILITIES_INDEX_MAX )
  {
    // もしFakeDataが設定されていたらそれを返す
    if( m_fakeFacilitiesIndex == facilitiesIndex )
    {
      GFL_PRINT(">JFSAVE FakeDataの施設 index:%d \n", facilitiesIndex);
      return &m_fakeFacilitiesCoreData;
    }

    return &m_body.m_facilities[ facilitiesIndex ];
  }
  GFL_ASSERT( 0 );
  return &m_body.m_facilities[ 0 ]; // 止まらないように
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  施設を変更する
 *  @param[in] facilitiesIndex  変個する施設Index
 *  @param[in] facilitiesId     新しい施設のID
 *  @param[in] characterId      新しい施設の店員
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::FacilitiesChange( const JoinFestaScript::FacilitiesIndex facilitiesIndex, const u8 facilitiesId, const u32 characterId, const u8 color )
{
  if( facilitiesIndex < JoinFestaScript::FACILITIES_INDEX_MAX )
  {
    // @fix GFNMCat[3768]：ペナルティフラグON時はリセットしない
    u32 saveItemBuyBit = m_body.m_facilities[ facilitiesIndex ].itemBuyBit; // 商品購入Bit

    // @fix GFNMCat[1819]：建て替えた時に紹介者情報が残っていた為、クリアしてから施設情報を入れる。
    gfl2::std::MemClear( &m_body.m_facilities[ facilitiesIndex ], sizeof(NetApp::JoinFesta::JoinFestaFacilitiesData::CoreData) );

    m_body.m_facilities[ facilitiesIndex ].facilitiesId = facilitiesId;
    m_body.m_facilities[ facilitiesIndex ].characterId = characterId;
    m_body.m_facilities[ facilitiesIndex ].color = color;

    // 立て直した時は、1日1回制限をリセット
    // @fix GFNMCat[3768]：ペナルティフラグON時はリセットしない
    {
      Savedata::GameTime* pGameTime = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetGameTime();
      if( pGameTime->IsPenaltyMode() )
      {
        // ペナルティ時は購入Bitを戻す
        m_body.m_facilities[ facilitiesIndex ].itemBuyBit = saveItemBuyBit;
      }
      else
      {
        m_body.m_facilities[ facilitiesIndex ].itemBuyBit = 0;
      }
    }
    m_body.m_facilities[ facilitiesIndex ].fortuneResultHit     = static_cast<u8>(JoinFestaScript::FORTUNE_RESULT_HIT_NULL);
    m_body.m_facilities[ facilitiesIndex ].fortuneResultAction  = static_cast<u8>(JoinFestaScript::FORTUNE_RESULT_ACTION_NULL);

    // こうかんじょの残り回数をリセット
    m_body.m_facilities[ facilitiesIndex ].tradeRemainCount = NetAppLib::JoinFesta::JoinFestaFacilitiesUtil::GetTradeRemainCountMax( facilitiesId );

    GFL_PRINT(">JFSAVE FacilitiesChange [%d]= ID:%d 店員:%d 色:%d \n", facilitiesIndex, facilitiesId, characterId, m_body.m_facilities[ facilitiesIndex ].color );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  施設の場所を入れ替える
 *  @param[in] facilitiesIndex  変個する施設Index
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::FacilitiesChangeIndex( const JoinFestaScript::FacilitiesIndex facilitiesIndex1, const JoinFestaScript::FacilitiesIndex facilitiesIndex2 )
{
  if( facilitiesIndex1 < JoinFestaScript::FACILITIES_INDEX_MAX && facilitiesIndex2 < JoinFestaScript::FACILITIES_INDEX_MAX )
  {
    NetApp::JoinFesta::JoinFestaFacilitiesData::CoreData tmp;
    tmp = m_body.m_facilities[ facilitiesIndex1 ];
    m_body.m_facilities[ facilitiesIndex1 ] = m_body.m_facilities[ facilitiesIndex2 ];
    m_body.m_facilities[ facilitiesIndex2 ] = tmp;
    GFL_PRINT(">JFSAVE FacilitiesChangeIndex [%d] と [%d] \n", facilitiesIndex1, facilitiesIndex2 );
  }
}


/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  施設を変更した振りをする
 *  @param[in] facilitiesIndex  変個する施設Index
 *  @param[in] facilitiesId     施設のID
 *  @param[in] characterId      施設の店員
 *  @param[in] color            施設の色
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::FacilitiesFakeChangeStart( const JoinFestaScript::FacilitiesIndex facilitiesIndex, const u8 facilitiesId, const u32 characterId, const u8 color )
{
  GFL_ASSERT( m_fakeFacilitiesIndex == JoinFestaScript::FACILITIES_INDEX_NULL );
  m_fakeFacilitiesIndex = facilitiesIndex;

  // 施設の外観だけなら以下のデータのみでOK
  m_fakeFacilitiesCoreData.facilitiesId     = facilitiesId;
  m_fakeFacilitiesCoreData.color            = color;
  m_fakeFacilitiesCoreData.characterId      = characterId;
  GFL_PRINT(">JFSAVE FacilitiesFakeChangeStart [%d]= ID:%d 店員:%d 色:%d \n", facilitiesIndex, facilitiesId, characterId, color );
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  施設を変更した振りを終了する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::FacilitiesFakeChangeEnd()
{
  GFL_ASSERT( m_fakeFacilitiesIndex != JoinFestaScript::FACILITIES_INDEX_NULL );
  gfl2::std::MemClear( &m_fakeFacilitiesCoreData, sizeof(NetApp::JoinFesta::JoinFestaFacilitiesData::CoreData) );
  m_fakeFacilitiesIndex = JoinFestaScript::FACILITIES_INDEX_NULL;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  起動するショップがどの施設Indexなのか設定
 *  @param[in] 施設Index
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetFacilitiesIndexOfScriptCall( const JoinFestaScript::FacilitiesIndex facilitiesIndex )
{
  m_facilitiesIndex = facilitiesIndex;
  GFL_PRINT(">JFSAVE SetFacilitiesIndexOfScriptCall =%d\n", m_facilitiesIndex );
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  施設の商品購入
 *  @param[in] itemNo   購入した商品の番号（上から0オリジン）
 *  @li     事前にSetFacilitiesIndexOfScriptCallでどの施設か設定する必要がある
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetFacilitiesBuy( const u32 itemNo )
{
  GFL_ASSERT(m_facilitiesIndex < JoinFestaScript::FACILITIES_INDEX_MAX);

  if( m_facilitiesIndex < JoinFestaScript::FACILITIES_INDEX_MAX )
  {
    m_body.m_facilities[ m_facilitiesIndex ].itemBuyBit |= (1<<itemNo);
    ICHI_PRINT(">JFSAVE SetFacilitiesBuy = 0x%x\n", m_body.m_facilities[ m_facilitiesIndex ].itemBuyBit );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  施設の商品購入してあるかチェック
 *  @param[in] itemNo   購入した商品の番号（上から0オリジン）
 *  @return 購入しているとtrue
 *  @li     事前にSetFacilitiesIndexOfScriptCallでどの施設か設定する必要がある
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaDataSave::IsFacilitiesBuy( const u32 itemNo )
{
  GFL_ASSERT(m_facilitiesIndex < JoinFestaScript::FACILITIES_INDEX_MAX);

  if( m_facilitiesIndex < JoinFestaScript::FACILITIES_INDEX_MAX )
  {
    if( m_body.m_facilities[ m_facilitiesIndex ].itemBuyBit & (1<<itemNo) )
    {
      return true;
    }
    return false;
  }
  return true;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  占いの結果を設定
 *  @param[in] facilitiesIndex  施設Index
 *  @param[in] fortuneResultHit 当たりの結果
 *  @param[in] fortuneResultAction 行動の結果
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetFortuneResult( const JoinFestaScript::FacilitiesIndex facilitiesIndex,
                         const JoinFestaScript::FortuneResultHit fortuneResultHit,
                         const JoinFestaScript::FortuneResultAction fortuneResultAction )
{
  if( facilitiesIndex < JoinFestaScript::FACILITIES_INDEX_MAX )
  {
    m_body.m_facilities[facilitiesIndex].fortuneResultHit     = fortuneResultHit;
    m_body.m_facilities[facilitiesIndex].fortuneResultAction  = fortuneResultAction;
    ICHI_PRINT(">JFSAVE SetFortuneResult Index:%d hit:%d action:%d \n", facilitiesIndex, fortuneResultHit, fortuneResultAction );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  占いやの結果取得（当たり）
 *  @param  fortuneResultAction　結果を取得したい行動
 *  @return 占いの当たり結果
 *  @li     結果が複数ある場合、結果として1番良いのを返す
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaScript::FortuneResultHit JoinFestaDataSave::GetFortuneResultHitByAction( const JoinFestaScript::FortuneResultAction fortuneResultAction ) const
{
  JoinFestaScript::FortuneResultHit retFortuneResultHit = JoinFestaScript::FORTUNE_RESULT_HIT_NULL; 

  for( u32 facilitiesIndex=0; facilitiesIndex<JoinFestaScript::FACILITIES_INDEX_MAX; facilitiesIndex++ )
  {
    JoinFestaScript::FortuneResultHit fortuneResultHit = static_cast<JoinFestaScript::FortuneResultHit>(m_body.m_facilities[facilitiesIndex].fortuneResultHit); 

    // 調べたい行動と一致しているかチェック
    if( m_body.m_facilities[facilitiesIndex].fortuneResultAction == fortuneResultAction )
    {
      // 1番良い結果で上書きする
      if( fortuneResultHit > retFortuneResultHit )
      {
        retFortuneResultHit = fortuneResultHit;
      }
    }
  }

  return retFortuneResultHit;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  コイン追加
 *  @param[in] 追加コイン枚数
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::AddCoin( const u32 addCoin )
{
  m_body.m_coin += addCoin;
  m_body.m_totalCoin += addCoin;
  if( m_body.m_coin > NetApp::JoinFesta::JoinFestaDefine::COIN_MAX )
  {
    m_body.m_coin = NetApp::JoinFesta::JoinFestaDefine::COIN_MAX;
  }
  if( m_body.m_totalCoin > NetApp::JoinFesta::JoinFestaDefine::COIN_MAX )
  {
    m_body.m_totalCoin = NetApp::JoinFesta::JoinFestaDefine::COIN_MAX;
  }

  // フェスランクカンストしても、ランクアップさせたい
  if( GetRank() == NetApp::JoinFesta::JoinFestaDefine::RANK_MAX )
  {
    m_body.m_fesRankMaxTotalCoin += addCoin;
  }

  GFL_PRINT(">JFSAVE AddCoin 獲得コイン    =%d\n", m_body.m_coin);
  GFL_PRINT(">JFSAVE 獲得コイン累計=%d\n", m_body.m_totalCoin);
}
/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  コイン減算
 *  @param[in] 減算コイン枚数
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SubCoin( const u32 subCoin )
{
  if( m_body.m_coin <= subCoin )
  {
    m_body.m_coin = 0;
  }
  else
  {
    m_body.m_coin -= subCoin;
  }

  // ジョインフェスタで使ったコイン累計
  Savedata::AddRecord( Savedata::Record::RECID_JF_USE_JCOIN, subCoin );
  GFL_PRINT(">JFSAVE SubCoin 獲得コイン =%d\n", m_body.m_coin);
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  コイン設定
 *  @param[in] コイン設定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetCoin( const u32 coin )
{
  m_body.m_coin = coin;
  if( m_body.m_coin > NetApp::JoinFesta::JoinFestaDefine::COIN_MAX )
  {
    m_body.m_coin = NetApp::JoinFesta::JoinFestaDefine::COIN_MAX;
  }
  ICHI_PRINT(">JFSAVE SetCoin 獲得コイン    =%d\n", m_body.m_coin);
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  獲得コイン取得
 *  @return 獲得コイン枚数
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaDataSave::GetCoin() const
{
  //ICHI_PRINT(">JFSAVE GetCoin 獲得コイン    =%d\n", m_body.m_coin);
  return m_body.m_coin;
}
/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  獲得コイン累計取得
 *  @return 獲得コイン累計枚数
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaDataSave::GetTotalCoin() const
{
  return m_body.m_totalCoin;
}

#if PM_DEBUG
/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  獲得コイン累計設定
 *  @param  獲得コイン累計枚数
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetTotalCoin( u32 coin )
{
  m_body.m_totalCoin = coin;
}
#endif

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  ジョインフェスタ名設定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetJoinFestaNameString( gfl2::str::StrBuf* src )
{
  src->PutStr( m_body.m_joinFestaName, NetApp::JoinFesta::JoinFestaDefine::STRLEN_JOIN_FESTA_NAME + ::System::EOM_LEN );
}
/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  ジョインフェスタ名取得
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::GetJoinFestaNameString( gfl2::str::StrBuf* dst ) const
{
  dst->SetStr( m_body.m_joinFestaName );
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  フェスランク設定
 *  @param[in] フェスランク
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetRank( const u16 rank )
{
  m_body.m_rank = rank;

  if( m_body.m_rank > NetApp::JoinFesta::JoinFestaDefine::RANK_MAX ) {
    m_body.m_rank = NetApp::JoinFesta::JoinFestaDefine::RANK_MAX;
  }
  GFL_PRINT(">JFSAVE フェスランク = %d\n", m_body.m_rank);
}
/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  フェスランク取得
 *  @return フェスランク
 */
/* ----------------------------------------------------------------------------------------- */
u16 JoinFestaDataSave::GetRank() const
{
  return m_body.m_rank;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  次のランクまでに必要なコイン取得
 *  @return コイン数（0だと満たしているのランクアップしてよい）
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaDataSave::GetCoinNecessaryForNextRank()
{
  // フェスランクカンスト値してもランクアップさせたい
  if( GetRank() == NetApp::JoinFesta::JoinFestaDefine::RANK_MAX )
  {
    if( m_body.m_fesRankMaxTotalCoin >= NetApp::JoinFesta::JoinFestaDefine::MAX_FESRANK_RANKUP_COIN )
    {// ランクアップ
      m_body.m_fesRankMaxTotalCoin -= NetApp::JoinFesta::JoinFestaDefine::MAX_FESRANK_RANKUP_COIN;
      return 0;
    }
    return ( NetApp::JoinFesta::JoinFestaDefine::MAX_FESRANK_RANKUP_COIN - m_body.m_fesRankMaxTotalCoin );
  }
  else
  {
    // 次のランクまでに必要な累計コイン枚数計算
    u32 coinSum = 0;
    for( u16 rank=1; rank< (GetRank() + 1); rank++ )
    {
      coinSum += NetAppLib::JoinFesta::JoinFestaFacilitiesUtil::GetNextRankFesCoin( rank );
    }

    if( coinSum >= GetTotalCoin() )
    {
      return ( coinSum - GetTotalCoin() );
    }

    return 0;
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  ご褒美状態設定
 *  @param[in] index  配列インデックス
 *  @param[in] state  状態(0:初期 1:条件満たした 2:ご褒美イベントこなした )
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetRewardState( const JoinFestaScript::RewardType index, const JoinFestaScript::RewardStateType state )
{
  GFL_ASSERT( index < JoinFestaScript::REWARD_MAX );
  if( index < JoinFestaScript::REWARD_MAX )
  {
    m_body.m_rewardState[ index ] = static_cast<u8>(state);
    ICHI_PRINT(">JFSAVE SET m_body.m_rewardState[ %d ] = %d \n", index, state);
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  ご褒美状態取得
 *  @param[in] index  配列インデックス
 *  @reutrn 状態(0:初期 1:条件満たした 2:ご褒美イベントこなした )
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaScript::RewardStateType JoinFestaDataSave::GetRewardState( const JoinFestaScript::RewardType index ) const
{
  GFL_ASSERT( index < JoinFestaScript::REWARD_MAX );
  if( index < JoinFestaScript::REWARD_MAX )
  {
    ICHI_PRINT(">JFSAVE GET m_body.m_rewardState[ %d ] = %d \n", index, m_body.m_rewardState[ index ]);
    return static_cast<JoinFestaScript::RewardStateType>( m_body.m_rewardState[ index ] );
  }
  return JoinFestaScript::REWARD_STATE_NULL;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  ご褒美イベント発動しているかチェック
 *  @reutrn 発動していたらtrue
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaDataSave::IsRewardEvent() const
{
  ICHI_PRINT(">JFSAVE JoinFestaDataSave::IsRewardEvent \n");
  for( u8 i=0; i<JoinFestaScript::REWARD_MAX; i++ )
  {
    if( m_body.m_rewardState[ i ] == JoinFestaScript::REWARD_STATE_SATISFY_CONDITION )
    {
      ICHI_PRINT(">JFSAVE ご褒美イベント発動している！\n");
      return true;
    }
  }
  ICHI_PRINT(">JFSAVE ご褒美イベント発動していない！\n");
  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  最高個人スコア取得
 *  @param  attractionId    アトラクションID
 *  @return 最高個人スコア
 */
/* ----------------------------------------------------------------------------------------- */
u16 JoinFestaDataSave::GetManHighScore( const JoinFestaScript::AttractionId attractionId ) const
{
  GFL_ASSERT( attractionId < JoinFestaScript::ATTRACTION_NUM );
  if( attractionId < JoinFestaScript::ATTRACTION_NUM )
  {
    ICHI_PRINT(">JFSAVE 最高個人スコア取得 ID=%d SCORE=%d\n", attractionId, m_body.m_attractionData[ attractionId ].manHighScore );
    return m_body.m_attractionData[ attractionId ].manHighScore;
  }
  return 0;
}
/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  最高個人スコア設定
 *  @param  attractionId    アトラクションID
 *  @param  score           更新するスコア値
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetManHighScore( const JoinFestaScript::AttractionId attractionId, const u16 score ) 
{
  GFL_ASSERT( attractionId < JoinFestaScript::ATTRACTION_NUM );
  if( attractionId < JoinFestaScript::ATTRACTION_NUM )
  {
    m_body.m_attractionData[ attractionId ].manHighScore = score;
    GFL_PRINT(">JFSAVE 最高個人スコア設定 ID=%d SCORE=%d\n", attractionId, score );
  }
}
/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  最高トータルスコア取得
 *  @param  attractionId    アトラクションID
 *  @return 最高トータルスコア
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaDataSave::GetTotalHighScore( const JoinFestaScript::AttractionId attractionId ) const
{
  GFL_ASSERT( attractionId < JoinFestaScript::ATTRACTION_NUM );
  if( attractionId < JoinFestaScript::ATTRACTION_NUM )
  {
    ICHI_PRINT(">JFSAVE 最高個人スコア取得 ID=%d SCORE=%d\n", attractionId, m_body.m_attractionData[ attractionId ].totalHighScore );
    return m_body.m_attractionData[ attractionId ].totalHighScore;
  }
  return 0;
}
/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  最高トータルスコア設定
 *  @param  attractionId    アトラクションID
 *  @param  score           更新するスコア値
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetTotalHighScore( const JoinFestaScript::AttractionId attractionId, const u32 score ) 
{
  GFL_ASSERT( attractionId < JoinFestaScript::ATTRACTION_NUM );
  if( attractionId < JoinFestaScript::ATTRACTION_NUM )
  {
    m_body.m_attractionData[ attractionId ].totalHighScore = score;
    GFL_PRINT(">JFSAVE 最高トータルスコア設定 ID=%d SCORE=%d\n", attractionId, score );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  プレイ回数加算
 *  @param  attractionId    アトラクションID
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::AddPlayCount( const JoinFestaScript::AttractionId attractionId )
{
  GFL_ASSERT( attractionId < JoinFestaScript::ATTRACTION_NUM );
  if( attractionId < JoinFestaScript::ATTRACTION_NUM )
  {
    if( m_body.m_attractionData[ attractionId ].playCount < 255 )
    {
      m_body.m_attractionData[ attractionId ].playCount++;
    }
    GFL_PRINT(">JFSAVE アトラクションPlayCount加算 %d\n", m_body.m_attractionData[ attractionId ].playCount);
  }
}
/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  プレイ回数取得
 *  @param  attractionId    アトラクションID
 *  @return プレイ回数
 */
/* ----------------------------------------------------------------------------------------- */
u16 JoinFestaDataSave::GetPlayCount( const JoinFestaScript::AttractionId attractionId ) const
{
  GFL_ASSERT( attractionId < JoinFestaScript::ATTRACTION_NUM );
  if( attractionId < JoinFestaScript::ATTRACTION_NUM )
  {
    ICHI_PRINT(">JFSAVE アトラクションPlayCount取得 ID=%d COUNT=%d\n", attractionId, m_body.m_attractionData[ attractionId ].playCount );
    return m_body.m_attractionData[ attractionId ].playCount;
  }
  return 0;
}

#if PM_DEBUG
/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  デバッグ用：アトラクション構造体データ取得
 *  @param  attractionId    アトラクションID
 */
/* ----------------------------------------------------------------------------------------- */
NetAppLib::JoinFesta::JoinFestaAttractionDefine::SaveData* JoinFestaDataSave::GetAttractionData( const u8 index )
{
  return &m_body.m_attractionData[ index ];
}
#endif

#if PM_DEBUG
/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  デバッグ用：アトラクション構造体データ設定
 *  @param  attractionId    アトラクションID
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetAttractionData( const u8 index, NetAppLib::JoinFesta::JoinFestaAttractionDefine::SaveData* pData )
{
  gfl2::std::MemCopy( pData, &m_body.m_attractionData[ index ], sizeof(NetAppLib::JoinFesta::JoinFestaAttractionDefine::SaveData) );
}
#endif

/************************************************************************/
/*  ブラックリスト用                                                    */
/************************************************************************/
/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  ブラックリスト追加（空いている場所に自動追加）
 *  @param  addPersonalData    追加するパーソナルデータ
 *  @return 追加できた場合true
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaDataSave::AddBlackList( const NetAppLib::JoinFesta::JoinFestaPersonalData& addPersonalData )
{
  // 押し出しチェック
  if( GetBlackListNum() ==  NetAppLib::JoinFesta::JoinFestaDefine::BLACK_LIST_MAX )
  {
    // 最も古いものを押し出し。
    DeleteBlackList( m_body.m_blackList[0].GetJoinFestaFriendKey() );
    ICHI_PRINT(">ICHI 押し出し削除\n");
  }

  for( u32 i=0; i<NetAppLib::JoinFesta::JoinFestaDefine::BLACK_LIST_MAX; i++ )
  {
    // 使用されていない場合追加
    if( m_body.m_blackList[ i ].isUse == false )
    {
      // パーソナルからブラックリストデータ作成
      m_body.m_blackList[ i ] = NetAppLib::JoinFestaUI::JoinFestaBlackPlayerData::CoreData( addPersonalData );
      // 使用フラグON
      m_body.m_blackList[ i ].isUse = true;

      GFL_PRINT(">JFSAVE ブラックリスト追加 index=%d \n", i);
      return true;
    }
  }
  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  ブラックリスト削除
 *  @param  deleteFriendKey    削除するプレイヤーデータのフレンドキー
 *  @return 削除できた場合true
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaDataSave::DeleteBlackList( const JoinFestaFriendKey deleteFriendKey )
{
  for( u32 i=0; i<NetAppLib::JoinFesta::JoinFestaDefine::BLACK_LIST_MAX; i++ )
  {
    // 使用されているかつフレンドキーが同じ場合削除
    if( m_body.m_blackList[ i ].isUse == true &&
        NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey( m_body.m_blackList[ i ].GetJoinFestaFriendKey(), deleteFriendKey ) )
    {
      // @fix Contact#525 おことわりリストは詰める
      u32 j;
      for( j=i+1; j<NetAppLib::JoinFesta::JoinFestaDefine::BLACK_LIST_MAX; j++ )
      {
        if( m_body.m_blackList[ j ].isUse == true )
        {
          m_body.m_blackList[ j - 1 ] = m_body.m_blackList[ j ];
        }
        else
        {
          break;
        }
      }
      // パーソナルデータ初期化
      m_body.m_blackList[ j - 1 ] = NetAppLib::JoinFestaUI::JoinFestaBlackPlayerData::CoreData();
      // 使用フラグOFF
      m_body.m_blackList[ j - 1 ].isUse = false;
      GFL_PRINT(">JFSAVE ブラックリスト削除 index=%d \n", j-1);

      return true;
    }
  }
  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  ブラックリストに追加されている人数取得
 *  @return ブラックリストに登録されている人数合計
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaDataSave::GetBlackListNum() const
{
  u32 num = 0;
  for( u32 i=0; i<NetAppLib::JoinFesta::JoinFestaDefine::BLACK_LIST_MAX; i++ )
  {
    const NetAppLib::JoinFestaUI::JoinFestaBlackPlayerData::CoreData* pBlackPlayerData = GetBlackListPlayerData(i);

    // 使用されているかつフレンドキーが有効な値な場合カウント
    if( pBlackPlayerData != NULL )
    {
      num++;
    }
  }
  ICHI_PRINT(">JFSAVE ブラックリスト人数 %d \n", num);
  return num;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  ブラックリストプレイヤーデータ取得
 *  @param  index    取得するプレイヤーデータのインデックス
 *  @return ブラックプレイヤーデータ（登録されていない場合NULL）
 */
/* ----------------------------------------------------------------------------------------- */
const NetAppLib::JoinFestaUI::JoinFestaBlackPlayerData::CoreData* JoinFestaDataSave::GetBlackListPlayerData( const u32 index ) const
{
  const NetAppLib::JoinFestaUI::JoinFestaBlackPlayerData::CoreData* pBlackPlayerData = &m_body.m_blackList[ index ];

  // 使用されているかつフレンドキーが有効な値な場合カウント
  if( pBlackPlayerData->isUse == true &&
    NetAppLib::JoinFesta::JoinFestaPacketUtil::IsValidFriendKey( pBlackPlayerData->GetJoinFestaFriendKey() )
    )
  {
    return pBlackPlayerData;
  }

  return NULL;
}


/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  ブラックリストに含まれているフレンドキーかチェック
 *  @param  targetFriendKey    チェックするプレイヤーデータのフレンドキー
 *  @return ブラックリストに含まれているならばtrue
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaDataSave::IsBlackList( const JoinFestaFriendKey targetFriendKey ) const
{
  for( u32 i=0; i<NetAppLib::JoinFesta::JoinFestaDefine::BLACK_LIST_MAX; i++ )
  {
    const NetAppLib::JoinFestaUI::JoinFestaBlackPlayerData::CoreData* pBlackPlayerData = GetBlackListPlayerData(i);

    // 使用されているかつフレンドキーが同じ場合
    if( pBlackPlayerData != NULL &&
        NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey( pBlackPlayerData->GetJoinFestaFriendKey(), targetFriendKey ) )
    {
      return true;
    }
  }
  return false;
}

/************************************************************************/
/* 定型コメント用                                                       */
/************************************************************************/
/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  定型コメント開放チェック
 *  @param  messageId    定型文のメッセージID
 *  @return trueで開放済み
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaDataSave::IsUseSelectWord( const u32 messageId ) const
{
  // 開放するメッセージかどうかチェック
  int msgIndex = NetAppLib::JoinFesta::JoinFestaSelectWordTable::IsOpenMessageId( messageId );

  // 開放するメッセージでない場合は、無条件で開放済みとする
  if( msgIndex == -1 ) { return true; }

  GFL_ASSERT( msgIndex < NetAppLib::JoinFesta::JoinFestaSelectWordTable::OPEN_MESSAGE_NUM );
  if( msgIndex < NetAppLib::JoinFesta::JoinFestaSelectWordTable::OPEN_MESSAGE_NUM )
  {
    return m_body.m_isUseSelectWord[ msgIndex ];
  }
  return false; 
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  定型コメント開放する
 *  @param  messageId    定型文のメッセージID
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetUseSelectWord( const u32 messageId )
{
  // 開放するメッセージかどうかチェック
  int msgIndex = NetAppLib::JoinFesta::JoinFestaSelectWordTable::IsOpenMessageId( messageId );

  // 開放するメッセージでない場合は、何もしない
  if( msgIndex == -1 ) { return; }

  GFL_ASSERT( msgIndex < NetAppLib::JoinFesta::JoinFestaSelectWordTable::OPEN_MESSAGE_NUM );
  if( msgIndex < NetAppLib::JoinFesta::JoinFestaSelectWordTable::OPEN_MESSAGE_NUM )
  {
    m_body.m_isUseSelectWord[ msgIndex ] = true;;
    GFL_PRINT(">JFSAVE 定型文開放 index=%d \n", msgIndex);
  }
}


/************************************************************************/
/*  グローバルアトラクション用                                          */
/************************************************************************/
/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  グローバルアトラクションデータ設定
 *  @param[in]  index   インデックス（0-11)
 *  @param[in]  pData   グローバルアトラクションデータ
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetGlobalAttractionData( const u8 index, NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData* pData )
{
  GFL_ASSERT( index < NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::RECORD_MAX );
  if( index < NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::RECORD_MAX )
  {
    gfl2::std::MemCopy( pData, &m_body.m_globalAttractionData[ index ], sizeof(NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData) );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  グローバルアトラクションデータ取得
 *  @param[in]  index   インデックス（0-11)
 *  @return  グローバルアトラクションデータ
 */
/* ----------------------------------------------------------------------------------------- */
NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData* JoinFestaDataSave::GetGlobalAttractionData( const u8 index )
{
  if( index < NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::RECORD_MAX )
  {
    return &m_body.m_globalAttractionData[ index ];
  }
  GFL_ASSERT(0);
  return &m_body.m_globalAttractionData[ 0 ];
}

/************************************************************************/
/*  その他用                                                            */
/************************************************************************/
/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  交換の電源切断ペナルティ用カウント設定
 *  @param  count    ペナルティカウント
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetGradePenaltyCount( const u32 count )
{
  m_body.m_tradePenaltyCount = count;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  交換の電源切断ペナルティ用カウント取得
 *  @return ペナルティカウント
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaDataSave::GetGradePenaltyCount() const
{
  return m_body.m_tradePenaltyCount;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   P2Pで最後に遊んだ人をセット
 * @param[in]   personalData パーソナルデータ
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetLastP2pPersonal( const NetAppLib::JoinFesta::JoinFestaPersonalData& personalData )
{
  GFL_PRINT(">JFSAVE SetLastP2pPersonal \n");

  // @fix GFNMcat[1453]：同じ人物で何度もイベント発動するのを防ぐ
  if( !personalData.IsSameFriendKey( m_body.m_lastP2pPersonal.m_personalInfo.GetJoinFestaFriendKey() ) )
  {
    // 一致しない場合は、P2Pパーソナルイベント発生
    m_body.m_isFinishLastP2pPersonalEvent = false;
  }

  personalData.SerializeCoreData( &m_body.m_lastP2pPersonal );
  ICHI_PRINT(">JFSave P2P 通信経路 %d \n", m_body.m_lastP2pPersonal.m_commRoute );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   P2Pで最後に遊んだ人のパーソナルイベント終了
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::FinishLastP2pPersonalEvent()
{
  GFL_PRINT(">JFSAVE FinishLastP2pPersonalEvent \n");
  m_body.m_isFinishLastP2pPersonalEvent = true;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   P2Pで最後に遊んだ人を取得
 * @return  friendKey JoinFestaフレンドキー
 */
/* ----------------------------------------------------------------------------------------- */
NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData& JoinFestaDataSave::GetLastP2pPersonal()
{
  return m_body.m_lastP2pPersonal;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   P2Pで最後に遊んだ人のパーソナルイベント終了フラグを取得
 * @reval   false:イベント未発動
 * @reval   true:イベント終了した
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaDataSave::IsFinishLastP2pPersonalEvent() const
{
  return m_body.m_isFinishLastP2pPersonalEvent;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   プロフィールアプリチュートリアルフラグを設定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetProfileAppTutorialFlag()
{
  m_body.m_isProfileAppTutorial = true;
}
/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   プロフィールアプリチュートリアルフラグを取得
 * @return  trueでチュートリアル終了済み
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaDataSave::GetProfileAppTutorialFlag() const
{
  return m_body.m_isProfileAppTutorial;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   プレイヤーリストアプリチュートリアルフラグを設定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetPlayerListShowAppTutorialFlag()
{
  m_body.m_isPlayerListShowAppTutorial = true;
}
/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   プレイヤーリストアプリチュートリアルフラグを取得
 * @return  trueでチュートリアル終了済み
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaDataSave::GetPlayerListShowAppTutorialFlag() const
{
  return m_body.m_isPlayerListShowAppTutorial;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   定型文選択アプリチュートリアルフラグを設定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetSelectWordAppTutorial()
{
  m_body.m_isSelectWordAppTutorial = true;
}
/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   定型文選択アプリチュートリアルフラグを取得
 * @return  trueでチュートリアル終了済み
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaDataSave::GetSelectWordAppTutorial() const
{
  return m_body.m_isSelectWordAppTutorial;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  プリセットNPC名前取得
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::GetPresetNpcNameString( const u32 sex, gfl2::str::STRCODE* dst ) const
{
  if( sex < 2 )
  {
    gfl2::str::StrNCopy( dst, m_body.m_presetNpcName[ sex ], ::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN ); 
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  プリセットNPCイベント状態取得
 */
/* ----------------------------------------------------------------------------------------- */
u8 JoinFestaDataSave::GetPresetNpcEventState( const u8 id ) const
{
  if( id < NetApp::JoinFesta::JoinFestaDefine::PRESET_ID_MAX )
  {
    return m_body.m_presetNpcEventState[ id ];
  }
  GFL_ASSERT_MSG(0,"%d",id);
  return 0;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  プリセットNPCイベントID取得
 */
/* ----------------------------------------------------------------------------------------- */
u8 JoinFestaDataSave::GetPresetNpcEventId( const u8 id ) const
{
  if( id < NetApp::JoinFesta::JoinFestaDefine::PRESET_ID_MAX )
  {
    return m_body.m_presetNpcEventId[ id ];
  }
  GFL_ASSERT_MSG(0,"%d",id);
  return 0;
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  プリセットNPCイベントID設定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetPresetNpcEventId( const u8 id, const u8 personalEventId )
{
  GFL_ASSERT( id < NetApp::JoinFesta::JoinFestaDefine::PRESET_ID_MAX );
  if( id < NetApp::JoinFesta::JoinFestaDefine::PRESET_ID_MAX )
  {
    // 抽選済みに変更する
    m_body.m_presetNpcEventId[ id ] = personalEventId;
    m_body.m_presetNpcEventState[ id ] = static_cast<u8>(NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::STATE_LOTTERY_OK );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 *  @brief  プリセットNPCイベント終了に変更
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::FnishPresetNpcEvent( const u8 id )
{
  GFL_ASSERT( id < NetApp::JoinFesta::JoinFestaDefine::PRESET_ID_MAX );
  if( id < NetApp::JoinFesta::JoinFestaDefine::PRESET_ID_MAX )
  {
    // イベント終了状態に変更する
    m_body.m_presetNpcEventState[ id ] = static_cast<u8>(NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::STATE_FINISH );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief 人物話しかけのカメラ決め用カウンタ取得
 */
/* ----------------------------------------------------------------------------------------- */
u8 JoinFestaDataSave::GetPersonalTalkCameraCount()
{
  return m_personalTalkCameraCount;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief 人物話しかけのカメラ決め用カウンタ加算
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::AddPersonalTalkCameraCount()
{
  m_personalTalkCameraCount++;
  if( m_personalTalkCameraCount > 2 )
  {
    m_personalTalkCameraCount = 0;
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief エアバルーンの注意喚起メッセージスキップフラグ
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetIsSkipMessageAirToy( bool flag )
{
  m_isSkipMessageAirToy = flag;
}
bool JoinFestaDataSave::GetIsSkipMessageAirToy() const
{
  return m_isSkipMessageAirToy;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief 屋台の注意喚起メッセージスキップフラグ
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetIsSkipMessageStall( bool flag )
{
  m_isSkipMessageStall = flag;
}
bool JoinFestaDataSave::GetIsSkipMessageStall() const
{
  return m_isSkipMessageStall;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief ホラーハウスの注意喚起メッセージスキップフラグ
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetIsSkipMessageHorrorHouse( bool flag )
{
  m_isSkipMessageHorrorHouse = flag;
}
bool JoinFestaDataSave::GetIsSkipMessageHorrorHouse() const
{
  return m_isSkipMessageHorrorHouse;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief 現在のテーマ一時保存用
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::SetTempSaveTheme( u8 theme )
{
  m_tempSaveTheme = theme;
}
u8 JoinFestaDataSave::GetTempSaveTheme() const
{
  return m_tempSaveTheme;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief 施設の初期値を設定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaDataSave::InitializeDefaultFacilities()
{
#if defined(GF_PLATFORM_CTR)
  u64 transferdId = nn::cfg::CTR::GetTransferableId( GameSys::GameManager::APLICATION_UNIQUE_ID );
#else
  u64 transferdId = System::GflUse::GetPublicRand();
#endif

  // ROMVersion
  const bool isSun = (GET_VERSION() == VERSION_SUN2 );

  // 施設のパターン（ROMバージョンで分かれる）
  const u8 lotteryA   = NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_LOTTERY_00;
  const u8 air_toyA   = NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_AIR_A_00;
  const u8 fun_houseA = NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_HAPPENING_00;
  const u8 stallA     = NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_KITCHEN_A_00;
  const u8 gift_shopA = NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_GIFT_A_00;
  const u8 fortuneA   = NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_FORTUNETELLING_ALL_00;
  const u8 dyeingA    = isSun ? NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_RED_00 : NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_YELLOW_00;
  const u8 lotteryB   = NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_LOTTERY_05;
  const u8 air_toyB   = NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_AIR_A_05;
  const u8 fun_houseB = NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_HAPPENING_05;
  const u8 stallB     = NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_KITCHEN_A_05;
  const u8 gift_shopB = NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_GIFT_A_03;
  const u8 fortuneB   = isSun ? NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_FORTUNETELLING_GS_00 : NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_FORTUNETELLING_RGB_00;
  const u8 dyeingB    = isSun ? NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_GREEN_00 : NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_BLUE_00;
  const u8 lotteryC   = NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_LOTTERY_10;
  const u8 air_toyC   = NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_AIR_B_00;
  const u8 fun_houseC = NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_HAPPENING_10;
  const u8 stallC     = NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_KITCHEN_B_00;
  const u8 gift_shopC = NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_GIFT_B_03;
  const u8 fortuneC   = isSun ? NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_FORTUNETELLING_XY_00 : NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_FORTUNETELLING_BW_00;
  const u8 dyeingC    = isSun ? NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_ORANGE_00 : NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_NAVY_00;


  // トランスファードIDを7で割った余りで決まる
  const u32 tblIndex = transferdId % 7; 
  ICHI_PRINT(">ICHI 初期施設のテーブル %d \n", tblIndex);

  u8 facilitiesIdTbl[7][JoinFestaScript::FACILITIES_INDEX_MAX] = {
    { lotteryA,   air_toyA,   fun_houseA, lotteryB,   air_toyB,   fun_houseB, stallC },
    { air_toyA,   fun_houseA, stallA,     air_toyB,   fun_houseB, stallB,     gift_shopC },
    { fun_houseA, stallA,     gift_shopA, fun_houseB, stallB,     gift_shopB, fortuneC },
    { stallA,     gift_shopA, fortuneA,   stallB,     gift_shopB, fortuneB,   dyeingC },
    { gift_shopA, fortuneA,   dyeingA,    gift_shopB, fortuneB,   dyeingB,    lotteryC },
    { fortuneA,   dyeingA,    lotteryA,   fortuneB,   dyeingB,    lotteryB,   air_toyC },
    { dyeingA,    lotteryA,   air_toyA,   dyeingB,    lotteryB,   air_toyB,   fun_houseC }
  };

  // 施設の初期値
  for( u8 i=0; i<JoinFestaScript::FACILITIES_INDEX_MAX; i++ )
  {
    const u8 facilitiesId = facilitiesIdTbl[tblIndex][i];
    m_body.m_facilities[i].facilitiesId = facilitiesId;
    ICHI_PRINT(">ICHI 初期施設[%d]:%d \n", i, facilitiesId );
    m_body.m_facilities[i].playerFlag = false;
    m_body.m_facilities[i].randomSeed = System::GflUse::GetPublicRand();
    m_body.m_facilities[i].characterId = System::GflUse::GetPublicRand( NetAppLib::JoinFesta::JoinFestaFacilitiesUtil::CHARACTER_TYPE_MAX );

    // 染物屋は色指定
    if( NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_RED_00 <= facilitiesId &&
        facilitiesId <= NetApp::JoinFesta::ShopItemInfoTable::SHOPITEM_DYE_PINK_02 )
    {
      m_body.m_facilities[i].color = NetAppLib::JoinFesta::JoinFestaFacilitiesUtil::ConvertDyeingColor( facilitiesId );
    }
    else
    {
      m_body.m_facilities[i].color = JoinFestaScript::FACILITIES_COLOR_1;
    }
  }
}

GFL_NAMESPACE_END(Savedata)

