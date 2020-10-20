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
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/renderer/gfl2_DrawManager.h>
#include <system/include/InputDevice/gfl2_Pad.h>
#include <system/include/Timer/gfl2_PerformanceCounter.h>


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
	static const u32																					MODEL_MAX = 2;
	math::Matrix																							m_Projection;
	math::Matrix																							m_View;
	math::Matrix																							m_World;

	renderer::DrawManager																			*m_pDrawManager;
	scenegraph::resource::ResourceNode												*m_pModelResourceRootNode[MODEL_MAX];
	scenegraph::resource::ResourceNode												*m_pMotionResourceRootNode[MODEL_MAX];
	scenegraph::instance::ModelInstanceNode										*m_pModelInstanceNode[MODEL_MAX];
	AnimationControler																				m_AnimationControler[MODEL_MAX];

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
			m_pDrawManager = renderer::DrawManager::Create();
		}

		for( u32 modelNo = 0; modelNo < MODEL_MAX; ++modelNo )
		{
			static const c8*				s_pModelFilePath[MODEL_MAX] = {
				"C:/home/niji/git_gflib2/RenderingEngine/Sample/data/tr0001old/tr0001old.gfmdl",
				"C:/home/niji/git_gflib2/RenderingEngine/Sample/data/tr0001old/tr0001morph.gfmdl",
			};

			static const c8*				s_pMotionFilePath[MODEL_MAX] = {
				"C:/home/niji/git_gflib2/RenderingEngine/Sample/data/tr0001old/anim_joint_01.gfmot",
				"C:/home/niji/git_gflib2/RenderingEngine/Sample/data/tr0001old/anim_morph_01.gfmot",
			};

			/*static const c8*				s_pModelFilePath[MODEL_MAX] = {
				"C:/Users/ishiguro_masateru/Desktop/Draw_Test/Defoult/tr0001_00_Defoult.gfmdl",
				"C:/Users/ishiguro_masateru/Desktop/Draw_Test/Target2/tr0001_00_Target2.gfmdl",
			};

			static const c8*				s_pMotionFilePath[MODEL_MAX] = {
				"C:/Users/ishiguro_masateru/Desktop/Draw_Test/Defoult/tr0001_00_Defoult.gfmot",
				"C:/Users/ishiguro_masateru/Desktop/Draw_Test/Target2/tr0001_00_Target2.gfmot",
			};*/

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

			m_AnimationControler[modelNo].Set( m_pModelInstanceNode[modelNo], m_pMotionResourceRootNode[modelNo] );

			//描画マネージャ経由で描画する場合は、追加します。インスタンスよりメッシュ情報を手動で引っ張ってきて自前で描画することもできます。
			m_pDrawManager->AddObject( m_pModelInstanceNode[modelNo] );
		}

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
			//m_World = matY;
			m_World = math::Matrix::GetIdentity();
		}

		for( u32 i = 0; i < MODEL_MAX; ++i )
		{
			u32			animFrame = m_AnimFrame % m_AnimationControler[i].GetFrameSize();
			m_AnimationControler[i].ApplyAnimation( animFrame );
		}

		m_pDrawManager->RemoveObject( m_pModelInstanceNode[0] );
		m_pDrawManager->RemoveObject( m_pModelInstanceNode[1] );

    math::Quaternion rot0;
    rot0.MatrixToQuaternion(m_World);
    m_pModelInstanceNode[0]->SetLocalTranslate(math::Vector(0.0f, 0.0f, 0.0f));
    m_pModelInstanceNode[0]->SetLocalRotation(rot0);

    math::Quaternion rot1;
    rot1.MatrixToQuaternion(m_World);
    m_pModelInstanceNode[1]->SetLocalTranslate(math::Vector(0.0f, 0.0f, 0.0f));
    m_pModelInstanceNode[1]->SetLocalRotation(rot1);

		switch( m_DrawMode ){
		case 0:
			{
				m_pDrawManager->AddObject( m_pModelInstanceNode[0] );
			}
			break;
		case 1:
			{
				m_pDrawManager->AddObject( m_pModelInstanceNode[1] );
			}
			break;
		case 2:
			{
        m_pModelInstanceNode[0]->SetLocalTranslate(math::Vector(-40.0f, 0.0f, 0.0f));
        m_pModelInstanceNode[1]->SetLocalTranslate(math::Vector(40.0f, 0.0f, 0.0f));

				m_pDrawManager->AddObject( m_pModelInstanceNode[0] );
				m_pDrawManager->AddObject( m_pModelInstanceNode[1] );
			}
			break;
		default:
			break;
		}

		{//トラバース処理にてワールド座標を更新する場合。
      scenegraph::SceneGraphManager::TraverseModel();
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

		GFGL::ClearRenderTarget( pBackBuffer, Color::GetDarkGray() );
		GFGL::ClearDepthStencil( pDepthBuffer, 1.0f, 255 );

		GFGL::SetViewport(0, 0, 1280, 720);

		{
			GFGL::Finish();
			u64				start = Timer::PerformanceCounter::GetMicroSecond();
			m_pDrawManager->Draw();
			GFGL::Finish();
			u64				end = Timer::PerformanceCounter::GetMicroSecond();

			m_RenderingTime += end - start;

			if ( (m_FrameCnt % 60) == 0 )
			{
				char		buff[32];
				memset(buff, 0, sizeof(buff));
				itoa( static_cast<s32>(m_RenderingTime / 60), buff, 10 );
				::OutputDebugStringA( buff );
				::OutputDebugStringA( "\n" );
				m_RenderingTime = 0;
			}
		}

		GFGL::EndScene();

		GFGL::SwapDisplay();

		return true;
	}

	virtual bool OnTerm( void )
	{
		InputDevice::Pad::Terminate();

		GFX_SAFE_DELETE( m_pDrawManager );

		for( u32 i = 0; i < MODEL_MAX; ++i )
		{
			delete m_pMotionResourceRootNode[i];
			delete m_pModelResourceRootNode[i];
			delete m_pModelInstanceNode[i];
		}

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