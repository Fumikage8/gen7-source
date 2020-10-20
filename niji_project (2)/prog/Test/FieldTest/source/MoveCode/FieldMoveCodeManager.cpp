//=================================================================================================
/**
*  @file   FieldMoveCodeManager.cpp
*  @brief  フィールド用 動作コードマネージャー
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include "../../include/MoveCode/FieldMoveCodeManager.h"
#include "../../include/MoveCode/FieldMoveCodeBase.h"
#include "../../include/MoveCode/FieldMoveCodeTemplate.h"
#include "../../include/MoveCode/FieldMoveCodePlayer.h"

namespace Field{ namespace MoveModel {

//=====================================================================================================================
// マネージャー自身に関する操作
//=====================================================================================================================

const FieldMoveCodeBase * FieldMoveCodeManager::m_MoveCodeTable[ FIELD_MOVE_CODE_NUM ] ={};

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveCodeManager::FieldMoveCodeManager()
{
  for (int i = 0; i < FIELD_MOVE_CODE_NUM; ++ i )
  {
    m_MoveCodeTable[i] = NULL;
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveCodeManager::~FieldMoveCodeManager()
{
  Terminate();
}

//----------------------------------------------------------------------------
/**
*  @brief  初期化
*
*  @param  pHeap  ワーク作成時に利用するヒープ
*  @return trueは成功、falseは失敗
*/
//-----------------------------------------------------------------------------
bool FieldMoveCodeManager::Initialize(gfl2::heap::HeapBase * pHeap)
{
  m_MoveCodeTable[FIELD_MOVE_CODE_NONE]       = GFL_NEW( pHeap ) FieldMoveCodeBase();
  m_MoveCodeTable[FIELD_MOVE_CODE_PLAYER]     = GFL_NEW( pHeap ) FieldMoveCodePlayer();
  m_MoveCodeTable[FIELD_MOVE_CODE_TEMPLATE]   = GFL_NEW( pHeap ) FieldMoveCodeTemplate();
  return true;
}

//----------------------------------------------------------------------------
/**
*  @brief  解放
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeManager::Terminate(void)
{
  for (int i = 0; i < FIELD_MOVE_CODE_NUM; ++ i )
  {
    GFL_SAFE_DELETE( m_MoveCodeTable[i] );
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コードの取得
*
*  @param  fieldMoveCodeId   動作コードID
*  @return 動作コード
*/
//-----------------------------------------------------------------------------
const FieldMoveCodeBase * FieldMoveCodeManager::GetMoveCode(FIELD_MOVE_CODE_ID fieldMoveCodeId)
{
  const FieldMoveCodeBase * move_code = m_MoveCodeTable[static_cast<u32>(fieldMoveCodeId)];
  if ( move_code )
  {
    return move_code;
  }
  return m_MoveCodeTable[0];  //MOVE_CODE_NONE
}

//=====================================================================================================================
// 動作コードのワークに関する操作
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
*  @brief  動作コードのワークを初期化
*
*  @param  pMoveCodeWork  動作コードワーク
*  @param  pModel         動作モデル
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeManager::InitializeMoveCodeWork(FieldMoveCodeWork* pMoveCodeWork, FieldMoveModel* pModel)
{
  pMoveCodeWork->pModel     = pModel;
  pMoveCodeWork->moveCodeId = FIELD_MOVE_CODE_NONE;
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コードのワークを取得
*    
*  @param  pMoveCodeWork  動作コード動作コードワーク
*  @param  size           必要なサイズ
*  @return 動作コードワーク
*/
//-----------------------------------------------------------------------------
void* FieldMoveCodeManager::GetMoveCodeWork(FieldMoveCodeWork* pMoveCodeWork, u32 size)
{
  GFL_ASSERT( size <= FIELD_MOVE_CODE_WORK_SIZE );
  return reinterpret_cast<void*>(pMoveCodeWork->work);
}

}; //end of namespace MoveModel
}; //end of namespace Field
