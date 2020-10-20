#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
//#include <math/include/gfl2_MathUtil.h>
#include <math/include/gfl2_MathCommon.h>
#include <math/include/gfl2_MathAABB.h>
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_GFGLUtil.h>
#include <gfx/include/gfl2_DrawUtil.h>

#include <renderingengine/Sample/Framework/IApplication.h>

#include <util/include/gfl2_FixedSizeContainer.h>

#include <renderingengine/include/scenegraph/gfl2_SceneGraphManager.h>
#include <renderingengine/include/scenegraph/resource/gfl2_GfMdlData.h>
#include <renderingengine/include/scenegraph/gfl2_DagNode.h>
#include <renderingengine/include/scenegraph/resource/gfl2_ResourceManager.h>
#include <renderingengine/include/scenegraph/instance/gfl2_InstanceCreator.h>
#include <renderingengine/include/renderer/gfl2_DrawManager.h>
#include <renderingengine/include/renderer/gfl2_RenderingPipeLine.h>

#include "MyRenderingPipeLine.h"

using namespace gfl2;
using namespace gfl2::gfx;
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

class SimpleDraw : public Sample::Framework::IApplication
{
public:
	static const u32																					MODEL_MAX = 2;
	math::Matrix																							m_Projection;   // 透視変換行列
	math::Matrix																							m_View;  // ビュー変換行列
	math::Matrix																							m_World;      // ワールド変換行列

	scenegraph::resource::ResourceNode												*m_pRootNode;
	scenegraph::instance::ModelInstanceNode										*m_pModelInstanceNode[MODEL_MAX];
	MyRenderingPipeLine																				*m_RenderingPipeLine;

	s32																					m_FrameCnt;

	SimpleDraw() : 
		Sample::Framework::IApplication(),
		m_FrameCnt(0)
	{
	
	}

	virtual ~SimpleDraw(){}

	virtual bool OnInit( void )
	{
		GFGL::Initialize( &m_Allocator );

		m_Handle = GFGL::CreateDisplay( DisplayMode::HD720P, Format::A8R8G8B8, Format::D24S8, MultiSample::MSAA4X );

		{//レンダリングエンジンの初期化
			//全ノードを管理するマネージャ初期化。機能を使わないのであれば必須でない。
			scenegraph::SceneGraphManager::Initialize();

			//リソースノードを生成するマネージャ初期化
			scenegraph::resource::ResourceManager::Initialize();
		
			//描画パイプラインを作成
			m_RenderingPipeLine = new MyRenderingPipeLine();
		}

		{
			//リソースローダー経由でノードは生成されます。
			scenegraph::resource::GfMdlData							resourceData;
			resourceData.ReadFile( "C:/home/niji/git_gflib2/renderingengine/Sample/data/pm0025_00/pm0025_00.gfmdl" );
			//m_pRootNodeには、様々なリソースがぶら下がっています。
			m_pRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
		}

		//同じリソースノードからインスタンスノードを２つ生成
		m_pModelInstanceNode[0] = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pRootNode );
		m_pModelInstanceNode[1] = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pRootNode );

		{
			// 射影変換行列(パースペクティブ(透視法)射影)
			m_Projection = math::Matrix::GetPerspective(
					math::Util::ConvDegToRad(37.0f),		// 視野角30°
					(float)GFGLUtil::GetDisplayWidthByDisplayMode( DisplayMode::HD720P ) / (float)GFGLUtil::GetDisplayHeightByDisplayMode( DisplayMode::HD720P ),	// アスペクト比
					1.1f,							// 前方投影面までの距離
					1000.0f);							// 後方投影面までの距離

			m_RenderingPipeLine->SetProjectionMatrix( m_Projection );
		}

    // 簡易描画初期化
		DrawUtil::Initialize(&m_Allocator, &m_Handle);

		return true;
	}

	virtual bool OnUpdate( void )
	{
		{
			// ビュー変換行列
			math::Vector eyePosition( 0.0f, 20.0f, 150.0f, 1.0f );  // 視点(カメラの位置)
			math::Vector focusPosition( 0.0f, 20.0f,  0.0f, 1.0f );  // 注視点
			math::Vector upDirection( 0.0f, 1.0f,  0.0f, 1.0f );  // カメラの上方向

			m_View = math::Matrix::GetLookAt(eyePosition, focusPosition, upDirection);

			m_RenderingPipeLine->SetViewMatrix( m_View );
		}

		{
			math::Matrix		matY, matX;
			f32							rotate;

			rotate = (f32)(PI * (m_FrameCnt % 300)) / 150.0f;
      rotate = m_FrameCnt * 0.001f;
			matY = math::Matrix::GetRotationY(rotate);
			rotate = (f32)(PI * (m_FrameCnt % 150)) / 75.0f;
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
      m_pModelInstanceNode[0]->SetLocalTranslate(math::Vector(-50.0f, 0.0f, 0.0f));
      m_pModelInstanceNode[0]->SetLocalRotation(rot0);

      math::Quaternion rot1;
      rot1.MatrixToQuaternion(m_World);
      m_pModelInstanceNode[1]->SetLocalTranslate(math::Vector(50.0f, 0.0f, 0.0f));
      m_pModelInstanceNode[1]->SetLocalRotation(rot1);

      scenegraph::SceneGraphManager::TraverseModel();
		}
#endif

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

		return true;
	}

	virtual bool OnTerm( void )
	{
		delete m_RenderingPipeLine;

		delete m_pRootNode;
		delete m_pModelInstanceNode[0];
		delete m_pModelInstanceNode[1];
    
    DrawUtil::Terminate();

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