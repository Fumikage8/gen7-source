#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
//#include <math/include/gfl2_MathUtil.h>
#include <math/include/gfl2_MathCommon.h>
#include <math/include/gfl2_MathAABB.h>
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_GFGLUtil.h>


#include <system/include/Timer/gfl2_PerformanceCounter.h>
#include <util/include/gfl2_FixedSizeContainer.h>

#include <RenderingEngine/Sample/Framework/IApplication.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfEnvData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/renderer/gfl2_DrawManager.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_AABBTree.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::system;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;

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
	math::Matrix																							m_Projection;   // 透視変換行列
	math::Matrix																							m_View;  // ビュー変換行列
	math::Matrix																							m_World;      // ワールド変換行列

	scenegraph::resource::ResourceNode												*m_pModelResourceRootNode;
	scenegraph::resource::ResourceNode												*m_pEnvResourceRootNode;

	scenegraph::instance::ModelInstanceNode										*m_pModelInstanceNode;
	DefaultRenderingPipeLine																	*m_RenderingPipeLine;
	scenegraph::instance::DrawEnvNode													*m_pDrawEnvNode;
	scenegraph::instance::LightSetNode												*m_pLightSetNode;
	scenegraph::instance::LightNode														*m_pDirectionalLightNode;
	scenegraph::instance::LightNode														*m_pAmbientLightNode;

	s32																												m_FrameCnt;
	u64																												m_PerformanceTime;

	SimpleDraw() : 
		Sample::Framework::IApplication(),
		m_FrameCnt(0),
		m_PerformanceTime( 0 ),
		m_pDrawEnvNode( NULL )
	{
	
	}

	virtual ~SimpleDraw(){}

	virtual bool OnInit( void )
	{
		GFGL::Initialize( &m_Allocator );

		m_Handle = GFGL::CreateDisplay( DisplayMode::HD720P, Format::A8R8G8B8, Format::D24S8, MultiSample::MSAA4X );

		{//レンダリングエンジンの初期化
			//全ノードを管理するマネージャ初期化。機能を使わないのであれば必須でない。
			scenegraph::SceneGraphManager::InitDescription			desc(1024 * 2);
			desc.m_AABBTreeSize.SetMinMax( math::Vector(-500.0f, -500.0f, -500.0f), math::Vector(500.0f, 500.0f, 500.0f) );
			scenegraph::SceneGraphManager::Initialize();

			//リソースノードを生成するマネージャ初期化
			scenegraph::resource::ResourceManager::Initialize();
		
			//描画パイプラインを作成
			m_RenderingPipeLine = new DefaultRenderingPipeLine();
		}

    {
			scenegraph::resource::GfEnvData							resourceData;
			resourceData.ReadFile( "C:/home/niji/git_gflib2/RenderingEngine/Sample/data/EnvTestMap/DrawEnvTest.gfenv" );

			m_pEnvResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
			m_pDrawEnvNode = scenegraph::instance::InstanceCreator::CreateDrawEnvNode( m_pEnvResourceRootNode );
		}

		{
			scenegraph::resource::GfMdlData							resourceData;
			resourceData.ReadFile( "C:/home/niji/git_gflib2/RenderingEngine/Sample/data/EnvTestMap/DrawEnvTest.gfmdl" );

			m_pModelResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
			m_pModelInstanceNode = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pModelResourceRootNode );
		}

		

		m_RenderingPipeLine->AddDrawEnv( m_pDrawEnvNode );
		m_RenderingPipeLine->SetProjectionMatrix( m_pDrawEnvNode->GetCamera( 0 )->GetProjectionMatrix() );

		return true;
	}

	virtual bool OnUpdate( void )
	{
		m_RenderingPipeLine->SetViewMatrix( m_pDrawEnvNode->GetCamera( 0 )->GetViewMatrix() );

		{
			{
				u64				start = Timer::PerformanceCounter::GetMicroSecond();
        scenegraph::SceneGraphManager::TraverseModel();
				u64				end = Timer::PerformanceCounter::GetMicroSecond();

				m_PerformanceTime += end - start;
			}
		}

		if ( m_FrameCnt % 60 == 0 )
		{
			char		buff[32];
			memset(buff, 0, sizeof(buff));
			sprintf_s( buff, sizeof(buff), "%d\n", static_cast<s32>(m_PerformanceTime / 60) );
			::OutputDebugStringA( buff );
			m_PerformanceTime = 0;
		}

		++m_FrameCnt;

		return true;
	}

	virtual bool OnDraw( void )
	{
		GFGL::SetActiveDisplay( m_Handle );

		Surface*			pBackBuffer = GFGL::GetBackBuffer( m_Handle );
		Surface*			pDepthBuffer = GFGL::GetDepthStencilBuffer( m_Handle );

		m_RenderingPipeLine->SetRenderTarget( pBackBuffer );
		m_RenderingPipeLine->SetDepthStencil( pDepthBuffer );

		m_RenderingPipeLine->Execute();

		{//! @note フラグの一斉クリアは後日用意
			class TransformNodeVisitor
			{
			public:
				TransformNodeVisitor(){}
				virtual ~TransformNodeVisitor() {}

				void Visit( scenegraph::instance::TransformNode* pNode )
				{
					pNode->SetTransformDirty( false );
				}
			};

			scenegraph::Traverser::Visitor<scenegraph::instance::TransformNode, TransformNodeVisitor>				visitor;
			scenegraph::SceneGraphManager::Traverse<scenegraph::instance::TransformNode, TransformNodeVisitor>( &visitor );
		}

		return true;
	}

	virtual bool OnTerm( void )
	{
		if ( m_pDrawEnvNode )
			delete m_pDrawEnvNode;
		//delete m_pLightSetNode;//DrawEnvNodeの子となっているので開放済み
		//delete m_pDirectionalLightNode;//LightSetNodeの子となっているので開放済み
		//delete m_pAmbientLightNode;//LightSetNodeの子となっているので開放済み

		delete m_RenderingPipeLine;

		delete m_pEnvResourceRootNode;
		delete m_pModelResourceRootNode;
		delete m_pModelInstanceNode;

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