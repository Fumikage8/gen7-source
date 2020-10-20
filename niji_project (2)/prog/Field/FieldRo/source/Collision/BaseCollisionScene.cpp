//=================================================================================================
/**
 *  @file   BaseCollisionScene.cpp
 *  @brief  ベースコリジョンシーン
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include "Field/FieldRo/include/Collision/BaseShape.h"
#include "Field/FieldRo/include/Collision/BaseActor.h"
#include "Field/FieldRo/include/Collision/BaseSolver.h"
#include "Field/FieldRo/include/Collision/BaseCollisionScene.h"
#include "Field/FieldRo/include/Collision/StaticActor.h"
#include "Field/FieldRo/include/Collision/DynamicActor.h"

#if PM_DEBUG
#include "Field/FieldRo/include/Debug/FieldDebugCollisionRenderManager.h"
#endif  //#if PM_DEBUG
//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
BaseCollisionScene::BaseCollisionScene()
: m_pSolver(NULL)
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
BaseCollisionScene::~BaseCollisionScene()
{
  Terminate();
}

//-----------------------------------------------------------------------------
/**
*  @brief  初期化
*
*  @param  pAllocator  利用するアロケータ
*  @param  pHeap       利用するヒープ
*/
//-----------------------------------------------------------------------------
void BaseCollisionScene::Setup(gfl2::gfx::IGLAllocator* pAllocator,gfl2::heap::HeapBase* pHeap)
{
  m_pAllocator = pAllocator;
  m_pHeap      = pHeap;

  // ルートはあくまでツリー形成のためなのでベースアクター決め打ちで問題ない
  // 場所をここから変更する際は使用されるアロケータも変わってしまうことに注意してください
  m_rootStaticActorRoot  = GLNew(m_pAllocator) BaseActor();
  m_rootDynamicActorRoot = GLNew(m_pAllocator) BaseActor();

  // 場所をここから変更する際は使用されるアロケータも変わってしまうことに注意してください
  m_pSolver = CreateSolver();
}

