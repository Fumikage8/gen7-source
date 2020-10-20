#if defined(GF_PLATFORM_WIN32)


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <fs/include/gfl2_Fs.h>
#include <util/include/gfl2_GLHeapAllocator.h>

#include <gfx/include/gfl2_GLMemory.h>
#include <gfx/include/gfl2_Texture.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNode.h>


// poke_3d_libのインクルード
#include <renderer/include/gfl2_BloomRenderPath.h>
#include "gfl2_WinOpenGLBloomRenderPathClasses.h"
#include "gfl2_BloomRenderPathUtil.h"


// nijiのデータのヘッダーファイルのインクルード
#include <arc_def_index/arc_def.h>  // poke_3d_lib内なのにnijiのarcファイルを使ってしまっている。ごめんなさい。
#include <arc_index/bloom_resource.gaix>
#include <niji_conv_header/bloom_resource/win32/bloom_resource_pack.h>




GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(renderer)


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


//! @brief  ブルームのリソースローダー
class BloomResourceLoader
{
  GFL_FORBID_COPY_AND_ASSIGN(BloomResourceLoader);

public:
  BloomResourceLoader(gfl2::heap::HeapBase* heapForImpl, gfl2::heap::HeapBase* heapForTemp, gfl2::fs::AsyncFileManager* asyncFileManager)
    : m_packBuf(NULL),
      m_packBufSize(0)
  {
    // ファイルリード
    {
      // オープン
      {
        gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
        req.arcId       = ARCID_BLOOM_RESOURCE;
        req.heapForFile = heapForTemp;  // すぐにクローズするのでtempを使う
        req.heapForReq  = heapForTemp;
        asyncFileManager->SyncArcFileOpen(req);
      }
     
      // ロード
      {
        gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
        req.arcId       = ARCID_BLOOM_RESOURCE;
        req.datId       = GARC_bloom_resource_bloom_resource_pack_PACK;
        req.ppBuf       = &m_packBuf;
        req.pBufSize    = &m_packBufSize;
        req.heapForBuf  = heapForImpl;
        req.align       = 128;
        req.heapForReq  = heapForTemp;
        asyncFileManager->SyncArcFileLoadData(req);
      }
 
      // クローズ
      {
        gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
        req.arcId       = ARCID_BLOOM_RESOURCE;
        req.heapForReq  = heapForTemp;
        asyncFileManager->SyncArcFileClose(req);
      }
    }
  }
  virtual ~BloomResourceLoader()
  {
    // ファイルリードしたものを破棄する
    {
      GflHeapSafeFreeMemory(m_packBuf);
      m_packBufSize = 0;
    }
  }
  void* GetPackBuf(void) const
  {
    return m_packBuf;
  }

private:
  void* m_packBuf;
  u32   m_packBufSize;
};

