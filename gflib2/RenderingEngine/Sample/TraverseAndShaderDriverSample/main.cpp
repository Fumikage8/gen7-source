#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
//#include <math/include/gfl2_MathUtil.h>
#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_GFGLUtil.h>

#include <RenderingEngine/Sample/Framework/IApplication.h>

#include <util/include/gfl2_FixedSizeContainer.h>

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/renderer/gfl2_DrawManager.h>
#include <system/include/InputDevice/gfl2_Pad.h>

//#include <boost/smart_ptr/shared_ptr.hpp>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::system;
using namespace gfl2::renderingengine;

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

class AdditionalShaderDriver : public gfx::GLMemory, public renderer::IShaderDriver
{
public:

	struct AdditionalConstants
	{
		math::Vector	cMaskColorA;
		math::Vector	cMaskColorB;

		static const gfx::ConstantBufferElement* GetConstantBufferElement()
		{
			static const gfx::ConstantBufferElement element[] = {
				{ gfx::ConstantType::Vector, 1 },
				{ gfx::ConstantType::Vector, 1 },
				gfx::ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
			};
			return element;
		}

		void Set( const renderer::RenderState& rRenderState, const scenegraph::resource::MaterialResourceNode *pMaterial )
		{

		}
	};

	AdditionalShaderDriver():gfx::GLMemory(),renderer::IShaderDriver()
	{
		m_AdditionalConstantsBuffer = GFGL::CreateConstantBuffer( AdditionalConstants::GetConstantBufferElement(), Usage::WriteOnly, Pool::Default );
		m_AdditionalConstants.cMaskColorA.Set(1, 0, 0, 1);
		m_AdditionalConstants.cMaskColorB.Set(1, 1, 0, 1);
	}

	virtual ~AdditionalShaderDriver()
	{
		if ( m_AdditionalConstantsBuffer )
			delete m_AdditionalConstantsBuffer;

		delete pOrigDriver;//ここはReleaseになる。今はスマートポインタになっていないので、仮でdelete。
	}

	virtual b32 Apply( const renderer::RenderState& rRenderState, const renderer::MeshDrawTag *pMeshDrawTag ) const
	{
		pOrigDriver->Apply( rRenderState );

		m_AdditionalConstantsBuffer->Update( &m_AdditionalConstants, sizeof(AdditionalConstants) );

		GFGL::SetPixelShaderConstant( 7, m_AdditionalConstantsBuffer );
		GFGL::SetTexture( 8, m_pTexture );

		return true;
	}

	renderer::IShaderDriver												*pOrigDriver;
	gfx::Texture																	*m_pTexture;
	gfx::ConstantBuffer														*m_AdditionalConstantsBuffer;
	AdditionalConstants														m_AdditionalConstants;
};

class SimpleDraw : public Sample::Framework::IApplication
{
public:
	static const u32																					MODEL_MAX = 2;
	math::Matrix																							m_Projection;   // 透視変換行列
	math::Matrix																							m_View;  // ビュー変換行列
	math::Matrix																							m_World;      // ワールド変換行列

	renderer::DrawManager																			*m_pDrawManager;
	scenegraph::resource::ResourceNode												*m_pRootNode;
	scenegraph::instance::ModelInstanceNode										*m_pModelInstanceNode[MODEL_MAX];
	gfx::Texture																							*m_pTextureMask; // テクスチャ
	util::FixedSizeContainer<AdditionalShaderDriver*, 10>			m_AdditionalShaderDriverContainer;
	scenegraph::resource::MaterialResourceNode								*m_pTopsMaterialResourceNode;

	s32																					m_FrameCnt;
	b32																					m_EnableMaskA;
	b32																					m_EnableMaskB;

	SimpleDraw() : 
		Sample::Framework::IApplication(),
		m_FrameCnt(0),
		m_EnableMaskA( true ),
		m_EnableMaskB( false )
	{
	
	}

	virtual ~SimpleDraw(){}

