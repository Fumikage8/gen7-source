//==============================================================================
/**
 @file    gfl2_BloomRenderPath.h
 @author  kawada_koji@gamefreak.co.jp(CTR版)
 @author  ikeuchi_yuya@gamefreak.co.jp(Win32版)
 @date    2015.05.27
 @brief   ブルームの描画パス
 @note    ブルーム(bloom)と言ったり、グレア(glare)と言ったりします。
 */
//==============================================================================


#if defined(GF_PLATFORM_CTR)


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <fs/include/gfl2_Fs.h>
#include <util/include/gfl2_std_string.h>
#include <util/include/gfl2_GLHeapAllocator.h>
#include <math/include/gfl2_MathTriangular.h>
#include <thread/include/gfl2_ThreadStatic.h>

#include <gfx/include/gfl2_GLMemory.h>
#include <gfx/include/gfl2_Surface.h>
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

// poke_3d_libのインクルード
#include <renderer/include/gfl2_BloomRenderPath.h>
#include "gfl2_BloomRenderPathUtil.h"

// nijiのデータのヘッダーファイルのインクルード
#include <arc_def_index/arc_def.h>  // poke_3d_lib内なのにnijiのarcファイルを使ってしまっている。ごめんなさい。
#include <arc_index/bloom_resource.gaix>
//#include <niji_conv_header/bloom_resource/ctr/bloom_resource_pack.h>  同じ定義ができてしまっているのでインクルードをやめた。




GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(renderer)


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//! @brief  ブルームの描画パスのインプリメント
class BloomRenderPath::BloomRenderPathImpl
{
  GFL_FORBID_COPY_AND_ASSIGN(BloomRenderPathImpl);

private: 
  //! @brief  生成状態
  enum CreateState
  {
    CREATE_STATE_NONE,      //!< 生成していない
    CREATE_STATE_CREATING,  //!< 生成中
    CREATE_STATE_CREATED,   //!< 生成済み
  };

  //! @brief  リソース
  enum
  {
    RESOURCE_HIGH_LUMINANCE_MODEL,
    RESOURCE_HIGH_LUMINANCE_VSH,
    RESOURCE_HIGH_LUMINANCE_FSH,
    RESOURCE_HIGH_LUMINANCE_TEX,
    RESOURCE_BOARD_MODEL,
    RESOURCE_BOARD_VSH,
    RESOURCE_BOARD_FSH,
    RESOURCE_BOARD_TEX,
    RESOURCE_SMALL_TEX_RGBA0,
    RESOURCE_SMALL_TEX_RGBA1,
    RESOURCE_NUM
  };
  //! @brief  テクスチャリソース
  enum
  {
    TEXTURE_RESOURCE_HIGH_LUMINANCE_TEX,
    TEXTURE_RESOURCE_BOARD_TEX,
    TEXTURE_RESOURCE_SMALL_TEX_RGBA0,
    TEXTURE_RESOURCE_SMALL_TEX_RGBA1,
    TEXTURE_RESOURCE_NUM
  };

  //! @brief  板ポリゴンのリソース
  enum
  {
    BOARD_RESOURCE_BOARD_MODEL,
    BOARD_RESOURCE_BOARD_VSH,
    BOARD_RESOURCE_BOARD_FSH,
    BOARD_RESOURCE_BOARD_TEX,
    BOARD_RESOURCE_NUM
  };
  //! @brief  板ポリゴン
  struct Board
  {
    void* binBuf[BOARD_RESOURCE_NUM];
    gfl2::renderingengine::scenegraph::resource::ResourceNode*              resourceNode[BOARD_RESOURCE_NUM];
    const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* textureResourceNode;  // resourceNode[BOARD_RESOURCE_BOARD_TEX]内のテクスチャリソースノードのアドレスを覚えておくだけ
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*         modelInstanceNode;
  };

public:
  BloomRenderPathImpl(void);
  virtual ~BloomRenderPathImpl();

public:
  void StartAsyncCreate(
    gfl2::heap::HeapBase*       heapForImpl,                                   // デバイスメモリのヒープ。
    gfl2::heap::HeapBase*       heapForTemp,                                   // 一時的に使用するヒープ。普通のメモリでもデバイスメモリでもどちらでもよい。
    gfl2::gfx::IGLAllocator*    allocatorForHighLuminanceRenderTargetTexture,  // VRAM限定。
    gfl2::gfx::IGLAllocator*    allocatorForHighLuminanceCopyTexture,          // VRAMかデバイスメモリ。Descriptionに書いたhighLuminanceCopyNumMax個テクスチャを作成します 
    gfl2::fs::AsyncFileManager* asyncFileManager,                              // ファイルマネージャ。                                                                     
    const BloomRenderPath::Description& desc,  // 呼び出し元はdescインスタンスを破棄してもいい(descに書かれているポインタが指すものは破棄しないように)
    BloomRenderPath::Parameter*         param  // paramに対する処理はBloomRenderPathImplで行う。呼び出し元とポインタを共有しているので、呼び出し元にも反映される。
  );
  
  void SyncCreate(
    gfl2::heap::HeapBase*       heapForImpl,                                   // デバイスメモリのヒープ。
    gfl2::heap::HeapBase*       heapForTemp,                                   // 一時的に使用するヒープ。普通のメモリでもデバイスメモリでもどちらでもよい。
    gfl2::gfx::IGLAllocator*    allocatorForHighLuminanceRenderTargetTexture,  // VRAM限定。
    gfl2::gfx::IGLAllocator*    allocatorForHighLuminanceCopyTexture,          // VRAMかデバイスメモリ。Descriptionに書いたhighLuminanceCopyNumMax個テクスチャを作成します 
    gfl2::fs::AsyncFileManager* asyncFileManager,                              // ファイルマネージャ。                                                                     
    const BloomRenderPath::Description& desc,  // 呼び出し元はdescインスタンスを破棄してもいい(descに書かれているポインタが指すものは破棄しないように)
    BloomRenderPath::Parameter*         param  // paramに対する処理はBloomRenderPathImplで行う。呼び出し元とポインタを共有しているので、呼び出し元にも反映される。
  );

  bool IsCreated(void) const;

  void Update(void);

  void SetDrawEnvNode(const gfl2::renderingengine::scenegraph::instance::DrawEnvNode* drawEnvNode);
  void SetAlphaMaskTexture(const gfl2::gfx::Texture* alphaMaskTex);

  void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
  bool ExecuteForHighLuminanceRenderTarget( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext, bool executeContinue );
  bool ExecuteForHighLuminanceCopy( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext, bool executeContinue );
  bool ExecuteForBoard( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext, bool executeContinue );

private:
  //! @name  Create
  //@{
  void updateAsyncCreate(void);
  
  //! @brief  バイナリをバッファに読み込む(非同期版開始)
  //! @note  生成するもの
  //!        m_binBuf
  //!        m_binBufSize
  void startAsyncCreateBinBuf(void);  // 呼び終わったら、外でm_createStateを変更して下さい。
  //! @brief  バイナリをバッファに読み込む(非同期版更新。trueを返したら生成済み)
  //! @note  生成するもの
  //!        m_binBuf
  //!        m_binBufSize
  bool updateAsyncCreateBinBuf(void);  // trueを返したら、外でm_createStateを変更して下さい。

  //! @brief  バイナリをバッファに読み込む
  //! @note  生成するもの
  //!        m_binBuf
  //!        m_binBufSize
  void syncCreateBinBuf(void);

  //! @brief  Implのメンバを生成する
  //! @note  生成するもの
  //!        m_allocatorForImpl
  void createImpl(void);
      //! @brief  boardを生成する
      //! @note  生成するもの
      //!        boardNum
      //!        boardArray
      void createBoard(void);
      //! @brief  リソース、インスタンスを生成する
      //! @note  生成するもの
      //!        m_resourceNode
      //!        m_textureResourceNode
      //!        m_highLuminanceModelInstanceNode
      void createResourceInstance(void);
      //! @brief  highLuminanceを生成する
      //! @note  生成するもの
      //!        m_highLuminanceCopyCount
      //!        m_highLuminanceBoardTexture
      //!        m_highLuminanceRenderTargetTexture
      //!        m_highLuminanceCopyTextureArray
      void createHighLuminance(void);
      //! @breif  ソースを扱えるようにするために必要なものを生成する
      //! @note  生成するもの
      //!        m_sourceSurfaceNum
      //!        m_sourceSurfaceArray
      //!        m_sourceTextureNum
      //!        m_sourceTextureArray
      void createSource(void);

