#if PM_DEBUG

#include "../include/IshiguroTestProc.h"

#include <RenderingEngine/include/Renderer/gfl2_MeshDrawTag.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>

#if defined(GF_PLATFORM_CTR)
#include <nn/fs.h>
#include <nn/hio.h>
#endif

using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(IshiguroTest)


struct LocalFunc{
	static void *ReadFile( const char* pFilePath )
  {//自前データロード
#if defined(GF_PLATFORM_CTR)

		void							*pBuffer = NULL;

#if defined( GFL_HIO_ENABLE_1496 )
    nn::hio::HostFile hf;

		nn::Result hfResult = hf.Open( pFilePath, nn::hio::HostFile::ACCESS_MODE_READ_WRITE, nn::hio::HostFile::OPEN_DISP_OPEN_ALWAYS);

		if (hfResult.IsSuccess())
		{
			s64		head;
			s64		tail;

			hf.Seek( &tail, 0, nn::hio::HostFile::SEEK_TYPE_END );
			hf.Seek( &head, 0, nn::hio::HostFile::SEEK_TYPE_SET );

			u32 fileSize = tail - head;
			pBuffer = reinterpret_cast<c8*>( GLMemory::Malloc(fileSize, 128) );

			hf.Read( pBuffer, fileSize);

			hf.Close();
		}

#endif

		return pBuffer;
#else
		FILE		*hFile;
		int			file_size;
		char		*read_buff;

		fopen_s( &hFile, pFilePath, "rb" );

		if ( hFile == NULL ){
			assert(0);
			return NULL;
		}

		fseek( hFile, 0, SEEK_END );
		file_size = ftell( hFile );
		fseek( hFile, 0, SEEK_SET );

		u32		align = 16;
		file_size += align;//必ずNULLが入るようにしたい。
		u32 buf_size = file_size + ((-file_size) & (align-1));

		read_buff = reinterpret_cast<c8*>( GLMemory::Malloc(buf_size) );
		memset( read_buff, 0, buf_size );
		fread( read_buff, file_size, 1, hFile );
		fclose( hFile );

		return read_buff;
#endif
  }
};

IshiguroTestProc::IshiguroTestProc( void ) : 
	m_Heap(NULL)
{
	
}
	
IshiguroTestProc::~IshiguroTestProc( void )
{
}

