#include "System/include/DressUp.h"

#include <System/include/PokemonVersion.h>
#include <arc_def_index/arc_def.h>

#include <System/include/GflUse.h>

namespace System {

using namespace poke_3d::model;

#if PM_DEBUG
  u32 DressUpParamSaveData::s_debugVersion = DressUpParamSaveData::DRESSUP_VERSION_NIJI; // デバッグ用のセーブデータバージョン
#endif

static s32 DRESSUP_ARCID_PARTS_LIST_BATTLE[] =
{
  ARCID_CHARA_MODEL_BATTLE_HERO_COLOR,
  ARCID_CHARA_MODEL_BATTLE_HERO_FACE,
  ARCID_CHARA_MODEL_BATTLE_HERO_HAIR,
  -1,
  ARCID_CHARA_MODEL_BATTLE_HERO_ACCEYE,
  -1,
  ARCID_CHARA_MODEL_BATTLE_HERO_BAG,
  ARCID_CHARA_MODEL_BATTLE_HERO_BNGL,
  -1,
  ARCID_CHARA_MODEL_BATTLE_HERO_BOTTOMS,
  ARCID_CHARA_MODEL_BATTLE_HERO_HAT,
  ARCID_CHARA_MODEL_BATTLE_HERO_LEGS,
  ARCID_CHARA_MODEL_BATTLE_HERO_SHOES,
  ARCID_CHARA_MODEL_BATTLE_HERO_TOPS,
  ARCID_CHARA_MODEL_BATTLE_HERO_SITU,

  ARCID_CHARA_MODEL_BATTLE_HEROINE_COLOR,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_FACE,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_HAIR,
  -1,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_ACCEYE,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_ACCHAIR,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_BAG,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_BNGL,
  -1,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_BOTTOMS,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_HAT,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_LEGS,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_SHOES,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_TOPS,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_SITU,
};

static s32 DRESSUP_ARCID_PARTS_LIST_FIELD[] =
{
  ARCID_CHARA_MODEL_FIELD_HERO_COLOR,
  ARCID_CHARA_MODEL_FIELD_HERO_FACE,
  ARCID_CHARA_MODEL_FIELD_HERO_HAIR,
  -1,
  ARCID_CHARA_MODEL_FIELD_HERO_ACCEYE,
  -1,
  ARCID_CHARA_MODEL_FIELD_HERO_BAG,
  ARCID_CHARA_MODEL_FIELD_HERO_BNGL,
  -1,
  ARCID_CHARA_MODEL_FIELD_HERO_BOTTOMS,
  ARCID_CHARA_MODEL_FIELD_HERO_HAT,
  ARCID_CHARA_MODEL_FIELD_HERO_LEGS,
  ARCID_CHARA_MODEL_FIELD_HERO_SHOES,
  ARCID_CHARA_MODEL_FIELD_HERO_TOPS,
  ARCID_CHARA_MODEL_FIELD_HERO_SITU,

  ARCID_CHARA_MODEL_FIELD_HEROINE_COLOR,
  ARCID_CHARA_MODEL_FIELD_HEROINE_FACE,
  ARCID_CHARA_MODEL_FIELD_HEROINE_HAIR,
  -1,
  ARCID_CHARA_MODEL_FIELD_HEROINE_ACCEYE,
  ARCID_CHARA_MODEL_FIELD_HEROINE_ACCHAIR,
  ARCID_CHARA_MODEL_FIELD_HEROINE_BAG,
  ARCID_CHARA_MODEL_FIELD_HEROINE_BNGL,
  -1,
  ARCID_CHARA_MODEL_FIELD_HEROINE_BOTTOMS,
  ARCID_CHARA_MODEL_FIELD_HEROINE_HAT,
  ARCID_CHARA_MODEL_FIELD_HEROINE_LEGS,
  ARCID_CHARA_MODEL_FIELD_HEROINE_SHOES,
  ARCID_CHARA_MODEL_FIELD_HEROINE_TOPS,
  ARCID_CHARA_MODEL_FIELD_HEROINE_SITU,
};

/**
 * @brief バトル用のArcIdリストを取得
 */
s32* DressUp::GetDressUpArcIdListBattle()
{
  STATIC_ASSERT((sizeof(DRESSUP_ARCID_PARTS_LIST_BATTLE) / sizeof(DRESSUP_ARCID_PARTS_LIST_BATTLE[0])) == (poke_3d::model::DressUpParam::ITEM_CATEGORY_COUNT * 2 + 2)); // +2はcolor
  return DRESSUP_ARCID_PARTS_LIST_BATTLE;
}
  
/**
 * @brief フィールド用のArcIdリストを取得
 */
s32* DressUp::GetDressUpArcIdListField()
{
  STATIC_ASSERT((sizeof(DRESSUP_ARCID_PARTS_LIST_FIELD) / sizeof(DRESSUP_ARCID_PARTS_LIST_FIELD[0])) == (poke_3d::model::DressUpParam::ITEM_CATEGORY_COUNT * 2 + 2)); // +2はcolor
  return DRESSUP_ARCID_PARTS_LIST_FIELD;
}

/**
 * @brief 男主人公のデフォルトの着せ替えパラメータ取得
 */
void DressUp::GetDressUpParamDefaultHero(poke_3d::model::DressUpParam* out_pDressUpParam)
{
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_SEX             ] = DressUpParam::SEX_MALE;                     // 性別
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_BODY_COLOR      ] = DRESSUP_COLOR_ID_SKIN_SYE;                  // 体の色      
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_EYE_COLOR       ] = DRESSUP_COLOR_ID_EYE_EBK;                   // 目の色
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_HAIR_COLOR      ] = DRESSUP_COLOR_ID_HAIR_HBK;                  // 髪の色
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR] = DRESSUP_COLOR_ID_LIP_LOF;                   // リップの色

  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_FACE            ] = DRESSUP_ITEM_ID_P1_FACE01;                  // 顔
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_HAIR_STYLE      ] = DRESSUP_ITEM_ID_P1_HAIR_MMJ_SHORT;          // 髪型
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCBADGE        ] = -1;                                         // バッジアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCEYE          ] = -1;                                         // アイアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCHAIR         ] = -1;                                         // ヘアアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BAG             ] = DRESSUP_ITEM_ID_P1_BAG_MMJ_DRUCK01;             // バッグ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BNGL            ] = DRESSUP_ITEM_ID_P1_BNGL_ZENBRESS01;         // バングル
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BODY            ] = -1;                                         // ボディ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BOTTOMS         ] = DRESSUP_ITEM_ID_P1_BOTTOMS_MMJ_DPAN01;      // ボトムス
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_HAT             ] = DRESSUP_ITEM_ID_P1_HAT_MMJ_DHAT01;              // 帽子
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_LEGS            ] = DRESSUP_ITEM_ID_P1_LEGS_MMJ_DLEG01;         // レッグ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_SHOES           ] = DRESSUP_ITEM_ID_P1_SHOES_MMJ_DSHOES01;          // シューズ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_TOPS            ] = DRESSUP_ITEM_ID_P1_TOPS_MMJ_DTANK01;        // トップス
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_SITU            ] = -1;                                         // シチュエーション着替え
}

