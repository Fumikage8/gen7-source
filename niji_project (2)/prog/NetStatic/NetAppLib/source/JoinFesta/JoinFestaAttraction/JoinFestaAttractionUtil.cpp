//==============================================================================
/**
 * @file	JoinFestaAttractionUtil.cpp
 * @brief	ジョインフェスタアトラクションユーティリティ
 * @author	ichiraku_katsuhiko
 * @date	2016/02/04　12:47
 */
// =============================================================================
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionUtil.h"

// ライブラリ
#include <base/include/gfl2_Singleton.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_FixedSizeContainer.h>

// niji
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "System/include/HeapDefine.h"
#include "System/include/GflUse.h"

// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "niji_conv_header/field/FieldGimmickTypes.h"

// ジョインフェスタギミック
#include "Field/FieldRos/FieldGimmickJoinFesta/include/FieldGimmickJoinFesta.h"
// ジョインフェスタリソースマネージャ
#include "NetApp/JoinFesta/include/JoinFestaResourceManager/JoinFestaResourceManager.h"
// ジョインフェスタパーソナルデータユーティリティ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataUtil.h"

// JoinFesta
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalEvent/JoinFestaPersonalEvent.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaSelectWordTable.h"
// Excelのデータ
#include "NetApp/JoinFesta/include/JoinFestaDataTable/JoinFestaDataTable.h"

// Save
#include "Savedata/include/JoinFestaDataSave.h"
#include "Savedata/include/MyStatus.h"

// msgId
#include  <niji_conv_header/message/msg_jf_phrase.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFesta)

// 眼鏡のグッズ
const u32  JoinFestaAttractionUtil::m_acceyeTable[2][5] = {
  // 男
  { DRESSUP_ITEM_ID_P1_ACCEYE_METAL02, DRESSUP_ITEM_ID_P1_ACCEYE_METAL01, DRESSUP_ITEM_ID_P1_ACCEYE_SUNGLASS01_WH, DRESSUP_ITEM_ID_P1_ACCEYE_WELL01_BK, DRESSUP_ITEM_ID_P1_ACCEYE_WELL01_WH },
  // 女
  { DRESSUP_ITEM_ID_P2_ACCEYE_WELL01_PP, DRESSUP_ITEM_ID_P2_ACCEYE_BUTTERFLY02, DRESSUP_ITEM_ID_P2_ACCEYE_SUNGLASS01_WH, DRESSUP_ITEM_ID_P2_ACCEYE_WELL01_BK, DRESSUP_ITEM_ID_P2_ACCEYE_BUTTERFLY03 },
};

// バッグのグッズ
const u32  JoinFestaAttractionUtil::m_bagTable[2][5] = {
  // 男
  { DRESSUP_ITEM_ID_P1_BAG_DRUCK01, DRESSUP_ITEM_ID_P1_BAG_RUCK01_RD2, DRESSUP_ITEM_ID_P1_BAG_LRUCK01_GR, DRESSUP_ITEM_ID_P1_BAG_SRUCK01_OR2, DRESSUP_ITEM_ID_P1_BAG_LRUCK01_NB1 },
  // 女
  { DRESSUP_ITEM_ID_P2_BAG_DBAG01, DRESSUP_ITEM_ID_P2_BAG_LBAG01_PN2, DRESSUP_ITEM_ID_P2_BAG_SBAG01_PP1, DRESSUP_ITEM_ID_P2_BAG_FBAG01_YE, DRESSUP_ITEM_ID_P2_BAG_BAG01_WH },
};

// 靴のグッズ
const u32  JoinFestaAttractionUtil::m_shoesTable[2][5] = {
  // 男
  { DRESSUP_ITEM_ID_P1_SHOES_LOAFER_PLOAFER09, DRESSUP_ITEM_ID_P1_SHOES_LOAFER_PLOAFER05,	DRESSUP_ITEM_ID_P1_SHOES_LOAFER_PLOAFER07, DRESSUP_ITEM_ID_P1_SHOES_LOAFER_PLOAFER06,	DRESSUP_ITEM_ID_P1_SHOES_DSHOES01 },
  // 女
  { DRESSUP_ITEM_ID_P2_SHOES_DSHOES01, DRESSUP_ITEM_ID_P2_SHOES_OPERA_OPERA01_GR1, DRESSUP_ITEM_ID_P2_SHOES_OPERA_POPERA01, DRESSUP_ITEM_ID_P2_SHOES_OPERA_OPERA01_RD3,	DRESSUP_ITEM_ID_P2_SHOES_LOWCUT_SLOWCUT01_BL }
};

const u16 JoinFestaAttractionUtil::m_patternTable[ JoinFestaAttractionUtil::RANDOM_PATTERN_TABLE_MAX ] = {
  msg_jf_phrase_m_021, msg_jf_phrase_m_022, msg_jf_phrase_m_023, msg_jf_phrase_m_024, msg_jf_phrase_m_025, msg_jf_phrase_m_026, msg_jf_phrase_m_027, msg_jf_phrase_m_028,
  msg_jf_phrase_m_045, msg_jf_phrase_m_046, msg_jf_phrase_m_047, msg_jf_phrase_m_048, msg_jf_phrase_m_049, msg_jf_phrase_m_050, msg_jf_phrase_m_051, msg_jf_phrase_m_052,
  msg_jf_phrase_m_053, msg_jf_phrase_m_054, msg_jf_phrase_m_055, msg_jf_phrase_m_056, msg_jf_phrase_m_057, msg_jf_phrase_m_058, msg_jf_phrase_m_059, msg_jf_phrase_m_060,
  msg_jf_phrase_m_064, msg_jf_phrase_m_065, msg_jf_phrase_m_066, msg_jf_phrase_m_067, msg_jf_phrase_m_068,
  msg_jf_phrase_m_069, msg_jf_phrase_m_070, msg_jf_phrase_m_071, msg_jf_phrase_m_072, msg_jf_phrase_m_073, msg_jf_phrase_m_074, msg_jf_phrase_m_075, msg_jf_phrase_m_076,
  msg_jf_phrase_m_077, msg_jf_phrase_m_078, msg_jf_phrase_m_079, msg_jf_phrase_m_080, msg_jf_phrase_m_081, msg_jf_phrase_m_189, msg_jf_phrase_m_190, msg_jf_phrase_m_191,
  msg_jf_phrase_m_192, msg_jf_phrase_m_193, msg_jf_phrase_m_194, msg_jf_phrase_m_195, msg_jf_phrase_m_196, msg_jf_phrase_m_197, msg_jf_phrase_m_198, msg_jf_phrase_m_199,
  msg_jf_phrase_m_200, msg_jf_phrase_m_201, msg_jf_phrase_m_202, msg_jf_phrase_m_203, msg_jf_phrase_m_204, msg_jf_phrase_m_205, msg_jf_phrase_m_206, msg_jf_phrase_m_207,
  msg_jf_phrase_m_208, msg_jf_phrase_m_209, msg_jf_phrase_m_210, msg_jf_phrase_m_211, msg_jf_phrase_m_212, msg_jf_phrase_m_213, msg_jf_phrase_m_214, msg_jf_phrase_m_215,
  msg_jf_phrase_m_216, msg_jf_phrase_m_217, msg_jf_phrase_m_218, msg_jf_phrase_m_219, msg_jf_phrase_m_220, msg_jf_phrase_m_221, msg_jf_phrase_m_222, msg_jf_phrase_m_223,
  msg_jf_phrase_m_224, msg_jf_phrase_m_225, msg_jf_phrase_m_226, msg_jf_phrase_m_227, msg_jf_phrase_m_228, msg_jf_phrase_m_229, msg_jf_phrase_m_230, msg_jf_phrase_m_231,
  msg_jf_phrase_m_232, msg_jf_phrase_m_233, msg_jf_phrase_m_234, msg_jf_phrase_m_235, msg_jf_phrase_m_236, msg_jf_phrase_m_237, msg_jf_phrase_m_238, msg_jf_phrase_m_239,
  msg_jf_phrase_m_240, msg_jf_phrase_m_241, msg_jf_phrase_m_242, msg_jf_phrase_m_243, msg_jf_phrase_m_244, msg_jf_phrase_m_245, msg_jf_phrase_m_246, msg_jf_phrase_m_247,
  msg_jf_phrase_m_248, msg_jf_phrase_m_249, msg_jf_phrase_m_250, msg_jf_phrase_m_251, msg_jf_phrase_m_252, msg_jf_phrase_m_253, msg_jf_phrase_m_254, msg_jf_phrase_m_255,
  msg_jf_phrase_m_256, msg_jf_phrase_m_257, msg_jf_phrase_m_258, msg_jf_phrase_m_259, msg_jf_phrase_m_260, msg_jf_phrase_m_261, msg_jf_phrase_m_262, msg_jf_phrase_m_263,
  msg_jf_phrase_m_264, msg_jf_phrase_m_265, msg_jf_phrase_m_266, msg_jf_phrase_m_281, msg_jf_phrase_m_282, msg_jf_phrase_m_283, msg_jf_phrase_m_284, msg_jf_phrase_m_285,
  msg_jf_phrase_m_286, msg_jf_phrase_m_287, msg_jf_phrase_m_288, msg_jf_phrase_m_289, msg_jf_phrase_m_290, msg_jf_phrase_m_291, msg_jf_phrase_m_292, msg_jf_phrase_m_293,
  msg_jf_phrase_m_294, msg_jf_phrase_m_295, msg_jf_phrase_m_296, msg_jf_phrase_m_297, msg_jf_phrase_m_298, msg_jf_phrase_m_299, msg_jf_phrase_m_300, msg_jf_phrase_m_301,
  msg_jf_phrase_m_302, msg_jf_phrase_m_303, msg_jf_phrase_m_304, msg_jf_phrase_m_305, msg_jf_phrase_m_307, msg_jf_phrase_m_308, msg_jf_phrase_m_309,
  msg_jf_phrase_m_310, msg_jf_phrase_m_311, msg_jf_phrase_m_312, msg_jf_phrase_m_313, msg_jf_phrase_m_314, msg_jf_phrase_m_315, msg_jf_phrase_m_316, msg_jf_phrase_m_317,
  msg_jf_phrase_m_318, msg_jf_phrase_m_319, msg_jf_phrase_m_320, msg_jf_phrase_m_321, msg_jf_phrase_m_348, msg_jf_phrase_m_349, msg_jf_phrase_m_350, msg_jf_phrase_m_351,
  msg_jf_phrase_m_352, msg_jf_phrase_m_353, msg_jf_phrase_m_354, msg_jf_phrase_m_355, msg_jf_phrase_m_356, msg_jf_phrase_m_357, msg_jf_phrase_m_358, msg_jf_phrase_m_359,
  msg_jf_phrase_m_360, msg_jf_phrase_m_361, msg_jf_phrase_m_362, msg_jf_phrase_m_363, msg_jf_phrase_m_364, msg_jf_phrase_m_365, msg_jf_phrase_m_366, msg_jf_phrase_m_419,
  msg_jf_phrase_m_420, msg_jf_phrase_m_421, msg_jf_phrase_m_422, msg_jf_phrase_m_423, msg_jf_phrase_m_424, msg_jf_phrase_m_425, msg_jf_phrase_m_426, msg_jf_phrase_m_427,
  msg_jf_phrase_m_428, msg_jf_phrase_m_429, msg_jf_phrase_m_430, msg_jf_phrase_m_431, msg_jf_phrase_m_432, msg_jf_phrase_m_433, msg_jf_phrase_m_434, msg_jf_phrase_m_435,
  msg_jf_phrase_m_436, msg_jf_phrase_m_437, msg_jf_phrase_m_438, msg_jf_phrase_m_439, msg_jf_phrase_m_440, msg_jf_phrase_m_441, msg_jf_phrase_m_442, msg_jf_phrase_m_443,
  msg_jf_phrase_m_444, msg_jf_phrase_m_445, msg_jf_phrase_m_446, msg_jf_phrase_m_447, msg_jf_phrase_m_448, msg_jf_phrase_m_449, msg_jf_phrase_m_450, msg_jf_phrase_m_451,
  msg_jf_phrase_m_452, msg_jf_phrase_m_453, msg_jf_phrase_m_454, msg_jf_phrase_m_455, msg_jf_phrase_m_456, msg_jf_phrase_m_457, 
  msg_jf_phrase_m_461, msg_jf_phrase_m_462, msg_jf_phrase_m_463, msg_jf_phrase_m_464, msg_jf_phrase_m_465, msg_jf_phrase_m_466, 
  msg_jf_phrase_m_469, msg_jf_phrase_m_471, msg_jf_phrase_m_472, msg_jf_phrase_m_473, msg_jf_phrase_m_474, msg_jf_phrase_m_475,
  msg_jf_phrase_m_477, msg_jf_phrase_m_478, msg_jf_phrase_m_479, msg_jf_phrase_m_480, msg_jf_phrase_m_482, 
  msg_jf_phrase_m_484, msg_jf_phrase_m_485, msg_jf_phrase_m_486, msg_jf_phrase_m_488, msg_jf_phrase_m_490, msg_jf_phrase_m_491,
  msg_jf_phrase_m_492, msg_jf_phrase_m_495, msg_jf_phrase_m_496, msg_jf_phrase_m_497, msg_jf_phrase_m_498, msg_jf_phrase_m_499,
  msg_jf_phrase_m_500, msg_jf_phrase_m_501, msg_jf_phrase_m_502, msg_jf_phrase_m_503, msg_jf_phrase_m_504, msg_jf_phrase_m_505, msg_jf_phrase_m_506, msg_jf_phrase_m_507,
  msg_jf_phrase_m_508, msg_jf_phrase_m_509, msg_jf_phrase_m_510, msg_jf_phrase_m_511, msg_jf_phrase_m_512, msg_jf_phrase_m_518
};

// お店案内での発動するパーソナルイベントテーブル
const u8 JoinFestaAttractionUtil::m_fakeHitPersonalEventTable[7][9] = {
  // くじやへ　おきゃくを　おくれ！
  {
    // 当たり
    //JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP4,//! オススメお店案内（自分へのごほうび）
    //JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP5,//! オススメお店案内（ドキドキしたい）
    0, 0, 0, 0, 1, 1, 0, 0, 0
  },
  // エアゆうぐへ　おきゃくを　おくれ！
  {
    // 当たり
    //JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP2,//! オススメお店案内（ポケモン楽しませたい）
    //JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP8,//! オススメお店案内（ポケモンを遊ばせたい）
    0, 0, 1, 0, 0, 0, 0, 0, 1
  },
  // ビックリハウスへ　おきゃくを　おくれ！
  {
    // 当たり
    //JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP2,//! オススメお店案内（ポケモン楽しませたい）
    //JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP5,//! オススメお店案内（ドキドキしたい）
    //JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP8,//! オススメお店案内（ポケモンを遊ばせたい）
    0, 0, 1, 0, 0, 1, 0, 0, 1
  },
  // やたいへ　おきゃくを　おくれ！
  {
    // 当たり
    //JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP0,//! オススメお店案内（おなかがへったよ）
    //JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP2,//! オススメお店案内（ポケモン楽しませたい）
    //JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP3,//! オススメお店案内（すごいわざを　たいけんしたい）
    1, 0, 1, 1, 0, 0, 0, 0, 0
  },
  // ギフトショップへ　おきゃくを　おくれ！
  {
    // 当たり
    //JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP1,//! オススメお店案内（かいものしたい）
    //JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP4,//! オススメお店案内（自分へのごほうび）
    0, 1, 0, 0, 1, 0, 0, 0, 0
  },
  // うらないやへ　おきゃくを　おくれ！
  {
    // 当たり
    //JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP3,//! オススメお店案内（すごいわざを　たいけんしたい）
    //JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP4,//! オススメお店案内（自分へのごほうび）
    //JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP5,//! オススメお店案内（ドキドキしたい）
    0, 0, 0, 1, 1, 1, 0, 0, 0
  },
  // そめものやへ　おきゃくを　おくれ！
  {
    // 当たり
    //JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP3,//! オススメお店案内（すごいわざを　たいけんしたい）
    //JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP4,//! オススメお店案内（自分へのごほうび）
    0, 0, 0, 1, 1, 0, 0, 0, 0
  },
};

