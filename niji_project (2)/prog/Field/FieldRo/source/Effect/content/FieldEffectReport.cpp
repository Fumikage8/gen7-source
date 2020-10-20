//======================================================================
/**
 * @file FieldEffectReport.cpp
 * @date 2015/11/09 12:59:31
 * @author takahashi_tomoya
 * @brief レポート表示
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/Effect/content/FieldEffectReport.h"

// field
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Window/FieldWindowManager.h"
#include "Field/FieldRo/include/FieldLayoutPriority.h"

#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"

// gfl2
#include <Layout/include/gfl2_LytSys.h>

// conv_header
#include <niji_conv_header/field/effect/report.h>
#include <niji_conv_header/field/effect/report_anim_list.h>
#include <niji_conv_header/field/effect/report_pane.h>


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

static const s32 EFFECT_LAYOUT_HEAP_SIZE    = 0x1000;
static const s32 EFFECT_LAYOUT_DEVHEAP_SIZE = 0x8800;


const u32 EffectReport::sc_ANIME_INDEX[ ANIME_MAX ] = 
{
  LA_REPORT_BALLOON_IN,
  LA_REPORT_BALLOON_LOOP,
  LA_REPORT_BALLOON_OUT
};

static const bool sc_AutoUnbind[ EffectReport::ANIME_MAX ] = 
{
  true,
  false,
  false
};

/**
 *  @brief  コンストラクタ
 */
