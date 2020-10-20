//===================================================================
/**
 * @file    SkyboxSunMoon.h
 * @brief   天球太陽と月クラス
 * @author  saita_kazuki
 * @date    2015.05.07
 */
//===================================================================

#if !defined( SKYBOX_SUN_MOON_H_INCLUDED )
#define SKYBOX_SUN_MOON_H_INCLUDED
#pragma once

#include "./SkyboxPartsBase.h"
#include "niji_reference_files/script/FieldPawnTypes.h"

GFL_NAMESPACE_BEGIN( System )
GFL_NAMESPACE_BEGIN( Skybox )

/**
 * @class 天球太陽と月クラス
 */
class SunMoon : public PartsBase
{
  /**
   * @briefコピーコンストラクタ＋代入禁止
   */
  GFL_FORBID_COPY_AND_ASSIGN( SunMoon );

public:

  ///< モデル種類
  enum ModelType
  {
    MODEL_TYPE_MOON,
    MODEL_TYPE_SUN,

    MODEL_TYPE_MAX
  };

  ///< アニメーション種類
  enum AnimationType
  {
    ANIMATION_TYPE_MOON_ALLTIME_JOINT,  ///< 月の移動(1日アニメーション)
    ANIMATION_TYPE_MOON_ALLTIME_SHADER, ///< 月の表現(1日アニメーション)
    ANIMATION_TYPE_MOON_CHANGE_UV,      ///< 月の満ち欠け(パターンアニメーション)

    ANIMATION_TYPE_SUN_ALLTIME_JOINT,   ///< 太陽の移動(1日アニメーション)
    ANIMATION_TYPE_SUN_UV,              ///< 太陽の表現

    ANIMATION_TYPE_MAX
  };

  ///< リソース
  struct ResourceData
  {
    void* pModelData[ MODEL_TYPE_MAX ];
    void* pMotionData[ ANIMATION_TYPE_MAX ];
  };

public:

  /**
   * @brief コンストラクタ
   */
  SunMoon( gfl2::heap::HeapBase* pParent, void* pResourceData);

  /**
   * @brief デストラクタ
   */
  virtual ~SunMoon();

  /**
   * @brief 初期化
   */
  virtual void Initialize( void* pResourceData);

  /**
   * @brief 破棄
   */
  virtual void Terminate();

  /**
   * @brief 更新
   */
  virtual void Update( const gfl2::math::Vector3& pos);

  /**
   * @brief 月の満ち欠けパターン変更
   * @param day 日
   */
  void ChangeMoonPattern( s32 day);
  void ChangeMoonPattern( MoonPattern pattern );

  /**
   * @brief 太陽の位置を取得
   * @return 太陽の位置
   */
  const gfl2::math::Vector3& GetSunPosition() const ;

  /**
   * @brief 月の位置を取得
   * @return 月の位置
   */
  const gfl2::math::Vector3& GetMoonPosition() const ;

private:

  ///< 月のアニメーションスロット番号
  enum AnimationIndexMoon
  {
    // ジョイント
    ANIMATION_INDEX_MOON_ALLTIME_JOINT    = 0,

    // マテリアル
    ANIMATION_INDEX_MOON_ALLTIME_SHADER   = 0,
    ANIMATION_INDEX_MOON_CHANGE_UV,
  };

  ///< 太陽のアニメーションスロット番号
  enum AnimationIndexSun
  {
    // ジョイント
    ANIMATION_INDEX_SUN_ALLTIME_JOINT     = 0,

    // マテリアル
    ANIMATION_INDEX_SUN_UV                = 0,
  };

private:

  static const u8 MOON_PATTERN_TABLE[31]; ///< 月の満ち欠けカレンダー

  MoonPattern   m_nowMoonPattern;         ///< 現在の月の満ち欠けパターン
  s32           m_sunJointIndex;          ///< 太陽のジョイント番号
  s32           m_moonJointIndex;         ///< 月のジョイント番号

  gfl2::math::Vector3 m_sunJointPos;      ///< 太陽ジョイントの位置
  gfl2::math::Vector3 m_moonJointPos;     ///< 月ジョイントの位置
};

GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( System )

#endif // SKYBOX_SUN_MOON_H_INCLUDED
