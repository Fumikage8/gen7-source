#if !defined(POKE_3D_LIB_MODEL_DRESSUP_PARAM_H_INCLUDED)
#define POKE_3D_LIB_MODEL_DRESSUP_PARAM_H_INCLUDED

#include "gfl2_CharaModel.h"

namespace poke_3d { namespace model {

/**
 * @brief 着せ替え用パラメータ
 */
struct DressUpParam
{
  /**
   * @brief 着せ替え性別
   */
  enum Sex
  {
    SEX_MALE,
    SEX_FEMALE,

    SEX_COUNT
  };

  /**
   * @brief ボディカテゴリ
   */
  enum BodyCategory
  {
    BODY_CATEGORY_SEX,                    // 性別
    BODY_CATEGORY_BODY_COLOR,             // 体の色
    BODY_CATEGORY_EYE_COLOR,              // 目の色
    BODY_CATEGORY_HAIR_COLOR,             // 髪の色
    BODY_CATEGORY_FEMALE_LIP_COLOR,       // リップの色

    BODY_CATEGORY_COUNT                   // ボディカテゴリ総数
  };

  /**
   * @brief アイテムカテゴリ
   */
  enum ItemCategory
  {
    ITEM_CATEGORY_FACE,                   // 顔
    ITEM_CATEGORY_HAIR_STYLE,             // 髪型
    ITEM_CATEGORY_ACCBADGE,               // バッジアクセサリ
    ITEM_CATEGORY_ACCEYE,                 // アイアクセサリ
    ITEM_CATEGORY_ACCHAIR,                // ヘアアクセサリ
    ITEM_CATEGORY_BAG,                    // バッグ
    ITEM_CATEGORY_BNGL,                   // バングル
    ITEM_CATEGORY_BODY,                   // ボディ
    ITEM_CATEGORY_BOTTOMS,                // ボトムス
    ITEM_CATEGORY_HAT,                    // 帽子
    ITEM_CATEGORY_LEGS,                   // レッグ
    ITEM_CATEGORY_SHOES,                  // シューズ
    ITEM_CATEGORY_TOPS,                   // トップス
    ITEM_CATEGORY_SITU,                   // シチュエーション着替え

    ITEM_CATEGORY_COUNT                   // アイテムカテゴリ総数
  };

  s16 bodyParams[BODY_CATEGORY_COUNT];     // ボディパラメータリスト
  s16 itemParams[ITEM_CATEGORY_COUNT];     // アイテムパラメータリスト

  b8 hasZRing;                             // Zリングを所持している
  b8 hasBagCharm;                          // 島巡りの証を所持している


  // 初期値を入れておく
  DressUpParam()
  {
    for (u32 i = 0; i < BODY_CATEGORY_COUNT; ++i)
    {
      this->bodyParams[i] = -1;
    }

    for (u32 i = 0; i < ITEM_CATEGORY_COUNT; ++i)
    {
      this->itemParams[i] = -1;
    }

    this->hasZRing = false;
    this->hasBagCharm = false;
  }
};

}}

#endif
