#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathTriangular.h>
#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_GFGLUtil.h>

#include <RenderingEngine/Sample/Framework/IApplication.h>

#include <util/include/gfl2_FixedSizeContainer.h>

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
//#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/Renderer/gfl2_DrawManager.h>
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>

#include <math/include/gfl2_QuaternionNative.h>
#include <math/include/gfl2_Quaternion.h>

#include <math/include/gfl2_EulerRotation.h>

#include <Effect/include/gfl2_EffectSystem.h>
#include <Effect/include/gfl2_EffectRenderPath.h>

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


class SampleRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
public:
	SampleRenderingPipeLine( const Effect::EffectRenderPath::Config& effect_config ) : 
    gfl2::renderingengine::renderer::RenderingPipeLine(),
    m_EffectRenderPath(effect_config),
		m_RenderPathCnt(0)
	{
		SceneRenderPath::InitDescription			desc;
		m_SceneRenderPath.Initialize( desc );
	}
	virtual ~SampleRenderingPipeLine()
	{

	}

  void AddDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode)
  {
    m_SceneRenderPath.AddDrawEnv(pNode);
  }

private:
	virtual b32 StartRenderPath()
	{
		m_RenderPathCnt = 0;
		return true;
	}

	virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath()
	{
		switch( m_RenderPathCnt ){
		case 0:
			return &m_SceneRenderPath;
			break;
		case 1:
			return &m_EffectRenderPath;
			break;
		case 2:
			return &m_PresentPath;
			break;
		}
		return NULL;
	}

	virtual b32 NextRenderPath()
	{
		m_RenderPathCnt++;
		if ( m_RenderPathCnt == 3 )
			return false;
		return true;
	}
	
	gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath		m_SceneRenderPath;
  gfl2::Effect::EffectRenderPath m_EffectRenderPath;
	gfl2::renderingengine::renderer::RenderingPipeLine::PresentPath				m_PresentPath;
	u32																																		m_RenderPathCnt;

};

class SimpleDraw : public Sample::Framework::IApplication
{
public:
	math::Matrix44																							m_Projection;
	math::Matrix34																							m_View;
	math::Matrix34																							m_World;

	SampleRenderingPipeLine												  *m_RenderingPipeLine;

  Effect::System* m_pEffectSys;   //! エフェクトシステム
  Effect::GFHeap * m_pEffectHeap;
  util::List<u32>* m_pGroupIDList;
  heap::HeapBase * m_pHeap;
  void*           m_pEffectResource;
  Effect::Handle m_EffectHandle;


	SimpleDraw() : 
		Sample::Framework::IApplication()
	{
	
	}

	virtual ~SimpleDraw(){}

	virtual bool OnInit( void )
	{
		math::Matrix				lerpMat;
		math::Vector				col0(0, 0, 0, 0);

    m_pHeap = gfl2::heap::Manager::GetHeapByHeapId(0);

		GFGL::Initialize( &m_Allocator );

		m_Handle = GFGL::CreateDisplay( DisplayMode::HD720P, Format::A8R8G8B8, Format::D24S8, MultiSample::None );

		{
			scenegraph::SceneGraphManager::Initialize();
			scenegraph::resource::ResourceManager::Initialize();
		}

    {
      m_pEffectHeap = new Effect::GFHeap(m_pHeap);
    }

    {
      Effect::Config config;

      config.SetEffectHeap( m_pEffectHeap );
#ifdef GF_PLATFORM_CTR
        config.SetParticleMemType(nw::eft::BUFFER_LOCATION_FCRAM);
#endif //GF_PLATFORM_CTR

      m_pEffectSys = new Effect::System(config);


      m_pGroupIDList = new util::List<u32>( m_pHeap, 1 );
      m_pGroupIDList->PushFront(0); // GroupID=0を追加
    }

    {
      Effect::EffectRenderPath::Config effect_config;

      effect_config.m_pSystem = m_pEffectSys;
      effect_config.m_cpGroupIDList = m_pGroupIDList;

			m_RenderingPipeLine = new SampleRenderingPipeLine( effect_config );
    }


    // エフェクトリソース読み込み＆登録
    {
      s32 size;
#if defined(GF_PLATFORM_WIN32)
      m_pEffectResource = ReadFile( m_pHeap, "../data/out_win32_/sample.wingl.ptcl", &size, 128 );
#elif defined(GF_PLATFORM_CTR)
      //m_pEffectResource = ReadFile( m_pHeap, "../../data/out_ctr_/sample.ctr.ptcl", &size, 128 );
      m_pEffectResource = ReadFile( m_pHeap, "rom:/sample.ctr.ptcl", &size, 128 );
#endif

      m_pEffectSys->EntryResource( m_pEffectHeap, m_pEffectResource, 0 );
      //m_pEffectSys->EntryResource( m_pEffectHeap, m_pEffectResource, 1 );
    }

		{
			// 射影変換行列(パースペクティブ(透視法)射影)
			m_Projection = math::Matrix44::GetPerspective(
        GFL_MATH_DEG_TO_RAD(37.0f),		// 視野角30°
					(float)GFGLUtil::GetDisplayWidthByDisplayMode( DisplayMode::HD720P ) / (float)GFGLUtil::GetDisplayHeightByDisplayMode( DisplayMode::HD720P ),	// アスペクト比
					1.1f,							// 前方投影面までの距離
					1000.0f);							// 後方投影面までの距離

			m_RenderingPipeLine->SetProjectionMatrix( m_Projection );
		}

		return true;
	}