/**
 * @brief 女主人公のデフォルトの着せ替えパラメータ取得
 */
void DressUp::GetDressUpParamDefaultHeroine(poke_3d::model::DressUpParam* out_pDressUpParam)
{
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_SEX             ] = DressUpParam::SEX_FEMALE;                   // 性別
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_BODY_COLOR      ] = DRESSUP_COLOR_ID_SKIN_SYE;                  // 体の色      
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_EYE_COLOR       ] = DRESSUP_COLOR_ID_EYE_EBK;                   // 目の色
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_HAIR_COLOR      ] = DRESSUP_COLOR_ID_HAIR_HBK;                  // 髪の色
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR] = DRESSUP_COLOR_ID_LIP_LOF;                   // リップの色

  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_FACE            ] = DRESSUP_ITEM_ID_P2_FACE01;                  // 顔
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_HAIR_STYLE      ] = DRESSUP_ITEM_ID_P2_HAIR_MMJ_SPONY;          // 髪型
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCBADGE        ] = -1;                                         // バッジアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCEYE          ] = -1;                                         // アイアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCHAIR         ] = -1;                                         // ヘアアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BAG             ] = DRESSUP_ITEM_ID_P2_BAG_MMJ_DBAG01;              // バッグ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BNGL            ] = DRESSUP_ITEM_ID_P2_BNGL_ZENBRESS01;         // バングル
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BODY            ] = -1;                                         // ボディ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BOTTOMS         ] = DRESSUP_ITEM_ID_P2_BOTTOMS_MMJ_DHOTPANTS01;  // ボトムス
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_HAT             ] = DRESSUP_ITEM_ID_P2_HAT_MMJ_DHAT01;              // 帽子
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_LEGS            ] = DRESSUP_ITEM_ID_P2_LEGS_BAREFOOT01;         // レッグ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_SHOES           ] = DRESSUP_ITEM_ID_P2_SHOES_MMJ_DSHOES01;          // シューズ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_TOPS            ] = DRESSUP_ITEM_ID_P2_TOPS_MMJ_DCUT01;         // トップス
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_SITU            ] = -1;                                         // シチュエーション着替え
}

/**
 * @brief デフォルトの着せ替えパラメータ取得：性別は引数で受け取る
 */
void DressUp::GetDressUpParamDefault(u32 sex, poke_3d::model::DressUpParam* out_pDressUpParam)
{
  if (sex == PM_MALE)
  {
    GetDressUpParamDefaultHero(out_pDressUpParam);
  }
  else
  {
    GetDressUpParamDefaultHeroine(out_pDressUpParam);
  }
}



/**
 *  @brief 男主人公　ライドスーツの着せ替えパラメータ取得
 *  @param  is_land   true:陸用  false:海用
 */
void DressUp::GetDressUpParamRideHero(bool is_land, poke_3d::model::DressUpParam* out_pDressUpParam)
{
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_SEX             ] = DressUpParam::SEX_MALE;                     // 性別
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_BODY_COLOR      ] = DRESSUP_COLOR_ID_SKIN_SYE;                  // 体の色      
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_EYE_COLOR       ] = DRESSUP_COLOR_ID_EYE_EBK;                   // 目の色
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_HAIR_COLOR      ] = DRESSUP_COLOR_ID_HAIR_HBK;                  // 髪の色
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR] = DRESSUP_COLOR_ID_LIP_LOF;                   // リップの色

  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_FACE            ] = DRESSUP_ITEM_ID_P1_FACE01;                  // 顔
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_HAIR_STYLE      ] = -1;                                         // 髪型
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCBADGE        ] = -1;                                         // バッジアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCEYE          ] = -1;                                         // アイアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCHAIR         ] = -1;                                         // ヘアアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BAG             ] = -1;                                         // バッグ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BNGL            ] = -1;                                         // バングル
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BODY            ] = -1;                                         // ボディ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BOTTOMS         ] = -1;                                         // ボトムス
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_HAT             ] = -1;                                         // 帽子
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_LEGS            ] = -1;                                         // レッグ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_SHOES           ] = -1;                                         // シューズ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_TOPS            ] = -1;                                         // トップス
  if( is_land )
  {
    out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_SITU            ] = DRESSUP_ITEM_ID_P1_SITU_RIDELS01;           // シチュエーション着替え
  }
  else
  {
    out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_SITU            ] = DRESSUP_ITEM_ID_P1_SITU_RIDESEA01;           // シチュエーション着替え
  }
}

/**
 *  @brief 女主人公　ライドスーツの着せ替えパラメータ取得
 *  @param  is_land   true:陸用  false:海用
 */
void DressUp::GetDressUpParamRideHeroine(bool is_land, poke_3d::model::DressUpParam* out_pDressUpParam)
{
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_SEX             ] = DressUpParam::SEX_FEMALE;                   // 性別
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_BODY_COLOR      ] = DRESSUP_COLOR_ID_SKIN_SYE;                  // 体の色      
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_EYE_COLOR       ] = DRESSUP_COLOR_ID_EYE_EBK;                   // 目の色
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_HAIR_COLOR      ] = DRESSUP_COLOR_ID_HAIR_HBK;                  // 髪の色
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR] = DRESSUP_COLOR_ID_LIP_LOF;                   // リップの色

  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_FACE            ] = DRESSUP_ITEM_ID_P2_FACE01;                  // 顔
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_HAIR_STYLE      ] = -1;                                         // 髪型
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCBADGE        ] = -1;                                         // バッジアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCEYE          ] = -1;                                         // アイアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCHAIR         ] = -1;                                         // ヘアアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BAG             ] = -1;                                         // バッグ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BNGL            ] = -1;                                         // バングル
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BODY            ] = -1;                                         // ボディ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BOTTOMS         ] = -1;                                         // ボトムス
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_HAT             ] = -1;                                         // 帽子
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_LEGS            ] = -1;                                         // レッグ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_SHOES           ] = -1;                                         // シューズ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_TOPS            ] = -1;                                         // トップス
  if( is_land )
  {
    out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_SITU            ] = DRESSUP_ITEM_ID_P2_SITU_RIDELS01;           // シチュエーション着替え
  }
  else
  {
    out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_SITU            ] = DRESSUP_ITEM_ID_P2_SITU_RIDESEA01;           // シチュエーション着替え
  }

}

