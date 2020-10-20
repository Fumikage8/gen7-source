//===================================================================
/**
 * @file    SkyboxCloudySky.h
 * @brief   天球曇り空クラス
 * @author  saita_kazuki
 * @date    2015.05.07
 */
//===================================================================

#if !defined( SKYBOX_CLOUDY_SKY_H_INCLUDED )
#define SKYBOX_CLOUDY_SKY_H_INCLUDED
#pragma once

#include "./SkyboxPartsBase.h"

GFL_NAMESPACE_BEGIN( System )
GFL_NAMESPACE_BEGIN( Skybox )

/**
 * @class 天球曇り空クラス
 */
class CloudySky : public PartsBase
{
  /**
   * @briefコピーコンストラクタ＋代入禁止
   */
  GFL_FORBID_COPY_AND_ASSIGN( CloudySky );

public:

  ///< モデル種類
  enum ModelType
  {
    MODEL_TYPE_NORMAL,

    MODEL_TYPE_MAX
  };

  ///< アニメーション種類
  enum AnimationType
  {
    ANIMATION_TYPE_MAX
  };

  ///< リソース
  struct ResourceData
  {
    void*   pModelData[ MODEL_TYPE_MAX ];
  };

public:

  /**
   * @brief コンストラクタ
   */
  CloudySky( gfl2::heap::HeapBase* pParent, void* pResourceData);

  /**
   * @brief デストラクタ
   */
  virtual ~CloudySky();

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
   * @brief フェードインリクエスト
   * @param changeFrame 変更フレーム
   */
  void FadeinRequest(u32 changeFrame = 1);

  /**
   * @brief フェードアウトリクエスト
   * @param changeFrame 変更フレーム
   */
  void FadeoutRequest(u32 changeFrame = 1);

  /**
   * @brief フェードが待ち状態(フェードしていない)か
   * @return bool trueで待ち状態
   */
  bool IsFadeIdle(void) {return (m_State == IDLE);}

  /**
   * @brief 表示状態
   * @param visible true で表示
   */
  void SetVisible(bool visible);

private:
  enum STATE{
    IDLE,
    FADE_OUT,
    FADE_IN,
  };

  STATE m_State;
  f32 m_Alpha;
  u32 m_changeFrame;
  u32 m_nowFrame;

  /**
   * @brief αセット
   */
  void SetAlpha(f32 alpha);

  /**
   * @brief フェード処理
   */
  void Fade(void);
};

GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( System )

#endif // SKYBOX_CLOUDY_SKY_H_INCLUDED

