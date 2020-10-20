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
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/renderer/gfl2_DrawManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNodeFactory.h>


#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfLocData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryLocData.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::scenegraph;

class GLAllocator : public IGLAllocator
{
public:
	GLAllocator():IGLAllocator(){}
	virtual ~GLAllocator(){}

	virtual void* _SystemMemoryMallocAlign(u32 size, u32 alignment,const char *filename , const int linenum)
	{
		return malloc(size);
	}

	/*virtual void* SystemMemoryMallocAlign(u32 size, u32 alignment){
		return malloc(size);
	}*/

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

class ResourceVisitor
{
public:
	class TraverseMode{
	public:
		enum Enum{
			DataCheck = 0,
			PickUpNode,
			Max
		};
	};

	ResourceVisitor::ResourceVisitor():
		m_MaterialResourceNum( 0 ),
		m_TextureResourceNum( 0 ),
		m_MeshResourceNum( 0 ),
		m_ShaderResourceNum( 0 ),
		m_pModelResourceNode( NULL ),
		m_pMaterialResourceNodeList( NULL ),
		m_pTextureResourceNodeList( NULL ),
		m_pShaderResourceNodeList( NULL ),
		m_pMeshResourceNodeList( NULL )
	{
		m_TraverseMode = TraverseMode::DataCheck;
	}

	~ResourceVisitor(){}

	void Visit( DagNode* node )
	{
		//::OutputDebugStringA( node->GetNodeName() );
		//::OutputDebugStringA( "\n" );

		if ( m_TraverseMode == TraverseMode::DataCheck )
		{
			if ( node->IsKindOf<resource::TextureResourceNode>() )
			{
				++m_TextureResourceNum;
			}
			else if ( node->IsKindOf<resource::MaterialResourceNode>() )
			{
				++m_MaterialResourceNum;
			}
			else if ( node->IsKindOf<resource::MeshResourceNode>() )
			{
				++m_MeshResourceNum;
			}
			else if ( node->IsKindOf<resource::ShaderResourceNode>() )
			{
				++m_ShaderResourceNum;
			}
		}
		else if ( m_TraverseMode == TraverseMode::PickUpNode )
		{
			if ( node->IsKindOf<resource::GfMdlResourceNode>() )
			{
				assert( m_pModelResourceNode == NULL );//２つGfMdlResourceNodeがぶら下がっているのは想定外。ずっと後にそういったパターンもサポートするかも？
				m_pModelResourceNode = node->SafeCast<resource::GfMdlResourceNode>();
			}
			else if ( node->IsKindOf<resource::TextureResourceNode>() )
			{
				m_pTextureResourceNodeList[m_TextureResourceNum] = node->SafeCast<resource::TextureResourceNode>();
				++m_TextureResourceNum;
			}
			else if ( node->IsKindOf<resource::MaterialResourceNode>() )
			{
				m_pMaterialResourceNodeList[m_MaterialResourceNum] = node->SafeCast<resource::MaterialResourceNode>();
				++m_MaterialResourceNum;
			}
			else if ( node->IsKindOf<resource::MeshResourceNode>() )
			{
				m_pMeshResourceNodeList[m_MeshResourceNum] = node->SafeCast<resource::MeshResourceNode>();
				++m_MeshResourceNum;
			}
			else if ( node->IsKindOf<resource::ShaderResourceNode>() )
			{
				m_pShaderResourceNodeList[m_ShaderResourceNum] = node->SafeCast<resource::ShaderResourceNode>();
				++m_ShaderResourceNum;
			}
		}
	}

	void CreatePushBuffer()
	{
		if ( m_MaterialResourceNum )
			m_pMaterialResourceNodeList = reinterpret_cast<resource::MaterialResourceNode**>( GLMemory::Malloc( sizeof(resource::MaterialResourceNode*) * m_MaterialResourceNum ) );
		if ( m_TextureResourceNum )
			m_pTextureResourceNodeList = reinterpret_cast<resource::TextureResourceNode**>( GLMemory::Malloc( sizeof(resource::TextureResourceNode*) * m_TextureResourceNum ) );
		if ( m_MeshResourceNum )
			m_pMeshResourceNodeList = reinterpret_cast<resource::MeshResourceNode**>( GLMemory::Malloc( sizeof(resource::MeshResourceNode*) * m_MeshResourceNum ) );
		if ( m_ShaderResourceNum )
			m_pShaderResourceNodeList = reinterpret_cast<resource::ShaderResourceNode**>( GLMemory::Malloc( sizeof(resource::ShaderResourceNode*) * m_ShaderResourceNum ) );

		//再度収集する際にカウンタとして使うのでリセット
		m_MaterialResourceNum = 0;
		m_TextureResourceNum = 0;
		m_MeshResourceNum = 0;
		m_ShaderResourceNum = 0;
	}