//-----------------------------------------------------------------------------
/**
*  @brief  削除
*/
//-----------------------------------------------------------------------------
void BaseCollisionScene::Terminate(void)
{
  if( m_rootStaticActorRoot )
  {
    delete m_rootStaticActorRoot;
    m_rootStaticActorRoot = NULL;
  }
  if( m_rootDynamicActorRoot )
  {
    delete m_rootDynamicActorRoot;
    m_rootDynamicActorRoot = NULL;
  }
  if( m_pSolver )
  {
    delete m_pSolver;
    m_pSolver = NULL;
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  コリジョン判定
*/
//-----------------------------------------------------------------------------
void BaseCollisionScene::Update(void)
{
  // AABB構築

  // AABBを利用したざっくりとした判定

  // 詳細判定
  if( m_pSolver )
  {
    m_pSolver->Solved( m_rootStaticActorRoot , m_rootDynamicActorRoot );
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  メッシュ形状を軽量化し保持する
*
*  @param  pos         ポリゴンを収集する中心値
*  @param  collectionR ポリゴンを収集する範囲
*/
//-----------------------------------------------------------------------------
void BaseCollisionScene::MeshCooking(gfl2::math::Vector3 pos,f32 collectionR)
{
  if( m_pSolver )
  {
    m_pSolver->MeshCooking( m_rootStaticActorRoot,pos,collectionR );
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  衝突されるアクターの作成
*
*  @param  pAllocator  利用するアロケータ
*  @param  pHeap       利用するヒープ
*  @return アクター
*/
//-----------------------------------------------------------------------------
StaticActor* BaseCollisionScene::CreateStaticActor(gfl2::gfx::IGLAllocator* pAllocator,gfl2::heap::HeapBase* pHeap)
{
  StaticActor* pStaticActor = pAllocator ? GLNew(pAllocator) StaticActor() : GLNew(m_pAllocator) StaticActor();
  pStaticActor->SetHeap( pHeap ? pHeap : m_pHeap );
  return pStaticActor;
}

//----------------------------------------------------------------------------
/**
*  @brief  衝突するアクターの作成
*
*  @param  pAllocator  利用するアロケータ
*  @param  pHeap       利用するヒープ
*  @return アクター
*/
//-----------------------------------------------------------------------------
DynamicActor* BaseCollisionScene::CreateDynamicActor(gfl2::gfx::IGLAllocator* pAllocator,gfl2::heap::HeapBase* pHeap)
{
  DynamicActor* pDynamicActor = pAllocator ? GLNew(pAllocator) DynamicActor() : GLNew(m_pAllocator) DynamicActor();
  pDynamicActor->SetHeap( pHeap ? pHeap : m_pHeap );
  return pDynamicActor;
}

//----------------------------------------------------------------------------
/**
*  @brief  衝突されるアクターの追加
*
*  @param  pActor  コリジョンシステムに登録したいアクター
*/
//-----------------------------------------------------------------------------
void BaseCollisionScene::RegistStaticActor(StaticActor *pActor)
{
  m_rootStaticActorRoot->AddChild( pActor );
}

//----------------------------------------------------------------------------
/**
*  @brief  衝突するアクターの追加
*
*  @param  pActor  コリジョンシステムに登録したいアクター
*/
//-----------------------------------------------------------------------------
void BaseCollisionScene::RegistDynamicActor(DynamicActor *pActor)
{
  m_rootDynamicActorRoot->AddChild( pActor );
}

//----------------------------------------------------------------------------
/**
*  @brief  衝突されるアクターの削除
*
*  @param  pActor  コリジョンシステムから削除したいアクター
*/
//-----------------------------------------------------------------------------
void BaseCollisionScene::UnRegistStaticActor(StaticActor *pActor)
{
  m_rootStaticActorRoot->RemoveChild(pActor);
}

//----------------------------------------------------------------------------
/**
*  @brief  衝突するアクターの削除
*
*  @param  pActor  コリジョンシステムから削除したいアクター
*/
//-----------------------------------------------------------------------------
void BaseCollisionScene::UnRegistDynamicActor(DynamicActor *pActor)
{
  m_rootDynamicActorRoot->RemoveChild(pActor);
}

//----------------------------------------------------------------------------
/**
*  @brief   解決者の作成
*
*  @return  solver
*  @note    この関数をオーバーライドすることにより衝突の挙動を変更できます
*/
//-----------------------------------------------------------------------------
BaseSolver* BaseCollisionScene::CreateSolver(void)
{
  return GFL_NEW(m_pHeap) BaseSolver();
}

//----------------------------------------------------------------------------
/**
*  @brief  シーンに登録されている衝突されるメッシュに対してのレイキャスト
*
*  @param  start     レイ開始位置
*  @param  end       レイ終了位置
*  @param  pHitData  レイの結果を受け取るための構造体(スタートからの最近接点が返ってきます)
*  @return           レイキャストがヒットしたかどうか
*/
//-----------------------------------------------------------------------------
b32 BaseCollisionScene::RaycastFromStaticActorsMesh(gfl2::math::Vector4& start,gfl2::math::Vector4& end,RaycastCustomCallback::HIT_DATA* pHitData)
{
  b32 isFirst = true;

  for( u32 i=0 ; i<m_rootStaticActorRoot->GetChildCount() ; i++ )
  {
    // コンテナからメッシュ形状を持つスタティックアクターを取得
    StaticActor* pStaticActor = m_rootStaticActorRoot->GetChild(i)->SafeCast<StaticActor>();
    if( !pStaticActor || pStaticActor->GetShapeType() != BaseActor::SHAPE_MESH || !pStaticActor->IsActive() )
    {
      continue;
    }

    // スタティックアクターからメッシュ形状を取得
    MeshShape* pStaticActorMesh = static_cast<MeshShape*>(pStaticActor->GetShape());
    if( !pStaticActorMesh || !pStaticActorMesh->m_pCollisionModel )
    {
      continue;
    }

    // メッシュ形状を利用しレイキャスト
    RaycastCustomCallback callback;
    pStaticActorMesh->m_pCollisionModel->GetIntersection( start,end,&callback );
    if( !callback.IsHit() )
    {
      continue;
    }

    // レイキャストの開始点に一番近いものを確保
    gfl2::math::Vector4 temp0( callback.GetHitData().intersection - start );
    gfl2::math::Vector4 temp1( pHitData->intersection             - start );
    if( isFirst || temp0.Length3() < temp1.Length3() )
    {
      isFirst = false;
      *pHitData = callback.GetHitData();
    }
  }

  return (isFirst) ? false : true;
}

//----------------------------------------------------------------------------
/**
*  @brief  シーンに登録されている衝突されるメッシュに対してのレイキャスト
*
*  @param  start     レイ開始位置
*  @param  end       レイ終了位置
*  @return           レイキャストがヒットしたポリゴンの数( 計測に失敗した場合は-1を返します )
*/
//-----------------------------------------------------------------------------
s32 BaseCollisionScene::RaycastFromStaticActorsMesh(gfl2::math::Vector4& start,gfl2::math::Vector4& end)
{
  s32 cnt = 0;

  for( u32 i=0 ; i<m_rootStaticActorRoot->GetChildCount() ; i++ )
  {
    // コンテナからメッシュ形状を持つスタティックアクターを取得
    StaticActor* pStaticActor = m_rootStaticActorRoot->GetChild(i)->SafeCast<StaticActor>();
    if( !pStaticActor || pStaticActor->GetShapeType() != BaseActor::SHAPE_MESH || !pStaticActor->IsActive() )
    {
      continue;
    }

    // スタティックアクターからメッシュ形状を取得
    MeshShape* pStaticActorMesh = static_cast<MeshShape*>(pStaticActor->GetShape());
    if( !pStaticActorMesh || !pStaticActorMesh->m_pCollisionModel )
    {
      continue;
    }

    // メッシュ形状を利用しレイキャスト
    s32 result = pStaticActorMesh->m_pCollisionModel->GetIntersectionCnt( start,end );
    if( result == -1 )
    {
      return -1;
    }
    cnt += result;
  }

  return cnt;
}

//----------------------------------------------------------------------------
/**
*  @brief  シーンに登録されている衝突されるシリンダーに対してのレイキャスト
*
*  @param  start     レイ開始位置
*  @param  end       レイ終了位置
*  @return           レイキャストがヒットしたかどうか
*/
//-----------------------------------------------------------------------------
b32 BaseCollisionScene::RaycastFromStaticActorsCylinder(gfl2::math::Vector4& start,gfl2::math::Vector4& end)
{
  gfl2::math::Vector3 st( start.x,start.y,start.z );
  gfl2::math::Vector3 ed(   end.x,  end.y,  end.z );

  for( u32 i=0 ; i<m_rootStaticActorRoot->GetChildCount() ; i++ )
  {
    // コンテナからシリンダー形状を持つスタティックアクターを取得
    StaticActor* pStaticActor = m_rootStaticActorRoot->GetChild(i)->SafeCast<StaticActor>();
    if( !pStaticActor || pStaticActor->GetShapeType() != BaseActor::SHAPE_CYLINDER || !pStaticActor->IsActive() )
    {
      continue;
    }

    // スタティックアクターからシリンダー形状を取得
    CylinderShape* pStaticActorCylinder = static_cast<CylinderShape*>(pStaticActor->GetShape());
    if( !pStaticActorCylinder )
    {
      continue;
    }

    // 光線vsシリンダー
    f32                 r     ( pStaticActorCylinder->m_r );
    gfl2::math::Vector3 pos   ( pStaticActorCylinder->m_pos + pStaticActor->GetPosition() );
    gfl2::math::Vector3 dir   ( ed-st );
    f32                 dirLen( dir.Length() );
    dir = dir.Normalize();
    if( !IntersectRaySphere( st, dir , dirLen , r, pos ) )
    {
      continue;
    }

    return true;
  }

  return false;
}

//----------------------------------------------------------------------------
/**
*  @brief  シーンに登録されている衝突される箱に対してのレイキャスト
*
*  @param  start     レイ開始位置
*  @param  end       レイ終了位置
*  @return           レイキャストがヒットしたかどうか
*/
//-----------------------------------------------------------------------------
b32 BaseCollisionScene::RaycastFromStaticActorsBox(gfl2::math::Vector4& start,gfl2::math::Vector4& end)
{
  gfl2::math::Vector3 st( start.x,start.y,start.z );
  gfl2::math::Vector3 ed(   end.x,  end.y,  end.z );

  for( u32 i=0 ; i<m_rootStaticActorRoot->GetChildCount() ; i++ )
  {
    // コンテナから箱形状を持つスタティックアクターを取得
    StaticActor* pStaticActor = m_rootStaticActorRoot->GetChild(i)->SafeCast<StaticActor>();
    if( !pStaticActor || pStaticActor->GetShapeType() != BaseActor::SHAPE_BOX || !pStaticActor->IsActive() )
    {
      continue;
    }

    // スタティックアクターから箱形状を取得
    BoxShape* pStaticActorBox = static_cast<BoxShape*>(pStaticActor->GetShape());
    if( !pStaticActorBox )
    {
      continue;
    }

    // 光線vs箱
    gfl2::math::Vector3 u0 (pStaticActorBox->m_u0.x,0.0f,pStaticActorBox->m_u0.z); u0.Normalize();
    gfl2::math::Vector3 u1 (pStaticActorBox->m_u1.x,0.0f,pStaticActorBox->m_u1.z); u1.Normalize();
    gfl2::math::Vector3 max( pStaticActor->GetPosition() + pStaticActorBox->m_pos + (u0 * pStaticActorBox->m_e0) + (u1 * pStaticActorBox->m_e1) ); max.y += pStaticActorBox->m_height;
    gfl2::math::Vector3 min( pStaticActor->GetPosition() + pStaticActorBox->m_pos - (u0 * pStaticActorBox->m_e0) - (u1 * pStaticActorBox->m_e1) ); max.y += 0.0f;
    gfl2::math::Vector3 c  ( (max+min)/2.0f );
    if( !IntersectRayObb( st,
                          ed,
                          c,
                          u0,
                          gfl2::math::Vector3(0.0f,1.0f,0.0f),
                          u1,
                          max-c,
                          pStaticActorBox->m_e0,
                          pStaticActorBox->m_height,
                          pStaticActorBox->m_e1) )
    {
      continue;
    }

    return true;
  }

  return false;
}

//----------------------------------------------------------------------------
/**
*  @brief   光線vsスフィア
*/
//-----------------------------------------------------------------------------
u32 BaseCollisionScene::IntersectRaySphere(gfl2::math::Vector3 p,gfl2::math::Vector3 d,f32 dLen,f32 cylinderR,gfl2::math::Vector3 cylinderPos)
{
  gfl2::math::Vector3 m = p - cylinderPos;
  f32 b = m.Dot(d);
  f32 c = m.Dot(m) - cylinderR * cylinderR;

  // rの原点がsの外側にあり(c > 0),rがsから離れていく方向を指している場合(b > 0)に終了
  if( c > 0.0f && b > 0.0f )
  {
    return 0;
  }
  f32 discr = b*b - c;

  // 負の判別式は光線が球を外れていることに一致
  if( discr < 0.0f )
  {
    return 0;
  }

  // これで光線は球と交差していることが分かり、交差する最小の値tを計算
  f32 t = -b - gfl2::math::FSqrt(discr);

  // tが負である場合、光線は球の内側から開始しているのでtをゼロにクランプ
  if( t<0.0f )
  {
    t = 0.0f;
  }

  // 交差する最小の点が線分よりおくにある
  if( t>dLen )
  {
    return 0;
  }

  return 1;
}

//----------------------------------------------------------------------------
/**
*  @brief   光線vsOBB
*/
//-----------------------------------------------------------------------------
u32 BaseCollisionScene::IntersectRayObb(gfl2::math::Vector3 p0,
                                        gfl2::math::Vector3 p1,
                                        gfl2::math::Vector3 c,
                                        gfl2::math::Vector3 u0,
                                        gfl2::math::Vector3 u1,
                                        gfl2::math::Vector3 u2,
                                        gfl2::math::Vector3 e,
                                        f32                 e0,
                                        f32                 e1,
                                        f32                 e2)
{
  gfl2::math::Vector3 m = (p0 + p1) * 0.5f;
  gfl2::math::Vector3 d = p1 - m;
  m = m - c;
  m = gfl2::math::Vector3( u0.Dot(m), u1.Dot(m), u2.Dot(m) );
  d = gfl2::math::Vector3( u0.Dot(d), u1.Dot(d), u2.Dot(d) );

  float adx = fabsf(d.x);
  if(fabsf(m.x) > e0 + adx) return 0;
  float ady = fabsf(d.y);
  if(fabsf(m.y) > e1 + ady) return 0;
  float adz = fabsf(d.z);
  if(fabsf(m.z) > e2 + adz) return 0;
  adx += EPSILON; 
  ady += EPSILON; 
  adz += EPSILON;

  if(fabsf(m.y * d.z - m.z * d.y) > e1 * adz + e2 * ady ) return 0;
  if(fabsf(m.z * d.x - m.x * d.z) > e0 * adz + e2 * adx ) return 0;
  if(fabsf(m.x * d.y - m.y * d.x) > e0 * ady + e1 * adx ) return 0;
  return 1;
}

#if PM_DEBUG
//----------------------------------------------------------------------------
/**
*  @brief  衝突されるアクターをデバッグ表示へ登録
*
*  @param  color          コリジョン表示色
*  @param  onlyActiveFlag true:アクティブフラグが立っているアクターのみ表示する false:シーンに登録されているアクターを全て表示する
*  @param  positon        表示位置(アクターの座標にさらに加算する値)
*  @param  pColorTable    色指定拡張機能、カラーテーブル渡し
*  @param  colorTableSize 色指定拡張機能、カラーテーブルのサイズ
*/
//-----------------------------------------------------------------------------
void BaseCollisionScene::DebugRegistRenderStaticActor( gfl2::math::Vector4 color, b32 onlyActiveFlag, gfl2::math::Vector3 positon, const gfl2::math::Vector4* pColorTable, u32 colorTableSize )
{
  for( u32 i=0 ; i<m_rootStaticActorRoot->GetChildCount() ; i++ )
  {
    // コンテナからメッシュ形状を持つスタティックアクターを取得
    StaticActor* pStaticActor = m_rootStaticActorRoot->GetChild(i)->SafeCast<StaticActor>();

    // アクティブになっていないアクターは登録しない
    if( onlyActiveFlag && !pStaticActor->IsActive() )
    {
      continue;
    }

    Field::Debug::DebugCollisionRenderManager* pManager = GFL_SINGLETON_INSTANCE( Field::Debug::DebugCollisionRenderManager );
    if( pManager )
    {
      pManager->RegistRenderCollisionActor( pStaticActor, color, positon, pColorTable, colorTableSize );
    }
  }
};

//---------------------------------------------------------------------------
/**
*  @brief  衝突するアクターをデバッグ表示へ登録
*
*  @param  color          コリジョン表示色
*  @param  onlyActiveFlag true:アクティブフラグが立っているアクターのみ表示する false:シーンに登録されているアクターを全て表示する
*  @param  positon        表示位置(アクターの座標にさらに加算する値)
*  @param  pColorTable    色指定拡張機能、カラーテーブル渡し
*  @param  colorTableSize 色指定拡張機能、カラーテーブルのサイズ
*/
//-----------------------------------------------------------------------------
void BaseCollisionScene::DebugRegistRenderDynamicActor( gfl2::math::Vector4 color, b32 onlyActiveFlag, gfl2::math::Vector3 positon, const gfl2::math::Vector4* pColorTable, u32 colorTableSize )
{
  for( u32 i=0 ; i<m_rootDynamicActorRoot->GetChildCount() ; i++ )
  {
    // コンテナからメッシュ形状を持つスタティックアクターを取得
    DynamicActor* pActor = m_rootDynamicActorRoot->GetChild(i)->SafeCast<DynamicActor>();

    // アクティブになっていないアクターは登録しない
    if( onlyActiveFlag && !pActor->IsActive() )
    {
      continue;
    }

    Field::Debug::DebugCollisionRenderManager* pManager = GFL_SINGLETON_INSTANCE( Field::Debug::DebugCollisionRenderManager );
    if( pManager )
    {
      pManager->RegistRenderCollisionActor( pActor, color, positon, pColorTable, colorTableSize );
    }
  }
}

#endif  //#if PM_DEBUG
