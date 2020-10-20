#if defined(GF_PLATFORM_WIN32)
//=================================================================================================
/**
* @file  SkeltonModel.cpp
* @brief
* @date 2014.02.24
* @author tamada@gamefreak.co.jp
*/
//=================================================================================================
#include "Test/Skelton/include/Field/SkeltonModel.h"

// #include "debugprint.h"

//#include "field/field_camera.h"

//#include "field/field_util.h"

#include "Test/Skelton/include/Field/SkeltonFieldTransform.h"

#include "Test/Skelton/include/Field/SkeltonModelManager.h"
#include "Test/Skelton/include/Field/SkeltonCode.h"
#include "Test/Skelton/include/SkeltonRscManager.h"

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMotData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/Renderer/gfl2_DrawManager.h>

#include <Animation/include/gfl2_AnimationController.h>
#include <system/include/InputDevice/gfl2_Pad.h>

#include "Shlwapi.h"

using namespace gfl2::renderingengine;
using namespace gfl2::animation;

GFL_NAMESPACE_BEGIN(Test)
  GFL_NAMESPACE_BEGIN(Skelton)


  //=================================================================================================
  //=================================================================================================

  //-----------------------------------------------------------------------------------------------
  //-----------------------------------------------------------------------------------------------
class gfx_impl
{
  friend class SkeltonModel;

  enum { MOTION_MAX = 13 };

private:
  SkeltonRscManager * m_rsc_man;   // @todo ポインタはやめたい
  const chara::ModelResource * m_rsc_data;
  scenegraph::resource::ResourceNode				*m_pModelResourceRootNode;
  scenegraph::resource::ResourceNode				*m_pMotionResourceRootNode[MOTION_MAX];
  scenegraph::instance::ModelInstanceNode   *m_pModelInstanceNode;
  AnimationController												m_AnimationController;

  u32 m_AnimeNo;

public:
  gfx_impl() : m_rsc_man( NULL ), m_rsc_data( NULL ), m_pModelResourceRootNode( NULL ), m_pModelInstanceNode( NULL )
  {
    for (int i = 0; i < MOTION_MAX; ++ i )
    {
      m_pMotionResourceRootNode[i] = NULL;
    }

  }

  ~gfx_impl(){}

public:
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode * Setup( SkeltonRscManager * p_rsc_man, const chara::ModelResource * p_resource, gfl2::heap::HeapBase* p_heap, System::nijiAllocator* p_Allocator )
  {
    OutputDebugString(L"SkeltonModel::Load ");
    OutputDebugString( p_resource->model_file_name );
    OutputDebugString(L"\n");

    //if ( p_rsc_man->HasResource( p_resource->model_file_name ) == false )
    {
      bool result = p_rsc_man->LoadModelResource( p_resource->model_file_name );
      if ( !result )
      {
        return NULL;
      }
    }

    GFL_ASSERT( p_resource->anime_max <= MOTION_MAX );
    m_pModelResourceRootNode = p_rsc_man->GetResourceNode( p_resource->model_file_name );
    for (int i = 0; i < p_resource->anime_max; ++ i )
    {
      if ( PathFileExists( p_resource->anime_file_table[i] ) == false )
      {
        m_pMotionResourceRootNode[i] = NULL;
      }
      else
      {
        bool result = p_rsc_man->LoadMotionResource( p_resource->anime_file_table[i] );
        if (!result)
        {
          return NULL;
        }
        m_pMotionResourceRootNode[i] = p_rsc_man->GetResourceNode( p_resource->anime_file_table[i] );
      }
    }
    m_rsc_man = p_rsc_man;
    m_rsc_data = p_resource;
    m_pModelInstanceNode = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( p_Allocator, m_pModelResourceRootNode );

    // アニメーションコントローラ初期化
    m_AnimationController.Initialize( p_heap, m_pModelInstanceNode);

    // 待機モーションを再生しておく
    JointAnimationSlot* pSlot = m_AnimationController.GetJointSlot();
    pSlot->SetAnimationResource(m_pMotionResourceRootNode[0]);
    pSlot->SetLoop(true);

    return m_pModelInstanceNode;
  }

