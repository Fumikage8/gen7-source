#if defined(GF_PLATFORM_WIN32)

#pragma once
//=================================================================================================
/**
 @file SkeltonFieldMapBlock.h
 @brief
 @date 2014.02.17
 @author tamada@gamefreak.co.jp

 */
//=================================================================================================


#include <math/include/gfl2_Vector4.h>
#include <math/include/gfl2_Matrix.h>

#include "SkeltonFieldTransform.h"
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMotData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/Renderer/gfl2_DrawManager.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfCollisionMdlData.h>
#include <Collision/include/gfl2_CollisionModel.h>
#include <Animation/include/gfl2_AnimationController.h>
#include "System/include/RenderPath/ModelRenderPath.h"
#include "System/include/nijiAllocator.h"

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class graphics;
class SkeltonRscManager;
class FieldRenderer;
class SkeltonFieldTransform;

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

// static const float MAP_SIZE_RATE = 10/3.0f;
  static const float MAP_SIZE_RATE = 1.0f;
//=================================================================================================
//=================================================================================================
struct MAP_BLOCK_PARAM
{
	const wchar_t * filename;
	gfl2::math::Vector4 pos;
};

//=================================================================================================
/**
	@class SkeltonFieldMapBlock
	@brief	個別地形モデル保持クラス
 */
//=================================================================================================
class SkeltonFieldMapBlock
{
	GFL_FORBID_COPY_AND_ASSIGN(SkeltonFieldMapBlock);
	
  friend class SkeltonFieldMapBlockManager;

public:
	SkeltonFieldMapBlock( gfl2::heap::HeapBase * p_heap, System::nijiAllocator* p_allocator );

	~SkeltonFieldMapBlock();

	bool activate(graphics* pGSys);
	bool Setup( SkeltonRscManager * p_rsc_man, const struct MAP_BLOCK_PARAM & p_param, System::ModelRenderPath* pModelPath );
	void Clear( SkeltonRscManager * p_rsc_man, System::ModelRenderPath* pModelPath );
 	void Update( void );

  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode * GetInstance( void )
  {
    return m_instance;
  }

  // @note 注意、無いこともある
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode * GetCollisionModelInstance( void )
  {
    return m_pCollisioModelInstance;
  }
		

	SkeltonFieldTransform * GetTransform( void )
	{
		return &m_transform;
	}

  gfl2::collision::CollisionModel* GetCollision( void )
  {
    if( HasCollisionData() )
    {
      return &m_CollisionModel;
    }
    else
    {
      return NULL;
    }
  }

  gfl2::collision::CollisionModel* GetCameraCollision( void )
  {
    if( HasCameraCollisionData() )
    {
      return &m_CameraCollisionModel;
    }
    else
    {
      return NULL;
    }
  }

  gfl2::collision::CollisionModel* GetJumpCollision( void )
  {
    if( HasJumpCollisionData() )
    {
      return &m_JumpCollisionModel;
    }
    else
    {
      return NULL;
    }
  }

  gfl2::collision::CollisionModel* GetCameraControlCollision( u32 idx )
  {
    if( HasCameraControlCollisionData( idx) )
    {
      return &m_CameraControlCollisionModel[idx];
    }
    else
    {
      return NULL;
    }
  }

  b32 HasCollisionData( void )
  {
    return ( m_pCollisionResource != NULL );
  }

  b32 HasCameraCollisionData( void )
  {
    return ( m_pCameraCollisionResource != NULL );
  }

  b32 HasJumpCollisionData( void )
  {
    return ( m_pJumpCollisionResource != NULL );
  }

  b32 HasCameraControlCollisionData( u32 idx )
  {
    if( idx >= CAMERA_CONTROL_COLLISION_MAX)
    {
      return false;
    }
    return ( m_pCameraControlCollisionResource[idx] != NULL );
  }

  void VisibleModeCountUp( void );

  u32 GetVisibleMode( void ) const
  {
    return m_VisibleMode;
  }

private:

  void UpdateVisible( void );

  const static u32 CAMERA_CONTROL_COLLISION_MAX = 2;

