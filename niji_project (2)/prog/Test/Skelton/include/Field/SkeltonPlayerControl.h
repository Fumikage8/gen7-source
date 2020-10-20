#if defined(GF_PLATFORM_WIN32)
#pragma once
//=================================================================================================
/**
@file player_control.h
@brief
@date 2014.03.02
@author tamada@gamefreak.co.jp
*/
//=================================================================================================


#include <math/include/gfl2_Vector4.h>
#include "Test/Skelton/include/Field/SkeltonModel.h"
#include <Collision/include/gfl2_CollisionModel.h>


GFL_NAMESPACE_BEGIN( field );
GFL_NAMESPACE_BEGIN( camera );
class SkeltonCamera;
GFL_NAMESPACE_END( camera );
GFL_NAMESPACE_END( field );

GFL_NAMESPACE_BEGIN(Test)
  GFL_NAMESPACE_BEGIN(Skelton)

class PlayerControl
{

public:

  enum PlayerMove
  {
    PLAYER_MOVE_WALK_SLOW = 0,
    PLAYER_MOVE_WALK,
    PLAYER_MOVE_WALK_FAST,
    PLAYER_MOVE_RUN,
    PLAYER_MOVE_DEBUG_RUN,

    PLAYER_MOVE_MAX,
  };

  PlayerControl();
  ~PlayerControl();

  bool activate( SkeltonModel * p_SMDL, SkeltonCamera * p_camera = NULL );
  void Update(void);

  SkeltonModel::Command CreatePCCommand( float dx, float dz, bool is_run, bool is_debug_run, gfl2::collision::CollisionModel* pCollisionModel );
  SkeltonModel::Command CreateNetworkCommand( float dx, float dz, bool is_run );

public:
  static float ChangeWalkRate( float change );
  static float ChangeMoveRate( float change );
  static float ChangeRunRate( float change );
  static float GetMoveRate( void ) {return MoveRate; }
  static float GetWalkRate( void ) {return WalkRate; }
  static float GetRunRate( void ) {return RunRate; }

  static void SetWalkSpeed( f32 speed ){ WalkSpeed = speed; };
  static void SetRunSpeed( f32 speed ){ RunSpeed = speed; };

private:
  static float WalkRate;
  static float RunRate;
  static float MoveRate;
  static float WalkSpeed;
  static float RunSpeed;
  void CalculateMoveVector( SkeltonCamera * p_camera, float dx, float dz, gfl2::math::Vector4 & mv, float & dir, PlayerMove playerMove );
  void CalculateMoveVector( SkeltonCamera * p_camera, float dx, float dz, gfl2::math::Vector4 & mv, float & dir );
  //	void Move( float dx, float dz );

  static PlayerMove GetPlayerMove( f32 stickLevel );

private:
  SkeltonModel * m_SMDL;
  SkeltonCamera * m_camera;

  gfl2::math::Matrix m_CameraMatrix;

};

GFL_NAMESPACE_END(Skelton)
  GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_WIN32