	void DeletePushBuffer()
	{
		if ( m_pMaterialResourceNodeList )
			GLMemory::Free( m_pMaterialResourceNodeList );
		if ( m_pTextureResourceNodeList )
			GLMemory::Free( m_pTextureResourceNodeList );
		if ( m_pMeshResourceNodeList )
			GLMemory::Free( m_pMeshResourceNodeList );
		if( m_pShaderResourceNodeList )
			GLMemory::Free( m_pShaderResourceNodeList );
	}

	u32															m_TraverseMode;
	u32															m_MaterialResourceNum;
	u32															m_TextureResourceNum;
	u32															m_MeshResourceNum;
	u32															m_ShaderResourceNum;

	resource::GfMdlResourceNode			*m_pModelResourceNode;
	resource::MaterialResourceNode	**m_pMaterialResourceNodeList;
	resource::TextureResourceNode		**m_pTextureResourceNodeList;
	resource::ShaderResourceNode		**m_pShaderResourceNodeList;
	resource::MeshResourceNode			**m_pMeshResourceNodeList;
};

class SimpleDraw : public Sample::Framework::IApplication
{
public:
	static const u32																					MODEL_MAX = 2;
	math::Matrix44																						m_Projection;   // 透視変換行列
	math::Matrix34																						m_View;  // ビュー変換行列
	math::Matrix																							m_World;      // ワールド変換行列

	renderer::DrawManager																			*m_pDrawManager;
	scenegraph::resource::ResourceNode												*m_pBinModelRootNode;
	scenegraph::resource::ResourceNode												*m_pResModelRootNode;
		scenegraph::resource::ResourceNode											*m_pModelResourceRootNode[MODEL_MAX];
	scenegraph::instance::ModelInstanceNode										*m_pModelInstanceNode[MODEL_MAX];
	scenegraph::instance::LightNode														*m_pDirectionalLightNode;

	scenegraph::resource::ResourceNode																			*m_pLocatorResourceRootNode;
	scenegraph::instance::LocatorInstanceNode																*m_pLocatorInstanceNodeTop;
	gfx::GLFixedSizeContainer<scenegraph::instance::LocatorInstanceNode*>		m_pLocatorInstanceNodeList;

	s32																					m_FrameCnt;

	SimpleDraw() : 
		Sample::Framework::IApplication(),
		m_FrameCnt(0)
	{
		m_pLocatorResourceRootNode = NULL;

		for(int i=0;i<MODEL_MAX;i++)
		{
			m_pModelResourceRootNode[i] = NULL;
		}
	}

	virtual ~SimpleDraw(){}

	virtual bool OnInit( void )
	{
		GFGL::Initialize( &m_Allocator );

		m_Handle = GFGL::CreateDisplay( DisplayMode::HD720P, Format::A8R8G8B8, Format::D24S8, MultiSample::None );

		{//レンダリングエンジンの初期化
			scenegraph::SceneGraphManager::InitDescription			desc(1024 * 10);
			//全ノードを管理するマネージャ初期化。機能を使わないのであれば必須でない。
			scenegraph::SceneGraphManager::Initialize( desc );

			//リソースノードを生成するマネージャ初期化
			scenegraph::resource::ResourceManager::Initialize();
		
			//描画マネージャの初期化。複数持ってレイヤー化してもよい。
			m_pDrawManager = renderer::DrawManager::Create();
		}

		{
			//リソースローダー経由でノードは生成されます。
			scenegraph::resource::GfBinaryMdlData							resourceData;

			s32		size;
			c8		*pBinData = ReadFile( "R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/pm0025_00.wingl.gfbmdl", &size );			
			
			resourceData.SetModelData( pBinData );

			m_pBinModelRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );	
		}

		{
			static const char* pShaderFileList[] = {
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Shaders/PokeNormal.wingl.gfbvsh",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Shaders/PDC_Body_Shadow.wingl.gfbfsh",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Shaders/PDC_pm_keikouo_Neon.wingl.gfbfsh",
			};

			u32				shaderNum = sizeof(pShaderFileList) / sizeof(pShaderFileList[0]);

			for( u32 shaderNo = 0; shaderNo < shaderNum; ++shaderNo )
			{
				scenegraph::resource::GfBinaryShdData							resourceData;

				s32		size;
				c8		*pBinData = ReadFile( pShaderFileList[shaderNo], &size );

				resourceData.SetShaderData( pBinData );

				scenegraph::resource::ResourceNode		*pBinShdNode = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
				m_pBinModelRootNode->AddChild( pBinShdNode );

				delete pBinData;
			}
		}

