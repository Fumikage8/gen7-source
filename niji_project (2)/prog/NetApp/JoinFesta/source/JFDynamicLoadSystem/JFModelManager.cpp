//==============================================================================
/**
 * @file	JFModelManager.cpp
 * @brief	ジョインフェスタモデルマネージャ
 * @author	ichiraku_katsuhiko
 * @date	2015/05/28 15:59:28
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

// JoinFesta
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFPoint.h"

// mine
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFModelManager.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)

// JFModel1体のヒープサイズ　使用用途はきせかえリソースの読み込み
static const u32 JFMODEL_LOAL_HEAP_SIZE = System::DressUp::FIELD_HEAP_SIZE_MODEL + System::DressUp::FIELD_HEAP_SIZE_ALLOCATOR;

// 衝突判定用アクターのヒープサイズ(FieldPlacementDataManagerと同じ)
static const s32 JFDUMMY_ACTOR_HEAP_SIZE = 0x10000;

// コンストラクタ
JFModelManager::JFModelManager( gfl2::heap::HeapBase* pHeap, JoinFestaEffectSystem* pJoinFestaEffectSystem ) :
    m_pHeap( pHeap )
    ,m_requestJfModelIndex( MODEL_MAX )
    ,m_deleteJfModelIndex( MODEL_MAX )
    ,m_traverseResetCount(0)
{
  for( u8 i=0; i<MODEL_MAX; i++ )
  {
    m_pLocalHeapForJFModel[i] = NULL;
    m_pLocalHeapForDummyActor[i] = NULL;
    m_pRequestJFModel[i] = NULL;
  }

  // memo:着せ替えに大量にメモリを使うので、フィールドで余っているイベント用ヒープを使う
  // FieldExtHeapとFieldDeviceの2つを使う
  gfl2::heap::HeapBase* pFieldEventHeap = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetFieldExtHeap();

  if( pFieldEventHeap != NULL )
  {
    u8 i=0;
    const int fieldExtHeapAllocCount = (MODEL_MAX > 5 ) ? 5 : MODEL_MAX;
    const int fieldDeviceHeapAllocCount = (MODEL_MAX > 5 ) ? MODEL_MAX - 5 : 0;
    ICHI_PRINT(">ICHI fieldExtHeapAllocCount=%d  fieldDeviceHeapAllocCount=%d \n", fieldExtHeapAllocCount, fieldDeviceHeapAllocCount);

    // FieldExtHeapから4つ
    for( ; i<fieldExtHeapAllocCount; i++ )
    {
      m_pLocalHeapForJFModel[i] = GFL_CREATE_LOCAL_HEAP_NAME( pFieldEventHeap, JFMODEL_LOAL_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "JFMODEL_DRESSUP1" );
      m_pLocalHeapForDummyActor[i] = GFL_CREATE_LOCAL_HEAP_NAME( pFieldEventHeap, JFDUMMY_ACTOR_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "JFDUMMYACTOR1" );
    }

    // FieldDeviceHeapから4つ
    for( ; i<(fieldExtHeapAllocCount + fieldDeviceHeapAllocCount); i++ )
    {
      m_pLocalHeapForJFModel[i] = GFL_CREATE_LOCAL_HEAP_NAME( pHeap, JFMODEL_LOAL_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "JFMODEL_DRESSUP2" );
      m_pLocalHeapForDummyActor[i] = GFL_CREATE_LOCAL_HEAP_NAME( pHeap, JFDUMMY_ACTOR_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "JFDUMMYACTOR2" );
    }
  }
  else
  {
    // FieldDeviceHeapから最低2つ
    const int allocCount = (MODEL_MAX < 2 ) ? MODEL_MAX : 2;
    for( int i=0; i<allocCount ; i++ )
    {
      m_pLocalHeapForJFModel[i] = GFL_CREATE_LOCAL_HEAP_NAME( pHeap, JFMODEL_LOAL_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "JFMODEL_DRESSUP3" );
      m_pLocalHeapForDummyActor[i] = GFL_CREATE_LOCAL_HEAP_NAME( pHeap, JFDUMMY_ACTOR_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "JFDUMMYACTOR3" );
    }
  }

  for( u8 i=0; i<MODEL_MAX; i++ )
  {
    if( m_pLocalHeapForJFModel[i] != NULL && m_pLocalHeapForDummyActor[i] != NULL )
    {
      m_pRequestJFModel[i] = GFL_NEW( m_pHeap ) JFModel( m_pLocalHeapForJFModel[i], m_pLocalHeapForDummyActor[i], pJoinFestaEffectSystem );
    }
  }
}

// デストラクタ
JFModelManager::~JFModelManager()
{
  for( u8 i=0; i<MODEL_MAX; i++ )
  {
    GFL_SAFE_DELETE( m_pRequestJFModel[i] );
    GFL_DELETE_HEAP( m_pLocalHeapForJFModel[i] );
    GFL_DELETE_HEAP( m_pLocalHeapForDummyActor[i] );
  }
}


//-----------------------------------------------------------------------------
/**
 *  @brief 表示リクエスト
 *  @param [in]pJFPersonalData  ジョインフェスタパーソナルデータ
 *  @param [in]jfPointIndex     JFPointのインデックス値
 *  @param [in]pos  表示位置
 */
