#pragma once
//=================================================================================================
/**
*  @file   FieldMoveModelShadowManager.h
*  @brief  フィールド用 動作モデルの影管理マネージャー
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include "../../include/MoveModel/FieldMoveModel.h"
#include "../../include/MoveModel/FieldMoveModelPlayer.h"
#include "../../include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldRo/include/FieldRootNode.h"

namespace Field
{
  namespace Effect
  {
    class EffectShadow;
    class EffectShadowFoot;
  }
}

namespace Field{ namespace MoveModel {

class FieldMoveModelShadowManager
{
public:

  enum SHADOW_TYPE
  {
    SHADOW_CIRCLE,
    SHADOW_DEPTH,
    SHADOW_WIDTH,
    SHADOW_MAX
  };

  static const f32 SHADOW_OFFSET_FROM_GROUND;

  //-----------------------------------------------------------------------------
  /**
  *  @brief コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveModelShadowManager( Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager,Field::Effect::EffectManager* pEffectManager );

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveModelShadowManager();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  影エフェクトの更新
  */
  //-----------------------------------------------------------------------------
  void UpdateShadow(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  影エフェクトの取得
  */
  //-----------------------------------------------------------------------------
  Field::Effect::EffectShadow* GetEffectShadow( u32 index) const ;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  足跡影エフェクトの取得
  */
  //-----------------------------------------------------------------------------
  Field::Effect::EffectShadowFoot* GetEffectShadowFoot() const ;

private:
  
  //=====================================================================================================================
  //! @brief 内部
  //=====================================================================================================================

  // マネージャー
  Field::MoveModel::FieldMoveModelManager*  m_pFieldCharaModelManager;
  Field::Effect::EffectManager*             m_pEffectManager;

  // 影
  Field::Effect::EffectShadow*              m_pEffectShadow[SHADOW_MAX];
  Field::Effect::EffectShadowFoot*          m_pEffectShadowFoot;
};

}; //end of namespace MoveModel
}; //end of namespace Field
