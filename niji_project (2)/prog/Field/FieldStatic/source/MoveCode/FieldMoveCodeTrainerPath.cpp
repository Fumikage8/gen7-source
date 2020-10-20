//======================================================================
/**
 * @file FieldMoveCodeTrainerPath.cpp
 * @date 2015/07/30 19:34:49
 * @author miyachi_soichi
 * @brief パス有トレーナー動作コード
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// gfl2
#include <types/include/gfl2_Typedef.h>
// MoveModel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
// MoveCode
#include "Field/FieldStatic/include/MoveCode/FieldMoveCode_define.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeTrainerPath.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(MoveModel)

static const f32 DEFAULT_EYE_HEIGHT = 6.5f;

/**
*  @brief  コンストラクタ
*/
FieldMoveCodeTrainerPath::FieldMoveCodeTrainerPath(void) 
{
}

/**
*  @brief  デストラクタ
*/
FieldMoveCodeTrainerPath::~FieldMoveCodeTrainerPath()
{
}

/**
 * @class FieldMoveCodeTrainerPath
 * @brief パス有トレーナー動作コード
 */
/**
 *  @brief  動作コード処理  初期化
 */
void FieldMoveCodeTrainerPath::InitProcess( FieldMoveCodeWork *pMoveCodeWork ) const
{
  // MOVECODEID設定
  pMoveCodeWork->moveCodeId = FIELD_MOVE_CODE_TRAINER_PATH;

  Work *pWork = static_cast<Work*>( FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) ) );
  memset( pWork, 0, sizeof(Work) );
  pWork->state              = STATE_NONE;
  pWork->eyeLengthRate      = 1.0f;
  pWork->eyeHeightRate      = DEFAULT_EYE_HEIGHT;
  pWork->motionSpeedRatio   = 1.0f;
  pWork->jointHeadIndex     = -1;
  pWork->locatorEyeIndex    = -1;
  pWork->comebackAnimID     =  0;
  pWork->comebackAnimFrame  = 0.0f;
  pWork->comebackAnimStepFrame  = 0.0f;
  pWork->progress           = 0.0f;
  pWork->pTrainerActor      = NULL;
  pWork->isUseOldEyePos     = false;
}

/**
*  @brief  動作コード処理  終了
*/
void FieldMoveCodeTrainerPath::Retirement(FieldMoveCodeWork* pMoveCodeWork,b32 isSave) const
{
  Work *pWork = static_cast<Work*>( FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) ) );

  if( pMoveCodeWork->pModel->IsSetupResource() )
  {
    pWork->comebackAnimID = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetAnimationId();
    pWork->comebackAnimFrame = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetAnimationFrame();

    f32 stepFrame = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetAnimationStepFrame();
    if( gfl2::math::IsAlmostZero( stepFrame))
    {
      pWork->comebackAnimStepFrame = 0.0f;
    }
    else
    {
      pWork->comebackAnimStepFrame = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetAnimationStepFrame();
    }
  }
}

GFL_NAMESPACE_END(MoveModel)
GFL_NAMESPACE_END(Field)
