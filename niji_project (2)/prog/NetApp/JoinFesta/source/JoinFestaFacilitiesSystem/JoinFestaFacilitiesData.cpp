//==============================================================================
/**
 * @file	JoinFestaFacilitiesData.cpp
 * @brief	ジョインフェスタ施設モデル
 * @author	ichiraku_katsuhiko
 * @date	2015/10/22　12:52
 */
// =============================================================================
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesData.h"

// ライブラリ
#include <base/include/gfl2_Singleton.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <str/include/gfl2_StrBuf.h>
#include <util/include/gfl2_List.h>

// system
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// Save
#include "Savedata/include/JoinFestaDataSave.h"

// モデルのインデックス
#include <niji_conv_header/field/chara/chara_model_id.h>

// ジョインフェスタデータテーブル
#include "NetApp/JoinFesta/include/JoinFestaDataTable/JoinFestaDataTable.h"
#include "NetApp/JoinFesta/include/JoinFestaDataTable/Info/JoinFestaShopItemInfo.h"
#include "NetApp/JoinFesta/include/JoinFestaDataTable/Info/JoinFestaLotInfo.h"

// パーソナルデータ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"
// 施設ユーティリティ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaFacilitiesUtil/JoinFestaFacilitiesUtil.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)

// コンストラクタ
JoinFestaFacilitiesData::JoinFestaFacilitiesData( JoinFestaScript::FacilitiesIndex facilitiesIndex, const JoinFestaDataTable* pDataTable ) :
  m_pShopItemInfo(NULL)
  ,m_pLotInfo(NULL)
  ,m_pCoreData(NULL)
{
  STATIC_ASSERT( (sizeof(CoreData) == 72) );

  // 初期化
  InitializeSetup( facilitiesIndex, pDataTable ); 

#if defined( DEBUG_ONLY_FOR_ichiraku_katsuhiko )
  //m_pShopItemInfo->Dump();
#endif
}

// デストラクタ
JoinFestaFacilitiesData::~JoinFestaFacilitiesData()
{
}


//=============================================================================
/**
 *  publicメソッド
 */
//=============================================================================
//----------------------------------------------------------------------------
// データテーブルのデータ取得系
//----------------------------------------------------------------------------
/**
 *  @brief  施設ランクの取得
 */
s32 JoinFestaFacilitiesData::GetRank() const
{
  return m_pShopItemInfo->rank;
}

/**
 *  @brief  施設種類の取得
 */
JoinFestaScript::FacilitiesType JoinFestaFacilitiesData::GetFacilitiesType() const
{
  //ICHI_PRINT("JoinFestaFacilitiesData::GetFacilitiesType %d \n",m_pShopItemInfo->shopType);
  return static_cast<JoinFestaScript::FacilitiesType>(m_pShopItemInfo->shopType);
}

/**
 *  @brief  店名のラベル取得
 */
u32 JoinFestaFacilitiesData::GetShopNameMsgId() const
{
  return m_pShopItemInfo->messageID_ShopName;
}

/**
 *  @brief  店説明のラベル
 */
u32 JoinFestaFacilitiesData::GetShopDescriptionMsgId() const
{
  return m_pShopItemInfo->messageID_Description;
}

/**
 *  @brief  サービス取得
 */
u32 JoinFestaFacilitiesData::GetServiceID( int slot ) const
{
  // 登録チェック
  if( m_pShopItemInfo->IsServiceAvailable( slot ) )
  {
    return m_pShopItemInfo->GetServiceID( slot );
  }
  GFL_ASSERT(0);
  return 0;
}



//----------------------------------------------------------------------------
// セーブのデータ系
//----------------------------------------------------------------------------
/**
 *  @brief  紹介者のセット
 */
void JoinFestaFacilitiesData::SetIntroducer( NetAppLib::JoinFesta::JoinFestaPersonalData *personal )
{
  m_pCoreData->playerFlag = true;

  // プレイヤー名
  personal->GetName( m_pCoreData->playerName );
  // プレイヤー性別
  m_pCoreData->playerSex = personal->GetSex();

  // 定型文
  m_pCoreData->patternHello   = personal->GetPattern( JoinFestaScript::PATTERN_HELLO );
  m_pCoreData->patternParting = personal->GetPattern( JoinFestaScript::PATTERN_PARTING );
  m_pCoreData->patternEmotion = personal->GetPattern( JoinFestaScript::PATTERN_EMOTION );
  m_pCoreData->patternRegret  = personal->GetPattern( JoinFestaScript::PATTERN_REGRET );

  // フレンドキーもコピー
  JoinFestaFriendKey friendKey =  personal->GetJoinFestaFriendKey();
  m_pCoreData->myPrincipalId = friendKey.principalId;
  m_pCoreData->transferdId = friendKey.localFriendCode;
}