/**
 * @brief ライドスーツの着せ替えパラメータ取得：性別は引数で受け取る
 *  @param  is_land   true:陸用  false:海用
 */
void DressUp::GetDressUpParamRide(u32 sex, bool is_land, poke_3d::model::DressUpParam* out_pDressUpParam)
{
  if (sex == PM_MALE)
  {
    GetDressUpParamRideHero(is_land, out_pDressUpParam);
  }
  else
  {
    GetDressUpParamRideHeroine(is_land, out_pDressUpParam);
  }
}

/**
 *  @brief 男主人公　宇宙服の着せ替えパラメータ取得
 */
void DressUp::GetDressUpParamSpaceHero(poke_3d::model::DressUpParam* out_pDressUpParam)
{
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_SEX             ] = DressUpParam::SEX_MALE;                     // 性別
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_BODY_COLOR      ] = DRESSUP_COLOR_ID_SKIN_SYE;                  // 体の色      
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_EYE_COLOR       ] = DRESSUP_COLOR_ID_EYE_EBK;                   // 目の色
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_HAIR_COLOR      ] = DRESSUP_COLOR_ID_HAIR_HBK;                  // 髪の色
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR] = DRESSUP_COLOR_ID_LIP_LOF;                   // リップの色

  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_FACE            ] = DRESSUP_ITEM_ID_P1_FACE01;                  // 顔
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_HAIR_STYLE      ] = -1;                                         // 髪型
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCBADGE        ] = -1;                                         // バッジアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCEYE          ] = -1;                                         // アイアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCHAIR         ] = -1;                                         // ヘアアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BAG             ] = -1;                                         // バッグ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BNGL            ] = -1;                                         // バングル
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BODY            ] = -1;                                         // ボディ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BOTTOMS         ] = -1;                                         // ボトムス
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_HAT             ] = -1;                                         // 帽子
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_LEGS            ] = -1;                                         // レッグ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_SHOES           ] = -1;                                         // シューズ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_TOPS            ] = -1;                                         // トップス
  
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_SITU            ] = DRESSUP_ITEM_ID_P1_SITU_MMJ_RIDEDIVE01;           // シチュエーション着替え
}

/**
 *  @brief 女主人公　宇宙服の着せ替えパラメータ取得
 */
void DressUp::GetDressUpParamSpaceHeroine(poke_3d::model::DressUpParam* out_pDressUpParam)
{
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_SEX             ] = DressUpParam::SEX_FEMALE;                   // 性別
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_BODY_COLOR      ] = DRESSUP_COLOR_ID_SKIN_SYE;                  // 体の色      
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_EYE_COLOR       ] = DRESSUP_COLOR_ID_EYE_EBK;                   // 目の色
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_HAIR_COLOR      ] = DRESSUP_COLOR_ID_HAIR_HBK;                  // 髪の色
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR] = DRESSUP_COLOR_ID_LIP_LOF;                   // リップの色

  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_FACE            ] = DRESSUP_ITEM_ID_P2_FACE01;                  // 顔
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_HAIR_STYLE      ] = -1;                                         // 髪型
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCBADGE        ] = -1;                                         // バッジアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCEYE          ] = -1;                                         // アイアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCHAIR         ] = -1;                                         // ヘアアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BAG             ] = -1;                                         // バッグ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BNGL            ] = -1;                                         // バングル
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BODY            ] = -1;                                         // ボディ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BOTTOMS         ] = -1;                                         // ボトムス
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_HAT             ] = -1;                                         // 帽子
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_LEGS            ] = -1;                                         // レッグ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_SHOES           ] = -1;                                         // シューズ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_TOPS            ] = -1;                                         // トップス
  
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_SITU            ] = DRESSUP_ITEM_ID_P2_SITU_MMJ_RIDEDIVE01;           // シチュエーション着替え
}

/**
 * @brief 宇宙服の着せ替えパラメータ取得：性別は引数で受け取る
 */
void DressUp::GetDressUpParamSpace(u32 sex, poke_3d::model::DressUpParam* out_pDressUpParam)
{
  if (sex == PM_MALE)
  {
    GetDressUpParamSpaceHero(out_pDressUpParam);
  }
  else
  {
    GetDressUpParamSpaceHeroine(out_pDressUpParam);
  }
}


/**
 * @brief ライドスーツの着せ替えパラメータ取得（Face以外の適用処理付き）：性別は引数で受け取る
 *  @param  is_land   true:陸用  false:海用
 */
void DressUp::OverwrideDressUpParamRide(u32 sex, bool is_land, poke_3d::model::DressUpParam* out_pDressUpParam)
{
  poke_3d::model::DressUpParam defaultParam;

  // ライドの着せ替え情報を取得
  System::DressUp::GetDressUpParamRide( sex, is_land, &defaultParam );

  // Face以外のカテゴリの値を代入
  for( int i=poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE; i<poke_3d::model::DressUpParam::ITEM_CATEGORY_COUNT; ++i )
  {
    // @fix GFNMCat[1105] Zリング入手後、ライドスーツ着用時Zリングが主人公についていない
    if( i == poke_3d::model::DressUpParam::ITEM_CATEGORY_BNGL )
    {
      continue; // スキップ
    }
    else
    {
      out_pDressUpParam->itemParams[i] = defaultParam.itemParams[i];
    }
  }
}

/**
 * @brief 宇宙服の着せ替えパラメータ取得（Face以外の適用処理付き）：性別は引数で受け取る
 */
void DressUp::OverwrideDressUpParamSpace(u32 sex, poke_3d::model::DressUpParam* out_pDressUpParam)
{
  poke_3d::model::DressUpParam defaultParam;

  // ライドの着せ替え情報を取得
  System::DressUp::GetDressUpParamSpace( sex, &defaultParam );

  // Face以外のカテゴリの値を代入
  for( int i=poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE; i<poke_3d::model::DressUpParam::ITEM_CATEGORY_COUNT; ++i )
  {
    // @fix GFNMCat[1105] Zリング入手後、ライドスーツ着用時Zリングが主人公についていない
    if( i == poke_3d::model::DressUpParam::ITEM_CATEGORY_BNGL )
    {
      continue; // スキップ
    }
    else
    {
      out_pDressUpParam->itemParams[i] = defaultParam.itemParams[i];
    }
  }
}


/**
 * @brief 男主人公のデフォルトの着せ替えパラメータ取得(きせかえではずすことができるのはすべて-1にしてきせかえではずすことができない値だけ残す)
 */
