//===================================================================
/**
 * @file    StaticModelManager.cpp
 * @brief   配置モデルの管理クラス
 * @author  saita_kazuki
 * @date    2015.04.08
 */
//===================================================================

#include "Field/FieldRo/include/StaticModel/StaticModelManager.h"
#include "Field/FieldRo/include/StaticModel/StaticModelResourceNodeManager.h"
#include "Field/FieldRo/include/StaticModel/StaticModel.h"
#include "./StaticModelPackAccessor.h"
#include "./StaticModelSEPlayer.h"

#include "Field/FieldRo/include/FieldRootNode.h" // ノードの登録のため

#include "System/include/nijiAllocator.h"

#include <Fs/include/gfl2_BinLinkerAccessor.h> // パックへのアクセスのため
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_std_string.h>

#include "Field/FieldRo/include/Collision/CollisionSceneDefault.h" // バウンディングボックスアニメーションで使用
#include "Field/FieldRo/include/Collision/DynamicActor.h"

// デバッグメニュー
#include "GameSys/include/GameManager.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Debug/FieldDebugMenu.h" 

#include <fs/include/gfl2_FsAsyncFileManager.h>

// arc
#include <arc_def_index/arc_def.h>

// conv_header
#include <niji_conv_header/field/static_model/static_model_id.h>
#include <niji_conv_header/field/zone/zone_id.h>

// debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include <fs/include/gfl2_FsPcUtil.h>

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( StaticModel )

/**
 * @brief 配置モデルマネージャー生成
 */
StaticModelManager* StaticModelManager::CreateStaticModelManager( gfl2::gfx::IGLAllocator* pAllocator )
{
  StaticModelManager* pManager = GLNew( pAllocator) StaticModelManager();
  return pManager;
};

/**
 * @brief   コンストラクタ
 */
StaticModelManager::StaticModelManager() :
  m_ppStaticModelArray( NULL),
  m_pHeap( NULL),
  m_pResourcePackBinLinker( NULL),
  m_pAllocator( NULL),
  m_pFieldRootNode( NULL),
  m_pSEPlayer( NULL),
  m_pCollisionSceneForBBAnimation( NULL),
  m_pLocalHeapForStatic( NULL),
  m_pModelAllocatorForStatic( NULL),
  m_ppStaticModelDynamicArray( NULL),
  m_dynamicModelCreateNum( 0),
  m_isUseCulling( true),
  m_pResourceNodeManager( NULL),
  m_playerOldPos( gfl2::math::Vector3( -1000.0f, -1000.0f, -1000.0f))

#if PM_DEBUG
  , m_pDebugModelName( NULL)
  , m_pDebuggerHeap( NULL)
  , m_pAsyncFileManager( NULL)
#endif // PM_DEBUG
{
  gfl2::std::MemClear( m_pLocalHeapForDynamic, sizeof( m_pLocalHeapForDynamic));
  gfl2::std::MemClear( m_pModelAllocatorForDynamic, sizeof( m_pModelAllocatorForDynamic));

#if PM_DEBUG
  gfl2::std::MemClear( m_pDebugModelPackDataBuffer, sizeof( m_pDebugModelPackDataBuffer));
#endif // PM_DEBUG
}

/**
 * @brief   デストラクタ
 */
StaticModelManager::~StaticModelManager()
{
}

/**
 * @brief ファイル読み込み管理
 */
void StaticModelManager::StartFileRead( void )
{
}

/**
 * @brief ファイル読み込み待ち
 */
bool StaticModelManager::WaitFileRead( void )
{
  return true;
}

/**
 * @brief 使用可能状態にセットアップ
 */
bool StaticModelManager::Setup( void )
{
  return true;
}

/**
 * @brief 破棄
 */
bool StaticModelManager::Finalize( void )
{
  if( m_pResourceNodeManager)
  {
    m_pResourceNodeManager->Terminate();
    GFL_DELETE( m_pResourceNodeManager);
    m_pResourceNodeManager = NULL;
  }

  for( s32 i = 0; i < STATIC_MODEL_MAX; ++i)
  {
    m_pFieldRootNode->RemoveInstanceChild( m_ppStaticModelArray[i]);
    if( m_ppStaticModelArray[i]->IsEmpty() == false)
    {
      m_ppStaticModelArray[i]->Terminate();
    }

    GFL_DELETE( m_ppStaticModelArray[i]);
  }
  GFL_DELETE( m_pModelAllocatorForStatic);

  for( s32 i = 0; i < STATIC_MODEL_DYNAMIC_MAX; ++i)
  {
    m_pFieldRootNode->RemoveInstanceChild( m_ppStaticModelDynamicArray[i]);
    if( m_ppStaticModelDynamicArray[i]->IsEmpty() == false)
    {
      m_ppStaticModelDynamicArray[i]->Terminate();
    }

    GFL_DELETE( m_pModelAllocatorForDynamic[i]);
    GFL_DELETE( m_ppStaticModelDynamicArray[i]);
  }

  GFL_SAFE_DELETE( m_pCollisionSceneForBBAnimation);

  GFL_DELETE_ARRAY( m_ppStaticModelArray);
  GFL_DELETE_ARRAY( m_ppStaticModelDynamicArray);

  m_pSEPlayer->Terminate();
  GFL_DELETE( m_pSEPlayer);

  GFL_DELETE( m_pAllocator);

  this->DeleteHeap();

#if PM_DEBUG
  this->TerminateDebugModelPackDataBuffer();
  // PCデバッガーヒープがあればデバッグモデルgarcをクローズする
  if( m_pDebuggerHeap)
  {
    this->SyncArcClose( m_pDebuggerHeap, m_pAsyncFileManager, ARCID_STATIC_MODEL_D);
  }
  m_pAsyncFileManager = NULL;
  m_pDebuggerHeap     = NULL;
#endif // PM_DEBUG

  return true;
}

