#pragma once
//=================================================================================================
/**
*  @file   FieldMoveModel_define.h
*  @brief  フィールド用 動作モデルの定義
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <types/include/gfl2_Typedef.h>
#include "../MoveCode/FieldMoveCode_define.h"

namespace Field{ namespace MoveModel {

  //-----------------------------------------------------------------------------
  //! @brief 動作モデル用のID
  //-----------------------------------------------------------------------------
  enum FIELD_MOVE_MODEL_ID
  {
    FIELD_MOVE_MODEL_NONE  = 0,
    FIELD_MOVE_MODEL_WORK_0,
    FIELD_MOVE_MODEL_MAX
  };

  //-----------------------------------------------------------------------------
  //! @brief 動作モデル用のセーブされない状態  動作ビット
  //-----------------------------------------------------------------------------
  enum FieldMoveModelStateBit
  {
    FIELD_MOVE_MODEL_STATEBIT_NON       = (0),      ///< 無し
    FIELD_MOVE_MODEL_STATEBIT_ACMD      = (1<<1),   ///< アクションコマンド中
    FIELD_MOVE_MODEL_STATEBIT_ACMD_END  = (1<<2),   ///< アクションコマンド終了
  } ;

  //-----------------------------------------------------------------------------
  //! @brief 動作モデル用の作成リクエストデータ
  //-----------------------------------------------------------------------------
  struct FieldMoveModelHeader
  {
    FIELD_MOVE_CODE_ID  moveCodeId;
    u32                 characterId;
  };

}; //end of namespace MoveModel
}; //end of namespace Field
