//=================================================================================================
/**
*  @file   FieldMoveModelManager.cpp
*  @brief  フィールド用 動作モデルマネージャー
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include "../../include/MoveModel/FieldMoveModelManager.h"

// RTTIデバッグシステム
#include "../../include/RttiDebug/RttiEditor.h"
#include "../../include/RttiDebug/RttiManager.h"

namespace Field{ namespace MoveModel {

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveModelManager::FieldMoveModelManager()
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveModelManager::~FieldMoveModelManager()
{
  Terminate();
}

//----------------------------------------------------------------------------
/**
*  @brief  初期化
*
*  @param  pFileManager  ファイルマネージャー
*  @param  pFileHeap    ファイルマネージャーで使われるヒープ
*  @param  arcId        ファイルマネージャー
*  @return trueは成功、falseは失敗
*/
//-----------------------------------------------------------------------------
bool FieldMoveModelManager::Initialize(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pFileHeap, u32 arcId)
{
  m_pHeap = pFileHeap;
  m_charaModelFactory.Initialize(pFileManager, m_pHeap, arcId);
  return true;
}

//-----------------------------------------------------------------------------
/**
*  @brief  解放
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::Terminate(void)
{
  UnloadAll();

  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    m_fieldMoveModel[i].Destroy();
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  前更新(コリジョン以外の座標/姿勢の変更を行う)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::PrevUpdate(void)
{
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if( m_fieldMoveModel[i].isSetup() )
    {
      m_fieldMoveModel[i].PrevUpdate();
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  更新(コリジョンシーンへの座標/姿勢の登録を行う)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::Update(void)
{
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if( m_fieldMoveModel[i].isSetup() )
    {
      m_fieldMoveModel[i].Update();
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  後更新(コリジョンシーンからの結果を適用し最終的な座標/姿勢の決定を行う)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::PostUpdate(void)
{
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if( m_fieldMoveModel[i].isSetup() )
    {
      m_fieldMoveModel[i].PostUpdate();
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作モデルの作成
*
*  @param  fieldMoveModelId       動作モデルID
*  @param  pFieldMoveModelHeader  動作モデルリクエスト
*  @return 成否
*/
//-----------------------------------------------------------------------------
bool FieldMoveModelManager::Create(FIELD_MOVE_MODEL_ID fieldMoveModelId,const FieldMoveModelHeader* pFieldMoveModelHeader)
{ 
  // @todo Gfxアロケータとアニメーションヒープの引数増やしたので対応お願いします 4/18 竹部
  if( m_charaModelFactory.Create( gfl2::gfx::GLMemory::GetAllocator(), gfl2::heap::Manager::GetDefaultNewUseHeap(), &m_fieldMoveModel[ (u32)fieldMoveModelId ] , pFieldMoveModelHeader->characterId ) )
  {
    m_fieldMoveModel[ (u32)fieldMoveModelId ].Setup( pFieldMoveModelHeader );
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作モデルの削除
*
*  @param  fieldMoveModelId  動作モデルID
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::Delete(FIELD_MOVE_MODEL_ID fieldMoveModelId)
{ 
  m_fieldMoveModel[ (u32)fieldMoveModelId ].Destroy();
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIエディタに自身のインスタンスを登録
*
*  @param  エディタに表示したい動作モデルID
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::CreateRttiEditorPage(FIELD_MOVE_MODEL_ID fieldMoveModelId)
{
#if USE_RTTI_DEBUG
  if( fieldMoveModelId == FIELD_MOVE_MODEL_WORK_0 ) { m_fieldMoveModel[ (u32)fieldMoveModelId ].CreateRttiEditorPage( L"ピカチュウ"); }
#endif
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIエディタから自身のインスタンスを削除
*
*  @param  エディタから削除したい動作モデルID
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::DeleteRttiEditorPage(FIELD_MOVE_MODEL_ID fieldMoveModelId)
{
#if USE_RTTI_DEBUG
  if( fieldMoveModelId == FIELD_MOVE_MODEL_WORK_0 ) { m_fieldMoveModel[ (u32)fieldMoveModelId ].DeleteRttiEditorPage(); }
#endif
}

}; //end of namespace MoveModel
}; //end of namespace Field
