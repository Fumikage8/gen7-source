//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		TestShadow.cpp
 *	@brief  影テスト描画システム
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "test/FieldTest/include/TestShadow.h"

#include "test/FieldTest/include/field_transform.h"
#include "test/FieldTest/include/FieldTestUtility.h"

// mmmodel
#include "test/FieldTest/include/movemodel.h"

// リソース
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

// デバッグ

#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

#if PM_DEBUG
#include "test/FieldTest/include/Debug/FieldDebugTypes.h"
#include "test/FieldTest/include/Debug/FieldDebugInstanceMemoryCollection.h"
#endif

// arc
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include "arc_def_index/arc_def.h"


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldTest )

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------
/**
 *  @brief コンストラクタ
 */ 
TestShadow::TestShadow() : 
  m_pHeap(NULL),
  m_pPipeLine(NULL)
{
  for( int i=0; i<RES_NUM; ++i ){
    m_pResourceRootNode[i] = NULL;
    m_pResBuf[i] = NULL;
    m_ResSize[i] = 0;
  }

  for( int i=0; i<MDL_NUM; ++i ){
    m_pModelInstanceNode[i] = NULL;
  }
}

/**
 *  @brief デストラクタ
 */ 
TestShadow::~TestShadow()
{
  Finalize();
}

/**
 *  @brief 初期化
 */
