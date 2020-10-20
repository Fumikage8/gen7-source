//==============================================================================
/**
* @file	BattleFesModelManager.cpp
* @brief	バトルフェスモデルマネージャー  ※BattleInstから移植
* @author	munakata_kai
* @date	2016/11/26
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
#include "Field/FieldRos/FieldGimmickBattleFes/include/BattleFes/BattleFesModelManager.h"

GFL_NAMESPACE_BEGIN( BattleFes )

// コンストラクタ
BattleFesModelManager::BattleFesModelManager( gfl2::heap::HeapBase* pHeap )
  :m_requestModelIndex( BattleFesDefine::BFES_MODEL_DISP_MAX )
  ,m_deleteModelIndex( BattleFesDefine::BFES_MODEL_DISP_MAX )
{
  for (u32 index(0); index < BattleFesDefine::BFES_MODEL_DISP_MAX; ++index)
  {
    m_pRequestModel[index] = GFL_NEW(pHeap) BattleFesModel(pHeap);
  }
}

// デストラクタ
BattleFesModelManager::~BattleFesModelManager()
{
  for (u32 index(0); index < BattleFesDefine::BFES_MODEL_DISP_MAX; ++index)
  {
    GFL_SAFE_DELETE(m_pRequestModel[index]);
  }
}


//-----------------------------------------------------------------------------
/**
 *  @brief モデルがセットアップ済みか確認
 *  @param modelIndex  モデルのインデックス
 *  @return trueで完了済み
 */
//-----------------------------------------------------------------------------
bool BattleFesModelManager::IsSetupEnd( const int modelIndex ) const
{
  return m_pRequestModel[modelIndex]->IsSetupEnd();
}

//-----------------------------------------------------------------------------
/**
 *  @brief 表示リクエスト
 *  @param p_data  リクエストデータ
 */
//-----------------------------------------------------------------------------
void BattleFesModelManager::Request(MoveModelData* p_data)
{
  // 表示リクエストリストにインデックスを登録
  MUNAKATA_PRINT("m_requestModelIndex :%d \n", p_data->index );
  GFL_ASSERT( p_data->isUsing == false );
  m_pRequestModel[ p_data->index ]->Setup( p_data );
  m_requestModelIndex = p_data->index;
  p_data->isUsing = true;
}

//-----------------------------------------------------------------------------
/**
 *  @brief 表示リクエスト(再読み込み)
 *  @param p_data リクエストデータ
 */
//-----------------------------------------------------------------------------
void BattleFesModelManager::RequestReload(MoveModelData* p_data)
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
bool BattleFesModelManager::IsRequestEnd(const bool isCheckHitPlayer)
{
  if( m_requestModelIndex < BattleFesDefine::BFES_MODEL_DISP_MAX )
  {
    if( m_pRequestModel[ m_requestModelIndex ]->SetupUpdate( isCheckHitPlayer ) == false )
    { // セットアップ未完了があればfalseを返す
      return false;
    }
    else
    {
      // セットアップ完了したのでインデックスは無効値
      m_requestModelIndex = BattleFesDefine::BFES_MODEL_DISP_MAX;
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
void BattleFesModelManager::RequestDelete(const u32 index)
{
  GFL_ASSERT(m_deleteModelIndex == BattleFesDefine::BFES_MODEL_DISP_MAX);
  m_deleteModelIndex = index;
  MUNAKATA_PRINT("Model Delete! m_deleteModelIndex:%d\n", m_deleteModelIndex );
}

//-----------------------------------------------------------------------------
/**
 *  @brief モデル削除の対応処理完了したか
 *  @return trueで破棄完了
 */
//-----------------------------------------------------------------------------
bool BattleFesModelManager::Delete()
{
  // 削除中がいればfalseを返す
  if( m_deleteModelIndex < BattleFesDefine::BFES_MODEL_DISP_MAX )
  {
    if( m_pRequestModel[ m_deleteModelIndex ]->Delete() == false )
    {// １フレ遅れ待ち
      return false;
    }
    else
    {
      // 破棄完了したのでインデックス無効値
      m_deleteModelIndex = BattleFesDefine::BFES_MODEL_DISP_MAX;
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
bool BattleFesModelManager::Terminate()
{
  // 表示処理中ならば一旦完了するまで待つ
  if( this->IsRequestEnd( false ) == false )
  {
    return false;
  }

  // モデル全て削除
  for( u8 i=0; i<BattleFesDefine::BFES_MODEL_DISP_MAX; i++ )
  {
    // 削除中がいればfalseを返す
    if( m_pRequestModel[ i ]->Delete() == false )
    {// １フレ遅れ待ち
      return false;
    }
  }

  return true;
}

GFL_NAMESPACE_END( BattleFes )
