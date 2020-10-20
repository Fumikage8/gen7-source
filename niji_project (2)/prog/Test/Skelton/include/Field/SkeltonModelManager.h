#if defined(GF_PLATFORM_WIN32)
#pragma once
//=================================================================================================
/**
*	@file			SkeltonModel_manager.h
*	@brief
*	@author		tamada@gamefreak.co.jp
*	@date			2014.03.11
*/
//=================================================================================================

#include <heap/include/gfl2_heap_base.h>
#include "Test/Skelton/include/Field/SkeltonModelDefine.h"
#include "System/include/RenderPath/ModelRenderPath.h"
#include "System/include/nijiAllocator.h"

GFL_NAMESPACE_BEGIN(Test)
  GFL_NAMESPACE_BEGIN(Skelton)

class graphics;
class SkeltonRscManager;
class FieldRenderer;
class SkeltonFieldRange;

class SkeltonModel;
class SkeltonCodeBase;	

//-------------------------------------------------------------------------------------------------
/**
@class CheckFunc
@brief	モデル検出用インターフェイス
*/
//-------------------------------------------------------------------------------------------------
class CheckFunc
{
public:
  virtual bool operator()(const SkeltonModel & p_SMDL ) const = 0;
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

//=================================================================================================
//=================================================================================================
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class SkeltonModelManager
{
  GFL_FORBID_COPY_AND_ASSIGN(SkeltonModelManager);

public:
  SkeltonModelManager(gfl2::heap::HeapBase * p_heap, System::nijiAllocator* p_Allocator );
  ~SkeltonModelManager();

  bool Initialize( graphics * p_graphics, FieldRenderer * p_renderer, SkeltonRscManager * p_rsc_man, System::ModelRenderPath* pModelPath);
  void Terminate( void );

  void SetModels( u32 npc_count, const SkeltonModelHeader * p_header );
  void SetModelsEx( u32 npc_count, SkeltonModelHeaderEx * p_header );

  //		void UpdateAllModels( f32 timediff );
  void UpdateAllModels( u32 frame );

  void DrawAllModels( void );

  void DeleteAllModels( void );

  SkeltonModel * CreateSkeltonModel( SMDL_ID SMDL_id, chara::RSC_ID rsc_id );
  SkeltonModel * CreateSkeltonModel( const SkeltonModelHeader * p_header );
  SkeltonModel * CreateSkeltonModel( SkeltonModelHeaderEx * p_header );
  void DeleteSkeltonModel( SkeltonModel * p_model );

  SkeltonModel * GetSkeltonModel( SMDL_ID id ) const;

  SkeltonModel * GetFollowerModel( SMDL_ID id ) const;
  SkeltonModel * GetSkeltonModelByDataIndex( u32 dataIndex ) const;

  SkeltonModel * GetSkeltonModel( const SkeltonFieldRange &range ) const;
  SkeltonModel * GetSkeltonModel( const CheckFunc &func ) const;

  static const SkeltonCodeBase * GetSkeltonCode( SKELTON_CODE code );

  static float ChangeSizeRate( float change );
  static float GetSizeRate( void ){ return SizeRate; }
  static float GetSizeRate2( void ) { return 10/3.0f * SizeRate; }

private:
  void deleteSkeltonModel( unsigned int index );

private:
  static float SizeRate;

  gfl2::heap::HeapBase * mHeap;
  FieldRenderer * m_pRenderer;
  SkeltonRscManager * m_pRscMan;
  graphics * m_pGraphics;
  System::ModelRenderPath* m_pModelPath;
  SkeltonModel * m_SkeltonModel[ SMDL_MAX ];

  //@todo 管理方法変更
  static const SkeltonCodeBase * m_SkeltonCodeTable[ SKELTON_CODE_NUM ];
};

GFL_NAMESPACE_END(Skelton)
  GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_WIN32