  //! @brief  テクスチャリソースノードを生成する
  void createTextureResourceNode(
    gfl2::renderingengine::scenegraph::resource::ResourceNode** resourceNode,                      // [out]
    const gfl2::renderingengine::scenegraph::resource::TextureResourceNode** textureResourceNode,  // [out]
    void* binBuf                                                                                   // [in]
  );
  //! @brief  モデルインスタンスノードを生成する(それに必要なリソースノードも生成する)
  //! @note  子ノードにしたものは生成していたとしてもNULLを入れて返す
  void createModelInstanceNode(
    gfl2::renderingengine::scenegraph::resource::ResourceNode**      resourceNodeOfModel,  // [out]
    gfl2::renderingengine::scenegraph::resource::ResourceNode**      resourceNodeOfVsh,    // [out]
    gfl2::renderingengine::scenegraph::resource::ResourceNode**      resourceNodeOfFsh,    // [out]
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode** modelInstanceNode,    // [out]
    void* binBufOfModel,                                                                   // [in]
    void* binBufOfVsh,                                                                     // [in]
    void* binBufOfFsh,                                                                     // [in]
    gfl2::renderingengine::scenegraph::resource::ResourceNode** resourceNodeOfTex          // [in,out]
  );
  //@}
  
  //! @name  Destroy
  //@{
  void destroyBinBuf(void);
  void destroyImpl(void);
      void destroyBoard(void);
      void destroyResourceInstance(void);
      void destroyHighLuminance(void);
      void destroySource(void);
  //@}

  //! @brief  highLuminanceの設定の更新
  void updateHighLuminanceSetting(void);
  //! @brief  boardの設定の更新
  void updateBoardSetting(void);

private:
  // 外部から受け取るもの
  gfl2::heap::HeapBase*        m_heapForImpl;                                   // デバイスメモリのヒープ。
  gfl2::heap::HeapBase*        m_heapForTemp;                                   // 一時的に使用するヒープ。普通のメモリでもデバイスメモリでもどちらでもよい。
  gfl2::gfx::IGLAllocator*     m_allocatorForHighLuminanceRenderTargetTexture;  // VRAM限定。
  gfl2::gfx::IGLAllocator*     m_allocatorForHighLuminanceCopyTexture;          // VRAMかデバイスメモリ。Descriptionに書いたhighLuminanceCopyNumMax個テクスチャを作成します。
  gfl2::fs::AsyncFileManager*  m_asyncFileManager;                              // ファイルマネージャ。
  BloomRenderPath::Description m_desc;
  BloomRenderPath::Parameter*  m_param;
 
  const gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_drawEnvNode;   // 設定しないときはNULL
                                                                                   // 渡したDrawEnvNodeの値が変わると、それに追従して本クラス内の値も変わります(Updateを呼ぶ必要あり)。
                                                                                   // つまり、渡したDrawEnvNodeがアニメをしているときは、ブルームがアニメをします(Updateを呼ぶ必要あり)。
  const gfl2::gfx::Texture*                                       m_alphaMaskTex;  // アルファマスクテクスチャ(白のところにブルームが掛かり、黒のところにブルームが掛からない)
                                                                                   // 設定しないときはNULL(白のテクスチャを渡しているのと同じになる)
                                                                                   // w512, h256の左下にある絵を等倍で使用

  // 生成状態
  CreateState  m_createState;

  // バイナリ
  void*   m_packBuf;
  size_t  m_packBufSize;
  void*   m_binBuf[RESOURCE_NUM];
  size_t  m_binBufSize[RESOURCE_NUM];

  // アロケータ
  gfl2::gfx::IGLAllocator*  m_allocatorForImpl;

  // board
  u32       m_boardNum;
  Board*    m_boardArray;  // m_boardArrayの要素数はm_boardNum個。

  // リソース、インスタンス
  gfl2::renderingengine::scenegraph::resource::ResourceNode*              m_resourceNode[RESOURCE_NUM];
  const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* m_textureResourceNode[TEXTURE_RESOURCE_NUM];  // m_resourceNode[i]内のテクスチャリソースノードのアドレスを覚えておくだけ
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*         m_highLuminanceModelInstanceNode;

  // highLuminance
  u32                 m_highLuminanceCopyCount;     // Descriptionに書いたhighLuminanceCopyNumMax以下。
                                                    // 0のときm_highLuminanceRenderTargetTextureを使い
                                                    // 1以上のときm_highLuminanceCopyTexture[m_highLuminanceCopyCount-1]を使う。
  gfl2::gfx::Texture* m_highLuminanceBoardTexture;  // boardに貼り付けるテクスチャのポインタを覚えておく。
                                                    // m_highLuminanceCopyCountの値によって
                                                    // m_highLuminanceRenderTargetTextureか
                                                    // m_highLuminanceCopyTexture[m_highLuminanceCopyCount-1]を
                                                    // 指す。

  gfl2::gfx::Texture*  m_highLuminanceRenderTargetTexture;
  gfl2::gfx::Texture** m_highLuminanceCopyTextureArray;     // 要素数はDescriptionに書いたhighLuminanceCopyNumMax個。
  GLenum m_highLuminanceTextureFormat;

  // ブルームを掛けられるソース
  u32                           m_sourceSurfaceNum;
  const gfl2::gfx::Surface**    m_sourceSurfaceArray;
  u32                           m_sourceTextureNum;
  gfl2::gfx::Texture**          m_sourceTextureArray;
};


BloomRenderPath::BloomRenderPathImpl::BloomRenderPathImpl(void)
  : m_heapForImpl(NULL),
    m_heapForTemp(NULL),
    m_allocatorForHighLuminanceRenderTargetTexture(NULL),
    m_allocatorForHighLuminanceCopyTexture(NULL),
    m_asyncFileManager(NULL),
    m_desc(),
    m_param(NULL),
    m_drawEnvNode(NULL),
    m_alphaMaskTex(NULL),
    m_createState(CREATE_STATE_NONE),
    m_packBuf(NULL),
    m_packBufSize(0),
    m_allocatorForImpl(NULL),
    m_boardNum(0),
    m_boardArray(NULL),
    m_highLuminanceModelInstanceNode(NULL),
    m_highLuminanceCopyCount(0),
    m_highLuminanceBoardTexture(NULL),
    m_highLuminanceRenderTargetTexture(NULL),
    m_highLuminanceCopyTextureArray(NULL),
    m_highLuminanceTextureFormat(0),
    m_sourceSurfaceNum(0),
    m_sourceSurfaceArray(NULL),
    m_sourceTextureNum(0),
    m_sourceTextureArray(NULL)
{
  for(s32 i=0; i<RESOURCE_NUM; ++i)
  {
    m_binBuf[i]       = NULL;
    m_binBufSize[i]   = 0;
    m_resourceNode[i] = NULL;
  }
  for(s32 i=0; i<TEXTURE_RESOURCE_NUM; ++i)
  {
    m_textureResourceNode[i] = NULL;
  }
}
BloomRenderPath::BloomRenderPathImpl::~BloomRenderPathImpl()
{
  this->destroyImpl();
  this->destroyBinBuf();

  // 初期化
  m_heapForImpl = NULL;
  m_heapForTemp = NULL;
  m_allocatorForHighLuminanceRenderTargetTexture = NULL;
  m_allocatorForHighLuminanceCopyTexture = NULL;
  m_asyncFileManager = NULL;
  m_desc = BloomRenderPath::Description();
  m_param = NULL;

  m_drawEnvNode = NULL;
  m_alphaMaskTex = NULL;

  m_createState = CREATE_STATE_NONE;
}

void BloomRenderPath::BloomRenderPathImpl::StartAsyncCreate(
  gfl2::heap::HeapBase*               heapForImpl,
  gfl2::heap::HeapBase*               heapForTemp,
  gfl2::gfx::IGLAllocator*            allocatorForHighLuminanceRenderTargetTexture,
  gfl2::gfx::IGLAllocator*            allocatorForHighLuminanceCopyTexture,
  gfl2::fs::AsyncFileManager*         asyncFileManager,                                                         
  const BloomRenderPath::Description& desc,
  BloomRenderPath::Parameter*         param
)
{
  if(m_createState == CREATE_STATE_NONE)
  {
    m_heapForImpl                                   = heapForImpl;                                 
    m_heapForTemp                                   = heapForTemp;
    m_allocatorForHighLuminanceRenderTargetTexture  = allocatorForHighLuminanceRenderTargetTexture;
    m_allocatorForHighLuminanceCopyTexture          = allocatorForHighLuminanceCopyTexture;        
    m_asyncFileManager                              = asyncFileManager;                            
    m_desc                                          = desc;
    m_param                                         = param;
 
    this->startAsyncCreateBinBuf();
    m_createState = CREATE_STATE_CREATING;
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。二重生成。
  }
}
void BloomRenderPath::BloomRenderPathImpl::updateAsyncCreate(void)
{
  if(m_createState == CREATE_STATE_CREATING)
  {
    if(this->updateAsyncCreateBinBuf())
    {
      this->createImpl();
      m_createState = CREATE_STATE_CREATED;
      
      this->Update();  // 初回反映するためにUpdateを呼んでおく
    }
  }
}