//! 施設IDの取得
u8  JoinFestaFacilitiesData::GetFacilitiesId() const
{
  return m_pCoreData->facilitiesId;
}

//! 施設カラーの取得
u8  JoinFestaFacilitiesData::GetFacilitiesColor() const
{
  return m_pCoreData->color;
}

/**
 *  @brief  店員のキャラモデルID取得
 */
u32 JoinFestaFacilitiesData::GetCharaModelId() const
{
  return NetAppLib::JoinFesta::JoinFestaFacilitiesUtil::GetCharaModelIdFromCharacterId( m_pCoreData->characterId );
}

/**
 *  @brief  店員のキャラクターID取得
 */
u32 JoinFestaFacilitiesData::GetCharacterId() const
{
  return m_pCoreData->characterId;
}

/**
 *  @brief  店員の性別
 */
u8  JoinFestaFacilitiesData::GetAssistantSex() const
{
  return NetAppLib::JoinFesta::JoinFestaFacilitiesUtil::GetCharaSexFromCharacterId( m_pCoreData->characterId );
}

/**
 *  @brief  施設紹介者がいるかどうか
 *  @return  trueでいる
 */
bool JoinFestaFacilitiesData::IsPlayerFlag() const
{
  return m_pCoreData->playerFlag;
}
/**
 *  @brief  施設紹介者の名前を取得
 */
const gfl2::str::STRCODE* JoinFestaFacilitiesData::GetPlayerName() const
{
  return m_pCoreData->playerName;
}
/**
 *  @brief  施設紹介者の性別を取得
 */
u8  JoinFestaFacilitiesData::GetPlayerSex() const
{
  return m_pCoreData->playerSex;
}
/**
 *  @brief  施設紹介者の定型文を取得
 */
u16 JoinFestaFacilitiesData::GetPlayerPattern( JoinFestaScript::PatternType pattern ) const
{
  switch( pattern ){
  case JoinFestaScript::PATTERN_HELLO:   return m_pCoreData->patternHello;
  case JoinFestaScript::PATTERN_PARTING: return m_pCoreData->patternParting;
  case JoinFestaScript::PATTERN_EMOTION: return m_pCoreData->patternEmotion;
  case JoinFestaScript::PATTERN_REGRET:  return m_pCoreData->patternRegret;
  }
  GFL_ASSERT(0);
  return 0;
}

/**
 *  @brief  占い結果：当たり取得
 */
u8 JoinFestaFacilitiesData::GetFortuneResultHit() const
{
  return m_pCoreData->fortuneResultHit;
}

/**
 *  @brief  占い結果：行動取得
 */
u8 JoinFestaFacilitiesData::GetFortuneResultAction() const
{
  return m_pCoreData->fortuneResultAction;
}

/**
 *  @brief  こうかんじょの残り回数取得
 */
u8 JoinFestaFacilitiesData::GetTradeRemainCount() const
{
  return m_pCoreData->tradeRemainCount;
}

/**
 *  @brief  こうかんじょの残り回数減算
 */
void JoinFestaFacilitiesData::SubTradeRemainCount()
{
  GFL_ASSERT( m_pCoreData->tradeRemainCount > 0 );
  if( m_pCoreData->tradeRemainCount > 0 )
  {
    m_pCoreData->tradeRemainCount--;
  }
}


/**
 *  @brief  乱数の種取得
 */
u32 JoinFestaFacilitiesData::GetRandomSeed() const
{
  return m_pCoreData->randomSeed;
}

/**
 *  @brief  乱数の種取得
 */
void JoinFestaFacilitiesData::ResetRandomSeed()
{
  m_pCoreData->randomSeed = System::GflUse::GetPublicRand();
}

/**
 *  @brief  抽選テーブルから乱数で抽選する
 */