u16 JoinFestaAttractionUtil::m_saveHitPokeTypeTable[ PERSONAL_MAX ];

// コンストラクタ
JoinFestaAttractionUtil::JoinFestaAttractionUtil()
{
  for( u8 i=0; i<PERSONAL_MAX; i++ )
  {
    m_saveHitPokeTypeTable[i] = POKETYPE_NULL;
  }
}

// デストラクタ
JoinFestaAttractionUtil::~JoinFestaAttractionUtil()
{
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクションIDから自分がプレイ可能かチェックする
 * @param[in]   attractionId    アトラクションID
 *
 * @retval  true : プレイ出来る
 * @retval  false : プレイ出来ない
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaAttractionUtil::CheckAttractionPlayEnable( const JoinFestaScript::AttractionId attractionId )
{
  switch( attractionId ){
  // 該当の施設を持っていない場合は、プレイ出来ない
  case JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_1:       //!< くじやへ　おきゃくを　おくれ！
    return CheckFacilitiesAttractionPlayEnable( attractionId, JoinFestaScript::FACILITIES_TYPE_LOTTERY );
  case JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_2:       //!< エアゆうぐへ　おきゃくを　おくれ！
    return CheckFacilitiesAttractionPlayEnable( attractionId, JoinFestaScript::FACILITIES_TYPE_AIR_TOY );
  case JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_3:       //!< ビックリハウスへ　おきゃくを　おくれ！
    return CheckFacilitiesAttractionPlayEnable( attractionId, JoinFestaScript::FACILITIES_TYPE_FUN_HOUSE );
  case JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_4:       //!< やたいへ　おきゃくを　おくれ！
    return CheckFacilitiesAttractionPlayEnable( attractionId, JoinFestaScript::FACILITIES_TYPE_STALL );
  case JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_5:       //!< ギフトショップへ　おきゃくを　おくれ！
    return CheckFacilitiesAttractionPlayEnable( attractionId, JoinFestaScript::FACILITIES_TYPE_GIFT_SHOP );
  case JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_6:       //!< うらないやへ　おきゃくを　おくれ！
    return CheckFacilitiesAttractionPlayEnable( attractionId, JoinFestaScript::FACILITIES_TYPE_FORTUNE );
  case JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_7:       //!< そめものやへ　おきゃくを　おくれ！
    return CheckFacilitiesAttractionPlayEnable( attractionId, JoinFestaScript::FACILITIES_TYPE_DYEING );
  }

  return true;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクションに出現するフィールド人物リスト作成
 * @param[in]   attractionId    アトラクションID
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionUtil::CreateFieldPersonalListForAttraction( const JoinFestaScript::AttractionId attractionId, const NetApp::JoinFesta::JoinFestaDataTable* pDataTable )
{
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
  NetAppLib::JoinFesta::JoinFestaPersonalData* pMyData = pJoinFestaPersonalDataManager->GetMyData();

  // フィールド出現リストはここで作り直す為、破棄する
  pJoinFestaPersonalDataManager->ReleaseFieldPersonalList();

  switch( attractionId ){
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_1:             //!< ベテラントレーナーに　あいさつ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_2:             //!< しんじんトレーナーに　ひとこと！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_3:             //!< ラブラブな　ひとを　あつめろ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_4:             //!< クールなひとを　あつめろ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_5:             //!< おとこともだち　ちょうさ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_6:             //!< おんなともだち　ちょうさ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_7:             //!< あにき　あねきを　みつけろ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_8:             //!< まんなかっこを　みつけろ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_9:             //!< すえっこを　みつけろ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_10:            //!< ひとりっこを　みつけろ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_11:            //!< なかみが　だいじなひとを　さがせ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_12:            //!< がいけんが　だいじなひとを　さがせ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_13:            //!< ひみつはまもるもの
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_14:            //!< ひみつはもらすもの
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_15:            //!< 休日はまったり
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_16:            //!< 休日はアクティブ
    {
      // ・VIPとゲストで該当アンケートに回答（1以上）している人のみ出す
      // ・当たりの人は一言ウィンドウを差し替える
      CreateFieldPersonalList( attractionId, 4, 8, PERSONAL_MAX );

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = pJoinFestaPersonalDataManager->GetFieldPersonalList();
      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);

        // NPCの場合は、着せ替え設定する
        if( pPersonalData != NULL && pPersonalData->IsAttractionDummyData() ) 
        {
          SetDressUpPattern( *pPersonalData, DRESS_UP_PARAM_PATTERN_DEFAULT, pDataTable );
          // @fix MMCat[36] : 性別に合わせて定型文変わるので再設定
          pPersonalData->ResettingAttractionDummyDataPatternHello();
        }
      }
    }
    break;
  case JoinFestaScript::ATTRACTION_DRESSUP_DIFFERENCE_1:        //!< ファンを探せ！
    {
      // ・自分の着せ替え状況のコピーを５名
      // ・自分の着せ替え状況のコピーしてアルゴリズムで一部変更５名
      //  （ランダムで次のパーツを１コ～３コの範囲で変更する。　1.　眼鏡、2.　バッグ、3.　靴　※１）
      // ・ダミー（Ａ枠)６名

      // NPC16人
      CreateFieldPersonalList( attractionId, 0, 0, PERSONAL_MAX );

      // ダミー枠パターン
      const DressUpParamPattern dummyPattern = (pMyData->GetSex() == PM_MALE) ? DRESS_UP_PARAM_PATTERN_A_MALE : DRESS_UP_PARAM_PATTERN_A_FEMALE;

      // 着せ替え内訳
      const DressUpParamPattern dressUpPatternTbl[16] = {
        DRESS_UP_PARAM_PATTERN_MY_COPY,
        DRESS_UP_PARAM_PATTERN_MY_COPY,
        DRESS_UP_PARAM_PATTERN_MY_COPY,
        DRESS_UP_PARAM_PATTERN_MY_COPY,
        DRESS_UP_PARAM_PATTERN_MY_COPY,
        DRESS_UP_PARAM_PATTERN_MY_COPY2,
        DRESS_UP_PARAM_PATTERN_MY_COPY2,
        DRESS_UP_PARAM_PATTERN_MY_COPY2,
        DRESS_UP_PARAM_PATTERN_MY_COPY2,
        DRESS_UP_PARAM_PATTERN_MY_COPY2,
        dummyPattern,
        dummyPattern,
        dummyPattern,
        dummyPattern,
        dummyPattern,
        dummyPattern,
      };
      u8 tblIndex = 0;

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = pJoinFestaPersonalDataManager->GetFieldPersonalList();
      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);

        // NPCの場合は、着せ替え設定する
        if( pPersonalData != NULL && pPersonalData->IsAttractionDummyData() ) 
        {
          if( dressUpPatternTbl[ tblIndex ] == DRESS_UP_PARAM_PATTERN_MY_COPY )
          { // 自分と同じ場合に加点対象
            pPersonalData->SetAttractionAddPointFlag();
          }

          SetDressUpPattern( *pPersonalData, dressUpPatternTbl[ tblIndex++ ], pDataTable );
          // @fix MMCat[36] : 性別に合わせて定型文変わるので再設定
          pPersonalData->ResettingAttractionDummyDataPatternHello();
        }
      }
    }
    break;
  case JoinFestaScript::ATTRACTION_DRESSUP_DIFFERENCE_2:        //!< だいかそうたいかい！
    {
      // ・自分の着せ替え状況のコピーを１１名
      // ・自分の着せ替え状況のコピーしてアルゴリズムで一部変更３名
      //  （ランダムで次のパーツを１コ～３コの範囲で変更する。　1.　眼鏡、2.　バッグ、3.　靴　※１）
      // ・ダミー（Ａ枠)２名

      // NPC16人
      CreateFieldPersonalList( attractionId, 0, 0, PERSONAL_MAX );

      // ダミー枠パターン
      const DressUpParamPattern dummyPattern = (pMyData->GetSex() == PM_MALE) ? DRESS_UP_PARAM_PATTERN_A_MALE : DRESS_UP_PARAM_PATTERN_A_FEMALE;

      // 着せ替え内訳
      const DressUpParamPattern dressUpPatternTbl[16] = {
        DRESS_UP_PARAM_PATTERN_MY_COPY,
        DRESS_UP_PARAM_PATTERN_MY_COPY,
        DRESS_UP_PARAM_PATTERN_MY_COPY,
        DRESS_UP_PARAM_PATTERN_MY_COPY,
        DRESS_UP_PARAM_PATTERN_MY_COPY,
        DRESS_UP_PARAM_PATTERN_MY_COPY,
        DRESS_UP_PARAM_PATTERN_MY_COPY,
        DRESS_UP_PARAM_PATTERN_MY_COPY,
        DRESS_UP_PARAM_PATTERN_MY_COPY,
        DRESS_UP_PARAM_PATTERN_MY_COPY,
        DRESS_UP_PARAM_PATTERN_MY_COPY,
        DRESS_UP_PARAM_PATTERN_MY_COPY2,
        DRESS_UP_PARAM_PATTERN_MY_COPY2,
        DRESS_UP_PARAM_PATTERN_MY_COPY2,
        dummyPattern,
        dummyPattern,
      };
      u8 tblIndex = 0;

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = pJoinFestaPersonalDataManager->GetFieldPersonalList();
      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);

        // NPCの場合は、着せ替え設定する
        if( pPersonalData != NULL && pPersonalData->IsAttractionDummyData() ) 
        {
          if( dressUpPatternTbl[ tblIndex ] != DRESS_UP_PARAM_PATTERN_MY_COPY )
          { // 自分と違う場合に加点対象
            pPersonalData->SetAttractionAddPointFlag();
          }
          SetDressUpPattern( *pPersonalData, dressUpPatternTbl[ tblIndex++ ], pDataTable );
          // @fix MMCat[36] : 性別に合わせて定型文変わるので再設定
          pPersonalData->ResettingAttractionDummyDataPatternHello();
        }
      }
    }
    break;

  case JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_1:       //!< くじやへ　おきゃくを　おくれ！
  case JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_2:       //!< エアゆうぐへ　おきゃくを　おくれ！
  case JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_3:       //!< ビックリハウスへ　おきゃくを　おくれ！
  case JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_4:       //!< やたいへ　おきゃくを　おくれ！
  case JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_5:       //!< ギフトショップへ　おきゃくを　おくれ！
  case JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_6:       //!< うらないやへ　おきゃくを　おくれ！
  case JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_7:       //!< そめものやへ　おきゃくを　おくれ！
    {
      u8 count = 0;
      // 正解の人を指定する為のテーブル
      u8 hitTbl[16] = { 1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0 };

      // 適当にバラけさせる
      for( int i=0; i<30; i++ )
      {
        u32 randomValue = System::GflUse::GetPublicRand(16);

        u8 tmp = hitTbl[0];
        hitTbl[0] = hitTbl[randomValue];
        hitTbl[randomValue] = tmp;
      }
      
      CreateFieldPersonalList( attractionId, 8, 8, PERSONAL_MAX );

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = pJoinFestaPersonalDataManager->GetFieldPersonalList();
      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);

        // Fakeパーソナルイベント発動
        const bool isHit = (hitTbl[count]==1) ? true : false;
        SetupFakePersonalEvent( attractionId, *pPersonalData, isHit );

        // NPCの場合は、着せ替え設定する
        if( pPersonalData != NULL && pPersonalData->IsAttractionDummyData() ) 
        {
          SetDressUpPattern( *pPersonalData, DRESS_UP_PARAM_PATTERN_DEFAULT, pDataTable );
          // パーソナルイベントで定型文変わるので再設定
          pPersonalData->ResettingAttractionDummyDataPatternHello();
        }
        count++;
      }
    }
    break;

  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_JP:          //!< にほんご　レッスン
  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_EN:          //!< えいご　レッスン
  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_FR:          //!< フランスご　レッスン
  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_IT:          //!< イタリアご　レッスン
  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_GE:          //!< ドイツご　レッスン
  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_SP:          //!< スペインご　レッスン
  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_KR:          //!< かんこくご　レッスン
  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_CT:          //!< はんたいじ　かんたいじ　レッスン
    {
      // NPC16人
      CreateFieldPersonalList( attractionId, 0, 0, PERSONAL_MAX );

      // 言語IDを取得
      u8 languageTbl[ PERSONAL_MAX ] = {0};
      SetupLanguageTbl( attractionId, languageTbl );
      u8 tblIndex = 0;

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = pJoinFestaPersonalDataManager->GetFieldPersonalList();
      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);

        // NPCの場合は、着せ替え設定する
        if( pPersonalData != NULL && pPersonalData->IsAttractionDummyData() ) 
        {
          SetDressUpPattern( *pPersonalData, DRESS_UP_PARAM_PATTERN_DEFAULT, pDataTable );
          // アトラクションごとに決まった言語IDにセットする
          pPersonalData->SetupAttractionDummyDataLanguageId( languageTbl[ tblIndex++ ] );
          // 言語で定型文変わるので再設定
          pPersonalData->ResettingAttractionDummyDataPatternHello();
        }
      }
    }
    break;

  case JoinFestaScript::ATTRACTION_LANGUAGE_1:                  //!< いぶんか　こうりゅう！
    {
      u32 npcIndex = 0;
      CreateFieldPersonalList( attractionId, 8, 8, PERSONAL_MAX );

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = pJoinFestaPersonalDataManager->GetFieldPersonalList();
      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);

        if( pPersonalData == NULL )
        {
          continue;
        }

        // NPCの場合は、着せ替え設定する
        if( pPersonalData->IsAttractionDummyData() ) 
        {
          SetDressUpPattern( *pPersonalData, DRESS_UP_PARAM_PATTERN_DEFAULT, pDataTable );
          // @fix MMCat[36] : 性別に合わせて定型文変わるので再設定
          pPersonalData->ResettingAttractionDummyDataPatternHello();

          // ４人に１人当たりにする
          if( (npcIndex++ % 4) == 0 )
          {
            // 自分の言語以外にする
            if( pPersonalData->GetLanguageId() == pMyData->GetLanguageId() )
            {
              u8 languageIdTbl[9] = {
                POKEMON_LANG_JAPAN,
                POKEMON_LANG_ENGLISH,
                POKEMON_LANG_FRANCE,
                POKEMON_LANG_ITALY,
                POKEMON_LANG_GERMANY,
                POKEMON_LANG_SPAIN,
                POKEMON_LANG_KOREA,
                POKEMON_LANG_CHINA,
                POKEMON_LANG_TAIWAN,
              };

              // 同じ言語IDの要素を上書きする
              for( u32 i=0; i<9; i++ )
              {
                if( languageIdTbl[i] == pMyData->GetLanguageId() )
                {
                  if( i == 8 ){
                    languageIdTbl[i] = languageIdTbl[0];
                  }else{
                    languageIdTbl[i] = languageIdTbl[i+1];
                  }
                  break;
                }
              }

              // どれを選んでも違う言語になる
              pPersonalData->SetupAttractionDummyDataLanguageId( languageIdTbl[ System::GflUse::GetPublicRand( 9 ) ] );
            }
          }
          else
          {
            // ハズレの場合は自分と同じ言語
            pPersonalData->SetupAttractionDummyDataLanguageId( pMyData->GetLanguageId() );
          }
        }

        // 自分と言語が違う人が加点対象
        if( pMyData->GetLanguageId() != pPersonalData->GetLanguageId() )
        {
          pPersonalData->SetAttractionAddPointFlag();

          if( pPersonalData->IsAttractionDummyData() ) 
          {
            // 加点対象の定型文が変わるため、再度セットアップを呼ぶ
            pPersonalData->ResettingAttractionDummyDataPatternHello();
          }
        }
      }
    }
    break;

  case JoinFestaScript::ATTRACTION_DRESSUP_DISGUISE_1:          //!< へんそうを　みやぶれ！
    {
      // Ｒ（帽子and眼鏡）から５名　正解
      // Ｓ（帽子or眼鏡）から７名　 不正解
      // Ａ（基本データ）から４名   不正解

      // NPC16人
      CreateFieldPersonalList( attractionId, 0, 0, PERSONAL_MAX );

      // 着せ替え内訳
      const DressUpParamPattern dressUpPatternTbl[16] = {
        DRESS_UP_PARAM_PATTERN_R,
        DRESS_UP_PARAM_PATTERN_R,
        DRESS_UP_PARAM_PATTERN_R,
        DRESS_UP_PARAM_PATTERN_R,
        DRESS_UP_PARAM_PATTERN_R,
        DRESS_UP_PARAM_PATTERN_S,
        DRESS_UP_PARAM_PATTERN_S,
        DRESS_UP_PARAM_PATTERN_S,
        DRESS_UP_PARAM_PATTERN_S,
        DRESS_UP_PARAM_PATTERN_S,
        DRESS_UP_PARAM_PATTERN_S,
        DRESS_UP_PARAM_PATTERN_S,
        DRESS_UP_PARAM_PATTERN_A,
        DRESS_UP_PARAM_PATTERN_A,
        DRESS_UP_PARAM_PATTERN_A,
        DRESS_UP_PARAM_PATTERN_A,
      };
      u8 tblIndex = 0;

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = pJoinFestaPersonalDataManager->GetFieldPersonalList();
      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);

        // NPCの場合は、着せ替え設定する
        if( pPersonalData != NULL && pPersonalData->IsAttractionDummyData() ) 
        {
          if( dressUpPatternTbl[ tblIndex ] == DRESS_UP_PARAM_PATTERN_R )
          { // 全部変装している人がに加点対象
            pPersonalData->SetAttractionAddPointFlag();
          }
          SetDressUpPattern( *pPersonalData, dressUpPatternTbl[ tblIndex++ ], pDataTable );
          // @fix MMCat[36] : 性別に合わせて定型文変わるので再設定
          pPersonalData->ResettingAttractionDummyDataPatternHello();
        }
      }
    }
    break;
  case JoinFestaScript::ATTRACTION_DRESSUP_DISGUISE_2:          //!< すがおが　いちばん！
    {
      // Ａ（基本データ）から５名 正解
      // Ｓ（帽子or眼鏡）から７名 不正解
      // Ｒ（帽子and眼鏡）から４名  不正解

      // NPC16人
      CreateFieldPersonalList( attractionId, 0, 0, PERSONAL_MAX );

      // 着せ替え内訳
      const DressUpParamPattern dressUpPatternTbl[16] = {
        DRESS_UP_PARAM_PATTERN_A,
        DRESS_UP_PARAM_PATTERN_A,
        DRESS_UP_PARAM_PATTERN_A,
        DRESS_UP_PARAM_PATTERN_A,
        DRESS_UP_PARAM_PATTERN_A,
        DRESS_UP_PARAM_PATTERN_S,
        DRESS_UP_PARAM_PATTERN_S,
        DRESS_UP_PARAM_PATTERN_S,
        DRESS_UP_PARAM_PATTERN_S,
        DRESS_UP_PARAM_PATTERN_S,
        DRESS_UP_PARAM_PATTERN_S,
        DRESS_UP_PARAM_PATTERN_S,
        DRESS_UP_PARAM_PATTERN_R,
        DRESS_UP_PARAM_PATTERN_R,
        DRESS_UP_PARAM_PATTERN_R,
        DRESS_UP_PARAM_PATTERN_R,
      };
      u8 tblIndex = 0;

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = pJoinFestaPersonalDataManager->GetFieldPersonalList();
      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);

        // NPCの場合は、着せ替え設定する
        if( pPersonalData != NULL && pPersonalData->IsAttractionDummyData() ) 
        {
          if( dressUpPatternTbl[ tblIndex ] == DRESS_UP_PARAM_PATTERN_A )
          { // 変装していない人が加点対象
            pPersonalData->SetAttractionAddPointFlag();
          }
          SetDressUpPattern( *pPersonalData, dressUpPatternTbl[ tblIndex++ ], pDataTable );
          // @fix MMCat[36] : 性別に合わせて定型文変わるので再設定
          pPersonalData->ResettingAttractionDummyDataPatternHello();
        }
      }
    }
    break;
  case JoinFestaScript::ATTRACTION_RIDE_KAIRIKY_1:              //!< みんなに　みせつけろ！　スプリント！
    {
      CreateFieldPersonalList( attractionId, 8, 8, PERSONAL_MAX );

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = pJoinFestaPersonalDataManager->GetFieldPersonalList();
      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);

        // NPCの場合は、着せ替え設定する
        if( pPersonalData != NULL && pPersonalData->IsAttractionDummyData() ) 
        {
          SetDressUpPattern( *pPersonalData, DRESS_UP_PARAM_PATTERN_DEFAULT, pDataTable );
          // @fix MMCat[36] : 性別に合わせて定型文変わるので再設定
          pPersonalData->ResettingAttractionDummyDataPatternHello();
        }
      }
    }
    break;
  case JoinFestaScript::ATTRACTION_RIDE_KENTAROSU_1:            //!< いわをこわせ！　スプリント！
    {
      // 人物は店員にしたのでフィールド人物は出さない
    }
    break;
  case JoinFestaScript::ATTRACTION_DRESSUP_RESEARCH_1:          //!< レッドリサーチ
    {
      // Ｂ（赤正解）を５名
      // Ｊ（一箇所だけ色が違う）を５名
      // Ｃ、Ｄ、Ｅ、Ｆ、Ｇ、Ｈ、Ｉ（その他の色の当たり枠から抽選）を６名

      // NPC16人
      CreateFieldPersonalList( attractionId, 0, 0, PERSONAL_MAX );

      // 着せ替え内訳
      const DressUpParamPattern dressUpPatternTbl[16] = {
        DRESS_UP_PARAM_PATTERN_B,
        DRESS_UP_PARAM_PATTERN_B,
        DRESS_UP_PARAM_PATTERN_B,
        DRESS_UP_PARAM_PATTERN_B,
        DRESS_UP_PARAM_PATTERN_B,
        DRESS_UP_PARAM_PATTERN_J,
        DRESS_UP_PARAM_PATTERN_J,
        DRESS_UP_PARAM_PATTERN_J,
        DRESS_UP_PARAM_PATTERN_J,
        DRESS_UP_PARAM_PATTERN_J,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
      };
      u8 tblIndex = 0;

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = pJoinFestaPersonalDataManager->GetFieldPersonalList();
      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);

        // NPCの場合は、着せ替え設定する
        if( pPersonalData != NULL && pPersonalData->IsAttractionDummyData() ) 
        {
          if( dressUpPatternTbl[ tblIndex ] == DRESS_UP_PARAM_PATTERN_B )
          { // 色が合っている人が加点対象
            pPersonalData->SetAttractionAddPointFlag();
          }
          SetDressUpPattern( *pPersonalData, dressUpPatternTbl[ tblIndex++ ], pDataTable );
          // @fix MMCat[36] : 性別に合わせて定型文変わるので再設定
          pPersonalData->ResettingAttractionDummyDataPatternHello();
        }
      }
    }
    break;
  case JoinFestaScript::ATTRACTION_DRESSUP_RESEARCH_2:          //!< グリーンリサーチ
    {
      // D（緑正解）を５名
      // L（一箇所だけ色が違う）を５名
      // Ｃ、Ｅ、Ｆ、Ｇ、Ｈ、Ｉ（その他の色の当たり枠から抽選）を６名

      // NPC16人
      CreateFieldPersonalList( attractionId, 0, 0, PERSONAL_MAX );

      // 着せ替え内訳
      const DressUpParamPattern dressUpPatternTbl[16] = {
        DRESS_UP_PARAM_PATTERN_D,
        DRESS_UP_PARAM_PATTERN_D,
        DRESS_UP_PARAM_PATTERN_D,
        DRESS_UP_PARAM_PATTERN_D,
        DRESS_UP_PARAM_PATTERN_D,
        DRESS_UP_PARAM_PATTERN_L,
        DRESS_UP_PARAM_PATTERN_L,
        DRESS_UP_PARAM_PATTERN_L,
        DRESS_UP_PARAM_PATTERN_L,
        DRESS_UP_PARAM_PATTERN_L,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
      };
      u8 tblIndex = 0;

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = pJoinFestaPersonalDataManager->GetFieldPersonalList();
      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);

        // NPCの場合は、着せ替え設定する
        if( pPersonalData != NULL && pPersonalData->IsAttractionDummyData() ) 
        {
          if( dressUpPatternTbl[ tblIndex ] == DRESS_UP_PARAM_PATTERN_D )
          { // 色が合っている人が加点対象
            pPersonalData->SetAttractionAddPointFlag();
          }
          SetDressUpPattern( *pPersonalData, dressUpPatternTbl[ tblIndex++ ], pDataTable );
          // @fix MMCat[36] : 性別に合わせて定型文変わるので再設定
          pPersonalData->ResettingAttractionDummyDataPatternHello();
        }
      }
    }
    break;
  case JoinFestaScript::ATTRACTION_DRESSUP_RESEARCH_3:          //!< オレンジリサーチ
    {
      // NPC16人
      CreateFieldPersonalList( attractionId, 0, 0, PERSONAL_MAX );

      // 着せ替え内訳
      const DressUpParamPattern dressUpPatternTbl[16] = {
        DRESS_UP_PARAM_PATTERN_F,
        DRESS_UP_PARAM_PATTERN_F,
        DRESS_UP_PARAM_PATTERN_F,
        DRESS_UP_PARAM_PATTERN_F,
        DRESS_UP_PARAM_PATTERN_F,
        DRESS_UP_PARAM_PATTERN_N,
        DRESS_UP_PARAM_PATTERN_N,
        DRESS_UP_PARAM_PATTERN_N,
        DRESS_UP_PARAM_PATTERN_N,
        DRESS_UP_PARAM_PATTERN_N,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
      };
      u8 tblIndex = 0;

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = pJoinFestaPersonalDataManager->GetFieldPersonalList();
      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);

        // NPCの場合は、着せ替え設定する
        if( pPersonalData != NULL && pPersonalData->IsAttractionDummyData() ) 
        {
          if( dressUpPatternTbl[ tblIndex ] == DRESS_UP_PARAM_PATTERN_F )
          { // 色が合っている人が加点対象
            pPersonalData->SetAttractionAddPointFlag();
          }
          SetDressUpPattern( *pPersonalData, dressUpPatternTbl[ tblIndex++ ], pDataTable );
          // @fix MMCat[36] : 性別に合わせて定型文変わるので再設定
          pPersonalData->ResettingAttractionDummyDataPatternHello();
        }
      }
    }
    break;
  case JoinFestaScript::ATTRACTION_DRESSUP_RESEARCH_4:          //!< パープルリサーチ
    {
      // NPC16人
      CreateFieldPersonalList( attractionId, 0, 0, PERSONAL_MAX );

      // 着せ替え内訳
      const DressUpParamPattern dressUpPatternTbl[16] = {
        DRESS_UP_PARAM_PATTERN_H,
        DRESS_UP_PARAM_PATTERN_H,
        DRESS_UP_PARAM_PATTERN_H,
        DRESS_UP_PARAM_PATTERN_H,
        DRESS_UP_PARAM_PATTERN_H,
        DRESS_UP_PARAM_PATTERN_P,
        DRESS_UP_PARAM_PATTERN_P,
        DRESS_UP_PARAM_PATTERN_P,
        DRESS_UP_PARAM_PATTERN_P,
        DRESS_UP_PARAM_PATTERN_P,
        DRESS_UP_PARAM_PATTERN_WARM,
        DRESS_UP_PARAM_PATTERN_WARM,
        DRESS_UP_PARAM_PATTERN_WARM,
        DRESS_UP_PARAM_PATTERN_WARM,
        DRESS_UP_PARAM_PATTERN_WARM,
        DRESS_UP_PARAM_PATTERN_WARM,
      };
      u8 tblIndex = 0;

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = pJoinFestaPersonalDataManager->GetFieldPersonalList();
      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);

        // NPCの場合は、着せ替え設定する
        if( pPersonalData != NULL && pPersonalData->IsAttractionDummyData() ) 
        {
          if( dressUpPatternTbl[ tblIndex ] == DRESS_UP_PARAM_PATTERN_H )
          { // 色が合っている人が加点対象
            pPersonalData->SetAttractionAddPointFlag();
          }
          SetDressUpPattern( *pPersonalData, dressUpPatternTbl[ tblIndex++ ], pDataTable );
          // @fix MMCat[36] : 性別に合わせて定型文変わるので再設定
          pPersonalData->ResettingAttractionDummyDataPatternHello();
        }
      }
    }
    break;
  case JoinFestaScript::ATTRACTION_DRESSUP_RESEARCH_5:          //!< イエローリサーチ
    {
      // NPC16人
      CreateFieldPersonalList( attractionId, 0, 0, PERSONAL_MAX );

      // 着せ替え内訳
      const DressUpParamPattern dressUpPatternTbl[16] = {
        DRESS_UP_PARAM_PATTERN_C,
        DRESS_UP_PARAM_PATTERN_C,
        DRESS_UP_PARAM_PATTERN_C,
        DRESS_UP_PARAM_PATTERN_C,
        DRESS_UP_PARAM_PATTERN_C,
        DRESS_UP_PARAM_PATTERN_K,
        DRESS_UP_PARAM_PATTERN_K,
        DRESS_UP_PARAM_PATTERN_K,
        DRESS_UP_PARAM_PATTERN_K,
        DRESS_UP_PARAM_PATTERN_K,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
      };
      u8 tblIndex = 0;

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = pJoinFestaPersonalDataManager->GetFieldPersonalList();
      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);

        // NPCの場合は、着せ替え設定する
        if( pPersonalData != NULL && pPersonalData->IsAttractionDummyData() ) 
        {
          if( dressUpPatternTbl[ tblIndex ] == DRESS_UP_PARAM_PATTERN_C )
          { // 色が合っている人が加点対象
            pPersonalData->SetAttractionAddPointFlag();
          }
          SetDressUpPattern( *pPersonalData, dressUpPatternTbl[ tblIndex++ ], pDataTable );
          // @fix MMCat[36] : 性別に合わせて定型文変わるので再設定
          pPersonalData->ResettingAttractionDummyDataPatternHello();
        }
      }
    }
    break;
  case JoinFestaScript::ATTRACTION_DRESSUP_RESEARCH_6:          //!< ブル－リサーチ
    {
      // NPC16人
      CreateFieldPersonalList( attractionId, 0, 0, PERSONAL_MAX );

      // 着せ替え内訳
      const DressUpParamPattern dressUpPatternTbl[16] = {
        DRESS_UP_PARAM_PATTERN_E,
        DRESS_UP_PARAM_PATTERN_E,
        DRESS_UP_PARAM_PATTERN_E,
        DRESS_UP_PARAM_PATTERN_E,
        DRESS_UP_PARAM_PATTERN_E,
        DRESS_UP_PARAM_PATTERN_M,
        DRESS_UP_PARAM_PATTERN_M,
        DRESS_UP_PARAM_PATTERN_M,
        DRESS_UP_PARAM_PATTERN_M,
        DRESS_UP_PARAM_PATTERN_M,
        DRESS_UP_PARAM_PATTERN_WARM,
        DRESS_UP_PARAM_PATTERN_WARM,
        DRESS_UP_PARAM_PATTERN_WARM,
        DRESS_UP_PARAM_PATTERN_WARM,
        DRESS_UP_PARAM_PATTERN_WARM,
        DRESS_UP_PARAM_PATTERN_WARM,
      };
      u8 tblIndex = 0;

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = pJoinFestaPersonalDataManager->GetFieldPersonalList();
      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);

        // NPCの場合は、着せ替え設定する
        if( pPersonalData != NULL && pPersonalData->IsAttractionDummyData() ) 
        {
          if( dressUpPatternTbl[ tblIndex ] == DRESS_UP_PARAM_PATTERN_E )
          { // 色が合っている人が加点対象
            pPersonalData->SetAttractionAddPointFlag();
          }
          SetDressUpPattern( *pPersonalData, dressUpPatternTbl[ tblIndex++ ], pDataTable );
          // @fix MMCat[36] : 性別に合わせて定型文変わるので再設定
          pPersonalData->ResettingAttractionDummyDataPatternHello();
        }
      }
    }
    break;
  case JoinFestaScript::ATTRACTION_DRESSUP_RESEARCH_7:          //!< ネイビーリサーチ
    {
      // NPC16人
      CreateFieldPersonalList( attractionId, 0, 0, PERSONAL_MAX );

      // 着せ替え内訳
      const DressUpParamPattern dressUpPatternTbl[16] = {
        DRESS_UP_PARAM_PATTERN_G,
        DRESS_UP_PARAM_PATTERN_G,
        DRESS_UP_PARAM_PATTERN_G,
        DRESS_UP_PARAM_PATTERN_G,
        DRESS_UP_PARAM_PATTERN_G,
        DRESS_UP_PARAM_PATTERN_O,
        DRESS_UP_PARAM_PATTERN_O,
        DRESS_UP_PARAM_PATTERN_O,
        DRESS_UP_PARAM_PATTERN_O,
        DRESS_UP_PARAM_PATTERN_O,
        DRESS_UP_PARAM_PATTERN_WARM,
        DRESS_UP_PARAM_PATTERN_WARM,
        DRESS_UP_PARAM_PATTERN_WARM,
        DRESS_UP_PARAM_PATTERN_WARM,
        DRESS_UP_PARAM_PATTERN_WARM,
        DRESS_UP_PARAM_PATTERN_WARM,
      };
      u8 tblIndex = 0;

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = pJoinFestaPersonalDataManager->GetFieldPersonalList();
      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);

        // NPCの場合は、着せ替え設定する
        if( pPersonalData != NULL && pPersonalData->IsAttractionDummyData() ) 
        {
          if( dressUpPatternTbl[ tblIndex ] == DRESS_UP_PARAM_PATTERN_G )
          { // 色が合っている人が加点対象
            pPersonalData->SetAttractionAddPointFlag();
          }
          SetDressUpPattern( *pPersonalData, dressUpPatternTbl[ tblIndex++ ], pDataTable );
          // @fix MMCat[36] : 性別に合わせて定型文変わるので再設定
          pPersonalData->ResettingAttractionDummyDataPatternHello();
        }
      }
    }
    break;
  case JoinFestaScript::ATTRACTION_DRESSUP_RESEARCH_8:          //!< ピンクリサーチ
    {
      // NPC16人
      CreateFieldPersonalList( attractionId, 0, 0, PERSONAL_MAX );

      // 着せ替え内訳
      const DressUpParamPattern dressUpPatternTbl[16] = {
        DRESS_UP_PARAM_PATTERN_I,
        DRESS_UP_PARAM_PATTERN_I,
        DRESS_UP_PARAM_PATTERN_I,
        DRESS_UP_PARAM_PATTERN_I,
        DRESS_UP_PARAM_PATTERN_I,
        DRESS_UP_PARAM_PATTERN_Q,
        DRESS_UP_PARAM_PATTERN_Q,
        DRESS_UP_PARAM_PATTERN_Q,
        DRESS_UP_PARAM_PATTERN_Q,
        DRESS_UP_PARAM_PATTERN_Q,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
        DRESS_UP_PARAM_PATTERN_COLD,
      };
      u8 tblIndex = 0;

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = pJoinFestaPersonalDataManager->GetFieldPersonalList();
      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);

        // NPCの場合は、着せ替え設定する
        if( pPersonalData != NULL && pPersonalData->IsAttractionDummyData() ) 
        {
          if( dressUpPatternTbl[ tblIndex ] == DRESS_UP_PARAM_PATTERN_I )
          { // 色が合っている人が加点対象
            pPersonalData->SetAttractionAddPointFlag();
          }
          SetDressUpPattern( *pPersonalData, dressUpPatternTbl[ tblIndex++ ], pDataTable );
          // @fix MMCat[36] : 性別に合わせて定型文変わるので再設定
          pPersonalData->ResettingAttractionDummyDataPatternHello();
        }
      }
    }
    break;
  case JoinFestaScript::ATTRACTION_AFFINITY_1:                  //!< タイプあいしょう しんだん！
  case JoinFestaScript::ATTRACTION_AFFINITY_SAKASA_1:           //!< サカサプあいしょう しんだん！
    {
      gfl2::heap::HeapBase *pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

      // NPC16人
      CreateFieldPersonalList( attractionId, 0, 0, PERSONAL_MAX );

      // ポケモン番号テーブル算出
      u16 monsNoTable[ PERSONAL_MAX ] = {0};
      u8 monsNoTableIndex = 0;
      CalcAffinityMonsNoTable( monsNoTable, pDataTable );

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = pJoinFestaPersonalDataManager->GetFieldPersonalList();
      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);

        // NPCの場合は、着せ替え設定する
        if( pPersonalData != NULL && pPersonalData->IsAttractionDummyData() ) 
        {
          SetDressUpPattern( *pPersonalData, DRESS_UP_PARAM_PATTERN_DEFAULT, pDataTable );
          {
            pml::pokepara::PokemonParam* pp;
            pp = GFL_NEW( pHeap->GetLowerHandle() ) pml::pokepara::PokemonParam( pHeap->GetLowerHandle(), static_cast<MonsNo>(monsNoTable[monsNoTableIndex++]), 98, pml::pokepara::INIT_SPEC_ID_RANDOM );

            // パーソナルにポケモンタイプを保存
            pPersonalData->SetAttractionPokeType( pp->GetType1(), pp->GetType2() );

            // 履歴用に保存する
            pPersonalData->SetAttractionMonsNo( pp->GetMonsNo() );

            GFL_DELETE pp;
          }

          // データ決まったので定型文再設定
          pPersonalData->ResettingAttractionDummyDataPatternHello();
        }
      }

      // 履歴として保存しておく
      SaveHitPokeType();
    }
    break;
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_1:               //!< ノーマルタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_2:               //!< かくとうタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_3:               //!< ひこうタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_4:               //!< どくタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_5:               //!< じめんタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_6:               //!< いわタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_7:               //!< むしタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_8:               //!< ゴーストタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_9:               //!< はがねタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_10:              //!< ほのおタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_11:              //!< みずタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_12:              //!< くさタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_13:              //!< でんきタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_14:              //!< エスパータイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_15:              //!< こおりタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_16:              //!< ドラゴンタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_17:              //!< あくタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_18:              //!< フェアリータイプすきを　さがせ！
    {
      // NPC16人
      CreateFieldPersonalList( attractionId, 0, 0, PERSONAL_MAX );

      u16 ngMonsNo = MONSNO_NULL;
      u8 pokeTypeTable[ PERSONAL_MAX ] = {0};
      u8 tblIndex = 0;

      // 正解のタイプ
      const u8 hitPokeType = GetPokeTypeFromAttractionId( attractionId );
      GFL_ASSERT( hitPokeType < POKETYPE_MAX );

      // タイプテーブル作成
      CalcSearchPokeTypeTable( hitPokeType, pokeTypeTable );

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = pJoinFestaPersonalDataManager->GetFieldPersonalList();
      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);

        // NPCの場合は、着せ替え設定する
        if( pPersonalData != NULL && pPersonalData->IsAttractionDummyData() ) 
        {
          SetDressUpPattern( *pPersonalData, DRESS_UP_PARAM_PATTERN_DEFAULT, pDataTable );
          SetSearchPokemon( *pPersonalData, false, hitPokeType, pokeTypeTable[ tblIndex ], ngMonsNo, pDataTable );
          // データ決まったので定型文再設定
          pPersonalData->ResettingAttractionDummyDataPatternHello();

          // 最初の当たりは取っておく
          if( tblIndex == 0 )
          {
            ngMonsNo = pPersonalData->GetAttractionMonsNo();
          }

          tblIndex++;
        }
      }

      // 履歴として保存しておく
      SaveHitPokeType();
    }
    break;
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_1:         //!< ノーマルタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_2:         //!< かくとうタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_3:         //!< ひこうタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_4:         //!< どくタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_5:         //!< じめんタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_6:         //!< いわタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_7:         //!< むしタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_8:         //!< ゴーストタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_9:         //!< はがねタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_10:         //!< ほのおタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_11:        //!< みずタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_12:        //!< くさタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_13:        //!< でんきタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_14:        //!< エスパータイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_15:        //!< こおりタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_16:        //!< ドラゴンタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_17:        //!< あくタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_18:        //!< フェアリータイプすきを　さがせ！（アローラずかん）
    {
      // NPC16人
      CreateFieldPersonalList( attractionId, 0, 0, PERSONAL_MAX );

      u16 ngMonsNo = MONSNO_NULL;
      u8 pokeTypeTable[ PERSONAL_MAX ] = {0};
      u8 tblIndex = 0;

      // 正解のタイプ
      const u8 hitPokeType = GetPokeTypeFromAttractionId( attractionId );
      GFL_ASSERT( hitPokeType < POKETYPE_MAX );

      // タイプテーブル作成
      CalcSearchPokeTypeTable( hitPokeType, pokeTypeTable );

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = pJoinFestaPersonalDataManager->GetFieldPersonalList();
      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);

        // NPCの場合は、着せ替え設定する
        if( pPersonalData != NULL && pPersonalData->IsAttractionDummyData() ) 
        {
          SetDressUpPattern( *pPersonalData, DRESS_UP_PARAM_PATTERN_DEFAULT, pDataTable );
          SetSearchPokemon( *pPersonalData, true, hitPokeType, pokeTypeTable[ tblIndex ], ngMonsNo, pDataTable );
          // データ決まったので定型文再設定
          pPersonalData->ResettingAttractionDummyDataPatternHello();

          // 最初の当たりは取っておく
          if( tblIndex == 0 )
          {
            ngMonsNo = pPersonalData->GetAttractionMonsNo();
          }

          tblIndex++;
        }
      }

      // 履歴として保存しておく
      SaveHitPokeType();
    }
    break;

  default:
    GFL_ASSERT(0);
    break;
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション開催中のダミーデータの定型：あいさつを取得
 * @param[in]  ダミーデータのパーソナルデータ
 *
 * @return  定型文挨拶のmsgID
 */