void BloomRenderPath::BloomRenderPathImpl::SyncCreate(
  gfl2::heap::HeapBase*               heapForImpl,                                 
  gfl2::heap::HeapBase*               heapForTemp,
  gfl2::gfx::IGLAllocator*            allocatorForHighLuminanceRenderTargetTexture,
  gfl2::gfx::IGLAllocator*            allocatorForHighLuminanceCopyTexture,        
  gfl2::fs::AsyncFileManager*         asyncFileManager,
  const BloomRenderPath::Description& desc,
  BloomRenderPath::Parameter*         param
)
{
  if(m_createState == CREATE_STATE_NONE)
  {
    m_heapForImpl                                   = heapForImpl;                                 
    m_heapForTemp                                   = heapForTemp;
    m_allocatorForHighLuminanceRenderTargetTexture  = allocatorForHighLuminanceRenderTargetTexture;
    m_allocatorForHighLuminanceCopyTexture          = allocatorForHighLuminanceCopyTexture;        
    m_asyncFileManager                              = asyncFileManager;                            
    m_desc                                          = desc;
    m_param                                         = param;
 
    this->syncCreateBinBuf();
    m_createState = CREATE_STATE_CREATING;
    
    this->createImpl();
    m_createState = CREATE_STATE_CREATED;
    
    this->Update();  // 初回反映するためにUpdateを呼んでおく
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。二重生成。
  }
}

bool BloomRenderPath::BloomRenderPathImpl::IsCreated(void) const
{
  return ( m_createState == CREATE_STATE_CREATED );
}

void BloomRenderPath::BloomRenderPathImpl::Update(void)
{
  if(this->IsCreated())
  {
    // DrawEnvNodeの内容をParameterに反映する
    if(m_drawEnvNode)
    {
      m_param->enable    = m_drawEnvNode->GetGlareEnable();
      m_param->reduceNum = m_drawEnvNode->GetGlareReduceNum();
      m_param->strength  = m_drawEnvNode->GetGlareStrength();
      m_param->range     = m_drawEnvNode->GetGlareRange();
      m_param->intensity = m_drawEnvNode->GetGlareIntensity();
      m_param->weight    = m_drawEnvNode->GetGlareWeight();
    }
 
    // 更新
    this->updateHighLuminanceSetting();
    this->updateBoardSetting();
  }
  else
  {
    this->updateAsyncCreate();
  }
}

void BloomRenderPath::BloomRenderPathImpl::SetDrawEnvNode(const gfl2::renderingengine::scenegraph::instance::DrawEnvNode* drawEnvNode)
{
  m_drawEnvNode = drawEnvNode;
}

void BloomRenderPath::BloomRenderPathImpl::SetAlphaMaskTexture(const gfl2::gfx::Texture* alphaMaskTex)
{
  m_alphaMaskTex = alphaMaskTex;
}

void BloomRenderPath::BloomRenderPathImpl::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
  if(this->IsCreated() && m_param->enable)
  {
    bool executeContinue = true;
    executeContinue = this->ExecuteForHighLuminanceRenderTarget(rDrawContext, executeContinue);
    executeContinue = this->ExecuteForHighLuminanceCopy(rDrawContext, executeContinue);
    executeContinue = this->ExecuteForBoard(rDrawContext, executeContinue);
  }
}