/**
 * @brief 破棄の必要があるか。
 */
bool StaticModelManager::IsExists( void ) const
{
  return true;
}

/**
 * @brief 初回初期化
 * @param initData モデル生成などに必要なデータが格納されたアクセサ
 */
void StaticModelManager::InitializeFirst( StaticModelManager::InitializeDataAccessor& initDataAccessor)
{
  m_pResourcePackBinLinker  = initDataAccessor.pResourcePackBinLinker;
  m_pFieldRootNode          = initDataAccessor.pFieldRootNode;

  this->CreateHeap( initDataAccessor.pHeap);

  m_pAllocator = GFL_NEW( m_pHeap) System::nijiAllocator( m_pHeap);

  m_pSEPlayer = GFL_NEW( m_pHeap) StaticModelSEPlayer();
  m_pSEPlayer->Initialize( initDataAccessor.pSETableData);

  // モデルが使用するアロケーターを作成
  m_pModelAllocatorForStatic =  GFL_NEW( m_pHeap) System::nijiAllocator( m_pLocalHeapForStatic);
  for( s32 i = 0; i < STATIC_MODEL_DYNAMIC_MAX; ++i)
  {
    m_pModelAllocatorForDynamic[i] =  GFL_NEW( m_pHeap) System::nijiAllocator( m_pLocalHeapForDynamic[i]);
  }

  // 配置モデルワークを作成
  m_ppStaticModelArray = GLNew( m_pAllocator) StaticModel* [ STATIC_MODEL_MAX ];
  for( s32 i = 0; i < STATIC_MODEL_MAX; ++i)
  {
    m_ppStaticModelArray[i] = GLNew( m_pAllocator) StaticModel();
    m_pFieldRootNode->AddInstanceChild( m_ppStaticModelArray[i]);
  }

  // 動的配置モデルワークを作成
  m_ppStaticModelDynamicArray = GLNew( m_pAllocator) StaticModel* [ STATIC_MODEL_DYNAMIC_MAX ];
  for( s32 i = 0; i < STATIC_MODEL_DYNAMIC_MAX; ++i)
  {
    m_ppStaticModelDynamicArray[i] = GLNew( m_pAllocator) StaticModel();
    m_pFieldRootNode->AddInstanceChild( m_ppStaticModelDynamicArray[i]);
  }

  // コリジョンシーン生成
  m_pCollisionSceneForBBAnimation = GFL_NEW( m_pHeap) CollisionSceneDefault;
  m_pCollisionSceneForBBAnimation->Setup( m_pAllocator, m_pHeap);

  // リソースノード管理クラス作成
  m_pResourceNodeManager = GFL_NEW( m_pHeap) StaticModelResourceNodeManager();
  StaticModelResourceNodeManager::InitializeDescription desc;
  desc.pHeap            = m_pLocalHeapForStatic;
  desc.pAllocator       = m_pModelAllocatorForStatic;
  desc.pFieldRootNode   = m_pFieldRootNode;
  desc.areaResourceNum  = m_pResourcePackBinLinker->GetDataMax();
  m_pResourceNodeManager->Initialize( desc);

#if defined( DEBUG_ONLY_FOR_saita_kazuki )
  m_pHeap->PrintHeapInfo( "StaticModelManager");
#endif

#if PM_DEBUG
  // PCデバッガーヒープがあればデバッグモデルgarcをオープンしておく
  if( gfl2::heap::Manager::IsExistHeapId( HEAPID_DEBUG_DEBUGGER))
  {
    m_pDebuggerHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_DEBUGGER);
  }
  else
  {
    m_pDebuggerHeap = NULL;
  }
  m_pAsyncFileManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager)->GetAsyncFileManager();
  if( m_pDebuggerHeap)
  {
    this->SyncArcOpen( m_pDebuggerHeap, m_pAsyncFileManager, ARCID_STATIC_MODEL_D);
  }
#endif // PM_DEBUG
}

/**
 * @brief Readyで1度だけ呼び出す
 * @param pDynamicActor 当たる側のアクター
 */
void StaticModelManager::Ready( DynamicActor* pDynamicActor)
{
  // バウンディングボックスアニメーションのコリジョンを更新
  this->UpdateBBAnimationCollision( pDynamicActor);

  for( u32 i = 0; i < STATIC_MODEL_MAX; ++i)
  {
    if( m_ppStaticModelArray[i]->IsEmpty() == false)
    {
      m_ppStaticModelArray[i]->Ready();
    }
  }
  for( u32 i = 0; i < STATIC_MODEL_DYNAMIC_MAX; ++i)
  {
    if( m_ppStaticModelDynamicArray[i]->IsEmpty() == false)
    {
      m_ppStaticModelDynamicArray[i]->Ready();
    }
  }

#if PM_DEBUG
  this->WriteTextStaticModel();
#endif // PM_DEBUG
}

/**
 * @brief バウンディングボックスアニメーションコリジョン更新
 */
