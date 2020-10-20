//=================================================================================================
/**
 @file SkeltonFieldMapBlock.cpp
 @brief
 @date 2014.02.17
 @author tamada@gamefreak.co.jp

 */
//=================================================================================================
#if defined(GF_PLATFORM_WIN32)
#include "Test/Skelton/include/Field/SkeltonFieldMapBlock.h"
#include <debug/include/gfl2_DebugPrint.h>
#include "Test/Skelton/include/SkeltonDebugInfo.h"

//#include "field/field_util.h"

//#include "field/field_renderer.h"
#include "Test/Skelton/include/SkeltonRscManager.h"

//#include "gfgl_use.h"

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>
//#include <field/field_rendering_pipeline.h>

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)


//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
SkeltonFieldMapBlock::SkeltonFieldMapBlock( gfl2::heap::HeapBase * p_heap, System::nijiAllocator* p_allocator ):
	mHeap(p_heap),
  mAllocator(p_allocator),
  m_mapResource(NULL),
  m_instance(NULL),
  m_pMapAnimationResource( NULL ),
  m_filepath(NULL),
	m_isExists(false),
  m_VisibleMode( 0 ),
  m_pCollisionResource( NULL ),
  m_pCollisionModelResource( NULL ),
  m_pCameraCollisionResource( NULL ),
  m_pJumpCollisionResource( NULL ),
  m_pCameraControlCollisionResource(),
  m_pCollisioModelInstance( NULL ),
  m_pCameraCollisionModelResource( NULL ),
	m_pCameraCollisioModelInstance( NULL ),
  m_pJumpCollisionModelResource( NULL ),
	m_pJumpCollisioModelInstance( NULL ),
  m_pCameraControlCollisionModelResource(),
	m_pCameraControlCollisioModelInstance()
{
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
SkeltonFieldMapBlock::~SkeltonFieldMapBlock()
{

}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
bool SkeltonFieldMapBlock::activate(graphics* pGSys)
{
  return true;
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
bool SkeltonFieldMapBlock::Setup( SkeltonRscManager * p_rsc_man, const struct MAP_BLOCK_PARAM & p_param, System::ModelRenderPath* pModelPath )
{
  wcscpy_s( m_RootPath, 1024, SkeltonDebugInfo::m_MapRootPath );

  c16 pathWork[1024];
  wcscpy_s( pathWork, 1024, m_RootPath );
  c16 pathWork2[1024];
  wcscpy_s( pathWork2, 1024, m_RootPath );


  u32 rootPathLength = wcslen( m_RootPath );

  pathWork[ rootPathLength ] = L'\0';
  wcscat_s( pathWork, 1024, L"\\map.gfmdl" );

  c16 outputPathWork[1024];
  wcscpy_s( outputPathWork, 1024, m_RootPath );

  if ( p_rsc_man->HasResource( pathWork ) == false )
  {
    bool result = p_rsc_man->LoadModelResource( pathWork );
    if ( !result ) return false;
  }
  m_filepath = p_param.filename;
  m_mapResource = p_rsc_man->GetResourceNode( pathWork );
  m_instance = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( mAllocator, m_mapResource );

  if( m_instance )
  {
    // @todo アルファ抜きで表示がおかしくなる、対応するまで無効化
    pModelPath->AddNode( m_instance );
    // pGraphics->GetGfglUse()->GetRenderPipeLine()->AddEdgeRenderingTarget( m_instance );

    pathWork[ rootPathLength ] = L'\0';
    wcscat_s( pathWork, 1024, L"\\map.gfmot" );
    if ( p_rsc_man->HasResource( pathWork ) == false )
    {
      bool result = p_rsc_man->LoadMotionResource( pathWork );
    }

    m_pMapAnimationResource = p_rsc_man->GetResourceNode( pathWork );
    if( m_pMapAnimationResource )
    {
      // アニメーションコントローラ初期化
      m_AnimationController.Initialize( mHeap, m_instance );

      // 待機モーションを再生しておく
      gfl2::animation::MaterialAnimationSlot* pMaterialAnimationSlot = m_AnimationController.GetMaterialSlot();
      if( pMaterialAnimationSlot )
      {
        pMaterialAnimationSlot->SetAnimationResource( m_pMapAnimationResource );
        pMaterialAnimationSlot->SetLoop( true );
        pMaterialAnimationSlot->SetStepFrame( 1.0f );
      }

      gfl2::animation::JointAnimationSlot* pJointAnimationSlot = m_AnimationController.GetJointSlot();
      if( pJointAnimationSlot )
      {
        pJointAnimationSlot->SetAnimationResource( m_pMapAnimationResource );
        pJointAnimationSlot->SetLoop( true );
        pJointAnimationSlot->SetStepFrame( 1.0f );
      }
    }
  }

  m_transform.Clear();
	m_transform.SetTranslation( p_param.pos );
	m_transform.SetScale( gfl2::math::Vector4( MAP_SIZE_RATE, MAP_SIZE_RATE, MAP_SIZE_RATE ) );

  // コリジョンモデル表示
  pathWork[ rootPathLength ] = L'\0';
  wcscat_s( pathWork, 1024, L"\\map_takasa.gfmdl" );


  if ( p_rsc_man->HasResource( pathWork ) == false )
  {
    bool result = p_rsc_man->LoadModelResource( pathWork );
  }

  m_pCollisionModelResource = p_rsc_man->GetResourceNode( pathWork );
  if( m_pCollisionModelResource )
  {
    m_pCollisioModelInstance = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( mAllocator, m_pCollisionModelResource );
    m_pCollisioModelInstance->SetVisible( false );
  }

  // コリジョン
  if( m_pCollisionModelResource )
  {
    outputPathWork[ rootPathLength ] = L'\0';
    wcscat_s( outputPathWork, 1024, L"\\CollisionModelCache\\map_takasa.gfcol" );
    ::std::string outputPath( gfl2::renderingengine::scenegraph::resource::ParserUtil::UTF16toShitJIS( outputPathWork));

    gfl2::gfx::GLMemory::SetInstanceAllocator( mAllocator);
    gfl2::renderingengine::scenegraph::resource::GfCollisionMdlData resourceData;
    gfl2::gfx::GLMemory::ClearInstanceAllocator();
    resourceData.ReadFile( gfl2::renderingengine::scenegraph::resource::ParserUtil::UTF16toShitJIS( pathWork ), outputPath );
    m_pCollisionResource = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( mAllocator, &resourceData );
    m_CollisionModel.Initialize( m_pCollisionResource );
  }


  // カメラコリジョンモデル表示
  pathWork[ rootPathLength ] = L'\0';
  wcscat_s( pathWork, 1024, L"\\map_camera.gfmdl" );
  pathWork2[ rootPathLength ] = L'\0';
  wcscat_s( pathWork2, 1024, L"\\camera_data.xml" );
  if ( p_rsc_man->HasResource( pathWork ) == false )
  {
    bool result = p_rsc_man->LoadModelResource( pathWork );
  }

  m_pCameraCollisionModelResource = p_rsc_man->GetResourceNode( pathWork );
  if( m_pCameraCollisionModelResource )
  {
    m_pCameraCollisioModelInstance = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( mAllocator, m_pCameraCollisionModelResource );
    m_pCameraCollisioModelInstance->SetVisible( false );
  }

  // カメラコリジョン
  if( m_pCameraCollisionModelResource )
  {
    outputPathWork[ rootPathLength ] = L'\0';
    wcscat_s( outputPathWork, 1024, L"\\CollisionModelCache\\map_camera.gfcol" );
    ::std::string outputPath( gfl2::renderingengine::scenegraph::resource::ParserUtil::UTF16toShitJIS( outputPathWork));

    gfl2::gfx::GLMemory::SetInstanceAllocator( mAllocator);
    gfl2::renderingengine::scenegraph::resource::GfCollisionMdlData resourceData;
    gfl2::gfx::GLMemory::ClearInstanceAllocator();
    resourceData.ReadFileForSkelton( gfl2::renderingengine::scenegraph::resource::ParserUtil::UTF16toShitJIS( pathWork ), outputPath, gfl2::renderingengine::scenegraph::resource::ParserUtil::UTF16toShitJIS( pathWork2 ));
    m_pCameraCollisionResource = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( mAllocator, &resourceData );
    m_CameraCollisionModel.Initialize( m_pCameraCollisionResource );
  }


  // ジャンプコリジョンモデル表示
  pathWork[ rootPathLength ] = L'\0';
  wcscat_s( pathWork, 1024, L"\\map_jump.gfmdl" );
    pathWork2[ rootPathLength ] = L'\0';
  wcscat_s( pathWork2, 1024, L"\\jump_data.xml" );
  if ( p_rsc_man->HasResource( pathWork ) == false )
  {
    bool result = p_rsc_man->LoadModelResource( pathWork );
  }

  m_pJumpCollisionModelResource = p_rsc_man->GetResourceNode( pathWork );
  if( m_pJumpCollisionModelResource )
  {
    m_pJumpCollisioModelInstance = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( mAllocator, m_pJumpCollisionModelResource );
    m_pJumpCollisioModelInstance->SetVisible( false );
  }
  
  // ジャンプコリジョン
  if( m_pJumpCollisionModelResource )
  {
    outputPathWork[ rootPathLength ] = L'\0';
    wcscat_s( outputPathWork, 1024, L"\\CollisionModelCache\\map_jump.gfcol" );
    ::std::string outputPath( gfl2::renderingengine::scenegraph::resource::ParserUtil::UTF16toShitJIS( outputPathWork));

    gfl2::gfx::GLMemory::SetInstanceAllocator( mAllocator);
    gfl2::renderingengine::scenegraph::resource::GfCollisionMdlData resourceData;
    gfl2::gfx::GLMemory::ClearInstanceAllocator();
    resourceData.ReadFileForSkelton( gfl2::renderingengine::scenegraph::resource::ParserUtil::UTF16toShitJIS( pathWork ), outputPath, gfl2::renderingengine::scenegraph::resource::ParserUtil::UTF16toShitJIS( pathWork2 ));
    m_pJumpCollisionResource = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( mAllocator, &resourceData );
    m_JumpCollisionModel.Initialize( m_pJumpCollisionResource );
  }


  // カメラ制御用コリジョンモデル表示
  const c16* camera_control_model_path[ CAMERA_CONTROL_COLLISION_MAX ] =
  {
    L"\\map_camera_collision_00.gfmdl",
    L"\\map_camera_collision_01.gfmdl",
  };
  const c16* camera_control_collision_output_path[ CAMERA_CONTROL_COLLISION_MAX ] =
  {
    L"\\map_camera_collision_00.gfcol",
    L"\\map_camera_collision_01.gfcol",
  };

  for( int i = 0; i < CAMERA_CONTROL_COLLISION_MAX; ++i)
  {
    pathWork[ rootPathLength ] = L'\0';
    wcscat_s( pathWork, 1024, camera_control_model_path[i] );
    if ( p_rsc_man->HasResource( pathWork ) == false )
    {
      bool result = p_rsc_man->LoadModelResource( pathWork );
    }

    m_pCameraControlCollisionModelResource[i] = p_rsc_man->GetResourceNode( pathWork );
    if( m_pCameraControlCollisionModelResource[i] )
    {
      m_pCameraControlCollisioModelInstance[i] = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( mAllocator, m_pCameraControlCollisionModelResource[i] );
      m_pCameraControlCollisioModelInstance[i]->SetVisible( false );
    }

    // カメラ制御用コリジョン
    if( m_pCameraControlCollisionModelResource[i] )
    {
      outputPathWork[ rootPathLength ] = L'\0';
      wcscat_s( outputPathWork, 1024, camera_control_collision_output_path[i] );
      ::std::string outputPath( gfl2::renderingengine::scenegraph::resource::ParserUtil::UTF16toShitJIS( outputPathWork));

      gfl2::gfx::GLMemory::SetInstanceAllocator( mAllocator);
      gfl2::renderingengine::scenegraph::resource::GfCollisionMdlData resourceData;
      gfl2::gfx::GLMemory::ClearInstanceAllocator();
      resourceData.ReadFile( gfl2::renderingengine::scenegraph::resource::ParserUtil::UTF16toShitJIS( pathWork ), outputPath );
      m_pCameraControlCollisionResource[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( mAllocator, &resourceData );
      m_CameraControlCollisionModel[i].Initialize( m_pCameraControlCollisionResource[i] );
    }
  }

  if( m_pCollisioModelInstance )
  {
    pModelPath->AddNode( m_pCollisioModelInstance );
  }

  if( m_pCameraCollisioModelInstance )
  {
    pModelPath->AddNode( m_pCameraCollisioModelInstance );
  }

  if( m_pJumpCollisioModelInstance )
  {
    pModelPath->AddNode( m_pCameraCollisioModelInstance );
  }

	m_isExists = true;
  UpdateVisible();
	return true;
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
void SkeltonFieldMapBlock::Clear( SkeltonRscManager * p_rsc_man, System::ModelRenderPath* pModelPath )
{
  if( m_isExists == false )
  {
    return;
  }

  if( m_pCollisioModelInstance )
  {
    pModelPath->RemoveNode( m_pCollisioModelInstance );
  }

  if( m_pCameraCollisioModelInstance )
  {
    pModelPath->RemoveNode( m_pCameraCollisioModelInstance );
  }

  if( m_pJumpCollisioModelInstance )
  {
    pModelPath->RemoveNode( m_pCameraCollisioModelInstance );
  }

  m_AnimationController.Finalize();

  c16 pathWork[1024];
  wcscpy_s( pathWork, 1024, m_RootPath );
  u32 rootPathLength = wcslen( m_RootPath );

  const c16* camera_control_model_path[ CAMERA_CONTROL_COLLISION_MAX ] =
  {
    L"\\map_camera_collision_00.gfmdl",
    L"\\map_camera_collision_01.gfmdl",
  };
  for( int i = 0; i < CAMERA_CONTROL_COLLISION_MAX; ++i)
  {
    GFL_SAFE_DELETE( m_pCameraControlCollisioModelInstance[i] );
    GFL_SAFE_DELETE( m_pCameraControlCollisionResource[i] );

    if( m_pCameraControlCollisionModelResource[i] )
    {
      pathWork[ rootPathLength ] = L'\0';
      wcscat_s( pathWork, 1024, camera_control_model_path[i] );

      p_rsc_man->UnloadResource( pathWork );
      m_pCameraControlCollisionModelResource[i] = NULL;
    }
  }

  GFL_SAFE_DELETE( m_pJumpCollisioModelInstance );
  GFL_SAFE_DELETE( m_pJumpCollisionResource );

  if( m_pJumpCollisionModelResource )
  {
    pathWork[ rootPathLength ] = L'\0';
    wcscat_s( pathWork, 1024, L"\\map_jump.gfmdl" );

    p_rsc_man->UnloadResource( pathWork );
    m_pJumpCollisionModelResource = NULL;
  }

  GFL_SAFE_DELETE( m_pCameraCollisioModelInstance );
  GFL_SAFE_DELETE( m_pCameraCollisionResource );

  if( m_pCameraCollisionModelResource )
  {
    pathWork[ rootPathLength ] = L'\0';
    wcscat_s( pathWork, 1024, L"\\map_camera.gfmdl" );

    p_rsc_man->UnloadResource( pathWork );
    m_pCameraCollisionModelResource = NULL;
  }

  GFL_SAFE_DELETE( m_pCollisioModelInstance );
  GFL_SAFE_DELETE( m_pCollisionResource );
  if( m_pCollisionModelResource )
  {
    pathWork[ rootPathLength ] = L'\0';
    wcscat_s( pathWork, 1024, L"\\map_takasa.gfmdl" );

    p_rsc_man->UnloadResource( pathWork );
    m_pCollisionModelResource = NULL;
  }

  pathWork[ rootPathLength ] = L'\0';
  wcscat_s( pathWork, 1024, L"\\map.gfmdl" );
  if( m_instance )
  {
    // @todo アルファ抜きで表示がおかしくなる、対応するまで無効化
    // pGraphics->GetGfglUse()->GetRenderPipeLine()->RemoveEdgeRenderingTarget( m_instance );
    pModelPath->RemoveNode( m_instance );
    GFL_SAFE_DELETE( m_instance );

  }
  p_rsc_man->UnloadResource( pathWork );
  m_filepath = NULL;
  m_mapResource = NULL;
  m_instance = NULL;

	m_isExists = false;
}

//-----------------------------------------------------------------------------------------------
//　ワールド行列を設定
//-----------------------------------------------------------------------------------------------
 void SkeltonFieldMapBlock::Update( void )
{
	if (m_isExists == false ) return;

	m_transform.Update();
  m_instance->SetLocalSRT(m_transform.GetSRT());

  // @todo アニメーションが無いときにハングしてしまうので
  if( m_pMapAnimationResource )
  {
    m_AnimationController.Update();
  }
}

 void SkeltonFieldMapBlock::VisibleModeCountUp( void )
 {
   if( m_VisibleMode == 3 )
   {
     m_VisibleMode = 0;
   }
   else
   {
      m_VisibleMode++;
   }

   UpdateVisible();
 }

 void SkeltonFieldMapBlock::UpdateVisible( void )
 {
   if( m_instance )
   {
     m_instance->SetVisible( (m_VisibleMode == 0 || m_VisibleMode == 4) );
   }

   if( m_pCollisioModelInstance )
   {
     m_pCollisioModelInstance->SetVisible( ( m_VisibleMode == 1 ) );
   }

   if( m_pCameraCollisioModelInstance )
   {
     m_pCameraCollisioModelInstance->SetVisible( ( m_VisibleMode == 2 ) );
   }

   if( m_pJumpCollisioModelInstance )
   {
     m_pJumpCollisioModelInstance->SetVisible( ( m_VisibleMode == 3 ) );
   }

   SkeltonDebugInfo::m_MapjumpCollisoinDrawFlag = ( m_VisibleMode == 4 );

   for( int i = 0; i < CAMERA_CONTROL_COLLISION_MAX; ++i)
   {
     if( m_pCameraControlCollisioModelInstance[i])
     {
       m_pCameraControlCollisioModelInstance[i]->SetVisible( ( m_VisibleMode == (5+i) ) );
     }
     if( m_instance )
     {
       if( m_VisibleMode == 5+i)
       {
         m_instance->SetVisible( true);
       }
     }
   }

   GFL_PRINT( "%d : Visible Mode SkeltonFieldMapBlock\n", m_VisibleMode);
 }

 


//=================================================================================================
//=================================================================================================

	
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
SkeltonFieldMapBlockManager::SkeltonFieldMapBlockManager( gfl2::heap::HeapBase * p_heap, System::nijiAllocator* p_allocator ) :
	mHeap(p_heap), mAllocator( p_allocator )
{
	for (int i = 0; i < MAP_BLOCK_MAX; ++ i )
	{
		m_SkeltonFieldMapBlock[i] = GFL_NEW( mHeap ) SkeltonFieldMapBlock(mHeap,mAllocator);
	}
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
SkeltonFieldMapBlockManager::~SkeltonFieldMapBlockManager()
{
	for (int i = 0; i < MAP_BLOCK_MAX; ++i )
	{
		GFL_DELETE( m_SkeltonFieldMapBlock[i] );
	}
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
void SkeltonFieldMapBlockManager::Initialize( graphics * p_graphics, FieldRenderer * p_renderer, SkeltonRscManager * p_rsc_man, System::ModelRenderPath* pModelPath )
{
  m_pModelPath = pModelPath;
	m_graphics = p_graphics;
	m_renderer = p_renderer;
  m_rsc_man  = p_rsc_man;

	for (int i = 0; i < MAP_BLOCK_MAX; ++ i )
	{
		m_SkeltonFieldMapBlock[i]->activate(p_graphics);
	}
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
void SkeltonFieldMapBlockManager::getParameter( unsigned int file_id, MAP_BLOCK_PARAM & para )
{
  // @todo このパラメータは現在参照されていない
  const wchar_t * const fileName[1] = {
		L"..\\resource\\niji_test_map\\niji_test_map01.gfmdl",
  };
  const float positions[1][3] = {
    { 0.0f, 0.0f/*-70.0f*/, 0.0f },
  };

	if( GFL_NELEMS(fileName) <= file_id ){ file_id = 0; }

	para.filename = fileName[file_id];
	para.pos = gfl2::math::Vector4( positions[file_id][0] * MAP_SIZE_RATE, positions[file_id][1] * MAP_SIZE_RATE, positions[file_id][2] * MAP_SIZE_RATE, 1.0f );
}
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
bool SkeltonFieldMapBlockManager::SetupSampleMaps( void )
{
//  return true;
	MAP_BLOCK_PARAM para;

	for (int i = 0; i < MAP_BLOCK_MAX; ++ i )
	{
		getParameter( i, para );
		if ( createModel( i, para ) == false )
		{
			return false;
		}
	}
	return true;
}

bool SkeltonFieldMapBlockManager::ReloadReq( void )
{
  for (int i = 0; i < MAP_BLOCK_MAX; ++ i )
  {
    if( m_SkeltonFieldMapBlock[i]->GetInstance() )
    {
      m_pModelPath->RemoveNode( m_SkeltonFieldMapBlock[i]->GetInstance() );
    }
  }
  return true;
}

bool SkeltonFieldMapBlockManager::IsReloadOk( void ) const
{
  b32 resultFlag( true );

  for (int i = 0; i < MAP_BLOCK_MAX; ++ i )
  {
    if( m_SkeltonFieldMapBlock[i]->GetInstance() )
    {
      if(  m_SkeltonFieldMapBlock[i]->GetInstance()->GetReferenceCnt() != 0 )
      {
        resultFlag = false;
      }
    }
  }

  return resultFlag;
}


//! 仮モデルを読み込みなおす
bool SkeltonFieldMapBlockManager::ReloadSampleMaps( void )
{
  // まず全削除
  DeleteAllModels();

  // 生成
  return SetupSampleMaps();
}

SkeltonFieldMapBlock* SkeltonFieldMapBlockManager::GetPrimary( void )
{
  return m_SkeltonFieldMapBlock[0];
}
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
bool SkeltonFieldMapBlockManager::createModel( u32 index, MAP_BLOCK_PARAM & para  )
{
  if ( m_SkeltonFieldMapBlock[index]->Setup(m_rsc_man,para,m_pModelPath ) == false )
	{
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
void SkeltonFieldMapBlockManager::deleteModel( u32 index )
{
  m_SkeltonFieldMapBlock[index]->Clear( m_rsc_man,m_pModelPath);
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
void SkeltonFieldMapBlockManager::Terminate( void )
{
	DeleteAllModels();
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
void SkeltonFieldMapBlockManager::UpdateAllModels( f32 timediff )
{
	for (int i = 0; i < MAP_BLOCK_MAX; ++i)
	{
		m_SkeltonFieldMapBlock[i]->Update();
	}

}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
void SkeltonFieldMapBlockManager::DrawAllModels( void )
{
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
void SkeltonFieldMapBlockManager::DeleteAllModels( void )
{
	for (int i = 0; i < MAP_BLOCK_MAX; ++ i )
	{
		deleteModel( i );
	}
}

GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_WIN32