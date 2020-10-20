//==============================================================================
/**
 * @file	JoinFestaPersonalDataUtil.cpp
 * @brief	ジョインフェスタパーソナルデータUtil
 * @author	ichiraku_katsuhiko
 * @date	2016/03/22　12:52
 */
// =============================================================================
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataUtil.h"

// ライブラリ
#include <base/include/gfl2_Singleton.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <str/include/gfl2_StrBuf.h>

// system
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// Save
#include "Savedata/include/JoinFestaDataSave.h"

// 定型文用msgId
#include  <niji_conv_header/message/msg_jf_phrase.h>
#include  <niji_conv_header/message/msg_setphrase.h>

// ジョインフェスタ施設テーブル定義
#include "NetApp/JoinFesta/include/JoinFestaDataTable/JoinFestaDataTable.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFesta)

/************************************************************************/
/* 初期定型文MSGのテーブル                                         */
/************************************************************************/
const u32 JoinFestaPersonalDataUtil::PATTERN_MSG_TABLE[ JoinFestaPersonalDataUtil::INIT_PATTENR_TABLE_NUM ][ 4 ] = {
  { msg_jf_phrase_m_316,msg_jf_phrase_m_470,msg_jf_phrase_m_267,msg_jf_phrase_m_304 },
  { msg_jf_phrase_m_317,msg_jf_phrase_m_423,msg_jf_phrase_m_268,msg_jf_phrase_m_505 },
  { msg_jf_phrase_m_318,msg_jf_phrase_m_321,msg_jf_phrase_m_275,msg_jf_phrase_m_502 },
  { msg_jf_phrase_m_319,msg_jf_phrase_m_320,msg_jf_phrase_m_277,msg_jf_phrase_m_283 },
  { msg_jf_phrase_m_322,msg_jf_phrase_m_474,msg_jf_phrase_m_271,msg_jf_phrase_m_422 },
  { msg_jf_phrase_m_478,msg_jf_phrase_m_469,msg_jf_phrase_m_272,msg_jf_phrase_m_479 },
  { msg_jf_phrase_m_424,msg_jf_phrase_m_475,msg_jf_phrase_m_270,msg_jf_phrase_m_421 },
  { msg_jf_phrase_m_300,msg_jf_phrase_m_301,msg_jf_phrase_m_466,msg_jf_phrase_m_298 },
  { msg_jf_phrase_m_287,msg_jf_phrase_m_292,msg_jf_phrase_m_285,msg_jf_phrase_m_288 },
  { msg_jf_phrase_m_490,msg_jf_phrase_m_492,msg_jf_phrase_m_476,msg_jf_phrase_m_501 },
  { msg_jf_phrase_m_460,msg_jf_phrase_m_459,msg_jf_phrase_m_280,msg_jf_phrase_m_281 },
  { msg_jf_phrase_m_462,msg_jf_phrase_m_486,msg_jf_phrase_m_439,msg_jf_phrase_m_465 },
  { msg_jf_phrase_m_498,msg_jf_phrase_m_512,msg_jf_phrase_m_273,msg_jf_phrase_m_500 },
  { msg_jf_phrase_m_314,msg_jf_phrase_m_313,msg_jf_phrase_m_306,msg_jf_phrase_m_504 },
}; 

/************************************************************************/
/* 初期定型文カテゴリMSGのテーブル                                         */
/************************************************************************/
const u32 JoinFestaPersonalDataUtil::CATEGORY_MSG_TABLE[ JoinFestaPersonalDataUtil::INIT_PATTENR_TABLE_NUM ][ 4 ] = {
  {	msg_setphrase_categ_14,	msg_setphrase_categ_14,	msg_setphrase_categ_09,	msg_setphrase_categ_13 },
  {	msg_setphrase_categ_14,	msg_setphrase_categ_14,	msg_setphrase_categ_09,	msg_setphrase_categ_13 },
  {	msg_setphrase_categ_14,	msg_setphrase_categ_14,	msg_setphrase_categ_09,	msg_setphrase_categ_10 },
  {	msg_setphrase_categ_14,	msg_setphrase_categ_14,	msg_setphrase_categ_09,	msg_setphrase_categ_10 },
  {	msg_setphrase_categ_14,	msg_setphrase_categ_15,	msg_setphrase_categ_09,	msg_setphrase_categ_13 },
  {	msg_setphrase_categ_14,	msg_setphrase_categ_14,	msg_setphrase_categ_09,	msg_setphrase_categ_11 },
  {	msg_setphrase_categ_14,	msg_setphrase_categ_17,	msg_setphrase_categ_09,	msg_setphrase_categ_15 },
  {	msg_setphrase_categ_12,	msg_setphrase_categ_12,	msg_setphrase_categ_12,	msg_setphrase_categ_12 },
  {	msg_setphrase_categ_10,	msg_setphrase_categ_11,	msg_setphrase_categ_10,	msg_setphrase_categ_10 },
  {	msg_setphrase_categ_17,	msg_setphrase_categ_17,	msg_setphrase_categ_09,	msg_setphrase_categ_15 },
  {	msg_setphrase_categ_15,	msg_setphrase_categ_15,	msg_setphrase_categ_09,	msg_setphrase_categ_10 },
  {	msg_setphrase_categ_14,	msg_setphrase_categ_15,	msg_setphrase_categ_15,	msg_setphrase_categ_11 },
  {	msg_setphrase_categ_12,	msg_setphrase_categ_12,	msg_setphrase_categ_09,	msg_setphrase_categ_13 },
  {	msg_setphrase_categ_13,	msg_setphrase_categ_13,	msg_setphrase_categ_13,	msg_setphrase_categ_13 },
};

