#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_GFGLUtil.h>

#include <gfx/Sample/Framework/IApplication.h>
#include <gfx/include/gfl2_Surface.h>
#include <gfx/include/gfl2_TimerQuery.h>


#include <stdlib.h>

#if defined(NN_PLATFORM_CTR)
#define TEST_CTR
#else
#define TEST_WIN
#endif


#if defined(TEST_WIN)
#define TEST_LOG(str, ...) { char outputString[1024]; sprintf_s(outputString, sizeof(outputString), str, __VA_ARGS__); OutputDebugStringA(outputString); }
#else
#define TEST_LOG(str, ...) NN_LOG(str, __VA_ARGS__)
#define TEST_LOG2(str) NN_LOG(str)
#endif

using namespace gfl2;
using namespace gfl2::gfx;

class GLAllocator : public IGLAllocator
{
public:
	GLAllocator():IGLAllocator(){}
	virtual ~GLAllocator(){}

	virtual void* SystemMemoryMalloc(u32 size, u32 alignment){
		return malloc(size);
	}

	virtual void SystemMemoryFree(void* const address)
	{
		free( address );
	}

	virtual void* LocalMemoryMalloc(u32 size, u32 alignment){
		return NULL;
	}

	virtual void LocalMemoryFree(void* const address)
	{
		
	}
};

class SimpleDraw : public Sample::Framework::IApplication
{
public:
	// 定数バッファのデータ定義①　変更しないデータ
	struct cbNeverChanges {
		math::Matrix Projection;   // 透視変換行列
	};

	// 定数バッファのデータ定義②　変更頻度の低いデータ
	struct cbChangesEveryFrame {
		math::Matrix  View;  // ビュー変換行列
	};

	// 定数バッファのデータ定義③　変更頻度の高いデータ
	struct cbChangesEveryObject {
		math::Matrix World;      // ワールド変換行列
	};

	VertexBuffer				*m_pVertexBuffer;
	IndexBuffer					*m_pIndexBuffer;
	Shader							*m_pVertexShader;   // 頂点シェーダ
	Shader							*m_pPixelShader;    // ピクセル・シェーダ
	ConstantBuffer			*m_pConstantBuffer[4];

  ConstantBuffer      *m_pProjBuffer[3];
  ConstantBuffer      *m_pViewBuffer[3];

	// 定数バッファのデータ
	cbNeverChanges										m_cbNeverChanges;       // 透視変換行列
	cbChangesEveryFrame								m_cbChangesEveryFrame;  // ビュー変換行列　光源座標
	cbChangesEveryObject							m_cbChangesEveryObject; // ワールド変換行列

	BlendStateObject					*m_pBlendStateObject;    // ブレンド・ステート・オブジェクト
	RasterizerStateObject			*m_RasterizerStateObject;// ラスタライザ・ステート・オブジェクト
	DepthStencilStateObject		*m_DepthStencilStateObject;	// 深度/ステンシル・ステート・オブジェクト
	DepthStencilStateObject		*m_DepthStencilStateObject2;	// 深度/ステンシル・ステート・オブジェクト
	SamplerStateObject				*m_SamplerStateObject;				// サンプラー・ステート・オブジェクト
	Texture										*m_pTexture;    // テクスチャ

	u32												m_FrameCnt;
	gfl2::gfx::Texture				*m_pRenderTexture;
	gfl2::gfx::Texture				*m_pDepthStencilTexture;
  gfl2::gfx::DisplayList    *m_pDisplayList;

  gfl2::gfx::TimerQuery     *m_pTimerQuery;

  void*                     m_pHostIoMemory;

	SimpleDraw() : 
		Sample::Framework::IApplication(),
		m_FrameCnt(0)
	{
		
	}

	virtual ~SimpleDraw(){}

