#if defined(GF_PLATFORM_WIN32)
//=================================================================================================
/**
*	@file			SkeltonModel_manager.cpp
*	@brief
*	@author		tamada@gamefreak.co.jp
*	@date			2014.03.11
*/
//=================================================================================================


#include "Test/Skelton/include/Field/SkeltonModelManager.h"
#include "Test/Skelton/include/Field/SkeltonModel.h"

//#include "gfgl_use.h"
//#include "field/field_renderer.h"
#include "Test/Skelton/include/Field/SkeltonFieldTransform.h"

#include "Test/Skelton/include/SkeltonRscManager.h"
#include "Test/Skelton/include/Field/SkeltonCodePlayer.h"

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

#include "Shlwapi.h"
#include "Test/Skelton/include/SkeltonDebugInfo.h"

GFL_NAMESPACE_BEGIN(Test)
  GFL_NAMESPACE_BEGIN(Skelton)


  //=================================================================================================
  //=================================================================================================
  //-------------------------------------------------------------------------------------------------
  //-------------------------------------------------------------------------------------------------
  //	float SkeltonModelManager::SizeRate = 0.3f;
  float SkeltonModelManager::SizeRate = 1.0f;

float SkeltonModelManager::ChangeSizeRate( float change )
{
  if ( change > 0 && SizeRate + change < 0.4f )
  {
    SizeRate += change;
  }
  else if ( SizeRate + change > 0.01f )
  {
    SizeRate += change;
  }
  return SizeRate;		
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
const SkeltonCodeBase * SkeltonModelManager::GetSkeltonCode( SKELTON_CODE code )
{
  const SkeltonCodeBase * SKELTON_CODE = m_SkeltonCodeTable[static_cast<u32>(code)];
  if ( SKELTON_CODE ) return SKELTON_CODE;
  return m_SkeltonCodeTable[0];	//SKELTON_CODE_NONE
}

const SkeltonCodeBase * SkeltonModelManager::m_SkeltonCodeTable[ SKELTON_CODE_NUM ] ={};

//=================================================================================================
//=================================================================================================
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
SkeltonModelManager::SkeltonModelManager( gfl2::heap::HeapBase * p_heap, System::nijiAllocator* p_Allocator ):
  mHeap(p_heap)
{
  for (int i = 0; i < SKELTON_CODE_NUM; ++ i )
  {
    m_SkeltonCodeTable[i] = NULL;
  }

  m_SkeltonCodeTable[SKELTON_CODE_NONE] = GFL_NEW( p_heap ) SkeltonCodePlayer(); // @todo
  m_SkeltonCodeTable[SKELTON_CODE_PLAYER] = GFL_NEW( p_heap ) SkeltonCodePlayer();
  //m_SkeltonCodeTable[SKELTON_CODE_NPC] = GFL_NEW( p_heap ) SkeltonCodePlayer();	//@todo 流用している
  //m_SkeltonCodeTable[SKELTON_CODE_STAY] = GFL_NEW( p_heap ) SkeltonCodeNone();
  //m_SkeltonCodeTable[SKELTON_CODE_FOLLOWER] = GFL_NEW( p_heap ) SkeltonCodeFollower();
  //m_SkeltonCodeTable[SKELTON_CODE_NPC_TEST] = GFL_NEW( p_heap ) SkeltonCodeNpcTest();


  for (int i = 0; i < SMDL_MAX; ++ i )
  {
    m_SkeltonModel[i] = GFL_NEW( p_heap ) SkeltonModel( p_heap, p_Allocator );
  }
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
SkeltonModelManager::~SkeltonModelManager()
{
  DeleteAllModels();

  for (int i = 0; i < SKELTON_CODE_NUM; ++ i )
  {
    GFL_SAFE_DELETE( m_SkeltonCodeTable[i] );
  }

  for (int i = 0; i < SMDL_MAX; ++ i )
  {
    GFL_SAFE_DELETE( m_SkeltonModel[i] );
  }
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool SkeltonModelManager::Initialize( graphics * p_graphics, FieldRenderer * p_renderer, SkeltonRscManager * p_rsc_man, System::ModelRenderPath* pModelPath )
{
  m_pGraphics = p_graphics;
  m_pRenderer = p_renderer;
  m_pRscMan   = p_rsc_man;
  m_pModelPath = pModelPath;

  for (int i = 0; i < SMDL_MAX; ++ i )
  {
    SkeltonModel * m_SMDL = m_SkeltonModel[i];
    m_SMDL->activate(this);
  }
  return true;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void SkeltonModelManager::Terminate( void )
{
  DeleteAllModels();

  m_pRscMan = NULL;
  m_pRenderer = NULL;
  m_pGraphics = NULL;
}
#if 0
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void SkeltonModelManager::UpdateAllModels( f32 timediff )
{
  for (int i = 0; i < SMDL_MAX; ++ i )
  {
    m_SkeltonModel[i]->update();
    m_SkeltonModel[i]->updateAnimation( timediff );
  }
}
#endif
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void SkeltonModelManager::UpdateAllModels( u32 frame )
{
  for (int i = 0; i < SMDL_MAX; ++ i )
  {
    m_SkeltonModel[i]->update();

    if( m_SkeltonModel[i]->GetID() == SMDL_ID::SMDL_PLAYER )
    {
      m_SkeltonModel[i]->UpdateAnimation( SkeltonDebugInfo::m_DebugInfoPlayerMotionStepFrame );
    }
    else
    {
      m_SkeltonModel[i]->UpdateAnimation( frame );

    }
    m_SkeltonModel[i]->updateAfter();
  }
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void SkeltonModelManager::SetModels( u32 npc_count, const SkeltonModelHeader * p_header )
{
  SkeltonModelHeader header;
  for ( u32 i = 0; i < npc_count; ++ i )
  {
    header = p_header[i];
    header.SMDL_id = static_cast<SMDL_ID>(i+1);	//@todo 適当
    SkeltonModel * result = CreateSkeltonModel( &header );
    if ( result == NULL ) OutputDebugString(L"動作モデル追加失敗！！！\n");
  }
}

void SkeltonModelManager::SetModelsEx( u32 npc_count, SkeltonModelHeaderEx * p_header )
{
  for ( u32 i = 0; i < npc_count; ++ i )
  {
    SkeltonModelHeaderEx* pHeader = &p_header[i];
    if( pHeader->enable == false )
    {
      continue;
    }
    SkeltonModel * result = CreateSkeltonModel( pHeader );
    if ( result == NULL ) OutputDebugString(L"動作モデル追加失敗！！！\n");
  }
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void SkeltonModelManager::DrawAllModels( void )
{
  for (int i = 0; i < SMDL_MAX; ++ i )
  {
    m_SkeltonModel[i]->draw();
  }
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void SkeltonModelManager::DeleteAllModels( void )
{
  for (int i = 0; i < SMDL_MAX; ++ i )
  {
    if ( m_SkeltonModel[i] )
    {
      deleteSkeltonModel( i );
    }
  }
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
SkeltonModel * SkeltonModelManager::GetSkeltonModel( SMDL_ID id ) const
{
  for ( int i = 0; i < SMDL_MAX; ++ i )
  {
    SkeltonModel * p_SMDL = m_SkeltonModel[i];
    if ( p_SMDL->GetExists() == true && p_SMDL->GetID() == id )
    {
      return p_SMDL;
    }
  }
  return NULL;
}

SkeltonModel * SkeltonModelManager::GetSkeltonModelByDataIndex( u32 dataIndex ) const
{
  return m_SkeltonModel[ dataIndex ];
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
SkeltonModel * SkeltonModelManager::CreateSkeltonModel( const SkeltonModelHeader * p_header )
{
  SkeltonModel * m_SMDL = NULL;
  for (int i = 0; i < SMDL_MAX; ++ i )
  {
    if ( m_SkeltonModel[i] != NULL && m_SkeltonModel[i]->GetExists() == false )
    {
      m_SMDL = m_SkeltonModel[i];
      break;
    }
  }
  if ( m_SMDL == NULL ) return NULL;

  if ( m_SMDL->Setup( m_pRscMan, p_header ) == false )
  {
    return NULL;
  }
  //m_SMDL->SetAnimation( chara::MOTION_ID_WAIT );
  m_SMDL->ChangeAnimation( chara::MOTION_ID_WAIT, true, 0.0f );

  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode * p_instance = m_SMDL->GetInstance();
  m_pModelPath->AddNode( p_instance );
  // m_pGraphics->GetGfglUse()->AddEdgeRenderObject( p_instance );

  return m_SMDL;
}

SkeltonModel * SkeltonModelManager::CreateSkeltonModel( SkeltonModelHeaderEx * pHeader )
{
  pHeader->resourcePathImpl = pHeader->resourcePath;
  // pHeader->SMDL_id = static_cast<SMDL_ID>(i+1);	//@todo 適当
  pHeader->modelResource.model_file_name = pHeader->resourcePathImpl.modelPath;

  for( u32 k = 0; k < chara::MOTION_ID_MAX; k++ )
  {
    pHeader->animaPathPtrTable[k] = pHeader->resourcePathImpl.animePathTable[k];
  }

  pHeader->modelResource.anime_file_table = pHeader->animaPathPtrTable;
  pHeader->modelResource.anime_max = chara::MOTION_ID_MAX;

  SkeltonModel * m_SMDL = NULL;
  for (int i = 0; i < SMDL_MAX; ++ i )
  {
    if ( m_SkeltonModel[i] != NULL && m_SkeltonModel[i]->GetExists() == false )
    {
      m_SMDL = m_SkeltonModel[i];
      break;
    }
  }
  if ( m_SMDL == NULL ) return NULL;

  if ( m_SMDL->Setup( m_pRscMan, pHeader ) == false )
  {
    return NULL;
  }
  //m_SMDL->SetAnimation( chara::MOTION_ID_WAIT );
  m_SMDL->ChangeAnimation( chara::MOTION_ID_WAIT, true, 0.0f );

  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode * p_instance = m_SMDL->GetInstance();
  m_pModelPath->AddNode( p_instance );
  // m_pGraphics->GetGfglUse()->AddEdgeRenderObject( p_instance );

  return m_SMDL;
}





//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
SkeltonModel * SkeltonModelManager::CreateSkeltonModel( SMDL_ID SMDL_id, chara::RSC_ID rsc_id )
{
  SkeltonModelHeader header = {
    SMDL_NONE,
    chara::RSCID_PLAYER_MALE,
    SKELTON_CODE_PLAYER,
    0,										// param0
    0,										// param1
    0,										// param2
    0.0f, 0.0f, 20.0f,		// x,y,z
  };
  header.SMDL_id = SMDL_id;
  header.rsc_id = rsc_id;
  return CreateSkeltonModel( &header );
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void SkeltonModelManager::DeleteSkeltonModel( SkeltonModel * p_model )
{
  for (int i = 0; i < SMDL_MAX; ++ i )
  {
    if ( m_SkeltonModel[i] == p_model )
    {
      deleteSkeltonModel( i );
      return;
    }
  }
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void SkeltonModelManager::deleteSkeltonModel( unsigned int index )
{
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode * p_instance = m_SkeltonModel[index]->GetInstance();
  m_SkeltonModel[index]->Clear();

  if( p_instance )
  {
    m_pModelPath->RemoveNode( p_instance );
    //GFL_ASSERT( m_pGraphics );
    //m_pGraphics->GetGfglUse()->RemoveEdgeRenderObject( p_instance );
  }
}



//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
SkeltonModel * SkeltonModelManager::GetFollowerModel( SMDL_ID id ) const
{
  for (int i = 0; i < SMDL_MAX; ++ i )
  {
    SkeltonModel * p_SMDL = m_SkeltonModel[i];
    if ( p_SMDL->GetExists() == true && p_SMDL->GetSkeltonCode() == SKELTON_CODE_FOLLOWER
      && static_cast<SMDL_ID>(p_SMDL->GetParam0()) == id )
    {
      return p_SMDL;
    }
  }
  return NULL;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
SkeltonModel * SkeltonModelManager::GetSkeltonModel( const SkeltonFieldRange &range ) const
{
  for (int i = 0; i < SMDL_MAX; ++ i )
  {
    SkeltonModel * p_SMDL = m_SkeltonModel[i];
    if ( p_SMDL->GetExists() == true && range.IsIn2D(p_SMDL->GetTransform()->GetTranslation()) == true )
    {
      return p_SMDL;
    }
  }
  return NULL;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
SkeltonModel * SkeltonModelManager::GetSkeltonModel( const CheckFunc &func ) const
{
  for (int i = 0; i < SMDL_MAX; ++ i )
  {
    SkeltonModel * p_SMDL = m_SkeltonModel[i];
    if ( p_SMDL->GetExists() == true && func(*p_SMDL) == true )
    {
      return p_SMDL;
    }
  }
  return NULL;
}

GFL_NAMESPACE_END(Skelton)
  GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_WIN32