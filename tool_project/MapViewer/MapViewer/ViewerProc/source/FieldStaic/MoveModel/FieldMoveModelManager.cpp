//=================================================================================================
/**
*  @file   FieldMoveModelManager.cpp
*  @brief  フィールド用 動作モデルマネージャー
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <heap/include/gfl2_Heap.h>
#include <arc_def_index/arc_def.h>
#include "ViewerProc/include/FieldStatic/MoveModel/FieldMoveModelManager.h"
#include "ViewerProc/include/FieldRo/MyRenderingPipeLine.h" 
#include <renderingengine/include/SceneGraph/Instance/gfl2_InstanceNode.h>
// RTTIデバッグシステム
///#include "../../include/RttiDebug/RttiEditor.h"
///#include "../../include/RttiDebug/RttiManager.h"

namespace Field{ namespace MoveModel {

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveModelManager::FieldMoveModelManager()
: m_pInstanceRootNode(NULL)
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveModelManager::~FieldMoveModelManager()
{
  TerminateWork();
  TerminateResource();
}

//----------------------------------------------------------------------------
/**
*  @brief  初期化(ワーク)
*
*  @param  pInstanceRootNode    ツリーを形成するためのルートノード
*  @param  pFileHeap            ワークで作成に利用されるヒープ
*
*  @note   ここで利用されるヒープのアドレスは内部で保持してはいけません。どのアプリか関係なくいつもいきているためゲーム起動時のみ利用可
*
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::InitializeWork(gfl2::renderingengine::scenegraph::instance::InstanceNode* pInstanceRootNode,
                                           gfl2::heap::HeapBase*                                      pHeap)
{
  m_pInstanceRootNode = pInstanceRootNode;
  m_pInstanceRootNode->AddChild( &m_fieldMoveModelRoot );

	m_ResourceAllocator.init(pHeap);
}

//----------------------------------------------------------------------------
/**
*  @brief  初期化(リソース)
*
*  @param  MyRenderingPipeLine  レンダリングパイプライン
*  @param  pAllocator           アロケータ
*  @param  pHeap                ヒープ
*  @param  pFileManager         ファイルマネージャー
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::InitializeResource(Field::MyRenderingPipeLine* pMyRenderingPipeLine,
                                               gfl2::gfx::IGLAllocator*    pAllocator,
                                               gfl2::heap::HeapBase*       pHeap,
                                               gfl2::fs::AsyncFileManager* pFileManager )
{
  m_pMyRenderingPipeLine  = pMyRenderingPipeLine;
  m_pAllocator            = pAllocator;
  m_pHeap                 = pHeap;
  m_pFileManager          = pFileManager;
  ///m_charaModelFactory.Initialize(pFileManager, m_pHeap, ARCID_CHARA_MODEL_FIELD);
}

//-----------------------------------------------------------------------------
/**
*  @brief  解放(ワーク)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::TerminateWork(void)
{
  // 動作モデルをツリーから登録解除しワークを削除
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if(  m_fieldMoveModel[i].IsSetupWork() )
    {
      m_fieldMoveModelRoot.RemoveChild( &m_fieldMoveModel[i] );
      m_fieldMoveModel[i].ClearWork();
    }
  }
  // 動作モデルをツリーを解放
  m_pInstanceRootNode->RemoveChild( &m_fieldMoveModelRoot );
  m_pInstanceRootNode = NULL;
}

//-----------------------------------------------------------------------------
/**
*  @brief  解放(リソース)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::TerminateResource(void)
{
  // パイプラインから登録解除しリソースを削除
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if(  m_fieldMoveModel[i].IsSetupResource() )
    {
      m_pMyRenderingPipeLine->RemoveEdgeRenderingTarget( m_fieldMoveModel[i].GetCharaDrawInstance()->GetModelInstanceNode() );
      m_fieldMoveModel[i].ClearResource();
    }
  }
  // キャラモデルファクトリーの解放
  ///m_charaModelFactory.Finalize();
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
    if( m_fieldMoveModel[i].IsSetup() )
    {
      m_fieldMoveModel[i].PrevUpdate();
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  使われていない動作モデルの要素を返す 
*
*  @return 動作モデルの要素(FIELD_MOVE_MODEL_MAXの場合はエラー)
*/
//-----------------------------------------------------------------------------
u32 FieldMoveModelManager::GetFieldMoveModelIndexFromFreeSpace(void) 
{ 
  for( u32 i=FIELD_MOVE_MODEL_NPC_START ; i<FIELD_MOVE_MODEL_MAX ; i++ ) 
  { 
    if( !(m_fieldMoveModel[i].IsSetupWork() || m_fieldMoveModel[i].IsSetupResource()) ) 
    { 
      return i; 
    } 
  }
  return FIELD_MOVE_MODEL_MAX; 
}

//-----------------------------------------------------------------------------
/**
*  @brief  イベントID(識別用のユニークなID)から動作モデルの要素を返す 
*
*  @param  eventId               イベントID
*  @return 動作モデルの要素(FIELD_MOVE_MODEL_MAXの場合はエラー)
*/
//-----------------------------------------------------------------------------
u32 FieldMoveModelManager::GetFieldMoveModelIndexFromEventId(u32 eventId) 
{ 
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ ) 
  { 
    if( m_fieldMoveModel[i].GetEventId() == eventId ) 
    { 
      return i; 
    } 
  }
  return FIELD_MOVE_MODEL_MAX; 
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
    if( m_fieldMoveModel[i].IsSetup() )
    {
      m_fieldMoveModel[i].Update();
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  後更新(コリジョンシーンからの結果を適用し、最終的な座標/姿勢の決定/イベントの実行を行う)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::PostUpdate(void)
{
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if( m_fieldMoveModel[i].IsSetup() )
    {
      m_fieldMoveModel[i].PostUpdate();
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作モデルのワーク作成
*
*  @param  fieldMoveModelId       動作モデルID
*  @param  pFieldMoveModelHeader  動作モデルリクエスト
*  @return 成否
*/
//-----------------------------------------------------------------------------
FieldMoveModelManager::CREATE_STATE FieldMoveModelManager::CreateWork(FIELD_MOVE_MODEL_ID fieldMoveModelId,const FieldMoveModelHeader* pFieldMoveModelHeader)
{
  // ワーク初期化
  if( !m_fieldMoveModel[ (u32)fieldMoveModelId ].IsSetupWork() )
  {
    // 動作モデルのワークを初期化
    m_fieldMoveModel[ (u32)fieldMoveModelId ].SetupWork( pFieldMoveModelHeader );
    // 動作モデルをツリーに登録
    m_fieldMoveModelRoot.AddChild( &m_fieldMoveModel[ (u32)fieldMoveModelId ] );
    return CREATE_SUCCESS;
  }
  return CREATE_ERROR;
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作モデルのリソース作成
*
*  @param  fieldMoveModelId       動作モデルID
*  @param  pFieldMoveModelHeader  動作モデルリクエスト
*  @return 成否
*/
//-----------------------------------------------------------------------------
FieldMoveModelManager::CREATE_STATE FieldMoveModelManager::CreateResource(FIELD_MOVE_MODEL_ID fieldMoveModelId,const FieldMoveModelHeader* pFieldMoveModelHeader)
{
  // リソース初期化
  if( !m_fieldMoveModel[ (u32)fieldMoveModelId ].IsSetupResource() )
  {
    // 動作モデルのリソースを初期化
		m_fieldMoveModel[ (u32)fieldMoveModelId ].SetupResource( m_pAllocator, m_pHeap , /*&m_charaModelFactory,*/ pFieldMoveModelHeader, &m_ResourceAllocator );
    // 動作モデルを描画パイプラインに登録
    m_pMyRenderingPipeLine->AddEdgeRenderingTarget( m_fieldMoveModel[ (u32)fieldMoveModelId ].GetCharaDrawInstance()->GetModelInstanceNode() );
    // ワークからリソースの状態を元に戻す
    if( m_fieldMoveModel[ (u32)fieldMoveModelId ].IsSetupWork() )
    {
      m_fieldMoveModel[ (u32)fieldMoveModelId ].ReturnResourceStateFromWork();
    }
    return CREATE_SUCCESS;
  }
  return CREATE_ERROR;
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
  if( m_fieldMoveModel[ (u32)fieldMoveModelId ].IsSetupResource() )
  {
    // 動作モデルを描画パイプラインから削除
    m_pMyRenderingPipeLine->RemoveEdgeRenderingTarget( m_fieldMoveModel[ (u32)fieldMoveModelId ].GetCharaDrawInstance()->GetModelInstanceNode() );
    // 動作モデルリソースを破棄
    m_fieldMoveModel[ (u32)fieldMoveModelId ].ClearResource();
  }
  if( m_fieldMoveModel[ (u32)fieldMoveModelId ].IsSetupWork() )
  {
    // 動作モデルをツリーから削除
    m_fieldMoveModelRoot.RemoveChild( &m_fieldMoveModel[ (u32)fieldMoveModelId ] );
    // 動作モデルワークを破棄
    m_fieldMoveModel[ (u32)fieldMoveModelId ].ClearWork();
  }
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
  if( fieldMoveModelId == FIELD_MOVE_MODEL_PLAYER ) { m_fieldMoveModel[ (u32)fieldMoveModelId ].CreateRttiEditorPage( L"プレイヤー"); }
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
  if( fieldMoveModelId == FIELD_MOVE_MODEL_PLAYER ) { m_fieldMoveModel[ (u32)fieldMoveModelId ].DeleteRttiEditorPage(); }
#endif
}

//-----------------------------------------------------------------------------
/**
*  @brief  有効な全動作モデルをイベント状態(アクションコマンド、動作コードを共に動かさない)にする
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::OnEventStateAll(void)
{
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if( m_fieldMoveModel[i].IsSetup() )
    {
      m_fieldMoveModel[i].OnStateBit( Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_EVENT );
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  有効な全動作モデルをイベント状態(アクションコマンド、動作コードを共に動かさない)から解除する
*/
//-----------------------------------------------------------------------------
void FieldMoveModelManager::OffEventStateAll(void)
{
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    if( m_fieldMoveModel[i].IsSetup() )
    {
      m_fieldMoveModel[i].OffStateBit( Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_EVENT );
    }
  }
}

}; //end of namespace MoveModel
}; //end of namespace Field