	virtual bool OnInit( void )
	{
		GFGL::Initialize( &m_Allocator );

		m_Handle = GFGL::CreateDisplay(512, 512, false, Format::A8R8G8B8, Format::D24S8, MultiSample::None);

		{
			VertexElement element[] = {
				{ 0, StreamUsage::Position,	0, VertexType::Float, 4 },
				{ 0, StreamUsage::TexCoord,	0, VertexType::Float, 4 },
				{ 0, StreamUsage::Color,		0, VertexType::Float, 4 },
				VertexElement::VERTEX_ELEMENT_END()
			};

			//GLSL
			m_pVertexShader = GFGL::CreateVertexShaderFromFile( "../data/tex_test/Nw4cTga.vert", "VS", element, NULL, FileFormat::ShaderCode );
			m_pPixelShader = GFGL::CreatePixelShaderFromFile( "../data/tex_test/Nw4cTga.frag", "PS", NULL, FileFormat::ShaderCode );
		}

		{
			struct VertexFormat{
				math::Vector				pos;
				math::Vector				uv;
				math::Vector				color;
				VertexFormat( math::Vector pos_, math::Vector uv_, math::Vector color_ )
				{
					pos = pos_;
					uv = uv_;
					color = color_;
				}
			};

			float									vscale = 1.0f;
			static const u32			vertexNum = 4;

			VertexFormat vertexDataArray[vertexNum] = {
				VertexFormat( math::Vector( -vscale,  vscale, 0.0f ), math::Vector(  0.0f, 1.0f, 0.0f ), math::Vector(1.0f,0.0f,0.0f,1.0f) ),
				VertexFormat( math::Vector(  vscale,  vscale, 0.0f ), math::Vector(  1.0f, 1.0f, 0.0f ), math::Vector(0.0f,1.0f,0.0f,1.0f) ),
				VertexFormat( math::Vector( -vscale, -vscale, 0.0f ), math::Vector(  0.0f, 0.0f, 0.0f ), math::Vector(0.0f,0.0f,1.0f,1.0f) ),
				VertexFormat( math::Vector(  vscale, -vscale, 0.0f ), math::Vector(  1.0f, 0.0f, 0.0f ), math::Vector(1.0f,1.0f,1.0f,0.0f) ),
			};

			m_pVertexBuffer = GFGL::CreateVertexBuffer( sizeof(VertexFormat), vertexNum, Usage::WriteOnly, Pool::Default, vertexDataArray );
		}

		{
			u8 idxVertexID[] = { 0, 1, 2, 3 };
			
			m_pIndexBuffer = GFGL::CreateIndexBuffer( sizeof(idxVertexID), Usage::WriteOnly, Pool::Default, IndexType::Index8, idxVertexID );
		}

		{
			ConstantBufferElement element[] = {
				{ ConstantType::Matrix, 1 },
				ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
			};

			m_pConstantBuffer[0] = GFGL::CreateConstantBuffer( element, Usage::WriteOnly, Pool::Default );
			m_pConstantBuffer[1] = GFGL::CreateConstantBuffer( element, Usage::WriteOnly, Pool::Default );
			m_pConstantBuffer[2] = GFGL::CreateConstantBuffer( element, Usage::WriteOnly, Pool::Default );
			m_pConstantBuffer[3] = GFGL::CreateConstantBuffer( element, Usage::WriteOnly, Pool::Default );

      for (u32 i = 0; i < 3; ++i)
      {
        m_pProjBuffer[i] = GFGL::CreateConstantBuffer( element, Usage::WriteOnly, Pool::Default );
        m_pViewBuffer[i] = GFGL::CreateConstantBuffer( element, Usage::WriteOnly, Pool::Default );
      }
		}

		
    Surface* pBackBuffer = GFGL::GetBackBuffer(m_Handle);
    const Surface::SurfaceDesc& backBufferDesc = pBackBuffer->GetSurfaceDesc();
		m_pRenderTexture = GFGL::CreateTexture( backBufferDesc.width, backBufferDesc.height, 1, Usage::RenderTarget, Format::A8R8G8B8, Pool::Default );

		m_pDepthStencilTexture = GFGL::CreateTextureFromSurface(GFGL::GetDepthStencilBuffer( m_Handle));

		m_pBlendStateObject = GFGL::CreateBlendStateObject();

		m_pBlendStateObject->SetAlphaTestEnable( true );
		m_pBlendStateObject->SetBlendEnable( true );
		m_pBlendStateObject->SetBlendOp( BlendOp::Add );
		m_pBlendStateObject->SetBlendFunc( BlendFunc::SrcAlpha, BlendFunc::InverseSrcAlpha );
		m_pBlendStateObject->UpdateState();

		m_RasterizerStateObject = GFGL::CreateRasterizerStateObject();
		m_RasterizerStateObject->SetCullMode( CullMode::CCW );
		//m_RasterizerStateObject->SetFillMode( FillMode::Line );
		m_RasterizerStateObject->UpdateState();

		m_DepthStencilStateObject = GFGL::CreateDepthStencilStateObject();
		m_DepthStencilStateObject->SetDepthTestEnable(false);
		m_DepthStencilStateObject->SetDepthWriteEnable(true);
		m_DepthStencilStateObject->SetStencilOp(PolygonFace::CW, StencilOp::Keep, StencilOp::Keep, StencilOp::Replace);
		m_DepthStencilStateObject->SetStencilFunc(PolygonFace::CW, CompareFunc::Always, 0x00, 0xFF);
    m_DepthStencilStateObject->SetStencilTestEnable(true);
    m_DepthStencilStateObject->SetStencilWriteMask(0xFF);
		m_DepthStencilStateObject->UpdateState();

		m_DepthStencilStateObject2 = GFGL::CreateDepthStencilStateObject();
		m_DepthStencilStateObject2->SetDepthTestEnable(false);
		m_DepthStencilStateObject2->SetDepthWriteEnable(false);
		m_DepthStencilStateObject2->SetStencilTestEnable(false);
		m_DepthStencilStateObject2->SetStencilWriteMask(0);
		m_DepthStencilStateObject2->UpdateState();

		m_SamplerStateObject = GFGL::CreateSamplerStateObject();
		m_SamplerStateObject->UpdateState();

		// **********************************************************



		ImageData				image;
		//image.ReadFromFile( "../data/tex_test/test.tga", ImageData::SwapType::OpenGL ); // 元データ
		//image.ReadFromFile( "../data/tex_test/test_rgba8.tga", ImageData::SwapType::OpenGL ); // RGBA8
		//image.ReadFromFile( "../data/tex_test/test_rgb8.tga", ImageData::SwapType::OpenGL ); // RGB8
		//image.ReadFromFile( "../data/tex_test/test_rgb565.tga", ImageData::SwapType::OpenGL ); // RGB565
		//image.ReadFromFile( "../data/tex_test/test_rgba4.tga", ImageData::SwapType::OpenGL ); // RGBA4
		//image.ReadFromFile( "../data/tex_test/test_rgb5_a1.tga", ImageData::SwapType::OpenGL ); // RGB5_A1
		//image.ReadFromFile( "../data/tex_test/test_a8.tga", ImageData::SwapType::OpenGL ); // A8
		//image.ReadFromFile( "../data/tex_test/test_a4.tga", ImageData::SwapType::OpenGL ); // A4
		//image.ReadFromFile( "../data/tex_test/test_l8.tga", ImageData::SwapType::OpenGL ); // L8
		//image.ReadFromFile( "../data/tex_test/test_l4.tga", ImageData::SwapType::OpenGL ); // L4
		//image.ReadFromFile( "../data/tex_test/test_la8.tga", ImageData::SwapType::OpenGL ); // LA8
		//image.ReadFromFile( "../data/tex_test/test_la4.tga", ImageData::SwapType::OpenGL ); // LA4
		//image.ReadFromFile( "../data/tex_test/test_hilo8.tga", ImageData::SwapType::OpenGL ); // HILO8
		//image.ReadFromFile( "../data/tex_test/test_etc1.tga", ImageData::SwapType::OpenGL ); // ETC1
		image.ReadFromFile( "../data/tex_test/test_etc1_a4.tga", ImageData::SwapType::OpenGL ); // ETC1_A4

		//image.ReadFromFile( "../data/tex_test/pm0025_00_BodyA1.tga", ImageData::SwapType::OpenGL ); // ETC1_A4
		m_pTexture = GFGL::CreateTextureFromImage( image, Pool::Default );


    // ************************************************************
		return true;
	}