bool BloomRenderPath::BloomRenderPathImpl::ExecuteForHighLuminanceRenderTarget( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext, bool executeContinue )
{
  if(!executeContinue) return executeContinue;

  if(m_desc.sourceType == SOURCE_TYPE_FRAME_BUFFER)
  {
    const gfl2::gfx::Surface* sourceSurface = NULL;
    gfl2::gfx::Texture*       sourceTexture = NULL;
    for(s32 i=0; i<gfl2::gfx::CtrDisplayNo::NumberOf; ++i)
    {
      if(m_sourceSurfaceArray[i] == rDrawContext.m_pRenderTarget)
      {
        sourceSurface = m_sourceSurfaceArray[i]; 
        sourceTexture = m_sourceTextureArray[i]; 
        break;
      }
    }

    if( sourceSurface )
    {
      gfl2::gfx::GFGL::SetRenderTarget( m_highLuminanceRenderTargetTexture->GetSurface() );
      gfl2::gfx::GFGL::SetDepthStencil( NULL );
        
      gfl2::gfx::GFGL::BeginScene();

      // クリア
      u32 bufWidth  = m_highLuminanceRenderTargetTexture->GetSurface()->GetSurfaceDesc().bufferWidth;
      u32 bufHeight = m_highLuminanceRenderTargetTexture->GetSurface()->GetSurfaceDesc().bufferHeight;

      gfl2::gfx::GFGL::SetViewport(0, 0, bufHeight, bufWidth);  // 縦横ひっくり返った値が入っているので、ひっくり返しスクリーンの縦横と合わせる。
      gfl2::gfx::GFGL::SetScissor(0, 0, bufHeight, bufWidth);   // 例: スクリーンサイズが(w400,h240)だったら、バッファサイズは(w512,h256)になっている。

      gfl2::gfx::GFGL::ClearRenderTarget( gfl2::gfx::Color(0.0f,0.0f,0.0f,1.0f) );

      // 設定
      u32 displayWidth  = rDrawContext.m_DisplayWidth;
      u32 displayHeight = rDrawContext.m_DisplayHeight;

      u32 highLuminanceDisplayWidth  =  displayWidth / m_desc.highLuminanceRenderTargetReciprocalScale;
      u32 highLuminanceDisplayHeight = displayHeight / m_desc.highLuminanceRenderTargetReciprocalScale;

      gfl2::gfx::GFGL::SetViewport(0, 0, highLuminanceDisplayWidth, highLuminanceDisplayHeight);  // スクリーンサイズ。縦横そのまま。例: (w400,h240)
      gfl2::gfx::GFGL::SetScissor(0, 0, highLuminanceDisplayWidth, highLuminanceDisplayHeight);
     
      gfl2::gfx::GFGL::FlushAllDirtyCommand();  // ViewportとSissorのコマンドを発行してくれるClearRenderTargetを呼んでいないので、自分でコマンドを発行しておく。

      gfl2::renderingengine::renderer::RenderState  renderState;
      gfl2::math::Matrix34 dummyMat34 = gfl2::math::Matrix34::GetIdentity();
      gfl2::math::Matrix44 dummyMat44 = gfl2::math::Matrix44::GetIdentity();
  
      renderState.SetProjectionMatrix( &dummyMat44 );
      renderState.SetViewMatrix( &dummyMat34 );
      renderState.SetWorldMatrix( &dummyMat34 );
  
      // 描画
      {
        const gfl2::gfx::Texture* alphaMaskTex = m_alphaMaskTex;  // アルファマスクテクスチャを設定する
        if(alphaMaskTex == NULL) alphaMaskTex = m_textureResourceNode[TEXTURE_RESOURCE_SMALL_TEX_RGBA1]->GetTexture();

        gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* mate = NULL;
        gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::TextureInfo* texInfo = NULL;

        mate = m_highLuminanceModelInstanceNode->GetMaterialInstanceNode(0);
        {
          texInfo = mate->GetTextureInfo(0);
         
          // 例: スクリーンサイズが(w400,h240)、ソースサイズが(w256,h512)のとき
          texInfo->m_Attribute.m_ScaleU = (static_cast<f32>(displayHeight)) / sourceSurface->GetSurfaceDesc().bufferWidth *(sourceSurface->GetSurfaceDesc().bufferWidth/(static_cast<f32>(displayHeight)));
              // 例: 240/256 *(256/240)  // 画像が上にはみ出ていて全部画面におさまっていないが、「*(256/240)」をすることではみ出ていた分をおさめることができる。  // 正方形テクスチャで試していないです。
          texInfo->m_Attribute.m_ScaleV = (static_cast<f32>(displayWidth)) / sourceSurface->GetSurfaceDesc().bufferHeight;  // 例: 400/512
        }
        {
          texInfo = mate->GetTextureInfo(1);

          /*
          // R:\home\niji\git_program\gflib2\RenderingEngine\source\Renderer\gfl2_CombinerCtrShaderDriver.cpp
          // uvMat = centerMat * rotMat * offSetMat * scaleMat * transMat; のとき
          // 例: アルファマスクテクスチャサイズが(w512,h256)(このうち左下から(w400,h240)使う)、スクリーンサイズが(w400,h240)、ソースサイズが(w256,h512)のとき
          texInfo->m_Attribute.m_Rotate     = GFL_MATH_DEG_TO_RAD(90);  // 90度回転  // これで縦横が正しくなる
          texInfo->m_Attribute.m_TranslateV = 1.0f + alphaMaskTex->GetTextureDesc().width / (static_cast<f32>(displayWidth));  // これで回転によって画面外にずれていた画像が画面内に出る。  // 正方形テクスチャで試していないです。
          texInfo->m_Attribute.m_ScaleU     = (static_cast<f32>(displayHeight)) / alphaMaskTex->GetTextureDesc().height *(256/(static_cast<f32>(displayHeight)));
              // 例: 240/256 *(256/240)  // 正方形テクスチャで試していないです。
          texInfo->m_Attribute.m_ScaleV     = (static_cast<f32>(displayWidth)) / alphaMaskTex->GetTextureDesc().width;  // 例: 400/512
          */
          
          // R:\home\niji\git_program\gflib2\RenderingEngine\source\Renderer\gfl2_CombinerCtrShaderDriver.cpp
          // uvMat = transMat * scaleMat * offSetMat * rotMat * centerMat; のとき
          // 例: アルファマスクテクスチャサイズが(w512,h256)(このうち左下から(w400,h240)使う)、スクリーンサイズが(w400,h240)、ソースサイズが(w256,h512)のとき
          texInfo->m_Attribute.m_Rotate     = GFL_MATH_DEG_TO_RAD(90);  // 90度回転  // これで縦横が正しくなる
          texInfo->m_Attribute.m_ScaleV     = (static_cast<f32>(displayHeight)) / alphaMaskTex->GetTextureDesc().height *(256/(static_cast<f32>(displayHeight)));
              // 例: 240/256 *(256/240)  // 正方形テクスチャで試していないです。
          texInfo->m_Attribute.m_ScaleU     = (static_cast<f32>(displayWidth)) / alphaMaskTex->GetTextureDesc().width;  // 例: 400/512
        }

        gfl2::renderingengine::renderer::MeshDrawTag* pTag = m_highLuminanceModelInstanceNode->GetMeshDrawTag( 0 );
        pTag->SetTexture( 0, sourceTexture );
        pTag->SetTexture( 1, alphaMaskTex );
        pTag->Draw( renderState );
      }
  
      gfl2::gfx::GFGL::SetTexture( 0, NULL );
      gfl2::gfx::GFGL::SetTexture( 1, NULL );
      gfl2::gfx::GFGL::SetTexture( 2, NULL );
  
      gfl2::gfx::GFGL::EndScene();
  
      gfl2::gfx::GFGL::SetRenderTarget( rDrawContext.m_pRenderTarget );
      gfl2::gfx::GFGL::SetDepthStencil( rDrawContext.m_pDepthStencil );
    }
    else
    {
      executeContinue = false;
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
    executeContinue = false;
  }

  return executeContinue;
}
bool BloomRenderPath::BloomRenderPathImpl::ExecuteForHighLuminanceCopy( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext, bool executeContinue )
{
  if(!executeContinue) return executeContinue;

  for(u32 i=0; i<m_highLuminanceCopyCount; ++i)
  {
    gfl2::gfx::ctr::CTRTexture* pSrcCTRTexture = NULL;
    gfl2::gfx::ctr::CTRTexture* pDstCTRTexture = NULL;
    if(i == 0)
    {
      pSrcCTRTexture = reinterpret_cast<gfl2::gfx::ctr::CTRTexture*>(m_highLuminanceRenderTargetTexture);
      pDstCTRTexture = reinterpret_cast<gfl2::gfx::ctr::CTRTexture*>(m_highLuminanceCopyTextureArray[0]);
    }
    else
    {
      pSrcCTRTexture = reinterpret_cast<gfl2::gfx::ctr::CTRTexture*>(m_highLuminanceCopyTextureArray[i-1]);
      pDstCTRTexture = reinterpret_cast<gfl2::gfx::ctr::CTRTexture*>(m_highLuminanceCopyTextureArray[i]);
    }

    GLvoid* srcAddr   = reinterpret_cast<GLvoid*>(pSrcCTRTexture->GetVirtualAddr());  // PhysicalAddrはダメ  // デバイスメモリあるいは VRAM に確保されている必要があります
    GLvoid* dstAddr   = reinterpret_cast<GLvoid*>(pDstCTRTexture->GetVirtualAddr());  // PhysicalAddrはダメ  // デバイスメモリあるいは VRAM に確保されている必要があります
    GLsizei srcWidth  = pSrcCTRTexture->GetTextureDesc().width;
    GLsizei srcHeight = pSrcCTRTexture->GetTextureDesc().height;
    GLenum  format    = m_highLuminanceTextureFormat;

    // コピー
    gfl2::gfx::GFGL::BeginScene();

    gfl2::gfx::ctr::CTRCommandList* pCommandList = gfl2::gfx::GFGL::GetCurrentCommandList();
    nn::gr::CommandBufferJumpHelper* pJumpHelper = gfl2::gfx::GFGL::GetJumpHelper();
    
    gfl2::gfx::GFGL::SyncCommandListAndJumpHelperBefore(pCommandList, pJumpHelper);  // 同期開始

    pCommandList->FilterBlockImage(srcAddr, dstAddr, srcWidth, srcHeight, format);

    gfl2::gfx::GFGL::SyncCommandListAndJumpHelperAfter(pCommandList, pJumpHelper);  // 同期終了

    gfl2::gfx::GFGL::EndScene();
  }

  return executeContinue;
}
bool BloomRenderPath::BloomRenderPathImpl::ExecuteForBoard( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext, bool executeContinue )
{
  if(!executeContinue) return executeContinue;

  if(m_desc.sourceType == SOURCE_TYPE_FRAME_BUFFER)
  {
    const gfl2::gfx::Surface* sourceSurface = NULL;
    /*gfl2::gfx::Texture*       sourceTexture = NULL;*/
    for(s32 i=0; i<gfl2::gfx::CtrDisplayNo::NumberOf; ++i)
    {
      if(m_sourceSurfaceArray[i] == rDrawContext.m_pRenderTarget)
      {
        sourceSurface = m_sourceSurfaceArray[i]; 
        /*sourceTexture = m_sourceTextureArray[i];*/
        break;
      }
    }

    if( sourceSurface )
    {
      gfl2::gfx::GFGL::SetRenderTarget( rDrawContext.m_pRenderTarget );
      gfl2::gfx::GFGL::SetDepthStencil( NULL );
 
      gfl2::gfx::GFGL::BeginScene();

      u32 displayWidth  = rDrawContext.m_DisplayWidth;
      u32 displayHeight = rDrawContext.m_DisplayHeight;
     
      gfl2::gfx::GFGL::SetViewport(0, 0, displayWidth, displayHeight);
      gfl2::gfx::GFGL::SetScissor(0, 0, displayWidth, displayHeight);

      gfl2::gfx::GFGL::FlushAllDirtyCommand();  // ViewportとSissorのコマンドを発行してくれるClearRenderTargetを呼んでいないので、自分でコマンドを発行しておく。

      gfl2::renderingengine::renderer::RenderState  renderState;
      gfl2::math::Matrix34 dummyMat34 = gfl2::math::Matrix34::GetIdentity();
      gfl2::math::Matrix44 dummyMat44 = gfl2::math::Matrix44::GetIdentity();
 
      renderState.SetProjectionMatrix( &dummyMat44 );
      renderState.SetViewMatrix( &dummyMat34 );
      renderState.SetWorldMatrix( &dummyMat34 );
 
      // 描画
      {
        for(u32 i=0; i<m_boardNum; ++i)
        {
          gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* mate = NULL;
          gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::TextureInfo* texInfo = NULL;
         
          mate = m_boardArray[i].modelInstanceNode->GetMaterialInstanceNode(0);
          
          for(s32 j=0; j<3; ++j)
          {
            texInfo = mate->GetTextureInfo(j);
        
            // 例: スクリーンサイズが(w400,h240)、ソースサイズが(w256,h512)、テクスチャサイズが(w64,h128)のとき  // テクスチャの縦横比率はソースと同じなので、ソースサイズを使えばいい。
            texInfo->m_Attribute.m_ScaleU = (static_cast<f32>(displayHeight)) / sourceSurface->GetSurfaceDesc().bufferWidth *(sourceSurface->GetSurfaceDesc().bufferWidth/(static_cast<f32>(displayHeight)));
                // 例: 240/256 *(256/240)  // 画像が上にはみ出ていて全部画面におさまっていないが、「*(256/240)」をすることではみ出ていた分をおさめることができる。  // 正方形テクスチャで試していないです。
            texInfo->m_Attribute.m_ScaleV = (static_cast<f32>(displayWidth)) / sourceSurface->GetSurfaceDesc().bufferHeight;  // 例: 400/512
          }

          gfl2::renderingengine::renderer::MeshDrawTag* pTag = m_boardArray[i].modelInstanceNode->GetMeshDrawTag( 0 );
          pTag->SetTexture( 0, m_highLuminanceBoardTexture );
          pTag->SetTexture( 1, m_highLuminanceBoardTexture );
          pTag->SetTexture( 2, m_highLuminanceBoardTexture );
          pTag->Draw( renderState );
        }
      }
 
      gfl2::gfx::GFGL::SetTexture( 0, NULL );
      gfl2::gfx::GFGL::SetTexture( 1, NULL );
      gfl2::gfx::GFGL::SetTexture( 2, NULL );
 
      gfl2::gfx::GFGL::EndScene();
 
      gfl2::gfx::GFGL::SetRenderTarget( rDrawContext.m_pRenderTarget );
      gfl2::gfx::GFGL::SetDepthStencil( rDrawContext.m_pDepthStencil );
    }
    else
    {
      executeContinue = false;
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
    executeContinue = false;
  }
 
  return executeContinue;
}

void BloomRenderPath::BloomRenderPathImpl::startAsyncCreateBinBuf(void)  // 呼び終わったら、外でm_createStateを変更して下さい。
{
  if(m_createState == CREATE_STATE_NONE)
  {
    // オープン
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
      req.arcId       = ARCID_BLOOM_RESOURCE;
      req.heapForFile = m_heapForTemp;  // すぐにクローズするのでtempを使う
      req.heapForReq  = m_heapForTemp;
      m_asyncFileManager->AddArcFileOpenReq(req);
    }
    
    // ロード
    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
      req.arcId       = ARCID_BLOOM_RESOURCE;
      req.datId       = GARC_bloom_resource_bloom_resource_pack_PACK;
      req.ppBuf       = &m_packBuf;
      req.pBufSize    = &m_packBufSize;
      req.heapForBuf  = m_heapForImpl;
      req.align       = 128;
      req.heapForReq  = m_heapForTemp;
      m_asyncFileManager->AddArcFileLoadDataReq(req);
    }
 
    // クローズ
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
      req.arcId       = ARCID_BLOOM_RESOURCE;
      req.heapForReq  = m_heapForTemp;
      m_asyncFileManager->AddArcFileCloseReq(req);
    }
  }
}