/**
* @brief   PROCの初期化を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
*/
gfl2::proc::Result IshiguroTestProc::InitFunc( gfl2::proc::Manager* pManager )
{
	gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;
#if 0	
	m_pRenderingPipeLine = GFL_NEW(pManager->GetHeap()) gfl2::renderingengine::renderer::DefaultRenderingPipeLine();

	{
		m_pDrawEnvNode = scenegraph::instance::InstanceCreator::CreateDrawEnvNode();
		m_pLightSetNode = scenegraph::instance::InstanceCreator::CreateLightSetNode();
		m_pDirectionalLightNode = scenegraph::instance::InstanceCreator::CreateLightNode();
		m_pAmbientLightNode = scenegraph::instance::InstanceCreator::CreateLightNode();

		{
			m_pDirectionalLightNode->SetNodeName( "DirectionalLight01" );
			gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(m_pDirectionalLightNode->GetNodeName()) );
			m_pDirectionalLightNode->SetNodeNameHash( nameHash.value );
			m_pDirectionalLightNode->SetLightType( LightType::Directional );
			gfl2::math::Vector3 position = gfl2::math::Vector3(0.0f, 10000000.0f, 10000000.0f);
			m_pDirectionalLightNode->SetPosition( position );
			gfl2::math::Vector3 direction = -position.Normalize();
			//direction.w = 0.0f;
			m_pDirectionalLightNode->SetDirection( direction );
			m_pDirectionalLightNode->SetColor( gfl2::math::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
		}

		{
			m_pAmbientLightNode->SetNodeName( "AmbientLight01" );
			gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(m_pAmbientLightNode->GetNodeName()) );
			m_pAmbientLightNode->SetNodeNameHash( nameHash.value );
			m_pAmbientLightNode->SetLightType( LightType::Ambient );
			m_pAmbientLightNode->SetColor( gfl2::math::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
		}

		m_pLightSetNode->SetSetNo( 0 );
		m_pLightSetNode->AddLight( m_pDirectionalLightNode );//LightSetNodeの子となる
		m_pLightSetNode->AddLight( m_pAmbientLightNode );//LightSetNodeの子となる
		m_pDrawEnvNode->AddLightSet( m_pLightSetNode );//DrawEnvNodeの子となる

		gfl2::math::AABB				envSpace( gfl2::math::Vector4(-500000.0f, -500000.0f, -500000.0f), gfl2::math::Vector4(500000.0f, 500000.0f, 500000.0f) );
		m_pDrawEnvNode->SetAABB( envSpace );
		scenegraph::SceneGraphManager::AddChild( m_pDrawEnvNode );

		m_pRenderingPipeLine->AddDrawEnv( m_pDrawEnvNode );
	}

	{
		//リソースローダー経由でノードは生成されます。
		scenegraph::resource::GfBinaryMdlData							resourceData;

//		s32		size;
#if defined(GF_PLATFORM_CTR)
		c8		*pBinData = (c8*)LocalFunc::ReadFile( "R:/home/momiji/exec/data_ctr/data/test/ishiguro_test/Kusa/f1.ctr.gfbmdl" );
		//c8		*pBinData = (c8*)LocalFunc::ReadFile( "R:/home/momiji/workspace/04_field/graphic/ibe_mana/exports/f1.ctr.gfbmdl" );
#else
		c8		*pBinData = (c8*)LocalFunc::ReadFile( "R:/home/momiji/exec/data_win32/data/test/ishiguro_test/Kusa/f1.wingl.gfbmdl" );			
#endif
	
		resourceData.SetModelData( pBinData );

		m_pBinModelRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );	
	}

	{
#if defined(GF_PLATFORM_CTR)
		static const c8* pShaderFileList[] = {
			"R:/home/momiji/exec/data_ctr/data/test/ishiguro_test/Kusa/Shaders/Kusa.ctr.gfbvsh",
			//"R:/home/momiji/workspace/04_field/graphic/ibe_mana/exports/Shaders/Kusa.ctr.gfbvsh",
			"R:/home/momiji/exec/data_ctr/data/test/ishiguro_test/Kusa/Shaders/KusaTest.ctr.gfbfsh",
		};
#else
		static const c8* pShaderFileList[] = {
			"R:/home/momiji/exec/data_win32/data/test/ishiguro_test/Kusa/Shaders/Kusa.wingl.gfbvsh",
			"R:/home/momiji/exec/data_win32/data/test/ishiguro_test/Kusa/Shaders/KusaTest.wingl.gfbfsh",
		};
#endif
		u32				shaderNum = sizeof(pShaderFileList) / sizeof(pShaderFileList[0]);

		for( u32 shaderNo = 0; shaderNo < shaderNum; ++shaderNo )
		{
			scenegraph::resource::GfBinaryShdData							resourceData;

//			s32		size;
			c8		*pBinData = (c8*)LocalFunc::ReadFile( pShaderFileList[shaderNo] );

			resourceData.SetShaderData( pBinData );

			scenegraph::resource::ResourceNode		*pBinShdNode = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
			m_pBinModelRootNode->AddChild( pBinShdNode );
		}
	}

	{

#if defined(GF_PLATFORM_CTR)
		static const c8*			pTextureFileList[] = {
			"R:/home/momiji/exec/data_ctr/data/test/ishiguro_test/Kusa/Textures/land01_encgrass_01.ctr.btex",
			"R:/home/momiji/exec/data_ctr/data/test/ishiguro_test/Kusa/Textures/land01_encgrass_02.ctr.btex",
		};
#else
		static const c8*			pTextureFileList[] = {
			"R:/home/momiji/exec/data_win32/data/test/ishiguro_test/Kusa/Textures/land01_encgrass_01.wingl.btex",
			"R:/home/momiji/exec/data_win32/data/test/ishiguro_test/Kusa/Textures/land01_encgrass_02.wingl.btex",
		};
#endif
		u32				texNum = sizeof(pTextureFileList) / sizeof(pTextureFileList[0]);

		for( u32 texNo = 0; texNo < texNum; ++texNo )
		{
			scenegraph::resource::GfBinaryTexData							texResourceData;
			scenegraph::resource::ResourceNode								*pBinTexRootNode;

//			s32		texsize;
//			s32		size;
			c8		*pTexBinData = (c8*)LocalFunc::ReadFile( pTextureFileList[texNo] );
			
			texResourceData.SetTextureData( pTexBinData );

			//m_pBinModelRootNodeには、様々なリソースがぶら下がっています。
			pBinTexRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &texResourceData );
			m_pBinModelRootNode->AddChild( pBinTexRootNode );
		}
	}

	m_pFieldKusaModel = Field::KusaModel::FieldKusaModel::CreateKusaModelInstanceNode( GLMemory::GetAllocator(), m_pBinModelRootNode );
	m_pFieldKusaModel->InitKusaShader( GLMemory::GetAllocator() );

	m_Cnt = 0;
		
	result = gfl2::proc::RES_FINISH;

	GFL_PRINT("IshiguroTestProc->InitFunc\n");