//-----------------------------------------------------------------------------
void JFModelManager::Request( NetAppLib::JoinFesta::JoinFestaPersonalData* pJFPersonalData, const u32 jfPointIndex, const gfl2::math::Vector3 pos )
{
  // JFモデルの作成
  for( u8 i=0; i<MODEL_MAX; i++ )
  {
    // 非表示状態のモデルを探す
    if( m_pRequestJFModel[ i ] && m_pRequestJFModel[ i ]->GetJFPointIndex() == JFPoint::INDEX_NULL )
    {
      m_pRequestJFModel[ i ]->Setup( pJFPersonalData, jfPointIndex, pos );

      // 表示リクエストリストにインデックスを登録
      GFL_ASSERT( m_requestJfModelIndex == MODEL_MAX );
      m_requestJfModelIndex = i;

      ICHI_PRINT("JFModel Request Success! i:%d JFPointIndex:%d\n", i, jfPointIndex );
      return;
    }
  }

  ICHI_PRINT("JFModel Request Failue! JFPointIndex:%d\n", jfPointIndex );
  GFL_ASSERT(0); // 来ないはず
}

//-----------------------------------------------------------------------------
/**
 *  @brief 表示リクエストの対応処理完了したか
 *  @param   JfModelSetupMode    セットアップモード
 *  @return trueで完了
 */
//-----------------------------------------------------------------------------
bool JFModelManager::IsRequestEnd( const JfModelSetupMode setupMode )
{
  if( m_requestJfModelIndex < MODEL_MAX )
  {
    if( m_pRequestJFModel[ m_requestJfModelIndex ] &&
        m_pRequestJFModel[ m_requestJfModelIndex ]->IsSetup( setupMode ) == false )
    { // セットアップ未完了があればfalseを返す
      return false;
    }
    else
    {
      // セットアップ完了したのでインデックスは無効値
      m_requestJfModelIndex = MODEL_MAX;
    }
  }
  return true;
}


//-----------------------------------------------------------------------------
/**
 *  @brief モデル削除
 *  @param [in]jfPointIndex     JFPointのインデックス値
 *  @note  IsDeleteEndで削除完了確認する
 */
//-----------------------------------------------------------------------------
void JFModelManager::Delete( const u32 jfPointIndex )
{
  GFL_ASSERT( m_deleteJfModelIndex == MODEL_MAX );

  // 検索
  for( u8 i=0; i<MODEL_MAX; i++ )
  {
    // 非表示状態のモデルを探す
    if( m_pRequestJFModel[ i ] &&
        m_pRequestJFModel[ i ]->GetJFPointIndex() == jfPointIndex )
    {
      m_deleteJfModelIndex = i;
      ICHI_PRINT("JFModel Delete! i:%d JFPointIndex:%d\n", i, jfPointIndex );
      return;
    }
  }

  ICHI_PRINT("JFModel Delete Error! JFPointIndex:%d\n", jfPointIndex );
  GFL_ASSERT(0); // 来ないはず
}

//-----------------------------------------------------------------------------
/**
 *  @brief モデル削除の対応処理完了したか
 *  @return trueで破棄完了
 */