//! @brief  ブルームの描画パスのインプリメント
class BloomRenderPath::BloomRenderPathImpl
{
  GFL_FORBID_COPY_AND_ASSIGN(BloomRenderPathImpl);

public:
  BloomRenderPathImpl( gfl2::heap::HeapBase* heapForImpl, gfl2::heap::HeapBase* heapForTemp, gfl2::fs::AsyncFileManager* asyncFileManager, u32 screenWidth, u32 screenHeight )
    : m_BloomResourceLoader(heapForImpl, heapForTemp, asyncFileManager),
      m_BinLinker( m_BloomResourceLoader.GetPackBuf() ),
      m_GLHeapAllocator(NULL),
      m_BloomRenderPathForLuminace(NULL),
      m_BloomRenderPathForScale(NULL),
      m_BloomRenderPathForBlur(NULL),
      m_BloomRenderPathForAdd(NULL)
  {
    m_GLHeapAllocator = GFL_NEW(heapForImpl) gfl2::util::GLHeapAllocator(heapForImpl);
  
    m_BloomRenderPathForLuminace = GFL_NEW(heapForImpl) BloomRenderPathForLuminance(
        m_GLHeapAllocator,
        m_BinLinker.GetData(BL_IDX_BLOOM_RESOURCE_PACK_BLOOMLUMINANCE_GFVSH), m_BinLinker.GetDataSize(BL_IDX_BLOOM_RESOURCE_PACK_BLOOMLUMINANCE_GFVSH),
        m_BinLinker.GetData(BL_IDX_BLOOM_RESOURCE_PACK_BLOOMLUMINANCE_GFFSH), m_BinLinker.GetDataSize(BL_IDX_BLOOM_RESOURCE_PACK_BLOOMLUMINANCE_GFFSH),
        screenWidth , screenHeight );

    m_BloomRenderPathForScale = GFL_NEW(heapForImpl) BloomRenderPathForScale(
        m_GLHeapAllocator,
        m_BinLinker.GetData(BL_IDX_BLOOM_RESOURCE_PACK_BLOOMSCALE_GFVSH), m_BinLinker.GetDataSize(BL_IDX_BLOOM_RESOURCE_PACK_BLOOMSCALE_GFVSH),
        m_BinLinker.GetData(BL_IDX_BLOOM_RESOURCE_PACK_BLOOMSCALE_GFFSH), m_BinLinker.GetDataSize(BL_IDX_BLOOM_RESOURCE_PACK_BLOOMSCALE_GFFSH),
        m_BloomRenderPathForLuminace->GetLuminanceTexture() );

#if USE_SCALE_BUFFER
    m_BloomRenderPathForBlur = GFL_NEW(heapForImpl) BloomRenderPathForBlur(
        m_GLHeapAllocator,
        m_BinLinker.GetData(BL_IDX_BLOOM_RESOURCE_PACK_BLOOMBLUR_GFVSH), m_BinLinker.GetDataSize(BL_IDX_BLOOM_RESOURCE_PACK_BLOOMBLUR_GFVSH),
        m_BinLinker.GetData(BL_IDX_BLOOM_RESOURCE_PACK_BLOOMBLUR_GFFSH), m_BinLinker.GetDataSize(BL_IDX_BLOOM_RESOURCE_PACK_BLOOMBLUR_GFFSH),
        m_BloomRenderPathForScale->GetMinScaleTexture() );
#else
    m_BloomRenderPathForBlur = GFL_NEW(heapForImpl) BloomRenderPathForBlur(
        m_GLHeapAllocator,
        m_BinLinker.GetData(BL_IDX_BLOOM_RESOURCE_PACK_BLOOMBLUR_GFVSH), m_BinLinker.GetDataSize(BL_IDX_BLOOM_RESOURCE_PACK_BLOOMBLUR_GFVSH),
        m_BinLinker.GetData(BL_IDX_BLOOM_RESOURCE_PACK_BLOOMBLUR_GFFSH), m_BinLinker.GetDataSize(BL_IDX_BLOOM_RESOURCE_PACK_BLOOMBLUR_GFFSH),
        m_BloomRenderPathForLuminace->GetLuminanceTexture() );
#endif

    m_BloomRenderPathForAdd = GFL_NEW(heapForImpl) BloomRenderPathForAdd(
        m_GLHeapAllocator,
        m_BinLinker.GetData(BL_IDX_BLOOM_RESOURCE_PACK_BLOOMADD_GFVSH), m_BinLinker.GetDataSize(BL_IDX_BLOOM_RESOURCE_PACK_BLOOMADD_GFVSH),
        m_BinLinker.GetData(BL_IDX_BLOOM_RESOURCE_PACK_BLOOMADD_GFFSH), m_BinLinker.GetDataSize(BL_IDX_BLOOM_RESOURCE_PACK_BLOOMADD_GFFSH),
        m_BloomRenderPathForLuminace->GetTargetTexture() , m_BloomRenderPathForBlur->GetBlurTexture() );
  }

  virtual ~BloomRenderPathImpl()
  {
    GFL_SAFE_DELETE(m_BloomRenderPathForAdd);
    GFL_SAFE_DELETE(m_BloomRenderPathForBlur);
    GFL_SAFE_DELETE(m_BloomRenderPathForScale);
    GFL_SAFE_DELETE(m_BloomRenderPathForLuminace);

    GFL_SAFE_DELETE(m_GLHeapAllocator);
  }

public:
  void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
  {
    m_BloomRenderPathForLuminace->Execute(rDrawContext);
#if USE_SCALE_BUFFER
    m_BloomRenderPathForScale->Execute(rDrawContext);
#endif
    m_BloomRenderPathForBlur->Execute(rDrawContext);
    m_BloomRenderPathForAdd->Execute(rDrawContext);
  }

public:
	// 輝度用ウェイトを設定
	void setBloomLuminanceWeight	(gfl2::math::Vector4 weight  ){ m_BloomRenderPathForLuminace->setLuminanceWeight(weight);			}
	// 抽出した輝度に対する係数を設定
	void setBloomLuminanceMulColor	(gfl2::math::Vector4 mulColor){ m_BloomRenderPathForLuminace->setLuminanceMulColor(mulColor);		}
	// 輝度の閾値を設定
	void setBloomLuminanceThreshold	(f32 threshold)				 { m_BloomRenderPathForLuminace->setLuminanceThreshold(threshold);	}
	// ブラー回数を設定
	void setBloomBlurCnt			(u32 cnt)					 { m_BloomRenderPathForBlur->setBlurCnt(cnt);						}  
	// ブラー方向を設定
	void setBloomBlurOffset(gfl2::math::Vector4& offset,u32 index){ m_BloomRenderPathForBlur->setBlurOffset(offset,index);			}
	// αマスクを設定
	void SetAlphaMaskTexture(const gfl2::gfx::Texture* pTexture) { m_BloomRenderPathForLuminace->SetAlphaMaskTexture(pTexture); }

