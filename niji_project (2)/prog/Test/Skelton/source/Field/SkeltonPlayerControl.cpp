#if defined(GF_PLATFORM_WIN32)

//=================================================================================================
/**
@file player_control.cpp
@brief
@date 2014.03.02
@author tamada@gamefreak.co.jp
*/
//=================================================================================================

#include "Test/Skelton/include/Field/SkeltonPlayerControl.h"
#include "Test/Skelton/include/Field/SkeltonModel.h"
#include "Test/Skelton/include/Field/Camera/SkeltonCamera.h"
#include "Test/Skelton/include/Field/SkeltonModelManager.h"
#include <math/include/gfl2_math.h>
#include <math/include/gfl2_MathCommon.h>

#include "Test/Skelton/include/Field/SkeltonFieldTransform.h"
#include "Test/Skelton/include/SkeltonDebugInfo.h"


GFL_NAMESPACE_BEGIN(Test)
  GFL_NAMESPACE_BEGIN(Skelton)



  //-------------------------------------------------------------------------------------------------
  //-------------------------------------------------------------------------------------------------
  float PlayerControl::WalkRate = 0.50f;
float PlayerControl::RunRate = 1.0f;
float PlayerControl::MoveRate = 14.0f;	//10.0f * FLD_PLAYER_SIZE_RATE * 2;

float PlayerControl::WalkSpeed = 2.1f;
float PlayerControl::RunSpeed = 4.2f;

float PlayerControl::ChangeWalkRate( float change )
{
  if ( change > 0 && WalkRate + change < 0.4f )
  {
    WalkRate += change;
  }
  else if ( WalkRate + change > 0.01f )
  {
    WalkRate += change;
  }
  return WalkRate;		
}
float PlayerControl::ChangeRunRate( float change )
{
  if ( change > 0 && RunRate + change < 0.4f )
  {
    RunRate += change;
  }
  else if ( RunRate + change > 0.01f )
  {
    RunRate += change;
  }
  return RunRate;		
}

float PlayerControl::ChangeMoveRate( float change )
{
  if ( change > 0 && MoveRate + change < 20.0f )
  {
    MoveRate += change;
  }
  else if ( MoveRate + change > 1.0f )
  {
    MoveRate += change;
  }
  return MoveRate;		
}

// コリジョンライブラリ、コールバックインターフェイス
class MapCallbackable : public gfl2::collision::ICollisionModelCallbackable
{
public:

  static const u32 MAX_HIT_COUNT = 10;

  u32 m_HitCount;
  f32 m_HitHeightTable[ MAX_HIT_COUNT ];

  MapCallbackable() : m_HitCount( 0 )
  {
    for( u32 i = 0; i < MAX_HIT_COUNT; i++ )
    {
      m_HitHeightTable[ i ] = 0.0f;
    }
  }

  virtual ~MapCallbackable()
  {
  }

  // 当たり判定コールバック関数
  virtual bool CollisionModelHeightCallback( const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, f32 height )
  {
    // 高さを記憶
    m_HitHeightTable[ m_HitCount ] = height;
    m_HitCount++;

    if( m_HitCount >= MAX_HIT_COUNT )
    {
      // 衝突検出終了
      return false;
    }
    else
    {
      // 衝突検出続行
      return true;
    }
  }


  // 指定の高さに一番近い高さを取得する、ヒットしていない場合は指定の高さがそのまま返される
  f32 GetNearHight( f32 height )
  {
    if( m_HitCount == 0 )
    {
      return height;
    }

    height += 10.0f;

    f32 resultHeight( m_HitHeightTable[0] );
    f32 resultDiff( gfl2::math::Abs( resultHeight - height ) );
    for( u32 i = 1; i < m_HitCount; i++ )
    {
      f32 diff( gfl2::math::Abs( m_HitHeightTable[i] - height ));
      if( diff < resultDiff )
      {
        resultHeight = m_HitHeightTable[i];
        resultDiff = diff;
      }
    }

    return resultHeight;
  }

  // 再取得可能な状態にする
  void Clear( void )
  {
    m_HitCount = 0;
  }

};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
PlayerControl::PlayerControl():
  m_SMDL(NULL),
  m_camera(NULL)
{
  m_CameraMatrix = gfl2::math::Matrix::GetIdentity();
}

PlayerControl::~PlayerControl()
{
}


bool PlayerControl::activate( SkeltonModel * p_mmdl, SkeltonCamera * p_camera )
{
  m_SMDL = p_mmdl;
  m_camera = p_camera;
  if ( m_camera )
  {
    // m_camera->SetOffset( gfl2::math::Vector4( 0, 40.0f * mmodel::SkeltonModelManager::GetSizeRate2(), 0, 0 ) );
  }

  return true;
}