//-----------------------------------------------------------------------------
bool JFModelManager::IsDeleteEnd()
{
  // 削除中がいればfalseを返す
  if( m_deleteJfModelIndex < MODEL_MAX )
  {
    if( m_pRequestJFModel[ m_deleteJfModelIndex ] &&
        m_pRequestJFModel[ m_deleteJfModelIndex ]->Delete() == false )
    {// １フレ遅れ待ち
      return false;
    }
    else
    {
      // 破棄完了したのでインデックス無効値
      m_deleteJfModelIndex = MODEL_MAX;
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
bool JFModelManager::Terminate()
{
  // 表示処理中ならば一旦完了するまで待つ
  if( this->IsRequestEnd( JF_MODEL_SETUP_END ) == false )
  {
    return false;
  }

  // モデル全て削除
  for( u8 i=0; i<MODEL_MAX; i++ )
  {
    // 削除中がいればfalseを返す
    if( m_pRequestJFModel[ i ] && m_pRequestJFModel[ i ]->Delete() == false )
    {// １フレ遅れ待ち
      return false;
    }
  }

  return true;
}

//------------------------------------------------------------------
/**
 *  @brief   トラバース直前の更新（処理負荷対策用）
 */
//------------------------------------------------------------------
void JFModelManager::TraverseBefore()
{
  // モデル全て
  for( u8 i=0; i<MODEL_MAX; i++ )
  {
    // トラバース更新チェックする必要があるか
    if( m_pRequestJFModel[ i ]->IsTraverseCheckNeed() == false )
    {
      continue;
    }

    bool isTraverseUpdate = false;

    // X回に1回処理するようにする
    // + 
    // 強制更新チェック
    if( (i % TRAVERSE_UPDATE_FRAME) == m_traverseResetCount ||
        m_pRequestJFModel[ i ]->IsForceTraverseUpdate() )
    {
      // トラバース処理させるようにする
      isTraverseUpdate = true;
    }

    m_pRequestJFModel[ i ]->SetTraverseUpdate( isTraverseUpdate );
  }

  // トラバース更新チェック用フレーム
  m_traverseResetCount++;
  if( m_traverseResetCount >= TRAVERSE_UPDATE_FRAME )
  {
    m_traverseResetCount = 0;
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief   JFパーソナルデータ取得
*  @param   eventId 動作モデルのイベントID （いない場合はNULL）
*/
//-----------------------------------------------------------------------------
NetAppLib::JoinFesta::JoinFestaPersonalData* JFModelManager::GetJoinFestaPersonalData( u32 eventId )
{
  JFModel* pJFModel = GetJFModel( eventId );

  if( pJFModel )
  {
    return pJFModel->GetJoinFestaPersonalData();
  }

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief   World座標の取得
 *  @param   eventId 動作モデルのイベントID
 */
//-----------------------------------------------------------------------------
const gfl2::math::Vector3* JFModelManager::GetWorldPosition( u32 eventId )
{
  JFModel* pJFModel = GetJFModel( eventId );

  if( pJFModel )
  {
    return pJFModel->GetWorldPosition();
  }
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief   WorldMatrixの取得
 *  @param   eventId 動作モデルのイベントID
 */
//-----------------------------------------------------------------------------
const gfl2::math::Matrix34* JFModelManager::GetWorldMatrix( u32 eventId )
{
  JFModel* pJFModel = GetJFModel( eventId );

  if( pJFModel )
  {
    return pJFModel->GetWorldMatrix();
  }
  return NULL;
}

//=============================================================================
/**
 *  privateメソッド
 */
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *  @brief   JFモデル取得
 *  @param   eventId 動作モデルのイベントID
 */
//-----------------------------------------------------------------------------
JFModel* JFModelManager::GetJFModel( u32 eventId )
{
  for( u8 i=0; i<MODEL_MAX; i++ )
  {
    // JF動作モデルのイベントID = ポイントインデックス + オフセット
    if( m_pRequestJFModel[ i ] )
    {
      u32 jfMoveModelEventId = m_pRequestJFModel[ i ]->GetJFPointIndex() + JoinFestaDefine::MODEL_EVENT_ID_OFFSET;
      if( jfMoveModelEventId == eventId  )
      {
        return m_pRequestJFModel[ i ];
      }
    }
  }

  return NULL;
}

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