	virtual bool OnUpdate( void )
	{
		return true;
	}

	virtual bool OnDraw( void )
	{
		{
      GFGL::SetActiveDisplay( m_Handle);

      Surface* pBackBuffer = GFGL::GetBackBuffer( m_Handle);
      Surface* pDepthBuffer = GFGL::GetDepthStencilBuffer( m_Handle);

      //フロントバッファへのレンダリング
			GFGL::SetRenderTarget( pBackBuffer );
			GFGL::SetDepthStencil( pDepthBuffer );

			GFGL::BeginScene();

      // ビューポートとシザーテストの設定
      u32 displayWidth = GFGL::GetDisplayWidth(m_Handle);
      u32 displayHeight = GFGL::GetDisplayHeight(m_Handle);

			GFGL::SetViewport(0, 0, displayWidth, displayHeight);
			GFGL::SetScissor(0, 0, displayWidth, displayHeight);

			GFGL::ClearRenderTarget( pBackBuffer, Color(0.3f, 0.3f, 0.3f, 0.3f));
			GFGL::ClearDepthStencil( pDepthBuffer, 1.0f, 255 );

			GFGL::SetBlendState( m_pBlendStateObject );
      GFGL::SetDepthStencilState(m_DepthStencilStateObject2);

			GFGL::SetVertexShader( m_pVertexShader );
			GFGL::SetPixelShader( m_pPixelShader );

			GFGL::SetTexture( 0, m_pTexture );
			GFGL::SetSamplerState( 0, m_SamplerStateObject );

			GFGL::SetIndices( m_pIndexBuffer );
			GFGL::SetStreamSource( 0, m_pVertexBuffer );

			//　描画
			GFGL::DrawIndexedPrimitive( PrimitiveType::TriangleStrip, 0, 4	);

			GFGL::EndScene();

      // ディスプレイバッファに転送
      GFGL::TransferDisplay();

		}

		GFGL::SwapDisplay();

		return true;
	}