		{
			static const c8*			pTextureFileList[] = {
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_Eye1.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_EyeNor.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_EyeHlm.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_Mouth1.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_MouthNor.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_MouthHlm.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_BodyALta.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_BodyA1.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/DummyTex.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_BodyANor.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/Default_Lta_10D.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_Hta.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/LinerTable.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_BodyAHlm.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_BodyB1.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_BodyBInc.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_BodyBNor.wingl.btex",
				"R:/home/niji/git_program/gflib2/RenderingEngine/Sample/data/pm0025_00/Textures/pm0025_00_BodyBHlm.wingl.btex"
			};

			u32				texNum = sizeof(pTextureFileList) / sizeof(pTextureFileList[0]);

			const c8*																					pFactoryName;
			scenegraph::resource::TextureResourceNodeFactory	*pTextureResourceNodeFactory;

			pFactoryName = scenegraph::resource::TextureResourceNodeFactory::GetFactoryTypeName();
			pTextureResourceNodeFactory = reinterpret_cast<scenegraph::resource::TextureResourceNodeFactory*>( scenegraph::resource::ResourceManager::GetResourceNodeFactory( resource::IResourceData::Type( pFactoryName ) ) );
			pTextureResourceNodeFactory->SetTemporaryAllocator( &m_Allocator );

			for( u32 texNo = 0; texNo < texNum; ++texNo )
			{
				scenegraph::resource::GfBinaryTexData							texResourceData;
				scenegraph::resource::ResourceNode								*pBinTexRootNode;

				s32		texsize;
				s32		size;
				c8		*pTexBinData = ReadFile( pTextureFileList[texNo], &size );
			
				texResourceData.SetTextureData( pTexBinData );

				//m_pBinModelRootNodeには、様々なリソースがぶら下がっています。
				pBinTexRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &texResourceData );
				m_pBinModelRootNode->AddChild( pBinTexRootNode );

				delete pTexBinData;
			}
		}

		{
			m_pModelInstanceNode[0] = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pBinModelRootNode );
			m_pModelInstanceNode[1] = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pBinModelRootNode );

			m_pModelInstanceNode[0]->SetDrawCheckListener( true );
			m_pModelInstanceNode[1]->SetDrawCheckListener( true );
		}

		//描画マネージャ経由で描画する場合は、追加します。インスタンスよりメッシュ情報を手動で引っ張ってきて自前で描画することもできます。
		m_pDrawManager->AddObject( m_pModelInstanceNode[0] );
		m_pDrawManager->AddObject( m_pModelInstanceNode[1] );
		
		{
			// 射影変換行列(パースペクティブ(透視法)射影)
			m_Projection = math::Matrix44::GetPerspective(
					math::ConvDegToRad(37.0f),		// 視野角30°
					(float)GFGLUtil::GetDisplayWidthByDisplayMode( DisplayMode::HD720P ) / (float)GFGLUtil::GetDisplayHeightByDisplayMode( DisplayMode::HD720P ),	// アスペクト比
					1.1f,							// 前方投影面までの距離
					10000.0f);							// 後方投影面までの距離

			m_pDrawManager->SetProjectionMatrix( m_Projection );
		}

		GFGL::Finish();

		return true;
	}

	virtual bool OnUpdate( void )
	{
		{
			// ビュー変換行列
			math::Vector3 eyePosition( 0.0f, 40.0f, 200.0f );  // 視点(カメラの位置)
			math::Vector3 focusPosition( 0.0f, 0.0f,  0.0f );  // 注視点
			math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向

			m_View = math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);

			m_pDrawManager->SetViewMatrix( m_View );
		}

		{//トラバース処理にてワールド座標を更新する場合。
			m_pModelInstanceNode[0]->SetLocalTranslate(math::Vector3(50.0f, 0.0f, 0.0f));
      m_pModelInstanceNode[1]->SetLocalTranslate(math::Vector3(-50.0f, 0.0f, 0.0f));

			static float rot = 0.0f;

			rot+=0.01f;

			math::Quaternion rotQ( math::Vector3(0.0f,1.0f,0.0f),rot);
			

			m_pModelInstanceNode[0]->SetLocalRotation(rotQ);

      scenegraph::SceneGraphManager::TraverseModelFast();
		}
		++m_FrameCnt;

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

		GFGL::ClearRenderTarget( pBackBuffer, Color::GetDarkSeaGreen() );
		GFGL::ClearDepthStencil( pDepthBuffer, 1.0f, 255 );

		GFGL::SetViewport(0, 0, 1280, 720);

		m_pDrawManager->Draw();

		u32				ref;

		ref = m_pModelInstanceNode[0]->GetReferenceCnt();

		GFGL::EndScene();

		GFGL::SwapDisplay();

		ref = m_pModelInstanceNode[0]->GetReferenceCnt();

		return true;
	}

	virtual bool OnTerm( void )
	{
		u32				ref;

		GFX_SAFE_DELETE( m_pDrawManager );

		for( u32 i = 0; i < MODEL_MAX; ++i )
		{
			if ( m_pModelResourceRootNode[i] )
			{
				u32				ref = m_pModelResourceRootNode[i]->GetReferenceCnt();
				delete m_pModelResourceRootNode[i];
			}
		}

		delete m_pBinModelRootNode;
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