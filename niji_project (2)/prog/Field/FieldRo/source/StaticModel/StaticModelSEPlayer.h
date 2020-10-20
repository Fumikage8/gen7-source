//===================================================================
/**
 * @file    StaticModelSEPlayer.h
 * @brief   配置モデルSE再生クラス
 * @author  saita_kazuki
 * @date    2015.06.25
 */
//===================================================================

#if !defined( STATIC_MODEL_SE_PLAYER_H_INCLUDED )
#define STATIC_MODEL_SE_PLAYER_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include "Field/FieldRo/include/StaticModel/StaticModelDefine.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( StaticModel )

/**
 * @class 配置モデルSE再生クラス
 */
class StaticModelSEPlayer
{
public:

  /**
   * @brief コンストラクタ
   */
  StaticModelSEPlayer();

  /**
   * @brief デストラクタ
   */
  virtual ~StaticModelSEPlayer();

  /**
   * @brief 初期化
   * @param SEテーブルバイナリデータ
   */
  void Initialize( void* pSETableData);

  /**
   * @brief 破棄
   */
  void Terminate();

  /**
   * @brief SE再生
   * @param programID プログラムID
   * @param index アニメーション番号
   * @param isInverse trueで逆再生
   */
  void PlaySE( u8 programID, u32 index, bool isInverse = false);

  /**
   * @brief SE再生をするプログラムIDか
   * @param programID プログラム指定ID
   * @retval true SE再生する
   * @retval true SE再生しない
   */
  bool IsPlaySE( u8 programID);

private:

  u32 GetSEID( u8 programID, u32 index, bool isInverse);

private:

  static const u32 PROGRAM_ID_NONE = 0xFF;  ///< 無効なプログラムID

  /**
   * @brief SEテーブルデータ構造体
   */
  struct Table
  {
    u32 SELabelID_Order[ EVENT_ANIMATION_INDEX_MAX ];     ///< 順再生SEラベル
    u32 SELabelID_Inverse[ EVENT_ANIMATION_INDEX_MAX ];   ///< 逆再生SEラベル
  };

  Table*  m_pSETable;   ///< SEテーブル
};

GFL_NAMESPACE_END( StaticModel )
GFL_NAMESPACE_END( Field )

#endif // STATIC_MODEL_SE_PLAYER_H_INCLUDED