void StaticModelManager::UpdateBBAnimationCollision( DynamicActor* pDynamicActor)
{
  // アクター不在時は更新しない。
  if(!pDynamicActor){ return ; }

  // 座標の更新が無ければ実効しない。
  gfl2::math::Vector3 playerNewPos = pDynamicActor->GetPosition();
  if( (playerNewPos - m_playerOldPos).Length() <= FLT_EPSILON)
  {
    return;
  }
  m_playerOldPos = playerNewPos;

  // コリジョンシーン更新
  m_pCollisionSceneForBBAnimation->Update();

  StaticModel::BBAnimationCollisionState state;
  state.isHit = false;

  // 衝突状態のクリア
  for( s32 i = 0; i < STATIC_MODEL_MAX; ++i)
  {
    if( m_ppStaticModelArray[i]->IsEmpty() == false)
    {
      m_ppStaticModelArray[i]->NotificationBBAnimationCollisionState( state);
    }
  }

  for( s32 i = 0; i < BaseActor::MAX_COLLISION_RESULT_CNT; ++i)
  {
    // 衝突したアクターを取得する。NULLなら衝突していない
    BaseActor* pCollisionActor = pDynamicActor->GetCollisionResult( i)->pTargetActor;
    if( pCollisionActor == NULL)
    {
      break;
    }

    // モデルを取得
    void* pUserData = pCollisionActor->GetUserData();
    StaticModel* pHitModel = reinterpret_cast<StaticModel*>( pUserData);

    state.isHit = true;
    pHitModel->NotificationBBAnimationCollisionState( state);
  }
}

/**
 * @brief 更新処理
 */
void StaticModelManager::Update()
{
  for( s32 i = 0; i < STATIC_MODEL_MAX; ++i)
  {
    m_ppStaticModelArray[i]->Update();
  }
  for( u32 i = 0; i < STATIC_MODEL_DYNAMIC_MAX; ++i)
  {
    m_ppStaticModelDynamicArray[i]->Update();
  }
}

/**
*  @brief  カリング
*  @param  ビュープロジェクション行列
*/
void StaticModelManager::UpdateCulling(gfl2::math::Matrix44 viewProj)
{
#if PM_DEBUG
  // デバッグメニューから
  Debug::FieldDebugMenu* pFieldDebugMenu = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetFieldDebugMenu();
  if( pFieldDebugMenu )
  {
    m_isUseCulling = pFieldDebugMenu->isUseCullingForStaticModel();
  }
#endif

  // カリングをする
  if( !m_isUseCulling )
  {
    return;
  }

  // カリング用行列の作成
  gfl2::math::Matrix44 cullingMatrixTable[4];
  gfl2::math::AABB::Util::CalcFrustumPlanes( cullingMatrixTable, viewProj );

  for( u32 i=0 ; i<STATIC_MODEL_MAX ; i++ )
  {
    u32 nowModelType = m_ppStaticModelArray[i]->GetNowModelType();
    if( nowModelType == MODEL_TYPE_NONE)
    {
      continue;
    }

    poke_3d::model::BaseModel* pBaseModel = m_ppStaticModelArray[i]->GetBaseModel( nowModelType );

    if( !m_ppStaticModelArray[i]->IsEmpty() 
      && pBaseModel
      && m_ppStaticModelArray[i]->IsUpdateCullingEnable()
      )
    {
      // カリング設定がNONEならカリングを行わない
      u8 cullingType = m_ppStaticModelArray[i]->GetCullingType();
      if( cullingType == CULLING_TYPE_NONE)
      {
        continue;
      }

      // モデルのサイズやアニメーションを含め十分な大きさのAABBを作成する
      const gfl2::math::Vector3 HALF_SIZE( 300.0f,300.0f,300.0f );
      const gfl2::math::Vector3 maxOffset( m_ppStaticModelArray[i]->GetLocalSRT().translate + HALF_SIZE );
      const gfl2::math::Vector3 minOffset( m_ppStaticModelArray[i]->GetLocalSRT().translate - HALF_SIZE );
      gfl2::math::AABB          aabb     ( minOffset,maxOffset );

      // 原点から大きく外れて作られている物はモデルのAABBを利用する
      if( pBaseModel->GetModelInstanceNode() )
      {
        if( cullingType == CULLING_TYPE_AABB)
        {
          aabb = pBaseModel->GetModelInstanceNode()->GetAABB();
          aabb.SetMin( aabb.GetMin() + m_ppStaticModelArray[i]->GetLocalSRT().translate );
          aabb.SetMax( aabb.GetMax() + m_ppStaticModelArray[i]->GetLocalSRT().translate );
        }
      }

      // ＡＡＢＢを使って画面外にいるかどうか調べる
      if ( !gfl2::math::AABB::Util::IsIntersectFrustum( cullingMatrixTable, aabb ) )
      {
        pBaseModel->SetVisible(false);
        pBaseModel->SetAnimationCalculateEnable(false);
      }
      else
      {
        // @fix GFNMCat[4973] アニメーション計算フラグの有効後にアニメーション更新をするので前回のフラグを保存しておく
        bool isPrevAnimationCalculateEnable = pBaseModel->IsAnimationCalculateEnable();

        // StaticModel::Update内部で描画オンオフは常に設定されているのでコメントアウト
        // pBaseModel->SetVisible(true);
        pBaseModel->SetAnimationCalculateEnable(true);

        // @fix GFNMCat[4973] 前回アニメーション計算が無効だった場合はアニメーション更新をかける
        if( isPrevAnimationCalculateEnable  == false )
        {
          // ステップフレームを0にし、現在のフレームから進まないようにして、現在のフレームでアニメーションを更新する
          // トラバースはこの後まとめてかかるので、ここでは不要
          f32 prevStepFrame = pBaseModel->GetAnimationStepFrame();
          pBaseModel->SetAnimationStepFrame( 0.0f );
          pBaseModel->UpdateAnimation();
          pBaseModel->SetAnimationStepFrame( prevStepFrame );
        }
      }
    }
  }
}