void DressUp::GetDressUpParamDefaultIndispensableHero(poke_3d::model::DressUpParam* out_pDressUpParam)
{
  GetDressUpParamDefaultHero(out_pDressUpParam);

	//	きせかえではずすことができる項目はすべて-1に
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCEYE          ] = -1; // アイアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCHAIR         ] = -1; // ヘアアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_HAT             ] = -1; // 帽子
}

/**
 * @brief 女主人公のデフォルトの着せ替えパラメータ取得(きせかえではずすことができるのはすべて-1にしてきせかえではずすことができない値だけ残す)
 */
void DressUp::GetDressUpParamDefaultIndispensableHeroine(poke_3d::model::DressUpParam* out_pDressUpParam)
{
  GetDressUpParamDefaultHeroine(out_pDressUpParam);

	//	きせかえではずすことができる項目はすべて-1に
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCEYE          ] = -1; // アイアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCHAIR         ] = -1; // ヘアアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_HAT             ] = -1; // 帽子
}

/**
 * @brief デフォルトの着せ替えパラメータ取得(きせかえではずすことができるのはすべて-1にしてきせかえではずすことができない値だけ残す)
 */
void DressUp::GetDressUpParamDefaultIndispensable(u32 sex, poke_3d::model::DressUpParam* out_pDressUpParam)
{
  if (sex == PM_MALE)
  {
    GetDressUpParamDefaultIndispensableHero(out_pDressUpParam);
  }
  else
  {
    GetDressUpParamDefaultIndispensableHeroine(out_pDressUpParam);
  }
}

// 着せ替え用パラメータ：セーブ用

#define DRESSUP_CHECK_BIT_RANGE(bit, maxValue) ((1 << bit) >= maxValue)
#define DRESSUP_CHECK_BIT_RANGE_ITEM(bit, maxValueHero, maxValueHeroine) (DRESSUP_CHECK_BIT_RANGE(bit, (maxValueHero + 1)) && DRESSUP_CHECK_BIT_RANGE(bit, (maxValueHeroine + 1)))

#define DRESSUP_STATIC_ASSERT_ARRAY_COUNT(arrayA, arrayB) STATIC_ASSERT((sizeof(arrayA)/sizeof(arrayA[0])) == (sizeof(arrayB)/sizeof(arrayB[0])))

// コンストラクタ
DressUpParamSaveData::DressUpParamSaveData() :
  version(0),

  bodyParamSex(0),
  bodyParamBodyColor(0),
  bodyParamEyeColor(0),
  bodyParamHairColor(0),
  bodyParamLipColor(0),

  itemParamHair(0),
  itemParamAccEye(0),
  itemParamAccHair(0),
  itemParamBag(0),
  itemParamBottoms(0),
  itemParamHat(0),
  itemParamLegs(0),
  itemParamShoes(0),
  itemParamTops(0),

  hasZRing(0),
  hasBagCharm(0),
  isRomVersionNiji(0)
{
  // 構造体サイズチェック
  STATIC_ASSERT(
    (DRESSUP_PARAM_SAVE_DATA_BIT_VERSION + 
    DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_SEX + 
    DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_BODY_COLOR + 
    DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_EYE_COLOR + 
    DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_HAIR_COLOR + 
    DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_LIP_COLOR + 
    DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_HAIR + 
    DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_00) == 32);

  STATIC_ASSERT(
    (DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_ACCEYE + 
    DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_ACCHAIR + 
    DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_BAG + 
    DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_01) == 32);

  STATIC_ASSERT(
    (DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_BOTTOMS + 
    DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_HAT + 
    DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_LEGS + 
    DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_02) == 32);

  STATIC_ASSERT(
    (DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_SHOES + 
    DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_TOPS + 
    DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_03 + 
    DRESSUP_PARAM_SAVE_DATA_BIT_FLAG_PARAM_ZRING +
    DRESSUP_PARAM_SAVE_DATA_BIT_FLAG_PARAM_BAGCHARM +
    DRESSUP_PARAM_SAVE_DATA_BIT_FLAG_PARAM_NIJI +
    DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_04) == 32);

  STATIC_ASSERT(sizeof(DressUpParamSaveData) == 16);

  // 指定されたビット数でデータを格納できるかチェック
  STATIC_ASSERT(DRESSUP_CHECK_BIT_RANGE(DRESSUP_PARAM_SAVE_DATA_BIT_VERSION, DRESSUP_VERSION_COUNT));                // バージョン情報

  STATIC_ASSERT(DRESSUP_CHECK_BIT_RANGE(DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_BODY_COLOR, DRESSUP_COLOR_ID_SKIN_COUNT));                // 体の色
  STATIC_ASSERT(DRESSUP_CHECK_BIT_RANGE(DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_EYE_COLOR, DRESSUP_COLOR_ID_EYE_COUNT));                  // 目の色
  STATIC_ASSERT(DRESSUP_CHECK_BIT_RANGE(DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_HAIR_COLOR, DRESSUP_COLOR_ID_HAIR_COUNT));                // 髪の色
  STATIC_ASSERT(DRESSUP_CHECK_BIT_RANGE(DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_LIP_COLOR, DRESSUP_COLOR_ID_LIP_COUNT));                  // リップの色

  STATIC_ASSERT(DRESSUP_CHECK_BIT_RANGE_ITEM(DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_HAIR, DRESSUP_ITEM_ID_HERO_HAIR_COUNT, DRESSUP_ITEM_ID_HEROINE_HAIR_COUNT));                           // 髪型
  STATIC_ASSERT(DRESSUP_CHECK_BIT_RANGE_ITEM(DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_ACCEYE, DRESSUP_ITEM_ID_HERO_ACCEYE_COUNT, DRESSUP_ITEM_ID_HEROINE_ACCEYE_COUNT));                       // アイアクセサリ
  STATIC_ASSERT(DRESSUP_CHECK_BIT_RANGE_ITEM(DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_ACCHAIR, 0, DRESSUP_ITEM_ID_HEROINE_ACCHAIR_COUNT));                     // ヘアアクセサリ
  STATIC_ASSERT(DRESSUP_CHECK_BIT_RANGE_ITEM(DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_BAG, DRESSUP_ITEM_ID_HERO_BAG_COUNT, DRESSUP_ITEM_ID_HEROINE_BAG_COUNT));                             // バッグ
  STATIC_ASSERT(DRESSUP_CHECK_BIT_RANGE_ITEM(DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_BOTTOMS, DRESSUP_ITEM_ID_HERO_BOTTOMS_COUNT, DRESSUP_ITEM_ID_HEROINE_BOTTOMS_COUNT));                     // ボトムス
  STATIC_ASSERT(DRESSUP_CHECK_BIT_RANGE_ITEM(DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_HAT, DRESSUP_ITEM_ID_HERO_HAT_COUNT, DRESSUP_ITEM_ID_HEROINE_HAT_COUNT));                             // 帽子
  STATIC_ASSERT(DRESSUP_CHECK_BIT_RANGE_ITEM(DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_LEGS, DRESSUP_ITEM_ID_HERO_LEGS_COUNT, DRESSUP_ITEM_ID_HEROINE_LEGS_COUNT));                           // レッグ
  STATIC_ASSERT(DRESSUP_CHECK_BIT_RANGE_ITEM(DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_SHOES, DRESSUP_ITEM_ID_HERO_SHOES_COUNT, DRESSUP_ITEM_ID_HEROINE_SHOES_COUNT));                         // シューズ
  STATIC_ASSERT(DRESSUP_CHECK_BIT_RANGE_ITEM(DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_TOPS, DRESSUP_ITEM_ID_HERO_TOPS_COUNT, DRESSUP_ITEM_ID_HEROINE_TOPS_COUNT));                           // トップス
}