#endif
	return result;
}


/**
* @brief	  PROCのメイン更新を行う関数
* @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
*/
gfl2::proc::Result IshiguroTestProc::UpdateFunc( gfl2::proc::Manager* pManager )
{	
#if 0	
	GFL_PRINT("IshiguroTestProc->UpdateFunc\n");

	gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModel();

#if 0
	m_Cnt %= Field::KusaModel::FieldKusaModel::GridTotalNum;
	s32			x = m_Cnt % Field::KusaModel::FieldKusaModel::GrassXMax;
	s32			y = m_Cnt / Field::KusaModel::FieldKusaModel::GrassXMax;

	m_pFieldKusaModel->SetShapeDrawFlag( x, y, false );
	++m_Cnt;
#else

	m_TestPos.x += 2.0f;
	m_TestPos.z += 2.0f;

	Field::KusaModel::FieldKusaModel::KusaGrid			grid = m_pFieldKusaModel->GetKusaGrid( m_TestPos );
	m_pFieldKusaModel->SetShapeDrawFlag( grid, false );

	gfl2::math::Vector3		kusaPos = m_pFieldKusaModel->GetKusaWorldPos( grid );

	GFL_PRINT("kusa pos( %f, %f, %f)\n", kusaPos.x, kusaPos.y, kusaPos.z );

#endif
#endif
	return gfl2::proc::RES_CONTINUE;
}

/**
* @brief	PROCの描画処理(下準備)
* @param pManager   プロセス管理マネージャー
* 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
* 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
*/
void IshiguroTestProc::PreDrawFunc( gfl2::proc::Manager* pManager )
{
	GFL_PRINT("IshiguroTestProc->PreDrawFunc\n");
}

/**
* @brief	PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void IshiguroTestProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{	
#if 0	
	gfl2::gfx::DisplayDeviceHandle displayHandle = System::GflUse::m_HandleList[displayNo];

  u32 displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth(displayHandle);
  u32 displayHeight = gfl2::gfx::GFGL::GetDisplayHeight(displayHandle);
	Surface		*pBackBuffer = GFGL::GetBackBuffer( displayHandle );
  Surface		*pDepthBuffer = GFGL::GetDepthStencilBuffer( displayHandle );

  // 射影変換行列(パースペクティブ(透視法)射影)
  gfl2::math::Matrix44 projMatrix = gfl2::math::Matrix44::GetPerspective(
      gfl2::math::ConvDegToRad(37.0f),		// 視野角30°
      (float)displayWidth / (float)displayHeight,	// アスペクト比
      1.1f,							// 前方投影面までの距離
      10000.0f,							// 後方投影面までの距離
      true);                // 画面の向きに合わせる

  m_pRenderingPipeLine->SetProjectionMatrix(projMatrix);

  // ビュー変換行列
  static gfl2::math::Vector3 eyePosition(500.0f, 2000.0f, 510.0f);  // 視点(カメラの位置)
  static gfl2::math::Vector3 focusPosition(500.0f, 0.0f,  500.0f);  // 注視点
  gfl2::math::Vector3 upDirection(0.0f, 1.0f,  0.0f);     // カメラの上方向

  gfl2::math::Matrix34 viewMatrix = gfl2::math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);

  m_pRenderingPipeLine->SetViewMatrix(viewMatrix);


  // 描画
	m_pRenderingPipeLine->SetRenderTarget( pBackBuffer );
	m_pRenderingPipeLine->SetDepthStencil( pDepthBuffer );
  m_pRenderingPipeLine->SetDisplaySize(displayWidth, displayHeight);
  m_pRenderingPipeLine->Execute();
  #endif
}

/**
* @brief   PROCのメインを行う関数
*  1/30フレームの②フレーム目の処理です。
*  1/60フレーム動作時には、使われません。
* @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
*/
gfl2::proc::Result IshiguroTestProc::NextFrameMainFunc(gfl2::proc::Manager* pManager)
{
	GFL_PRINT("IshiguroTestProc->NextFrameMainFunc\n");

	return gfl2::proc::RES_CONTINUE;	
}

/**
* @brief   PROCの終了を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
*/
gfl2::proc::Result IshiguroTestProc::EndFunc(gfl2::proc::Manager* pManager)
{	
	GFL_PRINT("IshiguroTestProc->EndFunc\n");

	return gfl2::proc::RES_FINISH;
}

GFL_NAMESPACE_END(IshiguroTest)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