/**
 * @brief 配置モデルの取得
 * @param index 配置モデルの番号
 * @return 配置モデルのポインタ。indexがオーバーしていたらNULL
 */
StaticModel* StaticModelManager::GetStaticModel( u32 index) const
{
  if( index < STATIC_MODEL_MAX)
  {
    if( m_ppStaticModelArray[index])
    {
      return m_ppStaticModelArray[index];
    }
  }
  else if( index < STATIC_MODEL_DYNAMIC_OFFSET + STATIC_MODEL_DYNAMIC_MAX)
  {
      return m_ppStaticModelDynamicArray[index- STATIC_MODEL_DYNAMIC_OFFSET];
  }
  return NULL;
}

/**
 * @brief イベントIDで配置モデルの取得
 * @param eventID イベントID
 * @return 配置モデルのポインタ。eventIDが無ければNULL
 */
StaticModel* StaticModelManager::GetEventIDStaticModel( u32 eventID ) const
{
  for( s32 i = 0; i < STATIC_MODEL_MAX; ++i)
  {
    if( m_ppStaticModelArray[i]->IsEmpty() == false)
    {
      if( m_ppStaticModelArray[i]->GetEventID() == eventID )
      {
        return m_ppStaticModelArray[i];
      }
    }
  }
  for( u32 i = 0; i < STATIC_MODEL_DYNAMIC_MAX; ++i)
  {
    if( m_ppStaticModelDynamicArray[i]->IsEmpty() == false)
    {
      if( m_ppStaticModelDynamicArray[i]->GetEventID() == eventID )
      {
        return m_ppStaticModelDynamicArray[i];
      }
    }
  }

  return NULL;
}

/**
 * @brief 配置モデルIDで配置モデルの取得
 * @param staticModelID 配置モデルID
 * @return 配置モデルのポインタ。見つからなければNULL
 */
StaticModel* StaticModelManager::GetStaticModelIDStaticModel( u32 staticModelID) const
{
  for( s32 i = 0; i < STATIC_MODEL_MAX; ++i)
  {
    if( m_ppStaticModelArray[i]->IsEmpty() == false)
    {
      if( m_ppStaticModelArray[i]->GetStaticModelID() == staticModelID )
      {
        return m_ppStaticModelArray[i];
      }
    }
  }
  for( u32 i = 0; i < STATIC_MODEL_DYNAMIC_MAX; ++i)
  {
    if( m_ppStaticModelDynamicArray[i]->IsEmpty() == false)
    {
      if( m_ppStaticModelDynamicArray[i]->GetStaticModelID() == staticModelID )
      {
        return m_ppStaticModelDynamicArray[i];
      }
    }
  }

  return NULL;
}

/**
 * @brief インスタンスの遅延破棄リクエスト発行
 */
void StaticModelManager::RequestDelayTerminate()
{
  for( u8 i = 0; i < STATIC_MODEL_MAX; ++i)
  {
    if( m_ppStaticModelArray[i]->IsEmpty() == false)
    {
      m_ppStaticModelArray[i]->RequestDelayTerminate( MODEL_TYPE_ALL);
    }
  }
}

/**
 * @brief 配置モデルの生成
 * @param modelID 配置モデルID
 * @param eventID イベントID
 * @param pos 初期位置
 * @param qt 向き
 * @return 生成した配置モデルのポインタ
 */
StaticModel* StaticModelManager::CreateModel( u32 modelID, u32 eventID, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& qt)
{
  void* pModelData = this->GetStaticModelIDToResourcePackDataEx( modelID);
  return this->CreateModelCore( m_pLocalHeapForStatic, m_pModelAllocatorForStatic, pModelData, modelID, eventID, pos, qt, false);
}

/**
 * @brief 配置モデルの生成
 * @param modelID 配置モデルID
 * @param eventID イベントID
 * @param pos 初期位置
 * @param qt 向き
 * @return 生成した配置モデルのポインタ
 */
StaticModel* StaticModelManager::CreateModelDynamic( u32 modelID, u32 eventID, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& qt)
{
  void* pModelData = this->GetStaticModelIDToResourcePackDataEx( modelID);
  return this->CreateModelDynamicCore( pModelData, modelID, eventID, pos, qt);
}

/**
 * @brief バウンディングボックス用コリジョンシーンの取得
 * @return バウンディングボックス用コリジョンシーン
 */
CollisionSceneDefault* StaticModelManager::GetCollisionSceneForBBAnimation() const
{
  return m_pCollisionSceneForBBAnimation;
}

/**
 * @brief 配置モデルから管理クラスにTerminate処理をしたことを通知する関数
 * @param pStaticModel 配置モデルのポインタ
 */
void StaticModelManager::TerminateNotification( StaticModel* pStaticModel)
{
  for( s32 i = 0; i < STATIC_MODEL_DYNAMIC_MAX; ++i)
  {
    if( m_ppStaticModelDynamicArray[i]->IsEmpty() == false && m_ppStaticModelDynamicArray[i] == pStaticModel)
    {
      m_dynamicModelCreateNum--;
      break;
    }
  }
}

#if PM_DEBUG

/**
 * @brief 配置モデルの生成
 * @param pHeap 配置モデル
 * @param pAllocator 配置モデルが使用するヒープアロケーター
 * @param modelID 配置モデルID
 * @param pModelData 配置モデルパックバイナリデータ
 * @param pos 初期位置
 * @param qt 向き
 * @return 生成した配置モデルのポインタ
 */