EffectReport::EffectReport( void )
: IEffectBase()
, m_appUtilHeap()
, m_pLayout( NULL )
, m_pWindowManager( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
EffectReport::~EffectReport( void )
{
}

/**
 *  @brief  初期化
 */
void EffectReport::Initialize( SetupData &setupData )
{
  m_pWindowManager = setupData.pWinMgr;

  m_pCameraManager = setupData.pCameraManager;

  // ローカルヒープ作成
  m_appUtilHeap.LocalHeapCreate( setupData.pHeap, setupData.pHeap, EFFECT_LAYOUT_HEAP_SIZE, EFFECT_LAYOUT_DEVHEAP_SIZE );

  // レイアウト生成
  app::util::AppRenderingManager *pRender = m_pWindowManager->GetRenderingManager();
  m_pLayout = GFL_NEW( setupData.pHeap ) Field::Effect::EffectLayout( &m_appUtilHeap, pRender );

  // レイアウト初期化
  {
    app::util::G2DUtil::LytResData resTbl[] = 
    {
      {
        setupData.pResource,
        0,
        app::util::G2DUtil::ATTACH_ROOT
      },
    };

    app::util::G2DUtil::LytwkData dataTbl[] =
    {
      {
        0,
        LYTRES_REPORT_BALLOON_BFLYT,
        LA_REPORT_BALLOON___NUM,
        setupData.pResource,
        false,
        app::util::G2DUtil::SETUP_UPPER,
        true
      },
    };

    Field::Effect::EffectLayout::SetupParam param;
    param.layoutNum = 1;
    param.lytResTbl = resTbl;
    param.resTblNum = GFL_NELEMS( resTbl );
    param.lytDataTbl = dataTbl;
    param.dataTblNum = GFL_NELEMS( dataTbl );
    m_pLayout->Initialize( param );
  }

  // 登録
  m_pWindowManager->GetUIView()->AddSubView( m_pLayout );
  m_pLayout->SetVisible( false );
  m_pLayout->SetPriority( Field::FIELD_LAYOUT_PRIORITY_3D );

  // 親の座標を求める
  if( setupData.pParentObject )
  {
    s32 joint_index = -1;
    poke_3d::model::BaseModel * pBaseModel = NULL;

    if( setupData.pParentObject->SafeCast<Field::MoveModel::FieldMoveModel>() != NULL )
    {
      Field::MoveModel::FieldMoveModel *pMoveModel = setupData.pParentObject->SafeCast<Field::MoveModel::FieldMoveModel>();
      pBaseModel = pMoveModel->GetCharaDrawInstance();
      joint_index = pBaseModel->GetJointIndex( "Loc_Head" );
    }

    // ジョイントすべきノードが見つからない
    if( joint_index == -1 )
    {
      gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *pNode = pBaseModel->GetModelInstanceNode();
      u32 jointCount = pNode->GetJointNum();
      for( u32 i = 0; i < jointCount; ++ i )
      {
        gfl2::renderingengine::scenegraph::instance::JointInstanceNode *pJoint = pNode->GetJointInstanceNode( i );

        GFL_PRINT( "joint_name %s\n", pJoint->GetName() );
      }

      GFL_ASSERT_MSG( false, "joint_name Loc_Head not found" );

      m_pJointNode = NULL;
      m_Pos = gfl2::math::Vector3(-1000,0,-1000);
    }
    else
    {
      gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *pNode = pBaseModel->GetModelInstanceNode();
      m_pJointNode = pNode->GetJointInstanceNode( joint_index );
      m_Pos = m_pJointNode->GetWorldMatrix().GetElemPosition();
    }

  }

}

/**
 *  @brief  破棄
 */
bool EffectReport::Terminate( void )
{
  GFL_SAFE_DELETE( m_pLayout );
  m_appUtilHeap.Delete();
  return true;
}

/**
 *  @brief  更新
 */
void EffectReport::updateNormal( void )
{
  //ポジションを更新
  gfl2::math::Matrix44 mtx_proj;
  gfl2::math::Matrix34 mtx_view;
  m_pCameraManager->GetMainViewCamera()->GetProjectionMatrix( &mtx_proj );
  m_pCameraManager->GetMainViewCamera()->GetViewMatrix( &mtx_view );
  gfl2::math::Vector4 view_pos = mtx_view * m_Pos;
  {
    view_pos.x = static_cast<f32>(static_cast<s32>(view_pos.x));
    view_pos.y = static_cast<f32>(static_cast<s32>(view_pos.y));
    view_pos.z = static_cast<f32>(static_cast<s32>(view_pos.z));
  }
  gfl2::math::Vector4 proj_pos = mtx_proj * view_pos;
  {
    proj_pos.x = static_cast<f32>(static_cast<s32>(proj_pos.x));
    proj_pos.y = static_cast<f32>(static_cast<s32>(proj_pos.y));
    proj_pos.z = static_cast<f32>(static_cast<s32>(proj_pos.z));
  }
  gfl2::math::Vector4 dst_pos = proj_pos;

  gfl2::math::Vector3 screen_pos( 0.0f, 0.0f, 0.0f);
  dst_pos.x = dst_pos.x / dst_pos.w;
  dst_pos.y = dst_pos.y / dst_pos.w;
  screen_pos.x = static_cast<f32>( static_cast<s32>( dst_pos.x * 200.0f ) );
  screen_pos.y = static_cast<f32>( static_cast<s32>( dst_pos.y * 120.0f ) );

  u32 idx = 0;
  m_pLayout->SetPosition( idx, &screen_pos );
}

/**
 *  @brief  終了しているか.
 */
bool EffectReport::IsAnimationLastFrame( void )
{
  app::util::G2DUtil *pUtil = m_pLayout->GetG2DUtil();
  if( !pUtil )
  {
    return true;
  }

  return pUtil->IsAnimeEnd( 0, m_AnimationID);
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectReport::Delete( void )
{
  // 登録は親からするが解除は子からする
  m_pLayout->RemoveFromSuperView();
}

/**
 *  @brief  開始
 */
void EffectReport::Start(AnimeIndex index)
{
  m_AnimationID = sc_ANIME_INDEX[index];
  m_pLayout->SetVisible( true );
  m_pLayout->StartAnimation( m_AnimationID, true, sc_AutoUnbind[index]);
}

/**
 *  @brief  停止
 */
void EffectReport::Stop(AnimeIndex animeID)
{
  u32 index = sc_ANIME_INDEX[animeID];
  m_pLayout->StopAnimation( index);
}


/**
 *  @brief  終了
 */
void EffectReport::End( void )
{
  m_pLayout->SetVisible( false );
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
