//=================================================================================================
/**
*  @file   FieldMoveCodeGimmickEncountPatrol.cpp
*  @brief  フィールド用 動作コード ギミックエンカウント巡回型
*  @author saita_kazuki
*  @date   2015.09.05
*/
//=================================================================================================
// gfl2
#include <types/include/gfl2_Typedef.h>
// MoveCode
#include "Field/FieldStatic/include/MoveCode/FieldMoveCode_define.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeGimmickEncountPatrol.h"

namespace Field{ namespace MoveModel {

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveCodeGimmickEncountPatrol::FieldMoveCodeGimmickEncountPatrol()
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveCodeGimmickEncountPatrol::~FieldMoveCodeGimmickEncountPatrol()
{
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理　初期化
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountPatrol::InitProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  Work *pWork = static_cast<Work*>( FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work)));
  pWork->pActor         = NULL;
  pWork->pPath          = NULL;
  pWork->state          = STATE_NONE;
  pWork->progressSpeed  = 0.0f;
  pWork->standbySEID    = 0;
  pWork->moveSEID       = 0;
  pWork->progress       = 0.0f;
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  終了
*
*  @param  pMoveCodeWork  動作コードワーク
*  @param  isSave         セーブ時に呼ばれたかどうか
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountPatrol::Retirement(FieldMoveCodeWork* pMoveCodeWork,b32 isSave) const
{
}

}; //end of namespace MoveModel
}; //end of namespace Field
