//=================================================================================================
/**
*  @file   FieldMoveCodeGimmickEncountChase.cpp
*  @brief  フィールド用 動作コード ギミックエンカウント接近型
*  @author saita_kazuki
*  @date   2015.02.17
*/
//=================================================================================================
// gfl2
#include <types/include/gfl2_Typedef.h>
// MoveCode
#include "Field/FieldStatic/include/MoveCode/FieldMoveCode_define.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeGimmickEncountChase.h"

namespace Field{ namespace MoveModel {

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveCodeGimmickEncountChase::FieldMoveCodeGimmickEncountChase()
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveCodeGimmickEncountChase::~FieldMoveCodeGimmickEncountChase()
{
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理　初期化
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountChase::InitProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  Work *pWork = static_cast<Work*>( FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work)));
  pWork->pActor       = NULL;
  pWork->state        = STATE_NONE;
  pWork->moveSpeed    = 0.0f;
  pWork->waitCnt      = 0;
  pWork->standbySEID  = 0;
  pWork->moveSEID     = 0;
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  終了
*
*  @param  pMoveCodeWork  動作コードワーク
*  @param  isSave         セーブ時に呼ばれたかどうか
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountChase::Retirement(FieldMoveCodeWork* pMoveCodeWork, b32 isSave) const
{
}

}; //end of namespace MoveModel
}; //end of namespace Field
