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
//#include <RenderingEngine/Sample/Framework/AnimationControler.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMotData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/renderer/gfl2_DrawManager.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_AABBTree.h>

#include "MyRenderingPipeLine.h"

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
	static const u32																					MODEL_MAX = 2;
	math::Matrix44																							m_Projection;   // 透視変換行列
	math::Matrix34																							m_View;  // ビュー変換行列
	math::Matrix34																							m_World;      // ワールド変換行列

	scenegraph::resource::ResourceNode												*m_pModelResourceRootNode;
	scenegraph::resource::ResourceNode												*m_pMotionResourceRootNode;

	scenegraph::instance::ModelInstanceNode										*m_pModelInstanceNode[MODEL_MAX];
	MyRenderingPipeLine																				*m_RenderingPipeLine;
	scenegraph::instance::DrawEnvNode													*m_pDrawEnvNode;
	scenegraph::instance::LightSetNode												*m_pLightSetNode;
	scenegraph::instance::LightNode														*m_pDirectionalLightNode;
	scenegraph::instance::LightNode														*m_pAmbientLightNode;

	//AnimationControler																				m_AnimationControler[MODEL_MAX];

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

		m_Handle = GFGL::CreateDisplay( DisplayMode::HD720P, Format::A8R8G8B8, Format::D24S8, MultiSample::None );

		{//レンダリングエンジンの初期化
			//全ノードを管理するマネージャ初期化。機能を使わないのであれば必須でない。
			scenegraph::SceneGraphManager::InitDescription			desc(1024 * 2);
			desc.m_AABBTreeSize.SetMinMax( math::Vector(-500.0f, -500.0f, -500.0f), math::Vector(500.0f, 500.0f, 500.0f) );
			scenegraph::SceneGraphManager::Initialize();

			//リソースノードを生成するマネージャ初期化
			scenegraph::resource::ResourceManager::Initialize();
		
			//描画パイプラインを作成
			m_RenderingPipeLine = new MyRenderingPipeLine();
		}

		{
			scenegraph::resource::GfMdlData							resourceData;
			//resourceData.ReadFile( "C:/home/niji/git_gflib2/skelton/resource/tr0001/tr0001_00.gfmdl" );
			//resourceData.ReadFile( "C:/home/niji/git_gflib2/skelton/resource/pm0025_00/pm0025_00.gfmdl" );
			//resourceData.ReadFile( "C:/home/niji/git_gflib2/RenderingEngine/Sample/data/pm0772/pm0772_51.gfmdl" );
			//resourceData.ReadFile( "C:/home/niji/git_gflib2/RenderingEngine/Sample/data/pm0006_00_lizardon/pm0006_00.gfmdl" );
			//resourceData.ReadFile( "C:/home/niji/git_gflib2/RenderingEngine/Sample/data/pm0579_00_lanculus/pm0579_00.gfmdl" );
			//resourceData.ReadFile( "C:/home/xy_pokemon/rg/pm0025_00_pikachu/exports/pm0025_00.gfmdl" );
			resourceData.ReadFile( "C:/home/niji/chara/temp/Cutin_Test/exports/bt9999_00.gfmdl" );
			m_pModelResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
		}

		{
			scenegraph::resource::GfMotData							motionResourceData;
			//motionResourceData.ReadFile( "C:/home/niji/git_gflib2/skelton/resource/tr0001/tr0001_00_fi01_wait01.gfmot" );
			//motionResourceData.ReadFile( "C:/home/niji/git_gflib2/skelton/resource/pm0025_00/pm0025_00_ba10_waitA01.gfmot" );
			//motionResourceData.ReadFile( "C:/home/niji/git_gflib2/RenderingEngine/Sample/data/pm0772/pm0772_51_ba20_buturi01.gfmot" );
			//motionResourceData.ReadFile( "C:/home/niji/git_gflib2/RenderingEngine/Sample/data/pm0006_00_lizardon/pm0006_00_ba20_buturi01.gfmot" );
			motionResourceData.ReadFile( "C:/home/xy_pokemon/rg/pm0025_00_pikachu/exports/pm0025_00.gfmot" );
			m_pMotionResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &motionResourceData );
		}

		for( u32 i = 0; i < MODEL_MAX; ++i )
		{//同じリソースノードからインスタンスノードを２つ生成
			m_pModelInstanceNode[i] = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pModelResourceRootNode );
			//m_AnimationControler[i].Set( m_pModelInstanceNode[i], m_pMotionResourceRootNode );
			m_pModelInstanceNode[i]->SetState( m_pModelInstanceNode[i]->GetState() | scenegraph::instance::CullingObjectNode::State::OcclusionCulling );
			m_pModelInstanceNode[i]->SetState( m_pModelInstanceNode[i]->GetState() & ~scenegraph::instance::CullingObjectNode::State::OcclusionCulling );
		}

		{
			m_pDrawEnvNode = scenegraph::instance::InstanceCreator::CreateDrawEnvNode();
			m_pLightSetNode = scenegraph::instance::InstanceCreator::CreateLightSetNode();
			m_pDirectionalLightNode = scenegraph::instance::InstanceCreator::CreateLightNode();
			m_pAmbientLightNode = scenegraph::instance::InstanceCreator::CreateLightNode();

			{
				m_pDirectionalLightNode->SetNodeName( "DirectionalLight01" );
				util::FnvHash				nameHash( reinterpret_cast<const u8*>(m_pDirectionalLightNode->GetNodeName()) );
				m_pDirectionalLightNode->SetNodeNameHash( nameHash.value );
				m_pDirectionalLightNode->SetLightType( LightType::Directional );
				math::Vector3 position = math::Vector3(0.0f, 10000000.0f, 10000000.0f);
				m_pDirectionalLightNode->SetPosition( position );
				math::Vector3 direction = -position.Normalize();
				//direction.w = 0.0f;
				m_pDirectionalLightNode->SetDirection( direction );
				m_pDirectionalLightNode->SetColor( math::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
			}

			{
				m_pAmbientLightNode->SetNodeName( "AmbientLight01" );
				util::FnvHash				nameHash( reinterpret_cast<const u8*>(m_pAmbientLightNode->GetNodeName()) );
				m_pAmbientLightNode->SetNodeNameHash( nameHash.value );
				m_pAmbientLightNode->SetLightType( LightType::Ambient );
				m_pAmbientLightNode->SetColor( math::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
			}

			m_pLightSetNode->SetSetNo( 0 );
			m_pLightSetNode->AddLight( m_pDirectionalLightNode );//LightSetNodeの子となる
			m_pLightSetNode->AddLight( m_pAmbientLightNode );//LightSetNodeの子となる
			m_pDrawEnvNode->AddLightSet( m_pLightSetNode );//DrawEnvNodeの子となる

			math::AABB				envSpace( math::Vector(-500000.0f, -500000.0f, -500000.0f), math::Vector(500000.0f, 500000.0f, 500000.0f) );
			m_pDrawEnvNode->SetAABB( envSpace );
			scenegraph::SceneGraphManager::AddChild( m_pDrawEnvNode );

			m_RenderingPipeLine->AddDrawEnv( m_pDrawEnvNode );
		}

		{
			// 射影変換行列(パースペクティブ(透視法)射影)
			m_Projection = math::Matrix44::GetPerspective(
					math::ConvDegToRad(37.0f),		// 視野角30°
					(float)GFGLUtil::GetDisplayWidthByDisplayMode( DisplayMode::HD720P ) / (float)GFGLUtil::GetDisplayHeightByDisplayMode( DisplayMode::HD720P ),	// アスペクト比
					0.1f,							// 前方投影面までの距離
					10000.0f);							// 後方投影面までの距離

			m_RenderingPipeLine->SetProjectionMatrix( m_Projection );
		}

		m_RenderingPipeLine->AddEdgeRenderingTarget( m_pModelInstanceNode[0] );
		m_RenderingPipeLine->AddEdgeRenderingTarget( m_pModelInstanceNode[1] );

		return true;
	}

	virtual bool OnUpdate( void )
	{
		{
			math::Matrix34		matY, matX;
			f32							rotate;

			rotate = (f32)(PI * (m_FrameCnt % 300)) / 150.0f;
			matY = math::Matrix34::GetRotationY(rotate);
			rotate = (f32)(PI * (m_FrameCnt % 150)) / 75.0f;
			matX = math::Matrix34::GetRotationX(rotate);

			//m_World = matY * matX;
			m_World = matY;
		}

		{
			// ビュー変換行列
			math::Vector3 eyePosition( 0.0f, 600.0f * 0.5f, 700.0f * 0.5f );  // 視点(カメラの位置)
			math::Vector3 focusPosition( 0.0f, 40.0f,  0.0f );  // 注視点
			math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向

			m_View = math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);

			m_View = m_View * m_World;

			m_RenderingPipeLine->SetViewMatrix( m_View );
		}

		for( u32 i = 0; i < MODEL_MAX; ++i )
		{
			//u32			animFrame = (m_FrameCnt/2) % m_AnimationControler[i].GetFrameSize();
			//m_AnimationControler[i].ApplyAnimation( animFrame );
		}

		{
      m_pModelInstanceNode[0]->SetLocalTranslate(math::Vector3(-499.0f, 0.0f, 0.0f));
      m_pModelInstanceNode[1]->SetLocalTranslate(math::Vector3(50.0f, 0.0f, 0.0f));

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
			itoa( static_cast<s32>(m_PerformanceTime / 60), buff, 10 );
			::OutputDebugStringA( buff );
			::OutputDebugStringA( "\n" );
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

		delete m_pMotionResourceRootNode;
		delete m_pModelResourceRootNode;
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