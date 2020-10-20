//=================================================================================================
/**
*  @file   FieldMoveCodeGimmickEncountCommon.cpp
*  @brief  フィールド用 動作コード ギミックエンカウント共通処理
*  @author saita_kazuki
*  @date   2015.10.13
*/
//=================================================================================================
// MoveModel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
// MoveCode
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeGimmickEncountCommon.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeUtility.h"
// Sound
#include "Sound/include/Sound.h"
// Convヘッダー
#include <niji_conv_header/field/chara/chara_model_id.h>
// GimmickEncount
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountLocalWork.h"

#if PM_DEBUG
#include "Field/FieldRo/include/Debug/FieldDebugGimmickEncount.h"
#endif

namespace Field{ namespace MoveModel {

//-----------------------------------------------------------------------------
/**
 * @brief アニメーション変更更新
 */
//-----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountCommon::UpdateChangeAnimation( poke_3d::model::BaseModel* pModel, s32 animationID)
{
  // 指定のモーションではなかったらモーション変更
  if( pModel->GetAnimationId() != animationID)
  {
    pModel->ChangeAnimation( animationID);
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief アニメーションフレームで制御するSE再生
 * @param pModel BaseModel
 * @param pLocalWork ギミックエンカウントローカルワーク
 * @param targetPos 鳴らすか鳴らさないかを判断するために使用するターゲット位置
 * @param seID 鳴らすSEラベル
 * @param animationFrame SEを鳴らすアニメーションフレーム
 */
//-----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountCommon::UpdateAnimationSE( poke_3d::model::BaseModel* pModel, GimmickEncount::LocalWork* pLocalWork, const gfl2::math::Vector3& targetPos, u32 seID, f32 animationFrame)
{
  gfl2::math::Vector3 pos = pModel->GetPosition();
  f32 length = (targetPos - pos).Length();

  // SEを再生する距離内にいるか
  f32 PLAY_SE_LENGTH = 100.0f * 15.0f;
#if PM_DEBUG
  if( Debug::DebugGimmickEncount::m_isSeEnable)
  {
    PLAY_SE_LENGTH = 100.0f * Debug::DebugGimmickEncount::m_playSeLength;
  }
#endif
  if( length > PLAY_SE_LENGTH)
  {
    return;
  }

  // アニメーションフレームが指定フレームか
  f32 nowFrame = pModel->GetAnimationFrame();
  if( gfl2::math::IsSimilar( nowFrame, animationFrame) == false)
  {
    return;
  }

  // SEのIDがNULLでなければ指定SEを再生予約
  if( seID != SMID_NULL)
  {
    pLocalWork->SetSEParameter( true, seID);
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief 草シンボルか
*  @param pModel 動作モデル
*  @retval true 草シンボル
*  @retval false それ以外
*/
//-----------------------------------------------------------------------------
bool FieldMoveCodeGimmickEncountCommon::IsGrassSymbol( FieldMoveModel* pMoveModel)
{
  u32 charaID = pMoveModel->GetCharacterId();
  switch( charaID)
  {
  case CHARA_MODEL_ID_IT0001_00_GRASS:
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief SE停止 (IDがNULLかどうか、再生しているかもチェックする)
*  @param seID 止めたいSEラベル
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountCommon::StopSE( u32 seID)
{
  if( seID != SMID_NULL)
  {
    if( Sound::IsSEPlaying( seID))
    {
      Sound::StopSE( seID);
    }
  }
}

}; //end of namespace MoveModel
}; //end of namespace Field
