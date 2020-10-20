//==============================================================================
/**
 @file    app_util_RenderTargetTexture.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.07.31
 @brief   レンダーターゲットテクスチャ
 */
//==============================================================================

// gfl2のインクルード
#include <gfx/include/gfl2_Texture.h>
#include <gfx/include/gfl2_Surface.h>
#include <util/include/gfl2_GLHeapAllocator.h>

// nijiのインクルード
#include <System/include/nijiAllocator.h>
#include <System/include/GflUse.h>
#include <AppLib/include/Util/app_util_2d.h>

#include <AppLib/include/Util/app_util_RenderTargetTexture.h>




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)


RenderTargetTexture::RenderTargetTexture(void)
  : m_vramAllocator(NULL),
    m_devGLAllocator(NULL),
    m_desc(),
    m_realWidth(0),
    m_realHeight(0),
    m_colorTexture(NULL),
    m_depthTexture(NULL),
    m_depthSurface(NULL),
    m_clearColor(gfl2::gfx::Color(0,0,0,0))
{}

RenderTargetTexture::~RenderTargetTexture()
{
  this->Destroy();
}

void RenderTargetTexture::Create(
    System::nijiAllocator*       vramAllocator,
    gfl2::util::GLHeapAllocator* devGLAllocator,
    const Description&           desc
)
{
  m_desc = desc;

  m_realWidth  = this->roundUpToPowerOf2(desc.width);
  m_realHeight = this->roundUpToPowerOf2(desc.height);

  // colorTexture
  m_colorTexture = gfl2::gfx::GFGL::CreateTexture(
      vramAllocator,
#if defined(GF_PLATFORM_CTR)
      m_realWidth,
      m_realHeight,
#elif defined(GF_PLATFORM_WIN32)
      512,//m_realWidth,
      256,//m_realHeight,
#endif  // GF_PLATFORM_
      1,
      gfl2::gfx::Usage::RenderTarget,
      desc.colorFormat,
      gfl2::gfx::Pool::Default
  );

  // depthTexture
#if defined(GF_PLATFORM_CTR)
  switch(m_desc.type)
  {
  case Type::DEPTH_BUFFER_RENTAL:
    {
/*
      gfl2::gfx::Surface::SurfaceDesc surfaceDesc;
      surfaceDesc.usage        = gfl2::gfx::Usage::DepthStencil;
      surfaceDesc.pool         = gfl2::gfx::Pool::Default;
      surfaceDesc.width        = m_desc.width;
      surfaceDesc.height       = m_desc.height;
      surfaceDesc.depth        = 1;
      surfaceDesc.format       = m_desc.depthFormat;
      surfaceDesc.multiSample  = gfl2::gfx::MultiSample::None;
      surfaceDesc.bufferWidth  = m_realWidth;
      surfaceDesc.bufferHeight = m_realWidth;

      // 借りる
      gfl2::gfx::DisplayDeviceHandle handle       = System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT];
      gfl2::gfx::Surface*            pDepthBuffer = gfl2::gfx::GFGL::GetDepthStencilBuffer( handle );
      void*                          virtualAddr  = reinterpret_cast<void*>((static_cast<gfl2::gfx::ctr::CTRSurface*>(pDepthBuffer))->GetHandle().depthStencilBuffer.virtualAddr);

      gfl2::gfx::GLMemory::SetInstanceAllocator(devGLAllocator);
      m_depthSurface = new gfl2::gfx::ctr::CTRSurface(surfaceDesc, virtualAddr);  // 小文字new, deleteを使っているので、やり方を下の方法に変更した。
      gfl2::gfx::GLMemory::ClearInstanceAllocator();
      
      break;
*/

      // 上のコメントアウトしてあるやり方から変更した
      gfl2::gfx::DisplayDeviceHandle handle       = System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT];
      gfl2::gfx::Surface*            pDepthBuffer = gfl2::gfx::GFGL::GetDepthStencilBuffer( handle );
      m_depthTexture = gfl2::gfx::GFGL::CreateTextureFromSurface(devGLAllocator, pDepthBuffer);
      m_depthSurface = const_cast<gfl2::gfx::Surface*>(m_depthTexture->GetSurface());  // constをはずしている。この後gfl2::renderingengine::renderer::RenderingPipeLineのSetDepthStencil関数に渡すために。石黒さん確認済み。
      break;
    }
  case Type::DEPTH_BUFFER_CREATE:
    {
      m_depthTexture = gfl2::gfx::GFGL::CreateTexture(
          vramAllocator,
          m_realWidth,
          m_realHeight,
          1,
          gfl2::gfx::Usage::DepthStencil | gfl2::gfx::Usage::RenderTarget,
          desc.depthFormat,
          gfl2::gfx::Pool::Default
      );
      m_depthSurface = const_cast<gfl2::gfx::Surface*>(m_depthTexture->GetSurface());  // constをはずしている。この後gfl2::renderingengine::renderer::RenderingPipeLineのSetDepthStencil関数に渡すために。石黒さん確認済み。
      break;
    }
  }
