//==============================================================================
/**
 @file    MobiclipRenderPath.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.10.18
 @brief   mobiclip用の描画パス
 @note    gf内製コード。mobiclipとは無関係。
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>

#include <gfx/include/gfl2_GLMemory.h>
#include <gfx/include/gfl2_Texture.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

#if defined(GF_PLATFORM_CTR)
#include <gfx/include/CTR/gfl2_CTRTexture.h>
#elif defined(GF_PLATFORM_WIN32)
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTexture.h>
#endif

#include <fs/include/gfl2_Fs.h.>
#include <math/include/gfl2_math.h>

// nijiのデータのヘッダーファイルのインクルード
#include <arc_def_index/arc_def.h>
#include <arc_index/mobiclip_resource.gaix>

// nijiのインクルード
#include "MobiclipRenderPathUtil.h"
#include "MobiclipRenderPath.h"


GFL_NAMESPACE_BEGIN(movie)
GFL_NAMESPACE_BEGIN(renderer)


MobiclipRenderPath::MobiclipRenderPath(void)
  : m_devHeap(NULL),
    m_glAllocator(NULL),
    m_asyncFileManager(NULL),
    m_createState(CREATE_STATE_NONE),
    m_packBuf(NULL),
    m_packBufSize(0),
    //m_textureResourceNode(NULL),  // 使っていないのでコメントアウト。
    m_modelInstanceNode(NULL),
    m_glAllocatorForTexture(NULL),
    m_bufForTexture(NULL),
    m_texture(NULL),
    m_imageClockwiseDegreeRotationZ(0)
{
  for(u32 i=0; i<RESOURCE_TYPE_NUM; ++i)
  {
    m_binBuf[i]     = NULL;
    m_binBufSize[i] = 0;
  }

  for(u32 i=0; i<RESOURCE_TYPE_NUM; ++i)
  {
    m_resourceNode[i] = NULL;
  }
}
MobiclipRenderPath::~MobiclipRenderPath()
{
  GFL_ASSERT( this->CanDestroy() );  // 開発中に気付かせるためのASSERT

  this->destroyTexture();
  GFL_SAFE_DELETE(m_modelInstanceNode);
  for(u32 i=0; i<RESOURCE_TYPE_NUM; ++i)
  {
    GFL_SAFE_DELETE(m_resourceNode[i]);
  }
  GflHeapSafeFreeMemory(m_packBuf);
}
 
void MobiclipRenderPath::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
  if( (this->IsCreated()) && (m_texture!=NULL) )
  {
    gfl2::gfx::GFGL::SetRenderTarget( rDrawContext.m_pRenderTarget );
    gfl2::gfx::GFGL::SetDepthStencil( NULL );
 
    gfl2::gfx::GFGL::BeginScene();

    u32 displayWidth  = rDrawContext.m_DisplayWidth;
    u32 displayHeight = rDrawContext.m_DisplayHeight;
    
    gfl2::gfx::GFGL::SetViewport(0, 0, displayWidth, displayHeight);
    gfl2::gfx::GFGL::SetScissor(0, 0, displayWidth, displayHeight);

#if defined(GF_PLATFORM_CTR)
    gfl2::gfx::GFGL::FlushAllDirtyCommand();  // ViewportとSissorのコマンドを発行してくれるClearRenderTargetを呼んでいないので、自分でコマンドを発行しておく。
#endif

    gfl2::renderingengine::renderer::RenderState  renderState;
    gfl2::math::Matrix34 dummyMat34 = gfl2::math::Matrix34::GetIdentity();
    gfl2::math::Matrix44 dummyMat44 = gfl2::math::Matrix44::GetIdentity();
 
    renderState.SetProjectionMatrix( &dummyMat44 );
    renderState.SetViewMatrix( &dummyMat34 );
    renderState.SetWorldMatrix( &dummyMat34 );
 
    // 描画
    {
      gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* mate =
          m_modelInstanceNode->GetMaterialInstanceNode(0);
      gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::TextureInfo* texInfo =
          mate->GetTextureInfo(0);

      texInfo->m_Attribute.m_Rotate     = GFL_MATH_DEG_TO_RAD(90);  // 90度回転  // これで縦横が正しくなる
      texInfo->m_Attribute.m_ScaleU     = (static_cast<f32>(displayWidth)) / m_texture->GetTextureDesc().width;
      //texInfo->m_Attribute.m_ScaleV     = (static_cast<f32>(displayHeight)) / m_texture->GetTextureDesc().height;                     // 縦長に描画されていたので、m_ScaleVとm_TranslateVに
      //texInfo->m_Attribute.m_TranslateV = ( (static_cast<f32>(displayHeight)) / m_texture->GetTextureDesc().height - 1.0f ) * 2.0f;   // 渡す値を下記のようにしたら直った。
      texInfo->m_Attribute.m_ScaleV     = 1.0f;
      texInfo->m_Attribute.m_TranslateV = ( (static_cast<f32>(displayHeight)) / m_texture->GetTextureDesc().height - 1.0f );

      gfl2::renderingengine::renderer::MeshDrawTag* pTag = m_modelInstanceNode->GetMeshDrawTag( 0 );
      pTag->SetTexture( 0, m_texture );
      pTag->Draw( renderState );
    }
 
    gfl2::gfx::GFGL::SetTexture( 0, NULL );
 
    gfl2::gfx::GFGL::EndScene();
 
    gfl2::gfx::GFGL::SetRenderTarget( rDrawContext.m_pRenderTarget );
    gfl2::gfx::GFGL::SetDepthStencil( rDrawContext.m_pDepthStencil );
  }
}

void MobiclipRenderPath::StartAsyncCreate(
    gfl2::heap::HeapBase*       devHeap,
    gfl2::gfx::IGLAllocator*    glAllocator,
    gfl2::fs::AsyncFileManager* asyncFileManager
)
{
  if( m_createState == CREATE_STATE_NONE)
  {
    m_devHeap          = devHeap;
    m_glAllocator      = glAllocator;
    m_asyncFileManager = asyncFileManager;

    this->aSyncCreateBinBuf();

    m_createState = CREATE_STATE_CREATING;
  }
  else
  {
    GFL_ASSERT(0);
  }
}

void MobiclipRenderPath::StartAsyncCreateFileOpne(
    gfl2::heap::HeapBase*       devHeap,            // デバイスメモリのヒープ
    gfl2::fs::AsyncFileManager* asyncFileManager    // ファイルマネージャ
  )
{
  gfl2::heap::HeapBase* tempHeap = devHeap->GetLowerHandle();

  // オープン
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
    req.arcId       = ARCID_MOBICLIP_RESOURCE;
    req.heapForFile = tempHeap;  // すぐにクローズするので一時ヒープ
    req.heapForReq  = tempHeap;
    asyncFileManager->AddArcFileOpenReq(req);
  }
}
void MobiclipRenderPath::StartAsyncCreateFileClose(
    gfl2::heap::HeapBase*       devHeap,            // デバイスメモリのヒープ
    gfl2::fs::AsyncFileManager* asyncFileManager    // ファイルマネージャ
  )
{
  gfl2::heap::HeapBase* tempHeap = devHeap->GetLowerHandle();

  // クローズ
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
    req.arcId       = ARCID_MOBICLIP_RESOURCE;
    req.heapForReq  = tempHeap;
    asyncFileManager->AddArcFileCloseReq(req);
  }

}
void MobiclipRenderPath::SyncCreate(
    gfl2::heap::HeapBase*       devHeap,
    gfl2::gfx::IGLAllocator*    glAllocator,
    gfl2::fs::AsyncFileManager* asyncFileManager
)
{
  if( m_createState == CREATE_STATE_NONE)
  {
    m_devHeap          = devHeap;
    m_glAllocator      = glAllocator;
    m_asyncFileManager = asyncFileManager;
 
    this->syncCreateBinBuf();
    this->createResourceNode();
    this->createInstanceNode();

    // もう使わないのでNULLを入れておく
    m_devHeap         = NULL;
    m_glAllocator     = NULL;
    m_asyncFileManager = NULL;
 
    m_createState = CREATE_STATE_CREATED;
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。既に生成中or生成済み。
  }
}
bool MobiclipRenderPath::IsCreated(void) const
{
  return ( m_createState == CREATE_STATE_CREATED );
}
bool MobiclipRenderPath::CanDestroy(void) const
{
  return ( m_createState != CREATE_STATE_CREATING );
}

bool MobiclipRenderPath::UpdateAsyncCreate(void)
{
  if( m_createState == CREATE_STATE_CREATING )
  {
    if( m_asyncFileManager->IsArcFileLoadDataFinished(&m_packBuf))
    {
      this->unpackBinLink();
      this->createResourceNode();
      this->createInstanceNode();

      // もう使わないのでNULLを入れておく
      m_devHeap         = NULL;
      m_glAllocator     = NULL;
      m_asyncFileManager = NULL;

      m_createState = CREATE_STATE_CREATED;
    }
  }

  return this->IsCreated();
}

bool MobiclipRenderPath::UpdateAsyncCreateFile(gfl2::fs::AsyncFileManager* asyncFileManager)
{

    if(  asyncFileManager->IsArcFileOpenFinished(ARCID_MOBICLIP_RESOURCE)
      && asyncFileManager->IsArcFileCloseFinished(ARCID_MOBICLIP_RESOURCE)
      )
    {
      return true;
    }
    return false;
}

void MobiclipRenderPath::SetBufForTexture(
    gfl2::gfx::IGLAllocator*  glAllocator,  
    const void*               bufForTexture,
    gfl2::gfx::Pool           pool,         
    u32                       width,        
    u32                       height,       
    gfl2::gfx::Format         format,
    float                     imageClockwiseDegreeRotationZ
)
{
  m_imageClockwiseDegreeRotationZ = imageClockwiseDegreeRotationZ;
  this->destroyTexture();
  this->createTexture(
    glAllocator,  
    bufForTexture,  
    pool,           
    width,          
    height,         
    format          
  );
}
void MobiclipRenderPath::UnsetBufForTexture(void)
{
  this->destroyTexture();
}

void MobiclipRenderPath::syncCreateBinBuf(void)
{
  gfl2::heap::HeapBase* tempHeap = m_devHeap->GetLowerHandle();

  // オープン
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
    req.arcId       = ARCID_MOBICLIP_RESOURCE;
    req.heapForFile = tempHeap;  // すぐにクローズするので一時ヒープ
    req.heapForReq  = tempHeap;
    m_asyncFileManager->SyncArcFileOpen(req);
  }
  
  // ロード
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
    req.arcId       = ARCID_MOBICLIP_RESOURCE;
    req.datId       = GARC_mobiclip_resource_mobiclip_resource_pack_PACK;
    req.ppBuf       = &m_packBuf;
    req.pBufSize    = &m_packBufSize;
    req.heapForBuf  = m_devHeap;
    req.align       = 128;
    req.heapForReq  = tempHeap;
    m_asyncFileManager->SyncArcFileLoadData(req);
  }

  // クローズ
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
    req.arcId       = ARCID_MOBICLIP_RESOURCE;
    req.heapForReq  = tempHeap;
    m_asyncFileManager->SyncArcFileClose(req);
  }

  this->unpackBinLink();
}

void MobiclipRenderPath::unpackBinLink(void)
{
  // BinLinker
  {
    // niji_conv_header/mobiclip_resource/ctr/mobiclip_resource_pack.h
    // に書かれている
    // BL_IDX_MOBICLIP_RESOURCE_PACK_MAXの並び
    // とRESOURCE_TYPE_NUMの並びは同じ。
    gfl2::fs::BinLinkerAccessor binLinker(m_packBuf);
    for(s32 i=0; i<RESOURCE_TYPE_NUM; ++i)
    {
      m_binBuf[i]     = binLinker.GetData(i);
      m_binBufSize[i] = binLinker.GetDataSize(i);
    }
  }
}

void MobiclipRenderPath::aSyncCreateBinBuf(void)
{
  gfl2::heap::HeapBase* tempHeap = m_devHeap->GetLowerHandle();

  //ロード
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
    req.arcId       = ARCID_MOBICLIP_RESOURCE;
    req.datId       = GARC_mobiclip_resource_mobiclip_resource_pack_PACK;
    req.ppBuf       = &m_packBuf;
    req.pBufSize    = &m_packBufSize;
    req.heapForBuf  = m_devHeap;
    req.align       = 128;
    req.heapForReq  = tempHeap;
    m_asyncFileManager->AddArcFileLoadDataReq(req);
  }
}

void MobiclipRenderPath::createResourceNode(void)
{
  // リソースノード生成
  // model
  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData mdlData;
    mdlData.SetModelData(static_cast<c8*>(m_binBuf[RESOURCE_TYPE_MODEL]));
    m_resourceNode[RESOURCE_TYPE_MODEL] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_glAllocator, &mdlData);
  }
  // vsh, fsh
  {
    for(u32 i=RESOURCE_TYPE_VSH; i<=RESOURCE_TYPE_FSH; ++i)
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryShdData shdData;
      shdData.SetShaderData(static_cast<c8*>(m_binBuf[i]));
      m_resourceNode[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_glAllocator, &shdData);
    } 
  }
  // tex
  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryTexData texData;
    texData.SetTextureData(static_cast<c8*>(m_binBuf[RESOURCE_TYPE_TEX]));
    m_resourceNode[RESOURCE_TYPE_TEX] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_glAllocator, &texData);
  
    // テクスチャリソースノードのアドレス
    //m_textureResourceNode = MobiclipRenderPathUtil::TextureResourceNodeGetter::GetTextureResourceNode(m_resourceNode[RESOURCE_TYPE_TEX]);  // 使っていないのでコメントアウト。
  }

  // 親子付け
  {
    for(u32 i=RESOURCE_TYPE_VSH; i<=RESOURCE_TYPE_TEX; ++i)
    {
      m_resourceNode[RESOURCE_TYPE_MODEL]->AddChild(m_resourceNode[i]);
      m_resourceNode[i] = NULL;  // 子ノードは親ノードが解放されるときに一緒に解放されるので、二重解放しないようにNULLを入れておく。
    } 
  }
}
void MobiclipRenderPath::createInstanceNode(void)
{
  // インスタンスノード生成
  // model
  m_modelInstanceNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode(m_glAllocator, m_resourceNode[RESOURCE_TYPE_MODEL]);
  {
    gfl2::renderingengine::scenegraph::instance::DrawableNode::UpdateDrawTagInfo info;
    m_modelInstanceNode->UpdateDrawTag(info);
  }
  gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild(m_modelInstanceNode);
}

void MobiclipRenderPath::createTexture(
    gfl2::gfx::IGLAllocator*  glAllocator,  
    const void*               bufForTexture, 
    gfl2::gfx::Pool           pool,          
    u32                       width,         
    u32                       height,        
    gfl2::gfx::Format         format         
)
{
  m_glAllocatorForTexture = glAllocator;
  m_bufForTexture         = bufForTexture;

  gfl2::gfx::Texture::TextureDesc desc;
  desc.type        = gfl2::gfx::TextureType::Image;
  desc.usage       = gfl2::gfx::Usage::None;  // Tiledか？→石黒さん曰く「直接テクスチャに描画を行わないのであればNoneで問題ない」とのこと。
  desc.pool        = pool;
  desc.width       = width;
  desc.height      = height;
  desc.depth       = 1;  // 3DSでは使用していないらしいが、将来的に流用されるコードなら1にしておくのがいいらしいので、1にした。
  desc.mipLevels   = 1;
  desc.format      = format;
  desc.multiSample = gfl2::gfx::MultiSample::None;

  m_texture = gfl2::gfx::GFGL::CreateTexture(m_glAllocatorForTexture, width, height, 1, gfl2::gfx::Usage::None, format, pool);
  m_texture->Update(m_bufForTexture);
}
void MobiclipRenderPath::destroyTexture(void)
{
  if(m_texture)
  {
    GFL_SAFE_DELETE(m_texture);
    m_glAllocatorForTexture = NULL;
    m_bufForTexture         = NULL;
    m_texture               = NULL;
  }
}


GFL_NAMESPACE_END(renderer)
GFL_NAMESPACE_END(movie)