u32 JoinFestaFacilitiesData::LotteryExecute( gfl2::math::Random* random ) const
{
  if( m_pLotInfo == NULL )
  {
    GFL_ASSERT(0);
    return 0;
  }

  gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  gfl2::util::List<const LotInfo::Item*> lotInfoList( pHeap->GetLowerHandle(), LotInfo::SLOT_MAX ); // 抽選対象アイテムリスト
  u32 totalWeight = 0; // 合計確率

  // 抽選テーブルのアイテム数分回す
  for( u32 i=0; i<LotInfo::SLOT_MAX; i++ )
  {
    if( m_pLotInfo->item[i].probabilityWeight > 0 )
    {
      // 抽選対象に加える
      lotInfoList.PushBack( &m_pLotInfo->item[i] );


      // 合計確率加算
      totalWeight += m_pLotInfo->item[i].probabilityWeight;
    }
  }
  //ICHI_PRINT(">ICHI 抽選対象アイテム数:%d \n", lotInfoList.Size() );

#if PM_DEBUG
  // デバッグ用：強制で特定スロット抽選した事にする
  if( m_DebugForceSlot > 0 )
  {
    // スロット1は0なので合わせる
    u32 slot = m_DebugForceSlot - 1;
    if( m_pLotInfo->item[slot].probabilityWeight > 0 )
    {
      return m_pLotInfo->item[slot].itemID;
    }
  }
#endif // PM_DEBUG



  // 乱数抽選
  const u32 randomValue = random->Next( totalWeight );
  //ICHI_PRINT(">ICHI 抽選乱数:%d \n", randomValue );


  u32 targetWeight = 0;
  for( gfl2::util::List<const LotInfo::Item*>::Iterator it = lotInfoList.Begin(); it != lotInfoList.End(); it++ )
  {
    targetWeight += (*it)->probabilityWeight;
    if( randomValue < targetWeight )
    {
      //ICHI_PRINT(">ICHI 抽選アイテムID:%d \n", (*it)->itemID );
      return (*it)->itemID;
    }
  }
  GFL_ASSERT(0);
  return 0;
}


/**
 *  @brief  アイテムIDで抽選アイテムのインデックスを取得
 */
u32 JoinFestaFacilitiesData::GetLotInfoIndexByItemId( const u32 itemId ) const
{
  if( m_pLotInfo == NULL )
  {
    GFL_ASSERT(0);
    return 0;
  }

  for( u32 i=0; i<LotInfo::SLOT_MAX; i++ )
  {
    if( m_pLotInfo->item[ i ].itemID == itemId )
    {
      return i;
    }
  }

  GFL_ASSERT(0);
  return 0;
}

/**
 *  @brief  ビックリハウス（ホラーハウス）の抽選
 *  @return 当たったアイテム（無い場合は0）
 */
void JoinFestaFacilitiesData::LotteryFunHouse( u32 (&outHitItem)[3] )
{
  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

  // 抽選用乱数作成
  gfl2::math::Random random;
  random.Initialize( GetRandomSeed() );

  // アイテム個数抽選
  u32 hitItemNum = 1;
  {
    // 個数抽選確率（3個、2個）
    u8 numWeight[2] = { 7, 30 };

    // 占い屋の結果によって確率変える
    switch( pSave->GetFortuneResultHitByAction( JoinFestaScript::FORTUNE_RESULT_ACTION_FUN_HOUSE ) ){
    case JoinFestaScript::FORTUNE_RESULT_HIT_SMALL:         //! 当たり
      numWeight[0] = 14; // 14%
      numWeight[1] = 44; // 30%
      break;
    case JoinFestaScript::FORTUNE_RESULT_HIT_MIDDLE:        //! 中当たり
      numWeight[0] = 21; // 21%
      numWeight[1] = 58; // 37%
      break;
    case JoinFestaScript::FORTUNE_RESULT_HIT_BIG:           //! 大当たり
      numWeight[0] = 35; // 35%
      numWeight[1] = 86; // 51%
      break;
    }

    const u32 randomValue = random.Next( 100 );

    if( randomValue < numWeight[0] )      { hitItemNum = 3; }
    else if( randomValue < numWeight[1] ) { hitItemNum = 2; }
    else                                  { hitItemNum = 1; }
  }

  // 初期化
  outHitItem[0] = 0;
  outHitItem[1] = 0;
  outHitItem[2] = 0;

  // 当たるアイテムの抽選
  for( u32 itemCount=0; itemCount<hitItemNum; itemCount++ )
  {
    outHitItem[ itemCount ] = LotteryExecute( &random );
  }

  // 乱数シードリセット
  ResetRandomSeed();
}