#elif defined(GF_PLATFORM_WIN32)
  // Win32版で対応していないタイプを修正する
  if(m_desc.type == Type::DEPTH_BUFFER_RENTAL) m_desc.type = Type::DEPTH_BUFFER_CREATE;  // Win32版では借り物でデプスバッファを作成することはできない

  switch(m_desc.type)
  {
  case Type::DEPTH_BUFFER_CREATE:
    {
      m_depthTexture = gfl2::gfx::GFGL::CreateTexture(
          vramAllocator,
          512,//m_realWidth,
          256,//m_realHeight,
          1,
          gfl2::gfx::Usage::DepthStencil | gfl2::gfx::Usage::RenderTarget,
          desc.depthFormat,
          gfl2::gfx::Pool::Default
      );
      m_depthSurface = const_cast<gfl2::gfx::Surface*>(m_depthTexture->GetSurface());  // constをはずしている。この後gfl2::renderingengine::renderer::RenderingPipeLineのSetDepthStencil関数に渡すために。石黒さん確認済み。
      break;
    }
  }
#endif  // GF_PLATFORM_

  m_vramAllocator  = vramAllocator;
  m_devGLAllocator = devGLAllocator;
}

void RenderTargetTexture::Destroy(void)
{
  switch(m_desc.type)
  {
  case Type::DEPTH_BUFFER_RENTAL:
    {
/*
      gfl2::gfx::GLMemory::SetInstanceAllocator(m_devGLAllocator);
      delete m_depthSurface;  // 小文字new, deleteを使っているので、やり方を下の方法に変更した。
      gfl2::gfx::GLMemory::ClearInstanceAllocator();
      m_depthTexture = NULL;
      break;
*/

      // 上のコメントアウトしてあるやり方から変更した
      m_depthSurface = NULL;
      GFL_SAFE_DELETE(m_depthTexture); 
      break;
    }
  case Type::DEPTH_BUFFER_CREATE:
    {
      m_depthSurface = NULL;
      GFL_SAFE_DELETE(m_depthTexture);
      break;
    }
  }

  GFL_SAFE_DELETE(m_colorTexture);

  // 初期化
  m_vramAllocator  = NULL;
  m_devGLAllocator = NULL;
  m_desc           = Description();
  m_realWidth      = 0;
  m_realHeight     = 0;
  m_colorTexture   = NULL;
  m_depthTexture   = NULL;
  m_depthSurface   = NULL;
}