	virtual bool OnUpdate( void )
	{
    //Sleep(16);
		{
			// ビュー変換行列
			math::Vector3 eyePosition( 0.0f, 30.0f, 50.0f );  // 視点(カメラの位置)
			math::Vector3 focusPosition( 0.0f, 0.0f,  0.0f );  // 注視点
			math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向

			m_View = math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);

			m_RenderingPipeLine->SetViewMatrix( m_View );
		}

    {
      // エミッターが無くなったら初期化
      if( m_pEffectSys->GetNwObject()->GetNumEmitter( 0 ) == 0 )
      {
        m_pEffectSys->CreateEmitterSetID( &m_EffectHandle, math::Vector3(0,0,0), 0, 0, 0 );
      }
    }

    // エフェクト更新処理
    {
      m_pEffectSys->BeginFrame();
      m_pEffectSys->Calc( 0 );
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


		if (m_RenderingPipeLine != NULL) delete m_RenderingPipeLine;

    m_pEffectSys->KillAllEmitter();  //CTRは1フレーム遅れ破棄が必要
    m_pEffectSys->ClearResource(m_pEffectHeap, 0);
    m_pEffectSys->ClearResource(m_pEffectHeap, 1);

    delete m_pEffectSys;   //! エフェクトシステム
    delete m_pGroupIDList;
    delete m_pEffectHeap;

    GflHeapFreeMemory( m_pEffectResource );


		scenegraph::resource::ResourceManager::Terminate();
    scenegraph::SceneGraphManager::Terminate();

		GFGL::Finish();

		GFGL::SetActiveDisplay(NULL);
		GFGL::DestroyDisplay(m_Handle);
		GFGL::Terminate();


    gfl2::heap::Manager::Finalize();

		return true;
	}

	//いずれＩＯクラスができたら置き換える。
	static c8* ReadFile( gfl2::heap::HeapBase* heap, const char* filename, s32* p_size, s32 align = 16 )
	{
#if defined(GF_PLATFORM_CTR)
    nn::fs::FileReader file(filename);
    u32 fileSize = file.GetSize();
    c8* pBinary = (c8*)GflHeapAllocMemoryAlign( heap, fileSize, 128 );
    file.Read( (void*)pBinary, fileSize);
    if(p_size){ *p_size = fileSize; }
    return pBinary;

#elif defined(GF_PLATFORM_WIN32)


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

		read_buff = reinterpret_cast<c8*>( GflHeapAllocMemoryAlign( heap, file_size, 128 ) );
		memset( read_buff, 0, file_size );
		fread( read_buff, file_size, 1, hFile );
		fclose( hFile );

	
		return( read_buff );
#endif
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








#if defined(WIN32)

#include <windows.h>

int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szCmdLine, int sw )
{	
	MSG msg;

  {
    // ヒープ生成
    const u32 total_heap_size = 30*1024*1024; // 30MB
#if defined(GF_PLATFORM_WIN32)
    void * mem = malloc( total_heap_size );
#elif defined(GF_PLATFORM_CTR)
    void * mem = reinterpret_cast<void*>(nn::os::GetDeviceMemoryAddress()); // Deviceヒープから取る
#endif

    gfl2::heap::Manager::Initialize( mem, total_heap_size, 1 );
  }

	Sample::Framework::IApplication	*app = GetApplication();

	app->OnInit();

	while( 1 )
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) == TRUE )
		{
			if( msg.message == WM_QUIT ){
				break;
			}
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			app->OnUpdate();
			app->OnDraw();
		}
	}

	app->OnTerm();

	TerminateApplication();

	return (int)msg.wParam;
}

