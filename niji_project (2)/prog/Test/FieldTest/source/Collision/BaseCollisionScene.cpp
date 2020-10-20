//=================================================================================================
/**
 *  @file   BaseCollisionScene.cpp
 *  @brief  ベースコリジョンシーン
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include "../../include/Collision/BaseActor.h"
#include "../../include/Collision/BaseSolver.h"
#include "../../include/Collision/BaseCollisionScene.h"
#include "../../include/Collision/StaticActor.h"
#include "../../include/Collision/DynamicActor.h"
#include <heap/include/gfl2_Heap.h>

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
BaseCollisionScene::BaseCollisionScene()
: m_pSolver(NULL)
, m_SolverUpdateCnt(2)
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
BaseCollisionScene::~BaseCollisionScene()
{
  terminate();
}

//-----------------------------------------------------------------------------
/**
*  @brief  初期化
*
*  @param  pHeap  利用するヒープ
*/
//-----------------------------------------------------------------------------
void BaseCollisionScene::setup(gfl2::gfx::IGLAllocator* pAllocator)
{
  // インスタンスノードはバッティングのためGFL_NEWを使えないので、↓の方法で対象アロケータを変更する
  gfl2::gfx::IGLAllocator* pDefaultAllocator = gfl2::gfx::GLMemory::GetAllocator();
  m_pAllocator = pAllocator;
  gfl2::gfx::GLMemory::SetAllocator(pAllocator);

  // ルートはあくまでツリー形成のためなのでベースアクター決め打ちで問題ない
  // 場所をここから変更する際は使用されるアロケータも変わってしまうことに注意してください
  m_rootStaticActorRoot  = new BaseActor();
  m_rootDynamicActorRoot = new BaseActor();

  // 場所をここから変更する際は使用されるアロケータも変わってしまうことに注意してください
  m_pSolver = createSolver();

  // 利用するアロケータを元に戻す
  gfl2::gfx::GLMemory::SetAllocator(pDefaultAllocator);  
}

//-----------------------------------------------------------------------------
/**
*  @brief  削除
*/
//-----------------------------------------------------------------------------
void BaseCollisionScene::terminate(void)
{
  if( m_rootStaticActorRoot )
  {
    delete m_rootStaticActorRoot;
  }
  if( m_rootDynamicActorRoot )
  {
    delete m_rootDynamicActorRoot;
  }
  if( m_pSolver )
  {
    delete m_pSolver;
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  コリジョン判定
*/
//-----------------------------------------------------------------------------
void BaseCollisionScene::update(void)
{
  // AABB構築

  // AABBを利用したざっくりとした判定

  // 詳細判定
  if( m_pSolver )
  {
    for( u32 i=0 ; i<m_SolverUpdateCnt ; i++ )
    {
      m_pSolver->Solved( m_rootStaticActorRoot , m_rootDynamicActorRoot );
    }
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  衝突されるアクターの作成
*
*  @return アクター
*/
//-----------------------------------------------------------------------------
StaticActor* BaseCollisionScene::createStaticActor(void)
{
  // コリジョンシステム全体で利用するアロケータを統一するために、↓の方法で対象アロケータを変更する(シーンにあるアクター用ノードのメモリ確保方法にあわせる)
  gfl2::gfx::IGLAllocator* pDefaultAllocator = gfl2::gfx::GLMemory::GetAllocator();
  gfl2::gfx::GLMemory::SetAllocator(m_pAllocator);

  StaticActor* pStaticActor = new StaticActor();
  pStaticActor->setAllocator( m_pAllocator );

  // 利用するアロケータを元に戻す
  gfl2::gfx::GLMemory::SetAllocator(pDefaultAllocator);

  return pStaticActor;
}

//----------------------------------------------------------------------------
/**
*  @brief  衝突するアクターの作成
*
*  @return アクター
*/
//-----------------------------------------------------------------------------
DynamicActor* BaseCollisionScene::createDynamicActor(void)
{
  // コリジョンシステム全体で利用するアロケータを統一するために、↓の方法で対象アロケータを変更する(シーンにあるアクター用ノードのメモリ確保方法にあわせる)
  gfl2::gfx::IGLAllocator* pDefaultAllocator = gfl2::gfx::GLMemory::GetAllocator();
  gfl2::gfx::GLMemory::SetAllocator(m_pAllocator);

  DynamicActor* pDynamicActor = new DynamicActor();
  pDynamicActor->setAllocator( m_pAllocator );

  // 利用するアロケータを元に戻す
  gfl2::gfx::GLMemory::SetAllocator(pDefaultAllocator);

  return pDynamicActor;
}

//----------------------------------------------------------------------------
/**
*  @brief  衝突されるアクターの追加
*
*  @param  pActor  コリジョンシステムに登録したいアクター
*/
//-----------------------------------------------------------------------------
void BaseCollisionScene::registStaticActor(StaticActor *pActor)
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
void BaseCollisionScene::registDynamicActor(DynamicActor *pActor)
{
  m_rootDynamicActorRoot->AddChild( pActor );
}

//----------------------------------------------------------------------------
/**
*  @brief   解決者の作成
*
*  @return  solver
*  @note    この関数をオーバーライドすることにより衝突の挙動を変更できます
*/
//-----------------------------------------------------------------------------
BaseSolver* BaseCollisionScene::createSolver(void)
{
  // 狙ってデフォルトnewを使用しています。(親クラスのm_pAllocatorから取られます)
  return new BaseSolver();
}