bool BloomRenderPath::BloomRenderPathImpl::updateAsyncCreateBinBuf(void)  // trueを返したら、外でm_createStateを変更して下さい。
{
  bool ret = false;
  if(m_createState == CREATE_STATE_CREATING)
  {
    if(    m_asyncFileManager->IsArcFileOpenFinished(ARCID_BLOOM_RESOURCE)
        && m_asyncFileManager->IsArcFileLoadDataFinished(&m_packBuf)
        && m_asyncFileManager->IsArcFileCloseFinished(ARCID_BLOOM_RESOURCE)
    )
    {
      // BinLinker
      {
        gfl2::fs::BinLinkerAccessor binLinker(m_packBuf);
        for(s32 i=0; i<RESOURCE_NUM; ++i)
        {
          m_binBuf[i]     = binLinker.GetData(i);
          m_binBufSize[i] = binLinker.GetDataSize(i);
        }
      }
      ret = true;
    }
  }
  return ret;
}

void BloomRenderPath::BloomRenderPathImpl::syncCreateBinBuf(void)
{
  // ファイルリード
  if(m_createState == CREATE_STATE_NONE)
  {
    /*
    arcにする前はこれらを読んでました、というメモ。

    static const char* binFileName[RESOURCE_NUM] =
    {
      //"data/app/test/kawada_test/not_conv/model_sample/bloom/bloom_high_luminance_model/bloom_high_luminance_model.ctr.gfbmdl",
      //"data/app/test/kawada_test/not_conv/model_sample/bloom/bloom_high_luminance_model/FillScrean.ctr.gfbvsh",
      //"data/app/test/kawada_test/not_conv/model_sample/bloom/bloom_high_luminance_model/Comb1_Manual.ctr.gfbfsh",
      //"data/app/test/kawada_test/not_conv/model_sample/bloom/bloom_high_luminance_model/bloom_dummy_tex.ctr.btex",

      "data/app/test/kawada_test/not_conv/model_sample/bloom/bloom_high_luminance_model_rgbxa/bloom_high_luminance_model_rgbxa.ctr.gfbmdl",
      "data/app/test/kawada_test/not_conv/model_sample/bloom/bloom_high_luminance_model_rgbxa/FillScrean.ctr.gfbvsh",
      "data/app/test/kawada_test/not_conv/model_sample/bloom/bloom_high_luminance_model_rgbxa/Comb1_Manual.ctr.gfbfsh",
      "data/app/test/kawada_test/not_conv/model_sample/bloom/bloom_high_luminance_model_rgbxa/bloom_dummy_tex.ctr.btex",

      "data/app/test/kawada_test/not_conv/model_sample/bloom/bloom_board_model/bloom_board_model.ctr.gfbmdl",
      "data/app/test/kawada_test/not_conv/model_sample/bloom/bloom_board_model/FillScrean.ctr.gfbvsh",
      "data/app/test/kawada_test/not_conv/model_sample/bloom/bloom_board_model/Comb1_Manual.ctr.gfbfsh",
      "data/app/test/kawada_test/not_conv/model_sample/bloom/bloom_board_model/bloom_dummy_tex.ctr.btex",
      "data/app/test/kawada_test/not_conv/model_sample/bloom/tex/bloom_small_tex_rgba0.ctr.btex",
      "data/app/test/kawada_test/not_conv/model_sample/bloom/tex/bloom_small_tex_rgba1.ctr.btex",
    };
   
    for(s32 i=0; i<RESOURCE_NUM; ++i)
    {
      gfl2::fs::AsyncFileManager::FileEasyReadReq req;
      req.fileName   = binFileName[i];
      req.ppBuf      = &m_binBuf[i];
      req.pBufSize   = &m_binBufSize[i];
      req.heapForBuf = m_heapForImpl;
      req.align      = 128;
      req.heapForReq = m_heapForTemp;
      m_asyncFileManager->SyncFileEasyRead(req);
    }
    */

    // オープン
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
      req.arcId       = ARCID_BLOOM_RESOURCE;
      req.heapForFile = m_heapForTemp;  // すぐにクローズするのでtempを使う
      req.heapForReq  = m_heapForTemp;
      m_asyncFileManager->SyncArcFileOpen(req);
    }
   
    // ロード
    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
      req.arcId       = ARCID_BLOOM_RESOURCE;
      req.datId       = GARC_bloom_resource_bloom_resource_pack_PACK;
      req.ppBuf       = &m_packBuf;
      req.pBufSize    = &m_packBufSize;
      req.heapForBuf  = m_heapForImpl;
      req.align       = 128;
      req.heapForReq  = m_heapForTemp;
      m_asyncFileManager->SyncArcFileLoadData(req);
    }

    // クローズ
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
      req.arcId       = ARCID_BLOOM_RESOURCE;
      req.heapForReq  = m_heapForTemp;
      m_asyncFileManager->SyncArcFileClose(req);
    }

    // BinLinker
    {
      //GFL_ASSERT(RESOURCE_NUM==BL_IDX_BLOOM_RESOURCE_PACK_MAX);  // 開発中に気付かせるためのASSERT
      // RESOURCE_NUMとBL_IDX_BLOOM_RESOURCE_PACK_MAXは同じ並び。

      gfl2::fs::BinLinkerAccessor binLinker(m_packBuf);
      for(s32 i=0; i<RESOURCE_NUM; ++i)
      {
        m_binBuf[i]     = binLinker.GetData(i);
        m_binBufSize[i] = binLinker.GetDataSize(i);
      }
    }
  }
}

void BloomRenderPath::BloomRenderPathImpl::createImpl(void)
{
  // allocatorForImpl
  m_allocatorForImpl = GFL_NEW(m_heapForImpl) gfl2::util::GLHeapAllocator(m_heapForImpl);

  this->createBoard();
  this->createResourceInstance();
  this->createHighLuminance();
  this->createSource();
}

void BloomRenderPath::BloomRenderPathImpl::createBoard(void)
{
  // boardの生成とboardのbinBuf
  {
    if(m_desc.bloomType == BloomRenderPath::BLOOM_TYPE_GAUSSIAN_FILTER_3X3)
    {
      m_boardNum = 3;  // 1個に3枚のテクスチャを貼れるので、9枚のテクスチャが必要なら3個でいい。
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
    }

    m_boardArray = GFL_NEW_ARRAY(m_heapForImpl) Board[m_boardNum];
   
    // binBufの所有者を変更する
    static const s32 boardResourceIndex[BOARD_RESOURCE_NUM] =
    {
      RESOURCE_BOARD_MODEL,
      RESOURCE_BOARD_VSH,
      RESOURCE_BOARD_FSH,
      RESOURCE_BOARD_TEX,
    };
    {
      for(s32 i=0; i<BOARD_RESOURCE_NUM; ++i)
      {
        s32 index = boardResourceIndex[i];
        m_boardArray[0].binBuf[i] = m_binBuf[index];
        m_binBuf[index] = NULL;
      }
    }

    // binBufを複製する
    for(s32 j=1; j<m_boardNum; ++j)
    {
      for(s32 i=0; i<BOARD_RESOURCE_NUM; ++i)
      {
        s32 index = boardResourceIndex[i];
        m_boardArray[j].binBuf[i] = GflHeapAllocMemoryAlign(m_heapForImpl, m_binBufSize[index], 128);
        gfl2::std::MemCopy( m_boardArray[0].binBuf[i], m_boardArray[j].binBuf[i], m_binBufSize[index] );
      }
    }
  }

  // boardのノード生成
  {
    for(s32 j=0; j<m_boardNum; ++j)
    {
      this->createTextureResourceNode(
          &m_boardArray[j].resourceNode[BOARD_RESOURCE_BOARD_TEX],
          &m_boardArray[j].textureResourceNode,
          m_boardArray[j].binBuf[BOARD_RESOURCE_BOARD_TEX]
      );
      
      this->createModelInstanceNode(
        &m_boardArray[j].resourceNode[BOARD_RESOURCE_BOARD_MODEL],
        &m_boardArray[j].resourceNode[BOARD_RESOURCE_BOARD_VSH],
        &m_boardArray[j].resourceNode[BOARD_RESOURCE_BOARD_FSH],
        &m_boardArray[j].modelInstanceNode,
        m_boardArray[j].binBuf[BOARD_RESOURCE_BOARD_MODEL],
        m_boardArray[j].binBuf[BOARD_RESOURCE_BOARD_VSH],
        m_boardArray[j].binBuf[BOARD_RESOURCE_BOARD_FSH],
        &m_boardArray[j].resourceNode[BOARD_RESOURCE_BOARD_TEX]
      );
    }
  }
}