  void Clear( void )
  {
    m_AnimationController.Finalize();

    if (m_rsc_man == NULL )
    {
      OutputDebugString(L"gfx_impl::Clear 複数回呼ばれている！！！\n");
      return;
    }
    m_rsc_man->UnloadResource( m_rsc_data->model_file_name );
    for ( int i = 0; i < m_rsc_data->anime_max; ++ i )
    {
      if (m_pMotionResourceRootNode[i] != NULL )
      {
        m_rsc_man->UnloadResource( m_rsc_data->anime_file_table[i] );
      }
    }
    for (int i = 0; i < MOTION_MAX; ++ i )
    {
      m_pMotionResourceRootNode[i] = NULL;
    }
    m_pModelResourceRootNode = NULL;
    delete m_pModelInstanceNode;
    m_pModelInstanceNode = NULL;
    m_rsc_data = NULL;
    m_rsc_man = NULL;
  }

  bool HasAnimation( u32 anime_id ) const
  {
    return m_pMotionResourceRootNode[anime_id] != NULL;
  }

  bool ChangeAnimation( u32 anime_id, bool loopFlag, f32 interp_frame )
  {
    // dprintf("gfx_impl::ChangeAnimation(%p)\n", this);
    if ( m_pMotionResourceRootNode[anime_id] == NULL ) return false;

    // 補完中なら無視
    if (m_AnimationController.IsInterp())
    {
      return false;
    }

    bool isInterp = interp_frame > 0.0f;

    // スロット取得
    JointAnimationSlot* pSlot = (isInterp)
      ? m_AnimationController.GetJointInterpSlot()
      : m_AnimationController.GetJointSlot();
    JointAnimationSlot* pSlotOld = m_AnimationController.GetJointSlot();

    pSlot->SetAnimationResource(m_pMotionResourceRootNode[anime_id]);
    pSlot->SetLoop(loopFlag);

    if (isInterp)
    {
      m_AnimationController.InterpRequest(interp_frame);
    }

    m_AnimeNo = anime_id;
    return true;
  }

  void IncAnimation( u32 anime_id, f32 add_frame )
  {
    if ( m_pMotionResourceRootNode[anime_id] == NULL ) return;

    JointAnimationSlot* pSlot = m_AnimationController.GetJointSlot();
    pSlot->SetStepFrame(add_frame);
    m_AnimationController.Update();
  }

  bool IsAnimeLastFrame( u32 anime_id ) const
  {
    // 補間中なら補間用のスロット
    const JointAnimationSlot* pSlot = (m_AnimationController.IsInterp())
      ? m_AnimationController.GetJointInterpSlot()
      : m_AnimationController.GetJointSlot();

    return pSlot->IsLastFrame();
  }

  f32 GetFrameCount( )
  {

  }

  bool ChangeAnimationIfOther( u32 anime_id, bool loopFlag, f32 interp_frame )
  {
    if ( m_AnimeNo == anime_id ) return false;

    OutputDebugString(L"ChangeAnimationIfOther(");
    OutputDebugString( chara::GetMotionName(m_AnimeNo ) );
    OutputDebugString(L"-->");
    OutputDebugString( chara::GetMotionName(anime_id ) );
    OutputDebugString(L")\n");

    //SetAnimation( anime_id, frame );
    return this->ChangeAnimation(anime_id, loopFlag, interp_frame);
  }

  f32 GetAnimationFrame() const
  {
    const JointAnimationSlot* pSlot = m_AnimationController.GetJointSlot();
    return pSlot->GetFrame();
  }

  f32 GetAnimationMaxFrame() const
  {
    const JointAnimationSlot* pSlot = m_AnimationController.GetJointSlot();
    return pSlot->GetFrameCount();
  }

  void SetAnimationFrame( f32 frame )
  {
    JointAnimationSlot* pSlot = m_AnimationController.GetJointSlot();
    return pSlot->SetFrame( frame );
  }

  void SetAnimationStepFrame( f32 frame )
  {
    JointAnimationSlot* pSlot = m_AnimationController.GetJointSlot();
    return pSlot->SetStepFrame( frame );
  }

  bool IsAnimeLooped() const
  {
    const JointAnimationSlot* pSlot = m_AnimationController.GetJointSlot();
    return pSlot->IsLastFrame();
  }

  bool IsAnimeInterp() const
  {
    return m_AnimationController.IsInterp();
  }