void RenderTargetTexture::GetColorTextureInfo(gfl2::lyt::LytResTextureInfo* textureInfo) const
{
  nw::lyt::TexFormat format = nw::lyt::TEXFORMAT_RGB565;

  switch(m_desc.colorFormat)
  {
  case gfl2::gfx::Format::A8R8G8B8:
    {
      format = nw::lyt::TEXFORMAT_RGBA8;
      break;
    }
  case gfl2::gfx::Format::X8R8G8B8:
    {
      format = nw::lyt::TEXFORMAT_RGB8;
      break;
    }
  case gfl2::gfx::Format::R5G6B5:
    {
      format = nw::lyt::TEXFORMAT_RGB565;
      break;
    }
  default:
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。フォーマットがよくない。
      break;
    }
  }

#if defined(GF_PLATFORM_CTR)
  // texObject
  uptr virtualAddr = (static_cast<gfl2::gfx::ctr::CTRTexture*>(m_colorTexture))->GetVirtualAddr();
  u32 texObject = app::util::G2DUtil::CTRCalcLytTextureInfoTexObject(virtualAddr);
  GFL_ASSERT( texObject != gfl2::lyt::LytResTextureInfo::INVALID );  // 開発中に気付かせるためのASSERT。テクスチャ用に確保されてメモリがVRAMAでもVRAMBでもなかった。

  nw::lyt::TexSize texSize(m_desc.width, m_desc.height);
  nw::lyt::TexSize realTexSize(m_realWidth, m_realHeight);

  textureInfo->Set(
    texObject,
    (static_cast<gfl2::gfx::ctr::CTRTexture*>(m_colorTexture))->GetPhysicalAddr(),
    texSize,
    realTexSize,
    format,
    nw::lyt::TEXELROTATION_NONE
  );
#elif defined(GF_PLATFORM_WIN32)
  u32 texObject = (static_cast<gfl2::gfx::winOpenGL::WinOpenGLTexture*>(m_colorTexture))->GetHandle();
  
  nw::lyt::TexSize texSize(m_desc.width, m_desc.height);
  
  textureInfo->Set(
    texObject,
    texSize,
    format
  );
#endif  // GF_PLATFORM_
}

//  テクスチャーバッファを取得
void* RenderTargetTexture::GetTextureBuffer()
{
#if defined(GF_PLATFORM_CTR)
  return  (void*)(static_cast<gfl2::gfx::ctr::CTRTexture*>(m_colorTexture))->GetVirtualAddr();
#elif defined(GF_PLATFORM_WIN32)
  return  NULL;
#endif
}

gfl2::gfx::Surface* RenderTargetTexture::GetColorSurface(void) const
{
  if(m_colorTexture)
  {
    return const_cast<gfl2::gfx::Surface*>(m_colorTexture->GetSurface());  // constをはずしている。この後gfl2::renderingengine::renderer::RenderingPipeLineのSetDepthStencil関数に渡すために。石黒さん確認済み。
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。テクスチャが生成されていない。
    return NULL;
  }
}

gfl2::gfx::Surface* RenderTargetTexture::GetDepthSurface(void) const
{
  return m_depthSurface;  // DEPTH_BUFFER_NONEのときはNULL。
}

gfl2::gfx::Texture* RenderTargetTexture::GetDepthTexture(void) const
{
  return m_depthTexture;  // DEPTH_BUFFER_NONEのときはNULL。
}

u32 RenderTargetTexture::roundUpToPowerOf2(u32 val)
{
  static const u32 VAL_MAX = 0x80000000;  // この計算で扱うことができるu32の範囲内での2のべき乗の最大値
  if(val == 0)
  {
    return 1;
  }
  else if(val <= VAL_MAX)
  {
    u32 val1 = val - 1;
    u32 val2 = val1 | (val1 >>  1);
    u32 val3 = val2 | (val2 >>  2);
    u32 val4 = val3 | (val3 >>  4);
    u32 val5 = val4 | (val4 >>  8);
    u32 val6 = val5 | (val5 >> 16);
    u32 val7 = val6 + 1;
    return val7;
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。valが大き過ぎる。
    return VAL_MAX;
  }
}


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

