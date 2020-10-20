//=================================================================================================
/**
*  @file   FieldEffectGimmickEncount.h
*  @brief  ギミックエンカウントの定義
*  @author saita_kazuki
*  @date   2015.10.06
*/
//=================================================================================================

#if !defined __FIELD_EFFECT_GIMMICK_ENCOUNT_H_INCLUDED__
#define __FIELD_EFFECT_GIMMICK_ENCOUNT_H_INCLUDED__
#pragma once

// gfl2
#include <Effect/include/gfl2_EffectHeap.h>
#include <Effect/include/gfl2_EffectSystem.h>
// Effect
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
 *  @class  EffectGimmickEncount
 *  @brief  ギミックエンカウント
 */
class EffectGimmickEncount : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectGimmickEncount);

public:

  /**
   *  @brief  初期化構造体
   */
  struct SetupData
  {
    gfl2::Effect::GFHeap*        pEffectHeap;
    gfl2::Effect::System*        pEffectSystem;
    gfl2::math::Vector3          vPos;
  };

  /**
   *  @brief  エフェクト種類
   */
  enum EffectType
  {
    EFFECT_TYPE_NONE,                     ///< なし
    EFFECT_TYPE_SCATTER_GRASS_TREE_0,     ///< 葉が飛び散る(通常の木)
    EFFECT_TYPE_SCATTER_GRASS_PLANT_0,    ///< 葉が飛び散る(通常の植物)

    EFFECT_TYPE_MAX
  };

public:

  /**
   *  @brief  コンストラクタ
   */
  EffectGimmickEncount( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectGimmickEncount( void );

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
   *  @brief  エフェクト生成
   */
  void Create( EffectType type );

protected:

  /**
   *  @brief  更新処理
   */
  virtual void updateNormal( void );

private:

  gfl2::Effect::GFHeap*       m_pEffectHeap;
  gfl2::Effect::System*       m_pEffectSystem;
  gfl2::Effect::Handle*       m_pEffectHandle;
  gfl2::math::Vector3         m_vPos;
};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_GIMMICK_ENCOUNT_H_INCLUDED__
