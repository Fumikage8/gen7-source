//=================================================================================================
/**
*  @file   FieldMoveCodeGimmickEncountEscape.cpp
*  @brief  フィールド用 動作コード ギミックエンカウント逃走型
*  @author saita_kazuki
*  @date   2015.10.07
*/
//=================================================================================================
// gfl2
#include <types/include/gfl2_Typedef.h>
// MoveCode
#include "Field/FieldStatic/include/MoveCode/FieldMoveCode_define.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeGimmickEncountEscape.h"

namespace Field{ namespace MoveModel {

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveCodeGimmickEncountEscape::FieldMoveCodeGimmickEncountEscape()
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveCodeGimmickEncountEscape::~FieldMoveCodeGimmickEncountEscape()
{
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理　初期化
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountEscape::InitProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  pMoveCodeWork->moveCodeId = FIELD_MOVE_CODE_GIMMICK_ENCOUNT_ESCAPE; 

  Work *pWork = static_cast<Work*>( FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work)));
  pWork->pActor             = NULL;
  pWork->pPath[PATH_TYPE_1] = NULL;
  pWork->pPath[PATH_TYPE_2] = NULL;
  pWork->state              = STATE_NONE;
  pWork->progressSpeed      = 0.0f;
  pWork->standbySEID        = 0;
  pWork->moveSEID           = 0;
  pWork->progress           = 0.0f;
  pWork->pathType           = PATH_TYPE_NONE;
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  終了
*
*  @param  pMoveCodeWork  動作コードワーク
*  @param  isSave         セーブ時に呼ばれたかどうか
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeGimmickEncountEscape::Retirement(FieldMoveCodeWork* pMoveCodeWork,b32 isSave) const
{
}

}; //end of namespace MoveModel
}; //end of namespace Field