StaticModel* StaticModelManager::CreateModelDebug( gfl2::heap::HeapBase* pHeap, System::nijiAllocator* pAllocator, void* pModelData, u32 modelID, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& qt)
{
  return this->CreateModelCore( pHeap, pAllocator, pModelData, modelID, 0xFFFFFFFF, pos, qt, false);
}

/**
 * @brief 配置モデルの生成
 * @param modelID 配置モデルID
 * @param pModelData 配置モデルパックバイナリデータ
 * @param pos 初期位置
 * @param qt 向き
 * @return 生成した配置モデルのポインタ
 */
StaticModel* StaticModelManager::CreateModelDebug( void* pModelData, u32 modelID, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& qt)
{
  return this->CreateModelCore( m_pLocalHeapForStatic, m_pModelAllocatorForStatic, pModelData, modelID, 0xFFFFFFFF, pos, qt, false);
}

/**
 * @brief 配置モデルの生成
 * @param modelID 配置モデルID
 * @param pModelData 配置モデルパックバイナリデータ
 * @param pos 初期位置
 * @param qt 向き
 * @return 生成した配置モデルのポインタ
 */
StaticModel* StaticModelManager::CreateModelDynamicDebug( void* pModelData, u32 modelID, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& qt)
{
  return this->CreateModelDynamicCore( pModelData, modelID, 0xFFFFFFFF, pos, qt);
}


//
// @fix CONNMCat[582]:攻略本用マップ撮影機能、ローカルアニメを特定のフレームで停止する機能
//

/**
* @brief ローカルアニメを指定フレームで一時停止させる
*
* @param frame 指定フレーム
*
* @return 無し
*/
void StaticModelManager::DebugSuspendLocalAnimation( f32 frame )
{
  for( u32 i=0 ; i<STATIC_MODEL_MAX ; i++ )
  {
    u32 nowModelType = m_ppStaticModelArray[i]->GetNowModelType();
    if( nowModelType == MODEL_TYPE_NONE)
    {
      continue;
    }

    poke_3d::model::BaseModel* pBaseModel = m_ppStaticModelArray[i]->GetBaseModel( nowModelType );

    if( !m_ppStaticModelArray[i]->IsEmpty() 
      && pBaseModel
      )
    {
      m_ppStaticModelArray[i]->SetUpdateCullingEnable( false );
      pBaseModel->SetVisible(true);
      pBaseModel->SetAnimationCalculateEnable(true);
      pBaseModel->SetAnimationStepFrame( 0.0f, ANIMATION_INDEX_LOOP_0 );
      pBaseModel->SetAnimationFrame( frame, ANIMATION_INDEX_LOOP_0 );
    }
  }
}

/**
* @brief ローカルアニメの一時停止を解除
*
* @return 無し
*/
void StaticModelManager::DebugResumeLocalAnimation( void )
{
  for( u32 i=0 ; i<STATIC_MODEL_MAX ; i++ )
  {
    u32 nowModelType = m_ppStaticModelArray[i]->GetNowModelType();
    if( nowModelType == MODEL_TYPE_NONE)
    {
      continue;
    }

    poke_3d::model::BaseModel* pBaseModel = m_ppStaticModelArray[i]->GetBaseModel( nowModelType );

    if( !m_ppStaticModelArray[i]->IsEmpty() 
      && pBaseModel
      )
    {
      m_ppStaticModelArray[i]->SetUpdateCullingEnable( true );
      pBaseModel->SetAnimationStepFrame( 1.0f, ANIMATION_INDEX_LOOP_0 );
    }
  }
}

#endif // PM_DEBUG

//*******************************************************************
//*******************************************************************
// 以下、private
//*******************************************************************
//*******************************************************************

/**
 * @brief ヒープ作成
 * @param pParent
 */
void StaticModelManager::CreateHeap( gfl2::heap::HeapBase* pParent)
{
  const u32 MANAGER_USE_HEAP_SIZE           = 0x4000;                                             ///< このクラスの使用分(インスタンス生成等) 16KB
  const u32 FOR_STATIC_TOTAL_HEAP_SIZE      = 0xB2000;                                            ///< 静的に作成される配置モデル全体 712KB
  const u32 FOR_DYNAMIC_HEAP_SIZE           = 0xA000;                                             ///< 動的に作成される配置モデル1つ分 40KB ( * 4 = 160KB )
  const u32 FOR_DYNAMIC_TOTAL_HEAP_SIZE     = FOR_DYNAMIC_HEAP_SIZE * STATIC_MODEL_DYNAMIC_MAX;   ///< 動的に作成される配置モデル全体 160KB

  // トータル…0xDDC00 (887KB = 0.886MB)
  const u32 STATIC_MODEL_TOTAL_HEAP_SIZE    = MANAGER_USE_HEAP_SIZE + FOR_STATIC_TOTAL_HEAP_SIZE + FOR_DYNAMIC_TOTAL_HEAP_SIZE;

  // 配置モデル全体のヒープを作成 (マネージャー使用量も含む)
  m_pHeap = GFL_CREATE_LOCAL_HEAP_NAME(
    pParent,
    STATIC_MODEL_TOTAL_HEAP_SIZE,
    gfl2::heap::HEAP_TYPE_EXP,
    false,
    "StaticModelTotal"
    );

  // 静的確保用ヒープを作成
  m_pLocalHeapForStatic = GFL_CREATE_LOCAL_HEAP_NAME(
    m_pHeap,
    FOR_STATIC_TOTAL_HEAP_SIZE,
    gfl2::heap::HEAP_TYPE_EXP,
    false,
    "StaticModelStatic"
    );

  // 動的確保用ヒープを作成
  for( s32 i = 0; i < STATIC_MODEL_DYNAMIC_MAX; ++i)
  {
    m_pLocalHeapForDynamic[i] = GFL_CREATE_LOCAL_HEAP_NAME(
      m_pHeap,
      FOR_DYNAMIC_HEAP_SIZE,
      gfl2::heap::HEAP_TYPE_EXP,
      false,
      "StaticModelDynamic"
      );
  }
}