/* ----------------------------------------------------------------------------------------- */
// @static_function
u32 JoinFestaAttractionUtil::GetPatternHelloFromDummyData( const NetAppLib::JoinFesta::JoinFestaPersonalData& person, bool isRandomSelect )
{
  NetAppLib::JoinFesta::JoinFestaAttractionManager* pAttractionManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager);

  GFL_ASSERT( pAttractionManager->IsPlaying() );

  const JoinFestaScript::AttractionId attractionId = pAttractionManager->GetPlayAttractionId();

  // 定型あいさつのメッセージID
  u32 patternHelloMsgId = MSG_ID_NULL;

  switch( attractionId ){
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_1:             //!< ベテラントレーナーに　あいさつ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_2:             //!< しんじんトレーナーに　ひとこと！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_3:             //!< ラブラブな　ひとを　あつめろ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_4:             //!< クールなひとを　あつめろ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_5:             //!< おとこともだち　ちょうさ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_6:             //!< おんなともだち　ちょうさ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_7:             //!< あにき　あねきを　みつけろ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_8:             //!< まんなかっこを　みつけろ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_9:             //!< すえっこを　みつけろ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_10:            //!< ひとりっこを　みつけろ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_11:            //!< なかみが　だいじなひとを　さがせ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_12:            //!< がいけんが　だいじなひとを　さがせ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_13:            //!< ひみつはまもるもの
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_14:            //!< ひみつはもらすもの
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_15:            //!< 休日はまったり
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_16:            //!< 休日はアクティブ
    {
      // アンケートアトラクション
      patternHelloMsgId = GetPatternHelloForAttractionQuestionnaire( person );

      // @fix NMCat[1568]ゲストではずれの場合は、定型文テーブルから選ぶ
      if( patternHelloMsgId == MSG_ID_NULL && person.IsAttractionDummyData() == false &&
          person.IsRelatonVip() == false )
      {
        // ランダムで選んでしまうと、一言ウィンドウを出す度にランダムになる。TIDで固定にする。
        const u32 msgTableIndex = person.GetTransferedID() % RANDOM_PATTERN_TABLE_MAX;
        u32 randomMsgId = m_patternTable[ msgTableIndex ];
        const int offsetSex = ( person.GetSex() == PM_MALE ) ? 0 : msg_jf_phrase_max/2 ;
        return randomMsgId + offsetSex;
      }
    }
    break;
  case JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_1:       //!< くじやへ　おきゃくを　おくれ！
  case JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_2:       //!< エアゆうぐへ　おきゃくを　おくれ！
  case JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_3:       //!< ビックリハウスへ　おきゃくを　おくれ！
  case JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_4:       //!< やたいへ　おきゃくを　おくれ！
  case JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_5:       //!< ギフトショップへ　おきゃくを　おくれ！
  case JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_6:       //!< うらないやへ　おきゃくを　おくれ！
  case JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_7:       //!< そめものやへ　おきゃくを　おくれ！
    {
      // 施設案内アトラクション
      patternHelloMsgId = GetPatternHelloForAttractionRecommendationShop( person );
    }
    break;

  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_JP:          //!< にほんご　レッスン
  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_EN:          //!< えいご　レッスン
  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_FR:          //!< フランスご　レッスン
  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_IT:          //!< イタリアご　レッスン
  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_GE:          //!< ドイツご　レッスン
  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_SP:          //!< スペインご　レッスン
  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_KR:          //!< かんこくご　レッスン
  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_CT:          //!< はんたいじ　かんたいじ　レッスン
    {
      // 言語レッスンアトラクション
      patternHelloMsgId = GetPatternHelloForAttractionLanguageLesson( person );
    }
    break;
  case JoinFestaScript::ATTRACTION_LANGUAGE_1:                  //!< いぶんか　こうりゅう！
    {
      // 加点対象のみ言語の挨拶にする
      if( person.IsAttractionAddPoint() )
      {
        patternHelloMsgId = GetPatternHelloForAttractionLanguageLesson( person );
      }

      // @fix NMCat[1568]ゲストではずれの場合は、定型文テーブルから選ぶ
      if( patternHelloMsgId == MSG_ID_NULL && person.IsAttractionDummyData() == false &&
          person.IsRelatonVip() == false )
      {
        // ランダムで選んでしまうと、一言ウィンドウを出す度にランダムになる。TIDで固定にする。
        const u32 msgTableIndex = person.GetTransferedID() % RANDOM_PATTERN_TABLE_MAX;
        u32 randomMsgId = m_patternTable[ msgTableIndex ];
        const int offsetSex = ( person.GetSex() == PM_MALE ) ? 0 : msg_jf_phrase_max/2 ;
        return randomMsgId + offsetSex;
      }
    }
    break;

  case JoinFestaScript::ATTRACTION_RIDE_KAIRIKY_1:              //!< みんなに　みせつけろ！　スプリント！
    {
      // カイリキーライドアトラクション
      patternHelloMsgId = GetPatternHelloForAttractionRideKairiky( person );
    }
    break;
  case JoinFestaScript::ATTRACTION_AFFINITY_1:                  //!< タイプあいしょう しんだん！
  case JoinFestaScript::ATTRACTION_AFFINITY_SAKASA_1:           //!< サカサプあいしょう しんだん！
    {
      // タイプ相性アトラクション
      patternHelloMsgId = GetPatternHelloForAttractionAffinity( person );
    }
    break;
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_1:               //!< ノーマルタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_2:               //!< かくとうタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_3:               //!< ひこうタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_4:               //!< どくタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_5:               //!< じめんタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_6:               //!< いわタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_7:               //!< むしタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_8:               //!< ゴーストタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_9:               //!< はがねタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_10:              //!< ほのおタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_11:              //!< みずタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_12:              //!< くさタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_13:              //!< でんきタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_14:              //!< エスパータイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_15:              //!< こおりタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_16:              //!< ドラゴンタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_17:              //!< あくタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_18:              //!< フェアリータイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_1:         //!< ノーマルタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_2:         //!< かくとうタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_3:         //!< ひこうタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_4:         //!< どくタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_5:         //!< じめんタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_6:         //!< いわタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_7:         //!< むしタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_8:         //!< ゴーストタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_9:         //!< はがねタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_10:         //!< ほのおタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_11:        //!< みずタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_12:        //!< くさタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_13:        //!< でんきタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_14:        //!< エスパータイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_15:        //!< こおりタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_16:        //!< ドラゴンタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_17:        //!< あくタイプすきを　さがせ！（アローラずかん）
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_18:        //!< フェアリータイプすきを　さがせ！（アローラずかん）
    {
      patternHelloMsgId = ( person.GetSex() == PM_MALE ) ? msg_jf_phrase_m_008 : msg_jf_phrase_f_008;
    }
    break;
  }

  // アトラクション専用のでは決まらなかった場合は、ランダム
  if( patternHelloMsgId == MSG_ID_NULL && isRandomSelect == true )
  {
    u32 randomMsgId = m_patternTable[ System::GflUse::GetPublicRand( RANDOM_PATTERN_TABLE_MAX ) ];
    const int offsetSex = ( person.GetSex() == PM_MALE ) ? 0 : msg_jf_phrase_max/2 ;
    return randomMsgId + offsetSex;
  }
  else
  {
    return patternHelloMsgId;
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション開催中のダミーデータの定型：あいさつを取得
 * @param[in]  ダミーデータのパーソナルデータ
 *
 * @return  定型文挨拶のmsgID
 */
/* ----------------------------------------------------------------------------------------- */
// @static_function
u32 JoinFestaAttractionUtil::GetPatternNotHelloFromDummyData( const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  u32 randomMsgId = m_patternTable[ System::GflUse::GetPublicRand( RANDOM_PATTERN_TABLE_MAX ) ];
  const int offsetSex = ( person.GetSex() == PM_MALE ) ? 0 : msg_jf_phrase_max/2 ;
  return randomMsgId + offsetSex;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アンケートアトラクション用にアンケート回答を設定
 * @param[in]  ダミーデータのパーソナルデータ
 * @param[in]  questionnaireId  アンケートID
 * @param[in]  isHit  正解の回答を入れるか trueで入れる。falseでハズレ回答
 *
 * @return  回答値
 */
/* ----------------------------------------------------------------------------------------- */
u8 JoinFestaAttractionUtil::GetQuestionnaireAnswwerFromDummyData( const NetAppLib::JoinFesta::JoinFestaPersonalData& person, const u8 questionnaireId, const bool isHit )
{
  NetAppLib::JoinFesta::JoinFestaAttractionManager* pAttractionManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager);

  GFL_ASSERT( pAttractionManager->IsPlaying() );

  const JoinFestaScript::AttractionId attractionId = pAttractionManager->GetPlayAttractionId();

  gfl2::util::FixedSizeContainer<u8, 13> answerList;

  const u8 answerMax = GetAnswerMaxFromQuestionnaireId( questionnaireId );
  // 全回答をチェック
  //  memo:回答0は未回答なので含めない
  for( u8 answerNo=1; answerNo<=answerMax; answerNo++ )
  {
    // 加点対象（=正解）か
    bool isScoreUp = pAttractionManager->IsQuestionAnswerScoreUp( attractionId, answerNo );

    if( isHit == isScoreUp )
    {
      answerList.push_back( answerNo );
    }
  }

  GFL_ASSERT( answerList.size() );
  if( answerList.size() == 1 )
  {
    // 1つしかないので固定
    return answerList[0];
  }
  else if( answerList.size() > 1 )
  {
    u32 randomValue = System::GflUse::GetPublicRand( (answerList.size()) );
    return answerList[ randomValue ];
  }
  return 0;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクションIDからポケモンタイプを取得
 */
/* ----------------------------------------------------------------------------------------- */
u8 JoinFestaAttractionUtil::GetPokeTypeFromAttractionId( const JoinFestaScript::AttractionId attractionId )
{
  switch( attractionId ){
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_1:               //!< ノーマルタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_1:         //!< ノーマルタイプすきを　さがせ！（アローラずかん）
    return POKETYPE_NORMAL;
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_2:               //!< かくとうタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_2:         //!< かくとうタイプすきを　さがせ！（アローラずかん）
    return POKETYPE_KAKUTOU;
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_3:               //!< ひこうタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_3:         //!< ひこうタイプすきを　さがせ！（アローラずかん）
    return POKETYPE_HIKOU;
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_4:               //!< どくタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_4:         //!< どくタイプすきを　さがせ！（アローラずかん）
    return POKETYPE_DOKU;
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_5:               //!< じめんタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_5:         //!< じめんタイプすきを　さがせ！（アローラずかん）
    return POKETYPE_JIMEN;
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_6:               //!< いわタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_6:         //!< いわタイプすきを　さがせ！（アローラずかん）
    return POKETYPE_IWA;
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_7:               //!< むしタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_7:         //!< むしタイプすきを　さがせ！（アローラずかん）
    return POKETYPE_MUSHI;
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_8:               //!< ゴーストタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_8:         //!< ゴーストタイプすきを　さがせ！（アローラずかん）
    return POKETYPE_GHOST;
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_9:               //!< はがねタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_9:         //!< はがねタイプすきを　さがせ！（アローラずかん）
    return POKETYPE_HAGANE;
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_10:              //!< ほのおタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_10:         //!< ほのおタイプすきを　さがせ！（アローラずかん）
    return POKETYPE_HONOO;
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_11:              //!< みずタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_11:        //!< みずタイプすきを　さがせ！（アローラずかん）
    return POKETYPE_MIZU;
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_12:              //!< くさタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_12:        //!< くさタイプすきを　さがせ！（アローラずかん）
    return POKETYPE_KUSA;
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_13:              //!< でんきタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_13:        //!< でんきタイプすきを　さがせ！（アローラずかん）
    return POKETYPE_DENKI;
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_14:              //!< エスパータイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_14:        //!< エスパータイプすきを　さがせ！（アローラずかん）
    return POKETYPE_ESPER;
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_15:              //!< こおりタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_15:        //!< こおりタイプすきを　さがせ！（アローラずかん）
    return POKETYPE_KOORI;
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_16:              //!< ドラゴンタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_16:        //!< ドラゴンタイプすきを　さがせ！（アローラずかん）
    return POKETYPE_DRAGON;
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_17:              //!< あくタイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_17:        //!< あくタイプすきを　さがせ！（アローラずかん）
    return POKETYPE_AKU;
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_18:              //!< フェアリータイプすきを　さがせ！
  case JoinFestaScript::ATTRACTION_TYPE_SEARCH_ALOLA_18:        //!< フェアリータイプすきを　さがせ！（アローラずかん）
    return POKETYPE_FAIRY;

  default:
    //GFL_ASSERT(0);
    return POKETYPE_NORMAL;
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクションIDからアンケートIDを取得
 */
/* ----------------------------------------------------------------------------------------- */
u8 JoinFestaAttractionUtil::GetQuestionnaireIdFromAttractionId( const JoinFestaScript::AttractionId attractionId )
{
  u8 questionnaireId = 0; // アンケートアトラクションに対応したアンケートID
  switch( attractionId ){
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_1:             //!< ベテラントレーナーに　あいさつ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_2:             //!< しんじんトレーナーに　ひとこと！
    questionnaireId = 15;
    break;
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_3:             //!< ラブラブな　ひとを　あつめろ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_4:             //!< クールなひとを　あつめろ！
    questionnaireId = 0;
    break;
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_5:             //!< おとこともだち　ちょうさ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_6:             //!< おんなともだち　ちょうさ！
    questionnaireId = 2;
    break;
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_7:             //!< あにき　あねきを　みつけろ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_8:             //!< まんなかっこを　みつけろ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_9:             //!< すえっこを　みつけろ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_10:            //!< ひとりっこを　みつけろ！
    questionnaireId = 1;
    break;
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_11:            //!< なかみが　だいじなひとを　さがせ！
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_12:            //!< がいけんが　だいじなひとを　さがせ！
    questionnaireId = 11;
    break;
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_13:            //!< ひみつはまもるもの
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_14:            //!< ひみつはもらすもの
    questionnaireId = 6;
    break;
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_15:            //!< 休日はまったり
  case JoinFestaScript::ATTRACTION_QUESTIONNAIRE_16:            //!< 休日はアクティブ
    questionnaireId = 4;
    break;
  default:
    GFL_ASSERT(0);
    break;
  }
  return questionnaireId;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アンケートIDから回答数を取得
 */
/* ----------------------------------------------------------------------------------------- */
u8 JoinFestaAttractionUtil::GetAnswerMaxFromQuestionnaireId( const u8 questionnaireId )
{
  static const u8 answerMaxTbl[16] ={ 4, 5, 9, 5, 12, 10, 4, 4, 3, 8, 6, 3, 7, 9, 8, 7 };

  if( questionnaireId < 16 )
  {
    return answerMaxTbl[ questionnaireId ];
  }
  GFL_ASSERT(0);
  return 1;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   タイプ相性とタイプ好きで抽選されたポケモン番号をリセット
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionUtil::ResetSaveHitPokeType()
{
  for( u8 i=0; i<PERSONAL_MAX; i++ )
  {
    m_saveHitPokeTypeTable[i] = POKETYPE_NULL;
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   施設案内アトラクション可能かどうか
 * @param   案内する施設（持っていないといけない）
 * @retval  true : プレイ出来る
 * @retval  false : プレイ出来ない
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaAttractionUtil::CheckFacilitiesAttractionPlayEnable( const JoinFestaScript::AttractionId attractionId, const JoinFestaScript::FacilitiesType type )
{
  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

  for( u32 i=0; i<JoinFestaScript::FACILITIES_INDEX_MAX; i++ )
  {
    JoinFestaScript::FacilitiesType myType = NetApp::JoinFesta::ShopItemInfoTable::GetFacilitiesType( pSave->GetFacilitiesData( static_cast<JoinFestaScript::FacilitiesIndex>(i) )->facilitiesId );
    // 持っている施設のタイプと一致するかチェック
    if( myType == type )
    {// 1つでも持っていればOK
      return true;
    }
  }
  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   人物比率でフィールドリスト作成
 * @param[in]   attractionId    アトラクションID
 * @param[in]  guestNum        ゲストの人数
 * @param[in]  vipNum          VIPの人数
 * @param[in]  npcMaxNum      NPCの最大人数
 */
/* ----------------------------------------------------------------------------------------- */
// @static_function
void JoinFestaAttractionUtil::CreateFieldPersonalList( const JoinFestaScript::AttractionId attractionId, const u32 guestNum, const u32 vipNum, const u32 npcMaxNum )
{
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);

  // リストに追加した人物カウンタ
  u8 listPersonalCount = 0;

  // VIP探す
  if( vipNum > 0 )
  {
    listPersonalCount += CreateFieldPersonalListForRelation( attractionId, NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP, vipNum );
  }
  // Guest探す
  if( guestNum > 0 )
  {
    listPersonalCount += CreateFieldPersonalListForRelation( attractionId, NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST, guestNum );
  }

  // 埋まらなかった分はダミー
  u8 dummyCount = 0;
  if( npcMaxNum > 0 )
  {
    for( ; listPersonalCount<npcMaxNum; listPersonalCount++ )
    {
      // ダミーの空データを追加する
      NetAppLib::JoinFesta::JoinFestaPersonalData dummyData;
      // 出現状態にさせておく
      dummyData.SetAttractionAppearanceState( NetAppLib::JoinFesta::JoinFestaAttractionDefine::APPEARANCE_STATE_OK );
      // ダミーフラグ立てる
      dummyData.SetupAttractionDummyData( dummyCount++ );
      pJoinFestaPersonalDataManager->AddFieldPersonalList( dummyData );
    }
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   関係別のリスト作成
 * @param[in]  attractionId    アトラクションID
 * @param[in]  relation        関係
 * @param[in]  num             人数
 * @return      追加した人数
 */
/* ----------------------------------------------------------------------------------------- */
// @static_function
u32 JoinFestaAttractionUtil::CreateFieldPersonalListForRelation( const JoinFestaScript::AttractionId attractionId, const NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION relation, const u32 maxNum )
{
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);

  NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList *list = pJoinFestaPersonalDataManager->GetPersonalDataList( relation );
  u8 addCount = 0;

  if( list != NULL && list->Size() > 0 )
  {
    for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it )
    {
      NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);

      // アンケートアトラクションだった場合、回答見る
      if( JoinFestaScript::ATTRACTION_QUESTIONNAIRE_1 <= attractionId && attractionId <= JoinFestaScript::ATTRACTION_QUESTIONNAIRE_16 )
      {
        // 受信していないのは見ない
        NetAppLib::JoinFesta::PacketGameStatus1 data1;
        bool bResult = pPersonalData->GetPacketGameStatus1( &data1 );
        if( bResult == false )
        {
          continue;
        }

        // アンケートアトラクションに対応したアンケートID取得
        u8 questionnaireId = GetQuestionnaireIdFromAttractionId( attractionId );
   
        // 回答(1以上)しているかチェック
        if( data1.questionnaireAnswer[ questionnaireId ] == 0 )
        { // 回答していないので除外
          continue;
        }
      }

      // 未出現だったら出現させる
      if( pPersonalData != NULL && pPersonalData->GetAttractionAppearanceState() == NetAppLib::JoinFesta::JoinFestaAttractionDefine::APPEARANCE_STATE_NOT )
      {
        // 出現状態にさせておく
        pPersonalData->SetAttractionAppearanceState( NetAppLib::JoinFesta::JoinFestaAttractionDefine::APPEARANCE_STATE_OK );
        // 同じ情報でフィールドリストに追加する
        pJoinFestaPersonalDataManager->AddFieldPersonalList( *pPersonalData );
        addCount++;

        if( addCount == maxNum )
        {
          ICHI_PRINT(">ICHI Relation%d %d人数満たした \n", relation, addCount);
          break;
        }
      }
    } // for
  }

  return addCount;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   着せ替えセット
 * @param[in]  ダミーデータのパーソナルデータ
 * @param[in]  セットする着せ替えのパターン
 */
/* ----------------------------------------------------------------------------------------- */
// @static_function
void JoinFestaAttractionUtil::SetDressUpPattern( NetAppLib::JoinFesta::JoinFestaPersonalData& person, DressUpParamPattern dressUpPattern, const NetApp::JoinFesta::JoinFestaDataTable* pDataTable )
{
  // ドレスアップパラメータ
  poke_3d::model::DressUpParam dressUpParam;

  u32 kisekaeId = 0;

  switch( dressUpPattern ){
  case DRESS_UP_PARAM_PATTERN_DEFAULT:   // A,R,Sからランダム
    {
      kisekaeId = System::GflUse::GetPublicRand( 101 );
      if( kisekaeId > 81 )
      {
        kisekaeId = System::GflUse::GetPublicRand( 20 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_R_00;
      }
      else if( kisekaeId > 61 )
      {
        kisekaeId = System::GflUse::GetPublicRand( 20 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_S_00;
      }
    }
    break;
  case DRESS_UP_PARAM_PATTERN_MY_COPY:   // 自分の着せ替え状況のコピー
    {
      // 自分と同じ着せ替えにする
      Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
      poke_3d::model::DressUpParam dressUpParam;
      dressUpParam = pMyStatus->GetDressup();
      person.SetupAttractionDummyDataDressup( dressUpParam );
    }
    return;

  case DRESS_UP_PARAM_PATTERN_MY_COPY2:  // 自分の着せ替え状況のコピーしてアルゴリズムで一部変更
    {
      // 自分と同じ着せ替えにする
      Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
      poke_3d::model::DressUpParam dressUpParam;
      dressUpParam = pMyStatus->GetDressup();

      u32 sex = dressUpParam.bodyParams[ poke_3d::model::DressUpParam::BODY_CATEGORY_SEX ];

      // @fix NMCat[2018]：ランダムで１～３個のパーツを変更する
      // パーツ変更数
      const u32 changeNum = System::GflUse::GetPublicRand( 3 );
      bool changeTbl[3];
      for( u32 i=0; i<3; i++ )
      {
        changeTbl[i] = (i<=changeNum) ? true : false;
      }
      // 適当にシャッフル
      for( u32 i=0; i<10; i++ )
      {
        u32 randomValue = System::GflUse::GetPublicRand( 3 );
        bool tmp = changeTbl[randomValue];
        changeTbl[randomValue] = changeTbl[0];
        changeTbl[0] = tmp;
      }

      if( changeTbl[0] == true )
      { // アイアクセサリ
        dressUpParam.itemParams[ poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCEYE ] = m_acceyeTable[ sex ][ System::GflUse::GetPublicRand( 5 ) ];
      }
      if( changeTbl[1] == true )
      { // バッグアクセサリ
        dressUpParam.itemParams[ poke_3d::model::DressUpParam::ITEM_CATEGORY_BAG ] = m_bagTable[ sex ][ System::GflUse::GetPublicRand( 5 ) ];
      }
      if( changeTbl[2] == true )
      { // 靴アクセサリ
        dressUpParam.itemParams[ poke_3d::model::DressUpParam::ITEM_CATEGORY_SHOES ] = m_shoesTable[ sex ][ System::GflUse::GetPublicRand( 5 ) ];
      }

      person.SetupAttractionDummyDataDressup( dressUpParam );
    }
    return;

  case DRESS_UP_PARAM_PATTERN_A:         // A(基本)からランダム
    kisekaeId = System::GflUse::GetPublicRand( 61 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_A_00;
    break;
  case DRESS_UP_PARAM_PATTERN_A_MALE:    // A(基本)からランダム(男のみ）
    kisekaeId = System::GflUse::GetPublicRand( 30 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_A_31;
    break;
  case DRESS_UP_PARAM_PATTERN_A_FEMALE:  // A(基本)からランダム(女のみ）
    kisekaeId = System::GflUse::GetPublicRand( 31 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_A_00;
    break;
  case DRESS_UP_PARAM_PATTERN_B:         // B(赤：正解)からランダム
    kisekaeId = System::GflUse::GetPublicRand( 12 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_B_00;
    break;
  case DRESS_UP_PARAM_PATTERN_C:         // C(黄：正解)からランダム
    kisekaeId = System::GflUse::GetPublicRand( 12 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_C_00;
    break;
  case DRESS_UP_PARAM_PATTERN_D:         // D(緑：正解)からランダム
    kisekaeId = System::GflUse::GetPublicRand( 12 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_D_00;
    break;
  case DRESS_UP_PARAM_PATTERN_E:         // E(青：正解)からランダム
    kisekaeId = System::GflUse::GetPublicRand( 12 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_E_00;
    break;
  case DRESS_UP_PARAM_PATTERN_F:         // F(橙：正解)からランダム
    kisekaeId = System::GflUse::GetPublicRand( 12 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_F_00;
    break;
  case DRESS_UP_PARAM_PATTERN_G:         // G(藍：正解)からランダム
    kisekaeId = System::GflUse::GetPublicRand( 12 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_G_00;
    break;
  case DRESS_UP_PARAM_PATTERN_H:         // H(紫：正解)からランダム
    kisekaeId = System::GflUse::GetPublicRand( 12 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_H_00;
    break;
  case DRESS_UP_PARAM_PATTERN_I:         // I(桃：正解)からランダム
    kisekaeId = System::GflUse::GetPublicRand( 12 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_I_00;
    break;
  case DRESS_UP_PARAM_PATTERN_J:         // J(赤：不正解)からランダム
    kisekaeId = System::GflUse::GetPublicRand( 7 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_J_00;
    break;
  case DRESS_UP_PARAM_PATTERN_K:         // K(黄：不正解)からランダム
    kisekaeId = System::GflUse::GetPublicRand( 7 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_K_00;
    break;
  case DRESS_UP_PARAM_PATTERN_L:         // L(緑：不正解)からランダム
    kisekaeId = System::GflUse::GetPublicRand( 7 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_L_00;
    break;
  case DRESS_UP_PARAM_PATTERN_M:         // M(青：不正解)からランダム
    kisekaeId = System::GflUse::GetPublicRand( 7 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_M_00;
    break;
  case DRESS_UP_PARAM_PATTERN_N:         // N(橙：不正解)からランダム
    kisekaeId = System::GflUse::GetPublicRand( 7 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_N_00;
    break;
  case DRESS_UP_PARAM_PATTERN_O:         // O(藍：不正解)からランダム
    kisekaeId = System::GflUse::GetPublicRand( 7 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_O_00;
    break;
  case DRESS_UP_PARAM_PATTERN_P:         // P(紫：不正解)からランダム
    kisekaeId = System::GflUse::GetPublicRand( 7 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_P_00;
    break;
  case DRESS_UP_PARAM_PATTERN_Q:         // Q(桃：不正解)からランダム
    kisekaeId = System::GflUse::GetPublicRand( 7 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_Q_00;
    break;
  case DRESS_UP_PARAM_PATTERN_R:         // R(帽子×眼鏡)からランダム
    kisekaeId = System::GflUse::GetPublicRand( 20 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_R_00;
    break;
  case DRESS_UP_PARAM_PATTERN_S:         // Sからランダム
    kisekaeId = System::GflUse::GetPublicRand( 20 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_S_00;
    break;
  case DRESS_UP_PARAM_PATTERN_COLD:      // 寒色(青藍紫)からランダム
    {
      // E,G,Hからランダム
      u32 randomColor = System::GflUse::GetPublicRand( 3 );
      if( randomColor == 0 )
      {
        kisekaeId = System::GflUse::GetPublicRand( 12 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_E_00;
      }
      else if( randomColor == 1 )
      {
        kisekaeId = System::GflUse::GetPublicRand( 12 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_G_00;
      }
      else
      {
        kisekaeId = System::GflUse::GetPublicRand( 12 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_H_00;
      }
    }
    break;
  case DRESS_UP_PARAM_PATTERN_WARM:      // 暖色(赤黄緑橙桃)からランダム
    {
      // B,C,D,F,Iからランダム
      u32 randomColor = System::GflUse::GetPublicRand( 5 );

      switch( randomColor ){
      case 0: kisekaeId = System::GflUse::GetPublicRand( 12 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_B_00; break;
      case 1: kisekaeId = System::GflUse::GetPublicRand( 12 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_C_00; break;
      case 2: kisekaeId = System::GflUse::GetPublicRand( 12 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_D_00; break;
      case 3: kisekaeId = System::GflUse::GetPublicRand( 12 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_F_00; break;
      case 4: kisekaeId = System::GflUse::GetPublicRand( 12 ) + NetApp::JoinFesta::KisekaePatternInfoTable::KISEKAE_ID_I_00; break;
      }
    }
    break;
  }

  // 着せ替えパターンで着せ替えデータ反映
  JoinFestaPersonalDataUtil::SetDressUpPattern( person, kisekaeId, pDataTable );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アンケートアトラクション時の挨拶文
 * @param[in]  ダミーデータのパーソナルデータ
 *
 * @return  定型文挨拶のmsgID
 */
/* ----------------------------------------------------------------------------------------- */
// @static_function
u32 JoinFestaAttractionUtil::GetPatternHelloForAttractionQuestionnaire( const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  NetAppLib::JoinFesta::JoinFestaAttractionManager* pAttractionManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager);
  const JoinFestaScript::AttractionId attractionId = pAttractionManager->GetPlayAttractionId();

  // アンケートアトラクション時は、当たりの人のメッセージを専用にする
  NetAppLib::JoinFesta::PacketGameStatus1 packet;
  bool ret = person.GetPacketGameStatus1( &packet );
  GFL_ASSERT(ret);

  // アンケートアトラクションに対応したアンケートID取得
  u8 questionnaireId = GetQuestionnaireIdFromAttractionId( attractionId );

  // 正解かチェック
  if( pAttractionManager->IsQuestionAnswerScoreUp( attractionId, packet.questionnaireAnswer[ questionnaireId ] ) )
  {
    static const u32 wordWindowMessageId[][2] = {
      { msg_jf_phrase_m_367, msg_jf_phrase_f_367 },      //!< ベテラントレーナーに　あいさつ！
      { msg_jf_phrase_m_368, msg_jf_phrase_f_368 },      //!< しんじんトレーナーに　ひとこと！
      { msg_jf_phrase_m_369, msg_jf_phrase_f_369 },      //!< ラブラブな　ひとを　あつめろ！
      { msg_jf_phrase_m_370, msg_jf_phrase_f_370 },      //!< クールなひとを　あつめろ！
      { msg_jf_phrase_m_371, msg_jf_phrase_f_371 },      //!< おとこともだち　ちょうさ！
      { msg_jf_phrase_m_372, msg_jf_phrase_f_372 },      //!< おんなともだち　ちょうさ！
      { msg_jf_phrase_m_373, msg_jf_phrase_f_373 },      //!< あにき　あねきを　みつけろ！
      { msg_jf_phrase_m_374, msg_jf_phrase_f_374 },      //!< まんなかっこを　みつけろ！
      { msg_jf_phrase_m_375, msg_jf_phrase_f_375 },      //!< すえっこを　みつけろ！
      { msg_jf_phrase_m_376, msg_jf_phrase_f_376 },     //!< ひとりっこを　みつけろ！
      { msg_jf_phrase_m_378, msg_jf_phrase_f_378 },     //!< なかみが　だいじなひとを　さがせ！
      { msg_jf_phrase_m_377, msg_jf_phrase_f_377 },     //!< がいけんが　だいじなひとを　さがせ！
      { msg_jf_phrase_m_379, msg_jf_phrase_f_379 },     //!< ひみつはまもるもの
      { msg_jf_phrase_m_380, msg_jf_phrase_f_380 },     //!< ひみつはもらすもの
      { msg_jf_phrase_m_381, msg_jf_phrase_f_381 },     //!< 休日はまったり
      { msg_jf_phrase_m_382, msg_jf_phrase_f_382 },     //!< 休日はアクティブ
    };

    ICHI_PRINT(">ICHI アンケート正解の一言ウィンドウ\n");
    // アトラクションIDを0オリジンにする
    const u8 tableIndex = attractionId - JoinFestaScript::ATTRACTION_QUESTIONNAIRE_1;
    return wordWindowMessageId[ tableIndex ][ person.GetSex() ];
  }

  return MSG_ID_NULL;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   施設案内アトラクション時の挨拶文
 * @param[in]  ダミーデータのパーソナルデータ
 *
 * @return  定型文挨拶のmsgID
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaAttractionUtil::GetPatternHelloForAttractionRecommendationShop( const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  // 施設案内アトラクション時は、イベントIDからメッセージ取得して返す
  if( person.IsAttractionPersonalEventEnable() )
  {
    return JoinFestaPersonalEvent::GetWordWindowMessageIdFromPersonalEvent( person.GetAttractionPersonalEventFake(), person.GetSex() );
  }
  return MSG_ID_NULL;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   言語レッスンアトラクション時の挨拶文
 * @param[in]  ダミーデータのパーソナルデータ
 *
 * @return  定型文挨拶のmsgID
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaAttractionUtil::GetPatternHelloForAttractionLanguageLesson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  const u32 personSex = person.GetSex();

  // @fix GFNMcat[1957]：ランダムで選んでしまうと、一言ウィンドウを出す度にランダムになる。TIDで固定にする。
  const u32 msgTableIndex = person.GetTransferedID() % 3;

  // 言語レッスンの場合は、定型：あいさつを差し替える
  u32 languageMsgId = 0;
  // 言語と同じあいさつにする
  switch( person.GetLanguageId() ){
  case POKEMON_LANG_JAPAN:
    {
      const u32 languageHelloMsgId[][2] = {
        { msg_jf_phrase_m_324, msg_jf_phrase_f_324 }, // こんにちは
        { msg_jf_phrase_m_332, msg_jf_phrase_f_332 }, // ありがとう
        { msg_jf_phrase_m_340, msg_jf_phrase_f_340 } // バイバイ
      };
      languageMsgId = languageHelloMsgId[ msgTableIndex ][ personSex ];
    }
    break;

  case POKEMON_LANG_ENGLISH:
    {
      const u32 languageHelloMsgId[][2] = {
        { msg_jf_phrase_m_325, msg_jf_phrase_f_325 }, // ハロー
        { msg_jf_phrase_m_333, msg_jf_phrase_f_333 }, // サンキュー
        { msg_jf_phrase_m_341, msg_jf_phrase_f_341 } // バーイ
      };
      languageMsgId = languageHelloMsgId[ msgTableIndex ][ personSex ];
    }
    break;
  case POKEMON_LANG_FRANCE:
    {
      const u32 languageHelloMsgId[][2] = {
        { msg_jf_phrase_m_326, msg_jf_phrase_f_326 }, // ボンジュール
        { msg_jf_phrase_m_334, msg_jf_phrase_f_334 }, // メルシー
        { msg_jf_phrase_m_342, msg_jf_phrase_f_342 } // オ フヴォワーフ
      };
      languageMsgId = languageHelloMsgId[ msgTableIndex ][ personSex ];
    }
    break;
  case POKEMON_LANG_ITALY:
    {
      const u32 languageHelloMsgId[][2] = {
        { msg_jf_phrase_m_327, msg_jf_phrase_f_327 }, // ボンジョルノ
        { msg_jf_phrase_m_335, msg_jf_phrase_f_335 }, // グラッツィエ
        { msg_jf_phrase_m_343, msg_jf_phrase_f_343 } // チャオ
      };
      languageMsgId = languageHelloMsgId[ msgTableIndex ][ personSex ];
    }
    break;
  case POKEMON_LANG_GERMANY:
    {
      const u32 languageHelloMsgId[][2] = {
        { msg_jf_phrase_m_328, msg_jf_phrase_f_328 }, // グーテンターク
        { msg_jf_phrase_m_336, msg_jf_phrase_f_336 }, // ダンケ
        { msg_jf_phrase_m_344, msg_jf_phrase_f_344 } // チュース
      };
      languageMsgId = languageHelloMsgId[ msgTableIndex ][ personSex ];
    }
    break;
  case POKEMON_LANG_SPAIN:
    {
      const u32 languageHelloMsgId[][2] = {
        { msg_jf_phrase_m_329, msg_jf_phrase_f_329 }, // オーラ
        { msg_jf_phrase_m_337, msg_jf_phrase_f_337 }, // グラシアス
        { msg_jf_phrase_m_345, msg_jf_phrase_f_345 } // アデオス
      };
      languageMsgId = languageHelloMsgId[ msgTableIndex ][ personSex ];
    }
    break;
  case POKEMON_LANG_KOREA:
    {
      const u32 languageHelloMsgId[][2] = {
        { msg_jf_phrase_m_330, msg_jf_phrase_f_330 }, // アンニョンハセヨ
        { msg_jf_phrase_m_338, msg_jf_phrase_f_338 }, // コマウォ
        { msg_jf_phrase_m_346, msg_jf_phrase_f_346 } // アンニョンヒ カセヨ
      };
      languageMsgId = languageHelloMsgId[ msgTableIndex ][ personSex ];
    }
    break;
  case POKEMON_LANG_CHINA:
  case POKEMON_LANG_TAIWAN:
    {
      const u32 languageHelloMsgId[][2] = {
        { msg_jf_phrase_m_331, msg_jf_phrase_f_331 }, // ニーハオ
        { msg_jf_phrase_m_339, msg_jf_phrase_f_339 }, // シェイシェイ
        { msg_jf_phrase_m_347, msg_jf_phrase_f_347 } // ザイジィエン
      };
      languageMsgId = languageHelloMsgId[ msgTableIndex ][ personSex ];
    }
    break;
  default:
    GFL_ASSERT_MSG(0,"%d", languageMsgId);
    break;
  }

  // 定型：あいさつ差し替え
  if( languageMsgId != 0 )
  {
    return languageMsgId;
  }

  return MSG_ID_NULL;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   カイリキーライドアトラクション時の挨拶文
 * @param[in]  ダミーデータのパーソナルデータ
 *
 * @return  定型文挨拶のmsgID
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaAttractionUtil::GetPatternHelloForAttractionRideKairiky( const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  NetAppLib::JoinFesta::JoinFestaAttractionManager* pAttractionManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager);

  // 加点対象の人は、定型分のうれしいカテゴリのテキストにする
  if( pAttractionManager->IsKairikyRideScoreUp( &person ) )
  {
    Field::Fieldmap* pFieldmap = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap();

    // 城内ギミックのみ
    if( pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA )
    {
      Field::FieldGimmick*          pFieldGimmick = pFieldmap->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE );
      Field::FieldGimmickJoinFesta* pJfGimmick = static_cast<Field::FieldGimmickJoinFesta*>(pFieldGimmick);
      NetApp::JoinFesta::JoinFestaResourceManager*  pJfResourceManager = pJfGimmick->GetJoinFestaResourceManager();

      // 定型文テーブル
      NetAppLib::JoinFesta::JoinFestaSelectWordTable* pSelectWordTable = pJfResourceManager->GetSelectWordTable();

      u32 messageTableSize = pSelectWordTable->GetMessageTableSize( JoinFestaSelectWordTable::CATEGORY_JOY );
      JoinFestaSelectWordTable::Message* pMessage = pSelectWordTable->GetMessageTable( JoinFestaSelectWordTable::CATEGORY_JOY );

      // @fix cov_ctr[11760]：ゼロ除算を防ぐ
      if( pMessage && messageTableSize != 0 )
      {
        // @fix GFNMcat[1957]：ランダムで選んでしまうと、一言ウィンドウを出す度にランダムになる。TIDで固定にする。
        const u32 msgTableIndex = person.GetTransferedID() % messageTableSize;

        return pMessage[ msgTableIndex ].messageID;
      }
    }
  }
  return MSG_ID_NULL;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   タイプ相性アトラクション開催中のダミーデータのあいさつ取得
 * @param[in]  ダミーデータのパーソナルデータ
 *
 * @return  定型文挨拶のmsgID
 */
/* ----------------------------------------------------------------------------------------- */
// @static_function
u32 JoinFestaAttractionUtil::GetPatternHelloForAttractionAffinity( const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  u8 pokeType1, pokeType2;
  const int offsetSex = ( person.GetSex() == PM_MALE ) ? 0 : msg_jf_phrase_max/2 ;
  u32 messageId = 0;

  // ポケモンタイプ取得
  person.GetAttractionPokeType( pokeType1, pokeType2 );

  switch( pokeType1 ){
  case POKETYPE_NORMAL:   messageId = msg_jf_phrase_m_392; break;
  case POKETYPE_KAKUTOU:  messageId = msg_jf_phrase_m_398; break;
  case POKETYPE_HIKOU:    messageId = msg_jf_phrase_m_401; break;
  case POKETYPE_DOKU:     messageId = msg_jf_phrase_m_399; break;
  case POKETYPE_JIMEN:    messageId = msg_jf_phrase_m_400; break;
  case POKETYPE_IWA:      messageId = msg_jf_phrase_m_404; break;
  case POKETYPE_MUSHI:    messageId = msg_jf_phrase_m_403; break;
  case POKETYPE_GHOST:    messageId = msg_jf_phrase_m_405; break;
  case POKETYPE_HAGANE:   messageId = msg_jf_phrase_m_408; break;
  case POKETYPE_HONOO:    messageId = msg_jf_phrase_m_393; break;
  case POKETYPE_MIZU:     messageId = msg_jf_phrase_m_394; break;
  case POKETYPE_KUSA:     messageId = msg_jf_phrase_m_396; break;
  case POKETYPE_DENKI:    messageId = msg_jf_phrase_m_395; break;
  case POKETYPE_ESPER:    messageId = msg_jf_phrase_m_402; break;
  case POKETYPE_KOORI:    messageId = msg_jf_phrase_m_397; break;
  case POKETYPE_DRAGON:   messageId = msg_jf_phrase_m_406; break;
  case POKETYPE_AKU:      messageId = msg_jf_phrase_m_407; break;
  case POKETYPE_FAIRY:    messageId = msg_jf_phrase_m_409; break;
    // defaultは来るので許容
  //default:
    //GFL_ASSERT(0);
    //break;
  }

  return messageId + offsetSex;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   施設案内アトラクションで発動するパーソナルイベント決める
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionUtil::SetupFakePersonalEvent( const JoinFestaScript::AttractionId attractionId, NetAppLib::JoinFesta::JoinFestaPersonalData& person, const bool isHit )
{
  const u8 fakeHitPersonalEventTableIndex = attractionId - JoinFestaScript::ATTRACTION_RECOMMENDATION_SHOP_1;

  u8 selectEventTable[9] = {0};
  u8 selectEventTableCount = 0;
  u8 fakePersonalEvent;

  // アトラクション毎のテーブルから選んで良いパーソナルイベントテーブル作る
  for( u32 i=0; i<9; i++ )
  {
    // @fix NMCat[1791]：「わたしのきもちわかる」は除外する
    {
      u8 personalEventId = i + JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP0;
      if( personalEventId == JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP6 || 
        personalEventId == JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP7 )
      {
        continue;
      }
    }

    if( isHit )
    {
      if( m_fakeHitPersonalEventTable[fakeHitPersonalEventTableIndex][i] == 1 )
      {
        selectEventTable[ selectEventTableCount++ ] = i;
      }
    }
    else
    {
      if( m_fakeHitPersonalEventTable[fakeHitPersonalEventTableIndex][i] == 0 )
      {
        selectEventTable[ selectEventTableCount++ ] = i;
      }
    }
  }

  ICHI_PRINT("Fakeイベントテーブル\n");
  for( u32 i=0; i<selectEventTableCount; i++ )
  {
    ICHI_PRINT("%d, ", selectEventTable[i]);
  }
  ICHI_PRINT("\n");


  // ランダムでイベント決める
  fakePersonalEvent = selectEventTable[ System::GflUse::GetPublicRand(selectEventTableCount) ];
  ICHI_PRINT("Fakeイベント:%d \n", fakePersonalEvent );
  fakePersonalEvent += JoinFestaScript::PERSONAL_EVENT_RECOMMENDATION_SHOP0;

  person.SetupAttractionPersonalEventFake( fakePersonalEvent );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   言語レッスンで、出現人物の言語決める
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionUtil::SetupLanguageTbl( const JoinFestaScript::AttractionId attractionId, u8 outLanguageTbl[PERSONAL_MAX] )
{
  switch( attractionId ){
  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_JP:
    {
      const u8 ngLanguageIdTbl[] = {
        POKEMON_LANG_ENGLISH,
        POKEMON_LANG_FRANCE,
        POKEMON_LANG_ITALY,
        POKEMON_LANG_GERMANY,
        POKEMON_LANG_SPAIN,
        POKEMON_LANG_KOREA,
        POKEMON_LANG_CHINA,
        POKEMON_LANG_TAIWAN,
      };

      int i;
      // 当たりは固定で５人
      for( i=0; i<5; i++ ){ outLanguageTbl[i] = POKEMON_LANG_JAPAN; }
      // ハズレは当たり以外の言語でランダム
      for( ; i<PERSONAL_MAX; i++ ){ outLanguageTbl[i] = ngLanguageIdTbl[ System::GflUse::GetPublicRand( 8 ) ]; }
    }
    break;
  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_EN:
    {
      const u8 ngLanguageIdTbl[] = {
        POKEMON_LANG_JAPAN,
        POKEMON_LANG_FRANCE,
        POKEMON_LANG_ITALY,
        POKEMON_LANG_GERMANY,
        POKEMON_LANG_SPAIN,
        POKEMON_LANG_KOREA,
        POKEMON_LANG_CHINA,
        POKEMON_LANG_TAIWAN,
      };

      int i;
      // 当たりは固定で５人
      for( i=0; i<5; i++ ){ outLanguageTbl[i] = POKEMON_LANG_ENGLISH; }
      // ハズレは当たり以外の言語でランダム
      for( ; i<PERSONAL_MAX; i++ ){ outLanguageTbl[i] = ngLanguageIdTbl[ System::GflUse::GetPublicRand( 8 ) ]; }
    }
    break;
  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_FR:
    {
      const u8 ngLanguageIdTbl[] = {
        POKEMON_LANG_JAPAN,
        POKEMON_LANG_ENGLISH,
        POKEMON_LANG_ITALY,
        POKEMON_LANG_GERMANY,
        POKEMON_LANG_SPAIN,
        POKEMON_LANG_KOREA,
        POKEMON_LANG_CHINA,
        POKEMON_LANG_TAIWAN,
      };

      int i;
      // 当たりは固定で５人
      for( i=0; i<5; i++ ){ outLanguageTbl[i] = POKEMON_LANG_FRANCE; }
      // ハズレは当たり以外の言語でランダム
      for( ; i<PERSONAL_MAX; i++ ){ outLanguageTbl[i] = ngLanguageIdTbl[ System::GflUse::GetPublicRand( 8 ) ]; }
    }
    break;
  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_IT:
    {
      const u8 ngLanguageIdTbl[] = {
        POKEMON_LANG_JAPAN,
        POKEMON_LANG_ENGLISH,
        POKEMON_LANG_FRANCE,
        POKEMON_LANG_GERMANY,
        POKEMON_LANG_SPAIN,
        POKEMON_LANG_KOREA,
        POKEMON_LANG_CHINA,
        POKEMON_LANG_TAIWAN,
      };

      int i;
      // 当たりは固定で５人
      for( i=0; i<5; i++ ){ outLanguageTbl[i] = POKEMON_LANG_ITALY; }
      // ハズレは当たり以外の言語でランダム
      for( ; i<PERSONAL_MAX; i++ ){ outLanguageTbl[i] = ngLanguageIdTbl[ System::GflUse::GetPublicRand( 8 ) ]; }
    }
    break;
  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_GE:
    {
      const u8 ngLanguageIdTbl[] = {
        POKEMON_LANG_JAPAN,
        POKEMON_LANG_ENGLISH,
        POKEMON_LANG_FRANCE,
        POKEMON_LANG_ITALY,
        POKEMON_LANG_SPAIN,
        POKEMON_LANG_KOREA,
        POKEMON_LANG_CHINA,
        POKEMON_LANG_TAIWAN,
      };

      int i;
      // 当たりは固定で５人
      for( i=0; i<5; i++ ){ outLanguageTbl[i] = POKEMON_LANG_GERMANY; }
      // ハズレは当たり以外の言語でランダム
      for( ; i<PERSONAL_MAX; i++ ){ outLanguageTbl[i] = ngLanguageIdTbl[ System::GflUse::GetPublicRand( 8 ) ]; }
    }
    break;
  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_SP:
    {
      const u8 ngLanguageIdTbl[] = {
        POKEMON_LANG_JAPAN,
        POKEMON_LANG_ENGLISH,
        POKEMON_LANG_FRANCE,
        POKEMON_LANG_ITALY,
        POKEMON_LANG_GERMANY,
        POKEMON_LANG_KOREA,
        POKEMON_LANG_CHINA,
        POKEMON_LANG_TAIWAN,
      };

      int i;
      // 当たりは固定で５人
      for( i=0; i<5; i++ ){ outLanguageTbl[i] = POKEMON_LANG_SPAIN; }
      // ハズレは当たり以外の言語でランダム
      for( ; i<PERSONAL_MAX; i++ ){ outLanguageTbl[i] = ngLanguageIdTbl[ System::GflUse::GetPublicRand( 8 ) ]; }
    }
    break;
  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_KR:
    {
      const u8 ngLanguageIdTbl[] = {
        POKEMON_LANG_JAPAN,
        POKEMON_LANG_ENGLISH,
        POKEMON_LANG_FRANCE,
        POKEMON_LANG_ITALY,
        POKEMON_LANG_GERMANY,
        POKEMON_LANG_SPAIN,
        POKEMON_LANG_CHINA,
        POKEMON_LANG_TAIWAN,
      };

      int i;
      // 当たりは固定で５人
      for( i=0; i<5; i++ ){ outLanguageTbl[i] = POKEMON_LANG_KOREA; }
      // ハズレは当たり以外の言語でランダム
      for( ; i<PERSONAL_MAX; i++ ){ outLanguageTbl[i] = ngLanguageIdTbl[ System::GflUse::GetPublicRand( 8 ) ]; }
    }
    break;
  case JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_CT:
    {
      const u8 ngLanguageIdTbl[] = {
        POKEMON_LANG_JAPAN,
        POKEMON_LANG_ENGLISH,
        POKEMON_LANG_FRANCE,
        POKEMON_LANG_ITALY,
        POKEMON_LANG_GERMANY,
        POKEMON_LANG_SPAIN,
        POKEMON_LANG_KOREA,
      };

      int i;
      // 当たりは固定で５人
      for( i=0; i<2; i++ ){ outLanguageTbl[i] = POKEMON_LANG_CHINA; }
      for(    ; i<5; i++ ){ outLanguageTbl[i] = POKEMON_LANG_TAIWAN; }
      // ハズレは当たり以外の言語でランダム
      for( ; i<PERSONAL_MAX; i++ ){ outLanguageTbl[i] = ngLanguageIdTbl[ System::GflUse::GetPublicRand( 7 ) ]; }
    }
    break;
  default:
    GFL_ASSERT(0);
    break;
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   タイプ相性診断で出るポケモンテーブル計算
 * @param[out]   算出したポケモンテーブル
 * @param[in]    データテーブル
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionUtil::CalcAffinityMonsNoTable( u16 (&outMonsNoTable)[PERSONAL_MAX], const NetApp::JoinFesta::JoinFestaDataTable* pDataTable )
{
  u32 level1MonsNoTable[ NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_1_ID_MAX ] = {0};
  u32 level2MonsNoTable[ NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_2_ID_MAX ] = {0};
  u32 level3MonsNoTable[ NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_3_ID_MAX ] = {0};
  u8  level1MonsNoCount = 0;
  u8  level2MonsNoCount = 0;
  u8  level3MonsNoCount = 0;

  // バイナリデータから各難易度レベルのポケモン番号テーブル作成する
  for( int i=0; i<NetApp::JoinFesta::AffinityAttractionInfoTable::AFFINITY_ID_MAX; i++ )
  {
    const NetApp::JoinFesta::AffinityAttractionInfo* pAffinityAttractionInfo = pDataTable->GetAffinityAttractionInfo( i );

    bool isSame = false;
    // 前回のポケモンと被っていればテーブルに追加しない（抽選しない）
    for( u8 i=0; i<PERSONAL_MAX; i++ )
    {
      if( pAffinityAttractionInfo->monsNo == m_saveHitPokeTypeTable[i] )
      {
        ICHI_PRINT(">ICHI 前回ポケモンとかぶっていた %d \n", m_saveHitPokeTypeTable[i]);
        isSame = true; 
        break;
      }
    }

    if( isSame == true )
    {
      continue;;
    }

    switch( pAffinityAttractionInfo->level ){
    case NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_1:
      level1MonsNoTable[ level1MonsNoCount++ ] = pAffinityAttractionInfo->monsNo;
      break;
    case NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_2:
      level2MonsNoTable[ level2MonsNoCount++ ] = pAffinityAttractionInfo->monsNo;
      break;
    case NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_3:
      level3MonsNoTable[ level3MonsNoCount++ ] = pAffinityAttractionInfo->monsNo;
      break;
    }
  }

  // 各テーブルをシャッフルする
  // 適当に15回くらい
  for( int i=0; i<15; i++ )
  {
    u32 tmp;

    if( level1MonsNoCount > 0 )
    {
      u8 level1ChangeIndex = System::GflUse::GetPublicRand(level1MonsNoCount);
      tmp = level1MonsNoTable[ level1ChangeIndex ];
      level1MonsNoTable[ level1ChangeIndex ] = level1MonsNoTable[ 0 ];
      level1MonsNoTable[ 0 ] = tmp;
    }

    if( level2MonsNoCount > 0 )
    {
      u8 level2ChangeIndex = System::GflUse::GetPublicRand(level2MonsNoCount);
      tmp = level2MonsNoTable[ level2ChangeIndex ];
      level2MonsNoTable[ level2ChangeIndex ] = level2MonsNoTable[ 0 ];
      level2MonsNoTable[ 0 ] = tmp;
    }

    if( level3MonsNoCount > 0 )
    {
      u8 level3ChangeIndex = System::GflUse::GetPublicRand(level3MonsNoCount);
      tmp = level3MonsNoTable[ level3ChangeIndex ];
      level3MonsNoTable[ level3ChangeIndex ] = level3MonsNoTable[ 0 ];
      level3MonsNoTable[ 0 ] = tmp;
    }
  }


  // 難易度レベルテーブル
  static const u8 levelTable[ PERSONAL_MAX ] = {
    NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_1,
    NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_1,
    NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_2,
    NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_2,
    NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_2,
    NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_2,
    NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_2,
    NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_2,
    NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_2,
    NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_2,
    NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_2,
    NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_2,
    NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_3,
    NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_3,
    NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_3,
    NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_3,
  };


  level1MonsNoCount = 0;
  level2MonsNoCount = 0;
  level3MonsNoCount = 0;
  for( int i=0; i<PERSONAL_MAX; i++ )
  {
    switch( levelTable[i] ){
    case NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_1:
      outMonsNoTable[i] = level1MonsNoTable[ level1MonsNoCount++ ];
      break;
    case NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_2:
      outMonsNoTable[i] = level2MonsNoTable[ level2MonsNoCount++ ];
      break;
    case NetApp::JoinFesta::AffinityAttractionInfoTable::DIFFICULTY_LEVEL_3:
      outMonsNoTable[i] = level3MonsNoTable[ level3MonsNoCount++ ];
      break;
    }

    ICHI_PRINT(">ICHI 選出ポケモン番号[%d:%d]\n", i, outMonsNoTable[i] );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   タイプ好き探せで、ポケモンを決める
 * @param[out]   person       パーソナルデータ
 * @param[in]    isAlola      アローラ種ならtrue。全国ならfalse。
 * @param[in]    hitPokeTyp   正解のポケモンタイプ
 * @param[in]    lotteryPokeTyp  抽選するポケモンタイプ
 * @param[in]    ngMonsNo     NGのポケモン番号
 * @param[in]    pDataTable   データテーブル
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionUtil::SetSearchPokemon( NetAppLib::JoinFesta::JoinFestaPersonalData& person, const bool isAlola, const u8 hitPokeType, const u8 lotteryPokeType, const u16 ngMonsNo, const NetApp::JoinFesta::JoinFestaDataTable* pDataTable )
{
  gfl2::heap::HeapBase *pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  u32 lotteryMonsNoTable[ 100 ] = {0};
  u32 lotteryFormNoTable[ 100 ] = {0};
  u32 lotteryMonsNoCount = 0;

  const u32 tableCountMax = isAlola ? NetApp::JoinFesta::PokeTypeAttractionAlolaInfoTable::POKE_TYPE_ID_MAX : NetApp::JoinFesta::PokeTypeAttractionAllInfoTable::POKE_TYPE_ID_MAX;

  for( int i=0; i<tableCountMax; i++ )
  {
    if( isAlola )
    {
      const NetApp::JoinFesta::PokeTypeAttractionInfo* pPokeTypeAttractionInfo = pDataTable->GetPokeTypeAttractionAlolaInfo( i );
      if( pPokeTypeAttractionInfo->pokeType == lotteryPokeType )
      {
        lotteryMonsNoTable[ lotteryMonsNoCount ] = pPokeTypeAttractionInfo->monsNo;
        lotteryFormNoTable[ lotteryMonsNoCount ] = pPokeTypeAttractionInfo->formNo;
        lotteryMonsNoCount++;
      }
    }
    else
    {
      const NetApp::JoinFesta::PokeTypeAttractionInfo* pPokeTypeAttractionInfo = pDataTable->GetPokeTypeAttractionAllInfo( i );
      if( pPokeTypeAttractionInfo->pokeType == lotteryPokeType )
      {
        lotteryMonsNoTable[ lotteryMonsNoCount ] = pPokeTypeAttractionInfo->monsNo;
        lotteryFormNoTable[ lotteryMonsNoCount ] = pPokeTypeAttractionInfo->formNo;
        lotteryMonsNoCount++;
      }
    }
  }

  // ポケモン抽選
  bool isLoop = true;
  do{

    u32 randomValue = System::GflUse::GetPublicRand(lotteryMonsNoCount);
    u32 targetMonsNo = lotteryMonsNoTable[ randomValue ]; 
    u32 targetFormNo = lotteryFormNoTable[ randomValue ]; 
    ICHI_PRINT(">ICHI 抽選タイプ%d　のポケモン数 %d 抽選したポケモンNo%d \n", lotteryPokeType, lotteryMonsNoCount, targetMonsNo );

    // 抽選したポケモンのタイプを取得
    pml::pokepara::PokemonParam* pp;
    pp = GFL_NEW( pHeap->GetLowerHandle() ) pml::pokepara::PokemonParam( pHeap->GetLowerHandle(), static_cast<MonsNo>(targetMonsNo), 98, pml::pokepara::INIT_SPEC_ID_RANDOM );

    if( targetFormNo > 0 )
    {
      pp->ChangeFormNo( targetFormNo );
    }

    // 正解ポケモンを選ぶ場合は、NGポケモンをチェック
    if( hitPokeType == lotteryPokeType )
    {
      if( ngMonsNo != targetMonsNo )
      {
        isLoop = false; // 抽選終了
      }
    }
    // タイプに正解のポケモンタイプが含まれているとダメ
    else if( !( pp->GetType1() == hitPokeType || pp->GetType2() == hitPokeType ) )
    {
      bool isSame = false;
      // 前回のポケモンと被っていなければOK
      for( u8 i=0; i<PERSONAL_MAX; i++ )
      {
        if( targetMonsNo == m_saveHitPokeTypeTable[i] )
        {
          ICHI_PRINT(">ICHI 前回ポケモンとかぶっていた %d \n", targetMonsNo);
          isSame = true; 
          break;
        }
      }

      if( isSame == false )
      {
        isLoop = false; // 抽選終了
      }
    }

    if( isLoop == false )
    {
      // パーソナルにポケモンタイプを保存
      person.SetAttractionPokeType( pp->GetType1(), pp->GetType2() );
      person.SetAttractionMonsNo( targetMonsNo );
      ICHI_PRINT(">ICHI 決定ポケモンNo%d タイプ[%d/%d] \n", targetMonsNo, pp->GetType1(), pp->GetType2() );
    }

    GFL_DELETE pp;
  }while( isLoop );


}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   タイプ好きのテーブル作る
 * @param[in]    hitPokeTyp   正解のポケモンタイプ
 * @param[out]   outTypeTable タイプがばらばらになるように作ったタイプテーブル
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionUtil::CalcSearchPokeTypeTable( const u8 hitPokeType, u8 (&outTypeTable)[PERSONAL_MAX] )
{
  // 正解タイプから2匹、正解以外のタイプからバラバラのタイプで14匹選ぶ
  outTypeTable[0] = hitPokeType;
  outTypeTable[1] = hitPokeType;

  // 選ばれないタイプを４つ選ぶ
  u8 ngTypeTable[4] = { POKETYPE_MAX, POKETYPE_MAX, POKETYPE_MAX, POKETYPE_MAX };
  {
    ngTypeTable[0] = hitPokeType; // 正解は必ず外す
    for( int i=1; i<4; i++ )
    {
      u8 randomType;
      bool isLoop = true;
      do{
        randomType = System::GflUse::GetPublicRand( POKETYPE_MAX );

        bool isSame = false;
        for( int j=0; j<4; j++ )
        {
          if( randomType == ngTypeTable[ j ] )
          {// 被ってたらダメ
            isSame = true;
            break;
          }
        }

        // １つも被っていなかったら採用
        if( isSame == false )
        {
          isLoop = false;
        }

      }while(isLoop);

      ngTypeTable[ i ] = randomType;
      ICHI_PRINT(">ICHI 選ばれないタイプ %d \n", ngTypeTable[i]);
    }
  }

  // 全タイプを回して、NGタイプ以外を入れていく
  u8 pokeTypeTableIndex = 2;
  for( int pokeType=0; pokeType<POKETYPE_MAX; pokeType++ )
  {
    bool isNgType = false;
    for( int i=0; i<4; i++ )
    {
      if( ngTypeTable[i] == pokeType )
      {
        isNgType = true;
        break;
      }
    }

    if( isNgType == false )
    {
      outTypeTable[ pokeTypeTableIndex++ ] = pokeType;
    }
  }

}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   タイプ相性とタイプ好きで抽選されたポケモン番号を保存
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionUtil::SaveHitPokeType()
{
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
  u8 tableIndex = 0;
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = pJoinFestaPersonalDataManager->GetFieldPersonalList();
  for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
  {
    NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData = (*it);

    // NPCの場合
    if( pPersonalData != NULL && pPersonalData->IsAttractionDummyData() ) 
    {
      // 保存していたものを保存
      m_saveHitPokeTypeTable[ tableIndex ] = pPersonalData->GetAttractionMonsNo();
      ICHI_PRINT(">ICHI 抽選したタイプ保存する [%d]%d \n", tableIndex, m_saveHitPokeTypeTable[ tableIndex ]);
      tableIndex++;
    }
  }
}



GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetAppLib)

