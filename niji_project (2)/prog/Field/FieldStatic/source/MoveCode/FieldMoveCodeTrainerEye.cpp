//=================================================================================================
/**
*  @file   FieldMoveCodeTrainerEye.cpp
*  @brief  フィールド視線トレーナー用 動作コード
*  @author saita_kazuki
*  @date   2015.06.09
*/
//=================================================================================================
// gfl2
#include <types/include/gfl2_Typedef.h>
// MoveModel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
// MoveCode
#include "Field/FieldStatic/include/MoveCode/FieldMoveCode_define.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeTrainerEye.h"

namespace Field{ namespace MoveModel {

static const f32 DEFAULT_EYE_HEIGHT = 6.5f;

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveCodeTrainerEye::FieldMoveCodeTrainerEye() 
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveCodeTrainerEye::~FieldMoveCodeTrainerEye()
{
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理　初期化
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeTrainerEye::InitProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  // MoveCodeIDを設定
  pMoveCodeWork->moveCodeId = FIELD_MOVE_CODE_TRAINER_EYE; 

  // ワーク初期化
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  memset( pWork, 0, sizeof(Work) );
  pWork->state            = STATE_NONE;
  pWork->motionID         = -1;
  pWork->eyeLengthRate    = 1.0f;
  pWork->eyeHeightRate    = DEFAULT_EYE_HEIGHT;
  pWork->motionSpeedRate  = 1.0f;
  pWork->jointHeadIndex   = -1;
  pWork->locatorEyeIndex  = -1;
  pWork->comebackAnimFrame= 0.0f;
  pWork->comebackAnimStepFrame  = 0.0f;
  pWork->pTrainerActor    = NULL;
  pWork->isUseOldEyePos   = false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  終了
*
*  @param  pMoveCodeWork  動作コードワーク
*  @param  isSave         セーブ時に呼ばれたかどうか
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeTrainerEye::Retirement(FieldMoveCodeWork* pMoveCodeWork,b32 isSave) const
{
  Work *pWork = static_cast<Work*>( FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) ) );

  if( pMoveCodeWork->pModel->IsSetupResource() )
  {
    pWork->comebackAnimFrame = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetAnimationFrame();
    f32 stepFrame = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetAnimationStepFrame();
    if( gfl2::math::IsAlmostZero( stepFrame ) )
    {
      pWork->comebackAnimStepFrame = 0.0f;
    }
    else
    {
      pWork->comebackAnimStepFrame = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetAnimationStepFrame();
    }
  }
}

}; //end of namespace MoveModel
}; //end of namespace Field