/**
 * @brief ヒープ削除
 */
void StaticModelManager::DeleteHeap()
{
  for( s32 i = 0; i < STATIC_MODEL_DYNAMIC_MAX; ++i)
  {
    GFL_DELETE_HEAP( m_pLocalHeapForDynamic[i]);
  }

  GFL_DELETE_HEAP( m_pLocalHeapForStatic);

  GFL_DELETE_HEAP( m_pHeap);
}

/**
 * @brief 空の配置モデルワークの取得
 * @param pOutWork モデルワーク格納先
 * @return 見つけた配置モデルワークの番号
 */
StaticModel* StaticModelManager::GetEmptyModelWork( s32* pOutIndex) const
{
  for( s32 i = 0; i < STATIC_MODEL_MAX; ++i)
  {
    if( m_ppStaticModelArray[i]->IsEmpty())
    {
      *pOutIndex = i;
      return m_ppStaticModelArray[i];
    }
  }
  GFL_ASSERT_MSG( 0, "WARNING !! Not found Empty Static Model !!\n");
  return NULL;
}

/**
 * @brief 配置モデルIDに一致するモデルを取得
 * @param modelID 配置モデルID
 * @return 配置モデル
 */
StaticModel* StaticModelManager::GetStaticModelIDToModelWork( u32 modelID) const
{
  for( s32 i = 0; i < STATIC_MODEL_MAX; ++i)
  {
    if( modelID == m_ppStaticModelArray[i]->GetStaticModelID())
    {
      return m_ppStaticModelArray[i];
    }
  }
  for( u32 i = 0; i < STATIC_MODEL_DYNAMIC_MAX; ++i)
  {
    if( modelID == m_ppStaticModelDynamicArray[i]->GetStaticModelID())
    {
      return m_ppStaticModelDynamicArray[i];
    }
  }
  GFL_ASSERT_MSG( 0, "WARNING !! Not found Static Model : ID %d !!\n", modelID);
  return NULL;
}

/**
 * @brief 配置モデルIDに一致するリソースパック内の配置モデルデータパックを取得
 * @param modelID 配置モデルID
 * @return 配置モデルデータパック
 */
void* StaticModelManager::GetStaticModelIDToResourcePackData( u32 modelID) const
{
#if PM_DEBUG
  // 出力用モデルIDワーク
  const u16 INVALID_ARRAY_INDEX = 0xFFFF;
  u16 modelIDList[ STATIC_MODEL_MAX ];
  for( u32 listIndex = 0; listIndex < STATIC_MODEL_MAX; ++listIndex)
  {
    modelIDList[listIndex] = INVALID_ARRAY_INDEX;
  }
#endif // PM_DEBUG


  for( s32 i = 0; i < m_pResourcePackBinLinker->GetDataMax(); ++i)
  {
    void* pData = m_pResourcePackBinLinker->GetData( i);
    Field::StaticModel::StaticModelPackAccessor accessor;
    accessor.SetData( pData);

    StaticModelPackAccessor::StaticModelInfo* info = reinterpret_cast<StaticModelPackAccessor::StaticModelInfo*>( accessor.GetStaticModelInfo());
    if( info->modelID == modelID)
    {
      return pData;
    }

#if PM_DEBUG
    // 出力用に検索したモデルIDを保存しておく
    for( u32 listIndex = 0; listIndex < STATIC_MODEL_MAX; ++listIndex)
    {
      if( modelIDList[listIndex] == INVALID_ARRAY_INDEX)
      {
        modelIDList[listIndex] = info->modelID;
        break;
      }
    }
#endif // PM_DEBUG
  }

#if PM_DEBUG

  // エラー時は検索したモデルIDの情報を出力する
  GFL_PRINT( "\n--- Search StaticModelD dump start ---\n");
  for( u32 listIndex = 0; listIndex < STATIC_MODEL_MAX; ++listIndex)
  {
    if( modelIDList[listIndex] != INVALID_ARRAY_INDEX)
    {
      GFL_PRINT( "  modelID [ %3d ]\n", modelIDList[listIndex]);
    }
  }
  GFL_PRINT( "--- Search StaticModelD dump end ---\n\n");

#endif // PM_DEBUG

  return NULL;
}

/**
 * @brief 配置モデルIDに一致するリソースパック内の配置モデルデータパックを取得 (デバッグモデル生成有り版)
 * @param modelID 配置モデルID
 * @return 配置モデルデータパック
 */
void* StaticModelManager::GetStaticModelIDToResourcePackDataEx( u32 modelID)
{
  void* pData = this->GetStaticModelIDToResourcePackData( modelID);
  if( pData == NULL)
  {
#if PM_DEBUG
    // PCデバッガーヒープがあれば、デバッグ用garcからモデルリソースを読み込む
    if( m_pDebuggerHeap)
    {
      GFL_ASSERT_MSG( 0, "Not found StaticModelID [ %d ]\n", modelID);
      GFL_PRINT( "\nStaticModelID [ %d ] use debug model.\n", modelID);
      return this->LoadDebugModelPackData( m_pDebuggerHeap, m_pAsyncFileManager, modelID);
    }
#endif // PM_DEBUG

    GFL_ASSERT_STOP_MSG( 0, "Not found StaticModelID [ %d ]\n", modelID);
  }
  return pData;
}