void TestShadow::Initialize( gfl2::heap::HeapBase* pParentHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, Test::FieldTest::MyRenderingPipeLine* pPipeLine )
{
  m_pPipeLine = pPipeLine;
  m_pHeap = pParentHeap;

  //===========================================================
  // ファイル読み込み
  //===========================================================
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = ARCID_SHADOW_TEST;
    openReq.heapForFile = m_pHeap;
    openReq.heapForArcSrc = m_pHeap;
    openReq.heapForReq = m_pHeap;
    pAsyncFileReadManager->SyncArcFileOpen( openReq );
  }
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;

    for( int i= 0; i< RES_NUM; ++i ){
      loadReq.arcId = ARCID_SHADOW_TEST;
      loadReq.datId = i; 
      loadReq.ppBuf = &m_pResBuf[i];
      loadReq.heapForBuf = m_pHeap;
      loadReq.pBufSize = &m_ResSize[i];
      loadReq.align = 128;
      pAsyncFileReadManager->SyncArcFileLoadData( loadReq );

      // リソース生成
      if( i != RES_SHADOW_2_MOT ){
        gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
        resourceData.SetModelData((c8*)m_pResBuf[i]);

        char strbuff[128] = {0};
        Utility::Sprintf_S( strbuff, 127, "Shadow ModelResource[%d]",  i );
        int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
        FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
        m_pResourceRootNode[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData);
        FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);
      }
      else{
        gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
        resourceData.SetMotionData((c8*)m_pResBuf[i]);

        char strbuff[128] = {0};
        Utility::Sprintf_S( strbuff, 127, "Shadow MotionResource[%d]",  i );
        int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
        FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
        m_pResourceRootNode[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData);
        FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);
      }

    }

    // モデルインスタンス生成
    for( int i= 0; i< MDL_NUM; ++i ){
      // インスタンス生成
      char strbuff[128] = {0};
      Utility::Sprintf_S( strbuff, 127, "ShadowModel[%d]", i );
      int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
      FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
      m_pModelInstanceNode[i] = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pResourceRootNode[i]);
      m_pModelInstanceNode[i]->SetNodeName(strbuff);
      FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);
      m_pModelInstanceNode[i]->SetVisible(true);
      m_pPipeLine->AddEdgeRenderingTarget( m_pModelInstanceNode[i] );
    }

    // ハイクオリティモデルの生成
    for( int i=0; i<HIGH_SHADOW_NUM; ++i ){
      // インスタンス生成
      char strbuff[128] = {0};
      Utility::Sprintf_S( strbuff, 127, "HighShadowModel[%d]", i );
      int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
      FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
      m_pHIGHShadowModel[i] = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pResourceRootNode[RES_SHADOW_2]);
      m_pHIGHShadowModel[i]->SetNodeName(strbuff);
      FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);
      m_pHIGHShadowModel[i]->SetVisible(false);
      gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild( m_pHIGHShadowModel[i] );

      {
        gfl2::animation::AnimationController::InitDescription desc;
        desc.jointSlotCount = 0;
        desc.materialSlotCount = 1;
        m_AnimationControler[i].Initialize(m_pHIGHShadowModel[i], desc);
        gfl2::animation::MaterialAnimationSlot* pSlot0 = m_AnimationControler[i].GetMaterialSlot(0);
        pSlot0->SetAnimationResource(m_pResourceRootNode[RES_SHADOW_2_MOT]);
        pSlot0->SetStepFrame(0.0f);
        pSlot0->SetLoop(false);
      }
    }
  }
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
    closeReq.arcId = ARCID_SHADOW_TEST;
    closeReq.heapForReq = m_pHeap;
    pAsyncFileReadManager->SyncArcFileClose( closeReq );
  }

  // 一旦画面外に退避
  static const c8* sc_JointNameTbl[SHADOW_NUM] = 
  {
    "ShadowA",
    "ShadowB",
    "ShadowC",
    "ShadowD",
    "ShadowE",
    "ShadowF",
    "ShadowG",
    "ShadowH",
    "ShadowI",
    "ShadowJ",
  };

  for( int i= 0; i< MDL_NUM; ++i ){
    int joint_num = m_pModelInstanceNode[i]->GetJointNum();
    int joint_index = 0;
    for( int j=0; j<joint_num; ++j ){

      gfl2::renderingengine::scenegraph::instance::JointInstanceNode * p_joint;
      p_joint = m_pModelInstanceNode[i]->GetJointInstanceNode( j );
      const c8 * name = p_joint->GetName();
      if( std::strncmp( name, sc_JointNameTbl[joint_index], 8 ) == 0 ){
        m_pJoint[i][joint_index] = p_joint;
        p_joint->SetLocalTranslate( gfl2::math::Vector3( -1024.0f, 0, 0 ) );

        joint_index ++;
        // 収集完了
        if( joint_index >= SHADOW_NUM ){ break; }
      }
    }

    // モデル情報表示
    {
      int l_mesh_num = m_pModelInstanceNode[i]->GetMehsNum();
      int l_material_num = m_pModelInstanceNode[i]->GetMaterialNum();
      int l_joint_num = m_pModelInstanceNode[i]->GetJointNum();
      GFL_RELEASE_PRINT( "mdl[%d] mesh[%d] material[%d] joint[%d]\n", i, l_mesh_num, l_material_num, l_joint_num );
    }

    GFL_ASSERT( joint_index == SHADOW_NUM );  // 全部収集できたか？
  }
}

/**
 * @brief 破棄
 */
void TestShadow::Finalize( void )
{
  for( int i=0; i<HIGH_SHADOW_NUM; ++i ){
    m_AnimationControler[i].Finalize();

    // インスタンス生成
    m_pPipeLine->RemoveEdgeRenderingTarget( m_pHIGHShadowModel[i] );
    GFL_DELETE( m_pHIGHShadowModel[i] ) ;
  }

  for( int i= 0; i< MDL_NUM; ++i ){
    if( m_pModelInstanceNode[i] ){
      m_pPipeLine->RemoveEdgeRenderingTarget( m_pModelInstanceNode[i] );
      GFL_DELETE m_pModelInstanceNode[i];
      m_pModelInstanceNode[i] = NULL;
    }
  }

  for( int i= 0; i< RES_NUM; ++i ){
    if( m_pResourceRootNode[i] ){
      GFL_DELETE m_pResourceRootNode[i];
      m_pResourceRootNode[i] = NULL;
    }
    GflHeapSafeFreeMemory( m_pResBuf[i] );
  }
}

/**
 * @brief 更新処理
 */