void BloomRenderPath::BloomRenderPathImpl::createResourceInstance(void)
{
  static const s32 texResourceIndex[TEXTURE_RESOURCE_NUM] =
  {
    RESOURCE_HIGH_LUMINANCE_TEX,
    RESOURCE_BOARD_TEX,
    RESOURCE_SMALL_TEX_RGBA0,
    RESOURCE_SMALL_TEX_RGBA1,
  };
  static const s32 texResourceIndexInTextureResource[TEXTURE_RESOURCE_NUM] =
  {
    TEXTURE_RESOURCE_HIGH_LUMINANCE_TEX,
    TEXTURE_RESOURCE_BOARD_TEX,
    TEXTURE_RESOURCE_SMALL_TEX_RGBA0,
    TEXTURE_RESOURCE_SMALL_TEX_RGBA1,
  };

  for(s32 i=0; i<TEXTURE_RESOURCE_NUM; ++i)
  {
    s32 index = texResourceIndex[i];
    s32 indexInTextureResource = texResourceIndexInTextureResource[i];
    if(index != RESOURCE_BOARD_TEX)  // boardのテクスチャリソースノードはboard側に任せるので、ここではつくらない。
    {
      this->createTextureResourceNode(
          &m_resourceNode[index],
          &m_textureResourceNode[indexInTextureResource],
          m_binBuf[index]
      );
    }
  }
  
  this->createModelInstanceNode(
    &m_resourceNode[RESOURCE_HIGH_LUMINANCE_MODEL],
    &m_resourceNode[RESOURCE_HIGH_LUMINANCE_VSH],
    &m_resourceNode[RESOURCE_HIGH_LUMINANCE_FSH],
    &m_highLuminanceModelInstanceNode,
    m_binBuf[RESOURCE_HIGH_LUMINANCE_MODEL],
    m_binBuf[RESOURCE_HIGH_LUMINANCE_VSH],
    m_binBuf[RESOURCE_HIGH_LUMINANCE_FSH],
    &m_resourceNode[RESOURCE_HIGH_LUMINANCE_TEX]
  );
}

void BloomRenderPath::BloomRenderPathImpl::createHighLuminance(void)
{
  m_highLuminanceCopyCount = m_desc.highLuminanceCopyNumMax;  // 外部からいつでも設定できるようにしてもいいと思う。

  if(m_desc.sourceType == SOURCE_TYPE_FRAME_BUFFER)
  {
    u32 maxWidth  = 0;
    u32 maxHeight = 0;
    gfl2::gfx::Format oneFormat = gfl2::gfx::Format::A8R8G8B8;
    for(u32 i=0; i<gfl2::gfx::CtrDisplayNo::NumberOf; ++i)  // 今後はこんなやり方せず「SOURCE_TYPE_FRAME_BUFFERのときはこれにしか対応しません！」というテーブルを持っておいたほうがいい。
    {
      if(m_desc.sourceFrameBuffer[i])
      {
        if(maxWidth < m_desc.sourceFrameBuffer[i]->GetSurfaceDesc().bufferWidth)
        {
          maxWidth = m_desc.sourceFrameBuffer[i]->GetSurfaceDesc().bufferWidth;
        }
        if(maxHeight < m_desc.sourceFrameBuffer[i]->GetSurfaceDesc().bufferHeight)
        {
          maxHeight = m_desc.sourceFrameBuffer[i]->GetSurfaceDesc().bufferHeight;
        }
        oneFormat = m_desc.sourceFrameBuffer[i]->GetSurfaceDesc().format;
      }
    }
    u32 width  = maxWidth;
    u32 height = maxHeight;
    gfl2::gfx::Format format = oneFormat;

    width  =  width / m_desc.highLuminanceRenderTargetReciprocalScale;
    height = height / m_desc.highLuminanceRenderTargetReciprocalScale;
        // 既に縦横ひっくり返っているので、そのまま使う。

    // m_highLuminanceRenderTargetTexture
    m_highLuminanceRenderTargetTexture = gfl2::gfx::GFGL::CreateTexture(
        m_allocatorForHighLuminanceRenderTargetTexture,
        width,
        height,
        1,
        gfl2::gfx::Usage::RenderTarget,
        format,
        gfl2::gfx::Pool::Default
    );
        // サイズは2のべき乗で。縦横はひっくり返す。
        // 例: スクリーンサイズが(w400,h240)のとき、(w256,h512)を渡す。

    u32 srcWidth  = width;
    u32 srcHeight = height;

    // m_highLuminanceCopyTextureArray
    if(m_desc.highLuminanceCopyNumMax > 0)
    {
      m_highLuminanceCopyTextureArray = GFL_NEW_ARRAY(m_heapForImpl) gfl2::gfx::Texture*[m_desc.highLuminanceCopyNumMax];  // @fix cov_ctr[11726]: 配列でメモリ確保
      for(u32 i=0; i<m_desc.highLuminanceCopyNumMax; ++i)
      {
        srcWidth  = width;
        srcHeight = height;
        width  =  width / 2;
        height = height / 2;
        
        m_highLuminanceCopyTextureArray[i] = gfl2::gfx::GFGL::CreateTexture(
            m_allocatorForHighLuminanceCopyTexture,
            width,
            height,
            1,
            gfl2::gfx::Usage::RenderTarget,
            format,
            gfl2::gfx::Pool::Default
        );
      }
    }

    // m_highLuminanceTextureFormat
    //bool isOk = true;
    switch(format)
    {
    case gfl2::gfx::Format::A8R8G8B8:
      m_highLuminanceTextureFormat = GL_RGBA8_OES;
      break;
    case gfl2::gfx::Format::X8R8G8B8:
      m_highLuminanceTextureFormat = GL_RGB8_OES;
      break;
    //case gfl2::gfx::Format::ない:
    //  m_highLuminanceTextureFormat = GL_RGBA4;
    //  break;
    //case gfl2::gfx::Format::ない:
    //  m_highLuminanceTextureFormat = GL_RGB5_A1;
    //  break;
    case gfl2::gfx::Format::R5G6B5:
      m_highLuminanceTextureFormat = GL_RGB565;
      break;
    default:
      GFL_ASSERT(0);  // 開発中に気付いてもらうためのASSERT
      //isOk = false;
      break;
    }

    // フォーマットによってコピー元のサイズに制限があるので確認する
    switch(m_highLuminanceTextureFormat)
    {
    case GL_RGBA8_OES:
    case GL_RGB8_OES:
      if( !( srcWidth>=64 && srcWidth%64==0   &&   srcHeight>=64 && srcHeight%64==0 ) )
      {
        GFL_ASSERT(0);  // 開発中に気付いてもらうためのASSERT
        //isOk = false;
      }
      break;
    case GL_RGBA4:
    case GL_RGB5_A1:
    case GL_RGB565:
      if( !( srcWidth>=128 && srcWidth%128==0   &&   srcHeight>=128 && srcHeight%16==0 ) )
      {
        GFL_ASSERT(0);  // 開発中に気付いてもらうためのASSERT
        //isOk = false;
      }
      break;
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
    m_highLuminanceCopyCount = 0;
  }

  if(m_highLuminanceCopyCount == 0)
  {
    m_highLuminanceBoardTexture = m_highLuminanceRenderTargetTexture;
  }
  else
  {
    m_highLuminanceBoardTexture = m_highLuminanceCopyTextureArray[m_highLuminanceCopyCount-1];
  }
}

void BloomRenderPath::BloomRenderPathImpl::createSource(void)
{
  if(m_desc.sourceType == SOURCE_TYPE_FRAME_BUFFER)
  {
    m_sourceSurfaceNum = gfl2::gfx::CtrDisplayNo::NumberOf;
    m_sourceSurfaceArray = GFL_NEW_ARRAY(m_heapForImpl) const gfl2::gfx::Surface*[m_sourceSurfaceNum];
    for(s32 i=0; i<gfl2::gfx::CtrDisplayNo::NumberOf; ++i)
    {
      m_sourceSurfaceArray[i] = m_desc.sourceFrameBuffer[i];  // ポインタを持っておくだけ
    }
    
    m_sourceTextureNum = m_sourceSurfaceNum;
    m_sourceTextureArray = GFL_NEW_ARRAY(m_heapForImpl) gfl2::gfx::Texture*[m_sourceTextureNum];
    for(u32 i=0; i<m_sourceSurfaceNum; ++i)
    {
      if(m_sourceSurfaceArray[i])
      {
        m_sourceTextureArray[i] = gfl2::gfx::GFGL::CreateTextureFromSurface(m_allocatorForImpl, m_sourceSurfaceArray[i]);
      }
      else
      {
        m_sourceTextureArray[i] = NULL;
      }
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  }
}

void BloomRenderPath::BloomRenderPathImpl::createTextureResourceNode(
  gfl2::renderingengine::scenegraph::resource::ResourceNode** resourceNode,                      
  const gfl2::renderingengine::scenegraph::resource::TextureResourceNode** textureResourceNode,  
  void* binBuf                                                                                   
)
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryTexData texData;
  texData.SetTextureData(static_cast<c8*>(binBuf));
  *resourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_allocatorForImpl, &texData);
  
  // テクスチャリソースノードのアドレス
  *textureResourceNode = BloomRenderPathUtil::TextureResourceNodeGetter::GetTextureResourceNode(*resourceNode);
}
void BloomRenderPath::BloomRenderPathImpl::createModelInstanceNode(
  gfl2::renderingengine::scenegraph::resource::ResourceNode**      resourceNodeOfModel,
  gfl2::renderingengine::scenegraph::resource::ResourceNode**      resourceNodeOfVsh,  
  gfl2::renderingengine::scenegraph::resource::ResourceNode**      resourceNodeOfFsh,  
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode** modelInstanceNode,
  void* binBufOfModel,                                                                 
  void* binBufOfVsh,                                                                   
  void* binBufOfFsh,                                                                   
  gfl2::renderingengine::scenegraph::resource::ResourceNode** resourceNodeOfTex         
)
{
  // リソースノード生成
  {
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData mdlData;
      mdlData.SetModelData(static_cast<c8*>(binBufOfModel));
      *resourceNodeOfModel = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_allocatorForImpl, &mdlData);
    }
 
    for(s32 i=0; i<2; ++i)
    {
      gfl2::renderingengine::scenegraph::resource::ResourceNode** resourceNode = NULL;
      void* binBuf = NULL;
      if(i == 0)
      {
        resourceNode = resourceNodeOfVsh;
        binBuf       = binBufOfVsh;
      }
      else
      {
        resourceNode = resourceNodeOfFsh;
        binBuf       = binBufOfFsh;
      }
 
      gfl2::renderingengine::scenegraph::resource::GfBinaryShdData shdData;
      shdData.SetShaderData(static_cast<c8*>(binBuf));
      *resourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_allocatorForImpl, &shdData);
    } 
 
    // モデルのリソースノードにぶら下げる
    {
      (*resourceNodeOfModel)->AddChild(*resourceNodeOfVsh);
      (*resourceNodeOfModel)->AddChild(*resourceNodeOfFsh);
      (*resourceNodeOfModel)->AddChild(*resourceNodeOfTex);

      // 子ノードは親ノードが解放されるときに一緒に解放されるので、二重解放しないようにNULLを入れておく。
      *resourceNodeOfVsh = NULL;
      *resourceNodeOfFsh = NULL;
      *resourceNodeOfTex = NULL;
    }
  }

  // インスタンスノード生成
  {
    *modelInstanceNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode(m_allocatorForImpl, *resourceNodeOfModel);

    {
      gfl2::renderingengine::scenegraph::instance::DrawableNode::UpdateDrawTagInfo info;
      (*modelInstanceNode)->UpdateDrawTag(info);
    }
 
    gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild(*modelInstanceNode);
  }
}

