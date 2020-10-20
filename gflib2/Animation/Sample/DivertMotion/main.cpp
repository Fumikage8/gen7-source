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
#include <Animation/include/gfl2_AnimationController.h>
#include <system/include/InputDevice/gfl2_Pad.h>
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>

#include <math/include/gfl2_QuaternionNative.h>
#include <math/include/gfl2_Quaternion.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::animation;
using namespace gfl2::system;

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
	static const u32																					ModelMax = 3;
	static const u32																					MotionMax = 6;

	math::Matrix44																							m_Projection;
	math::Matrix34																							m_View;
	math::Matrix34																							m_World;

	renderer::DefaultRenderingPipeLine												*m_RenderingPipeLine;
	scenegraph::resource::ResourceNode												*m_pModelResourceRootNode[ModelMax];
	scenegraph::resource::ResourceNode												*m_pMotionResourceRootNode[MotionMax];
	scenegraph::instance::ModelInstanceNode										*m_pModelInstanceNode[ModelMax];
	AnimationController																				m_AnimationController[ModelMax];
	//AnimationController::DivertMotionInfo											m_DivertMotionInfo;

	scenegraph::resource::ResourceNode												*m_pPlaneModelResourceRootNode;
	scenegraph::instance::ModelInstanceNode										*m_pPlaneModelInstanceNode;

	s32																												m_FrameCnt;
	s32																												m_MotionNo;
	math::Vector3																							m_CameraControl;

	SimpleDraw() : 
		Sample::Framework::IApplication(),
		m_FrameCnt(0),
		m_MotionNo(0),
		m_CameraControl(0, 0, 300)
	{
	
	}

	virtual ~SimpleDraw(){}

	virtual bool OnInit( void )
	{
		math::Matrix				lerpMat;
		math::Vector				col0(0, 0, 0, 0);

		GFGL::Initialize( &m_Allocator );
		InputDevice::Pad::Initialize();

		m_Handle = GFGL::CreateDisplay( DisplayMode::HD720P, Format::A8R8G8B8, Format::D24S8, MultiSample::MSAA4X );

		{
			scenegraph::SceneGraphManager::Initialize();
			scenegraph::resource::ResourceManager::Initialize();
			m_RenderingPipeLine = new renderer::DefaultRenderingPipeLine();
		}

		{
			scenegraph::resource::GfMdlData							modelResourceData;
			modelResourceData.ReadFile( "C:/home/niji/git_gflib2/Animation/Sample/data/plane/plane.gfmdl" );
			m_pPlaneModelResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &modelResourceData );

			m_pPlaneModelInstanceNode = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pPlaneModelResourceRootNode );

      m_pPlaneModelInstanceNode->SetLocalScale(math::Vector3( 10.0f, 0.1f, 10.0f ));
		}

		for( u32 i = 0; i < ModelMax; ++i )
		{
			static const c8*				s_pModelFilePath[ModelMax] = {
				"C:/home/niji/git_gflib2/Animation/Sample/data/testpreset_B/testpreset_B.gfmdl",
				"C:/home/niji/git_gflib2/Animation/Sample/data/testpreset_Dm/testpreset_Dm.gfmdl",
				"C:/home/niji/git_gflib2/Animation/Sample/data/testpreset_E/testpreset_E.gfmdl"
				/*"C:/next/next_3d/chara/motion_test/testpreset_B/exports/testpreset_B.gfmdl",
				"C:/next/next_3d/chara/motion_test/testpreset_Dm/exports/testpreset_Dm.gfmdl",
				"C:/next/next_3d/chara/motion_test/testpreset_E/exports/testpreset_E.gfmdl"*/
			};

			scenegraph::resource::GfMdlData							modelResourceData;

			modelResourceData.ReadFile( s_pModelFilePath[i] );

			m_pModelResourceRootNode[i] = scenegraph::resource::ResourceManager::CreateResourceNode( &modelResourceData );
		}

		for( u32 i = 0; i < MotionMax; ++i )
		{
			static const c8*				s_pMotionFilePath[MotionMax] = {
				"C:/home/niji/git_gflib2/Animation/Sample/data/DivertMotion/testpreset_Dm_wait01.gfmot",
				"C:/home/niji/git_gflib2/Animation/Sample/data/DivertMotion/testpreset_Dm_wait02.gfmot",
				"C:/home/niji/git_gflib2/Animation/Sample/data/DivertMotion/testpreset_Dm_run.gfmot",
				"C:/home/niji/git_gflib2/Animation/Sample/data/DivertMotion/testpreset_Dm_sit.gfmot",
				"C:/home/niji/git_gflib2/Animation/Sample/data/DivertMotion/testpreset_Dm_scale01.gfmot",
				"C:/home/niji/git_gflib2/Animation/Sample/data/DivertMotion/testpreset_Dm_peace.gfmot"
				/*"C:/next/next_3d/chara/motion_test/testpreset_Dm/exports/testpreset_Dm_wait01.gfmot",
				"C:/next/next_3d/chara/motion_test/testpreset_Dm/exports/testpreset_Dm_wait02.gfmot",
				"C:/next/next_3d/chara/motion_test/testpreset_Dm/exports/testpreset_Dm_run.gfmot",
				"C:/next/next_3d/chara/motion_test/testpreset_Dm/exports/testpreset_Dm_sit.gfmot",
				"C:/next/next_3d/chara/motion_test/testpreset_Dm/exports/testpreset_Dm_scale01.gfmot",
				"C:/next/next_3d/chara/motion_test/testpreset_Dm/exports/testpreset_Dm_peace.gfmot"*/
			};

			scenegraph::resource::GfMotData							motionResourceData;

			motionResourceData.ReadFile( s_pMotionFilePath[i] );

			m_pMotionResourceRootNode[i] = scenegraph::resource::ResourceManager::CreateResourceNode( &motionResourceData );

			{//指定ジョイントノードのアニメーションを切るサンプル(部分アニメーションサンプル)
				class ResourceNodeVisitor
				{
				public:
					ResourceNodeVisitor(){}
					virtual ~ResourceNodeVisitor() {}

					util::FixedSizeContainer<scenegraph::resource::ResourceNode*, 256>				m_pJointContainer;

					void Visit( scenegraph::resource::ResourceNode* pNode )
					{
						static const char* pJointName[] = {
							"EndHead", "FJlipL", "FJlipU", "LFJLipS", "Hair01A", "Hair02A",
							"EndlipL", "FJteethL", "EndlipU", "FJteethU", "EndLLipS", "Hair01B", "Hair02B",
							"EndteethL", "EndteethU", "Hair01C", "Hair02C",
							"EndHair01", "EndHair02",
							"EndRLipS", "EndRLipS", "Mic", "EndMic", "Hair", "EndHair", "RFJLipS", "EndLipS"
						};
						u32		jointNum = sizeof(pJointName) / sizeof(pJointName[0]);

						const c8*			pName = pNode->GetNodeName();
						for( u32 i = 0; i < jointNum; ++i )
						{
							if ( std::strcmp( pName, pJointName[i] ) == 0 )
							{
								m_pJointContainer.push_back( pNode );
								break;
							}
						}
					}
				};

				scenegraph::Traverser::Visitor<scenegraph::resource::ResourceNode, ResourceNodeVisitor>				visitor;
				scenegraph::Traverser::Traverse<scenegraph::resource::ResourceNode, ResourceNodeVisitor>( m_pMotionResourceRootNode[i], &visitor, 256 );

				for( u32 i = 0; i < visitor.m_pJointContainer.size(); ++i )
				{//指定ジョイントアニメは削除
					visitor.m_pJointContainer[i]->RemoveFromParent();
					delete visitor.m_pJointContainer[i];
				}
			}
		}

    AnimationController::InitDescription desc;
		for( u32 i = 0; i < ModelMax; ++i )
		{
			m_pModelInstanceNode[i] = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pModelResourceRootNode[i] );

      desc.jointSlotCount = 1;
			m_AnimationController[i].Initialize( m_pModelInstanceNode[i], desc );
		}

		{
			const c8*																	pTransformJointName = "Waist";
			math::Matrix															centerMat;

			//centerMat = m_AnimationController[1].GetMatrix( pTransformJointName );//今回の共有モーションはtestpreset_Dm.gfmdl基準

			//m_DivertMotionInfo.pTransformJointName = pTransformJointName;
			//m_DivertMotionInfo.divertTransformJointHeight = centerMat[3].y;

			for( u32 i = 0; i < ModelMax; ++i )
			{
        JointAnimationSlot* pJointSlot = m_AnimationController[i].GetJointSlot();
        pJointSlot->SetAnimationResource(m_pMotionResourceRootNode[0]);
        pJointSlot->SetLoop(true);
        pJointSlot->SetStepFrame(0.5f);

        // 共有モーション設定
        if (i != 1)
        {
          pJointSlot->SetDivertAnimation("Waist", true);
        }
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

		for( u32 i = 0; i < ModelMax; ++i )
		{
			m_pModelInstanceNode[i]->SetLocalTranslate(math::Vector3( -100.0f + (100.0f * i), 0.0f, 0.0f ));
		}

		return true;
	}

	virtual bool OnUpdate( void )
	{
		InputDevice::Pad::Update();

		if ( InputDevice::Pad::IsConnecting( 0 ) )
		{
			if( InputDevice::Pad::IsPush( 0, InputDevice::Button::Up ) )
			{
				++m_MotionNo;
				m_MotionNo%=MotionMax;

				for( u32 i = 0; i < ModelMax; ++i )
        {
          JointAnimationSlot* pJointSlot = m_AnimationController[i].GetJointSlot();
          pJointSlot->SetAnimationResource(m_pMotionResourceRootNode[m_MotionNo]);
          pJointSlot->SetLoop(true);
          pJointSlot->SetStepFrame(0.5f);
        }
			}
			else if( InputDevice::Pad::IsPush( 0, InputDevice::Button::Down ) )
			{
				--m_MotionNo;
				if ( m_MotionNo < 0 )
					m_MotionNo = MotionMax - 1;

				for( u32 i = 0; i < ModelMax; ++i )
        {
          JointAnimationSlot* pJointSlot = m_AnimationController[i].GetJointSlot();
          pJointSlot->SetAnimationResource(m_pMotionResourceRootNode[m_MotionNo]);
          pJointSlot->SetLoop(true);
          pJointSlot->SetStepFrame(0.5f);
        }
			}

			{
				InputDevice::StickPos				pos;
				pos = InputDevice::Pad::GetStick( 0, InputDevice::Stick::Right );
				m_CameraControl.x += -pos.y;//X軸回転
				m_CameraControl.y += -pos.x;//Y軸回転

				pos = InputDevice::Pad::GetStick( 0, InputDevice::Stick::Left );
				m_CameraControl.z += pos.y;//奥行き移動
			}
		}

		{
			// ビュー変換行列
			math::Vector3 eyePosition( 0.0f, 0.0f, m_CameraControl.z );  // 視点(カメラの位置)
			math::Vector3 focusPosition( 0.0f, 80.0f,  0.0f );  // 注視点
			math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向

			math::Matrix34			rotX = math::Matrix34::GetRotationX( math::ConvDegToRad(m_CameraControl.x) );
			math::Matrix34			rotY = math::Matrix34::GetRotationY( math::ConvDegToRad(m_CameraControl.y) );
			math::Matrix34			rotMat = rotY * rotX;

			eyePosition = rotMat * m_CameraControl;
			eyePosition.y += focusPosition.y;

			m_View = math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);

			m_RenderingPipeLine->SetViewMatrix( m_View );
		}

		{
			++m_FrameCnt;
			s32			frame = m_FrameCnt / 2;

			for( u32 i = 0; i < ModelMax; ++i )
			{
        m_AnimationController[i].Update();
			}
		}

    // ワールド行列更新
    scenegraph::SceneGraphManager::TraverseModel();

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
		for( u32 i = 0; i < ModelMax; ++i )
		{
			if (m_pMotionResourceRootNode[i] != NULL) delete m_pMotionResourceRootNode[i];
			if (m_pModelResourceRootNode[i] != NULL) delete m_pModelResourceRootNode[i];
			if (m_pModelInstanceNode[i] != NULL) delete m_pModelInstanceNode[i];
		}

		if (m_pPlaneModelResourceRootNode != NULL) delete m_pPlaneModelResourceRootNode;
		if (m_pPlaneModelInstanceNode != NULL) delete m_pPlaneModelInstanceNode;

		if (m_RenderingPipeLine != NULL) delete m_RenderingPipeLine;

		scenegraph::resource::ResourceManager::Terminate();
    scenegraph::SceneGraphManager::Terminate();

		GFGL::Finish();

		GFGL::SetActiveDisplay(NULL);
		GFGL::DestroyDisplay(m_Handle);

		InputDevice::Pad::Terminate();
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