//===================================================================
/**
 * @file    SkyboxCloud.h
 * @brief   天球雲クラス
 * @author  saita_kazuki
 * @date    2015.05.07
 */
//===================================================================

#if !defined( SKYBOX_CLOUD_H_INCLUDED )
#define SKYBOX_CLOUD_H_INCLUDED
#pragma once

#include "./SkyboxPartsBase.h"

GFL_NAMESPACE_BEGIN( System )
GFL_NAMESPACE_BEGIN( Skybox )

/**
 * @class 天球雲クラス
 */
class Cloud : public PartsBase
{
  /**
   * @briefコピーコンストラクタ＋代入禁止
   */
  GFL_FORBID_COPY_AND_ASSIGN( Cloud );

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
    ANIMATION_TYPE_ALLTIME_SHADER,  ///< 1日アニメーション
    ANIMATION_TYPE_UV,              ///< 通常UVアニメーション

    ANIMATION_TYPE_MAX
  };

  ///< リソース
  struct ResourceData
  {
    void*   pModelData[ MODEL_TYPE_MAX ];
    void*   pMotionData[ ANIMATION_TYPE_MAX ];
  };

public:

  /**
   * @brief コンストラクタ
   */
  Cloud( gfl2::heap::HeapBase* pParent, void* pResourceData, bool isAllocateByVRAM, System::nijiAllocator::VramMallocMode vramMallocMode);

  /**
   * @brief デストラクタ
   */
  virtual ~Cloud();

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

private:

  ///< アニメーションスロット番号
  enum AnimationIndex
  {
    // マテリアル
    ANIMATION_INDEX_ALLTIME_SHADER = 0,  ///< 1日アニメーション
    ANIMATION_INDEX_UV,                  ///< 通常UVアニメーション
  };


};

GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( System )

#endif // SKYBOX_CLOUD_H_INCLUDED