void BloomRenderPath::BloomRenderPathImpl::destroyBinBuf(void)
{
  for(s32 i=0; i<RESOURCE_NUM; ++i)
  {
    //GflHeapSafeFreeMemory(m_binBuf[i]);  arcにする前はこれらを読んでました、というメモ。
    m_binBuf[i] = NULL;

    // 初期化
    m_binBufSize[i] = 0;
  }

  GflHeapSafeFreeMemory(m_packBuf);
  m_packBufSize = 0;
}
void BloomRenderPath::BloomRenderPathImpl::destroyImpl(void)
{
  this->destroySource();
  this->destroyHighLuminance();
  this->destroyResourceInstance();
  this->destroyBoard();

  // allocatorForImpl
  GFL_SAFE_DELETE(m_allocatorForImpl);
}
void BloomRenderPath::BloomRenderPathImpl::destroyBoard(void)
{
  if(m_boardArray)
  {
    for(u32 i=0; i<m_boardNum; ++i)
    {
      GFL_SAFE_DELETE(m_boardArray[i].modelInstanceNode);
      for(s32 j=0; j<BOARD_RESOURCE_NUM; ++j)
      {
        GFL_SAFE_DELETE(m_boardArray[i].resourceNode[j]);
      }
      if(i>=1)  // i==0はm_packBufのものだから解放しない。
      {
        for(s32 j=0; j<BOARD_RESOURCE_NUM; ++j)
        {
          GflHeapSafeFreeMemory(m_boardArray[i].binBuf[j]);
        }
      }
    }
    GFL_SAFE_DELETE_ARRAY(m_boardArray);
  }
  
  // 初期化
  m_boardNum = 0;
}
void BloomRenderPath::BloomRenderPathImpl::destroyResourceInstance(void)
{
  GFL_SAFE_DELETE(m_highLuminanceModelInstanceNode);
  for(s32 i=0; i<RESOURCE_NUM; ++i)
  {
    GFL_SAFE_DELETE(m_resourceNode[i]);
  }

  for(s32 i=0; i<TEXTURE_RESOURCE_NUM; ++i)
  {
    // 初期化
    m_textureResourceNode[i] = NULL;
  }
}
void BloomRenderPath::BloomRenderPathImpl::destroyHighLuminance(void)
{
  if(m_highLuminanceCopyTextureArray)
  {
    for(u32 i=0; i<m_desc.highLuminanceCopyNumMax; ++i)
    {
      GFL_SAFE_DELETE(m_highLuminanceCopyTextureArray[i]);
    }
    GFL_SAFE_DELETE_ARRAY(m_highLuminanceCopyTextureArray);
  }

  GFL_SAFE_DELETE(m_highLuminanceRenderTargetTexture);

  // 初期化
  m_highLuminanceCopyCount     = 0;
  m_highLuminanceBoardTexture  = NULL;
  m_highLuminanceTextureFormat = 0;
}
void BloomRenderPath::BloomRenderPathImpl::destroySource(void)
{
  if(m_desc.sourceType == SOURCE_TYPE_FRAME_BUFFER)
  {
    if(m_sourceTextureArray)
    {
      for(s32 i=0; i<m_sourceTextureNum; ++i)
      {
        GFL_SAFE_DELETE(m_sourceTextureArray[i]);
      }
      GFL_SAFE_DELETE_ARRAY(m_sourceTextureArray);
    }
    if(m_sourceSurfaceArray)
    {
      GFL_SAFE_DELETE_ARRAY(m_sourceSurfaceArray);
    }
    
    // 初期化
    m_sourceTextureNum = 0;
    m_sourceSurfaceNum = 0;
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  }
}