void PlayerControl::Update( void )
{
  gfl2::math::Vector4 player_pos = m_SMDL->GetTransform()->GetTranslation();
  if ( m_camera && SkeltonDebugInfo::m_DebugInfoCameraStopFlag == false )
  {
    // m_camera->SetTargetPos( player_pos );
  }

  // 自機が止まっている場合は、スティック操作用行列を更新
  if ( m_camera && ( m_SMDL->GetAnimeID() == chara::MOTION_ID_WAIT || m_camera->IsManual() ) )
  {
    gfl2::math::Matrix mat;
    m_camera->GetViewMatrix( &mat );
    m_CameraMatrix = mat.Inverse();
  }
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void PlayerControl::CalculateMoveVector( SkeltonCamera * p_camera, float dx, float dz,
                                        gfl2::math::Vector4 & mv, float & dir, PlayerMove playerMove )
{
  if (dx == 0 && dz == 0 ) return;

  float rate = MoveRate * SkeltonModelManager::GetSizeRate();

  gfl2::math::Matrix mtx = gfl2::math::Matrix::GetIdentity();
  gfl2::math::Vector4 target_pos;
  if ( p_camera )
  {
    p_camera->GetViewMatrix( &mtx );
  }
  const gfl2::math::Matrix inv_mtx = mtx.Inverse();

  {
    // コントローラー移動量を実際の移動量に計算
    gfl2::math::Vector4 raw_move( dx, 0, dz, 0 );
    mv = m_CameraMatrix * raw_move;
    // mv = raw_move;
    mv.y = 0;

    float rad = atan2( mv.z, mv.x );
    dir = gfl2::math::NormalizeRadianAngle(-rad + 0.5f * PI);
    mv = mv.Normalize3() * SkeltonDebugInfo::m_PlayerMoveSettingTable[ playerMove ].moveSpeed * 100.0f / 30.0f;
  }
}

void PlayerControl::CalculateMoveVector( SkeltonCamera * p_camera, float dx, float dz,
                                        gfl2::math::Vector4 & mv, float & dir )
{
  if (dx == 0 && dz == 0 ) return;

  float rate = MoveRate * SkeltonModelManager::GetSizeRate();

  gfl2::math::Matrix mtx = gfl2::math::Matrix::GetIdentity();
  gfl2::math::Vector4 target_pos;
  if ( p_camera )
  {
    p_camera->GetViewMatrix( &mtx );
  }
  const gfl2::math::Matrix inv_mtx = mtx.Inverse();

  {
    // コントローラー移動量を実際の移動量に計算
    gfl2::math::Vector4 raw_move( dx, 0, dz, 0 );
    mv = m_CameraMatrix * raw_move;
    // mv = raw_move;
    mv.y = 0;

    float rad = atan2( mv.z, mv.x );
    dir = gfl2::math::NormalizeRadianAngle(-rad + 0.5f * PI);

    mv = mv.Normalize3();
  }
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
SkeltonModel::Command PlayerControl::CreatePCCommand( float dx, float dz, bool is_run, bool is_debug_run, gfl2::collision::CollisionModel* pCollisionModel )
{
  const SkeltonFieldTransform * trans = m_SMDL->GetTransform();
  gfl2::math::Vector4 mv( dx, 0.0f, dz, 0.0f );
  f32 inputValue( mv.Length() );
  if( inputValue > 1.0f )
  {
    inputValue = 1.0f;
  }

  if ( inputValue < 0.05f )
  {
    mv = gfl2::math::Vector4(0,0,0);

    // 高さ判定
    if( pCollisionModel )
    {
      gfl2::math::Vector4 newPos( trans->GetTranslation() );
      MapCallbackable mapCallbackabl;
      pCollisionModel->GetHeight( newPos, NULL, &mapCallbackabl );
      f32 newHight = mapCallbackabl.GetNearHight( newPos.y );
      mv.y = newHight - newPos.y;
    }

    return SkeltonModel::Command::CreateVectorCommand( mv, trans->GetAngle(), 0 );
  }

  float dir;
  PlayerMove playerMove = GetPlayerMove( inputValue ) ;

  if( is_run )
  {
    playerMove = static_cast<PlayerMove>( SkeltonDebugInfo::m_PlayerMoveByDush );
  }

  if( is_debug_run )
  {
    playerMove = PLAYER_MOVE_DEBUG_RUN;
  }


  CalculateMoveVector( m_camera, dx, dz, mv, dir, playerMove );

  // 高さ判定
  if( pCollisionModel )
  {
    gfl2::math::Vector4 newPos( trans->GetTranslation() + mv );
    MapCallbackable mapCallbackabl;
    pCollisionModel->GetHeight( newPos, NULL, &mapCallbackabl );
    f32 newHight = mapCallbackabl.GetNearHight( newPos.y );
    mv.y = newHight - newPos.y;
  }

  return SkeltonModel::Command::CreateVectorCommand( mv, dir, playerMove );
}

SkeltonModel::Command PlayerControl::CreateNetworkCommand( float dx, float dz, bool is_run )
{
  const SkeltonFieldTransform * trans = m_SMDL->GetTransform();

  if (dx == 0 && dz == 0 )
  {
    return SkeltonModel::Command::CreatePositionCommand( trans->GetTranslation(), trans->GetAngle() ); 
  }

  float dir;
  gfl2::math::Vector4 mv;
  PlayerMove playerMove = PLAYER_MOVE_WALK ;

  if( is_run )
  {
    playerMove = PLAYER_MOVE_RUN;
  }

  CalculateMoveVector( m_camera, dx, dz, mv, dir, playerMove );
  mv += trans->GetTranslation(); 

  return SkeltonModel::Command::CreatePositionCommand( mv, dir ); 
}


PlayerControl::PlayerMove PlayerControl::GetPlayerMove( f32 stickLevel )
{
  if( stickLevel > 1.0f )
  {
    stickLevel = 1.0f;
  }

  for( u32 i = 0; i < PLAYER_MOVE_MAX; i++ )
  {
    if( stickLevel <= SkeltonDebugInfo::m_PlayerMoveSettingTable[i].stickLevel )
    {
      return static_cast< PlayerMove >(i);
    }
  }

  return PLAYER_MOVE_WALK_FAST;
}

GFL_NAMESPACE_END(Skelton)
  GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_WIN32