/**
 *  @brief  くじやの再挑戦出来るか抽選
 *  @terval  true:再挑戦出来る
 *  @terval  false:再挑戦出来ない
 */
bool JoinFestaFacilitiesData::IsRetryDrawLot( gfl2::math::Random* random ) const
{
  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();
  u32 retryWeight = 10; // 確率10%

  // 占い屋の結果によって確率変える
  switch( pSave->GetFortuneResultHitByAction( JoinFestaScript::FORTUNE_RESULT_ACTION_LOTTERY ) ){
  case JoinFestaScript::FORTUNE_RESULT_HIT_SMALL:         //! 当たり
    retryWeight += 20;
    break;
  case JoinFestaScript::FORTUNE_RESULT_HIT_MIDDLE:        //! 中当たり
    retryWeight += 30;
    break;
  case JoinFestaScript::FORTUNE_RESULT_HIT_BIG:           //! 大当たり
    retryWeight += 50;
    break;
  }

  if( retryWeight > random->Next( 100 ) )
  {
    return true;
  }
  return false;
}

/**
 *  @brief  うらないやの[当たり・中あたり・大当たり]の抽選
 *  @return 当たり結果
 */
JoinFestaScript::FortuneResultHit JoinFestaFacilitiesData::LotteryFortuneResultHit( gfl2::math::Random* random ) const
{
  u32 hitRandom = random->Next(10);
  if( hitRandom < 1 )       { return JoinFestaScript::FORTUNE_RESULT_HIT_BIG; }
  else if(  hitRandom < 3 ) { return JoinFestaScript::FORTUNE_RESULT_HIT_MIDDLE; }
  else                      { return JoinFestaScript::FORTUNE_RESULT_HIT_SMALL; }
}

/**
 *  @brief  うらないやの当たり行動の抽選
 *  @return 当たり行動結果
 */
JoinFestaScript::FortuneResultAction JoinFestaFacilitiesData::LotteryFortuneResultAction( gfl2::math::Random* random ) const
{
  // @fix GFNMcat[1514]：１オリジンではあるがenum最大値は0番を含んでいたので-1しないと最大値になってしまう
  u32 actionRandom = random->Next( JoinFestaScript::FORTUNE_RESULT_ACTION_MAX - 1 ) + 1; // 1オリジン
  return static_cast<JoinFestaScript::FortuneResultAction>(actionRandom);
}

#if PM_DEBUG
/**
 *  @brief  強制初期化
 */
void JoinFestaFacilitiesData::DEBUG_ForceInitialize( JoinFestaScript::FacilitiesIndex facilitiesIndex, const JoinFestaDataTable* pDataTable )
{
  InitializeSetup( facilitiesIndex, pDataTable ); 
}
#endif // PM_DEBUG
  

//=============================================================================
/**
 *  privateメソッド
 */
//=============================================================================
/**
 *  @brief  初期化
 */
void JoinFestaFacilitiesData::InitializeSetup( JoinFestaScript::FacilitiesIndex facilitiesIndex, const JoinFestaDataTable* pDataTable )
{
  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

  // データのポインタ保存
  m_pCoreData = pSave->GetFacilitiesData( facilitiesIndex );
  m_pShopItemInfo = pDataTable->GetShopItemInfo( m_pCoreData->facilitiesId ); 

  // くじや、ビックリハウス、うらないやは、抽選テーブルがある
  switch( GetFacilitiesType() ){
  case JoinFestaScript::FACILITIES_TYPE_LOTTERY:    //! くじ引き屋
  case JoinFestaScript::FACILITIES_TYPE_FUN_HOUSE:  //! ビックリハウス
  case JoinFestaScript::FACILITIES_TYPE_FORTUNE:    //! 占い
    {
      // サービス0番目に抽選テーブルIDが入っている
      u32 lotId = m_pShopItemInfo->GetServiceID( 0 );
      m_pLotInfo = pDataTable->GetLotInfo( lotId ); 
      GFL_ASSERT(m_pLotInfo); // @fix 開発中のアサート
    }
    break;
  }
}


GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