/**
 *  @brief  初期定型文MSGテーブル取得
 */
u32 JoinFestaPersonalDataUtil::GetInitPatternMsg( const u32 tblIndex, const u32 patternIndex )
{
  if( tblIndex < INIT_PATTENR_TABLE_NUM && patternIndex < 4 )
  {
    return PATTERN_MSG_TABLE[ tblIndex ][ patternIndex ];
  }

  GFL_ASSERT(0);
  return PATTERN_MSG_TABLE[ 0 ][ 0 ];
}

/**
 *  @brief  初期定型文カテゴリMSGテーブル取得
 */
u32 JoinFestaPersonalDataUtil::GetInitCategoryMsg( const u32 tblIndex, const u32 patternIndex )
{
  if( tblIndex < INIT_PATTENR_TABLE_NUM && patternIndex < 4 )
  {
    return CATEGORY_MSG_TABLE[ tblIndex ][ patternIndex ];
  }

  GFL_ASSERT(0);
  return CATEGORY_MSG_TABLE[ 0 ][ 0 ];
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   着せ替えセット
 * @param[in]  ダミーデータのパーソナルデータ
 * @param[in]  セットする着せ替えのパターン
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPersonalDataUtil::SetDressUpPattern( NetAppLib::JoinFesta::JoinFestaPersonalData& person, const u32 kisekaeId, const NetApp::JoinFesta::JoinFestaDataTable* pDataTable )
{
  // ドレスアップパラメータ
  poke_3d::model::DressUpParam dressUpParam;

  // テーブルから着せ替えパラメータ引っ張ってくる
  {
    ICHI_PRINT(">ICHI NPCの着せ替えパターンID %d \n", kisekaeId);
    const NetApp::JoinFesta::KisekaePatternInfo* pKisekaePatternInfo = pDataTable->GetKisekaePatternInfo( kisekaeId );

    dressUpParam.bodyParams[ poke_3d::model::DressUpParam::BODY_CATEGORY_SEX             ] = pKisekaePatternInfo->sex;
    dressUpParam.bodyParams[ poke_3d::model::DressUpParam::BODY_CATEGORY_BODY_COLOR      ] = pKisekaePatternInfo->body_color;     // 体の色      
    dressUpParam.bodyParams[ poke_3d::model::DressUpParam::BODY_CATEGORY_EYE_COLOR       ] = pKisekaePatternInfo->eye_color;      // 目の色
    dressUpParam.bodyParams[ poke_3d::model::DressUpParam::BODY_CATEGORY_HAIR_COLOR      ] = pKisekaePatternInfo->hair_color;     // 髪の色
    dressUpParam.bodyParams[ poke_3d::model::DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR] = pKisekaePatternInfo->female_lip_color;// リップの色

    dressUpParam.itemParams[ poke_3d::model::DressUpParam::ITEM_CATEGORY_FACE            ] = pKisekaePatternInfo->face;           // 顔
    dressUpParam.itemParams[ poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE      ] = pKisekaePatternInfo->hair_style;     // 髪型
    dressUpParam.itemParams[ poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCBADGE        ] = -1;          // バッジアクセサリ
    dressUpParam.itemParams[ poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCEYE          ] = pKisekaePatternInfo->acceye;         // アイアクセサリ
    dressUpParam.itemParams[ poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR         ] = pKisekaePatternInfo->acchair;        // ヘアアクセサリ
    dressUpParam.itemParams[ poke_3d::model::DressUpParam::ITEM_CATEGORY_BAG             ] = pKisekaePatternInfo->bag;            // バッグ
    dressUpParam.itemParams[ poke_3d::model::DressUpParam::ITEM_CATEGORY_BNGL            ] = pKisekaePatternInfo->bngl;           // バングル
    dressUpParam.itemParams[ poke_3d::model::DressUpParam::ITEM_CATEGORY_BODY            ] = pKisekaePatternInfo->body;           // ボディ
    dressUpParam.itemParams[ poke_3d::model::DressUpParam::ITEM_CATEGORY_BOTTOMS         ] = pKisekaePatternInfo->bottoms;        // ボトムス
    dressUpParam.itemParams[ poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT             ] = pKisekaePatternInfo->hat;            // 帽子
    dressUpParam.itemParams[ poke_3d::model::DressUpParam::ITEM_CATEGORY_LEGS            ] = pKisekaePatternInfo->legs;           // レッグ
    dressUpParam.itemParams[ poke_3d::model::DressUpParam::ITEM_CATEGORY_SHOES           ] = pKisekaePatternInfo->shoes;          // シューズ
    dressUpParam.itemParams[ poke_3d::model::DressUpParam::ITEM_CATEGORY_TOPS            ] = pKisekaePatternInfo->tops;           // トップス
  }

  // 着せ替えデータ反映
  person.SetupAttractionDummyDataDressup( dressUpParam );
}


GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetAppLib)