// デストラクタ
DressUpParamSaveData::~DressUpParamSaveData()
{
}


// ボディパラメータ設定
static u32 SetSaveDataBodyParam(s16 bodyParam)
{
  // そのまま
  return static_cast<u32>(bodyParam);
}
// ボディパラメータ取得
static bool GetSaveDataBodyParam(s16* out_pBodyParam, u32 savedBodyParam, u32 maxValue)
{
  // 上限を超えていたらデフォルト値に変更
  if (savedBodyParam >= maxValue)
  {
    return false;
  }

  // そのまま
  *out_pBodyParam = static_cast<s16>(savedBodyParam);
  return true;
}

// アイテムパラメータ設定
static u32 SetSaveDataItemParam(s16 itemParam)
{
  // 何も付けてない状態を0にする
  if (itemParam < 0)
  {
    return 0;
  }

  // 上の対応用にインデックスを+1
  return static_cast<u32>(itemParam + 1);
}
// アイテムパラメータ取得
static bool GetSaveDataItemParam(s16* out_pItemParam, u32 savedItemParam, u32 maxValue)
{
  // 何も付けてない状態を0にする
  if (savedItemParam == 0)
  {
    *out_pItemParam = -1;
    return true;
  }

  // 上の対応用にインデックスを+1
  u32 itemParam = savedItemParam - 1;

  // 上限を超えていたらデフォルト値に変更
  if (itemParam >= maxValue)
  {
    return false;
  }

  *out_pItemParam = static_cast<s16>(itemParam);
  return true;
}

// フラグパラメータ設定
static u32 SetSaveDataFlagParam(b8 flagParam)
{
  // そのまま
  return static_cast<u32>(flagParam);
}

// フラグパラメータ取得
static bool GetSaveDataFlagParam(b8* out_pFlagParam, u32 savedFlagParam, u32 maxValue)
{
  // そのまま
  *out_pFlagParam = static_cast<b8>(savedFlagParam);
  return true;
}

// 着せ替え用パラメータから変換
void DressUpParamSaveData::ConvertFromDressUpParam(DressUpParamSaveData* out_pSaveData, const poke_3d::model::DressUpParam& dressUpParam)
{
  // 書き込み先が無い
  if (out_pSaveData == NULL)
  {
    return;
  }

  out_pSaveData->version = DRESSUP_VERSION_NIJI;                                                                                                    // バージョン情報：niji

#if PM_DEBUG
  // デバッグ用のセーブデータバージョンを書き込む
  out_pSaveData->version = s_debugVersion;
#endif

  out_pSaveData->bodyParamSex = SetSaveDataBodyParam(dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX]);                     // 性別
  out_pSaveData->bodyParamBodyColor = SetSaveDataBodyParam(dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_BODY_COLOR]);        // 体の色
  out_pSaveData->bodyParamEyeColor = SetSaveDataBodyParam(dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_EYE_COLOR]);          // 目の色
  out_pSaveData->bodyParamHairColor = SetSaveDataBodyParam(dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_HAIR_COLOR]);        // 髪の色
  out_pSaveData->bodyParamLipColor = SetSaveDataBodyParam(dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR]);   // リップの色

  out_pSaveData->itemParamHair = SetSaveDataItemParam(dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE]);             // 髪型
  out_pSaveData->itemParamAccEye = SetSaveDataItemParam(dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCEYE]);               // アイアクセサリ
  out_pSaveData->itemParamAccHair = SetSaveDataItemParam(dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR]);             // ヘアアクセサリ
  out_pSaveData->itemParamBag = SetSaveDataItemParam(dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BAG]);                     // バッグ
  out_pSaveData->itemParamBottoms = SetSaveDataItemParam(dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BOTTOMS]);             // ボトムス
  out_pSaveData->itemParamHat = SetSaveDataItemParam(dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT]);                     // 帽子
  out_pSaveData->itemParamLegs = SetSaveDataItemParam(dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_LEGS]);                   // レッグ
  out_pSaveData->itemParamShoes = SetSaveDataItemParam(dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_SHOES]);                 // シューズ
  out_pSaveData->itemParamTops = SetSaveDataItemParam(dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_TOPS]);                   // トップス

  out_pSaveData->hasZRing = SetSaveDataFlagParam(dressUpParam.hasZRing);                                                                            // Zリングを所持している
  out_pSaveData->hasBagCharm = SetSaveDataFlagParam(dressUpParam.hasBagCharm);                                                                      // 島巡りの証を所持している
}

