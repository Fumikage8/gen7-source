/**
 *  GAME FREAK inc.
 *
 *  @file   FieldStaticModelActor.cpp
 *  @brief  配置モデル用アクター
 *  @author takahashi tomoya 
 *  @date   2015.05.30
 */

// Field
#include "Field/FieldRo/include/PlacementData/actor/FieldStaticModelActor.h"
#include "Field/FieldRo/include/StaticModel/StaticModelManager.h"
#include "Field/FieldRo/include/StaticModel/StaticModel.h"
#include "Field/FieldRo/include/StaticModel/StaticModelDefine.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainManager.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Savedata/include/EventWork.h"

GFL_NAMESPACE_BEGIN( Field );

/**
 *  @brief  コンストラクタ
 */
FieldStaticModelActor::FieldStaticModelActor( void )
  : FieldStaticModelActorBase()
  , m_pData( NULL)
{
}

/**
 *  @brief  デストラクタ
 */
FieldStaticModelActor::~FieldStaticModelActor( void )
{
}

/**
 *  @brief  配置モデルの作成
 */
void FieldStaticModelActor::Initialize( StaticModelSetupData& setupData )
{
  m_pStaticModel = this->createStaticModel( setupData);
  this->initialize( m_pStaticModel, setupData, setupData.pAccessor->GetCollisionData( *(setupData.pData) ));
}

/**
 *  @brief  配置モデルの作成 : コリジョンデータを外部から与えるバージョン
 */
void FieldStaticModelActor::Initialize(StaticModelSetupData& setupData, const void* pCollisionData)
{
  m_pStaticModel = this->createStaticModel( setupData);
  this->initialize( m_pStaticModel, setupData, pCollisionData);
}

/**
 *  @brief  配置モデルの動的作成
 */
void FieldStaticModelActor::InitializeForDynamic( StaticModelSetupData& setupData )
{
  m_pStaticModel = this->createStaticModelDynamic( setupData);
  this->initialize( m_pStaticModel, setupData, setupData.pAccessor->GetCollisionData( *(setupData.pData) ));
}

/**
 *  @brief  起動
 */
bool FieldStaticModelActor::Kick( const KickRequest &req, bool * p_outIsEventCreate )
{
  if( p_outIsEventCreate ) *p_outIsEventCreate = false; 

  // 配置モデル単体で起動することはない。
  if( IsInvalid() ){ return false; }
  
  return false;
}

/**
 *  @brief  判定
 */
bool FieldStaticModelActor::IsInvalid( void )
{
  // 配置モデル単体で起動することはない。
  return false;
}

/**
 *  @brief  フラグワークの値セット
 */
bool FieldStaticModelActor::SetEventFlagWork( u16 value )
{
  if( m_pData->flagwork == 0 ) return false;

  EventWork * pEventWork = GameSys::GameManager::GetInstance()->GetGameData()->GetEventWork();

  // フラグワークのチェック
  if( EventWork::CheckIDFlagRenge( m_pData->flagwork ) ){
    if( value == 0 ){
      pEventWork->ResetEventFlag( m_pData->flagwork );
    }else{
      pEventWork->SetEventFlag( m_pData->flagwork );
    }
  }else{
    pEventWork->SetEventWork( m_pData->flagwork, value );
  }
  return true;
}

/**
 *  @brief  追加
 */
void FieldStaticModelActor::registActor( void )
{
  // ユーザー指定のRegist:true　Culling:falseのときのみ本当に登録
  if( isNeedRegistActor() )
  {
    // コリジョンシーンへの登録
    this->registCollisionScene();

    // 配置モデルの表示ON
    this->visibleStaticModel();
  }
}

/**
 *  @brief  削除
 */
void FieldStaticModelActor::unregistActor( void )
{
  // ユーザー指定のRegist:false　か　Culling:trueのときは、unregist
  if( isNeedUnRegistActor() )
  {
    // コリジョンシーンからの削除
    this->unregistCollisionScene();

    // 配置モデルの表示OFF
    this->invisibleStaticModel();
  }
}

/**
 *  @brief  初期化
 */
void FieldStaticModelActor::initialize( StaticModel::StaticModel* pStaticModel, StaticModelSetupData& setupData, const void* pCollisionData )
{
  pStaticModel->SetVisibleAll(false);  // 初期状態は全て非表示
  pStaticModel->SetStaticActor(NULL);

  // 配置モデル情報
  m_pData = setupData.pData;
  // 座標情報を保存 miyachi 2015.06.02
  m_vPos = setupData.vPos;

  // 通知リスナーの登録削除のために保持する
  m_pTerrainManager = setupData.pTerrainManager;
  m_pRenderingPipeLine = setupData.pRenderingPipeLine;

  // 衝突用コリジョンの設定
  {
    if( pCollisionData == NULL)
    {
      return;
    }

    FieldCollisionAccessor collisionAccessor;
    collisionAccessor.Initialize( pCollisionData );

    m_Size = collisionAccessor.GetDataMax();
    if( m_Size > 0 )
    {
      m_pCollisionScene = setupData.pCollisionScene;

      // アクター配列確保
      m_ppActor = GFL_NEW_ARRAY( setupData.pHeap ) StaticActor*[m_Size];

      // アクター生成
      for( u32 i = 0; i < m_Size; ++ i )
      {
        // データ取得
        FieldCollisionAccessor::CollisionData colData;
        if( getCollisionData( &colData, collisionAccessor, i) == false)
        {
          continue;
        }

        // アクターの生成、コリジョン設定
        StaticActor *pActor = m_pCollisionScene->CreateStaticActor( setupData.pAllocator, setupData.pHeap);
        setupCollision( colData, setupData.vPos, pActor, this);

        m_pStaticModel->SetStaticActor(pActor);
        m_ppActor[ i ] = pActor;
      }
    }
  }
}

/**
 *  @brief  静的配置モデル生成
 */
StaticModel::StaticModel* FieldStaticModelActor::createStaticModel( StaticModelSetupData& setupData)
{
  StaticModel::StaticModel* pStaticModel = NULL;
  // ☆配置モデルの生成
  // コリジョンは別で生成するので、配置モデルで生成する必要は無い。
  // その代わり、配置モデルの有効無効は、FieldStaticModelActorに対して行なう必要がある。
  {
    pStaticModel = setupData.pStaticModelManager->CreateModel(
      setupData.pData->staticModelID,
      setupData.pData->eventID,
      setupData.vPos,
      setupData.Quaternion
      );


  }
  return pStaticModel;
}

/**
 *  @brief  動的配置モデル生成
 */
StaticModel::StaticModel* FieldStaticModelActor::createStaticModelDynamic( StaticModelSetupData& setupData)
{
  StaticModel::StaticModel* pStaticModel = NULL;
  // ☆配置モデルの生成
  // コリジョンは別で生成するので、配置モデルで生成する必要は無い。
  // その代わり、配置モデルの有効無効は、FieldStaticModelActorに対して行なう必要がある。
  {
    pStaticModel = setupData.pStaticModelManager->CreateModelDynamic(
      setupData.pData->staticModelID,
      setupData.pData->eventID,
      setupData.vPos,
      setupData.Quaternion
      );
  }
  return pStaticModel;
}

GFL_NAMESPACE_END( Field );
