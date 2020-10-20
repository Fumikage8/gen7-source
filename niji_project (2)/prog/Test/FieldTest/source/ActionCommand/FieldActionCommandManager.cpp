//=================================================================================================
/**
*  @file   FieldActionCommandManager.cpp
*  @brief  フィールド用 アクションコマンドマネージャー
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================

#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_Assert.h>
#include "../../include/ActionCommand/FieldActionCommand_define.h"
#include "../../include/ActionCommand/FieldActionCommandManager.h"
#include "../../include/ActionCommand/FieldActionCommandBase.h"
#include "../../include/ActionCommand/FieldActionCommandTemplate.h"

namespace Field{ namespace MoveModel{

//=====================================================================================================================
//! @brief マネージャー自身に関する操作
//=====================================================================================================================

// アクションコマンドテーブル
ActionCommandBase* FieldActionCommandManager::m_pTbl[ FIELD_ACTION_COMMAND_MAX ];

//----------------------------------------------------------------------------
/**
 *  @brief    初期化
 *
 *  @param    pHeap    ヒープ
 */
//-----------------------------------------------------------------------------
void FieldActionCommandManager::Initialize(gfl2::heap::HeapBase* pHeap)
{
  // m_pTblを生成
  m_pTbl[ FIELD_ACTION_COMMAND_NONE ]     = GFL_NEW( pHeap ) ActionCommandBase();
  m_pTbl[ FIELD_ACTION_COMMAND_TEMPLATE ] = GFL_NEW( pHeap ) ActionCommandTemplate();

  // 振り向き系

  // 移動系

  // その場移動系

  // ジャンプ系

  // ウェイト系

  // 描画フラグ系

  // ワープ系

  // アイコン表示
  
  // ローラースケート系

  // 忍び足系

  // 自転車系

  // 波乗り系

  // サイホーン系

  // やぎ系

  // アニメーションアクション

#if PM_DEBUG
  // 整合性チェック
  for( u32 i=0; i<FIELD_ACTION_COMMAND_MAX; ++i )
  {
    if( m_pTbl[i]->GetActionCommandID() != i ){
      GFL_ASSERT( 0 );
      break;
    }
  }
#endif
}


//----------------------------------------------------------------------------
/**
 *  @brief    破棄
 */
//-----------------------------------------------------------------------------
void FieldActionCommandManager::Finalize(void)
{
  for( u32 i=0; i<FIELD_ACTION_COMMAND_MAX; ++i )
  {
    if( m_pTbl[i] )
    {
      GFL_DELETE m_pTbl[i];
      m_pTbl[i] = NULL;
    }
  }
}

//=====================================================================================================================
//! @brief ActionCommandのワークに関する操作
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドワークの初期化
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @param  pModel             動作モデル
*/
//-----------------------------------------------------------------------------
void FieldActionCommandManager::InitializeActionCommandWork(ActionCommandWork* pActionCommandWork, Field::MoveModel::FieldMoveModel* pModel)
{
  pActionCommandWork->pModel          = pModel;
  pActionCommandWork->actionCommandId = static_cast<FieldActionCommandId>(0);
  pActionCommandWork->sequence        = 0;
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドワークのリセット
*
*  @param  pActionCommandWork  アクションコマンドワーク
*/
//-----------------------------------------------------------------------------
void FieldActionCommandManager::ClearActionCommandWork(ActionCommandWork* pActionCommandWork)
{
  pActionCommandWork->actionCommandId = static_cast<FieldActionCommandId>(0);
  pActionCommandWork->sequence        = 0;
  pActionCommandWork->pModel->offStateBit( Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_ACMD );
  pActionCommandWork->pModel->offStateBit( Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_ACMD_END );
}

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
void* FieldActionCommandManager::GetActionCommandWork(ActionCommandWork* pActionCommandWork, u32 size)
{
  GFL_ASSERT( size < ACTION_COMMAND_WORK_SIZE );
  return reinterpret_cast<void*>(pActionCommandWork->work);
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドワークにアクションコマンドの設定
*
*  @param  pActionCommandWork  アクションコマンドワーク
*  @param  actionCommandId     アクションコマンドID
*/
//-----------------------------------------------------------------------------
void FieldActionCommandManager::SetActionCommand(ActionCommandWork* pActionCommandWork, FieldActionCommandId actionCommandId)
{
  GFL_ASSERT( actionCommandId < FIELD_ACTION_COMMAND_MAX );
  pActionCommandWork->actionCommandId = actionCommandId;
  pActionCommandWork->sequence        = 0;
  pActionCommandWork->pModel->onStateBit ( Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_ACMD );
  pActionCommandWork->pModel->offStateBit( Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_ACMD_END );
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドワークからアクションコマンドの実行
*
*  *コマンドの終了状態を保持している。
*  *コマンドを実行した人が、ActionCommandWorkをクリアする必要がある。
*/
//-----------------------------------------------------------------------------
void FieldActionCommandManager::UpdateActionCommand(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  ActionCommandBase* pAcmd;
  
  if( pActionCommandWork->actionCommandId < FIELD_ACTION_COMMAND_MAX )
  {
    pAcmd = m_pTbl[pActionCommandWork->actionCommandId];
  }
  else
  {
    GFL_ASSERT( pActionCommandWork->actionCommandId < FIELD_ACTION_COMMAND_MAX );
    pAcmd      = m_pTbl[FIELD_ACTION_COMMAND_NONE];
    pActionCommandWork->actionCommandId = FIELD_ACTION_COMMAND_NONE;
    pActionCommandWork->sequence        = 0;
  }

  // 実行
  pAcmd->UpdateActionCommand( pActionCommandWork );
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドワーク内にあるアクションコマンドのシーケンス(実行状況)を指定分だけ進める
*
*  @param  pActionCommandWork  アクションコマンドワーク
*  @param  add                 進めたいシーケンス値
*/
//-----------------------------------------------------------------------------
void FieldActionCommandManager::AddActionCommandWorkSequence(ActionCommandWork* pActionCommandWork, s32 add)
{
  if( (static_cast<s32>(pActionCommandWork->sequence) + add) >= 0 )
  {
    pActionCommandWork->sequence += add;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  共通終了処理
 *
 *  @param  pActionCommandWork  アクションコマンドワーク
 *
 *  @retval ACMD_RET_END
 */
//-----------------------------------------------------------------------------
ACTION_COMMAND_RETURN FieldActionCommandManager::ActionCommandEnd(ActionCommandWork* /*pActionCommandWork*/)
{
  return ACTION_COMMAND_RETURN_END;
}

} // MoveModel
} // Field