/**
 * @brief 配置モデルの生成
 * @param pHeap 配置モデルが使用するヒープ
 * @param pAllocator 配置モデルが使用するヒープアロケーター
 * @param pData 配置モデルバイナリパックデータ
 * @param modelID 配置モデルID
 * @param eventID イベントID
 * @param pos 初期位置
 * @param qt 向き
 * @return 生成した配置モデルのポインタ
 */
StaticModel* StaticModelManager::CreateModelCore( gfl2::heap::HeapBase* pHeap, System::nijiAllocator* pAllocator, void* pModelData, u32 modelID, u32 eventID, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& qt, bool isDynamic)
{
  StaticModel* pWork = NULL;
  s32 index = 0;
  if( isDynamic)
  {
    pWork = GetEmptyDynamicModelWork( &index);
    index = STATIC_MODEL_DYNAMIC_OFFSET + index;
  }
  else
  {
    pWork = GetEmptyModelWork( &index);
  }

  if( pWork)
  {
    gfl2::math::SRT srt(
      gfl2::math::Vector3( 1.0f, 1.0f, 1.0f),
      qt,
      pos
      );

    StaticModel::InitializeData initData;
    initData.pHeap            = pHeap;
    initData.modelID          = modelID;
    initData.eventID          = eventID;
    initData.pStaticModelPack = pModelData;
    initData.srt              = srt;
    initData.pFieldRootNode   = m_pFieldRootNode;
    initData.pAllocator       = pAllocator;
    initData.pSEPlayer        = m_pSEPlayer;
    initData.modelIndex       = index;
    initData.pManager         = this;
    initData.pResourceNodeManager = m_pResourceNodeManager;
    initData.isDynamic            = isDynamic;

    pWork->Initialize( initData);

    if( pWork->IsBBAnimationExist())
    {
      pWork->CreateBBAnimationStaticActor( m_pCollisionSceneForBBAnimation);
    }
  }

  return pWork;
}

/**
 * @brief 配置モデルの動的生成
 * @param pData 配置モデルバイナリパックデータ
 * @param modelID 配置モデルID
 * @param eventID イベントID
 * @param pos 初期位置
 * @param qt 向き
 * @return 生成した配置モデルのポインタ
 */
StaticModel* StaticModelManager::CreateModelDynamicCore( void* pModelData, u32 modelID, u32 eventID, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& qt)
{
  GFL_ASSERT_MSG( m_dynamicModelCreateNum < STATIC_MODEL_DYNAMIC_MAX, "StaticModelManager::CreateModelDynamic : 動的生成配置モデルの最大数オーバー\n");

  s32 index = this->GetEmptyDynamicModelIndex();
  if( index == -1)
  {
    return NULL;
  }

  StaticModel* pStaticModel = this->CreateModelCore( m_pLocalHeapForDynamic[ index ], m_pModelAllocatorForDynamic[ index ], pModelData, modelID, eventID, pos, qt, true);

  m_dynamicModelCreateNum++;

  return pStaticModel;
}

/**
 * @brief 空いている動的生成用ヒープ、アロケーター番号の取得
 * @return 空いている動的生成用ヒープ、アロケーター番号
 */
s32 StaticModelManager::GetEmptyDynamicModelIndex() const
{
  for( u32 i = 0; i < STATIC_MODEL_DYNAMIC_MAX; ++i)
  {
    if( m_ppStaticModelDynamicArray[i]->IsEmpty())
    {
      return i;
    }
  }
  GFL_ASSERT_MSG( 0, "StaticModelManager::GetEmptyDynamicModelIndex : 動的生成配置モデルの空きがない\n");
  return -1;
}

/**
 * @brief 空の動的生成用配置モデルワークの取得
 * @param pOutIndex ワーク番号格納先
 * @return 空の動的生成用配置モデルワーク
 */
StaticModel* StaticModelManager::GetEmptyDynamicModelWork( s32* pOutIndex) const
{
  s32 index = this->GetEmptyDynamicModelIndex();
  if( index != -1)
  {
    *pOutIndex = index;
    return m_ppStaticModelDynamicArray[index];
  }
  GFL_ASSERT_STOP_MSG( 0, "StaticModelManager::GetEmptyDynamicModelIndex : 動的生成配置モデルの空きがない\n");
  return NULL;
}

//-------------------------------------------------------------------
// 以下デバッグ用途
//-------------------------------------------------------------------
#if PM_DEBUG

/**
 *  @brief  同期arcオープン
 */
void StaticModelManager::SyncArcOpen( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager, u32 arcID)
{
  gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
  openReq.arcId         = arcID;
  openReq.heapForFile   = pHeap;
  openReq.heapForArcSrc = pHeap;
  openReq.heapForReq    = pHeap->GetLowerHandle();
  pAsyncFileManager->SyncArcFileOpen( openReq );
}

/**
 *  @brief  同期arcクローズ
 */
void StaticModelManager::SyncArcClose( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager, u32 arcID)
{
  gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
  closeReq.arcId      = arcID;
  closeReq.heapForReq = pHeap;
  pAsyncFileManager->SyncArcFileClose( closeReq );
}

/**
 *  @brief  同期データ読み込み
 */
void StaticModelManager::SyncArcLoad( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager, u32 arcID, u32 datID, void** ppBuff, u32* pSize, u32 align)
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
  loadReq.arcId       = arcID;
  loadReq.datId       = datID;
  loadReq.ppBuf       = ppBuff;
  loadReq.pBufSize    = pSize;
  loadReq.heapForBuf  = pHeap;
  loadReq.align       = align;
  pAsyncFileManager->SyncArcFileLoadData( loadReq );
}

