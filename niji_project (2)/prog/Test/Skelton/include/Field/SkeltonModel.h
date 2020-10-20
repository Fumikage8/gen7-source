#if defined(GF_PLATFORM_WIN32)
#pragma once
//=================================================================================================
/**
@file SkeltonModel.h
@brief
@date 2014.02.24
@author tamada@gamefreak.co.jp

*/
//=================================================================================================

#include <math/include/gfl2_Vector4.h>
#include <math/include/gfl2_Matrix.h>

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_JointInstanceNode.h>

#include "Test/Skelton/include/Field/SkeltonModelDefine.h"
#include "System/include/nijiAllocator.h"

GFL_NAMESPACE_BEGIN(Test)
  GFL_NAMESPACE_BEGIN(Skelton)

class SkeltonFieldTransform;
class SkeltonCamera;
class SkeltonRscManager;

class gfx_impl;

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
class SkeltonModel
{
  GFL_FORBID_COPY_AND_ASSIGN(SkeltonModel);

public:
  //-------------------------------------------------------------------------------------------------
  //-------------------------------------------------------------------------------------------------
  typedef enum COMMAND_TYPE
  {
    COMMAND_TYPE_NONE,

    COMMAND_TYPE_VECTOR,
    COMMAND_TYPE_POSITION,
    COMMAND_TYPE_NETWORK,
    COMMAND_TYPE_ORIGIN_NODE_MOVE,      ///< Originノードの移動量で移動
  };

  struct Command {
    COMMAND_TYPE type;
    union DATA_UNION{
      struct VEC_DATA
      {
        f32 x, y, z;
        float m_dir;
        u8 palyerMove;
      }vec_data;
      struct POS_DATA
      {
        f32 x, y, z;
        float m_dir;
      }pos_data;
      struct NETWORK_DATA
      {
        f32 x, y, z;
        float m_dir;
        unsigned int m_net_id;
      }network_data;
    }data;

    Command();
    //Command( gfl2::math::Vector4 &vec );
    //Command( gfl2::math::Vector4 &pos, float direction );

    static Command CreateVectorCommand( gfl2::math::Vector4 &vec, float direction, u8 playerMove = 0 );
    static Command CreatePositionCommand( gfl2::math::Vector4 &pos, float direction );
    static Command CreateOriginNodeMoveCommand( gfl2::math::Vector4 &vec, float direction );
  };

public:
  SkeltonModel( gfl2::heap::HeapBase * p_heap, System::nijiAllocator* p_Allocator );
  ~SkeltonModel();

  bool Setup( SkeltonRscManager * p_rsc_man, const SkeltonModelHeader * p_header );
  bool Setup( SkeltonRscManager * p_rsc_man, const SkeltonModelHeaderEx * p_header );

  bool activate(SkeltonModelManager * p_manager);

  void Clear( void );

  bool GetExists( void ) const { return m_exists; }
  SMDL_ID GetID( void ) const { return m_id; }

  void ChangeAnimation( unsigned int id, bool loopFlag, f32 interp_frame );
  bool ChangeAnimationIfOther( u32 id, bool loopFlag, f32 interp_frame );
  f32 GetAnimationFrame( void ) const;
  f32 GetAnimationMaxFrame( void ) const;
  void SetAnimationFrame( f32 frame );
  void SetAnimationStepFrame( f32 frame );
  bool HasAnimation( u32 id ) const;
  bool IsAnimeLooped( void ) const;
  bool IsAnimeLastFrame( void ) const;

  void update( void );
  void updateAnimation( float timediff );
  void UpdateAnimation (f32 frame );
  void updateAfter( void );
  void draw( void );

  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode * GetInstance( void );

  SkeltonFieldTransform * GetTransform( void ) const;

  int GetAnimeID( void ) const { return m_animeNo; }

  void SetCommand( const Command & cmd );
  const Command & GetCommand( void ) const { return m_commandBuffer; }


  chara::RSC_ID GetResourceID( void ) const { return m_rsc_id; }
  SKELTON_CODE GetSkeltonCode( void ) const { return m_mv_code; }
  u16 GetParam0( void ) const { return m_param0; }
  u16 GetParam1( void ) const { return m_param1; }
  u16 GetParam2( void ) const { return m_param2; }


  const SkeltonModelManager * GetManager( void ) const { return m_pManager; }

  static bool IsMoveCommand( const SkeltonModel * p_model, gfl2::math::Vector4 & mv );

  // スケールテスト
  void SetWaistScale(f32 scale);
  f32 GetWaistScale();
  //void SetMotionMoveDir(const gfl2::math::Vector4& dir, f32 angle)
  //{
  //  m_MotionMoveDir = dir;
  //  m_MotionMoveAngle = angle;
  //}

  void Pause( b32 flag ){ m_PauseFlag = flag; }
  b32 IsPause( void ) const { return m_PauseFlag; }

  void SetAngleLock( b32 flag ){ m_AngleLockForMoveFlag = flag; }
  b32 IsAngleLock( void ){ return m_AngleLockForMoveFlag; }

private:

  void doCommand( void );
  void doCommandAfter( void );

private:
  SkeltonModelManager * m_pManager;
  bool m_exists;

  SMDL_ID	      m_id;
  chara::RSC_ID m_rsc_id;
  SKELTON_CODE     m_mv_code;
  u16				    m_param0;
  u16				    m_param1;
  u16				    m_param2;

  gfx_impl * m_gfx_impl;
  SkeltonFieldTransform * m_transform;

  const SkeltonCodeBase * m_SkeltonCode;
  SkeltonCodeWork	m_SkeltonCodeWork;

  bool m_animeFlag;
  int m_animeNo;
  float m_animeCounter;

  const SkeltonCamera * m_fieldCamera;

  Command m_commandBuffer;

  // スケールテスト
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode* m_pOriginNode;
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode* m_pWaistNode;
  f32 m_WaistScale;

  f32 m_MotionMoveSpeed;
  b32 m_PauseFlag;

  b32 m_AngleLockForMoveFlag;    // trueなら移動で向きが変更されない

  gfl2::heap::HeapBase* mHeap;
  System::nijiAllocator* mAllocator;
};

GFL_NAMESPACE_END(Skelton)
  GFL_NAMESPACE_END(Test)

#endif