// 着せ替え用パラメータに変換
void DressUpParamSaveData::ConvertToDressUpParam(poke_3d::model::DressUpParam* out_pDressUpParam, const DressUpParamSaveData& saveData)
{
  // ボディリスト
  static const poke_3d::model::DressUpParam::BodyCategory sc_bodyCategoryList[] =
  {
    poke_3d::model::DressUpParam::BODY_CATEGORY_BODY_COLOR,       // 体の色
    poke_3d::model::DressUpParam::BODY_CATEGORY_EYE_COLOR,        // 目の色
    poke_3d::model::DressUpParam::BODY_CATEGORY_HAIR_COLOR,       // 髪の色
    poke_3d::model::DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR, // リップの色
  };

  // ボディ数リスト
  static const u32 sc_maxValueListBody[] =
  {
    DRESSUP_COLOR_ID_SKIN_COUNT,        // 体の色
    DRESSUP_COLOR_ID_EYE_COUNT,         // 目の色
    DRESSUP_COLOR_ID_HAIR_COUNT,        // 髪の色
    DRESSUP_COLOR_ID_LIP_COUNT,         // リップの色
  };
  DRESSUP_STATIC_ASSERT_ARRAY_COUNT(sc_bodyCategoryList, sc_maxValueListBody);

  // アイテムリスト
  static const poke_3d::model::DressUpParam::ItemCategory sc_itemCategoryList[] =
  {
    poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE,   // 髪型
    poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCEYE,       // アイアクセサリ
    poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR,      // ヘアアクセサリ
    poke_3d::model::DressUpParam::ITEM_CATEGORY_BAG,          // バッグ
    poke_3d::model::DressUpParam::ITEM_CATEGORY_BOTTOMS,      // ボトムス
    poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT,          // 帽子
    poke_3d::model::DressUpParam::ITEM_CATEGORY_LEGS,         // レッグ
    poke_3d::model::DressUpParam::ITEM_CATEGORY_SHOES,        // シューズ
    poke_3d::model::DressUpParam::ITEM_CATEGORY_TOPS,         // トップス
  };
  
  // アイテム数リスト：男
  static const u32 sc_maxValueListItemHero[] =
  {
    DRESSUP_ITEM_ID_HERO_HAIR_COUNT,                          // 髪型
    DRESSUP_ITEM_ID_HERO_ACCEYE_COUNT,                        // アイアクセサリ
    0,                                                        // ヘアアクセサリ
    DRESSUP_ITEM_ID_HERO_BAG_COUNT,                           // バッグ
    DRESSUP_ITEM_ID_HERO_BOTTOMS_COUNT,                       // ボトムス
    DRESSUP_ITEM_ID_HERO_HAT_COUNT,                           // 帽子
    DRESSUP_ITEM_ID_HERO_LEGS_COUNT,                          // レッグ
    DRESSUP_ITEM_ID_HERO_SHOES_COUNT,                         // シューズ
    DRESSUP_ITEM_ID_HERO_TOPS_COUNT,                          // トップス
  };
  DRESSUP_STATIC_ASSERT_ARRAY_COUNT(sc_itemCategoryList, sc_maxValueListItemHero);

  // アイテム数リスト：女
  static const u32 sc_maxValueListItemHeroine[] =
  {
    DRESSUP_ITEM_ID_HEROINE_HAIR_COUNT,                          // 髪型
    DRESSUP_ITEM_ID_HEROINE_ACCEYE_COUNT,                        // アイアクセサリ
    DRESSUP_ITEM_ID_HEROINE_ACCHAIR_COUNT,                       // ヘアアクセサリ
    DRESSUP_ITEM_ID_HEROINE_BAG_COUNT,                           // バッグ
    DRESSUP_ITEM_ID_HEROINE_BOTTOMS_COUNT,                       // ボトムス
    DRESSUP_ITEM_ID_HEROINE_HAT_COUNT,                           // 帽子
    DRESSUP_ITEM_ID_HEROINE_LEGS_COUNT,                          // レッグ
    DRESSUP_ITEM_ID_HEROINE_SHOES_COUNT,                         // シューズ
    DRESSUP_ITEM_ID_HEROINE_TOPS_COUNT,                          // トップス
  };
  DRESSUP_STATIC_ASSERT_ARRAY_COUNT(sc_itemCategoryList, sc_maxValueListItemHeroine);

  // 書き込み先が無い
  if (out_pDressUpParam == NULL)
  {
    return;
  }

  // 性別
  GetSaveDataBodyParam(&out_pDressUpParam->bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX], saveData.bodyParamSex, poke_3d::model::DressUpParam::SEX_COUNT);

  // デフォルト値を取得
  poke_3d::model::DressUpParam defaultParam;
  const u32* maxValueItemList = NULL;
  if (out_pDressUpParam->bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX] == poke_3d::model::DressUpParam::SEX_MALE)
  {
    DressUp::GetDressUpParamDefaultHero(&defaultParam);
    maxValueItemList = sc_maxValueListItemHero;
  }
  else
  {
    DressUp::GetDressUpParamDefaultHeroine(&defaultParam);
    maxValueItemList = sc_maxValueListItemHeroine;
  }
  *out_pDressUpParam = defaultParam; // 初期値はデフォルトで埋める

  // 知らないバージョンならデフォルト値のまま返す
  u32 targetVersion = DRESSUP_VERSION_NIJI;

#if PM_DEBUG
   targetVersion = s_debugVersion;
#endif

  if (saveData.version != targetVersion)
  {
    return;
  }

  // ボディパラメータ読み取り
  u32 bodyParamList[] =
  {
    saveData.bodyParamBodyColor,        // 体の色
    saveData.bodyParamEyeColor,         // 目の色
    saveData.bodyParamHairColor,        // 髪の色
    saveData.bodyParamLipColor,         // リップの色
  };
  DRESSUP_STATIC_ASSERT_ARRAY_COUNT(bodyParamList, sc_bodyCategoryList);

  for (u32 i = 0; i < sizeof(sc_bodyCategoryList)/sizeof(sc_bodyCategoryList[0]); ++i)
  {
    if (!GetSaveDataBodyParam(&out_pDressUpParam->bodyParams[sc_bodyCategoryList[i]], bodyParamList[i], sc_maxValueListBody[i]))
    {
      // 有効な値の範囲を超えていたら全てデフォルト値に設定して終了
      *out_pDressUpParam = defaultParam;
      return;
    }
  }

  // アイテムパラメータ読み取り
  u32 itemParamList[] =
  {
    saveData.itemParamHair,                          // 髪型
    saveData.itemParamAccEye,                        // アイアクセサリ
    saveData.itemParamAccHair,                       // ヘアアクセサリ
    saveData.itemParamBag,                           // バッグ
    saveData.itemParamBottoms,                       // ボトムス
    saveData.itemParamHat,                           // 帽子
    saveData.itemParamLegs,                          // レッグ
    saveData.itemParamShoes,                         // シューズ
    saveData.itemParamTops,                          // トップス
  };
  DRESSUP_STATIC_ASSERT_ARRAY_COUNT(itemParamList, sc_itemCategoryList);

  for (u32 i = 0; i < sizeof(sc_itemCategoryList)/sizeof(sc_itemCategoryList[0]); ++i)
  {
    if (!GetSaveDataItemParam(&out_pDressUpParam->itemParams[sc_itemCategoryList[i]], itemParamList[i], maxValueItemList[i]))
    {
      // 有効な値の範囲を超えていたら全てデフォルト値に設定して終了
      *out_pDressUpParam = defaultParam;
      return;
    }
  }

  // 最低限のアイテムを着ていなければデフォルトに戻す
  static bool sc_needItemList[] =
  {
    true,                           // 髪型
    false,                          // アイアクセサリ
    false,                          // ヘアアクセサリ
    true,                           // バッグ
    true,                           // ボトムス
    false,                          // 帽子
    true,                           // レッグ
    true,                           // シューズ
    true,                           // トップス
  };
  DRESSUP_STATIC_ASSERT_ARRAY_COUNT(sc_needItemList, sc_itemCategoryList);

  for (u32 i = 0; i < sizeof(sc_itemCategoryList)/sizeof(sc_itemCategoryList[0]); ++i)
  {
    // チェックする必要があるアイテムか
    if (!sc_needItemList[i])
    {
      continue;
    }

    if (out_pDressUpParam->itemParams[sc_itemCategoryList[i]] < 0)
    {
      // 全てデフォルト値に設定して終了
      *out_pDressUpParam = defaultParam;
      return;
    }
  }

  // @fix GFMCat[5278] : ARC内のデータを使った組み合わせチェック
  {
    s32* pArcIdList = DRESSUP_ARCID_PARTS_LIST_FIELD;

    // 女用アーカイブは配列の後半
    if (out_pDressUpParam->bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX] == DressUpParam::SEX_FEMALE)
    {
      u32 arcNum = sizeof(DRESSUP_ARCID_PARTS_LIST_FIELD) / sizeof(DRESSUP_ARCID_PARTS_LIST_FIELD[0]);
      pArcIdList += arcNum / 2;
    }

    DressUpModelResourceManagerCore dressUpModelResourceManager;
    dressUpModelResourceManager.Initialize(NULL, NULL, pArcIdList, true);

    if (!dressUpModelResourceManager.CheckDressUpCombination(*out_pDressUpParam))
    {
      // 全てデフォルト値に設定して終了
      *out_pDressUpParam = defaultParam;
      return;
    }
  }

  // Zリング所持フラグ設定
  if (!GetSaveDataFlagParam(&out_pDressUpParam->hasZRing, saveData.hasZRing, true))
  {
    // 有効な値の範囲を超えていたら全てデフォルト値に設定して終了
    *out_pDressUpParam = defaultParam;
    return;
  }

  // 島巡りの証所持フラグ設定
  if (!GetSaveDataFlagParam(&out_pDressUpParam->hasBagCharm, saveData.hasBagCharm, true))
  {
    // 有効な値の範囲を超えていたら全てデフォルト値に設定して終了
    *out_pDressUpParam = defaultParam;
    return;
  }

  // Zリング所持状態をバングルに反映
  if(out_pDressUpParam->hasZRing)
  {
    if(out_pDressUpParam->bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX] == DressUpParam::SEX_MALE)
    {
      out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BNGL] = DRESSUP_ITEM_ID_P1_BNGL_ZENBRESS01;
    }
    else if(out_pDressUpParam->bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX] == DressUpParam::SEX_FEMALE)
    {
      out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BNGL] = DRESSUP_ITEM_ID_P2_BNGL_ZENBRESS01;
    }
  }
  else
  {
    out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BNGL] = -1;
  }
}

