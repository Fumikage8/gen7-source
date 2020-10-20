#ifndef GFLIB2_GRX_CLR_GFGL_CLR_H_INCLUDED
#define GFLIB2_GRX_CLR_GFGL_CLR_H_INCLUDED

#using <System.Drawing.dll>

#include <clr/include/gfx/gfl2_Texture_CLR.h>
#include <clr/include/gfx/gfl2_ImageData_CLR.h>
#include <clr/include/gfx/gfl2_StateObject_CLR.h>
#include <clr/include/gfx/gfl2_Buffer_CLR.h>
#include <clr/include/gfx/gfl2_Color_CLR.h>
#include <clr/include/gfx/gfl2_Surface_CLR.h>
#include <clr/include/gfx/gfl2_Shader_CLR.h>
#include <clr/include/gfx/gfl2_types_CLR.h>
#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace clr { namespace gfx {

/**
 * @brief ネイティブのアロケータクラス
 */
class GLAllocatorNative : public gfl2::gfx::IGLAllocator
{
public:
	GLAllocatorNative():IGLAllocator(){}
	virtual ~GLAllocatorNative(){}

#if GFL_DEBUG
  virtual void* _SystemMemoryMallocAlign(u32 size, u32 alignment,const char *filename , const int linenum)
#else
  virtual void* SystemMemoryMallocAlign(u32 size, u32 alignment);
#endif
  {
		return malloc(size);
	}

	virtual void SystemMemoryFree(void* const address)
	{
		free( address );
	}

	virtual void* LocalMemoryMalloc(u32 size, u32 alignment)
  {
		return NULL;
	}

	virtual void LocalMemoryFree(void* const address)
	{
		
	}
};

/**
 * @brief C#上で扱うためのアロケータクラス
 */