  gfl2::math::AABB GetAABB() const
  {
    return m_AnimationController.GetAABB();
  }
};

//=================================================================================================
//=================================================================================================

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
SkeltonModel::SkeltonModel( gfl2::heap::HeapBase * p_heap, System::nijiAllocator* p_Allocator ):
  m_exists(false),
  m_id(SMDL_NONE),
  m_animeFlag(false),
  m_animeNo(0),
  m_animeCounter(0.0),
  m_fieldCamera(NULL),
  m_pOriginNode(NULL),
  m_pWaistNode(NULL),
  m_WaistScale(1.0f),
  m_MotionMoveSpeed(0.0f),
  m_PauseFlag( false ),
  m_AngleLockForMoveFlag( false ),
  mHeap( p_heap),
  mAllocator( p_Allocator )
{
  m_transform = GFL_NEW( p_heap ) SkeltonFieldTransform;
  m_gfx_impl = GFL_NEW( p_heap ) gfx_impl;
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
SkeltonModel::~SkeltonModel()
{
  GFL_SAFE_DELETE( m_gfx_impl );
  GFL_SAFE_DELETE( m_transform );
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
bool SkeltonModel::activate(SkeltonModelManager * p_manager)
{
  m_pManager = p_manager;
  //		return m_model->activate(pGSys);
  return true;
}
gfl2::renderingengine::scenegraph::instance::ModelInstanceNode * SkeltonModel::GetInstance( void )
{
  return m_gfx_impl->m_pModelInstanceNode;
}

SkeltonFieldTransform * SkeltonModel::GetTransform( void ) const
{
  return m_transform;
};

void SkeltonModel::Clear( void )
{
  m_exists = false;
  //		m_modelData.cleanup();
  m_gfx_impl->Clear();
}

bool SkeltonModel::Setup( SkeltonRscManager * p_rsc_man, const SkeltonModelHeader * p_header )
{
  m_id				= p_header->SMDL_id;
  m_rsc_id		= p_header->rsc_id;
  m_mv_code		=	p_header->SKELTON_CODE;
  m_param0		= p_header->param0;
  m_param1		= p_header->param1;
  m_param2		= p_header->param2;

  m_SkeltonCode	= SkeltonModelManager::GetSkeltonCode( m_mv_code );

  m_transform->Clear();
  const chara::ModelResource * p_resource = chara::GetModelResource( m_rsc_id );
  float size_rate = SkeltonModelManager::GetSizeRate() * p_resource->adjustHeightValue;
  //		float size_rate = SkeltonModelManager::GetSizeRate();
  m_transform->SetScale( gfl2::math::Vector4(size_rate,size_rate,size_rate) );
  m_transform->SetTranslation( gfl2::math::Vector4(p_header->x, p_header->y, p_header->z ) );
  m_transform->SetAngle( p_header->angle );

  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode * p_instance = m_gfx_impl->Setup( p_rsc_man, p_resource, mHeap, mAllocator );
  assert( p_instance != NULL );

  SkeltonCodeBase::InitializeSkeltonCodeWork( &m_SkeltonCodeWork, this, NULL );
  m_SkeltonCode->InitProcess( &m_SkeltonCodeWork );
  m_exists = true;

  // オリジンとウエストのジョイント取得
  for (u32 i = 0; i < p_instance->GetJointNum(); ++i)
  {
    gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pJointNode = p_instance->GetJointInstanceNode(i);
    if (strcmp(pJointNode->GetNodeName(), "Origin") == 0)
    {
      m_pOriginNode = pJointNode;
    }
    if (strcmp(pJointNode->GetNodeName(), "Waist") == 0)
    {
      m_pWaistNode = pJointNode;
    }
  }
  return true;
}


bool SkeltonModel::Setup( SkeltonRscManager * p_rsc_man, const SkeltonModelHeaderEx * p_header )
{
  m_id				= p_header->SMDL_id;
  m_rsc_id		= p_header->rsc_id;
  m_mv_code		=	p_header->SKELTON_CODE;
  m_param0		= p_header->param0;
  m_param1		= p_header->param1;
  m_param2		= p_header->param2;

  m_SkeltonCode	= SkeltonModelManager::GetSkeltonCode( m_mv_code );

  m_transform->Clear();
  const chara::ModelResource * p_resource = &p_header->modelResource;


  float size_rate = SkeltonModelManager::GetSizeRate() * p_resource->adjustHeightValue;
  //		float size_rate = SkeltonModelManager::GetSizeRate();
  m_transform->SetScale( gfl2::math::Vector4(size_rate,size_rate,size_rate) );
  m_transform->SetTranslation( gfl2::math::Vector4(p_header->x, p_header->y, p_header->z ) );
  m_transform->SetAngle( p_header->angle );

  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode * p_instance = m_gfx_impl->Setup( p_rsc_man, p_resource, mHeap, mAllocator );
  if( p_instance == NULL )
  {
    return false;
  }

  SkeltonCodeBase::InitializeSkeltonCodeWork( &m_SkeltonCodeWork, this, NULL );
  m_SkeltonCode->InitProcess( &m_SkeltonCodeWork );
  m_exists = true;

  // オリジンとウエストのジョイント取得
  for (u32 i = 0; i < p_instance->GetJointNum(); ++i)
  {
    gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pJointNode = p_instance->GetJointInstanceNode(i);
    if (strcmp(pJointNode->GetNodeName(), "Origin") == 0)
    {
      m_pOriginNode = pJointNode;
    }
    if (strcmp(pJointNode->GetNodeName(), "Waist") == 0)
    {
      m_pWaistNode = pJointNode;
    }
  }
  return true;
}



void SkeltonModel::updateAnimation( float timediff )
{
  if ( m_exists == false ) return;

  if(m_animeFlag == true)
  {
    m_animeCounter += timediff;
    if(m_animeCounter > 1.0f){
      unsigned short a = (unsigned short)m_animeCounter;
      m_gfx_impl->IncAnimation(m_animeNo, a );
      m_animeCounter -= a;
    }
  }
}

void SkeltonModel::UpdateAnimation( f32 frame )
{
  if ( m_exists == false ) return;

  if(m_animeFlag == true)
  {
    m_gfx_impl->IncAnimation(m_animeNo, static_cast<f32>(frame) );

  }
}

void SkeltonModel::ChangeAnimation( unsigned int id, bool loopFlag, f32 interp_frame )
{
  //m_gfx_impl->SetAnimation( id, 0 );
  if (m_gfx_impl->ChangeAnimation(id, loopFlag, interp_frame))
  {
    m_animeNo = id;
    m_animeFlag = true;
    m_animeCounter = 0;
  }
}

bool SkeltonModel::ChangeAnimationIfOther( u32 id, bool loopFlag, f32 interp_frame )
{
  if ( m_gfx_impl->ChangeAnimationIfOther( id, loopFlag, interp_frame ) )
  {
    m_animeNo = id;
    m_animeFlag = true;
    m_animeCounter = 0;
    return true;
  }
  return false;
}

f32 SkeltonModel::GetAnimationFrame( void ) const
{
  //return m_gfx_impl->m_AnimeFrame;
  return m_gfx_impl->GetAnimationFrame();
}

f32 SkeltonModel::GetAnimationMaxFrame( void ) const
{
  //return m_gfx_impl->m_AnimeFrame;
  return m_gfx_impl->GetAnimationMaxFrame();
}

void SkeltonModel::SetAnimationFrame( f32 frame )
{
  return m_gfx_impl->SetAnimationFrame( frame );
}

void SkeltonModel::SetAnimationStepFrame( f32 frame )
{
  return m_gfx_impl->SetAnimationStepFrame( frame );
}


bool SkeltonModel::HasAnimation( u32 id ) const
{
  return m_gfx_impl->HasAnimation(id);
}

bool SkeltonModel::IsAnimeLooped( void ) const
{
  //return m_gfx_impl->m_IsAnimeLooped;
  return m_gfx_impl->IsAnimeLooped();
}

bool SkeltonModel::IsAnimeLastFrame( void ) const
{
  return m_gfx_impl->IsAnimeLastFrame(m_animeNo);
}

void SkeltonModel::draw( void )
{
  // do nothing
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
void SkeltonModel::update( void )
{
  if ( m_exists == false ) return;

  m_SkeltonCode->MainProcess( &m_SkeltonCodeWork );

  doCommand();

  m_transform->Update();
  m_gfx_impl->m_pModelInstanceNode->SetLocalSRT(m_transform->GetSRT());
}


//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
void SkeltonModel::doCommand( void )
{
  switch (m_commandBuffer.type )
  {
  case COMMAND_TYPE_NONE:
    break;
  case COMMAND_TYPE_VECTOR:
    {
      const Command::DATA_UNION::VEC_DATA & vec_data = m_commandBuffer.data.vec_data;
      const gfl2::math::Vector4 dvalue( vec_data.x, vec_data.y, vec_data.z );
      // if ( dvalue.x == 0 && dvalue.z == 0 ) break;

      gfl2::math::Vector4 org_pos = m_transform->GetTranslation();
      org_pos += dvalue;
      m_transform->SetTranslation( org_pos );

      if( m_AngleLockForMoveFlag  == false )
      {
        m_transform->SetAngle( vec_data.m_dir );
      }
    }
    break;

  case COMMAND_TYPE_POSITION:
    {
      const Command::DATA_UNION::POS_DATA & pos_data = m_commandBuffer.data.pos_data;
      const gfl2::math::Vector4 new_pos( pos_data.x, pos_data.y, pos_data.z );

      gfl2::math::Vector4 org_pos = m_transform->GetTranslation();
      m_transform->SetTranslation( new_pos );

      if( m_AngleLockForMoveFlag  == false )
      {
        m_transform->SetAngle( pos_data.m_dir );
      }
    }
    break;

  case COMMAND_TYPE_ORIGIN_NODE_MOVE:
    break;

  }
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
void SkeltonModel::updateAfter( void )
{
  if ( m_exists == false ) return;

  // Originノードでの移動用にアニメーション更新後に更新する
  doCommandAfter();
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
void SkeltonModel::doCommandAfter( void )
{
  switch (m_commandBuffer.type )
  {
  case COMMAND_TYPE_NONE:
  case COMMAND_TYPE_VECTOR:
  case COMMAND_TYPE_POSITION:
    break;

  case COMMAND_TYPE_ORIGIN_NODE_MOVE:
    {
      if (m_pOriginNode == NULL) break;

      const Command::DATA_UNION::VEC_DATA& vec_data = m_commandBuffer.data.vec_data;
      const gfl2::math::Vector4 dvalue( vec_data.x, vec_data.y, vec_data.z );
      if ( dvalue.x == 0 && dvalue.z == 0 ) break;

      // 前回からのOriginの移動量計算
      gfl2::math::Vector3 originPos3 = m_pOriginNode->GetLocalTranslate();
      gfl2::math::Vector4 originPos(originPos3.x, originPos3.y, originPos3.z); 
      m_MotionMoveSpeed = originPos.z;

      // y以外の平行移動成分は削除
      originPos.x = 0.0f;
      originPos.z = 0.0f;

      originPos3.Set(originPos.x, originPos.y, originPos.z);
      m_pOriginNode->SetLocalTranslate(originPos3);

      // Originにスケールをかける
      m_pOriginNode->SetLocalScale(m_pOriginNode->GetLocalScale() * m_WaistScale);
      for (u32 i = 0; i < m_pOriginNode->GetChildCount(); ++i)
      {
        // 階層スケーリングを無効にする
        m_pOriginNode->GetChild(i)->SafeCast<gfl2::renderingengine::scenegraph::instance::JointInstanceNode>()->SetNonInheritanceScale(false);
      }

      if (!gfl2::system::InputDevice::Pad::IsPress(0, gfl2::system::InputDevice::Button::Y))
      {
        if (originPos.y > 0.0f)
        {
          f32 miny = m_gfx_impl->GetAABB().GetMin().y;
          // めり込み対策にyにも係数を掛ける
          miny = miny - originPos.y;
          miny *= m_WaistScale;
          miny += originPos.y;
          if (miny < 0.0f)
          {
            originPos.y -= miny;
            originPos3.Set(originPos.x, originPos.y, originPos.z);
            m_pOriginNode->SetLocalTranslate(originPos3);
          }
        }
      }

      // 移動量を現在のトランスに加算
      gfl2::math::Vector4 trans = m_transform->GetTranslation();
      trans += dvalue * m_MotionMoveSpeed * m_WaistScale;
      m_transform->SetTranslation(trans);
      m_transform->SetAngle(vec_data.m_dir);

      // モデルインスタンスの行列更新
      m_transform->Update();
      m_gfx_impl->m_pModelInstanceNode->SetLocalSRT(m_transform->GetSRT());
    }
    break;

  }
  m_commandBuffer.type = COMMAND_TYPE_NONE;
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
void SkeltonModel::SetCommand(const SkeltonModel::Command &cmd )
{
  m_commandBuffer = cmd;
}


//=================================================================================================
//=================================================================================================
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
SkeltonModel::Command::Command() :
  type(SkeltonModel::COMMAND_TYPE_NONE)
{
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
SkeltonModel::Command SkeltonModel::Command::CreateVectorCommand( gfl2::math::Vector4 &vec, float direction, u8 playerMove )
{
  Command dt;

  dt.type = COMMAND_TYPE_VECTOR;
  dt.data.vec_data.x = vec.x;
  dt.data.vec_data.y = vec.y;
  dt.data.vec_data.z = vec.z;
  dt.data.vec_data.m_dir = direction;
  dt.data.vec_data.palyerMove = playerMove;
  return dt;
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
SkeltonModel::Command SkeltonModel::Command::CreatePositionCommand( gfl2::math::Vector4 &pos, float direction )
{
  Command dt;
  dt.type = COMMAND_TYPE_POSITION;
  dt.data.pos_data.x = pos.x;
  dt.data.pos_data.y = pos.y;
  dt.data.pos_data.z = pos.z;
  dt.data.pos_data.m_dir = direction;
  return dt;
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
SkeltonModel::Command SkeltonModel::Command::CreateOriginNodeMoveCommand( gfl2::math::Vector4 &vec, float direction )
{
  Command dt;

  dt.type = COMMAND_TYPE_ORIGIN_NODE_MOVE;
  dt.data.vec_data.x = vec.x;
  dt.data.vec_data.y = vec.y;
  dt.data.vec_data.z = vec.z;
  dt.data.vec_data.m_dir = direction;
  return dt;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool SkeltonModel::IsMoveCommand( const SkeltonModel * p_model, gfl2::math::Vector4 & mv )
{
  bool is_move_command = false;

  {
    const SkeltonModel::Command cmd = p_model->GetCommand();
    switch ( cmd.type )
    {
    case SkeltonModel::COMMAND_TYPE_VECTOR:
      {
        if ( cmd.data.vec_data.x != 0 
          || cmd.data.vec_data.y != 0
          || cmd.data.vec_data.z != 0 )
        {
          is_move_command = true;
          mv = gfl2::math::Vector4(cmd.data.vec_data.x,cmd.data.vec_data.y,cmd.data.vec_data.z);
        }
      }
      break;
    case SkeltonModel::COMMAND_TYPE_POSITION:
      {
        const gfl2::math::Vector4 now_pos = p_model->GetTransform()->GetTranslation();
        if ( cmd.data.pos_data.x != now_pos.x
          || cmd.data.pos_data.y != now_pos.y
          || cmd.data.pos_data.z != now_pos.z )
        {
          is_move_command = true;
          mv = gfl2::math::Vector4(cmd.data.pos_data.x - now_pos.x,
            cmd.data.pos_data.y -now_pos.y, cmd.data.pos_data.z - now_pos.z);
        }
      }
      break;
    case SkeltonModel::COMMAND_TYPE_NETWORK:
      {
        const gfl2::math::Vector4 now_pos = p_model->GetTransform()->GetTranslation();
        if ( cmd.data.network_data.x != now_pos.x
          || cmd.data.network_data.y != now_pos.y
          || cmd.data.network_data.z != now_pos.z )
        {
          is_move_command = true;
          mv = gfl2::math::Vector4(cmd.data.network_data.x - now_pos.x,
            cmd.data.network_data.y -now_pos.y, cmd.data.network_data.z - now_pos.z);
        }
      }
    }
  }
  return is_move_command;
}

// スケールテスト
void SkeltonModel::SetWaistScale(f32 scale)
{
  m_WaistScale = scale;
}
f32 SkeltonModel::GetWaistScale()
{
  return m_WaistScale;
}

GFL_NAMESPACE_END(Skelton)
  GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_WIN32