// バングルをnijiのものに上書き(MMCat[337]用に作成）
void DressUpParamSaveData::OverrideNijiBngl( poke_3d::model::DressUpParam* out_pDressUpParam )
{
  if( out_pDressUpParam == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  // Zリング所持状態をバングルに反映
  if( out_pDressUpParam->hasZRing )
  {
    if(out_pDressUpParam->bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX] == DressUpParam::SEX_MALE)
    {
      out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BNGL] = DRESSUP_ITEM_ID_P1_BNGL_ZENBRESS_NIJI01;
    }
    else if(out_pDressUpParam->bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX] == DressUpParam::SEX_FEMALE)
    {
      out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BNGL] = DRESSUP_ITEM_ID_P2_BNGL_ZENBRESS_NIJI01;
    }
  }
}

#if PM_DEBUG
/**
 * @brief ランダムで着せ替えパラメータ取得：性別は引数で受け取る
 */
void DressUp::Debug_GetDressUpParamRandom(u32 sex, const poke_3d::model::DressUpModelResourceManagerCore* pDressUpModelResourceManager, poke_3d::model::DressUpParam* out_pDressUpParam, gfl2::math::SFMTRandom* pSFMTRandom)
{
  static const s16 sc_bodyCountList[] = 
  {
    -1,                           // 性別
    DRESSUP_COLOR_ID_SKIN_COUNT,  // 体の色
    DRESSUP_COLOR_ID_EYE_COUNT,   // 目の色
    DRESSUP_COLOR_ID_HAIR_COUNT,  // 髪の色
    DRESSUP_COLOR_ID_LIP_COUNT,   // リップの色
  };

  static const s16 sc_itemCountList_Hero[] =
  {
    DRESSUP_ITEM_ID_HERO_FACE_COUNT,          //ITEM_CATEGORY_FACE,                   // 顔
    DRESSUP_ITEM_ID_HERO_HAIR_COUNT,          //ITEM_CATEGORY_HAIR_STYLE,             // 髪型
    -1,                                       //ITEM_CATEGORY_ACCBADGE,               // バッジアクセサリ
    DRESSUP_ITEM_ID_HERO_ACCEYE_COUNT,        //ITEM_CATEGORY_ACCEYE,                 // アイアクセサリ
    -1,                                       //ITEM_CATEGORY_ACCHAIR,                // ヘアアクセサリ
    DRESSUP_ITEM_ID_HERO_BAG_COUNT,           //ITEM_CATEGORY_BAG,                    // バッグ
    -1,                                       //ITEM_CATEGORY_BNGL,                   // バングル
    -1,                                       //ITEM_CATEGORY_BODY,                   // ボディ
    DRESSUP_ITEM_ID_HERO_BOTTOMS_COUNT,       //ITEM_CATEGORY_BOTTOMS,                // ボトムス
    DRESSUP_ITEM_ID_HERO_HAT_COUNT,           //ITEM_CATEGORY_HAT,                    // 帽子
    DRESSUP_ITEM_ID_HERO_LEGS_COUNT,          //ITEM_CATEGORY_LEGS,                   // レッグ
    DRESSUP_ITEM_ID_HERO_SHOES_COUNT,         //ITEM_CATEGORY_SHOES,                  // シューズ
    DRESSUP_ITEM_ID_HERO_TOPS_COUNT,          //ITEM_CATEGORY_TOPS,                   // トップス
    -1,                                       //ITEM_CATEGORY_SITU,                   // シチュエーション着替え
  };

  static const s16 sc_itemCountList_Heroine[] =
  {
    DRESSUP_ITEM_ID_HEROINE_FACE_COUNT,       //ITEM_CATEGORY_FACE,                   // 顔
    DRESSUP_ITEM_ID_HEROINE_HAIR_COUNT,       //ITEM_CATEGORY_HAIR_STYLE,             // 髪型
    -1,                                       //ITEM_CATEGORY_ACCBADGE,               // バッジアクセサリ
    DRESSUP_ITEM_ID_HEROINE_ACCEYE_COUNT,     //ITEM_CATEGORY_ACCEYE,                 // アイアクセサリ
    DRESSUP_ITEM_ID_HEROINE_ACCHAIR_COUNT,    //ITEM_CATEGORY_ACCHAIR,                // ヘアアクセサリ
    DRESSUP_ITEM_ID_HEROINE_BAG_COUNT,        //ITEM_CATEGORY_BAG,                    // バッグ
    -1,                                       //ITEM_CATEGORY_BNGL,                   // バングル
    -1,                                       //ITEM_CATEGORY_BODY,                   // ボディ
    DRESSUP_ITEM_ID_HEROINE_BOTTOMS_COUNT,    //ITEM_CATEGORY_BOTTOMS,                // ボトムス
    DRESSUP_ITEM_ID_HEROINE_HAT_COUNT,        //ITEM_CATEGORY_HAT,                    // 帽子
    DRESSUP_ITEM_ID_HEROINE_LEGS_COUNT,       //ITEM_CATEGORY_LEGS,                   // レッグ
    DRESSUP_ITEM_ID_HEROINE_SHOES_COUNT,      //ITEM_CATEGORY_SHOES,                  // シューズ
    DRESSUP_ITEM_ID_HEROINE_TOPS_COUNT,       //ITEM_CATEGORY_TOPS,                   // トップス
    -1,                                       //ITEM_CATEGORY_SITU,                   // シチュエーション着替え
  };

  // ボディ設定
  for (u32 i = 0; i < DressUpParam::BODY_CATEGORY_COUNT; ++i)
  {
    out_pDressUpParam->bodyParams[i] = -1;

    if (sc_bodyCountList[i] < 0)
    {
      continue;
    }

    out_pDressUpParam->bodyParams[i] = pSFMTRandom ? pSFMTRandom->Next64(sc_bodyCountList[i]) : System::GflUse::GetPublicRand(sc_bodyCountList[i]);
  }


  const s16* pItemCountList = NULL;

  // 性別ごとの設定
  if (sex == PM_MALE)
  {
    out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_SEX] = DressUpParam::SEX_MALE;
    out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR] = DRESSUP_COLOR_ID_LIP_LOF; // 男ならリップは無し
    out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BNGL] = DRESSUP_ITEM_ID_P1_BNGL_ZENBRESS01;         // バングルは必ず付ける
    pItemCountList = sc_itemCountList_Hero;
  }
  else
  {
    out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_SEX] = DressUpParam::SEX_FEMALE;
    out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BNGL] = DRESSUP_ITEM_ID_P2_BNGL_ZENBRESS01;         // バングルは必ず付ける
    pItemCountList = sc_itemCountList_Heroine;
  }

  // アイテム設定
  for (u32 i = 0; i < DressUpParam::ITEM_CATEGORY_COUNT; ++i)
  {
    out_pDressUpParam->itemParams[i] = -1;

    if (pItemCountList[i] < 0)
    {
      continue;
    }

    out_pDressUpParam->itemParams[i] = pSFMTRandom ? pSFMTRandom->Next64(pItemCountList[i]) : System::GflUse::GetPublicRand(pItemCountList[i]);
  }

  u32 rand = 0;

  // ランダムで帽子、アクセサリー、全力リングを外す
  rand = pSFMTRandom ? pSFMTRandom->Next64() : System::GflUse::GetPublicRand();
  if (rand % 2 == 0)
  {
    out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT] = -1;
  }
  rand = pSFMTRandom ? pSFMTRandom->Next64() : System::GflUse::GetPublicRand();
  if (rand % 2 == 0)
  {
    out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCEYE] = -1;
  }
  rand = pSFMTRandom ? pSFMTRandom->Next64() : System::GflUse::GetPublicRand();
  if (rand % 2 == 0)
  {
    out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR] = -1;
  }
  rand = pSFMTRandom ? pSFMTRandom->Next64() : System::GflUse::GetPublicRand();
  if (rand % 2 == 0)
  {
    out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BNGL] = -1;
  }

  // 帽子がかぶれない髪型なら帽子を外す
  if (!pDressUpModelResourceManager->CanDressUpHairWearHat(out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE]))
  {
    out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT] = -1;
  }
  // 帽子をかぶっているならヘアアクセサリーは外す
  if (out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT] >= 0)
  {
    out_pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR] = -1;
  }

  // 念のため組み合わせチェックをして問題があればデフォルトに戻す
  if (!pDressUpModelResourceManager->CheckDressUpCombination(*out_pDressUpParam))
  {
    GetDressUpParamDefault(sex, out_pDressUpParam);
  }
}

