//======================================================================
/**
 * @file FieldEffectSandStorm.h
 * @date 2015/12/09 18:08:38
 * @author saito_nozomu
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_SAND_STORM_H_INCLUDED__
#define __FIELD_EFFECT_SAND_STORM_H_INCLUDED__
#pragma once

// gfl2
#include <Effect/include/gfl2_EffectHeap.h>
#include <Effect/include/gfl2_EffectSystem.h>
// poke_3d
#include <model/include/gfl2_BaseModel.h>
// Effect
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( util )
class GLHeapAllocator;
GFL_NAMESPACE_END( util )

namespace renderingengine { namespace scenegraph {
  namespace resource {
    class ResourceNode;
  }
  namespace instance {
    class ModelInstanceNode;
  }
}}

GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN(Field)

class MyRenderingPipeLine;
class FieldRootNode;

GFL_NAMESPACE_BEGIN(Effect)

/**
 *  @class  EffectSandStorm
 *  @brief  砂嵐
 */
class EffectSandStorm : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectSandStorm);

public:
  /**
   *  @brief  初期化構造体
   */
  struct SetupData
  {
    void                        *pResource;
    gfl2::Effect::GFHeap        *pEffectHeap;
    gfl2::Effect::System        *pEffectSystem;
    Field::MyRenderingPipeLine  *pPipeLine;
    FieldRootNode               *pRootNode;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  EffectSandStorm( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectSandStorm( void );

  /**
   *  @brief  初期化処理
   */
  void Initialize( SetupData &setupData );

  /**
   *  @brief  破棄
   */
  virtual bool Terminate( void );

  /**
   *  @brief  各エフェクトで必要な破棄作業
   */
  virtual void Delete( void );

  /**
   *  @brief エフェクト　空用　作成
   */
  void CreateEffect( void );

  /**
   *  @brief 砂嵐モデルを取得
   *  @return 砂嵐モデルへのポインタ
   */
  poke_3d::model::BaseModel* GetSandStormModel(void);

  /**
   *  @brief 削除確認
   */
  bool IsDead(void);

private:
  /**
   *  @brief ビルボードの作成
   */
  void CreateBillboard( void );

protected:
  /**
   *  @brief  更新処理
   */
  virtual void updateNormal( void );

  void createModel( int index, void *pModelBinary );
  void createMotion( int index, void *pModelBinary );
private:
  enum
  {
    BILLBOARD_TYPE_SANDSTORM,
    BILLBOARD_TYPE_NUM,
  };

  gfl2::Effect::GFHeap     *m_pEffectHeap;
  gfl2::Effect::System     *m_pEffectSystem;
  gfl2::Effect::Handle     *m_pEffectHandle;

  void *m_pResource;
  Field::MyRenderingPipeLine  *m_pPipeLine;
  poke_3d::model::BaseModel                                       m_Model[BILLBOARD_TYPE_NUM];
  gfl2::util::GLHeapAllocator                                     *m_pAllocator;
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pModelResourceRootNode[BILLBOARD_TYPE_NUM];
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pMotionResourceRootNode[BILLBOARD_TYPE_NUM];
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode  *m_pModelInstanceNode[BILLBOARD_TYPE_NUM];
  FieldRootNode                                                   *m_pFieldRootNode;      //!< フィールドルートノード
};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_SAND_STORM_H_INCLUDED__