	// 輝度用ウェイトを取得
	gfl2::math::Vector4	getBloomLuminanceWeight	  (void)		{ return m_BloomRenderPathForLuminace->getLuminanceWeight();		}
	// 抽出した輝度に対する係数を取得
	gfl2::math::Vector4  getBloomLuminanceMulColor (void)		{ return m_BloomRenderPathForLuminace->getLuminanceMulColor();	}
	// 輝度の閾値を取得
	f32					getBloomLuminanceThreshold(void)		{ return m_BloomRenderPathForLuminace->getLuminanceThreshold();	}
	// ブラー回数を取得
	u32					getBloomBlurCnt			  (void)		{ return m_BloomRenderPathForBlur->getBlurCnt();					}
	// ブラー方向を取得
	gfl2::math::Vector4	getBloomBlurOffset		  (u32 index)	{ return m_BloomRenderPathForBlur->getBlurOffset(index);			}

private:
  BloomResourceLoader          m_BloomResourceLoader;
  gfl2::fs::BinLinkerAccessor  m_BinLinker;
  gfl2::util::GLHeapAllocator* m_GLHeapAllocator;
  BloomRenderPathForLuminance* m_BloomRenderPathForLuminace;
  BloomRenderPathForScale*     m_BloomRenderPathForScale;
  BloomRenderPathForBlur*      m_BloomRenderPathForBlur;
  BloomRenderPathForAdd*       m_BloomRenderPathForAdd;
};


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
    GFL_ASSERT(0);  // 破棄してはいけないときに破棄しようとしている。ケア済みのASSERT。  // 同期生成のケアしかできていないので、非同期生成もつくったららケアを忘れずに書くこと！
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
    this->SyncCreate(
      heapForImpl,
      heapForTemp,
      allocatorForHighLuminanceRenderTargetTexture,
      allocatorForHighLuminanceCopyTexture,        
      asyncFileManager,
      desc
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
    m_impl = GFL_NEW(heapForImpl) BloomRenderPathImpl(heapForImpl,heapForTemp,asyncFileManager,512,256);  // (400,240)ではハングした。  // (512,256)固定で上下画面きれいに出たが、スクリーンごとにサイズを変えなければならないかもしれない。

    // 初期値を反映しておく
    {
      gfl2::math::Vector4 mulColor = m_impl->getBloomLuminanceMulColor();
      m_param.strength = mulColor.y;  // greenを採用したがいいか？

      m_param.intensity = m_impl->getBloomLuminanceThreshold();

      m_param.weight = m_impl->getBloomLuminanceWeight();
    }
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
    isCreated = true;
  }
  return isCreated;
}

bool BloomRenderPath::CanDestroy(void) const
{
  return true;
}

void BloomRenderPath::Update(void)
{
  // DrawEnvNodeの内容をParameterに反映する
  if(m_drawEnvNode)
  {
    m_param.enable    = m_drawEnvNode->GetGlareEnable();
    m_param.reduceNum = m_drawEnvNode->GetGlareReduceNum();
    m_param.strength  = m_drawEnvNode->GetGlareStrength();
    m_param.range     = m_drawEnvNode->GetGlareRange();
    m_param.intensity = m_drawEnvNode->GetGlareIntensity();
    m_param.weight    = m_drawEnvNode->GetGlareWeight();
  }

  // ブルームの無効処理
  if( !m_param.enable )
  {
    m_param.strength = 0.0f;
  }

  // 更新
  if(m_impl)
  {
    gfl2::math::Vector4 mulColor(m_param.strength, m_param.strength, m_param.strength, 0.0f);
    m_impl->setBloomLuminanceMulColor(mulColor);
    
    m_impl->setBloomLuminanceThreshold(m_param.intensity);
    
    m_impl->setBloomLuminanceWeight(m_param.weight);
  }
}

void BloomRenderPath::SetDrawEnvNode(const gfl2::renderingengine::scenegraph::instance::DrawEnvNode* drawEnvNode)
{
  m_drawEnvNode = drawEnvNode;
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


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_END(renderer)
GFL_NAMESPACE_END(poke_3d)


#endif  // GF_PLATFORM_WIN32

