/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEffectLensFlare.h
 *  @brief  フィールドエフェクト：レンズフレア
 *  @author miyachi_soichi
 *  @date   2015.06.10
 */

#if !defined(__FIELD_EFFECT_LENSFLARE_H__)
#define __FIELD_EFFECT_LENSFLARE_H__

// gfl2
#include <Animation/include/gfl2_AnimationController.h>

// poke_3d
#include <model/include/gfl2_BaseModel.h>

// Field
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"

// 前方宣言
GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( util )
class GLHeapAllocator;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN( Field )
class MyRenderingPipeLine;
class FieldRootNode;
GFL_NAMESPACE_END( Field )

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

class EffectLensFlare : public IEffectBase
{
  //! コピー禁止
  GFL_FORBID_COPY_AND_ASSIGN( EffectLensFlare );

public:
  /**
   *  初期化構造体
   */
  struct SetupData
  {
    void                        *pResource;
    gfl2::heap::HeapBase        *pHeap;
    FieldRootNode               *pRootNode;
    Field::MyRenderingPipeLine  *pPipeLine;
    gfl2::math::Vector3         vPos;
    gfl2::math::Quaternion      quaternion;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  EffectLensFlare( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectLensFlare( void );

  /**
   *  @brief  初期化処理：座標指定
   *  @param  setupData 初期化構造体
   */
  virtual void Initialize( SetupData &setupData );

  /**
   *  @brief  破棄処理
   */
  virtual bool Terminate( void );

  /**
   *  @brief  更新処理
   */
  virtual void updateNormal( void );

  /**
   *  @brief  各エフェクトで必要な破棄作業
   */
  virtual void Delete( void );

  /**
   *  @brief  アニメーション再生終了を検知
   *  @return アニメーション終了
   */
  virtual bool IsAnimationLastFrame( void );

private:
  /**
   *  @brief  モデルの作成
   *  @param  pModelBinary  モデルバイナリ
   */
  void createModel( void *pModelBinary );

  /**
   *  @brief  モーションの作成
   *  @param  pMotionBinary モーションバイナリ
   */
  void createMotion( void *pMotionBinary );

private:
  poke_3d::model::BaseModel                                       m_Model;

  gfl2::util::GLHeapAllocator                                     *m_pAllocator;
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pModelResourceRootNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pMotionResourceRootNode;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode  *m_pModelInstanceNode;
  FieldRootNode                                                   *m_pFieldRootNode;      //!< フィールドルートノード
  Field::MyRenderingPipeLine                                      *m_pRenderingPipeLine;  //!< 描画シーンへの追加削除用

  u32 m_nLife;
}; // class EffectLensFlare : public IEffectBase

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )

#endif // !defined(__FIELD_EFFECT_LENSFLARE_H__)
