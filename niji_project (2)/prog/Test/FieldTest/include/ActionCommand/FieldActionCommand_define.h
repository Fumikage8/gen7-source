#pragma once
//=================================================================================================
/**
*  @file   FieldActionCommand_define.h
*  @brief  フィールド用 アクションコマンドの定義
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <math/include/gfl2_Quaternion.h>

namespace Field{ namespace MoveModel {

  class FieldMoveModel;

  //----------------------------------------------------------------------------------------
  //! @brief アクションコマンド用のID
  //----------------------------------------------------------------------------------------
  enum FieldActionCommandId
  {
    FIELD_ACTION_COMMAND_NONE,          ///< 何も動かないアクションコマンド
    FIELD_ACTION_COMMAND_TEMPLATE,      ///< 量産用のテンプレートアクションコマンド
    FIELD_ACTION_COMMAND_MAX,           ///< アクションコマンド最大数
  };

  //-----------------------------------------------------------------------------
  //! @brief アクションコマンド 更新の戻り値
  //-----------------------------------------------------------------------------
  enum ACTION_COMMAND_RETURN
  {
    ACTION_COMMAND_RETURN_NONE,           ///< 何も実行されなかった
    ACTION_COMMAND_RETURN_CONTINUE,       ///< 続く
    ACTION_COMMAND_RETURN_ONE_TIME_END,   ///< このフレームの処理は終わり
    ACTION_COMMAND_RETURN_END,            ///< コマンドの処理完了
  };
  
  //-----------------------------------------------------------------------------
  //! @brief 動作モデルの各インスタンスが保持するアクションコマンドのワークサイズ
  //-----------------------------------------------------------------------------
  static const u32 ACTION_COMMAND_WORK_SIZE  = 256;

  //-----------------------------------------------------------------------------
  //! @brief アクションコマンド処理の共通ワーク
  //-----------------------------------------------------------------------------
  struct ActionCommandWork
  {
    Field::MoveModel::FieldMoveModel* pModel;               ///< 動作モデル
    FieldActionCommandId              actionCommandId;      ///< アクションコマンドの種類
    u32                               sequence;             ///< シーケンス
    u32                               work[ACTION_COMMAND_WORK_SIZE/4];  ///< 共有ワーク
  };

}; //end of namespace MoveModel
}; //end of namespace Field
