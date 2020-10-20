//===================================================================
/**
 * @file    SkyboxPartsBaseBase.h
 * @brief   天球オブジェクト基底クラス
 * @author  saita_kazuki
 * @date    2015.05.07
 */
//===================================================================

#include "./SkyboxPartsBase.h"

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNodeFactory.h>


// debug
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>


GFL_NAMESPACE_BEGIN( System )
GFL_NAMESPACE_BEGIN( Skybox )

/**
 * @brief コンストラクタ
 */
PartsBase::PartsBase( gfl2::heap::HeapBase* pParent, u32 heapSize, c8* heapName, u32 modelWorkSize, u32 motionWorkSize, bool isAllocateByVRAM, System::nijiAllocator::VramMallocMode vramMallocMode) :
  m_pHeap( NULL),
  m_pGLAllocator( NULL),
  m_modelNum( 0),
  m_isEnable( false),
  m_isUpdateAnimation( false),
  m_allTimeAnimationFrame( 0.0f),
  m_ppModel( NULL),
  m_ppModelResourceRootNode( NULL),
  m_ppMotionResourceRootNode( NULL),
  m_isAllocateByVRAM( isAllocateByVRAM),
  m_vramMallocMode( vramMallocMode)
{
  m_pHeap = this->CreateLocalHeap( pParent, heapSize, heapName);
  m_pGLAllocator = GFL_NEW( m_pHeap) System::nijiAllocator( m_pHeap, System::nijiAllocator::APPLICATION, m_vramMallocMode );
  this->CreateWorks( modelWorkSize, motionWorkSize);
}

/**
 * @brief デストラクタ
 */
PartsBase::~PartsBase()
{
  this->DeleteWorks();
  GFL_SAFE_DELETE( m_pGLAllocator);
  this->DeleteLocalHeap();

}

/**
 * @brief 1日アニメーションフレーム更新
 */
void PartsBase::UpdateAllTimeAnimationFrame( f32 frame)
{
  m_allTimeAnimationFrame = frame;
  this->RequestUpdateAnimation();
}

/**
 * @brief アニメーション更新リクエスト
 */
void PartsBase::RequestUpdateAnimation()
{
  m_isUpdateAnimation = true;
}

/**
 * @brief 表示設定 全モデルに設定版
 * @param isVisible trueで表示
 */
void PartsBase::SetVisibleAll( bool isVisible)
{
  u32 num = this->GetModelNum();
  for( u32 i = 0; i < num; ++i)
  {
    this->SetVisible( i, isVisible);
  }
}

/**
 * @brief 表示設定
 * @param index モデル番号
 * @param isVisible trueで表示
 */
void PartsBase::SetVisible( u32 index, bool isVisible)
{
  m_ppModel[index]->SetVisible( isVisible);
}

/**
 * @brief 表示されているか 全モデルをチェック版
 * @retval true 全て表示
 * @retval false どれかひとつでも非表示
 */
bool PartsBase::IsVisibleAll()
{
  u32 num = this->GetModelNum();
  for( u32 i = 0; i < num; ++i)
  {
    if( this->IsVisible( i) == false)
    {
      return false;
    }
  }
  return true;
}

/**
 * @brief 表示されているか
 * @param index モデル番号
 * @retval true 表示
 * @retval false 非表示
 */
bool PartsBase::IsVisible( u32 index)
{
  return GFL_BOOL_CAST( m_ppModel[index]->IsVisible());
}

/**
 * @brief モデルの取得
 * @param index モデル番号
 */
poke_3d::model::BaseModel* PartsBase::GetModel( u32 index) const
{
  return m_ppModel[index];
}

/**
 * @brief モデル数の取得
 * @return 生成したモデル数
 */
u32 PartsBase::GetModelNum() const
{
  return m_modelNum;
}

/**
 * @brief 有効無効フラグの設定
 * @param isEnable trueで有効
 * @note falseにするとUpdateが実行されない
 */
void PartsBase::SetEnable( bool isEnable)
{
  if( m_isEnable != isEnable)
  {
    this->SetVisibleAll( isEnable);
    m_isEnable = isEnable;
  }
}

/**
 * @brief 有効無効フラグの取得
 * @retval true 有効
 * @retval false 無効
 */
bool PartsBase::IsEnable()
{
  return m_isEnable;
}

/**
 * @brief ローカルヒープ生成
 * @param pParent 親ヒープ
 * @param size サイズ
 * @param name ヒープ名
 */
gfl2::heap::HeapBase* PartsBase::CreateLocalHeap( gfl2::heap::HeapBase* pParent, u32 size, c8* name)
{
  return GFL_CREATE_LOCAL_HEAP_NAME(
    pParent,
    size,
    gfl2::heap::HEAP_TYPE_EXP,
    false,
    name
    );
}

/**
 * @brief ローカルヒープ削除
 */
void PartsBase::DeleteLocalHeap()
{
  if( m_pHeap)
  {
    GFL_DELETE_HEAP( m_pHeap);
    m_pHeap = NULL;
  }
}