/**
 *  @brief  デバッグ用 空のデータバッファ取得
 */
void** StaticModelManager::GetEmptyDebugModelPackDataBuffer()
{
  for( u32 i = 0; i < STATIC_MODEL_MAX; ++i)
  {
    if( m_pDebugModelPackDataBuffer[i] == NULL)
    {
      return &m_pDebugModelPackDataBuffer[i];
    }
  }
  GFL_ASSERT_STOP_MSG( 0, "Not found empty debug resource buffer."); // ここに来たらそもそもワークの上限をオーバーしている
  return NULL;
}

/**
 *  @brief  デバッグ用 モデルパックデータ読み込み
 */
void* StaticModelManager::LoadDebugModelPackData( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager, u32 modelID)
{
  void** ppBuffer = this->GetEmptyDebugModelPackDataBuffer();
  this->SyncArcLoad( pHeap, pAsyncFileManager, ARCID_STATIC_MODEL_D, modelID, ppBuffer, NULL, 128);
  return *ppBuffer;
}

/**
 *  @brief  デバッグ用 モデルパックデータ破棄
 */
void StaticModelManager::TerminateDebugModelPackDataBuffer()
{
  for( u32 i = 0; i < STATIC_MODEL_MAX; ++i)
  {
    GflHeapSafeFreeMemory( m_pDebugModelPackDataBuffer[i]);
  }
}

/**
 *  @brief  デバッグ用 モデル名取得
 */
char* StaticModelManager::GetModelName( u32 modelID)
{
  if( modelID < STATIC_MODEL_ID_MAX)
  {
    const u32 NAME_LENGTH = 24;
    return &m_pDebugModelName[ modelID * NAME_LENGTH ];
  }
  GFL_ASSERT( 0);
  return &m_pDebugModelName[0];
}

/**
 *  @brief  デバッグ用 ゾーン名取得
 */
char* StaticModelManager::GetZoneName( u32 zoneID)
{
  if( m_zoneID < ZONE_ID_MAX)
  {
    const u32 NAME_LENGTH = 16;
    return &m_pDebugZoneName[ zoneID * NAME_LENGTH ];
  }
  GFL_ASSERT( 0);
  return &m_pDebugZoneName[0];
}

/**
 *  @brief  デバッグ用 配置モデルの情報をテキストに出力
 */
void StaticModelManager::WriteTextStaticModel()
{
#if defined(GF_PLATFORM_CTR)
  if( Debug::DebugTypes::s_isOutputStaticModelMemoryText == false)
  {
    return;
  }

  if( m_pDebuggerHeap == NULL)
  {
    return;
  }

  bool isEnable;
  gfl2::fs::Result result = gfl2::fs::PcUtil::IsPcAccessEnableFlag( &isEnable);
  if( result.IsSuccess() == false || isEnable == false)
  {
    return;
  }

  this->SyncArcOpen( m_pDebuggerHeap->GetLowerHandle(), m_pAsyncFileManager, ARCID_STATIC_MODEL_NAME_LIST_D);
  this->SyncArcLoad( m_pDebuggerHeap, m_pAsyncFileManager, ARCID_STATIC_MODEL_NAME_LIST_D, 0, reinterpret_cast<void**>( &m_pDebugModelName), NULL, 4);
  this->SyncArcClose( m_pDebuggerHeap, m_pAsyncFileManager, ARCID_STATIC_MODEL_NAME_LIST_D);

  this->SyncArcOpen( m_pDebuggerHeap->GetLowerHandle(), m_pAsyncFileManager, ARCID_ZONE_DATA_D);
  this->SyncArcLoad( m_pDebuggerHeap, m_pAsyncFileManager, ARCID_ZONE_DATA_D, 0, reinterpret_cast<void**>( &m_pDebugZoneName), NULL, 4);
  this->SyncArcClose( m_pDebuggerHeap, m_pAsyncFileManager, ARCID_ZONE_DATA_D);

  char buffer[2048] = {0};
  u32 offset = 0;
  offset += sprintf( &buffer[ offset ], "ModelName,InstanceSize,AnimationSize\n");
  for( u32 i = 0; i < STATIC_MODEL_MAX; ++i)
  {
    if( m_ppStaticModelArray[i]->IsEmpty() == false)
    {
      StaticModel* pStaticModel = m_ppStaticModelArray[i];
      u32 modelID = pStaticModel->GetStaticModelID();
      offset += sprintf( &buffer[ offset ], "%s,%d,%d\n",
        this->GetModelName( modelID), pStaticModel->GetInstanceAllocSize(), pStaticModel->GetAnimationAllocSize()
        );
    }
  }
  buffer[ offset] = '\0';
  offset++;

  u32 offset2 = 0;
  char name[256] = {0};
  // data_ctrがルート
  offset2 += sprintf( name, "../direct_sdmc/Field/StaticModel/StaticModelMemory_%s.txt", this->GetZoneName( m_zoneID));
  name[ offset2 ] = '\0';

  char fullpath[256] = {0};
  u32 fullpathLen = 0;
  gfl2::fs::PcUtil::GetFullPathFileName( &fullpathLen, fullpath, 256, m_pAsyncFileManager, name);

  gfl2::fs::PcUtil::WriteFile( fullpath, buffer, offset);

  GflHeapSafeFreeMemory( m_pDebugZoneName);
  GflHeapSafeFreeMemory( m_pDebugModelName);
#endif // GF_PLATFORM_CTR
}

#endif // PM_DEBUG

GFL_NAMESPACE_END( StaticModel )
GFL_NAMESPACE_END( Field )
