#include <clr/include/gfx/gfl2_GFGL_CLR.h>
#include <gfx/include/gfl2_GFGL.h>
#include <windows.h>

using System::IntPtr;
using System::Runtime::InteropServices::Marshal;

namespace gfl2 { namespace clr { namespace gfx {

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
 void GFGL::Initialize()
{
  m_pGLAllocator = new GLAllocatorNative();
  gfl2::gfx::GFGL::Initialize(m_pGLAllocator);
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void GFGL::Terminate()
{
	if ( m_pGLAllocator == NULL )
  {
    return;
  }

	gfl2::gfx::GFGL::SetActiveDisplay(NULL);
	gfl2::gfx::GFGL::Terminate();
}

u32 GFGL::GetNativeClientWidth(System::IntPtr hWnd)
{
  HWND NativeHandle = reinterpret_cast<HWND>(hWnd.ToPointer());
  RECT rect;
  GetClientRect(NativeHandle, &rect);
  return (rect.right - rect.left);
}

u32 GFGL::GetNativeClientHeight(System::IntPtr hWnd)
{
  HWND NativeHandle = reinterpret_cast<HWND>(hWnd.ToPointer());
  RECT rect;
  GetClientRect(NativeHandle, &rect);
  return (rect.bottom - rect.top);
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
DisplayDeviceHandle^ GFGL::CreateDisplay(
  System::IntPtr hWnd,
  gfl2::clr::gfx::Format::Enum colorFormat,
  gfl2::clr::gfx::Format::Enum depthStencilFormat,
  gfl2::clr::gfx::MultiSample::Enum multiSample)
{
	HWND										NativeHandle = reinterpret_cast< HWND >( hWnd.ToPointer() );

	gfl2::gfx::DisplayDeviceHandle			nativeHandle;

	nativeHandle = gfl2::gfx::GFGL::CreateDisplayEx(
    gfl2::clr::gfx::GFGL::GetGLAllocator(),
    NativeHandle,
    static_cast<gfl2::gfx::Format::Enum>(colorFormat),
    static_cast<gfl2::gfx::Format::Enum>(depthStencilFormat),
    static_cast<gfl2::gfx::MultiSample::Enum>(multiSample)
    );

	DisplayDeviceHandle^		dispHandle = gcnew DisplayDeviceHandle( nativeHandle );

	return dispHandle;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void GFGL::DestroyDisplay( DisplayDeviceHandle^ hDisplay )
{
	gfl2::gfx::GFGL::DestroyDisplay( hDisplay->GetNative() );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
bool GFGL::SetActiveDisplay( DisplayDeviceHandle^ hDisplay )
{
  if (hDisplay == nullptr)
  {
  	return gfl2::gfx::GFGL::SetActiveDisplay( NULL ) ? true : false;
  }
	return gfl2::gfx::GFGL::SetActiveDisplay( hDisplay->GetNative() ) ? true : false;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
Surface^ GFGL::GetBackBuffer( DisplayDeviceHandle^ hDisplay )
{
	gfl2::gfx::Surface				*pNative;
	pNative = gfl2::gfx::GFGL::GetBackBuffer( hDisplay->GetNative() );

	Surface^			surface = gcnew Surface( pNative, true );
	return surface;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
Surface^ GFGL::GetDepthStencilBuffer( DisplayDeviceHandle^ hDisplay )
{
	gfl2::gfx::Surface				*pNative;
	pNative = gfl2::gfx::GFGL::GetDepthStencilBuffer( hDisplay->GetNative() );

	Surface^			surface = gcnew Surface( pNative, true );
	return surface;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
Shader^ GFGL::CreateVertexShaderFromSource( System::String^ code, System::String^ entry, array<VertexElement^>^ pVertexElements, array<System::String^>^ args )
{
	IntPtr												ptr;
	const char										*code_ = NULL;
	const char										*entry_ = NULL;
	const char										**args_ = NULL;
	gfl2::gfx::VertexElement			*pVertexElements_ = NULL;

	ptr = Marshal::StringToHGlobalAnsi( code );
	code_ = (const char*)ptr.ToPointer();

	ptr = Marshal::StringToHGlobalAnsi( entry );
	entry_ = (const char*)ptr.ToPointer();

	if ( pVertexElements && pVertexElements->Length)
	{
		pVertexElements_ = new gfl2::gfx::VertexElement[pVertexElements->Length];

		for( int i = 0; i < pVertexElements->Length; ++i )
		{
			pVertexElements_[i] = *pVertexElements[i]->GetNative();
		}
	}

	if ( args && args->Length)
	{
		args_ = new const char*[args->Length];

		for( int i = 0; i < args->Length; ++i ){
			ptr = Marshal::StringToHGlobalAnsi( args[i] );
			args_[i] = (const char*)ptr.ToPointer();
		}
	}

	gfl2::gfx::Shader*				pNative = NULL;

  pNative = gfl2::gfx::GFGL::CreateVertexShaderFromSource(GFGL::GetGLAllocator(), code_, entry_, pVertexElements_, args_ );

	Shader^			pShader = gcnew Shader( pNative );

	if ( args && args->Length)
	{
		for( int i = 0; i < args->Length; ++i )
		{
			Marshal::FreeHGlobal( IntPtr((void*)args_[i]) );
		}
		if( args_ )
			delete[] args_;
	}
	if ( pVertexElements_ )
		delete[] pVertexElements_;
	Marshal::FreeHGlobal( IntPtr((void*)entry_) );
	Marshal::FreeHGlobal( IntPtr((void*)code_) );
	
	return pShader;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
Shader^ GFGL::CreatePixelShaderFromSource( System::String^ code, System::String^ entry, array<System::String^>^ args )
{
	IntPtr												ptr;
	const char										*code_ = NULL;
	const char										*entry_ = NULL;
	const char										**args_ = NULL;

	ptr = Marshal::StringToHGlobalAnsi( code );
	code_ = (const char*)ptr.ToPointer();

	ptr = Marshal::StringToHGlobalAnsi( entry );
	entry_ = (const char*)ptr.ToPointer();

	if ( args && args->Length)
	{
		args_ = new const char*[args->Length];

		for( int i = 0; i < args->Length; ++i ){
			ptr = Marshal::StringToHGlobalAnsi( args[i] );
			args_[i] = (const char*)ptr.ToPointer();
		}
	}

	gfl2::gfx::Shader*				pNative;

	pNative = gfl2::gfx::GFGL::CreatePixelShaderFromSource(GFGL::GetGLAllocator(), code_, entry_, args_ );

	Shader^			pShader = gcnew Shader( pNative );

	if ( args && args->Length)
	{
		for( int i = 0; i < args->Length; ++i )
		{
			Marshal::FreeHGlobal( IntPtr((void*)args_[i]) );
		}
		if( args_ )
			delete[] args_;
	}
	Marshal::FreeHGlobal( IntPtr((void*)entry_) );
	Marshal::FreeHGlobal( IntPtr((void*)code_) );
	
	return pShader;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
ConstantBuffer^ GFGL::CreateConstantBuffer( array<ConstantBufferElement^>^	pConstantBufferElements, u32 usage, Pool::Enum pool )
{
	gfl2::gfx::ConstantBufferElement				*pConstantBufferElements_ = NULL;

	if ( pConstantBufferElements && pConstantBufferElements->Length)
	{
		pConstantBufferElements_ = new gfl2::gfx::ConstantBufferElement[pConstantBufferElements->Length];

		for( int i = 0; i < pConstantBufferElements->Length; ++i ){
			pConstantBufferElements_[i] = *pConstantBufferElements[i]->GetNative();
		}
	}

	gfl2::gfx::ConstantBuffer*				pNative;
  pNative = gfl2::gfx::GFGL::CreateConstantBuffer(GFGL::GetGLAllocator(), pConstantBufferElements_, usage, static_cast<gfl2::gfx::Pool>( (u32)pool ) );

	ConstantBuffer^				pConstantBuffer = gcnew ConstantBuffer(pNative);

	if ( pConstantBufferElements && pConstantBufferElements->Length)
	{
		if( pConstantBufferElements_ )
			delete[] pConstantBufferElements_;
	}

	return pConstantBuffer;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
VertexBuffer^ GFGL::CreateVertexBuffer(u32 size, u32 vertex_num, u32 usage, Pool::Enum pool, array<byte>^ buffer )
{
	pin_ptr<byte>		buffer_ = &buffer[0];
	gfl2::gfx::Pool	pool_ = static_cast<gfl2::gfx::Pool>( (u32)pool);

	gfl2::gfx::VertexBuffer*				pNative;
	pNative = gfl2::gfx::GFGL::CreateVertexBuffer(GFGL::GetGLAllocator(), size, vertex_num, usage, pool_, buffer_ );

	VertexBuffer^				pVertexBuffer = gcnew VertexBuffer(pNative);
	return pVertexBuffer;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
IndexBuffer^ GFGL::CreateIndexBuffer( u32 size, u32	usage, Pool::Enum pool, IndexType::Enum type, array<byte>^ buffer )
{
	pin_ptr<byte>					buffer_ = &buffer[0];
	gfl2::gfx::Pool				pool_ = static_cast<gfl2::gfx::Pool>( (u32)pool);
	gfl2::gfx::IndexType	type_ = static_cast<gfl2::gfx::IndexType>( (u32)type);

	gfl2::gfx::IndexBuffer*				pNative;
	pNative = gfl2::gfx::GFGL::CreateIndexBuffer(GFGL::GetGLAllocator(), size, usage, pool_, type_, buffer_ );

	IndexBuffer^				pVertexBuffer = gcnew IndexBuffer(pNative);
	return pVertexBuffer;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
BlendStateObject^ GFGL::CreateBlendStateObject()
{
	gfl2::gfx::BlendStateObject*				pNative;

	pNative = gfl2::gfx::GFGL::CreateBlendStateObject(GFGL::GetGLAllocator());

	BlendStateObject^				pObject = gcnew BlendStateObject(pNative);
	return pObject;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
RasterizerStateObject^ GFGL::CreateRasterizerStateObject()
{
	gfl2::gfx::RasterizerStateObject*				pNative;

	pNative = gfl2::gfx::GFGL::CreateRasterizerStateObject(GFGL::GetGLAllocator());

	RasterizerStateObject^				pObject = gcnew RasterizerStateObject(pNative);
	return pObject;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
DepthStencilStateObject^ GFGL::CreateDepthStencilStateObject()
{
	gfl2::gfx::DepthStencilStateObject*				pNative;

	pNative = gfl2::gfx::GFGL::CreateDepthStencilStateObject(GFGL::GetGLAllocator());

	DepthStencilStateObject^				pObject = gcnew DepthStencilStateObject(pNative);
	return pObject;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
SamplerStateObject^ GFGL::CreateSamplerStateObject()
{
	gfl2::gfx::SamplerStateObject*				pNative;

	pNative = gfl2::gfx::GFGL::CreateSamplerStateObject(GFGL::GetGLAllocator());

	SamplerStateObject^				pObject = gcnew SamplerStateObject(pNative);
	return pObject;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
Texture^ GFGL::CreateTextureFromImage( ImageData^ rImage, Pool::Enum pool)
{
	gfl2::gfx::Texture*				pNative;
	gfl2::gfx::Pool													pool_ = static_cast<gfl2::gfx::Pool>( (u32)pool );

	pNative = gfl2::gfx::GFGL::CreateTextureFromImage(GFGL::GetGLAllocator(), *(rImage->GetNative()), pool_ );

	Texture^				pObject = gcnew Texture(pNative);
	return pObject;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
bool GFGL::SetRenderTarget(unsigned int index, Surface^ const pSurface)
{
	return gfl2::gfx::GFGL::SetRenderTarget(index, pSurface->GetNative() ) ? true : false;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
bool GFGL::SetDepthStencil(Surface^ const pSurface)
{
	return gfl2::gfx::GFGL::SetDepthStencil(pSurface->GetNative() ) ? true : false;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void GFGL::SetViewport(const s32 xp, const s32 yp, const s32 width, const s32 height)
{
	gfl2::gfx::GFGL::SetViewport(xp, yp, width, height);
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
bool GFGL::BeginScene()
{
	return gfl2::gfx::GFGL::BeginScene() ? true : false;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void GFGL::ClearRenderTarget( Surface^ pSurface, Color^ clearColor )
{
	gfl2::gfx::GFGL::ClearRenderTarget( pSurface->GetNative(), *(clearColor->GetNative()) );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void GFGL::ClearDepthStencil( Surface^ pSurface, f32 depth, u8 stencil )
{
	gfl2::gfx::GFGL::ClearDepthStencil( pSurface->GetNative(), depth, stencil );
}

void GFGL::SetVertexShader( Shader^ pShader )
{
	gfl2::gfx::GFGL::SetVertexShader( pShader->GetNative() );
}

void GFGL::SetPixelShader( Shader^ pShader )
{
	gfl2::gfx::GFGL::SetPixelShader( pShader->GetNative() );
}

void GFGL::SetVertexShaderConstant(u32	registerNo, ConstantBuffer ^pBuffer	)
{
	gfl2::gfx::GFGL::SetVertexShaderConstant( registerNo, pBuffer->GetNative()	);
}

void GFGL::SetPixelShaderConstant(u32	registerNo, ConstantBuffer ^pBuffer	)
{
	gfl2::gfx::GFGL::SetPixelShaderConstant( registerNo, pBuffer->GetNative()	);
}

void GFGL::SetIndices( IndexBuffer^ pIndices)
{
	gfl2::gfx::GFGL::SetIndices( pIndices->GetNative() );
}

void GFGL::SetStreamSource( u32 index, VertexBuffer^ pVertexBuffer)
{
	gfl2::gfx::GFGL::SetStreamSource( index, pVertexBuffer->GetNative() );
}

void GFGL::DrawPrimitive( PrimitiveType::Enum   primitive, u32 start, u32	count )
{
	gfl2::gfx::PrimitiveType   primitive_ = static_cast<gfl2::gfx::PrimitiveType>((u32)primitive);
	gfl2::gfx::GFGL::DrawPrimitive( primitive_, start, count );
}

void GFGL::DrawIndexedPrimitive( PrimitiveType::Enum primitive, u32 start, u32 count )
{
	gfl2::gfx::PrimitiveType   primitive_ = static_cast<gfl2::gfx::PrimitiveType>((u32)primitive);
	gfl2::gfx::GFGL::DrawIndexedPrimitive( primitive_, start, count );
}

void GFGL::SetBlendState( BlendStateObject ^pState )
{
	gfl2::gfx::GFGL::SetBlendState( pState->GetNative() );
}

void GFGL::SetRasterizerState( RasterizerStateObject ^pState )
{
	gfl2::gfx::GFGL::SetRasterizerState( pState->GetNative() );
}

void GFGL::SetDepthStencilState( DepthStencilStateObject ^pState )
{
	gfl2::gfx::GFGL::SetDepthStencilState( pState->GetNative() );
}

void GFGL::SetSamplerState( u32 texNo, SamplerStateObject ^pState )
{
	gfl2::gfx::GFGL::SetSamplerState( texNo, pState->GetNative() );
}

void GFGL::SetTexture( u32 texNo, Texture ^pTexture )
{
	gfl2::gfx::GFGL::SetTexture( texNo, pTexture->GetNative() );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void GFGL::EndScene()
{
	gfl2::gfx::GFGL::EndScene();
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void GFGL::Finish()
{
	gfl2::gfx::GFGL::Finish();
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void GFGL::SwapDisplay( bool fVsyncWait )
{
	gfl2::gfx::GFGL::SwapDisplay( fVsyncWait );
}


}}}