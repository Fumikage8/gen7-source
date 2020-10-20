#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
//#include <math/include/gfl2_MathUtil.h>
#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_GFGLUtil.h>

#include <RenderingEngine/Sample/Framework/IApplication.h>
#include <RenderingEngine/Sample/Framework/AnimationControler.h>

#include <util/include/gfl2_FixedSizeContainer.h>

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfLocData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/renderer/gfl2_DrawManager.h>
#include <system/include/InputDevice/gfl2_Pad.h>

#include "MyRenderingPipeLine.h"
#include "DebugWindow.h"

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

class SimpleDraw : public Sample::Framework::IApplication
{
public:
  enum MODEL_ID
  {
    MODEL_ID_BODY,
    MODEL_ID_MUSTACHE_A,
    MODEL_ID_MUSTACHE_B,
    MODEL_ID_MUSTACHE_C,
    MODEL_ID_SUNGLASS_A,
    MODEL_ID_SUNGLASS_B,
    MODEL_ID_SUNGLASS_C,
    MODEL_ID_HAT,
    MODEL_ID_HORN,

    MODEL_MAX
  };
	static const u32																					ATTACH_POINT_MAX = 6;
	math::Matrix																							m_Projection;
	math::Matrix																							m_View;
	math::Matrix																							m_World;

	//renderer::DrawManager																									*m_pDrawManager;
	scenegraph::resource::ResourceNode																			*m_pModelResourceRootNode[MODEL_MAX];
	scenegraph::resource::ResourceNode																			*m_pMotionResourceRootNode[MODEL_MAX];
	scenegraph::resource::ResourceNode																			*m_pLocatorResourceRootNode;
	scenegraph::instance::ModelInstanceNode																	*m_pModelInstanceNode[MODEL_MAX];
	scenegraph::instance::LocatorInstanceNode																*m_pLocatorInstanceNodeTop;
	gfx::GLFixedSizeContainer<scenegraph::instance::LocatorInstanceNode*>		m_pLocatorInstanceNodeList;
	AnimationControler																											m_AnimationControler[MODEL_MAX];

	s32																					m_FrameCnt;
	s32																					m_AttachPoint;

	MyRenderingPipeLine*                                      m_RenderingPipeLine;
  DebugWindowProc*                                          m_pDebugWindowProc;

	SimpleDraw() : 
		Sample::Framework::IApplication(),
		m_FrameCnt(0),
		m_AttachPoint(0)
	{
		for( u32 i = 0; i < MODEL_MAX; ++i )
		{
			m_pModelResourceRootNode[i] = NULL;
			m_pMotionResourceRootNode[i] = NULL;
		}
		m_pLocatorResourceRootNode = NULL;
	}

	virtual ~SimpleDraw(){}

	virtual bool OnInit( void )
	{
		GFGL::Initialize( &m_Allocator );
		InputDevice::Pad::Initialize();

		m_Handle = GFGL::CreateDisplay( DisplayMode::HD720P, Format::A8R8G8B8, Format::D24S8, MultiSample::MSAA4X );

		{
			scenegraph::SceneGraphManager::Initialize();
			scenegraph::resource::ResourceManager::Initialize();
			//m_pDrawManager = renderer::DrawManager::Create();
			//描画パイプラインを作成
			m_RenderingPipeLine = new MyRenderingPipeLine();
		}

		for( u32 modelNo = 0; modelNo < MODEL_MAX; ++modelNo )
		{
			static const std::string				s_ModelFilePath[] = {
				"C:/home/niji/git_gflib2/RenderingEngine/Sample/data/Acc_test_0723/pm0051_00.gfmdl",
				"C:/home/niji/git_gflib2/RenderingEngine/Sample/data/Acc_test_0723/mustache.gfmdl",
				"C:/home/niji/git_gflib2/RenderingEngine/Sample/data/Acc_test_0723/mustache.gfmdl",
				"C:/home/niji/git_gflib2/RenderingEngine/Sample/data/Acc_test_0723/mustache.gfmdl",
				"C:/home/niji/git_gflib2/RenderingEngine/Sample/data/Acc_test_0723/sunglasses.gfmdl",
				"C:/home/niji/git_gflib2/RenderingEngine/Sample/data/Acc_test_0723/sunglasses.gfmdl",
				"C:/home/niji/git_gflib2/RenderingEngine/Sample/data/Acc_test_0723/sunglasses.gfmdl",
				"C:/home/niji/git_gflib2/RenderingEngine/Sample/data/Acc_test_0723/hat.gfmdl",
				"C:/home/niji/git_gflib2/RenderingEngine/Sample/data/Acc_test_0723/horn.gfmdl",
			};

			static const std::string				s_pMotionFilePath[] = {
				"C:/home/niji/git_gflib2/RenderingEngine/Sample/data/Acc_test_0723/pm0051_00_ba10_waitA01.gfmot",
				"",
				"",
				"",
				"",
				"",
				"",
				"",
				"",
			};

			scenegraph::resource::GfMdlData							modelResourceData;
			modelResourceData.ReadFile( s_ModelFilePath[modelNo] );
			m_pModelResourceRootNode[modelNo] = scenegraph::resource::ResourceManager::CreateResourceNode( &modelResourceData );

			if ( s_pMotionFilePath[modelNo].length() )
			{
				scenegraph::resource::GfMotData							motionResourceData;
				motionResourceData.ReadFile( s_pMotionFilePath[modelNo] );
				m_pMotionResourceRootNode[modelNo] = scenegraph::resource::ResourceManager::CreateResourceNode( &motionResourceData );
			}
		}

		{
			s32					size;			
			c8*					pGflocData;

			pGflocData = ReadFile( "C:/home/niji/git_gflib2/RenderingEngine/Sample/data/Acc_test_0723/pm0051_00.gfloc", &size );
			scenegraph::resource::GfLocData							locatorResourceData( pGflocData );

			m_pLocatorResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &locatorResourceData );

			delete pGflocData;
		}