	virtual bool OnInit( void )
	{
		GFGL::Initialize( &m_Allocator );
		InputDevice::Pad::Initialize();

		m_Handle = GFGL::CreateDisplay( DisplayMode::HD720P, Format::A8R8G8B8, Format::D24S8, MultiSample::MSAA4X );

		{//レンダリングエンジンの初期化
			//全ノードを管理するマネージャ初期化。機能を使わないのであれば必須でない。
			scenegraph::SceneGraphManager::Initialize();

			//リソースノードを生成するマネージャ初期化
			scenegraph::resource::ResourceManager::Initialize();
		
			//描画マネージャの初期化。複数持ってレイヤー化してもよい。
			m_pDrawManager = renderer::DrawManager::Create();
		}

		{
		
			//リソースローダー経由でノードは生成されます。
			scenegraph::resource::GfMdlData							resourceData;
			resourceData.ReadFile( "C:/home/niji/git_gflib2/RenderingEngine/Sample/data/kisekae/tr0001_01.gfmdl" );

			//m_pRootNodeには、様々なリソースがぶら下がっています。
			m_pRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

			{//トラバーサーサンプル（指定ノード以下にある欲しいノードの取得方法）
				class MaterialResourceNodeVisitor
				{
				public:
					MaterialResourceNodeVisitor():m_pTargetMaterialNode(NULL){}
					virtual ~MaterialResourceNodeVisitor() {}

					void Visit( scenegraph::resource::MaterialResourceNode* pNode )
					{
						if ( pNode->GetNameHash() == util::ConstHash< 't','r','0','0','0','1','_','T','t','o','p','s' >::value && 
								 strcmp( pNode->GetName(), "tr0001_Ttops" ) == 0 )
						{
							m_pTargetMaterialNode = pNode;
						}
					}

					scenegraph::resource::MaterialResourceNode		*m_pTargetMaterialNode;
				};

				scenegraph::Traverser::Visitor<scenegraph::resource::MaterialResourceNode, MaterialResourceNodeVisitor>				visitor;
				scenegraph::Traverser::Traverse<scenegraph::resource::MaterialResourceNode, MaterialResourceNodeVisitor>( m_pRootNode, &visitor );

				m_pTopsMaterialResourceNode = visitor.m_pTargetMaterialNode;
			}
		}

		//同じリソースノードからインスタンスノードを２つ生成
		m_pModelInstanceNode[0] = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pRootNode );
		m_pModelInstanceNode[1] = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pRootNode );

		{//カスタムシェーダーを当てる方法

			{//カスタムシェーダー用テクスチャ
				ImageData				image;
				image.ReadFromFile( "C:/home/niji/git_gflib2/RenderingEngine/Sample/data/tr0001/Textures/tr0001_Ttops_m.tga" );
				m_pTextureMask = GFGL::CreateTextureFromImage( image, Pool::Default );
			}

			for( u32 modelNo = 0; modelNo < MODEL_MAX; ++modelNo )
			{
				u32		materialNum = m_pModelInstanceNode[modelNo]->GetMaterialNum();

				for( u32 materialNo = 0; materialNo < materialNum; ++materialNo )
				{
					scenegraph::instance::MaterialInstanceNode				*pMaterialInstanceNode;

					pMaterialInstanceNode = m_pModelInstanceNode[modelNo]->GetMaterialInstanceNode( materialNo );

					//ConstHashの使い方
					if ( pMaterialInstanceNode->GetNameHash() == util::ConstHash< 't','r','0','0','0','1','_','T','t','o','p','s' >::value && 
							 strcmp( pMaterialInstanceNode->GetName(), "tr0001_Ttops" ) == 0 )
					{
						AdditionalShaderDriver			*pAdditionalShaderDriver = new AdditionalShaderDriver();
						pAdditionalShaderDriver->pOrigDriver = pMaterialInstanceNode->GetShaderDriver();						
						pAdditionalShaderDriver->m_pTexture = m_pTextureMask;
						pMaterialInstanceNode->SetShaderDriver( pAdditionalShaderDriver );

						m_AdditionalShaderDriverContainer.push_back( pAdditionalShaderDriver );
					}
				}

				{//新しいシェーダードライバを使うよう、DrawTagを更新します。
					m_pModelInstanceNode[modelNo]->SetUpDrawTag();
				}
			}
		}

		//描画マネージャ経由で描画する場合は、追加します。インスタンスよりメッシュ情報を手動で引っ張ってきて自前で描画することもできます。
		m_pDrawManager->AddObject( m_pModelInstanceNode[0] );
		//m_pDrawManager->AddObject( m_pModelInstanceNode[1] );

		{
			// 射影変換行列(パースペクティブ(透視法)射影)
			m_Projection = math::Matrix::GetPerspective(
					math::Util::ConvDegToRad(37.0f),		// 視野角30°
					(float)GFGLUtil::GetDisplayWidthByDisplayMode( DisplayMode::HD720P ) / (float)GFGLUtil::GetDisplayHeightByDisplayMode( DisplayMode::HD720P ),	// アスペクト比
					1.1f,							// 前方投影面までの距離
					1000.0f);							// 後方投影面までの距離

			m_pDrawManager->SetProjectionMatrix( m_Projection );
		}

		return true;
	}

	virtual bool OnUpdate( void )
	{
		InputDevice::Pad::Update();

		for( u32 padNo = 0; padNo < InputDevice::Pad::MAX_USE_PAD; ++padNo )
		{
			if ( !InputDevice::Pad::IsConnecting( padNo ) )
				break;

			if( InputDevice::Pad::IsPush( padNo, InputDevice::Button::R1 ) )
			{
				m_EnableMaskA = true;
				m_EnableMaskB = false;
			}
			else if( InputDevice::Pad::IsPush( padNo, InputDevice::Button::L1 ) )
			{
				m_EnableMaskA = false;
				m_EnableMaskB = true;
			}
				
			math::Vector			color(0, 0, 0, 0);
			if( InputDevice::Pad::IsPress( padNo, InputDevice::Button::B ) )
			{
				color.x = 1.0f;
			}
			else if( InputDevice::Pad::IsPress( padNo, InputDevice::Button::A ) )
			{
				color.y = 1.0f;
			}
			else if( InputDevice::Pad::IsPress( padNo, InputDevice::Button::X ) )
			{
				color.z = 1.0f;
			}
			else if( InputDevice::Pad::IsPress( padNo, InputDevice::Button::Y ) )
			{
				color.w = 1.0f;
			}

			color /= 60.0f;

			if( InputDevice::Pad::IsPress( padNo, InputDevice::Button::Up ) )
			{
				for( u32 i = 0; i < m_AdditionalShaderDriverContainer.size(); ++i )
				{
					if ( m_EnableMaskA )
						m_AdditionalShaderDriverContainer[i]->m_AdditionalConstants.cMaskColorA += color;

					if ( m_EnableMaskB )
						m_AdditionalShaderDriverContainer[i]->m_AdditionalConstants.cMaskColorB += color;

					for( u32 elm = 0; elm < 4; ++elm )
					{
						m_AdditionalShaderDriverContainer[i]->m_AdditionalConstants.cMaskColorA[elm] = math::min<f32>( m_AdditionalShaderDriverContainer[i]->m_AdditionalConstants.cMaskColorA[elm], 1.0f );
						m_AdditionalShaderDriverContainer[i]->m_AdditionalConstants.cMaskColorB[elm] = math::min<f32>( m_AdditionalShaderDriverContainer[i]->m_AdditionalConstants.cMaskColorB[elm], 1.0f );
					}
				}
			}
			else if( InputDevice::Pad::IsPress( padNo, InputDevice::Button::Down ) )
			{
				for( u32 i = 0; i < m_AdditionalShaderDriverContainer.size(); ++i )
				{
					if ( m_EnableMaskA )
						m_AdditionalShaderDriverContainer[i]->m_AdditionalConstants.cMaskColorA -= color;

					if ( m_EnableMaskB )
						m_AdditionalShaderDriverContainer[i]->m_AdditionalConstants.cMaskColorB -= color;

					for( u32 elm = 0; elm < 4; ++elm )
					{
						m_AdditionalShaderDriverContainer[i]->m_AdditionalConstants.cMaskColorA[elm] = math::max<f32>( m_AdditionalShaderDriverContainer[i]->m_AdditionalConstants.cMaskColorA[elm], 0.0f );
						m_AdditionalShaderDriverContainer[i]->m_AdditionalConstants.cMaskColorB[elm] = math::max<f32>( m_AdditionalShaderDriverContainer[i]->m_AdditionalConstants.cMaskColorB[elm], 0.0f );
					}
				}
			}

			{
				const InputDevice::StickPos&				pos = InputDevice::Pad::GetStick( padNo, InputDevice::Stick::Left );
				if ( pos.x > 0.5f )
					m_FrameCnt += 5;
				else if ( pos.x < -0.5f )
					m_FrameCnt -= 5;
			}

			{
				scenegraph::resource::MaterialResourceNode::TextureInfo& rTextureInfo = m_pTopsMaterialResourceNode->GetTextureInfo( 1 );

				if( InputDevice::Pad::IsPress( padNo, InputDevice::Button::Right ) )				rTextureInfo.m_Attribute.m_UvSetNo = 1;
				if( InputDevice::Pad::IsPress( padNo, InputDevice::Button::Left ) )					rTextureInfo.m_Attribute.m_UvSetNo = 2;
			}
		}

		{
			// ビュー変換行列
			math::Vector eyePosition( 0.0f, 100.0f, 150.0f, 1.0f );  // 視点(カメラの位置)
			math::Vector focusPosition( 0.0f, 100.0f,  0.0f, 1.0f );  // 注視点
			math::Vector upDirection( 0.0f, 1.0f,  0.0f, 1.0f );  // カメラの上方向

			m_View = math::Matrix::GetLookAt(eyePosition, focusPosition, upDirection);

			m_pDrawManager->SetViewMatrix( m_View );
		}

		{
			math::Matrix		matY, matX;
			f32							rotate;

			rotate = (f32)(PI * (m_FrameCnt % 3000)) / 1500.0f;
			matY = math::Matrix::GetRotationY(rotate);
			rotate = (f32)(PI * (m_FrameCnt % 1500)) / 750.0f;
			matX = math::Matrix::GetRotationX(rotate);

			//m_World = matY * matX;
			m_World = matY;
		}

#if 0
		{// モデルにワールド座標を直接指定する場合。
			m_World[3].Set( -50.0f, 0.0f, 0.0f, 1.0f );
			m_pModelInstanceNode[0]->SetWorldMatrix( m_World );

			m_World[3].Set( 50.0f, 0.0f, 0.0f, 1.0f );
			m_pModelInstanceNode[1]->SetWorldMatrix( m_World );
		}
#else
		{//トラバース処理にてワールド座標を更新する場合。

      math::Quaternion rot0;
      rot0.MatrixToQuaternion(m_World);
      m_pModelInstanceNode[0]->SetLocalTranslate(math::Vector(0.0f, 0.0f, 0.0f));
      m_pModelInstanceNode[0]->SetLocalRotation(rot0);

      math::Quaternion rot1;
      rot1.MatrixToQuaternion(m_World);
      m_pModelInstanceNode[1]->SetLocalTranslate(math::Vector(50.0f, 0.0f, 0.0f));
      m_pModelInstanceNode[1]->SetLocalRotation(rot1);

      scenegraph::SceneGraphManager::TraverseModel();
		}
#endif

		//++m_FrameCnt;

		return true;
	}

	virtual bool OnDraw( void )
	{
		GFGL::SetActiveDisplay( m_Handle );

		Surface*			pBackBuffer = GFGL::GetBackBuffer( m_Handle );
		Surface*			pDepthBuffer = GFGL::GetDepthStencilBuffer( m_Handle );

		GFGL::SetRenderTarget( pBackBuffer );
		GFGL::SetDepthStencil( pDepthBuffer );

		GFGL::BeginScene();

		GFGL::ClearRenderTarget( pBackBuffer, Color::GetWhiteSmoke() );
		GFGL::ClearDepthStencil( pDepthBuffer, 1.0f, 255 );

		GFGL::SetViewport(0, 0, 1280, 720);

		m_pDrawManager->Draw();

		GFGL::EndScene();

		GFGL::SwapDisplay();

		return true;
	}

	virtual bool OnTerm( void )
	{
		GFX_SAFE_DELETE( m_pTextureMask );
		GFX_SAFE_DELETE( m_pDrawManager );

		delete m_pRootNode;
		delete m_pModelInstanceNode[0];
		delete m_pModelInstanceNode[1];

		scenegraph::resource::ResourceManager::Terminate();

		GFGL::Finish();

		GFGL::SetActiveDisplay(NULL);
		GFGL::DestroyDisplay(m_Handle);
		GFGL::Terminate();
		return true;
	}

	//いずれＩＯクラスができたら置き換える。
	static c8* ReadFile( const char* filename, s32* p_size, s32 align = 16 )
	{
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
	}


private:
	DisplayDeviceHandle					m_Handle;
	GLAllocator									m_Allocator;
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