/**
* @brief 処理負荷の高い着せ替えパラメータ取得（女主人公）
*/
void DressUp::Debug_GetDressUpParamTopPfm(poke_3d::model::DressUpParam* out_pDressUpParam)
{
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_SEX] = DressUpParam::SEX_FEMALE;                     // 性別
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_BODY_COLOR] = DRESSUP_COLOR_ID_SKIN_SYE;                  // 体の色      
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_EYE_COLOR] = DRESSUP_COLOR_ID_EYE_EBK;                   // 目の色
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_HAIR_COLOR] = DRESSUP_COLOR_ID_HAIR_HBK;                  // 髪の色
  out_pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR] = DRESSUP_COLOR_ID_LIP_LOF;                   // リップの色

  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_FACE] = DRESSUP_ITEM_ID_P2_FACE01;                  // 顔
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_HAIR_STYLE] = DRESSUP_ITEM_ID_P2_HAIR_MMJ_SPONY_PATTUN;          // 髪型
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCBADGE] = -1;                                         // バッジアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCEYE] = DRESSUP_ITEM_ID_P2_ACCEYE_WELL01_BK;          // アイアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCHAIR] = -1;                                         // ヘアアクセサリ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BAG] = DRESSUP_ITEM_ID_P2_BAG_LBAG01_BE;             // バッグ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BNGL] = DRESSUP_ITEM_ID_P2_BNGL_ZENBRESS01;         // バングル
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BODY] = -1;                                         // ボディ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BOTTOMS] = DRESSUP_ITEM_ID_P2_BOTTOMS_PLEAT01_BE;      // ボトムス
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_HAT] = DRESSUP_ITEM_ID_P2_HAT_RESORT01_BE;              // 帽子
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_LEGS] = DRESSUP_ITEM_ID_P2_LEGS_KNEEHI01_BL;         // レッグ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_SHOES] = DRESSUP_ITEM_ID_P2_SHOES_MMJ_DSHOES01;          // シューズ
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_TOPS] = DRESSUP_ITEM_ID_P2_TOPS_DCUT01_BK;        // トップス
  out_pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_SITU] = -1;                                         // シチュエーション着替え

  out_pDressUpParam->hasBagCharm = 1;
}

#endif
}