void TestShadow::Update( const fld::mmodel::MoveModelManager * cpMmdl )
{

  // 動作モデルのモデルデータを取得し、影座標を更新する。
  {
    for( int i=0; i<MDL_NUM; ++i )
    {
#if PM_DEBUG
      // フラグがたっていなかったら生成しない
      if( !Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::DRAW_SWITCH_SHADOW_PRI + i ] ){
        continue;
      }
#endif

      int joint_num = m_pModelInstanceNode[i]->GetJointNum();
      int joint_count = 0;

      for( int j=0; j<fld::mmodel::MMDL_MAX; ++j ){

        fld::mmodel::MoveModel* pModel = cpMmdl->GetMoveModelByDataIndex( j );
        if( pModel && (joint_count < joint_num) && (pModel->GetAnimeID() >= 0) ){
          // 影の位置反映
          gfl2::renderingengine::scenegraph::instance::JointInstanceNode * p_joint;
          p_joint = m_pJoint[i][joint_count];

          gfl2::math::Vector trans = pModel->GetTransform()->GetTranslation();
          p_joint->SetLocalTranslate( gfl2::math::Vector3( trans.GetX(), trans.GetY(), trans.GetZ() ) );

          ++joint_count;
        }
      }
    }


    // ハイクオリティ
#if PM_DEBUG
    // フラグがたっていなかったら生成しない
    if( Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::DRAW_SWITCH_SHADOW_TEXANM ] )
#endif
    {
      int joint_num = HIGH_SHADOW_NUM;
      int joint_count = 0;

      for( int j=0; j<fld::mmodel::MMDL_MAX; ++j ){

        fld::mmodel::MoveModel* pModel = cpMmdl->GetMoveModelByDataIndex( j );
        if( pModel && (joint_count < joint_num) && (pModel->GetAnimeID() >= 0) ){
          // 影の位置反映
          gfl2::math::Vector trans = pModel->GetTransform()->GetTranslation();
          m_pHIGHShadowModel[joint_count]->SetLocalTranslate( gfl2::math::Vector3( trans.GetX(), trans.GetY(), trans.GetZ() ) );

          // 止まっているときアニメーション0
          gfl2::animation::MaterialAnimationSlot* pSlot0 = m_AnimationControler[joint_count].GetMaterialSlot(0);
          switch( pModel->GetAnimeID() ){
          case 0:
            pSlot0->SetFrame(0);
            break;
          default:
            pSlot0->SetFrame(10);
            break;
          }
          // 歩いているときアニメーション1
          
          if( !m_pHIGHShadowModel[joint_count]->IsVisible() ){
            m_pHIGHShadowModel[joint_count]->SetVisible(true);
            gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild( m_pHIGHShadowModel[joint_count] );
            m_pPipeLine->AddEdgeRenderingTarget( m_pHIGHShadowModel[joint_count] );
          }
          ++joint_count;
        }
      }
    }
#if PM_DEBUG
    else
    {
      for( int i=0; i<HIGH_SHADOW_NUM; ++i ){
        if( m_pHIGHShadowModel[i]->IsVisible() ){
          m_pHIGHShadowModel[i]->SetVisible( false );
          gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild( m_pHIGHShadowModel[i] );
          m_pPipeLine->RemoveEdgeRenderingTarget( m_pHIGHShadowModel[i] );
        }
      }
    }
#endif

    // アニメーション更新
    for( int i=0; i<HIGH_SHADOW_NUM; ++i ){
      m_AnimationControler[i].Update();
    }
  }


#if PM_DEBUG
  ControlVisible( Field::debug::FieldDebugTypes::s_DrawSwitchTable[Field::debug::FieldDebugTypes::DRAW_SWITCH_SHADOW_PRI], m_pModelInstanceNode[0], m_pPipeLine );
  ControlVisible( Field::debug::FieldDebugTypes::s_DrawSwitchTable[Field::debug::FieldDebugTypes::DRAW_SWITCH_SHADOW_TEX], m_pModelInstanceNode[1], m_pPipeLine );
#endif
}

GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )
