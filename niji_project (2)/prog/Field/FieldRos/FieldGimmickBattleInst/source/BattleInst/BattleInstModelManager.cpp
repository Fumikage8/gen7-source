//==============================================================================
/**
 * @file	BattleInstModelManager.cpp
 * @brief	バトル施設モデルマネージャー
 * @author	onoeu_masayuki
 * @date	2016/01/27
 */
// =============================================================================

#include <util/include/gfl2_std_string.h>
#include <base/include/gfl2_Singleton.h>
// system
#include "System/include/GflUse.h"
#include "System/include/DressUp.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// フィールドマップ
#include "Field/FieldRo/include/Fieldmap.h"

// mine
#include "Field/FieldRos/FieldGimmickBattleInst/include/BattleInst/BattleInstModelManager.h"

#include "System/include/HeapSize.h"

GFL_NAMESPACE_BEGIN(BattleInst)

// 1体のヒープサイズ　使用用途はきせかえリソースの読み込み
static const u32 BINST_MODEL_HEAP_SIZE = System::DressUp::FIELD_HEAP_SIZE_MODEL + System::DressUp::FIELD_HEAP_SIZE_ALLOCATOR;

// コンストラクタ
BattleInstModelManager::BattleInstModelManager( gfl2::heap::HeapBase* pHeap,u32 useModelCount ) :
    m_pHeap( pHeap )
    ,m_requestModelIndex( useModelCount )
    ,m_deleteModelIndex( useModelCount )
    ,m_useModelCount( useModelCount )
{
  {
    gfl2::heap::HeapBase* pFieldEventHeap = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetFieldExtHeap();
    gfl2::heap::HeapBase* pSubHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FIELD_DEVICE );
    const int maxFieldExtHeapAllocCount = 2; // 拡張ヒープから確保できる最大数
    const int fieldExtHeapAllocCount = m_useModelCount < maxFieldExtHeapAllocCount ? m_useModelCount : maxFieldExtHeapAllocCount; // 実際に拡張ヒープから確保する数

    {
      u8 i = 0;
      for( ; i<fieldExtHeapAllocCount; i++ )
      {
        m_pLocalHeapForModel[i] = GFL_CREATE_LOCAL_HEAP_NAME( pFieldEventHeap, BINST_MODEL_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "BATTLE_INST_MODEL_DRESSUP1" );
        ONOUE_PRINT("バトル施設 モデルアロケータ %d:拡張ヒープから確保\n",i);
      }

      for( ; i<m_useModelCount; i++ )
      {
        m_pLocalHeapForModel[i] = GFL_CREATE_LOCAL_HEAP_NAME( pSubHeap, BINST_MODEL_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "BATTLE_INST_MODEL_DRESSUP1" );
        ONOUE_PRINT("バトル施設 モデルアロケータ %d:サブヒープから確保\n",i);
      }
    }

    for( u8 i=0; i<m_useModelCount; i++ )
    {
      m_pRequestModel[i] = GFL_NEW( pFieldEventHeap ) BattleInstModel( m_pLocalHeapForModel[i] );
    }
  }
}

// デストラクタ
BattleInstModelManager::~BattleInstModelManager()
{
  for( u8 i=0; i<m_useModelCount; i++ )
  {
    GFL_SAFE_DELETE( m_pRequestModel[i] );
    GFL_DELETE_HEAP( m_pLocalHeapForModel[i] );
  }
}


//-----------------------------------------------------------------------------
/**
 *  @brief モデルがセットアップ済みか確認
 *  @param modelIndex  モデルのインデックス
 *  @return trueで完了済み
 */
//-----------------------------------------------------------------------------
bool BattleInstModelManager::IsSetupEnd( int modelIndex )
{
  return m_pRequestModel[modelIndex]->IsSetupEnd();
}

//-----------------------------------------------------------------------------
/**
 *  @brief 表示リクエスト
 *  @param p_data  リクエストデータ
 */
//-----------------------------------------------------------------------------
void BattleInstModelManager::Request( BattleInst::MoveModelData* p_data )
{
  // 表示リクエストリストにインデックスを登録
  ONOUE_PRINT("m_requestModelIndex :%d \n", p_data->index );
  GFL_ASSERT( p_data->isUsing == false );
  m_pRequestModel[ p_data->index ]->Setup( p_data );
  m_requestModelIndex = p_data->index;
  p_data->isUsing = true;
  return;
}

//-----------------------------------------------------------------------------
/**
 *  @brief 表示リクエスト(再読み込み)
 *  @param p_data リクエストデータ
 */
//-----------------------------------------------------------------------------
void BattleInstModelManager::RequestReload( BattleInst::MoveModelData* p_data )
{
  GFL_ASSERT( p_data->isUsing == true );
  p_data->isUsing = false;
  Request(p_data);
}

//-----------------------------------------------------------------------------
/**
 *  @brief 表示リクエストの対応処理完了したか
 *  @param  isCheckHitPlayer    主人公との衝突判定をするかどうか(trueでする）
 *  @return trueで完了
 */
//-----------------------------------------------------------------------------
bool BattleInstModelManager::IsRequestEnd( const bool isCheckHitPlayer )
{
  if( m_requestModelIndex < m_useModelCount )
  {
    if( m_pRequestModel[ m_requestModelIndex ]->SetupUpdate( isCheckHitPlayer ) == false )
    { // セットアップ未完了があればfalseを返す
      return false;
    }
    else
    {
      // セットアップ完了したのでインデックスは無効値
      m_requestModelIndex = m_useModelCount;
    }
  }
  return true;
}


//-----------------------------------------------------------------------------
/**
 *  @brief モデル削除
 *  @note  IsDeleteEndで削除完了確認する
 */
//-----------------------------------------------------------------------------
void BattleInstModelManager::Delete( const u32 index )
{
  m_deleteModelIndex = index;
  ONOUE_PRINT("Model Delete! m_deleteModelIndex:%d\n", m_deleteModelIndex );
}

//-----------------------------------------------------------------------------
/**
 *  @brief モデル削除の対応処理完了したか
 *  @return trueで破棄完了
 */
//-----------------------------------------------------------------------------
bool BattleInstModelManager::IsDeleteEnd()
{
  // 削除中がいればfalseを返す
  if( m_deleteModelIndex < m_useModelCount )
  {
    if( m_pRequestModel[ m_deleteModelIndex ]->Delete() == false )
    {// １フレ遅れ待ち
      return false;
    }
    else
    {
      // 破棄完了したのでインデックス無効値
      m_deleteModelIndex = m_useModelCount;
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
/**
 *  @brief 破棄
 *  @retval true:破棄完了
 *  @retval false:破棄中
 */
//-----------------------------------------------------------------------------
bool BattleInstModelManager::Terminate()
{
  // 表示処理中ならば一旦完了するまで待つ
  if( this->IsRequestEnd( false ) == false )
  {
    return false;
  }

  // モデル全て削除
  for( u8 i=0; i<m_useModelCount; i++ )
  {
    // 削除中がいればfalseを返す
    if( m_pRequestModel[ i ]->Delete() == false )
    {// １フレ遅れ待ち
      return false;
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
/**
*  @brief   モデルデータを更新する
*/
//-----------------------------------------------------------------------------
void BattleInstModelManager::UpdateModelData( BattleInst::MoveModelData* p_data )
{
  m_pRequestModel[p_data->index]->UpdateModelData(p_data);
}

//=============================================================================
/**
 *  privateメソッド
 */
//=============================================================================

GFL_NAMESPACE_END(BattleInst)

