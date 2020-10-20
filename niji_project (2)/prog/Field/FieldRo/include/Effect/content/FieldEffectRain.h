//======================================================================
/**
 * @file FieldEffectRain.h
 * @date 2015/09/03 11:21:30
 * @author sazito_nozomu
 * @brief 雨
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_RAIN_H_INCLUDED__
#define __FIELD_EFFECT_RAIN_H_INCLUDED__
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
 *  @class  EffectRain
 *  @brief  雨
 */
class EffectRain : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectRain);

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
  EffectRain( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectRain( void );

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
   *  @brief 雨
   *  @param emitter_pos エミッター位置
   *  @param pos 座標
   *  @param dir 方向
   *  @param rot　回転
   *  @param emitter_length　距離
   */
  void CreateRain( const gfl2::math::Vector3 &emitter_pos, const gfl2::math::Vector3 &pos, float emitter_length );

  /**
   *  @brief 雨地面跳ね返り
   *  @param 座標
   */
  void CreateSprash( const gfl2::math::Vector3 &pos );

  /**
   *  @brief 雷モデルを取得
   *  @return 雷モデルへのポインタ
   */
  poke_3d::model::BaseModel* GetThunderModel(void);

  /**
   *  @brief 雨モデルを取得
   *  @return 雨モデルへのポインタ
   */
  poke_3d::model::BaseModel* GetRainModel(void);

  /**
   *  @brief 霧モデルを取得
   *  @return 霧モデルへのポインタ
   */
  poke_3d::model::BaseModel* GetMistModel(void);

  
#if 0
  /**
   *  @brief  座標セット
   */
  void SetPosition( gfl2::math::Vector3 pos );
#endif
  /**
   *  @brief  座標更新
   */
  void UpdatePosition( const gfl2::math::Vector3 &pos, const gfl2::math::Vector3 &dir );

  /**
   *  @brief 削除確認
   */
  bool IsDead(void);

  void VisibleEmitter(bool visible);

private:
  /**
   *  @brief ビルボードの作成
   */
  void CreateBillboard( void );

#if 0
  /**
   *  @brief  座標オフセットの設定
   */
  void setOffsetPosition( gfl2::math::Vector3  offsetPos );
#endif
protected:
  /**
   *  @brief  更新処理
   */
  virtual void updateNormal( void );
#if 0
  /**
   *  @brief  座標取得
   */
  void jointPosition( u32 foots, gfl2::math::Vector3 *pos );

  /**
   *  @brief  座標更新
   */
  void updatePosition( void );
#endif
  void EffectRain::createModel( int index, void *pModelBinary );
  void EffectRain::createMotion( int index, void *pModelBinary );
private:
  enum
  {
    BILLBOARD_TYPE_RAIN,
    BILLBOARD_TYPE_THUNDER,
    BILLBOARD_TYPE_MIST,
    BILLBOARD_TYPE_NUM,
  };

  Effect::EffectResourceID m_ResID;
  float m_EmitterLength;

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

#endif // __FIELD_EFFECT_RAIN_H_INCLUDED__