void BloomRenderPath::BloomRenderPathImpl::updateHighLuminanceSetting(void)
{
  if(m_highLuminanceModelInstanceNode)
  {
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* mate = NULL;
    gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam* attr = NULL;

    mate = m_highLuminanceModelInstanceNode->GetMaterialInstanceNode(0);
    
    attr = mate->GetAttributeParam();
    attr->m_ConstantColor[0] = gfl2::math::Vector4(m_param->weight.x, m_param->weight.y, m_param->weight.z, 0.0f);

    gfl2::gfx::BlendStateObject* blendStateObject = mate->GetBlendStateObject();
    blendStateObject->SetAlphaRef(m_param->intensity);
    blendStateObject->SetAlphaCompareFunc(gfl2::gfx::CompareFunc::Greater);
    blendStateObject->SetAlphaTestEnable(true);
    blendStateObject->UpdateState();
  }

  // 縮小テクスチャ
  {
    if(m_param->reduceNum == 0)
    {
      m_highLuminanceCopyCount = 0;
    }
    else
    {
      if(m_param->reduceNum <= m_desc.highLuminanceCopyNumMax)
      {
        m_highLuminanceCopyCount = m_param->reduceNum;
      }
      else
      {
        GFL_ASSERT(0);  // 縮小コピー回数が、実際に行ったコピー回数より大きいのでダメ。  // 開発中に気付かせるためのASSERT
      }
    }
 
    if(m_highLuminanceCopyCount == 0)
    {
      if(m_highLuminanceRenderTargetTexture)
      {
        m_highLuminanceBoardTexture = m_highLuminanceRenderTargetTexture;
      }
    }
    else
    {
      if(m_highLuminanceCopyTextureArray)
      {
        m_highLuminanceBoardTexture = m_highLuminanceCopyTextureArray[m_highLuminanceCopyCount-1];
      }
    }
  }
}
void BloomRenderPath::BloomRenderPathImpl::updateBoardSetting(void)
{
  if(m_desc.bloomType == BloomRenderPath::BLOOM_TYPE_GAUSSIAN_FILTER_3X3)
  {
    struct BoardChartForGaussianFilter3x3
    {
      f32 translateU;
      f32 translateV;
      f32 blurBasePower;
    };
    static const BoardChartForGaussianFilter3x3 chart[3*3] =
    {
      //  transU,  transV,  blurBasePower
      {   0.000f,  0.000f,     4.0f/16.0f },
      {   0.002f,  0.000f,     2.0f/16.0f },
      {  -0.002f,  0.000f,     2.0f/16.0f },
      {   0.000f,  0.002f,     2.0f/16.0f },
      {   0.000f, -0.002f,     2.0f/16.0f },
      //{   0.002f,  0.002f,     1.0f/16.0f },
      //{   0.002f, -0.002f,     1.0f/16.0f },
      {  -0.002f,  0.002f,     1.0f/16.0f },
      {  -0.002f, -0.002f,     1.0f/16.0f },
      {   0.002f, -0.002f,     1.0f/16.0f },  // f=t:   フレームバッファ正常, (-0.002,-0.002)を描画;                       f=t+1: フレームバッファに主人公分の穴があく, (-0.002,-0.002)を描画;
      {   0.002f,  0.002f,     1.0f/16.0f },  // f=t+2: フレームバッファに主人公分の穴があく, (-0.002,-0.002)を描画しない; f=t+3: フレームバッファ正常, (-0.002,-0.002)を描画しない;
    };                                        // このような現象が起きたので、(-0.002,-0.002)の描画の場所を変えた。

    if(m_boardArray)
    {
      if(m_boardNum == 3)
      {
        u32 totalCount = 0;
        for(u32 i=0; i<3; ++i)
        {
          gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* mate = NULL;
          gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::TextureInfo* texInfo = NULL;
          gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam* attr = NULL;
       
          mate = m_boardArray[i].modelInstanceNode->GetMaterialInstanceNode(0);
          attr = mate->GetAttributeParam();
          
          for(u32 j=0; j<3; ++j)
          {
            texInfo = mate->GetTextureInfo(j);
       
            texInfo->m_Attribute.m_TranslateU = chart[totalCount].translateU * m_param->range;
            texInfo->m_Attribute.m_TranslateV = chart[totalCount].translateV * m_param->range;
       
            f32 blurPower = chart[totalCount].blurBasePower * m_param->strength;
            attr->m_ConstantColor[0] = gfl2::math::Vector4(blurPower, blurPower, blurPower, 0.0f);
            ++totalCount;
          }
        }
      }
      else
      {
        GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。まず起こり得ない。
      }
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  }
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


BloomRenderPath::BloomRenderPath(void)
  : gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath(),
    m_drawEnvNode(NULL),
    m_alphaMaskTex(NULL),
    m_impl(NULL),
    m_param()
{}
BloomRenderPath::~BloomRenderPath()
{
  if( !(this->CanDestroy()) )
  {
    // 破棄してはいけない状態のとき
    GFL_ASSERT(0);  // 破棄してはいけないときに破棄しようとしている。ケア済みのASSERT。
   
    u32 count = 0;
    while(count < 10000)  // 無限ループ対策
    {
      gfl2::thread::ThreadStatic::CurrentSleep(1);
      this->Update();
      if( this->CanDestroy() )
      {
        break;
      }
      ++count;
    }
  }

  // ここまで来たら、破棄してもいい状態になったので破棄する
  GFL_SAFE_DELETE(m_impl);
}
 
void BloomRenderPath::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
  if(m_impl)
  {
    m_impl->Execute(rDrawContext);
  }
}

void BloomRenderPath::StartAsyncCreate(
    gfl2::heap::HeapBase*       heapForImpl,                                  
    gfl2::heap::HeapBase*       heapForTemp,
    gfl2::gfx::IGLAllocator*    allocatorForHighLuminanceRenderTargetTexture,
    gfl2::gfx::IGLAllocator*    allocatorForHighLuminanceCopyTexture,        
    gfl2::fs::AsyncFileManager* asyncFileManager,
    const Description& desc
)
{
  if(m_impl == NULL)
  {
    m_impl = GFL_NEW(heapForImpl) BloomRenderPathImpl();
    m_impl->SetDrawEnvNode(m_drawEnvNode);
    m_impl->SetAlphaMaskTexture(m_alphaMaskTex);
    m_impl->StartAsyncCreate(
        heapForImpl,
        heapForTemp,
        allocatorForHighLuminanceRenderTargetTexture,
        allocatorForHighLuminanceCopyTexture,        
        asyncFileManager,
        desc,
        &m_param
    );
  }
  else
  {
    GFL_ASSERT(0);  // 既に生成済み。開発中に気付かせるためのASSERT。
  }
}
void BloomRenderPath::SyncCreate(
    gfl2::heap::HeapBase*       heapForImpl,                                 
    gfl2::heap::HeapBase*       heapForTemp,
    gfl2::gfx::IGLAllocator*    allocatorForHighLuminanceRenderTargetTexture,
    gfl2::gfx::IGLAllocator*    allocatorForHighLuminanceCopyTexture,        
    gfl2::fs::AsyncFileManager* asyncFileManager,
    const Description& desc
)
{
  if(m_impl == NULL)
  {
    m_impl = GFL_NEW(heapForImpl) BloomRenderPathImpl();
    m_impl->SetDrawEnvNode(m_drawEnvNode);
    m_impl->SetAlphaMaskTexture(m_alphaMaskTex);
    m_impl->SyncCreate(
        heapForImpl,
        heapForTemp,
        allocatorForHighLuminanceRenderTargetTexture,
        allocatorForHighLuminanceCopyTexture,        
        asyncFileManager,
        desc,
        &m_param
    );
  }
  else
  {
    GFL_ASSERT(0);  // 既に生成済み。開発中に気付かせるためのASSERT。
  }
}

bool BloomRenderPath::IsCreated(void) const
{
  bool isCreated = false;
  if(m_impl)
  {
    isCreated = m_impl->IsCreated();
  }
  return isCreated;
}

bool BloomRenderPath::CanDestroy(void) const
{
  bool canDestroy = true;
  if(m_impl)
  {
    if( !(this->IsCreated()) )
    {
      canDestroy = false;
    }
  }
  return canDestroy;
}

void BloomRenderPath::Update(void)
{
  if(m_impl)
  {
    m_impl->Update();
  }
}

void BloomRenderPath::SetDrawEnvNode(const gfl2::renderingengine::scenegraph::instance::DrawEnvNode* drawEnvNode)
{
  m_drawEnvNode = drawEnvNode;
  if(m_impl)
  {
    m_impl->SetDrawEnvNode(m_drawEnvNode);
  }
}

void BloomRenderPath::SetAlphaMaskTexture(const gfl2::gfx::Texture* alphaMaskTex)
{
  m_alphaMaskTex = alphaMaskTex;
  if(m_impl)
  {
    m_impl->SetAlphaMaskTexture(m_alphaMaskTex);
  }
}

gfl2::renderingengine::scenegraph::resource::TextureResourceNode* BloomRenderPath::GetTextureResourceNode(gfl2::renderingengine::scenegraph::DagNode* dagNode)
{
  gfl2::renderingengine::scenegraph::resource::TextureResourceNode* textureResourceNode = BloomRenderPathUtil::TextureResourceNodeGetter::GetTextureResourceNode(dagNode);
  return textureResourceNode;
}

void BloomRenderPath::SetEnable(bool enable)
{
  m_param.enable = enable;
}
bool BloomRenderPath::GetEnable(void) const
{
  return m_param.enable;
}

void BloomRenderPath::SetReduceNum(u32 reduceNum)
{
  m_param.reduceNum = reduceNum;
}
u32  BloomRenderPath::GetReduceNum(void) const
{
  return m_param.reduceNum;
}

void BloomRenderPath::SetStrength(f32 strength)
{
  m_param.strength = strength;
}
f32 BloomRenderPath::GetStrength(void) const
{
  return m_param.strength;
}

void BloomRenderPath::SetRange(f32 range)
{
  m_param.range = range;
}
f32 BloomRenderPath::GetRange(void) const
{
  return m_param.range;
}

void BloomRenderPath::SetIntensity(f32 intensity)
{
  m_param.intensity = intensity;
}
f32 BloomRenderPath::GetIntensity(void) const
{
  return m_param.intensity;
}

void BloomRenderPath::SetWeight(const gfl2::math::Vector4& weight)
{
  m_param.weight = weight;
}
gfl2::math::Vector4 BloomRenderPath::GetWeight(void) const
{
  return m_param.weight;
}


GFL_NAMESPACE_END(renderer)
GFL_NAMESPACE_END(poke_3d)


#endif  // GF_PLATFORM_CTR

