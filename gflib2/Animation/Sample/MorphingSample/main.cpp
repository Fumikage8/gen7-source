#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_GFGLUtil.h>

#include <RenderingEngine/Sample/Framework/IApplication.h>

#include <util/include/gfl2_FixedSizeContainer.h>

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMotData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/Renderer/gfl2_DrawManager.h>
#include <system/include/InputDevice/gfl2_Pad.h>
#include <system/include/Timer/gfl2_PerformanceCounter.h>
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>

#include <Animation/include/gfl2_AnimationController.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::system;
using namespace gfl2::renderingengine;
using namespace gfl2::animation;

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
	math::Matrix44																							m_Projection;
	math::Matrix34																							m_View;
	math::Matrix34																							m_World;

	renderer::DefaultRenderingPipeLine												*m_RenderingPipeLine;
	scenegraph::resource::ResourceNode												*m_pModelResourceRootNode[MODEL_MAX];
	scenegraph::resource::ResourceNode												*m_pMotionResourceRootNode[MODEL_MAX];
	scenegraph::instance::ModelInstanceNode										*m_pModelInstanceNode[MODEL_MAX];
	AnimationController																				m_AnimationController[MODEL_MAX];

	s32																					m_FrameCnt;
	s32																					m_AnimFrame;
	s32																					m_DrawMode;
	u64																					m_RenderingTime;

	SimpleDraw() : 
		Sample::Framework::IApplication(),
		m_FrameCnt(0),
		m_AnimFrame(0),
		m_DrawMode(0),
		m_RenderingTime(0)
	{
	
	}

	virtual ~SimpleDraw(){}

	virtual bool OnInit( void )
	{
		InputDevice::Pad::Initialize();
		GFGL::Initialize( &m_Allocator );

		m_Handle = GFGL::CreateDisplay( DisplayMode::HD720P, Format::A8R8G8B8, Format::D24S8, MultiSample::MSAA4X );

		{
			scenegraph::SceneGraphManager::Initialize();
			scenegraph::resource::ResourceManager::Initialize();
			m_RenderingPipeLine = new renderer::DefaultRenderingPipeLine();
		}

		for( u32 modelNo = 0; modelNo < MODEL_MAX; ++modelNo )
		{
			static const c8*				s_pModelFilePath[MODEL_MAX] = {
				"C:/home/niji/git_gflib2/Animation/Sample/data/tr0001old/tr0001old.gfmdl",
				"C:/home/niji/git_gflib2/Animation/Sample/data/tr0001old/tr0001morph.gfmdl",
			};

			static const c8*				s_pMotionFilePath[MODEL_MAX] = {
				"C:/home/niji/git_gflib2/Animation/Sample/data/tr0001old/anim_joint_01.gfmot",
				"C:/home/niji/git_gflib2/Animation/Sample/data/tr0001old/anim_morph_01.gfmot",
			  //"C:/home/niji/tool_project/AnimationConverter/anim_joint_01.bgfmot",
			  //"C:/home/niji/tool_project/AnimationConverter/anim_morph_01.bgfmot",
			};

			scenegraph::resource::GfMdlData							modelResourceData;
			scenegraph::resource::GfMotData							motionResourceData;

			modelResourceData.ReadFile( s_pModelFilePath[modelNo] );
			motionResourceData.ReadFile( s_pMotionFilePath[modelNo] );

			{
				u32			size = 0;
				char		buff[32];
				memset(buff, 0, sizeof(buff));

				for( u32 i = 0; i < static_cast<u32>(modelResourceData.GetSize()); ++i )
					size += modelResourceData.GetDataSize(i);
				size /= 1024;
				itoa( size, buff, 10 );

				::OutputDebugStringA( s_pModelFilePath[modelNo] );
				::OutputDebugStringA( " = " );
				::OutputDebugStringA( buff );
				::OutputDebugStringA( " KByte\n" );
			}

			{
				u32			size = 0;
				char		buff[32];
				memset(buff, 0, sizeof(buff));

				for( u32 i = 0; i < static_cast<u32>(motionResourceData.GetSize()); ++i )
					size += motionResourceData.GetDataSize(i);
				size /= 1024;
				itoa( size, buff, 10 );

				::OutputDebugStringA( s_pMotionFilePath[modelNo] );
				::OutputDebugStringA( " = " );
				::OutputDebugStringA( buff );
				::OutputDebugStringA( " KByte\n" );
			}

			m_pModelResourceRootNode[modelNo] = scenegraph::resource::ResourceManager::CreateResourceNode( &modelResourceData );
			m_pMotionResourceRootNode[modelNo] = scenegraph::resource::ResourceManager::CreateResourceNode( &motionResourceData );
		}

		for( u32 modelNo = 0; modelNo < MODEL_MAX; ++modelNo )
		{
			//同じリソースノードからインスタンスノードを２つ生成
			m_pModelInstanceNode[modelNo] = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pModelResourceRootNode[modelNo] );

      AnimationController::InitDescription desc;
      desc.jointSlotCount = 1;
      if (modelNo != 0)
      {
        desc.blendShapeSlotCount = 1;
      }
			m_AnimationController[modelNo].Initialize( m_pModelInstanceNode[modelNo], desc );

      JointAnimationSlot* pJointSlot = m_AnimationController[modelNo].GetJointSlot();
      pJointSlot->SetAnimationResource(m_pMotionResourceRootNode[modelNo]);
      pJointSlot->SetLoop(true);
      pJointSlot->SetStepFrame(0.1f);

      if (modelNo != 0)
      {
        BlendShapeAnimationSlot* pBlendShape = m_AnimationController[modelNo].GetBlendShapeSlot();
        pBlendShape->SetAnimationResource(m_pMotionResourceRootNode[modelNo]);
        pBlendShape->SetLoop(true);
        pBlendShape->SetStepFrame(0.1f);
      }
		}

		{
			// 射影変換行列(パースペクティブ(透視法)射影)
			m_Projection = math::Matrix44::GetPerspective(
					math::ConvDegToRad(37.0f),		// 視野角30°
					(float)GFGLUtil::GetDisplayWidthByDisplayMode( DisplayMode::HD720P ) / (float)GFGLUtil::GetDisplayHeightByDisplayMode( DisplayMode::HD720P ),	// アスペクト比
					1.1f,							// 前方投影面までの距離
					1000.0f);							// 後方投影面までの距離

			m_RenderingPipeLine->SetProjectionMatrix( m_Projection );
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

			if( InputDevice::Pad::IsPush( padNo, InputDevice::Button::A ) )
			{
				++m_AnimFrame;
			}
			else if( InputDevice::Pad::IsPress( padNo, InputDevice::Button::B ) )
			{
				++m_AnimFrame;
			}

			if( InputDevice::Pad::IsPush( padNo, InputDevice::Button::Y ) )
			{
				m_DrawMode++;
				m_DrawMode %= 3;
			}
		}

		{
			// ビュー変換行列
			math::Vector3 eyePosition( 0.0f, 100.0f, 150.0f );  // 視点(カメラの位置)
			math::Vector3 focusPosition( 0.0f, 100.0f,  0.0f );  // 注視点
			math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向

			m_View = math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);

			m_RenderingPipeLine->SetViewMatrix( m_View );
		}

		{
			math::Matrix34		matY, matX;
			f32							rotate;

			rotate = (f32)(PI * (m_FrameCnt % 3000)) / 1500.0f;
			matY = math::Matrix34::GetRotationY(rotate);
			rotate = (f32)(PI * (m_FrameCnt % 1500)) / 750.0f;
			matX = math::Matrix34::GetRotationX(rotate);

			//m_World = matY * matX;
			//m_World = matY;
			m_World = math::Matrix34::GetIdentity();
		}

		for( u32 i = 0; i < MODEL_MAX; ++i )
		{
      m_AnimationController[i].Update();
		}

		m_pModelInstanceNode[0]->SetVisible( true );
		m_pModelInstanceNode[1]->SetVisible( true );

		switch( m_DrawMode ){
		case 0:
			{
				m_pModelInstanceNode[0]->SetLocalTranslate(math::Vector3(0, 0, 0));
				m_pModelInstanceNode[1]->SetLocalTranslate(math::Vector3(0, 0, 0));
				m_pModelInstanceNode[0]->SetVisible( true );
				m_pModelInstanceNode[1]->SetVisible( false );
			}
			break;
		case 1:
			{
				m_pModelInstanceNode[0]->SetLocalTranslate(math::Vector3(0, 0, 0));
				m_pModelInstanceNode[1]->SetLocalTranslate(math::Vector3(0, 0, 0));
				m_pModelInstanceNode[0]->SetVisible( false );
				m_pModelInstanceNode[1]->SetVisible( true );
			}
			break;
		case 2:
			{
				m_pModelInstanceNode[0]->SetLocalTranslate(math::Vector3( -40.0f, 0.0f, 0.0f ));
				m_pModelInstanceNode[1]->SetLocalTranslate(math::Vector3( 40.0f, 0.0f, 0.0f ));

				m_pModelInstanceNode[0]->SetVisible( true );
				m_pModelInstanceNode[1]->SetVisible( true );
			}
			break;
		default:
			break;
		}

    // ワールド行列更新
    scenegraph::SceneGraphManager::TraverseModel();

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
		InputDevice::Pad::Terminate();

		for( u32 i = 0; i < MODEL_MAX; ++i )
		{
			if (m_pMotionResourceRootNode[i] != NULL) delete m_pMotionResourceRootNode[i];
			if (m_pModelResourceRootNode[i] != NULL) delete m_pModelResourceRootNode[i];
			if (m_pModelInstanceNode[i] != NULL) delete m_pModelInstanceNode[i];
		}

		if (m_RenderingPipeLine != NULL) delete m_RenderingPipeLine;

		scenegraph::resource::ResourceManager::Terminate();
    scenegraph::SceneGraphManager::Terminate();

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