public ref class GLAllocator
{
public:
  /**
   * @brief コンストラクタ
   */
  GLAllocator()
  {
    m_pNative = new GLAllocatorNative();
  }

  /**
   * @brief デストラクタ
   */
  ~GLAllocator()
  {
    this->!GLAllocator();
  }

  /**
   * @brief ファイナライザ
   */
  !GLAllocator()
  {
    if (m_pNative)
    {
      delete m_pNative;
      m_pNative = NULL;
    }
  }

#if GFL_DEBUG
  virtual void* _SystemMemoryMallocAlign(u32 size, u32 alignment,const char *filename , const int linenum)
#else
  virtual void* SystemMemoryMallocAlign(u32 size, u32 alignment);
#endif
  {
    //u32 alignment = 4;
    return m_pNative->SystemMemoryMalloc(size, alignment);
	}

	void SystemMemoryFree(void* const address)
	{
    m_pNative->SystemMemoryFree(address);
	}

	void* LocalMemoryMalloc(u32 size)
  {
    u32 alignment = 4;
    return m_pNative->LocalMemoryMalloc(size, alignment);
	}

	void LocalMemoryFree(void* const address)
	{
    m_pNative->LocalMemoryFree(address);
	}

  GLAllocatorNative* GetNative()
  {
    return m_pNative;
  }

private:
  GLAllocatorNative* m_pNative;
};

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■		デバイスクラス
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
public ref class DisplayDeviceHandle{
public:

	DisplayDeviceHandle( gfl2::gfx::DisplayDeviceHandle nativeHandle ){ m_NativeHandle = nativeHandle; }
	~DisplayDeviceHandle(){this->!DisplayDeviceHandle();}
	!DisplayDeviceHandle(){m_NativeHandle = NULL;}

	gfl2::gfx::DisplayDeviceHandle GetNative(){ return m_NativeHandle; }

private:

	gfl2::gfx::DisplayDeviceHandle			m_NativeHandle;
};

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■		デバイスクラス
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
public ref class GFGL
{
public:

  static void Initialize();
	static void Terminate();

  static u32 GetNativeClientWidth(System::IntPtr hWnd);
  static u32 GetNativeClientHeight(System::IntPtr hWnd);

  static DisplayDeviceHandle^ CreateDisplay( System::IntPtr hWnd, gfl2::clr::gfx::Format::Enum colorFormat, gfl2::clr::gfx::Format::Enum depthStencilFormat, gfl2::clr::gfx::MultiSample::Enum multiSample);
	static void DestroyDisplay( DisplayDeviceHandle^ hDisplay );

	static bool SetActiveDisplay( DisplayDeviceHandle^ hDisplay );

	static Surface^ GetBackBuffer( DisplayDeviceHandle^ hDisplay );
	static Surface^ GetDepthStencilBuffer( DisplayDeviceHandle^ hDisplay );

	static Shader^ GFGL::CreateVertexShaderFromSource( System::String^ code, System::String^ entry, array<VertexElement^>^ pVertexElements, array<System::String^>^ args );
	static Shader^ GFGL::CreatePixelShaderFromSource( System::String^ code, System::String^ entry, array<System::String^>^ args );

	static ConstantBuffer^ CreateConstantBuffer(
			array<ConstantBufferElement^>^	pConstantBufferElements,
			u32															usage,
			Pool::Enum											pool
		);

	static VertexBuffer^ CreateVertexBuffer(u32 size, u32 vertex_num, u32 usage, Pool::Enum pool, array<byte>^ buffer );
	static IndexBuffer^ CreateIndexBuffer( u32 size, u32	usage, Pool::Enum pool, IndexType::Enum type, array<byte>^ buffer );

	static BlendStateObject^ CreateBlendStateObject();
	static RasterizerStateObject^ CreateRasterizerStateObject();
	static DepthStencilStateObject^ CreateDepthStencilStateObject();
	static SamplerStateObject^ CreateSamplerStateObject();

	static Texture^ CreateTextureFromImage( ImageData^ rImage, Pool::Enum pool);

	static bool SetRenderTarget(unsigned int index, Surface^ const pSurface);
	static bool SetRenderTarget(Surface^ const pSurface){return SetRenderTarget( 0, pSurface );}
	static bool SetDepthStencil(Surface^ const pSurface);

	static void SetViewport(const s32 xp, const s32 yp, const s32 width, const s32 height);

	static bool BeginScene();
	static void EndScene();

	static void ClearRenderTarget( Surface^ pSurface, Color^ clearColor );
	static void ClearDepthStencil( Surface^ pSurface ){ ClearDepthStencil( pSurface, 1.0f, 0xFF ); }
	static void ClearDepthStencil( Surface^ pSurface, f32 depth, u8 stencil );

	static void SetVertexShader( Shader^ pShader );
	static void SetPixelShader( Shader^ pShader );

	static void SetVertexShaderConstant(u32	registerNo, ConstantBuffer ^pBuffer	);
	static void SetPixelShaderConstant(u32	registerNo, ConstantBuffer ^pBuffer	);

	static void SetIndices( IndexBuffer^ pIndices);
	static void SetStreamSource( u32 index, VertexBuffer^ pVertexBuffer);

	static void DrawPrimitive( PrimitiveType::Enum   primitive, u32 start, u32	count );
	static void DrawIndexedPrimitive( PrimitiveType::Enum primitive, u32 start, u32 count );

	static void SetBlendState( BlendStateObject ^pState );
	static void SetRasterizerState( RasterizerStateObject ^pState );
	static void SetDepthStencilState( DepthStencilStateObject ^pState );
	static void SetSamplerState( u32 texNo, SamplerStateObject ^pState );
	static void SetTexture( u32 texNo, Texture ^pTexture );

	static void Finish();

	static void SwapDisplay(){ SwapDisplay(true); }
	static void SwapDisplay( bool fVsyncWait );

  static GLAllocatorNative* GetGLAllocator() { return m_pGLAllocator; }

private:
	GFGL(){}
	~GFGL(){this->!GFGL();}
	!GFGL(){}

  static GLAllocatorNative* m_pGLAllocator;
};

}}}

#endif