/**
 *  GAME FREAK inc.
 *
 *  @file   FieldActorBase.cpp
 *  @brief  フィールド用アクター規定
 *  @author miyachi_soichi
 *  @date   2015.05.26
 */
// System
#include "System/include/nijiAllocator.h"
// Field
#include "Field/FieldRo/include/PlacementData/actor/FieldActorBase.h"

GFL_NAMESPACE_BEGIN( Field );

/**
 *  @brief  コンストラクタ
 */
IFieldActorBase::IFieldActorBase( void )
  : m_pCollisionScene( NULL )
  , m_ppActor( NULL )
  , m_Size( 0 )
  , m_bIsRegist( false )
  , m_vPos()
{
}

/**
 *  @brief  デストラクタ
 */
IFieldActorBase::~IFieldActorBase( void )
{
}

/**
 *  @brief  初期化
 *  CollisionSceneへの登録は行いませんので別途Registを呼び出してください
 */
void IFieldActorBase::Initialize( SetupData &data )
{
  if( data.pColAccessor->GetDataMax() <= 0 ){ return; }
  m_pCollisionScene = data.pScene;
  m_Size = data.pColAccessor->GetDataMax();
  m_vPos = data.vPos;

  // アクター配列確保
  m_ppActor = GFL_NEW_ARRAY( data.pHeap ) StaticActor*[m_Size];

  // アクター生成
  for( u32 i = 0; i < m_Size; ++ i )
  {
    GFL_ASSERT_STOP( m_pCollisionScene ); // CollisionSceneはある必要がある。

    // データ取得
    FieldCollisionAccessor::CollisionData colData;
    data.pColAccessor->GetData( i, &colData );

    // アクターの生成
    StaticActor *pActor = m_pCollisionScene->CreateStaticActor( data.pAllocator, data.pHeap );

    // コリジョン設定
    FieldCollisionAccessor::CreateActor( pActor, colData, data.vPos );
    // ユーザーデータ設定
    pActor->SetUserData( reinterpret_cast<void*>( this ) );

    m_ppActor[ i ] = pActor;
  }
}

/**
 *  @brief  破棄
 *  安全のためにUnregistActorを呼び出しているので破棄の際に別途Unregistは不要
 */
void IFieldActorBase::Terminate( void )
{
  UnregistActor();
  for( u32 i = 0; i < m_Size; ++ i )
  {
    if( !m_ppActor[ i ] ){ continue; }
    GFL_SAFE_DELETE( m_ppActor[ i ] );
  }
  GFL_SAFE_DELETE_ARRAY( m_ppActor );
}

/**
 *  @brief  追加
 */
void IFieldActorBase::RegistActor( void )
{
  if( !m_pCollisionScene ){ return; }

  for( u32 i = 0; i < m_Size; ++ i )
  {
    if( !m_ppActor[ i ] ){ continue; }
    m_pCollisionScene->RegistStaticActor( m_ppActor[ i ] );
  }

  m_bIsRegist = true;
}

/**
 *  @brief  削除
 */
void IFieldActorBase::UnregistActor( void )
{
  if( !m_pCollisionScene ){ return; }
  for( u32 i = 0; i < m_Size; ++ i )
  {
    if( !m_ppActor[ i ] ){ continue; }
    if( m_ppActor[ i ]->GetUserData() != NULL )
    {
      m_pCollisionScene->UnRegistStaticActor( m_ppActor[ i ] );
    }
  }

  m_bIsRegist = false;
}

/**
 *  @brief  スタティックコリジョンアクターの生成
 *          (現在円柱のみに対応しているが、他の形状も修正により対応可能)
 *  @param  pOut              結果を受け取る構造体
 *  @param  desc              初期化用情報
 *  @param  vPos              座標
 *  @param  pAllocator        コリジョン生成に使用するアロケータ
 *  @param  pHeap             コリジョンインスタンスの取得元
 *  @param  pFieldActor       紐付けるイベントのアクター
 */
void IFieldActorBase::CreateCollisionInstance(
  StaticCollisionPair* pOut,
  const CollisionSetupData& desc, 
  gfl2::math::Vector3& vPos,
  System::nijiAllocator* pAllocator,
  gfl2::heap::HeapBase* pHeap,
  IFieldActorBase* pFieldActor
  )
{
  FieldCollisionAccessor collisionAccessor;
  FieldCollisionAccessor::CollisionData colData;

  collisionAccessor.Initialize(desc.pCollisionData);    
  GFL_ASSERT_MSG(collisionAccessor.GetDataMax() == 1, "現在円柱のみに対応.");
  collisionAccessor.GetData(0, &colData);

  pOut->pScene = desc.pScene;
  pOut->pActor = desc.pScene->CreateStaticActor(pAllocator, pHeap);
  FieldCollisionAccessor::CreateActor(pOut->pActor, colData, vPos);
  pOut->pActor->SetUserData(reinterpret_cast<void*>(pFieldActor));
}

/**
 *  @brief  コリジョンの登録
 */
void IFieldActorBase::RegisterCollision(const StaticCollisionPair* pPair)
{
  pPair->pScene->RegistStaticActor(pPair->pActor);
}

/**
 *  @brief  コリジョンの登録解除
 */
void IFieldActorBase::UnregisterCollision(const StaticCollisionPair* pPair)
{
  if(pPair->pActor == NULL )
  {
    return;
  }

  if(pPair->pActor->GetUserData() == NULL )
  {
    return;
  }

  pPair->pScene->UnRegistStaticActor(pPair->pActor);
}

/**
 *  @brief  フラグワークの値セット
 */
bool IFieldActorBase::SetEventFlagWork( u16 /*value*/ )
{
  return false;
}


GFL_NAMESPACE_END( Field );
