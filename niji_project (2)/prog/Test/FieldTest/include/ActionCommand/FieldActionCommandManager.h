#pragma once
//=================================================================================================
/**
*  @file   FieldActionCommandManager.h
*  @brief  フィールドアクションコマンドマネージャー
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <heap/include/gfl2_Heap.h>
#include "../../../../../../poke_3d_lib/model/include/gfl2_CharaModelFactory.h"
#include "../../include/MoveModel/FieldMoveModel.h"
#include "FieldActionCommandBase.h"

namespace Field{ namespace MoveModel {

//----------------------------------------------------------------------------------------
//! @brief フィールドアクションコマンドマネージャー
//----------------------------------------------------------------------------------------
class FieldActionCommandManager
{
public:

  //=====================================================================================================================
  //! @brief マネージャー自身に関する操作
  //=====================================================================================================================

  //----------------------------------------------------------------------------
  /**
  *  @brief    初期化
  *
  *  @param    pHeap    ヒープ
  */
  //-----------------------------------------------------------------------------
  static void Initialize(gfl2::heap::HeapBase* pHeap);

  //----------------------------------------------------------------------------
  /**
  *  @brief    破棄
  */
  //-----------------------------------------------------------------------------
  static void Finalize(void);

  //=====================================================================================================================
  //! @brief アクションコマンドのワークに関する操作
  //=====================================================================================================================

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドワークの初期化
  *
  *  @param  pActionCommandWork   アクションコマンドワーク
  *  @param  pModel               動作モデル
  */
  //-----------------------------------------------------------------------------
  static void InitializeActionCommandWork(ActionCommandWork* pActionCommandWork, Field::MoveModel::FieldMoveModel* pModel);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドワークのリセット
  *
  *  @param  pActionCommandWork  アクションコマンドワーク
  */
  //-----------------------------------------------------------------------------
  static void ClearActionCommandWork(ActionCommandWork* pActionCommandWork);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドワークから共有ワークを取得
  *
  *  @param  pActionCommandWork  アクションコマンドワーク
  *  @param  size                必要なサイズ
  *
  *  @return 共有ワーク
  */
  //-----------------------------------------------------------------------------
  static void* GetActionCommandWork(ActionCommandWork* pActionCommandWork, u32 size);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドワークにアクションコマンドの設定
  *
  *  @param  pActionCommandWork  アクションコマンドワーク
  *  @param  actionCommandId     アクションコマンドID
  */
  //-----------------------------------------------------------------------------
  static void SetActionCommand(ActionCommandWork* pActionCommandWork, FieldActionCommandId actionCommandId);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドワークからアクションコマンドの実行
  *
  *  *コマンドの終了状態を保持している。
  *  *コマンドを実行した人が、ActionCommandWorkをクリアする必要がある。
  */
  //-----------------------------------------------------------------------------
  static void UpdateActionCommand(Field::MoveModel::ActionCommandWork* pActionCommandWork);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドワーク内にあるアクションコマンドのシーケンス(実行状況)を指定分だけ進める
  *
  *  @param  pActionCommandWork  アクションコマンドワーク
  *  @param  add                 進めたいシーケンス値
  */
  //-----------------------------------------------------------------------------
  static void AddActionCommandWorkSequence(ActionCommandWork* pActionCommandWork, s32 add);

  //----------------------------------------------------------------------------
  /**
  * @brief  共通終了処理
  *
  * @param  pActionCommandWork  アクションコマンドワーク
  *
  * @retval ACMD_RET_END
  */
  //-----------------------------------------------------------------------------
  static ACTION_COMMAND_RETURN ActionCommandEnd(ActionCommandWork* pActionCommandWork);

private:

  //! アクションコマンドテーブル
  static ActionCommandBase* m_pTbl[ FIELD_ACTION_COMMAND_MAX ];
  
};

}; //end of namespace MoveModel
}; //end of namespace Field
