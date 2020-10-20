#pragma once
//=================================================================================================
/**
*  @file   FieldMoveCodeGimmickEncountCommon.h
*  @brief  フィールド用 動作コード ギミックエンカウント共通処理
*  @author saita_kazuki
*  @date   2015.10.13
*/
//=================================================================================================
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>

// 前方宣言
namespace poke_3d { namespace model {
  class BaseModel;
}}
namespace Field {
  namespace MoveModel {
    class FieldMoveModel;
  }
  namespace GimmickEncount {
    class LocalWork;
  }
}

namespace Field{ namespace MoveModel {

class FieldMoveCodeGimmickEncountCommon
{
public:
  
  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodeGimmickEncountCommon(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodeGimmickEncountCommon();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アニメーション更新
  */
  //-----------------------------------------------------------------------------
  static void UpdateChangeAnimation( poke_3d::model::BaseModel* pModel, s32 animationID);

  //-----------------------------------------------------------------------------
  /**
  *  @brief アニメーションフレームで制御するSE再生
  *  @param pModel BaseModel
  *  @param pLocalWork ギミックエンカウントローカルワーク
  *  @param targetPos 鳴らすか鳴らさないかを判断するために使用するターゲット位置
  *  @param seID 鳴らすSEラベル
  *  @param animationFrame SEを鳴らすアニメーションフレーム
  */
  //-----------------------------------------------------------------------------
  static void UpdateAnimationSE( poke_3d::model::BaseModel* pModel, GimmickEncount::LocalWork* pLocalWork, const gfl2::math::Vector3& targetPos, u32 seID, f32 animationFrame);

  //-----------------------------------------------------------------------------
  /**
  *  @brief 草シンボルか
  *  @param pModel 動作モデル
  *  @retval true 草シンボル
  *  @retval false それ以外
  */
  //-----------------------------------------------------------------------------
  static bool IsGrassSymbol( FieldMoveModel* pMoveModel);

  //-----------------------------------------------------------------------------
  /**
  *  @brief SE停止 (再生しているかもチェックする)
  *  @param seID 止めたいSEラベル
  */
  //-----------------------------------------------------------------------------
  static void StopSE( u32 seID);


};  // end of class FieldMoveCodeGimmickEncountCommon;
  
}; //end of namespace mmodel
}; //end of namespace fld