	gfl2::heap::HeapBase * mHeap; 
  System::nijiAllocator   *mAllocator;
	gfl2::renderingengine::scenegraph::resource::ResourceNode * m_mapResource;
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode * m_instance;

	gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pMapAnimationResource;
  gfl2::animation::AnimationController                       m_AnimationController;

  const wchar_t * m_filepath;
  c16 m_RootPath[ 1024 ];

	SkeltonFieldTransform m_transform;
	bool m_isExists;
  u32 m_VisibleMode;

  // コリジョン情報
  gfl2::renderingengine::scenegraph::resource::ResourceNode*      m_pCollisionResource;
  gfl2::collision::CollisionModel                                 m_CollisionModel;

  // カメラ情報
  gfl2::renderingengine::scenegraph::resource::ResourceNode*      m_pCameraCollisionResource;
  gfl2::collision::CollisionModel                                 m_CameraCollisionModel;

  // マップジャンプモデル
  gfl2::renderingengine::scenegraph::resource::ResourceNode*      m_pJumpCollisionResource;
  gfl2::collision::CollisionModel                                 m_JumpCollisionModel;

  // カメラ移動制御用コリジョンモデル
  gfl2::renderingengine::scenegraph::resource::ResourceNode*      m_pCameraControlCollisionResource[ CAMERA_CONTROL_COLLISION_MAX ];
  gfl2::collision::CollisionModel                                 m_CameraControlCollisionModel[ CAMERA_CONTROL_COLLISION_MAX ];

  // コリジョン表示用情報
	gfl2::renderingengine::scenegraph::resource::ResourceNode*      m_pCollisionModelResource;
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* m_pCollisioModelInstance;
  
  // カメラコリジョン表示用情報
	gfl2::renderingengine::scenegraph::resource::ResourceNode*      m_pCameraCollisionModelResource;
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* m_pCameraCollisioModelInstance;

  // マップジャンプコリジョン表示用情報
	gfl2::renderingengine::scenegraph::resource::ResourceNode*      m_pJumpCollisionModelResource;
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* m_pJumpCollisioModelInstance;

  // カメラ制御用コリジョン表示用情報
	gfl2::renderingengine::scenegraph::resource::ResourceNode*      m_pCameraControlCollisionModelResource[ CAMERA_CONTROL_COLLISION_MAX ];
	gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* m_pCameraControlCollisioModelInstance[ CAMERA_CONTROL_COLLISION_MAX ];
};



//=================================================================================================
/**
	@class	SkeltonFieldMapBlockManager
	@brief	地形モデル全体クラス
 */
//=================================================================================================
class SkeltonFieldMapBlockManager
{
	GFL_FORBID_COPY_AND_ASSIGN(SkeltonFieldMapBlockManager);

public:
	SkeltonFieldMapBlockManager( gfl2::heap::HeapBase * p_heap, System::nijiAllocator* p_allocator );
	~SkeltonFieldMapBlockManager();

		void Initialize( graphics * p_graphics, FieldRenderer * p_renderer, SkeltonRscManager * p_rsc_man, System::ModelRenderPath* pModelPath );
		void Terminate( void );
		void UpdateAllModels( f32 timediff );
		void DrawAllModels( void );
		void DeleteAllModels( void );

		bool SetupSampleMaps( void );
        
        //! 仮モデルを読み込みなおす
        bool ReloadReq( void );
        bool IsReloadOk( void ) const;
        bool ReloadSampleMaps( void );

    SkeltonFieldMapBlock* GetPrimary( void );

private:
		void getParameter( unsigned int file_id, MAP_BLOCK_PARAM & para  );
		bool createModel( u32 index, MAP_BLOCK_PARAM & para  );
		void deleteModel( u32 index );

private:
	enum { MAP_BLOCK_MAX = 1 };

	gfl2::heap::HeapBase * mHeap;
  System::nijiAllocator* mAllocator;
	graphics * m_graphics;
	FieldRenderer * m_renderer;
  SkeltonRscManager * m_rsc_man;
	SkeltonFieldMapBlock * m_SkeltonFieldMapBlock[ MAP_BLOCK_MAX ];
  System::ModelRenderPath* m_pModelPath;
};


GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_WIN32