	virtual bool OnTerm( void )
	{
		GFGL::Finish();

		delete m_pRenderTexture;
		delete m_pTexture;
		delete m_SamplerStateObject;
		delete m_DepthStencilStateObject;
		delete m_RasterizerStateObject;
		delete m_pBlendStateObject;

		delete m_pIndexBuffer;
		delete m_pVertexBuffer;
		delete m_pConstantBuffer[2];
		delete m_pConstantBuffer[1];
		delete m_pConstantBuffer[0];
		delete m_pVertexShader;
		delete m_pPixelShader;

		GFGL::SetActiveDisplay(NULL);

    GFGL::DestroyDisplay(m_Handle);
		GFGL::Terminate();

		return true;
	}

private:
	DisplayDeviceHandle					m_Handle;
	GLAllocator									m_Allocator;

	//いずれＩＯクラスができたら置き換える。
	static c8* ReadFile( const char* filename, s32* p_size, s32 align = 16 )
	{
#if 0
		FILE		*hFile;
		int			file_size;
		char		*read_buff;

		fopen_s( &hFile, filename, "rb" );

		if ( hFile == NULL ){
			assert(0);
			return NULL;
		}

		fseek( hFile, 0, SEEK_END );
		file_size = ftell( hFile );
		*p_size = file_size;
		fseek( hFile, 0, SEEK_SET );

		file_size += align;//必ずNULLが入るようにしたい。
		u32 buf_size = file_size + ((-file_size) & (align-1));

		read_buff = reinterpret_cast<c8*>( GLMemory::Malloc(buf_size) );
		memset( read_buff, 0, buf_size );
		fread( read_buff, file_size, 1, hFile );
		fclose( hFile );

	
		return( read_buff );
#endif
    return NULL;
	}
};

SimpleDraw	*g_pApp = NULL;

Sample::Framework::IApplication* GetApplication()
{
	if( g_pApp == NULL ) {
		g_pApp = new SimpleDraw();
	}
	return g_pApp;
}

void TerminateApplication()
{
	delete g_pApp;
}