#elif defined(GF_PLATFORM_CTR)

#include <nn/os.h>
#include <nn/dbg.h>
#include <nn/init.h>

#include <nn/applet.h>

static void SetStartupMemorySize( size_t device_memory_size, size_t heap_memory_size )
{
#if NN_PLATFORM_HAS_MMU
#pragma push
#pragma diag_suppress 1361

  //size_tはunsignd intなので0以下チェックできない。なのでs32にした
  const s32 assingment   = nn::os::GetAppMemorySize();
  const s32 currentUsing = nn::os::GetUsingMemorySize();
  const s32 programsize  = nn::os::GetCodeRegionSize();

  const s32 available    = assingment - currentUsing;
  const s32 freeSize     = available - static_cast<s32>(device_memory_size + heap_memory_size);

  if ( freeSize < 0 )
  { // 足りないパターン
    GFL_ASSERT_STOP( 0 );
  }

  {
    GFL_ASSERT_STOP( heap_memory_size % 4096 == 0 );
    GFL_ASSERT_STOP( device_memory_size % 4096 == 0 );
    nn::Result result;
    result = nn::os::SetHeapSize( heap_memory_size );
    NN_UTIL_PANIC_IF_FAILED( result );
    result = nn::os::SetDeviceMemorySize( device_memory_size );
    NN_UTIL_PANIC_IF_FAILED( result );
  }

#pragma pop
#endif  // if NN_PLATFORM_HAS_MMU
}


//=======================================================
/*
  @brief  mainの前に動く初期化関数の中身
 */
//=======================================================
void StartupApplication(void)
{
  size_t device_memory_size = 0x1E00000;
  size_t heap_memory_size = 0x1E00000;

  //メモリ領域初期化
  SetStartupMemorySize( device_memory_size, heap_memory_size );

  {
    // ヒープ生成
    const u32 total_heap_size = 30*1024*1024; // 30MB
#if defined(GF_PLATFORM_WIN32)
    void * mem = malloc( total_heap_size );
#elif defined(GF_PLATFORM_CTR)
    void * mem = reinterpret_cast<void*>(nn::os::GetDeviceMemoryAddress()); // Deviceヒープから取る
#endif

    gfl2::heap::Manager::Initialize( mem, total_heap_size, 1 );
  }
}

extern "C" void nninitStartUp(void)
{
  StartupApplication();

  nn::fs::Initialize();

  enum {
    MAX_FILE = 16,
    MAX_DIRECTORY = 4,
  };

  gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId(0);

  s32 work_memory_size = nn::fs::GetRomRequiredMemorySize( MAX_FILE, MAX_DIRECTORY, true ); 
  void* work_memory = GflHeapAllocMemory( pHeap, work_memory_size ); 
  nn::Result result = nn::fs::MountRom( MAX_FILE, MAX_DIRECTORY, work_memory, work_memory_size );

  GFL_ASSERT_STOP( result.IsSuccess() );

}


void nnMain(void)
{

  // ホームメニューからも実行できるよう nn::applet::Enable() のみ呼ぶ
  // ホームメニュー遷移や電源ボタンクリック、スリープには非対応
  nn::applet::Enable();

  Sample::Framework::IApplication	*app = GetApplication();

  app->OnInit();

  while( 1 )
  {
    app->OnUpdate();
    app->OnDraw();
  }

  app->OnTerm();

  TerminateApplication();
}

#endif
