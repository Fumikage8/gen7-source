//======================================================================
/**
 * @file FieldEffectSnow.h
 * @date 2015/09/03 11:21:30
 * @author sazito_nozomu
 * @brief 雪
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_SNOW_H_INCLUDED__
#define __FIELD_EFFECT_SNOW_H_INCLUDED__
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
 *  @class  EffectSnow
 *  @brief  雪
 */
class EffectSnow : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectSnow);

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
  EffectSnow( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectSnow( void );

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
   *  @aram id エフェクトＩＤ
   *  @param pos 座標
   *  @param dir 方向
   *  @param rot　回転
   *  @param emitter_length　距離
   */
  void CreateSkyEffect( Effect::EffectResourceID id, const gfl2::math::Vector3 &pos, float emitter_length );

  /**
   *  @brief エミッター　空用　作成
   *  @aram id エフェクトＩＤ
   *  @param pos 座標
   */
  void CreateEmitter( Effect::EffectResourceID id, const gfl2::math::Vector3 &pos );


  /**
   *  @brief ダイヤモンドダスト地面跳ね返り
   *  @param 座標
   */
  void CreateGroundDiamondDust( const gfl2::math::Vector3 &pos );
  
  /**
   *  @brief 雪モデルを取得
   *  @return 雪モデルへのポインタ
   */
  poke_3d::model::BaseModel* GetSnowModel(void);

  /**
   *  @brief エミッターセットの削除
   */
  void KillEmitter(void);

#if 0
  /**
   *  @brief  座標セット
   */
  void SetPosition( gfl2::math::Vector3 pos );
#endif
  /**
   *  @brief  座標更新
   */
  void UpdatePosition( const gfl2::math::Vector3 &pos, const gfl2::math::Vector3 &dir);

  /**
   *  @brief 削除確認
   */
  bool IsDead(void);

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
  void createModel( int index, void *pModelBinary );
  void createMotion( int index, void *pModelBinary );
private:
  enum
  {
    BILLBOARD_TYPE_SNOWSTORM,
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

#endif // __FIELD_EFFECT_SNOW_H_INCLUDED__