		for( u32 modelNo = 0; modelNo < MODEL_MAX; ++modelNo )
		{
			//同じリソースノードからインスタンスノードをMODEL_MAX個生成
			m_pModelInstanceNode[modelNo] = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pModelResourceRootNode[modelNo] );

			if ( m_pMotionResourceRootNode[modelNo] )
			{
				m_AnimationControler[modelNo].Set( m_pModelInstanceNode[modelNo], m_pMotionResourceRootNode[modelNo] );
			}

			//描画マネージャ経由で描画する場合は、追加します。インスタンスよりメッシュ情報を手動で引っ張ってきて自前で描画することもできます。
			//m_pDrawManager->AddObject( m_pModelInstanceNode[modelNo] );
		}

    m_pModelInstanceNode[MODEL_ID_BODY]->SetNodeName("body");
    m_pModelInstanceNode[MODEL_ID_HAT]->SetNodeName("parts_hat");
    m_pModelInstanceNode[MODEL_ID_HORN]->SetNodeName("parts_horn");
    m_pModelInstanceNode[MODEL_ID_MUSTACHE_A]->SetNodeName("parts_mustache_a");
    m_pModelInstanceNode[MODEL_ID_MUSTACHE_B]->SetNodeName("parts_mustache_b");
    m_pModelInstanceNode[MODEL_ID_MUSTACHE_C]->SetNodeName("parts_mustache_c");
    m_pModelInstanceNode[MODEL_ID_SUNGLASS_A]->SetNodeName("parts_sunglass_a");
    m_pModelInstanceNode[MODEL_ID_SUNGLASS_B]->SetNodeName("parts_sunglass_b");
    m_pModelInstanceNode[MODEL_ID_SUNGLASS_C]->SetNodeName("parts_sunglass_c");

		{//コントロールしやすいように収集。このようにしないといけないわけではない。
			m_pLocatorInstanceNodeTop = scenegraph::instance::InstanceCreator::CreateLocatorInstanceNode( m_pLocatorResourceRootNode );
			m_pLocatorInstanceNodeList.CreateBuffer( m_pLocatorInstanceNodeTop->GetChildCount() + 1 );

			m_pLocatorInstanceNodeList.push_back( m_pLocatorInstanceNodeTop );
			for( u32 i = 0; i < m_pLocatorInstanceNodeTop->GetChildCount(); ++i )
			{
				scenegraph::DagNode													*pNode = m_pLocatorInstanceNodeTop->GetChild( i );
				scenegraph::instance::LocatorInstanceNode		*pLocatorNode = pNode->SafeCast<scenegraph::instance::LocatorInstanceNode>();
				m_pLocatorInstanceNodeList.push_back( pLocatorNode );
			}
		}
			
		{//LocatorInstanceNodeをモデルの持つJointInstanceNodeにぶら下げる。
			scenegraph::instance::JointInstanceNode			*pJointInstanceNodeTop = m_pModelInstanceNode[0]->GetJointInstanceNode( 0 );
			scenegraph::instance::LocatorInstanceNode::Util::AttachToJointInstanceNode( pJointInstanceNodeTop, m_pLocatorInstanceNodeTop );
		}

		//各パーツモデルインスタンスをロケーターノードの子にする
		//m_pLocatorInstanceNodeTop->GetChild関数ではアクセスできなくなったので、先に収集しておいたm_pLocatorInstanceNodeListを用いる。
		for( u32 i = 0; i < m_pLocatorInstanceNodeList.size(); ++i )
		{
			scenegraph::instance::LocatorInstanceNode			*pNode = m_pLocatorInstanceNodeList[i];

			if ( strcmp( pNode->GetNodeName(), "AccNoseA1" ) == 0 )
			{
				pNode->AddChild( m_pModelInstanceNode[MODEL_ID_MUSTACHE_A] );
			}
			else if ( strcmp( pNode->GetNodeName(), "AccNoseB1" ) == 0 )
			{
				pNode->AddChild( m_pModelInstanceNode[MODEL_ID_MUSTACHE_B] );
			}
			else if ( strcmp( pNode->GetNodeName(), "AccNoseC1" ) == 0 )
			{
				pNode->AddChild( m_pModelInstanceNode[MODEL_ID_MUSTACHE_C] );
			}
			else if ( strcmp( pNode->GetNodeName(), "AccHeadA1" ) == 0 )
			{
				pNode->AddChild( m_pModelInstanceNode[MODEL_ID_HORN] );
			}
			else if ( strcmp( pNode->GetNodeName(), "AccHeadB1" ) == 0 )
			{
				pNode->AddChild( m_pModelInstanceNode[MODEL_ID_HAT] );
			}
			else if ( strcmp( pNode->GetNodeName(), "AccEyeA1" ) == 0 )
			{
				pNode->AddChild( m_pModelInstanceNode[MODEL_ID_SUNGLASS_A] );
			}
			else if ( strcmp( pNode->GetNodeName(), "AccEyeB1" ) == 0 )
			{
				pNode->AddChild( m_pModelInstanceNode[MODEL_ID_SUNGLASS_B] );
			}
			else if ( strcmp( pNode->GetNodeName(), "AccEyeC1" ) == 0 )
			{
				pNode->AddChild( m_pModelInstanceNode[MODEL_ID_SUNGLASS_C] );
			}
		}
		{
			// 射影変換行列(パースペクティブ(透視法)射影)
			m_Projection = math::Matrix::GetPerspective(
					math::Util::ConvDegToRad(37.0f),		// 視野角30°
					(float)GFGLUtil::GetDisplayWidthByDisplayMode( DisplayMode::HD720P ) / (float)GFGLUtil::GetDisplayHeightByDisplayMode( DisplayMode::HD720P ),	// アスペクト比
					1.1f,							// 前方投影面までの距離
					1000.0f);							// 後方投影面までの距離

			//m_pDrawManager->SetProjectionMatrix( m_Projection );
      m_RenderingPipeLine->SetProjectionMatrix(m_Projection);
		}

		{
			// ビュー変換行列
			math::Vector eyePosition( 0.0f, 20.0f, 200.0f, 1.0f );  // 視点(カメラの位置)
			math::Vector focusPosition( 0.0f, 20.0f,  0.0f, 1.0f );  // 注視点
			math::Vector upDirection( 0.0f, 1.0f,  0.0f, 1.0f );  // カメラの上方向

			m_View = math::Matrix::GetLookAt(eyePosition, focusPosition, upDirection);

			//m_pDrawManager->SetViewMatrix( m_View );
      m_RenderingPipeLine->SetViewMatrix(m_View);
		}

    // 簡易描画初期化
		DrawUtil::Initialize(&m_Allocator, &m_Handle);

    // デバッグウィンドウ作成
    gfl2::renderingengine::scenegraph::DagNode* pRootNode = scenegraph::SceneGraphManager::GetRootNode();
    m_pDebugWindowProc = new DebugWindowProc();
    m_pDebugWindowProc->ParseNode(pRootNode);

    return true;
	}

	virtual bool OnUpdate( void )
	{
		{
			math::Matrix		matY, matX;
			f32							rotate;

			rotate = (f32)(PI * (m_FrameCnt % 3000)) / 1500.0f;
			matY = math::Matrix::GetRotationY(rotate);
			rotate = (f32)(PI * (m_FrameCnt % 1500)) / 750.0f;
			matX = math::Matrix::GetRotationX(rotate);

			//m_World = matY * matX;
			m_World = matY;
      //m_pModelInstanceNode[MODEL_ID_BODY]->SetLocalMatrix(m_World);
		}

		//ジョイントマトリックス更新
		for( u32 i = 0; i < MODEL_MAX; ++i )
		{
			if ( m_AnimationControler[i].GetFrameSize() )
			{
				u32			animFrame = (m_FrameCnt/2) % m_AnimationControler[i].GetFrameSize();
				m_AnimationControler[i].ApplyAnimation( animFrame );
			}
		}

		{//トラバース処理にてワールド座標を更新する場合。
      scenegraph::SceneGraphManager::TraverseModel();
		}

		++m_FrameCnt;

    // デバッグウィンドウ更新
    this->m_pDebugWindowProc->Update();

		return true;
	}

	virtual bool OnDraw( void )
	{
		GFGL::SetActiveDisplay( m_Handle );

		Surface*			pBackBuffer = GFGL::GetBackBuffer( m_Handle );
		Surface*			pDepthBuffer = GFGL::GetDepthStencilBuffer( m_Handle );

    // 選択中のノードを設定
    m_RenderingPipeLine->SetSelectedNode(m_pDebugWindowProc->GetSelectedNode());

		m_RenderingPipeLine->SetRenderTarget( pBackBuffer );
		m_RenderingPipeLine->SetDepthStencil( pDepthBuffer );

		m_RenderingPipeLine->Execute();

    // デバッグウィンドウ描画
    m_pDebugWindowProc->Draw();


		return true;
	}

	virtual bool OnTerm( void )
	{
		//m_pDrawManager.reset();

		for( u32 i = 0; i < MODEL_MAX; ++i )
		{
			delete m_pMotionResourceRootNode[i];
			delete m_pModelResourceRootNode[i];
		}

		delete m_pModelInstanceNode[0];

		delete m_pLocatorResourceRootNode;
		
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