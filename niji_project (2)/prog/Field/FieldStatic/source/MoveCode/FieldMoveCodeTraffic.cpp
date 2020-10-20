/**
 *  GAME FREAK inc.
 *
 *  @file   FieldMoveCodeTraffic.cpp
 *  @brief  往来NPC専用動作コード
 *  @author miyachi_soichi
 *  @date   2015/06/26
 */

// gfl2
#include <types/include/gfl2_Typedef.h>
// MoveCode
#include "Field/FieldStatic/include/MoveCode/FieldMoveCode_define.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeTraffic.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(MoveModel)

/**
 *  @brief  コンストラクタ
 */
FieldMoveCodeTraffic::FieldMoveCodeTraffic( void )
{
}

/**
 *  @brief  デストラクタ
 */
FieldMoveCodeTraffic::~FieldMoveCodeTraffic( void )
{
}

/**
 *  @brief  動作コード処理　初期化
 */
void FieldMoveCodeTraffic::InitProcess( FieldMoveCodeWork *pMoveCodeWork ) const
{
  // MOVECODEID設定
  pMoveCodeWork->moveCodeId = FIELD_MOVE_CODE_TRAFFIC;

  // WORKの初期化
  Work *pWork = static_cast<Work*>( FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) ) );
  memset( pWork, 0, sizeof(Work) );
  pWork->state = STATE_NONE;

}

GFL_NAMESPACE_END(MoveModel)
GFL_NAMESPACE_END(Field)