/**
 * @brief ワーク生成
 * @param modelNum モデルを生成する数
 * @param motionNum モーションを生成する数
 */
void PartsBase::CreateWorks( u32 modelNum,  u32 motionNum)
{
  // モデルは必ずある
  m_ppModel = GFL_NEW_ARRAY( m_pHeap) poke_3d::model::BaseModel*[ modelNum ];
  m_ppModelResourceRootNode = GFL_NEW_ARRAY( m_pHeap) gfl2::renderingengine::scenegraph::resource::ResourceNode*[ modelNum ];

  // モーションはチェック
  if( motionNum > 0)
  {
    m_ppMotionResourceRootNode = GFL_NEW_ARRAY( m_pHeap) gfl2::renderingengine::scenegraph::resource::ResourceNode*[ motionNum ];
  }
}

/**
 * @brief ワーク破棄
 */
void PartsBase::DeleteWorks()
{
  GFL_SAFE_DELETE_ARRAY( m_ppMotionResourceRootNode);
  GFL_SAFE_DELETE_ARRAY( m_ppModelResourceRootNode);
  GFL_SAFE_DELETE_ARRAY( m_ppModel);
}

/**
 * @brief モデル生成
 * @param pModelData モデルリソースノード
 */
poke_3d::model::BaseModel* PartsBase::CreateModel( gfl2::renderingengine::scenegraph::resource::ResourceNode* pModelResourceNode)
{
  poke_3d::model::BaseModel* pModel = GFL_NEW( m_pHeap) poke_3d::model::BaseModel;
  pModel->Create( m_pGLAllocator, m_pHeap, pModelResourceNode);

  m_modelNum++;

  return pModel;
}

/**
 * @brief モデルリソースノード生成
 * @param pData モデルパックバイナリデータ
 * @param isAllocateByVRAM trueでテクスチャをVRAMに乗せる
 */
gfl2::renderingengine::scenegraph::resource::ResourceNode* PartsBase::CreateModelResourceNode( void* pData)
{
  // テクスチャリソースファクトリを取得する
  gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory* pTextureResourceNodeFactory = 
    static_cast<gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory*>(
      gfl2::renderingengine::scenegraph::resource::ResourceManager::GetResourceNodeFactory(
        gfl2::renderingengine::scenegraph::resource::IResourceData::Type(gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory::GetFactoryTypeName())
        ));

  gfl2::gfx::Pool oldPool = pTextureResourceNodeFactory->GetPool();

  if( m_isAllocateByVRAM)
  {
    // vramから確保に変更
    pTextureResourceNodeFactory->SetPool( this->GetPool( m_vramMallocMode ) );
  }

  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
  resourceData.SetModelData( static_cast<c8*>( pData));

  gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode;
  pResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pGLAllocator, &resourceData);

  if( m_isAllocateByVRAM)
  {
    // 確保先を戻す
    pTextureResourceNodeFactory->SetPool(oldPool);
  }

  return pResourceNode;
}

/**
 * @brief モーションリソースノード生成
 * @param pData モーションバイナリデータ
 */
gfl2::renderingengine::scenegraph::resource::ResourceNode* PartsBase::CreateMotionResourceNode( void* pData)
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
  resourceData.SetMotionData( static_cast<c8*>( pData));
  return gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pGLAllocator, &resourceData);
}

/**
 * @brief アニメーションのアタッチ
 * @param pModel モーションをアタッチするモデル
 * @param pMotionResourceNode モデルにアタッチしたいモーションリソースノード
 * @param isLoop trueでループ設定
 * @param slotIndex スロット番号
 * @param animationBit アニメーションの種類を示すビットフラグ
 */
void PartsBase::AttachAnimation( poke_3d::model::BaseModel* pModel, gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionResourceNode, bool isLoop, u32 slotIndex, poke_3d::model::BaseModel::AnimationBit animationBit)
{
  pModel->ChangeAnimationByResourceNode( pMotionResourceNode, slotIndex, animationBit);
  pModel->SetAnimationLoop( isLoop, slotIndex, animationBit);
}

/**
 * @brief VRAM領域指定の取得
 */
gfl2::gfx::Pool PartsBase::GetPool( System::nijiAllocator::VramMallocMode vramMallocMode ) const
{
  switch( vramMallocMode)
  {
  case System::nijiAllocator::VRAM_MALLOC_A:
    return gfl2::gfx::Pool::LocalA;
  case System::nijiAllocator::VRAM_MALLOC_B:
    return gfl2::gfx::Pool::LocalB;
  case System::nijiAllocator::VRAM_MALLOC_AB:
    GFL_ASSERT_STOP( 0); // @fix 開発中に気付くため。gflib2側でTextureResourceNodeとCTRSurfaceで意味の合わない実装となっているので非対応とする
    return gfl2::gfx::Pool::Managed;
  }
  GFL_ASSERT( 0);
  return gfl2::gfx::Pool::Default;
}

GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( System )

