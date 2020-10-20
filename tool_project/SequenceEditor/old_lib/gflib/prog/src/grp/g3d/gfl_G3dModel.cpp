//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dModel.cpp
 *	@brief  3D���f��
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>
#include <nw/ut.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Math.h>
#include <gfl_debug.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>
#include <grp/g3d/gfl_G3dSystem.h>
#include <grp/g3d/gfl_G3dUserData.h>

#include <grp/g3d/gfl_G3dModel.h>




#define GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD (1)  // ���b�V���̃��\�[�X�̎擾�̎d����ύX����B
                                                  // 0�̂Ƃ�nw::gfx::Model::GetResMeshes�Ŏ擾����B
                                                  // 1�̂Ƃ�nw::gfx::res::ResModel::GetMeshes�Ŏ擾����B

#if 0
/*
@note
���b�V���̃��\�[�X���牽�𓾂������ŁAGetResMeshes���g��������K�v�����肻�����B
����
GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD
�őS����؂�ւ��Ă��邪�A�擾������̂ɂ���ēK�؂ȕ����g���悤�ɂ������B

C:\home\xy_ctrlib\NintendoWare\CTR\documents\API\nw\gfx\Model\GetResMeshes.html
nw::gfx::Model::GetResMeshes �����o�֐�
���b�V���̃��\�[�X�̔z����擾���܂��B
���b�V���̃��\�[�X�̓��f�����\�[�X����擾����̂ł͂Ȃ��A ���̃��\�b�h���g���Ď擾����悤�ɂ��Ă��������B

[����] ���b�V���̕`��D��x�𓮓I�ɕύX����ɂ́H
�c����NTSC 2010-08-18 19:48:37  
ResModel����ResMesh���擾����Ă���̂������ł��B
���ۂ̕`��ł�ResModel��ResMesh�ł͂Ȃ��A
nw::gfx::Model::GetResMeshes�Ŏ擾�ł���ResMesh�̔z�񂪎g�p����Ă��܂��B
���ׁ̈A��������擾����悤�ɂ��ĕ`��D��x��ύX���Ă��������B 
*/
#endif


namespace gfl {
namespace grp {
namespace g3d {


namespace internal {

// NintendoWare/CTR/tools/Viewer/
// NodeConstraint.h
// NodeConstraint.cpp
// ProtocolEM4C.h  // enum FollowBitType // Flag�p�r�b�g��`�i=IsContinue|IsFollowScale�̂悤�Ɏg�p���܂��j  // FollowBitType_None = 0x00,           // ���ݒ�
// ParamSetter.h   // �Z��ParticleEmitter�̐ݒ�  // constraint.m_UseOffset[0] = true;

/*---------------------------------------------------------------------------*
  Name:         
  
  Description: 
  
  Arguments:    None.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MultScale(nw::math::MTX34* dstMatrix, const nw::math::MTX34* srcMatrix, const nw::math::VEC3& scale)
{
    dstMatrix->f._00 = srcMatrix->f._00 * scale.x;
    dstMatrix->f._10 = srcMatrix->f._10 * scale.x;
    dstMatrix->f._20 = srcMatrix->f._20 * scale.x;

    dstMatrix->f._01 = srcMatrix->f._01 * scale.y;
    dstMatrix->f._11 = srcMatrix->f._11 * scale.y;
    dstMatrix->f._21 = srcMatrix->f._21 * scale.y;

    dstMatrix->f._02 = srcMatrix->f._02 * scale.z;
    dstMatrix->f._12 = srcMatrix->f._12 * scale.z;
    dstMatrix->f._22 = srcMatrix->f._22 * scale.z;
}

/*---------------------------------------------------------------------------*
  Name:         
  
  Description: 
  
  Arguments:    None.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void CopyRotate(nw::math::MTX34* dstMatrix, const nw::math::MTX34& srcMatrix)
{
    dstMatrix->f._00 = srcMatrix.f._00;
    dstMatrix->f._10 = srcMatrix.f._10;
    dstMatrix->f._20 = srcMatrix.f._20;

    dstMatrix->f._01 = srcMatrix.f._01;
    dstMatrix->f._11 = srcMatrix.f._11;
    dstMatrix->f._21 = srcMatrix.f._21;

    dstMatrix->f._02 = srcMatrix.f._02;
    dstMatrix->f._12 = srcMatrix.f._12;
    dstMatrix->f._22 = srcMatrix.f._22;
}

/*---------------------------------------------------------------------------*
  Name:         
  
  Description: 
  
  Arguments:    None.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void CopyTranslate(nw::math::MTX34* dstMatrix, const nw::math::MTX34& srcMatrix)
{
    dstMatrix->f._03 = srcMatrix.f._03;
    dstMatrix->f._13 = srcMatrix.f._13;
    dstMatrix->f._23 = srcMatrix.f._23;
}

/*---------------------------------------------------------------------------*
  Name:         
  
  Description: 
  
  Arguments:    None.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void
CalculateWorldMatrixOfSkeleton(
    nw::math::MTX34* transformMatrix,
    nw::math::VEC3* scale,
    const nw::gfx::CalculatedTransform& localTransform,
    const nw::gfx::CalculatedTransform& parentWorldTransform,
    const nw::gfx::CalculatedTransform& parentLocalTransform,
    const bool inheritTransform[ 3 ]
) {
    GFL_ASSERT_STOP_MSG(transformMatrix, "transformMatrix��NULL�ł��B\n" );

    const nw::math::MTX34& parentMatrix = parentWorldTransform.TransformMatrix();  // ���W�ϊ���\���s����擾���܂��B �����̃X�P�[���͊|�����Ă��炸�A�e�̃X�P�[���͊|�����Ă��܂�
    nw::math::VEC3 localTranslate = localTransform.TransformMatrix().GetColumn(3);

    // TR(i) = TR(i-1) * S(i-1) * t(i) * r(i)
    nw::math::MTX34Identity(transformMatrix);

    if (inheritTransform[ 1 ]) CopyRotate(transformMatrix, parentMatrix);
    if (inheritTransform[ 2 ]) CopyTranslate(transformMatrix, parentMatrix);
    if (inheritTransform[ 0 ]) {
        MultScale(transformMatrix, transformMatrix, parentLocalTransform.Scale());  // �X�P�[�����O�ϊ��x�N�g�����擾���܂�
        nw::math::VEC3Mult(scale, &parentWorldTransform.Scale(), &localTransform.Scale());  // �X�P�[�����O�ϊ��x�N�g�����擾���܂�
    } else {
        scale->Set(localTransform.Scale());
    }
    nw::math::MTX34MultTranslate(transformMatrix, transformMatrix, &localTranslate);
    nw::math::MTX33Mult(transformMatrix, transformMatrix, &localTransform.TransformMatrix());
}

void
CalculateWorldMatrixOfTransformNode(
    nw::math::MTX34* transformMatrix,
    nw::math::VEC3* scale,
    const nw::gfx::CalculatedTransform& localTransform,
    const nw::gfx::CalculatedTransform& parentWorldTransform,
    const bool inheritTransform[ 3 ]
) {
    GFL_ASSERT_STOP_MSG(transformMatrix, "transformMatrix��NULL�ł��B\n" );

    const nw::math::MTX34& parentMatrix = parentWorldTransform.TransformMatrix();  // ���W�ϊ���\���s����擾���܂��B �����̃X�P�[���͊|�����Ă��炸�A�e�̃X�P�[���͊|�����Ă��܂�
    nw::math::VEC3 localTranslate = localTransform.TransformMatrix().GetColumn(3);

    // TR(i) = TR(i-1) * S(i-1) * t(i) * r(i)
    nw::math::MTX34Identity(transformMatrix);

    if (inheritTransform[ 1 ]) CopyRotate(transformMatrix, parentMatrix);
    if (inheritTransform[ 2 ]) CopyTranslate(transformMatrix, parentMatrix);
    if (inheritTransform[ 0 ]) {
        nw::math::VEC3Mult(scale, &parentWorldTransform.Scale(), &localTransform.Scale());  // �X�P�[�����O�ϊ��x�N�g�����擾���܂�
    } else {
        scale->Set(localTransform.Scale());
    }
    nw::math::MTX34MultTranslate(transformMatrix, transformMatrix, &localTranslate);
    nw::math::MTX33Mult(transformMatrix, transformMatrix, &localTransform.TransformMatrix());
}


//-------------------------------------
///	�R�[���o�b�N�N���X�Ŏg�p����\����
//=====================================
struct ConstraintBaton
{
  gfl::grp::g3d::SceneNode*  baton_scene_node;
  b32                        inherit_scale;      // true�̂Ƃ��X�P�[���̒Ǐ]����
  b32                        inherit_rotate;     // true�̂Ƃ���]�̒Ǐ]����
  b32                        inherit_translate;  // true�̕��s�ړ��̒Ǐ]����
  // @note
  // constraint���ɌĂ΂��R�[���o�b�N�ł������s����
  // �����ւ�replace���ɌĂ΂��R�[���o�b�N�ł������s����
  // �����̃R�[���o�b�N�Ŏ��s����
  // ���̎w�肪�ł���悤�ɂ�����
};
struct ConstraintBatons
{
  u32                         baton_num_max;  // baton_array�̗v�f��
  u32                         baton_num;      // baton_array�Ŏ��ۂɒl�������Ă�����B
                                              // baton_array�͏�ɑO�ɋl�߂Ă����̂�0<= <baton_num���������΂悢�B
  ConstraintBaton*            baton_array;

  // �ǉ��ł�����true��Ԃ�
  // ���X�ǉ�����Ă���V���ɒǉ��ł��Ȃ��Ƃ���GFL_ASSERT_MSG������false��Ԃ�  //@check �����ɂЂ�������Ȃ��悤�Ɋm�F�ς̈�������Ă���
  b32 AddBatonSceneNode(
    gfl::grp::g3d::SceneNode* baton_scene_node,
    const b32  inherit_scale,
    const b32  inherit_rotate,
    const b32  inherit_translate
  )
  {
    b32 ret = false;
    b32 exist = false;
    u32 i;
    for( i=0; i<baton_num; ++i )
    {
      ConstraintBaton* baton = &baton_array[i];
      if( baton->baton_scene_node == baton_scene_node )
      {
        exist = true;
        break;
      }
    }
    if( exist )
    {
      GFL_ASSERT_MSG(0, "���ɒǉ�����Ă���̂ŐV���ɒǉ��ł��܂���B\n");
      ret = false;
    }
    else
    {
      if(baton_num < baton_num_max)
      {
        ConstraintBaton* baton   = &baton_array[baton_num];
        baton->baton_scene_node  = baton_scene_node;
        baton->inherit_scale     = inherit_scale; 
        baton->inherit_rotate    = inherit_rotate;   
        baton->inherit_translate = inherit_translate;
        ++baton_num;
        ret = true;
      }
      else
      {
        GFL_ASSERT_MSG(0, "baton_array�̗v�f��%d���z�����̂Œǉ��ł��܂���B\n", baton_num_max);
        ret = false;
      }
    }
    return ret;
  }

  // �ύX�ł�����true��Ԃ�
  // ���X�ǉ�����Ă��Ȃ��Ƃ���GFL_ASSERT_MSG������false��Ԃ�  //@check �����ɂЂ�������Ȃ��悤�Ɋm�F�ς̈�������Ă���
  b32 ChangeBatonSceneNodeInherit(
    gfl::grp::g3d::SceneNode* baton_scene_node,
    const b32  inherit_scale,
    const b32  inherit_rotate,
    const b32  inherit_translate
  )
  {
    b32 ret = false;
    b32 exist = false;
    u32 i;
    for( i=0; i<baton_num; ++i )
    {
      ConstraintBaton* baton = &baton_array[i];
      if( baton->baton_scene_node == baton_scene_node )
      {
        exist = true;
        break;
      }
    }
    if( exist )
    {
      ConstraintBaton* baton   = &baton_array[i];
      baton->inherit_scale     = inherit_scale; 
      baton->inherit_rotate    = inherit_rotate;   
      baton->inherit_translate = inherit_translate;
      ret = true;
    }
    else
    {
      GFL_ASSERT_MSG(0, "���̃V�[���m�[�h�͒ǉ�����Ă��Ȃ��̂ŒǏ]��ύX�ł��܂���B\n");
      ret = false;
    }
    return ret;
  }

  // �폜�ł��Ȃ��Ƃ���false��Ԃ�
  // ���X�ǉ�����Ă��Ȃ��Ƃ���GFL_ASSERT_MSG������false��Ԃ�  //@check �����ɂЂ�������Ȃ��悤�Ɋm�F�ς̈�������Ă���
  b32 RemoveBatonSceneNode(
    SceneNode* baton_scene_node
  )
  {
    b32 ret = false;
    b32 exist = false;
    u32 i;
    for( i=0; i<baton_num; ++i )
    {
      ConstraintBaton* baton = &baton_array[i];
      if( baton->baton_scene_node == baton_scene_node )
      {
        exist = true;
        break;
      }
    }
    if( exist )
    {
      for( u32 j=i; j<baton_num-1; ++j )
      {
        baton_array[j] = baton_array[j+1];
      }
      baton_array[baton_num-1].baton_scene_node = NULL;
      --baton_num;
      ret = true;
    }
    else
    {
      GFL_ASSERT_MSG(0, "���̃V�[���m�[�h�͒ǉ�����Ă��Ȃ��̂ō폜�ł��܂���B\n");
      ret = false;
    }
    return ret;
  }
};
struct ConstraintHand
{
  s32               hand_bone_index;
  ConstraintBatons  batons;  // hand�{�[���ɂ�������baton
};
struct ConstraintHumanOfSkeleton
{
  nw::gfx::Skeleton*        human_skeleton;
  u32                       hand_num_max;  // hand_array�̗v�f��
  u32                       hand_num;      // hand_array�Ŏ��ۂɒl�������Ă�����B
                                           // hand_array�͏�ɑO�ɋl�߂Ă����̂�0<= <hand_num���������΂悢�B
  ConstraintHand*           hand_array;

  // �ǉ��ł�����true��Ԃ�
  // ���X�ǉ�����Ă���V���ɒǉ��ł��Ȃ��Ƃ���GFL_ASSERT_MSG������false��Ԃ�  //@check �����ɂЂ�������Ȃ��悤�Ɋm�F�ς̈�������Ă���
  b32 AddBatonSceneNode(
    gfl::grp::g3d::SceneNode* baton_scene_node,
    const s32  hand_bone_index,
    const b32  inherit_scale,
    const b32  inherit_rotate,
    const b32  inherit_translate
  )
  {
    b32 ret = false;
    b32 exist = false;
    u32 h;
    for( h=0; h<hand_num; ++h )
    {
      ConstraintHand* hand = &hand_array[h];
      if( hand->hand_bone_index == hand_bone_index )
      {
        exist = true;
        break;
      }
    }
    if( exist )
    {
      ConstraintHand* hand = &hand_array[h];
      ret = hand->batons.AddBatonSceneNode(
        baton_scene_node,
        inherit_scale,
        inherit_rotate,
        inherit_translate
      );
    }
    else
    {
      if(hand_num < hand_num_max)
      {
        ConstraintHand* hand = &hand_array[hand_num];
        ret = hand->batons.AddBatonSceneNode(
          baton_scene_node,
          inherit_scale,
          inherit_rotate,
          inherit_translate
        );
        if(ret)
        {
          hand->hand_bone_index = hand_bone_index;
          ++hand_num;
        }
      }
      else
      {
        GFL_ASSERT_MSG(0, "hand_array�̗v�f��%d���z�����̂Œǉ��ł��܂���B\n", hand_num_max);
        ret = false;
      }
    }
    return ret;
  }

  // �ύX�ł�����true��Ԃ�
  // ���X�ǉ�����Ă��Ȃ��Ƃ���GFL_ASSERT_MSG������false��Ԃ�  //@check �����ɂЂ�������Ȃ��悤�Ɋm�F�ς̈�������Ă���
  b32 ChangeBatonSceneNodeInherit(
    gfl::grp::g3d::SceneNode* baton_scene_node,
    const s32  hand_bone_index,
    const b32  inherit_scale,
    const b32  inherit_rotate,
    const b32  inherit_translate
  )
  {
    b32 ret = false;
    b32 exist = false;
    u32 h;
    for( h=0; h<hand_num; ++h )
    {
      ConstraintHand* hand = &hand_array[h];
      if( hand->hand_bone_index == hand_bone_index )
      {
        exist = true;
        break;
      }
    }
    if( exist )
    {
      ConstraintHand* hand = &hand_array[h];
      ret = hand->batons.ChangeBatonSceneNodeInherit(
        baton_scene_node,
        inherit_scale,
        inherit_rotate,
        inherit_translate
      );
    }
    else
    {
      GFL_ASSERT_MSG(0, "���̃V�[���m�[�h�͒ǉ�����Ă��Ȃ��̂ŒǏ]��ύX�ł��܂���B\n");
      ret = false;
    }
    return ret;
  }

  // �폜�ł��Ȃ��Ƃ���false��Ԃ�
  // ���X�ǉ�����Ă��Ȃ��Ƃ���GFL_ASSERT_MSG������false��Ԃ�  //@check �����ɂЂ�������Ȃ��悤�Ɋm�F�ς̈�������Ă���
  b32 RemoveBatonSceneNode(
    SceneNode* baton_scene_node,
    const s32  hand_bone_index
  )
  {
    b32 ret = false;
    b32 exist = false;
    u32 h;
    for( h=0; h<hand_num; ++h )
    {
      ConstraintHand* hand = &hand_array[h];
      if( hand->hand_bone_index == hand_bone_index )
      {
        exist = true;
        break;
      }
    }
    if( exist )
    {
      ConstraintHand* hand = &hand_array[h];
      ret = hand->batons.RemoveBatonSceneNode(
        baton_scene_node
      );
      if( ret )
      {
        if( hand->batons.baton_num == 0 )
        {
          // ����hand�̑Sbaton���Ȃ��Ȃ����̂�hand���̂��Ȃ���
          b32 remove_hand = true;
          u32 g;
          for( g=h; g<hand_num-1; ++g )
          {
            {
              // hand_array[g]��hand_array[g+1]���R�s�[����
              if( hand_array[g].batons.baton_num_max == hand_array[g+1].batons.baton_num_max )
              {
                hand_array[g].hand_bone_index      = hand_array[g+1].hand_bone_index;
                //hand_array[g].batons.baton_num_max == hand_array[g+1].batons.baton_num_max; �ύX�����Ȃ̂ŃR�����g�A�E�g
                hand_array[g].batons.baton_num     = hand_array[g+1].batons.baton_num;
                for( u32 i=0; i<hand_array[g].batons.baton_num; ++i )
                {
                  hand_array[g].batons.baton_array[i] = hand_array[g+1].batons.baton_array[i];
                }
              }
              else
              {
                remove_hand = false;
                break;
              }
            }
          }
          {
            // g��(hand_num-1)�Ƃ͌���Ȃ��Bg��(hand_num-1)�ł��������A�r���ł������B
            // hand_array[g]��hand_array[g-1]�ɃR�s�[���Ă���̂ŁAhand_array[g]����ɂ���B
            // g=h�̂Ƃ���hand_array[g]����ɂ��邾���Ȃ̂Ŗ��Ȃ��B
            hand_array[g].hand_bone_index      = gfl::grp::g3d::Model::BONE_INDEX_NONE;
            //hand_array[g].batons.baton_num_max == ; �ύX�����Ȃ̂ŃR�����g�A�E�g
            for( u32 i=0; i<hand_array[g].batons.baton_num; ++i )
            {
              hand_array[g].batons.baton_array[i].baton_scene_node = NULL;
            }
            hand_array[g].batons.baton_num = 0;
          }
          if( remove_hand )
          {
            --hand_num;
          }
        }
      }
    }
    else
    {
      GFL_ASSERT_MSG(0, "���̃V�[���m�[�h�͒ǉ�����Ă��Ȃ��̂ō폜�ł��܂���B\n");
      ret = false;
    }
    return ret;
  }
};
struct ConstraintHumanOfTransformNode
{
  nw::gfx::TransformNode*  human_transform_node;
  ConstraintBatons         batons;  // human�g�b�v�ɂ�������baton
};

#if 0
/*
NintendoWare/CTR/include/nw/gfx/gfx_SceneNode.h
    typedef ut::Signal2<void, SceneNode*, SceneContext*> UpdateSignal;
NintendoWare/CTR/include/nw/gfx/gfx_TransformNode.h
    typedef ut::Signal2<void, TransformNode*, SceneContext*> CalculateMatrixSignal;
NintendoWare/CTR/include/nw/gfx/gfx_Skeleton.h
    typedef ut::Signal2<void, Skeleton*, int> CalculateMatrixSignal;

NintendoWare/CTR/include/nw/ut/ut_Signal.h
NintendoWare/CTR/include/nw/ut/generated/ut_Signal.hi
    //! "nw/ut/ut_Config.h"��"NW_SIGNAL_MULTI_SLOT_ENABLED"��`�������̏ꍇ�A
    //! �V�O�i���ɐڑ��ł���X���b�g���͂P�ɂȂ�܂��B
    //! 
    //! "NW_SIGNAL_MULTI_SLOT_ENABLED"��`�������̏ꍇ�A
    //! Connect ����O�ɕK�� Disconnect ����悤�ɂ��Ă��������B
    //! Disconnect ���Ă��Ȃ��ꍇ�́A�A�T�[�g�Œ�~���܂��B

��
NW_SIGNAL_MULTI_SLOT_ENABLED��`���L���̏ꍇ�A
1�{�[�����ƂɃR�[���o�b�N��p�ӂ��A1�X�P���g���ɕ����̃R�[���o�b�N��n�����Ƃ��ł��邪�A
NW_SIGNAL_MULTI_SLOT_ENABLED��`�������̏ꍇ�A
1�X�P���g����1�R�[���o�b�N�����n���Ȃ��B
�����NW_SIGNAL_MULTI_SLOT_ENABLED��`�������̏ꍇ�ɑΉ��ł���悤�Ȃ���ɂ����B
*/
#endif 


//-------------------------------------
///	�R�[���o�b�N�N���X
//=====================================

/*!--------------------------------------------------------------------------*
  @brief        �R�[���o�b�N�R���X�g���C���̊֐��I�u�W�F�N�g�ł��B

  ���̃R�[���o�b�N�ɂ��ẮA�֐����t�@�����X�́u���x�ȋ@�\�v���������������B
 *---------------------------------------------------------------------------*/
class CallbackConstraintOfSkeleton : public nw::gfx::Skeleton::CalculateMatrixSlot
{
public:
  //! @brief �R���X�g���N�^�ł��B
  CallbackConstraintOfSkeleton(
    gfl::heap::NwAllocator*  heap_allocator,
    gfl::heap::NwAllocator*  device_allocator,
    nw::gfx::Skeleton*       human_skeleton,
    u32                      hand_num_max,       // ����������hand�̍ő吔(���̃��f���̃{�[���̌��ȉ�)
    u32                      hand_baton_num_max  // 1��hand�ɍS�������baton�̍ő吔
  )
    : nw::gfx::Skeleton::CalculateMatrixSlot(device_allocator)
  {
    GFL_ASSERT_STOP_MSG( heap_allocator, "heap_allocator��NULL�ł��B\n" );
    GFL_ASSERT_STOP_MSG( device_allocator, "device_allocator��NULL�ł��B\n" );
    GFL_ASSERT_STOP_MSG( human_skeleton, "human_skeleton��NULL�ł��B\n" );
    GFL_ASSERT_STOP_MSG( hand_num_max > 0, "hand_num_max=%d��0����łȂ���΂Ȃ�܂���B\n", hand_num_max );
    GFL_ASSERT_STOP_MSG( hand_baton_num_max > 0, "hand_baton_num_max=%d��0����łȂ���΂Ȃ�܂���B\n", hand_baton_num_max );

    m_constraint.human_skeleton = human_skeleton;
    m_constraint.hand_array     = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) ConstraintHand[hand_num_max];
    m_constraint.hand_num_max   = hand_num_max;
    m_constraint.hand_num       = 0;

    for( u32 h=0; h<hand_num_max; ++h )
    {
      m_constraint.hand_array[h].batons.baton_array   = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) ConstraintBaton[hand_baton_num_max];
      m_constraint.hand_array[h].batons.baton_num_max = hand_baton_num_max;
      m_constraint.hand_array[h].batons.baton_num     = 0;
    }

    // �֐��I�u�W�F�N�g���V�O�i���ɐڑ����܂��B
    m_constraint.human_skeleton->PostCalculateMatrixSignal().Connect(this);
  }

  // �f�X�g���N�^
  virtual ~CallbackConstraintOfSkeleton()
  {
    for( u32 h=0; h<m_constraint.hand_num; ++h )
    {
      for( u32 i=0; i<m_constraint.hand_array[h].batons.baton_num; ++i )
      {
        m_constraint.hand_array[h].batons.baton_array[i].baton_scene_node->CleanupConstraint();
      }
    }

    m_constraint.human_skeleton->PostCalculateMatrixSignal().Disconnect(this);

    for( u32 h=0; h<m_constraint.hand_num_max; ++h )
    {
      if( m_constraint.hand_array[h].batons.baton_array )
      {
        GFL_DELETE_ARRAY m_constraint.hand_array[h].batons.baton_array;
        m_constraint.hand_array[h].batons.baton_array = NULL;
      }
    }

    if( m_constraint.hand_array )
    {
      GFL_DELETE_ARRAY m_constraint.hand_array;
      m_constraint.hand_array = NULL;
    }
  }

  //! @brief �S�������I�u�W�F�N�g��ǉ����܂��B
  bool AddBatonSceneNode(
      gfl::grp::g3d::SceneNode* baton_scene_node,
      const s32  hand_bone_index,
      const b32  inherit_scale,
      const b32  inherit_rotate,
      const b32  inherit_translate
  )
  {
    return m_constraint.AddBatonSceneNode(
      baton_scene_node,
      hand_bone_index,
      inherit_scale,
      inherit_rotate,
      inherit_translate
    );
  }
  // �S�������I�u�W�F�N�g�̒Ǐ]��ύX����
  bool ChangeBatonSceneNodeInherit(
      gfl::grp::g3d::SceneNode* baton_scene_node,
      const s32  hand_bone_index,
      const b32  inherit_scale,
      const b32  inherit_rotate,
      const b32  inherit_translate
  )
  {
    return m_constraint.ChangeBatonSceneNodeInherit(
      baton_scene_node,
      hand_bone_index,
      inherit_scale,
      inherit_rotate,
      inherit_translate
    );
  }
  // �S�������I�u�W�F�N�g���폜���܂��B
  bool RemoveBatonSceneNode(
      gfl::grp::g3d::SceneNode* baton_scene_node,
      const s32  hand_bone_index
  )
  {
    return m_constraint.RemoveBatonSceneNode(
      baton_scene_node,
      hand_bone_index
    );
  }

  //! @brief �R�[���o�b�N�֐��ł��B
  virtual void Invoke(nw::gfx::Skeleton* skeleton, int boneIdx)
  {
    if( skeleton != m_constraint.human_skeleton )
    {
      return;
    }

    for( u32 h=0; h<m_constraint.hand_num; ++h )
    {
      if( m_constraint.hand_array[h].hand_bone_index == boneIdx )
      {
        for( u32 i=0; i<m_constraint.hand_array[h].batons.baton_num; ++i )
        {
          ConstraintBaton*        baton = &m_constraint.hand_array[h].batons.baton_array[i];
          nw::gfx::TransformNode* baton_nw_transform_node = baton->baton_scene_node->GetNwTransformNode();
      
          bool l_InheritTransform[ 3 ] = { baton->inherit_scale, baton->inherit_rotate, baton->inherit_translate };

          nw::gfx::CalculatedTransform* localPose = skeleton->LocalTransformPose().GetTransform(boneIdx);
          nw::gfx::CalculatedTransform* worldPose = skeleton->WorldTransformPose().GetTransform(boneIdx);

          // �I�t�Z�b�g�t�����[���h�ϊ��s��̌v�Z
          CalculateWorldMatrixOfSkeleton(
              &baton_nw_transform_node->Transform().DirectTransformMatrix(),
              &baton_nw_transform_node->Transform().DirectScale(),
              *(baton->baton_scene_node->GetConstraintNwCalculatedTransform()),
              *worldPose,
              *localPose,
              l_InheritTransform
          );
 
          // �L���b�V���𖳌���
          baton_nw_transform_node->Transform().EnableFlags(
              nw::gfx::CalculatedTransform::FLAG_IS_DIRTY
          );
        }
        break;  // �����{�[���͂Ȃ��̂�break
      }
    }
  }

private:
  ConstraintHumanOfSkeleton  m_constraint;
};

class CallbackConstraintOfTransformNode : public nw::gfx::TransformNode::CalculateMatrixSlot
{
public:
  //! @brief �R���X�g���N�^�ł��B
  CallbackConstraintOfTransformNode(
    gfl::heap::NwAllocator*  heap_allocator,
    gfl::heap::NwAllocator*  device_allocator,
    nw::gfx::TransformNode*  human_transform_node,
    u32                      human_baton_num_max  // human�ɍS�������baton�̍ő吔
  )
    : nw::gfx::TransformNode::CalculateMatrixSlot(device_allocator)
  {
    GFL_ASSERT_STOP_MSG( heap_allocator, "heap_allocator��NULL�ł��B\n" );
    GFL_ASSERT_STOP_MSG( device_allocator, "device_allocator��NULL�ł��B\n" );
    GFL_ASSERT_STOP_MSG( human_transform_node, "human_transform_node��NULL�ł��B\n" );
    GFL_ASSERT_STOP_MSG( human_baton_num_max > 0, "human_baton_num_max=%d��0����łȂ���΂Ȃ�܂���B\n", human_baton_num_max );

    m_constraint.human_transform_node = human_transform_node;
    m_constraint.batons.baton_array   = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) ConstraintBaton[human_baton_num_max];
    m_constraint.batons.baton_num_max = human_baton_num_max;
    m_constraint.batons.baton_num     = 0;

    // �֐��I�u�W�F�N�g���V�O�i���ɐڑ����܂��B
    m_constraint.human_transform_node->PostCalculateWorldMatrixSignal().Connect(this);
  }

  // �f�X�g���N�^
  virtual ~CallbackConstraintOfTransformNode()
  {
    for( u32 i=0; i<m_constraint.batons.baton_num; ++i )
    {
      m_constraint.batons.baton_array[i].baton_scene_node->CleanupConstraint();
    }
    
    m_constraint.human_transform_node->PostCalculateWorldMatrixSignal().Disconnect(this);

    if( m_constraint.batons.baton_array )
    {
      GFL_DELETE_ARRAY m_constraint.batons.baton_array;
      m_constraint.batons.baton_array = NULL;
    }
  }

  //! @brief �S�������I�u�W�F�N�g��ǉ����܂��B
  bool AddBatonSceneNode(
      gfl::grp::g3d::SceneNode* baton_scene_node,
      const b32  inherit_scale,
      const b32  inherit_rotate,
      const b32  inherit_translate
  )
  {
    return m_constraint.batons.AddBatonSceneNode(
      baton_scene_node,
      inherit_scale,
      inherit_rotate,
      inherit_translate
    );
  }
  // �S�������I�u�W�F�N�g�̒Ǐ]��ύX����
  bool ChangeBatonSceneNodeInherit(
      gfl::grp::g3d::SceneNode* baton_scene_node,
      const b32  inherit_scale,
      const b32  inherit_rotate,
      const b32  inherit_translate
  )
  {
    return m_constraint.batons.ChangeBatonSceneNodeInherit(
      baton_scene_node,
      inherit_scale,
      inherit_rotate,
      inherit_translate
    );
  }
  // �S�������I�u�W�F�N�g���폜���܂��B
  bool RemoveBatonSceneNode(
      gfl::grp::g3d::SceneNode* baton_scene_node
  )
  {
    return m_constraint.batons.RemoveBatonSceneNode(
      baton_scene_node
    );
  }

  //! @brief �R�[���o�b�N�֐��ł��B
  virtual void Invoke(nw::gfx::TransformNode* transformNode, nw::gfx::SceneContext* sceneContext)
  {
    GFL_UNUSED(sceneContext);

    if( transformNode != m_constraint.human_transform_node )
    {
      return;
    }

    for( u32 i=0; i<m_constraint.batons.baton_num; ++i )
    {
      ConstraintBaton*        baton = &m_constraint.batons.baton_array[i];
      nw::gfx::TransformNode* baton_nw_transform_node = baton->baton_scene_node->GetNwTransformNode();
      
      bool l_InheritTransform[ 3 ] = { baton->inherit_scale, baton->inherit_rotate, baton->inherit_translate };

      nw::gfx::CalculatedTransform worldPose = transformNode->WorldTransform();

      // �I�t�Z�b�g�t�����[���h�ϊ��s��̌v�Z
      CalculateWorldMatrixOfTransformNode(
          &baton_nw_transform_node->Transform().DirectTransformMatrix(),
          &baton_nw_transform_node->Transform().DirectScale(),
          *(baton->baton_scene_node->GetConstraintNwCalculatedTransform()),
          worldPose,
          l_InheritTransform
      );

/*
      GFL_PRINT(
          "[%d] (%f, %f, %f), (%f, %f, %f)\n",
          i,
          baton_nw_transform_node->Transform().DirectTransformMatrix().f._03,
          baton_nw_transform_node->Transform().DirectTransformMatrix().f._13,
          baton_nw_transform_node->Transform().DirectTransformMatrix().f._23,
          worldPose.TransformMatrix().f._03,
          worldPose.TransformMatrix().f._13,
          worldPose.TransformMatrix().f._23
      );
*/

      // �L���b�V���𖳌���
      baton_nw_transform_node->Transform().EnableFlags(
          nw::gfx::CalculatedTransform::FLAG_IS_DIRTY
      );
    }
  }

private:
  ConstraintHumanOfTransformNode  m_constraint;
};


// �R�[���o�b�N�N���X�̐���
void CreateCallbackConstraint(
    CallbackConstraintOfSkeleton**      callback_constraint_of_skeleton,        // [out]
    CallbackConstraintOfTransformNode** callback_constraint_of_transform_node,  // [out]
    gfl::heap::NwAllocator*  heap_allocator,
    gfl::heap::NwAllocator*  device_allocator,
    nw::gfx::Model*          human_scene_node,
    u32                      human_baton_num_max,  // human�ɍS�������baton�̍ő吔
    u32                      hand_num_max,         // ����������hand�̍ő吔(human_scene_node�̃{�[���̌��ȉ�)
    u32                      hand_baton_num_max    // 1��hand�ɍS�������baton�̍ő吔
                                                   // (���vhand_num_max*hand_baton_num_max�̃V�[���m�[�h���{�[���ɂ���������)
)
{
  *callback_constraint_of_skeleton = NULL;
  *callback_constraint_of_transform_node = NULL;
  
  if( hand_num_max > 0 && hand_baton_num_max > 0 )
  {
    nw::gfx::SkeletalModel* skeletal_model = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(human_scene_node);
    if( skeletal_model ) 
    {
      nw::gfx::Skeleton* skeleton = skeletal_model->GetSkeleton(); 
      CallbackConstraintOfSkeleton* l_callback_constraint_of_skeleton = GFL_NEW(heap_allocator->GetHeapBase()) CallbackConstraintOfSkeleton(
          heap_allocator,
          device_allocator,
          skeleton,
          hand_num_max,
          hand_baton_num_max
      );
      *callback_constraint_of_skeleton = l_callback_constraint_of_skeleton;
    }
  }

  if( human_baton_num_max > 0 )
  {
    CallbackConstraintOfTransformNode* l_callback_constraint_of_transform_node = GFL_NEW(heap_allocator->GetHeapBase()) CallbackConstraintOfTransformNode(
        heap_allocator,
        device_allocator,
        human_scene_node,
        human_baton_num_max
    );
    *callback_constraint_of_transform_node = l_callback_constraint_of_transform_node;
  }
}

// �R�[���o�b�N�N���X�̔j��
void DestroyCallbackConstraint(
    CallbackConstraintOfSkeleton**      callback_constraint_of_skeleton,       // [in,out]
    CallbackConstraintOfTransformNode** callback_constraint_of_transform_node  // [in,out]
)
{
  if( *callback_constraint_of_transform_node )
  {
    GFL_DELETE (*callback_constraint_of_transform_node);
    *callback_constraint_of_transform_node = NULL;
  }
  if( *callback_constraint_of_skeleton )
  {
    GFL_DELETE (*callback_constraint_of_skeleton);
    *callback_constraint_of_skeleton = NULL;
  }
}


}  // namespace internal


//-----------------------------------------------------------------------------
/**
 *					�N���X��`
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3D���f���N���X
//=====================================

#if GFL_DEBUG
  //----------------------------------------------------------------
  //
  // ���S�Ƀ��f��������ł��������m�F����@�\�̗L�������ݒ�
  //
  //----------------------------------------------------------------
  b32 Model::s_SafeDeleteCheckEnable = false;
  b32 Model::s_SafeDeleteCheckAssert = false;
#endif    


// �ݒ���e
void Model::InitializeDescriptionForDefault(
    Description*  description
)
{
  description->material_buffer_option = MATERIAL_BUFFER_OPTION_NOT_USE;

  description->material_share_source_model             = NULL;
  description->mesh_node_visibility_share_source_model = NULL;

  description->max_callbacks            = 4;
  
  description->constraint_human_baton_num_max  = 0;
  description->constraint_hand_num_max         = 0;
  description->constraint_hand_baton_num_max   = 0;

  description->animation_slot_num       = 0;
  description->animation_blend_unit_num = 0;
  description->animation_num_in_unit = 0;

  description->calculate_callback_num_max   = 1;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �R���X�g���N�^
 *                 Create�͌Ă΂�܂���B
 */
//-----------------------------------------------------------------------------
Model::Model(void)
  : SceneNode(),
    //m_tree_flag(true),
    m_animation_slot_num(0),
    //m_animation_slot_array(NULL),
    m_animation_blend_unit_num(0),
    m_animation_num_in_unit(0),
    m_animation_manager(NULL),
    m_skeleton_share_type(SHARE_TYPE_NONE),
    m_material_share_type(SHARE_TYPE_NONE),
    m_mesh_node_visibility_share_type(SHARE_TYPE_NONE),
    m_material_buffer_option(MATERIAL_BUFFER_OPTION_NOT_USE),
    m_calculate_callback_num_max(0),
    m_calculate_callback_function_array(NULL),
    m_calculate_callback_work_array(NULL),
    m_callback_constraint_of_skeleton(NULL),
    m_callback_constraint_of_transform_node(NULL),
    m_replace_bone_data_array(NULL)
{
  STATIC_ASSERT(USER_DATA_INDEX_NOT_FOUND==-1);
  STATIC_ASSERT(BONE_INDEX_NOT_FOUND==-1);

  GFL_SINGLETON_INSTANCE(System)->GetModelManager()->AddObject(this);
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �f�X�g���N�^
 *                 Destroy���Ăт܂��B
 */
//-----------------------------------------------------------------------------
Model::~Model()
{
  Destroy();
  GFL_SINGLETON_INSTANCE(System)->GetModelManager()->RemoveObject(this);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ����
 *
 *  @param[in]     allocator          �A���P�[�^
 *  @param[in]     device_allocator   �f�o�C�X�������̃A���P�[�^
 *  @param[in]     resource           ���\�[�X
 *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
 *  @param[in]     description        �ݒ���e
 */
//-----------------------------------------------------------------------------
void Model::Create(
    gfl::heap::NwAllocator*           allocator,
    gfl::heap::NwAllocator*           device_allocator,
    Resource*                         resource,
    s32                               index_in_resource,
    Description*                      description
)
{
  SceneNode::Create();
  
  Description l_description;
  if( description )
  {
    l_description = *description;
  }

  if( resource == NULL )
  {
    GFL_ASSERT_MSG( 0, "���\�[�X���s���̂��߃V�[���I�u�W�F�N�g�������ł��܂���ł����B\n" );
    return;
  }
  resource->PrintUnresolvedReference();

  nw::gfx::ResGraphicsFile* nw_res_graphics_file = resource->GetNwResGraphicsFile();
  nw::gfx::ResModel res = nw_res_graphics_file->GetModels(index_in_resource);

  nw::gfx::Skeleton* skeleton_share_source_skeleton;
  m_skeleton_share_type = CheckSkeletonShareSourceModel(
    NULL,
    &skeleton_share_source_skeleton );

  nw::gfx::Model* material_share_source_nw_model;
  m_material_share_type = CheckMaterialShareSourceModel(
    l_description.material_share_source_model,
    l_description.material_buffer_option,
    &material_share_source_nw_model );

  nw::gfx::Model* mesh_node_visibility_share_source_nw_model;
  m_mesh_node_visibility_share_type = CheckMeshNodeVisibilityShareSourceModel(
    l_description.mesh_node_visibility_share_source_model,
    &mesh_node_visibility_share_source_nw_model );


#define GFL_GRP_G3D_MODEL_MEMORY_DEBUG_PRINT (GFL_DEBUG && (0))

#if GFL_GRP_G3D_MODEL_MEMORY_DEBUG_PRINT
  {
    nw::gfx::SceneBuilder scene_builder;
    scene_builder
        .Resource(res)
        .IsFixedSizeMemory(true)
        .MaxChildren(0)
        .MaxCallbacks(l_description.max_callbacks)
        .BufferOption( ConvertMaterialBufferOptionToNwBufferOption(l_description.material_buffer_option) )
        .SharedMaterialModel(material_share_source_nw_model)
        .SharedMeshNodeVisibiltiyModel(mesh_node_visibility_share_source_nw_model)
        .MaxAnimObjectsPerGroup(l_description.animation_slot_num)
        .ParticleSetMarginCount(0)
        .IsAnimationEnabled( (l_description.animation_slot_num>0)?(true):(false) );  // ������false����nw::gfx::AnimGroup���擾�ł��Ȃ�  // ������O������ݒ�ł���悤�ɂ��邩�A�A�j���[�V�����Ȃ����f���N���X�����邩

            nw::gfx::SkeletalModel::Builder ske_model_builder;
            ske_model_builder
                .IsFixedSizeMemory(true)
                .MaxChildren(0)
                .MaxCallbacks(l_description.max_callbacks)
                .BufferOption( ConvertMaterialBufferOptionToNwBufferOption(l_description.material_buffer_option) )
                .SharedMaterialModel(material_share_source_nw_model)
                .SharedMeshNodeVisibilityModel(mesh_node_visibility_share_source_nw_model)
                .MaxAnimObjectsPerGroup(l_description.animation_slot_num)
                .IsAnimationEnabled( l_description.animation_slot_num>0)?(true):(false) );
   
    GFL_PRINT("[[[[decimal number]]]]\n");
    GFL_PRINT("hep_size=%d, dev_size=%d, alignment=default\n", scene_builder.GetMemorySize(), scene_builder.GetDeviceMemorySize() );
    GFL_PRINT("hep_size=%d, dev_size=%d, alignment=2\n", scene_builder.GetMemorySize(2), scene_builder.GetDeviceMemorySize(2) );
    GFL_PRINT("hep_size=%d, dev_size=%d, alignment=4\n", scene_builder.GetMemorySize(4), scene_builder.GetDeviceMemorySize(4) );
    // alignment=128����
    // os_Memory.h:413 Panic:NW:Failed assertion alignment <= MAX_ALIGNMENT
    // �ƂȂ�B
    // class MemorySizeCalculator
    //   static const size_t MAX_ALIGNMENT = IAllocator::CACHE_LINE_ALIGNMENT;
    // class IAllocator
    //   static const int CACHE_LINE_ALIGNMENT = 32;
    // �ő�32�̂悤���B
    GFL_PRINT("hep_size=%d, dev_size=%d, alignment=32\n", scene_builder.GetMemorySize(32), scene_builder.GetDeviceMemorySize(32) );
    
   
    // gfx_SkeletalModel.h
    // GetMemorySizeInternal
    {
      nw::gfx::ResSkeletalModel res_ske_model = nw::ut::ResStaticCast<nw::gfx::ResSkeletalModel>(res);
      GFL_PRINT("ske_model_builder  hep_size=%d, dev_size=%d\n", ske_model_builder.GetMemorySize(res_ske_model), ske_model_builder.GetDeviceMemorySize(res_ske_model) );
      
      nw::gfx::ResSkeleton res_ske = res_ske_model.GetSkeleton();
     
#ifdef NW_MOVE_ARRAY_CACHE_LINE_ALIGNMENT_ENABLED
      GFL_PRINT("alignment=%d\n", nw::gfx::Skeleton::TransformPose::TransformArray::MEMORY_ALIGNMENT);
#else
      GFL_PRINT("no alignment\n");
#endif
      
      GFL_PRINT("[+] Skeleton::TransformPose::Transform=%d * GetBonesCount=%d\n", sizeof(nw::gfx::Skeleton::TransformPose::Transform), res_ske.GetBonesCount());
     
      // gfx_StandardSkeleton.h
      // GetMemorySizeInternal
      {
        GFL_PRINT("[+] StandardSkeleton=%d\n", sizeof(nw::gfx::StandardSkeleton));
        GFL_PRINT("[+] Skeleton::TransformPose::Transform=%d * GetBonesCount=%d\n", sizeof(nw::gfx::Skeleton::TransformPose::Transform), res_ske.GetBonesCount());
        GFL_PRINT("[+] math::MTX34=%d * GetBonesCount=%d\n", sizeof(nw::math::MTX34), res_ske.GetBonesCount());
        GFL_PRINT("[+] math::MTX34=%d * GetBonesCount=%d\n", sizeof(nw::math::MTX34), res_ske.GetBonesCount());
     
        // gfx_Skeleton.h
        // GetMemorySizeForConstruct
        {
          GFL_PRINT("maxCallbacks=%d\n", l_description.max_callbacks);
          if( l_description.max_callbacks == 0 )
          {
            // ut/generated/ut_Signal.hi
            // GetMemorySizeForInvalidateSignalInternal
            GFL_PRINT("[+] CalculateMatrixSignal=%d\n", sizeof(nw::gfx::Skeleton::CalculateMatrixSignal));
            GFL_PRINT("[+] CalculateMatrixSignal=%d\n", sizeof(nw::gfx::Skeleton::CalculateMatrixSignal));
          }
          else
          {
            // ut/generated/ut_Signal.hi
            // GetMemorySizeForFixedSizedSignalInternal
#ifdef NW_SIGNAL_MULTI_SLOT_ENABLED
            GFL_PRINT("multi_slot=%d\n", l_description.max_callbacks);
#else
            GFL_PRINT("no multi_slot\n");
#endif
            GFL_PRINT("[+] CalculateMatrixSignal=%d\n", sizeof(nw::gfx::Skeleton::CalculateMatrixSignal));
            GFL_PRINT("[+] CalculateMatrixSignal=%d * maxCallbacks=%d\n", sizeof(nw::gfx::Skeleton::CalculateMatrixSignal), l_description.max_callbacks);
          }
        }
      }
     
      GFL_PRINT("[+] SkeletalModel=%d\n", sizeof(nw::gfx::SkeletalModel));
     
#if 0
//      ���������ǂ�ǂ�[���Ȃ�̂ŁA�ǂ��̂���߂��B

      // gfx_SkeletalModel.h
      // GetMemorySizeForInitialize
      {
              SkeletalModel::GetMemorySizeForInitialize(pSize, resModel, m_Description);
      }
    
    static void GetMemorySizeForInitialize(
        os::MemorySizeCalculator* pSize,
        ResSkeletalModel resModel,
        Description description)
    {
        // SkeletalModel::Initialize
        os::MemorySizeCalculator& size = *pSize;

        Model::GetMemorySizeForInitialize(pSize, resModel, description);

        // SkeletalModel::CreateSkeletalAnimGroup
        if (description.isAnimationEnabled)
        {
            const int animGroupCount = resModel.GetAnimGroupsCount();
            for (int animGroupIdx = 0; animGroupIdx < animGroupCount; ++animGroupIdx)
            {
                anim::ResAnimGroup resAnimGroup = resModel.GetAnimGroups(animGroupIdx);
                const int targetType = resAnimGroup.GetTargetType();
                const bool transformFlag = 
                    (resAnimGroup.GetFlags() & anim::ResAnimGroup::FLAG_IS_CALCULATED_TRANSFORM) != 0;
                if (transformFlag &&
                    targetType == anim::ResGraphicsAnimGroup::TARGET_TYPE_BONE)
                {
                    AnimGroup::Builder()
                        .ResAnimGroup(resAnimGroup)
                        .UseOriginalValue(true)
                        .GetMemorySizeInternal(pSize);
                    break; // ���ɃX�P���^�����f����p�̃A�j���[�V�����O���[�v���Ȃ��̂�
                }
            }
        }
    }

#endif 
    }  
  }
#endif  // #if GFL_GRP_G3D_MODEL_MEMORY_DEBUG_PRINT

#if GFL_GRP_G3D_MODEL_MEMORY_DEBUG_PRINT
gfl::debug::AddPrintFlag( gfl::debug::PRINT_FLAG_LEVEL1|gfl::debug::PRINT_FLAG_CATEGORY_HEAP );
#endif

  nw::gfx::SceneObject* scene_object = nw::gfx::SceneBuilder()
      .Resource(res)
      .IsFixedSizeMemory(true)
      .MaxChildren(0)
      .MaxCallbacks(l_description.max_callbacks)
      .BufferOption( ConvertMaterialBufferOptionToNwBufferOption(l_description.material_buffer_option) )
      .SharedMaterialModel(material_share_source_nw_model)
      .SharedMeshNodeVisibiltiyModel(mesh_node_visibility_share_source_nw_model)
      .MaxAnimObjectsPerGroup(l_description.animation_slot_num)
      .ParticleSetMarginCount(0)
      .IsAnimationEnabled( (l_description.animation_slot_num>0)?(true):(false) )  // ������false����nw::gfx::AnimGroup���擾�ł��Ȃ�  // ������O������ݒ�ł���悤�ɂ��邩�A�A�j���[�V�����Ȃ����f���N���X�����邩
      .CreateObject(allocator, device_allocator);

#if GFL_GRP_G3D_MODEL_MEMORY_DEBUG_PRINT
gfl::debug::ResetPrintFlag();
#endif

  GFL_ASSERT_STOP_MSG( scene_object, "index_in_resource=%d�̃V�[���I�u�W�F�N�g�������ł��܂���ł����B\n", index_in_resource );  //@fix STOP�ɕύX

  nw::gfx::SceneNode* scene_node = nw::ut::DynamicCast<nw::gfx::SceneNode*>(scene_object);
  SetNwSceneNode(scene_node);
  SetResourceIdAndIndexInResource(resource->GetId(), index_in_resource);

  // �Ǝ����� ��������
  m_material_buffer_option = l_description.material_buffer_option;

  AddRenderQueue(0);  // �f�t�H���g��0�Ԃ̃����_�[�L���[�ŕ`�悷��
  // UserParameter��RenderQueue�ɂ͎g���Ă��Ȃ����Ƃ����������̂ŁA
  // SafeDeleteCheck�p�̎g�킹�Ă��炤�B

  //m_tree_flag = true;

#if 0
  m_animation_slot_num = l_description.animation_slot_num;
  if( m_animation_slot_num > 0 )
  {
    m_animation_slot_array = GFL_NEW_ARRAY(allocator->GetHeapBase()) AnimationSlot[m_animation_slot_num];
    for( s32 i=0; i<m_animation_slot_num; ++i )
    {
      m_animation_slot_array[i].SetObjectIndex(i);
    }
  }
#endif
  m_animation_slot_num = l_description.animation_slot_num;
  m_animation_blend_unit_num = l_description.animation_blend_unit_num;
  m_animation_num_in_unit = l_description.animation_num_in_unit;
  //m_animation_manager = GFL_NEW(allocator->GetHeapBase()) AnimationManager;
  //m_animation_manager->Slot_Create(
  //    allocator,
  //    device_allocator,
  //    this,
  //    l_description.animation_slot_num
  //);

  m_calculate_callback_num_max = l_description.calculate_callback_num_max;
  if(m_calculate_callback_num_max > 0)
  {
    m_calculate_callback_function_array = GFL_NEW_ARRAY(allocator->GetHeapBase()) ModelCalculateCallbackFunction[m_calculate_callback_num_max];
    m_calculate_callback_work_array = GFL_NEW_ARRAY(allocator->GetHeapBase()) void*[m_calculate_callback_num_max];
    for( u32 i=0; i<m_calculate_callback_num_max; ++i )
    {
      m_calculate_callback_function_array[i] = NULL;
      m_calculate_callback_work_array[i] = NULL;
    }
  }
  internal::CreateCallbackConstraint(
    &m_callback_constraint_of_skeleton,
    &m_callback_constraint_of_transform_node,
    allocator,
    device_allocator,
    GetNwModel(),
    l_description.constraint_human_baton_num_max,
    l_description.constraint_hand_num_max,
    l_description.constraint_hand_baton_num_max
  );
  // �Ǝ����� �����܂�


#if GFL_DEBUG
  RemoveAllRenderQueue();  // �r����s_SafeDeleteCheckEnable��ON�ɂ����Ƃ��ɑΉ��ł���悤��NULL�ɂ��Ă����B
  if(s_SafeDeleteCheckEnable)
  {
    SetSafeDeleteCheckInfo(res.GetName());
  }
#endif

}

//-----------------------------------------------------------------------------
/*
 *  @brief         �X�P���g�������L�������f���̐���
 *
 *  @param[in]     allocator                   �A���P�[�^
 *  @param[in]     device_allocator            �f�o�C�X�������̃A���P�[�^
 *  @param[in]     resource                    ���\�[�X
 *  @param[in]     index_in_resource           ���\�[�X���ɂ�����C���f�b�N�X
 *  @param[in]     skeleton_share_source_model �X�P���g���̋��L���̃��f��
 *  @param[in]     description                 �ݒ���e
 */
//-----------------------------------------------------------------------------
void Model::CreateSkeletalModelSharingSkeleton(
    gfl::heap::NwAllocator*           allocator,
    gfl::heap::NwAllocator*           device_allocator,
    Resource*                         resource,
    s32                               index_in_resource,
    Model*                            skeleton_share_source_model,
    Description*                      description
)
{
  SceneNode::Create();

  Description l_description;
  if( description )
  {
    l_description = *description;
  }

  if( resource == NULL )
  {
    GFL_ASSERT_MSG( 0, "���\�[�X���s���̂��߃V�[���I�u�W�F�N�g�������ł��܂���ł����B\n" );
    return;
  }
  resource->PrintUnresolvedReference();

  nw::gfx::ResGraphicsFile* nw_res_graphics_file = resource->GetNwResGraphicsFile();
  nw::gfx::ResModel res = nw_res_graphics_file->GetModels(index_in_resource);

  // �X�P���g�����L�`�F�b�N
  if( !skeleton_share_source_model )
  {
    GFL_ASSERT_MSG( 0, "�X�P���g���̋��L���̃��f�����s���̂��߃��f���������ł��܂���ł����B\n" );
    return;
  }

  nw::gfx::Skeleton* skeleton_share_source_skeleton;
  m_skeleton_share_type = CheckSkeletonShareSourceModel(
    skeleton_share_source_model,
    &skeleton_share_source_skeleton );

  nw::gfx::Model* material_share_source_nw_model;
  m_material_share_type = CheckMaterialShareSourceModel(
    l_description.material_share_source_model,
    l_description.material_buffer_option,
    &material_share_source_nw_model );

  nw::gfx::Model* mesh_node_visibility_share_source_nw_model;
  m_mesh_node_visibility_share_type = CheckMeshNodeVisibilityShareSourceModel(
    l_description.mesh_node_visibility_share_source_model,
    &mesh_node_visibility_share_source_nw_model );
  
  nw::gfx::SkeletalModel* skeletal_model = nw::gfx::SkeletalModel::Builder()
    .IsFixedSizeMemory(true)
    .MaxChildren(0)
    .MaxCallbacks(l_description.max_callbacks)
    .BufferOption( ConvertMaterialBufferOptionToNwBufferOption(l_description.material_buffer_option) )
    .SharedSkeleton( skeleton_share_source_skeleton )
    .SharedMaterialModel(material_share_source_nw_model)
    .SharedMeshNodeVisibilityModel(mesh_node_visibility_share_source_nw_model)
    .MaxAnimObjectsPerGroup( l_description.animation_slot_num )
    .IsAnimationEnabled( (l_description.animation_slot_num>0)?(true):(false) )  // ������false����nw::gfx::AnimGroup���擾�ł��Ȃ�  // ������O������ݒ�ł���悤�ɂ��邩�A�A�j���[�V�����Ȃ����f���N���X�����邩
    .Create( NULL, res, device_allocator );

  GFL_ASSERT_STOP_MSG( skeletal_model, "index_in_resource=%d�̃V�[���I�u�W�F�N�g�������ł��܂���ł����B\n", index_in_resource );  //@fix STOP�ɂ���

  SetNwSceneNode(skeletal_model);
  SetResourceIdAndIndexInResource(resource->GetId(), index_in_resource);

  // �Ǝ����� ��������
  m_material_buffer_option = l_description.material_buffer_option;

  AddRenderQueue(0);  // �f�t�H���g��0�Ԃ̃����_�[�L���[�ŕ`�悷��
  // UserParameter��RenderQueue�ɂ͎g���Ă��Ȃ����Ƃ����������̂ŁA
  // SafeDeleteCheck�p�̎g�킹�Ă��炤�B

  //m_tree_flag = true;
#if 0
  m_animation_slot_num = l_description.animation_slot_num;
  if( m_animation_slot_num > 0 )
  {
    m_animation_slot_array = GFL_NEW_ARRAY(allocator->GetHeapBase()) AnimationSlot[m_animation_slot_num];
    for( s32 i=0; i<m_animation_slot_num; ++i )
    {
      m_animation_slot_array[i].SetObjectIndex(i);
    }
  }
#endif
  m_animation_slot_num = l_description.animation_slot_num;
  m_animation_blend_unit_num = l_description.animation_blend_unit_num;
  m_animation_num_in_unit = l_description.animation_num_in_unit;
  //m_animation_manager = GFL_NEW(allocator->GetHeapBase()) AnimationManager;
  //m_animation_manager->Slot_Create(
  //    allocator,
  //    device_allocator,
  //    this,
  //    l_description.animation_slot_num
  //);

  m_calculate_callback_num_max = l_description.calculate_callback_num_max;
  if(m_calculate_callback_num_max > 0)
  {
    m_calculate_callback_function_array = GFL_NEW_ARRAY(allocator->GetHeapBase()) ModelCalculateCallbackFunction[m_calculate_callback_num_max];
    m_calculate_callback_work_array = GFL_NEW_ARRAY(allocator->GetHeapBase()) void*[m_calculate_callback_num_max];
    for( u32 i=0; i<m_calculate_callback_num_max; ++i )
    {
      m_calculate_callback_function_array[i] = NULL;
      m_calculate_callback_work_array[i] = NULL;
    }
  }
  internal::CreateCallbackConstraint(
    &m_callback_constraint_of_skeleton,
    &m_callback_constraint_of_transform_node,
    allocator,
    device_allocator,
    GetNwModel(),
    l_description.constraint_human_baton_num_max,
    l_description.constraint_hand_num_max,
    l_description.constraint_hand_baton_num_max
  );
  // �Ǝ����� �����܂�
 

#if GFL_DEBUG
  RemoveAllRenderQueue();  // �r����s_SafeDeleteCheckEnable��ON�ɂ����Ƃ��ɑΉ��ł���悤��NULL�ɂ��Ă����B
  if(s_SafeDeleteCheckEnable)
  {
    SetSafeDeleteCheckInfo(res.GetName());
  }
#endif

}

//-----------------------------------------------------------------------------
/*
 *  @brief         �j��
 *                 ����Ă�ł����v�B�f�X�g���N�^�͌Ă΂�܂���B 
 */
//-----------------------------------------------------------------------------
void Model::Destroy(void)
{
  // �Ǝ����� ��������
  DestroyReplaceBoneData();
  internal::DestroyCallbackConstraint(
    &m_callback_constraint_of_skeleton,
    &m_callback_constraint_of_transform_node
  );
  if( m_calculate_callback_work_array )
  {
    GFL_DELETE_ARRAY m_calculate_callback_work_array;
    m_calculate_callback_work_array = NULL;
  }
  if( m_calculate_callback_function_array )
  {
    GFL_DELETE_ARRAY m_calculate_callback_function_array;
    m_calculate_callback_function_array = NULL;
  }
#if 0
  if( m_animation_slot_array )
  {
    GFL_DELETE_ARRAY m_animation_slot_array;
    m_animation_slot_array = NULL;
  }
#endif
  if( m_animation_manager )
  {
    GFL_DELETE m_animation_manager;
    m_animation_manager = NULL;
  }

  //m_tree_flag = true;
  m_skeleton_share_type = SHARE_TYPE_NONE;
  m_material_share_type = SHARE_TYPE_NONE;
  m_mesh_node_visibility_share_type = SHARE_TYPE_NONE;
  m_material_buffer_option = MATERIAL_BUFFER_OPTION_NOT_USE;
  m_animation_slot_num = 0;
  m_animation_blend_unit_num = 0;
  m_animation_num_in_unit = 0;
  m_calculate_callback_num_max = 0;
  // �Ǝ����� �����܂�

  SceneNode::Destroy();


#if GFL_DEBUG
  if(s_SafeDeleteCheckEnable)
  {
    u32   cnt = gfl::grp::GraphicsSystem::GetDrawFrameCount();

    if ( ( m_DrawFrameCount + 2 ) > cnt )
    {
      if( s_SafeDeleteCheckAssert )
      {
        GFL_ASSERT_MSG( 0, "gfx���f��%s�͕`�撆�ɉ������܂����B : �Ō�ɃR�}���h���ς܂ꂽ�t���[�� = %d : �j���t���[�� = %d\n\n", m_ModelName, m_DrawFrameCount, cnt );
      }
      else
      {
        GFL_PRINT( "gfx���f��%s�͕`�撆�ɉ������܂����B : �Ō�ɃR�}���h���ς܂ꂽ�t���[�� = %d : �j���t���[�� = %d\n\n", m_ModelName, m_DrawFrameCount, cnt );
      }
    }
  }
#endif

}

//-----------------------------------------------------------------------------
/*
 *  @brief         �v�Z����
 */
//-----------------------------------------------------------------------------
void Model::Calculate(void)
{
  // �v�Z�����ɂČĂяo�����R�[���o�b�N
  for( u32 i=0; i<m_calculate_callback_num_max; ++i )
  {
    if( m_calculate_callback_function_array[i] )
    {
      m_calculate_callback_function_array[i](this, m_calculate_callback_work_array[i]);
    }
  }

#if 0
  // �A�j���[�V�����X���b�g
  for( u32 i=0; i<m_animation_slot_num; i++ )
  {
    m_animation_slot_array[i].Calculate();
  }
#endif
  // �A�j���[�V�����}�l�[�W��
  if( m_animation_manager )
  {
    m_animation_manager->Calculate();
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ���W�ϊ�
 */
//-----------------------------------------------------------------------------
void Model::SetTransformMatrix(const gfl::math::MTX34& m)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetTransformMatrix( m );
}
void Model::SetTransformMatrix(
    f32 m00,
    f32 m01,
    f32 m02,
    f32 m03,
    f32 m10,
    f32 m11,
    f32 m12,
    f32 m13,
    f32 m20,
    f32 m21,
    f32 m22,
    f32 m23
)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetTransformMatrix(
    m00,
    m01,
    m02,
    m03,
    m10,
    m11,
    m12,
    m13,
    m20,
    m21,
    m22,
    m23
  );
}
void Model::GetTransformMatrix(gfl::math::MTX34* m) const  // �����̃X�P�[���͊|�����Ă��炸�A�e�̃X�P�[���͊|�����Ă��܂��B
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  *m = transform_node->Transform().TransformMatrix();
}
gfl::math::MTX34 Model::GetTransformMatrix(void) const  // �����̃X�P�[���͊|�����Ă��炸�A�e�̃X�P�[���͊|�����Ă��܂��B
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  gfl::math::MTX34 m = transform_node->Transform().TransformMatrix();
  return m;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ���s�ړ�
 */
//-----------------------------------------------------------------------------
void Model::SetTranslate(f32 x, f32 y, f32 z)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetTranslate( x, y, z );
}
void Model::SetTranslate(const gfl::math::VEC3& t)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetTranslate( t );
}
void Model::GetTranslate(gfl::math::VEC3* t) const
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().GetTranslate( t );
}
gfl::math::VEC3 Model::GetTranslate(void) const
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  return transform_node->Transform().GetTranslate();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ��]
 *                 �P�ʂ̓��W�A���B
 */
//-----------------------------------------------------------------------------
void Model::SetRotateXYZ(f32 x, f32 y, f32 z)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetRotateXYZ( x, y, z );
}
void Model::SetRotateXYZ(const gfl::math::VEC3& t)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetRotateXYZ( t.x, t.y, t.z );
}
void Model::GetRotateXYZ(gfl::math::VEC3* t) const  // �y���ӁzSetRotateXYZ�œn���ꂽ�l��Ԃ������ł��BSetRotateXYZ�ȊO�ŉ�]���w�肵�Ă����ꍇ��
{                                                   //         �����ڂƍ���Ȃ��l���Ԃ��Ă��܂��B
  *t = m_rotate;
}
gfl::math::VEC3 Model::GetRotateXYZ(void) const  // �y���ӁzSetRotateXYZ�œn���ꂽ�l��Ԃ������ł��BSetRotateXYZ�ȊO�ŉ�]���w�肵�Ă����ꍇ��
{                                                //         �����ڂƍ���Ȃ��l���Ԃ��Ă��܂��B
  return m_rotate;
}

void Model::SetRotateMatrix(
    f32 m00,
    f32 m01,
    f32 m02,
    f32 m10,
    f32 m11,
    f32 m12,
    f32 m20,
    f32 m21,
    f32 m22
)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetRotateMatrix(
    m00,
    m01,
    m02,
    m10,
    m11,
    m12,
    m20,
    m21,
    m22
  );
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �X�P�[��
 */
//-----------------------------------------------------------------------------
void Model::SetScale(f32 x, f32 y, f32 z)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetScale( x, y, z );
}
void Model::SetScale(const gfl::math::VEC3& t)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetScale( t );
}
void Model::GetScale(gfl::math::VEC3* t) const
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  *t = transform_node->Transform().Scale();
}
gfl::math::VEC3 Model::GetScale(void) const
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  gfl::math::VEC3 t = transform_node->Transform().Scale();
  return t;
}

#if 0
//���̃��f���Ɉڂ�����̓��삪�悭�Ȃ��̂ŁA���̊֐��͂����ƃe�X�g���Ă���łȂ��Ǝg���Ȃ��B
//m_skeleton_type -> m_skeleton_share_type
//SKELETON_TYPE_ -> SHARE_TYPE_
//-----------------------------------------------------------------------------
/*
 *  @brief         �X�P���g���̋��L���ł��錠���𑼂̃��f���Ɉڂ�
 *                 this���X�P���g���̋��L���ł���A
 *                 skeleton_share_new_source_model�����̃X�P���g�������L���Ă��郂�f���ł���K�v������܂��B
 *
 *  @param[in]     skeleton_share_new_source_model  �V�����X�P���g���̋��L���̌�����^�����郂�f��
 */
//-----------------------------------------------------------------------------
void Model::ChangeSkeletonShareSource(Model* skeleton_share_new_source_model)
{
  if(    ( this->m_skeleton_type                            != SKELETON_TYPE_SHARE_SOURCE      )
      || ( skeleton_share_new_source_model->m_skeleton_type != SKELETON_TYPE_SHARE_DESTINATION ) )
  {
    GFL_ASSERT_MSG( 0, "�X�P���g���̋��L�ݒ肪�ُ�̂��߁A�X�P���g���̋��L���ł��錠���𑼂̃��f���Ɉڂ��܂���ł����B\n" );
    return;
  }

  nw::gfx::SkeletalModel* skeletal_model = \
      nw::ut::DynamicCast<nw::gfx::SkeletalModel*>( this->GetNwSceneNode() );

  nw::gfx::SkeletalModel* skeleton_share_new_source_skeletal_model = \
      nw::ut::DynamicCast<nw::gfx::SkeletalModel*>( skeleton_share_new_source_model->GetNwSceneNode() );

  skeletal_model->SwapSkeleton( skeleton_share_new_source_skeletal_model );

  this->m_skeleton_type                            = SKELETON_TYPE_SHARE_DESTINATION;
  skeleton_share_new_source_model->m_skeleton_type = SKELETON_TYPE_SHARE_SOURCE;
}
#endif

//-----------------------------------------------------------------------------
/*
 *  @brief         �`�揇
 *                 �v���O���}�������ݒ�ł��郂�f���̃��C���[ID
 *                 �ŏ��ɕ`�� 0 <= u8 <= 127 �Ō�ɕ`��
 *                 �ŗD�悳���`�揇 ModelLayerId > MaterialTranslucencyKind > ���̑� �ŗ�コ���`�揇
 *
 *                 **** Model��Particle�ɂ���S�������ɕ��ׂ�����ԍ��ł��B�f�t�H���g0000000�ł��B ****
 */
//-----------------------------------------------------------------------------
void Model::SetModelLayerId(u8 model_layer_id)
{
  // u8�����炵�Ďg���B�Ō��1�r�b�g�ɂ�Model0�AParticle1�̃f�t�H���g�l������Ă���B
  GFL_ASSERT(model_layer_id <= 127);  //@check �`�揇����������
  
  nw::gfx::Model* model = GetNwModel();
  model->SetLayerId((model_layer_id << 1));
  model->InvalidateRenderKeyCache();  // ���ꂪ�Ȃ��ƕ`�揇���ς��Ȃ�
}

/*
[����] NintendoWare��ł̕`��D��x�̎w��ɂ���

�J���� : CTR-SDK 0.9.7/NintendoWare 0.7.4

�c����NTSC 2010-06-03 09:02:17  
>>���C��ID �� ������ �� ���b�V���`��D��x �� �}�e���A��ID�A�[�x�l
>������Ɋւ��Ă͊m�F���Ă���܂��̂ł��҂����������B
������Ɋւ��Ă͈ꉞ����Ő������Ƃ̂��Ƃł��B

��������ł̓}�e���A���D��̃\�[�g�����ǒ��ňꎞ�I�ɕ`��D��x���͂����Ă���A
����ȍ~��0�`16�i�K�Ń\�[�g���ł���悤�ɏC������\��Ƃ̂��Ƃł�
*/

u8 Model::GetModelLayerId(void) const
{
  // u8�����炵�Ďg���B�Ō��1�r�b�g�ɂ�Model0�AParticle1�̃f�t�H���g�l������Ă���B
  return ((GetNwModel()->GetLayerId()) >> 1);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �}�e���A��
 */
//-----------------------------------------------------------------------------
s32 Model::GetMaterialCount() const
{
  s32 count = 0;
  nw::gfx::Model* model = GetNwModel();
  if( model )
  {
    count = model->GetMaterialCount();
  }
  return count;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �}�e���A���̃C���f�b�N�X�ԍ����擾����
 *
 *  @param[in]     key  �}�e���A���̖��O
 * 
 *  @retval        s32    �}�e���A���̃C���f�b�N�X�ԍ�
 *                        �w�肵�����O�����}�e���A�������݂��Ȃ��ꍇ�AMATERIAL_INDEX_NOT_FOUND��Ԃ�
 */
//-----------------------------------------------------------------------------
s32 Model::GetMaterialIndex(const char* key) const
{
  s32              count   = GetMaterialCount();
  nw::gfx::Model*  model   = GetNwModel();
  for( s32 i=0; i<count; ++i )
  {
    nw::gfx::Material*  material = model->GetMaterial(i);
    if( gfl::std::StringCmp( key, material->GetName() ) == 0 ) return i;
  }
  return MATERIAL_INDEX_NOT_FOUND;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �}�e���A���J���[
 *                 u8�̂Ƃ���0����&�����A255����&�s����
 *                 f32�̂Ƃ���0.0f����&�����A1.0f����&�s����
 */
//-----------------------------------------------------------------------------
// �f�B�t���[�Y�J���[
void Model::SetMaterialColorDiffuse(s32 material_index, f32 r, f32 g, f32 b)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();
 

  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  res_material_color.SetDiffuse(r, g, b); 
  res_material.SetMaterialColorHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�


  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
void Model::SetMaterialColorDiffuse(s32 material_index, f32 r, f32 g, f32 b, f32 a)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  res_material_color.SetDiffuse(r, g, b, a); 
  res_material.SetMaterialColorHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�


  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
void Model::SetMaterialColorDiffuse(s32 material_index, const gfl::grp::ColorF32& c)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  res_material_color.SetDiffuse(c); 
  res_material.SetMaterialColorHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�


  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
void Model::SetMaterialColorDiffuse(s32 material_index, u8 r, u8 g, u8 b)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  res_material_color.SetDiffuse(
      (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX
  ); 
  res_material.SetMaterialColorHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�


  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
void Model::SetMaterialColorDiffuse(s32 material_index, u8 r, u8 g, u8 b, u8 a)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  res_material_color.SetDiffuse(
      (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(a))/gfl::grp::ColorU8::ELEMENT_MAX
  ); 
  res_material.SetMaterialColorHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�


  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
void Model::SetMaterialColorDiffuse(s32 material_index, const gfl::grp::ColorU8& c)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();

  
  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  res_material_color.SetDiffuse(
      (static_cast<f32>(c.r))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(c.g))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(c.b))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(c.a))/gfl::grp::ColorU8::ELEMENT_MAX
  ); 
  res_material.SetMaterialColorHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�


  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
gfl::grp::ColorF32 Model::GetMaterialColorDiffuse(s32 material_index) const
{
  gfl::grp::ColorF32 c(0.0f, 0.0f, 0.0f, 0.0f);

  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  c = res_material_color.GetDiffuse(); 


  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }

  return c;
}

void Model::GetMaterialColorDiffuse(s32 material_index, gfl::grp::ColorF32* c) const
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  *c = res_material_color.GetDiffuse(); 


  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
void Model::GetMaterialColorDiffuse(s32 material_index, gfl::grp::ColorU8*  c) const
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  gfl::grp::ColorF32 c_f32 = res_material_color.GetDiffuse(); 
  c->Set(
    static_cast<s32>(c_f32.r*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.g*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.b*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.a*gfl::grp::ColorU8::ELEMENT_MAX)
  );


  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}

// �R���X�^���g�J���[
void Model::SetMaterialColorConstant(s32 material_index, s32 no, f32 r, f32 g, f32 b)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();
 

  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  switch(no)
  {
  case 0: res_material_color.SetConstant0(r, g, b); break;
  case 1: res_material_color.SetConstant1(r, g, b); break;
  case 2: res_material_color.SetConstant2(r, g, b); break;
  case 3: res_material_color.SetConstant3(r, g, b); break;
  case 4: res_material_color.SetConstant4(r, g, b); break;
  case 5: res_material_color.SetConstant5(r, g, b); break;
  default:
    GFL_ASSERT_MSG( 0, "�R���X�^���g�J���[�̔ԍ����ُ�ł��B\n" );
  }
  res_material.SetMaterialColorHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�


  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
void Model::SetMaterialColorConstant(s32 material_index, s32 no, f32 r, f32 g, f32 b, f32 a)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();
 

  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  switch(no)
  {
  case 0: res_material_color.SetConstant0(r, g, b, a); break;
  case 1: res_material_color.SetConstant1(r, g, b, a); break;
  case 2: res_material_color.SetConstant2(r, g, b, a); break;
  case 3: res_material_color.SetConstant3(r, g, b, a); break;
  case 4: res_material_color.SetConstant4(r, g, b, a); break;
  case 5: res_material_color.SetConstant5(r, g, b, a); break;
  default:
    GFL_ASSERT_MSG( 0, "�R���X�^���g�J���[�̔ԍ����ُ�ł��B\n" );
  }
  res_material.SetMaterialColorHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�


  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
void Model::SetMaterialColorConstant(s32 material_index, s32 no, const gfl::grp::ColorF32& c)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();
 

  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  switch(no)
  {
  case 0: res_material_color.SetConstant0(c); break;
  case 1: res_material_color.SetConstant1(c); break;
  case 2: res_material_color.SetConstant2(c); break;
  case 3: res_material_color.SetConstant3(c); break;
  case 4: res_material_color.SetConstant4(c); break;
  case 5: res_material_color.SetConstant5(c); break;
  default:
    GFL_ASSERT_MSG( 0, "�R���X�^���g�J���[�̔ԍ����ُ�ł��B\n" );
  }
  res_material.SetMaterialColorHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�


  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
void Model::SetMaterialColorConstant(s32 material_index, s32 no, u8 r, u8 g, u8 b)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();
 
    f32 fr = (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX;
    f32 fg = (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX;
    f32 fb = (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX;


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  switch(no)
  {
  case 0: res_material_color.SetConstant0(fr, fg, fb); break;
  case 1: res_material_color.SetConstant1(fr, fg, fb); break;
  case 2: res_material_color.SetConstant2(fr, fg, fb); break;
  case 3: res_material_color.SetConstant3(fr, fg, fb); break;
  case 4: res_material_color.SetConstant4(fr, fg, fb); break;
  case 5: res_material_color.SetConstant5(fr, fg, fb); break;
  default:
    GFL_ASSERT_MSG( 0, "�R���X�^���g�J���[�̔ԍ����ُ�ł��B\n" );
  }
  res_material.SetMaterialColorHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�


  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
void Model::SetMaterialColorConstant(s32 material_index, s32 no, u8 r, u8 g, u8 b, u8 a)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();
 
    f32 fr = (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX;
    f32 fg = (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX;
    f32 fb = (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX;
    f32 fa = (static_cast<f32>(a))/gfl::grp::ColorU8::ELEMENT_MAX;


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  switch(no)
  {
  case 0: res_material_color.SetConstant0(fr, fg, fb, fa); break;
  case 1: res_material_color.SetConstant1(fr, fg, fb, fa); break;
  case 2: res_material_color.SetConstant2(fr, fg, fb, fa); break;
  case 3: res_material_color.SetConstant3(fr, fg, fb, fa); break;
  case 4: res_material_color.SetConstant4(fr, fg, fb, fa); break;
  case 5: res_material_color.SetConstant5(fr, fg, fb, fa); break;
  default:
    GFL_ASSERT_MSG( 0, "�R���X�^���g�J���[�̔ԍ����ُ�ł��B\n" );
  }
  res_material.SetMaterialColorHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�


  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
void Model::SetMaterialColorConstant(s32 material_index, s32 no, const gfl::grp::ColorU8& c)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();

    f32 fr = (static_cast<f32>(c.r))/gfl::grp::ColorU8::ELEMENT_MAX;
    f32 fg = (static_cast<f32>(c.g))/gfl::grp::ColorU8::ELEMENT_MAX;
    f32 fb = (static_cast<f32>(c.b))/gfl::grp::ColorU8::ELEMENT_MAX;
    f32 fa = (static_cast<f32>(c.a))/gfl::grp::ColorU8::ELEMENT_MAX;
 

  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  switch(no)
  {
  case 0: res_material_color.SetConstant0(fr, fg, fb, fa); break;
  case 1: res_material_color.SetConstant1(fr, fg, fb, fa); break;
  case 2: res_material_color.SetConstant2(fr, fg, fb, fa); break;
  case 3: res_material_color.SetConstant3(fr, fg, fb, fa); break;
  case 4: res_material_color.SetConstant4(fr, fg, fb, fa); break;
  case 5: res_material_color.SetConstant5(fr, fg, fb, fa); break;
  default:
    GFL_ASSERT_MSG( 0, "�R���X�^���g�J���[�̔ԍ����ُ�ł��B\n" );
  }
  res_material.SetMaterialColorHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�


  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
gfl::grp::ColorF32 Model::GetMaterialColorConstant(s32 material_index, s32 no) const
{
  gfl::grp::ColorF32 c(0.0f, 0.0f, 0.0f, 0.0f);

  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  switch(no)
  {
  case 0: c = res_material_color.GetConstant0(); break;
  case 1: c = res_material_color.GetConstant1(); break;
  case 2: c = res_material_color.GetConstant2(); break;
  case 3: c = res_material_color.GetConstant3(); break;
  case 4: c = res_material_color.GetConstant4(); break;
  case 5: c = res_material_color.GetConstant5(); break;
  default:
    GFL_ASSERT_MSG( 0, "�R���X�^���g�J���[�̔ԍ����ُ�ł��B\n" );
  }


  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }

  return c;
}
void Model::GetMaterialColorConstant(s32 material_index, s32 no, gfl::grp::ColorF32* c) const
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  switch(no)
  {
  case 0: *c = res_material_color.GetConstant0(); break;
  case 1: *c = res_material_color.GetConstant1(); break;
  case 2: *c = res_material_color.GetConstant2(); break;
  case 3: *c = res_material_color.GetConstant3(); break;
  case 4: *c = res_material_color.GetConstant4(); break;
  case 5: *c = res_material_color.GetConstant5(); break;
  default:
    GFL_ASSERT_MSG( 0, "�R���X�^���g�J���[�̔ԍ����ُ�ł��B\n" );
  }


  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
void Model::GetMaterialColorConstant(s32 material_index, s32 no, gfl::grp::ColorU8*  c) const
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetMaterialColorResMaterial();


  nw::gfx::res::ResMaterialColor res_material_color = res_material.GetMaterialColor();
  gfl::grp::ColorF32 c_f32;
  switch(no)
  {
  case 0: c_f32 = res_material_color.GetConstant0(); break;
  case 1: c_f32 = res_material_color.GetConstant1(); break;
  case 2: c_f32 = res_material_color.GetConstant2(); break;
  case 3: c_f32 = res_material_color.GetConstant3(); break;
  case 4: c_f32 = res_material_color.GetConstant4(); break;
  case 5: c_f32 = res_material_color.GetConstant5(); break;
  default:
    GFL_ASSERT_MSG( 0, "�R���X�^���g�J���[�̔ԍ����ُ�ł��B\n" );
  }
  c->Set(
    static_cast<s32>(c_f32.r*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.g*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.b*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.a*gfl::grp::ColorU8::ELEMENT_MAX)
  );


  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �}�e���A�����e�����󂯂�t�H�O
 */
//-----------------------------------------------------------------------------
void Model::SetMaterialFogEnable(s32 material_index, bool enable)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    //nw::gfx::res::ResMaterial& res_material = material->GetSceneEnvironmentResMaterial();
    nw::gfx::res::ResMaterial  res_material = material->GetShadingParameterResMaterial();  // �t�H�O�͂��̃}�e���A��
    u32 flags = res_material.GetFlags();
    u32 fog_bit = nw::gfx::res::ResMaterialData::FLAG_FOG_ENABLED;
    if(enable)
    {
      flags |= fog_bit;
    }
    else
    {
      flags &= ~fog_bit;
    }
    res_material.SetFlags(flags);
    // SceneEnvironmentResMaterial�p��SetMaterial*Hash�֐����Ȃ������f����Ă��邩�H
    res_material.SetShadingParameterHash(0x0);  // �t�H�O�͂���Hash
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
bool Model::IsMaterialFogEnable(s32 material_index) const
{
  bool enable = false;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    //nw::gfx::res::ResMaterial& res_material = material->GetSceneEnvironmentResMaterial();
    nw::gfx::res::ResMaterial res_material = material->GetShadingParameterResMaterial();  // �t�H�O�͂��̃}�e���A��
    u32 flags = res_material.GetFlags();
    u32 fog_bit = nw::gfx::res::ResMaterialData::FLAG_FOG_ENABLED;
    flags &= fog_bit;
    if(flags)
    {
      enable = true;
    }
    else
    {
      enable = false;
    }
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return enable;
}
void Model::SetMaterialFogIndex(s32 material_index, s32 index)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    //nw::gfx::res::ResMaterial& res_material = material->GetSceneEnvironmentResMaterial();
    nw::gfx::res::ResMaterial  res_material = material->GetShadingParameterResMaterial();  // �t�H�O�͂��̃}�e���A��
    res_material.SetFogIndex(index);
    // SceneEnvironmentResMaterial�p��SetMaterial*Hash�֐����Ȃ������f����Ă��邩�H
    res_material.SetShadingParameterHash(0x0);  // �t�H�O�͂���Hash
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
s32  Model::GetMaterialFogIndex(s32 material_index) const
{
  s32 index = 0;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    //nw::gfx::res::ResMaterial& res_material = material->GetSceneEnvironmentResMaterial();
    nw::gfx::res::ResMaterial  res_material = material->GetShadingParameterResMaterial();  // �t�H�O�͂��̃}�e���A��
    index = res_material.GetFogIndex();
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return index;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �}�e���A�����e�����󂯂郉�C�g
 */
//-----------------------------------------------------------------------------
//void SetMaterialFragmentLightEnable(s32 material_index, bool enable);
//bool IsMaterialFragmentLightEnable(s32 material_index) const;
//void SetMaterialVertexLightEnable(s32 material_index, bool enable);
//bool IsMaterialVertexLightEnable(s32 material_index) const;
//void SetMaterialHemiSphereLightEnable(s32 material_index, bool enable, bool oclusion_enable);
//bool IsMaterialHemiSphereLightEnable(s32 material_index, bool* oclusion_enable) const;

void Model::SetMaterialLightSetIndex(s32 material_index, s32 index)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetSceneEnvironmentResMaterial();  // @note ���C�g�Z�b�g�͂���ł����̂��H��gfx_RenderContext.cpp RenderContext::ActivateSceneEnvironment���A�����Ɣ��f�ł���B
    //nw::gfx::res::ResMaterial& res_material = material->GetFragmentLightingResMaterial();
    //nw::gfx::res::ResMaterial& res_material = material->GetFragmentLightingTableResMaterial();
    res_material.SetLightSetIndex(index);
    // @note SceneEnvironmentResMaterial�p��SetMaterial*Hash�֐����Ȃ������f����Ă��邩�H�����̃C���f�b�N�X�̓n�b�V���Ɋ֌W�Ȃ��B
    //res_material.SetFragmentLightingHash(0x0);  // @note ����3��Hash���Ă�ł���Ă��������f����Ă��Ȃ��C�����遨���̃C���f�b�N�X�̓n�b�V���Ɋ֌W�Ȃ��B
    //res_material.SetFragmentLightingTableHash(0x0);  // ResMaterial��Get���炵�ĊԈ���Ă���̂����B
    //res_material.SetFragmentLightingTableParametersHash(0x0);  // �������Ƃ�����A���L��Get�֐���ResMaterial�����g���������ƁB
                                                               // @note ActiveLightSet�Ƃ����֐������邪�A�����̃��C�g�Z�b�g�𓯎��ɃA�N�e�B�u�ɂł��邾�낤���B���ł��Ă��邩��ł���񂾂낤���B�������gfx_SceneEnvironment.h�Ń}�e���A�����Ƃɕ`�悷��ۂɃA�N�e�B�u�ȃ��C�g�Z�b�g��؂�ւ��Ă���Ƃ������ƁB
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
s32  Model::GetMaterialLightSetIndex(s32 material_index) const
{
  s32 index = 0;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetSceneEnvironmentResMaterial();  // @note ���C�g�Z�b�g�͂���ł����̂��H��gfx_RenderContext.cpp RenderContext::ActivateSceneEnvironment���A�����Ɣ��f�ł���B
    index = res_material.GetLightSetIndex();
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return index;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �}�e���A���̃J�����O���[�h
 */
//-----------------------------------------------------------------------------
void Model::SetMaterialCullingMode(s32 material_index, MaterialCullingMode mode)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetRasterizationResMaterial();
    nw::gfx::res::ResRasterization res_rasterization = res_material.GetRasterization();
    res_rasterization.SetCullingMode(static_cast<nw::gfx::res::ResRasterization::CullingMode>(mode));
    res_material.SetRasterizationHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
Model::MaterialCullingMode Model::GetMaterialCullingMode(s32 material_index) const
{
  MaterialCullingMode mode = MATERIAL_CULLING_MODE_BACK;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetRasterizationResMaterial();
    nw::gfx::res::ResRasterization res_rasterization = res_material.GetRasterization();
    mode = static_cast<MaterialCullingMode>(res_rasterization.GetCullingMode());
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return mode;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �}�e���A���̃A���t�@�e�X�g
 */
//-----------------------------------------------------------------------------
bool Model::IsMaterialAlphaTestEnable(s32 material_index) const
{
  bool enable = false;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetAlphaTestResMaterial();
    nw::gfx::res::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
    nw::gfx::res::ResAlphaTest res_alpha_test = res_fragment_shader.GetAlphaTest();
    
    enable = res_alpha_test.IsTestEnabled();
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return enable;
}
void Model::SetMaterialAlphaTestEnable(s32 material_index, bool enable)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetAlphaTestResMaterial();
    nw::gfx::res::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
    nw::gfx::res::ResAlphaTest res_alpha_test = res_fragment_shader.GetAlphaTest();
   
    res_alpha_test.SetTestEnabled(enable);
    res_material.SetAlphaTestHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
Model::MaterialTestFunction Model::GetMaterialAlphaTestFunction(s32 material_index) const
{
  MaterialTestFunction function = MATERIAL_TEST_FUNCTION_NEVER;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetAlphaTestResMaterial();
    nw::gfx::res::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
    nw::gfx::res::ResAlphaTest res_alpha_test = res_fragment_shader.GetAlphaTest();
    
    function = static_cast<MaterialTestFunction>(res_alpha_test.GetTestFunc());
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return function;
}
void Model::SetMaterialAlphaTestFunction(s32 material_index, MaterialTestFunction function)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetAlphaTestResMaterial();
    nw::gfx::res::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
    nw::gfx::res::ResAlphaTest res_alpha_test = res_fragment_shader.GetAlphaTest();
   
    res_alpha_test.SetTestFunc(static_cast<nw::gfx::res::ResAlphaTest::TestFunc>(function));
    res_material.SetAlphaTestHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
f32 Model::GetMaterialAlphaTestReference(s32 material_index) const
{
  f32 reference = 0.0f;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetAlphaTestResMaterial();
    nw::gfx::res::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
    nw::gfx::res::ResAlphaTest res_alpha_test = res_fragment_shader.GetAlphaTest();
    
    reference = res_alpha_test.GetTestReference();
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return reference;
}
void Model::SetMaterialAlphaTestReference(s32 material_index, f32 reference)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetAlphaTestResMaterial();
    nw::gfx::res::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
    nw::gfx::res::ResAlphaTest res_alpha_test = res_fragment_shader.GetAlphaTest();
   
    res_alpha_test.SetTestReference(reference);
    res_material.SetAlphaTestHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �}�e���A���̃f�v�X�e�X�g
 */
//-----------------------------------------------------------------------------
bool Model::IsMaterialDepthTestEnable(s32 material_index) const          // DepthOperation IsTestEnabled
{
  bool enable = false;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResDepthOperation res_depth_operation = res_fragment_operation.GetDepthOperation();
    u32 flags = res_depth_operation.GetFlags();
    u32 test_bit = nw::gfx::res::ResDepthOperationData::FLAG_TEST_ENABLED;
    flags &= test_bit;
    if(flags)
    {
      enable = true;
    }
    else
    {
      enable = false;
    }
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return enable;
}
void Model::SetMaterialDepthTestEnable(s32 material_index, bool enable)  // true�̂Ƃ��f�v�X�e�X�g�L��
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResDepthOperation res_depth_operation = res_fragment_operation.GetDepthOperation();
    u32 flags = res_depth_operation.GetFlags();
    u32 test_bit = nw::gfx::res::ResDepthOperationData::FLAG_TEST_ENABLED;
    if(enable)
    {
      flags |= test_bit;
    }
    else
    {
      flags &= ~test_bit;
    }
    res_depth_operation.SetFlags(flags);
    res_material.SetFragmentOperationHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
bool Model::IsMaterialDepthTestMaskEnable(s32 material_index) const          // DepthOperation IsMaskEnabled
{
  bool enable = false;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResDepthOperation res_depth_operation = res_fragment_operation.GetDepthOperation();
    u32 flags = res_depth_operation.GetFlags();
    u32 mask_bit = nw::gfx::res::ResDepthOperationData::FLAG_MASK_ENABLED;
    flags &= mask_bit;
    if(flags)
    {
      enable = true;
    }
    else
    {
      enable = false;
    }
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return enable;
}
void Model::SetMaterialDepthTestMaskEnable(s32 material_index, bool enable)  // true�̂Ƃ��f�v�X�}�X�N�L��(true�̂Ƃ��f�v�X�o�b�t�@���X�V����)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResDepthOperation res_depth_operation = res_fragment_operation.GetDepthOperation();
    u32 flags = res_depth_operation.GetFlags();
    u32 mask_bit = nw::gfx::res::ResDepthOperationData::FLAG_MASK_ENABLED;
    if(enable)
    {
      flags |= mask_bit;
    }
    else
    {
      flags &= ~mask_bit;
    }
    res_depth_operation.SetFlags(flags);
    res_material.SetFragmentOperationHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
Model::MaterialTestFunction Model::GetMaterialDepthTestFunction(s32 material_index) const  // DepthOperation TestFunction
{
  MaterialTestFunction function = MATERIAL_TEST_FUNCTION_NEVER;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResDepthOperation res_depth_operation = res_fragment_operation.GetDepthOperation();
    function = static_cast<MaterialTestFunction>(res_depth_operation.GetTestFunc()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return function;
}
void Model::SetMaterialDepthTestFunction(s32 material_index, MaterialTestFunction function)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResDepthOperation res_depth_operation = res_fragment_operation.GetDepthOperation();
    res_depth_operation.SetTestFunc(static_cast<nw::gfx::res::ResDepthOperation::TestFunc>(function)); 
    res_material.SetFragmentOperationHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �}�e���A���̃X�e���V���e�X�g
 */
//-----------------------------------------------------------------------------
bool Model::IsMaterialStencilTestEnable(s32 material_index) const
{
  bool enable = false;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    enable = res_stencil_operation.IsTestEnabled(); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return enable;
}
void Model::SetMaterialStencilTestEnable(s32 material_index, bool enable)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    res_stencil_operation.SetTestEnabled(enable); 
    res_material.SetFragmentOperationHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
Model::MaterialTestFunction Model::GetMaterialStencilTestFunction(s32 material_index) const
{
  MaterialTestFunction function = MATERIAL_TEST_FUNCTION_NEVER;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    function = static_cast<MaterialTestFunction>(res_stencil_operation.GetTestFunction()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return function;
}
void Model::SetMaterialStencilTestFunction(s32 material_index, MaterialTestFunction function)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    res_stencil_operation.SetTestFunction(static_cast<nw::gfx::res::ResStencilOperation::TestFunc>(function)); 
    res_material.SetFragmentOperationHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
s32 Model::GetMaterialStencilTestReference(s32 material_index) const
{
  s32 value = 0;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    value = res_stencil_operation.GetTestReference(); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return value;
}
void Model::SetMaterialStencilTestReference(s32 material_index, s32 value)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    res_stencil_operation.SetTestReference(value); 
    res_material.SetFragmentOperationHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
u32 Model::GetMaterialStencilTestMask(s32 material_index) const
{
  u32 value = 0;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    value = res_stencil_operation.GetTestMask(); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return value;
}
void Model::SetMaterialStencilTestMask(s32 material_index, u32 value)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    res_stencil_operation.SetTestMask(value); 
    res_material.SetFragmentOperationHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
Model::MaterialStencilOperation Model::GetMaterialStencilTestFailOperation(s32 material_index) const
{
  MaterialStencilOperation operation = MATERIAL_STENCIL_OPERATION_KEEP;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    operation = static_cast<MaterialStencilOperation>(res_stencil_operation.GetFailOperation()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return operation;
}
void Model::SetMaterialStencilTestFailOperation(s32 material_index, MaterialStencilOperation operation)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    res_stencil_operation.SetFailOperation(static_cast<nw::gfx::res::ResStencilOperation::StencilOp>(operation)); 
    res_material.SetFragmentOperationHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
Model::MaterialStencilOperation Model::GetMaterialStencilTestZFailOperation(s32 material_index) const
{
  MaterialStencilOperation operation = MATERIAL_STENCIL_OPERATION_KEEP;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    operation = static_cast<MaterialStencilOperation>(res_stencil_operation.GetZFailOperation()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return operation;
}
void Model::SetMaterialStencilTestZFailOperation(s32 material_index, MaterialStencilOperation operation)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    res_stencil_operation.SetZFailOperation(static_cast<nw::gfx::res::ResStencilOperation::StencilOp>(operation)); 
    res_material.SetFragmentOperationHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
Model::MaterialStencilOperation Model::GetMaterialStencilTestPassOperation(s32 material_index) const
{
  MaterialStencilOperation operation = MATERIAL_STENCIL_OPERATION_KEEP;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    operation = static_cast<MaterialStencilOperation>(res_stencil_operation.GetPassOperation()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return operation;
}
void Model::SetMaterialStencilTestPassOperation(s32 material_index, MaterialStencilOperation operation)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResStencilOperation res_stencil_operation = res_fragment_operation.GetStencilOperation();
    res_stencil_operation.SetPassOperation(static_cast<nw::gfx::res::ResStencilOperation::StencilOp>(operation)); 
    res_material.SetFragmentOperationHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �}�e���A���̃u�����_�[
 */
//-----------------------------------------------------------------------------
#if 1
Model::MaterialBlendMode Model::GetMaterialBlendMode(s32 material_index) const
{
  MaterialBlendMode mode = MATERIAL_BLEND_MODE_NOT_USE;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    nw::gfx::res::ResBlendOperation::Mode nw_mode = res_blend_operation.GetMode();
    
    switch(nw_mode)
    {
    case nw::gfx::res::ResBlendOperation::MODE_SEPARATE_BLEND:
      {
        mode = MATERIAL_BLEND_MODE_SEPARATE_BLEND;
        if(
             GetMaterialBlendFunctionSourceRgb(material_index)        == MATERIAL_BLEND_FACTOR_ONE
          && GetMaterialBlendFunctionSourceAlpha(material_index)      == MATERIAL_BLEND_FACTOR_ONE
          && GetMaterialBlendFunctionDestinationRgb(material_index)   == MATERIAL_BLEND_FACTOR_ZERO
          && GetMaterialBlendFunctionDestinationAlpha(material_index) == MATERIAL_BLEND_FACTOR_ZERO
          && GetMaterialBlendEquationAlpha(material_index)            == MATERIAL_BLEND_EQUATION_FUNC_ADD
        )
        {
          mode = MATERIAL_BLEND_MODE_NOT_USE;
        }
      }
      break;
    case nw::gfx::res::ResBlendOperation::MODE_LOGIC:
      {
        mode = MATERIAL_BLEND_MODE_LOGIC;
      }
      break;
    case nw::gfx::res::ResBlendOperation::MODE_NOT_USE:
      {
          mode = MATERIAL_BLEND_MODE_NOT_USE;
      }
      break;
    case nw::gfx::res::ResBlendOperation::MODE_BLEND:
      {
          mode = MATERIAL_BLEND_MODE_BLEND;
      }
      break;
    default:
      {
        GFL_ASSERT_MSG(0, "�u�����h���[�h���ُ�ł��B%d\n", nw_mode);
      }
      break;
    }
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return mode;
}
void Model::SetMaterialBlendMode(s32 material_index, MaterialBlendMode mode)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    switch(mode) 
    {
    case MATERIAL_BLEND_MODE_NOT_USE:
      {
    // gfx_ResMaterial.h
    //
    //! MODE_NOT_USE �����̃u�����h�p�����[�^��ݒ肷�邽�߂ɂ́A
    //! SetBlendMode(true) �Ńu�����h��L���ɂ��A
    //! SetBlendFuncSrcRgb() �� SetBlendFuncSrcAlpha() �� BLENDFACTOR_ONE ���A
    //! SetBlendFuncDstRgb() �� SetBlendFuncDstAlpha() �� BLENDFACTOR_ZERO ���A
    //! SetBlendEquationRgb() �� SetBlendEquationAlpha() �� BLENDEQ_FUNC_ADD ��ݒ肵�Ă��������B
        res_blend_operation.SetBlendMode(true);
        SetMaterialBlendFunctionSourceRgb(material_index, MATERIAL_BLEND_FACTOR_ONE);
        SetMaterialBlendFunctionSourceAlpha(material_index, MATERIAL_BLEND_FACTOR_ONE);
        SetMaterialBlendFunctionDestinationRgb(material_index, MATERIAL_BLEND_FACTOR_ZERO);
        SetMaterialBlendFunctionDestinationAlpha(material_index, MATERIAL_BLEND_FACTOR_ZERO);
        SetMaterialBlendEquationAlpha(material_index, MATERIAL_BLEND_EQUATION_FUNC_ADD);
      }
      break;
    case MATERIAL_BLEND_MODE_BLEND:
    case MATERIAL_BLEND_MODE_SEPARATE_BLEND:
      {
        res_blend_operation.SetBlendMode(true);
      }
      break;
    case MATERIAL_BLEND_MODE_LOGIC:
      {
        res_blend_operation.SetBlendMode(false);
      }
      break;
    }
    res_material.SetFragmentOperationHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
#else
Model::MaterialBlendMode Model::GetMaterialBlendMode(s32 material_index) const
{
  MaterialBlendMode mode = MATERIAL_BLEND_MODE_NOT_USE;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    mode = static_cast<MaterialBlendMode>(res_blend_operation.GetMode()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return mode;
}
void Model::SetMaterialBlendMode(s32 material_index, MaterialBlendMode mode)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    res_blend_operation.SetMode(static_cast<nw::gfx::res::ResBlendOperation::Mode>(mode)); 
    res_material.SetFragmentOperationHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
#endif

void Model::SetMaterialBlendColor(s32 material_index, f32 r, f32 g, f32 b)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    res_blend_operation.SetBlendColor(r,g,b); 
    res_material.SetFragmentOperationHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
void Model::SetMaterialBlendColor(s32 material_index, f32 r, f32 g, f32 b, f32 a)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    res_blend_operation.SetBlendColor(r,g,b,a); 
    res_material.SetFragmentOperationHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
void Model::SetMaterialBlendColor(s32 material_index, const gfl::grp::ColorF32& c)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    res_blend_operation.SetBlendColor(c); 
    res_material.SetFragmentOperationHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
void Model::SetMaterialBlendColor(s32 material_index, u8 r, u8 g, u8 b)
{
  SetMaterialBlendColor(
      material_index,
      (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX
  );
}
void Model::SetMaterialBlendColor(s32 material_index, u8 r, u8 g, u8 b, u8 a)
{
  SetMaterialBlendColor(
      material_index,
      (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(a))/gfl::grp::ColorU8::ELEMENT_MAX
  );
}
void Model::SetMaterialBlendColor(s32 material_index, const gfl::grp::ColorU8& c)
{
  SetMaterialBlendColor(
      material_index,
      (static_cast<f32>(c.r))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(c.g))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(c.b))/gfl::grp::ColorU8::ELEMENT_MAX,
      (static_cast<f32>(c.a))/gfl::grp::ColorU8::ELEMENT_MAX
  );
}
gfl::grp::ColorF32 Model::GetMaterialBlendColor(s32 material_index) const
{
  gfl::grp::ColorF32 c;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    c = res_blend_operation.GetBlendColor(); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return c;
}
void Model::GetMaterialBlendColor(s32 material_index, gfl::grp::ColorF32* c) const
{
  *c = GetMaterialBlendColor(material_index);
}
void Model::GetMaterialBlendColor(s32 material_index, gfl::grp::ColorU8*  c) const
{
  gfl::grp::ColorF32 c_f32 = GetMaterialBlendColor(material_index);
  c->Set(
    static_cast<s32>(c_f32.r*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.g*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.b*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.a*gfl::grp::ColorU8::ELEMENT_MAX)
  );
}

Model::MaterialBlendLogicOperation Model::GetMaterialBlendLogicOperation(s32 material_index) const
{
  MaterialBlendLogicOperation operation = MATERIAL_BLEND_LOGIC_OPERATION_CLEAR;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    operation = static_cast<MaterialBlendLogicOperation>(res_blend_operation.GetLogicOperation()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return operation;
}
void Model::SetMaterialBlendLogicOperation(s32 material_index, MaterialBlendLogicOperation operation)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    res_blend_operation.SetLogicOperation(static_cast<nw::gfx::res::ResBlendOperation::LogicOp>(operation)); 
    res_material.SetFragmentOperationHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}

Model::MaterialBlendFactor Model::GetMaterialBlendFunctionSourceRgb(s32 material_index) const
{
  MaterialBlendFactor factor = MATERIAL_BLEND_FACTOR_ZERO;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    factor = static_cast<MaterialBlendFactor>(res_blend_operation.GetBlendFuncSrcRgb()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return factor;
}
void Model::SetMaterialBlendFunctionSourceRgb(s32 material_index, MaterialBlendFactor factor)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    res_blend_operation.SetBlendFuncSrcRgb(static_cast<nw::gfx::res::ResBlendOperation::BlendFactor>(factor)); 
    res_material.SetFragmentOperationHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
Model::MaterialBlendFactor Model::GetMaterialBlendFunctionSourceAlpha(s32 material_index) const
{
  MaterialBlendFactor factor = MATERIAL_BLEND_FACTOR_ZERO;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    factor = static_cast<MaterialBlendFactor>(res_blend_operation.GetBlendFuncSrcAlpha()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return factor;
}
void Model::SetMaterialBlendFunctionSourceAlpha(s32 material_index, MaterialBlendFactor factor)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    res_blend_operation.SetBlendFuncSrcAlpha(static_cast<nw::gfx::res::ResBlendOperation::BlendFactor>(factor)); 
    res_material.SetFragmentOperationHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}

Model::MaterialBlendFactor Model::GetMaterialBlendFunctionDestinationRgb(s32 material_index) const
{
  MaterialBlendFactor factor = MATERIAL_BLEND_FACTOR_ZERO;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    factor = static_cast<MaterialBlendFactor>(res_blend_operation.GetBlendFuncDstRgb()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return factor;
}
void Model::SetMaterialBlendFunctionDestinationRgb(s32 material_index, MaterialBlendFactor factor)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    res_blend_operation.SetBlendFuncDstRgb(static_cast<nw::gfx::res::ResBlendOperation::BlendFactor>(factor)); 
    res_material.SetFragmentOperationHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
Model::MaterialBlendFactor Model::GetMaterialBlendFunctionDestinationAlpha(s32 material_index) const
{
  MaterialBlendFactor factor = MATERIAL_BLEND_FACTOR_ZERO;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    factor = static_cast<MaterialBlendFactor>(res_blend_operation.GetBlendFuncDstAlpha()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return factor;
}
void Model::SetMaterialBlendFunctionDestinationAlpha(s32 material_index, MaterialBlendFactor factor)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    res_blend_operation.SetBlendFuncDstAlpha(static_cast<nw::gfx::res::ResBlendOperation::BlendFactor>(factor)); 
    res_material.SetFragmentOperationHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}

Model::MaterialBlendEquation Model::GetMaterialBlendEquationRgb(s32 material_index) const
{
  MaterialBlendEquation equation = MATERIAL_BLEND_EQUATION_FUNC_ADD;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    equation = static_cast<MaterialBlendEquation>(res_blend_operation.GetBlendEquationRgb()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return equation;
}
void Model::SetMaterialBlendEquationRgb(s32 material_index, MaterialBlendEquation equation)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    res_blend_operation.SetBlendEquationRgb(static_cast<nw::gfx::res::ResBlendOperation::BlendEquation>(equation)); 
    res_material.SetFragmentOperationHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}
Model::MaterialBlendEquation Model::GetMaterialBlendEquationAlpha(s32 material_index) const
{
  MaterialBlendEquation equation = MATERIAL_BLEND_EQUATION_FUNC_ADD;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    equation = static_cast<MaterialBlendEquation>(res_blend_operation.GetBlendEquationAlpha()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return equation;
}
void Model::SetMaterialBlendEquationAlpha(s32 material_index, MaterialBlendEquation equation)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    nw::gfx::res::ResMaterial  res_material = material->GetFragmentOperationResMaterial();
    nw::gfx::res::ResFragmentOperation res_fragment_operation = res_material.GetFragmentOperation();
    nw::gfx::res::ResBlendOperation res_blend_operation = res_fragment_operation.GetBlendOperation();
    res_blend_operation.SetBlendEquationAlpha(static_cast<nw::gfx::res::ResBlendOperation::BlendEquation>(equation)); 
    res_material.SetFragmentOperationHash(0x0);  // ���ꂪ�Ȃ��Ɣ��f����Ȃ�
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �`�揇
 *                 �������̎��(�`�惌�C���[)
 *                 �f�U�C�i���G�f�B�^��Őݒ�ł���}�e���A���̕`�惌�C���[
 *                 �ŏ��ɕ`�� MATERIAL_TRANSLUCENCY_KIND_LAYER0 <= MaterialTranslucencyKind <= MATERIAL_TRANSLUCENCY_KIND_LAYER3 �Ō�ɕ`��
 *                 �ŗD�悳���`�揇 ModelLayerId > MaterialTranslucencyKind > ���̑� �ŗ�コ���`�揇
 */
//-----------------------------------------------------------------------------
Model::MaterialTranslucencyKind Model::GetMaterialTranslucencyKind(s32 material_index) const
{
  MaterialTranslucencyKind kind = MATERIAL_TRANSLUCENCY_KIND_OPAQUE;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);
    //nw::gfx::res::ResMaterial& res_material = material->GetShaderParameterResMaterial();
    nw::gfx::res::ResMaterial  res_material = material->GetShadingParameterResMaterial();
    kind = static_cast<MaterialTranslucencyKind>(res_material.GetTranslucencyKind()); 
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
  return kind;
}
void Model::SetMaterialTranslucencyKind(s32 material_index, MaterialTranslucencyKind kind)
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Model*            model        = GetNwModel();
    nw::gfx::Material*         material     = model->GetMaterial(material_index);

    /*
    { 
      nw::gfx::res::ResMaterial res_material = material->GetOriginal();
      if( res_material.IsValid() )
      {
        res_material.SetTranslucencyKind(static_cast<nw::gfx::res::ResMaterial::TranslucencyKind>(kind));
      }
    }
    */
    /*
    {
      nw::gfx::res::ResMaterial res_material = material->GetBuffer();
      if( res_material.IsValid() )
      {
        res_material.SetTranslucencyKind(static_cast<nw::gfx::res::ResMaterial::TranslucencyKind>(kind));
      }
    }
    */
    /*
    {
      nw::gfx::res::ResMaterial& res_material = material->GetShaderParameterResMaterial();
      if( res_material.IsValid() )
      {
        res_material.SetTranslucencyKind(static_cast<nw::gfx::res::ResMaterial::TranslucencyKind>(kind)); 
        res_material.SetShaderParametersHash(0x0);
      }
    }
    */
    { 
      nw::gfx::res::ResMaterial  res_material = material->GetShadingParameterResMaterial();
      if( res_material.IsValid() )
      {
        res_material.SetTranslucencyKind(static_cast<nw::gfx::res::ResMaterial::TranslucencyKind>(kind)); 
        res_material.SetShadingParameterHash(0x0);
      }
    }
  
    model->InvalidateRenderKeyCache();  // ���ꂪ�Ȃ��ƕ`�揇���ς��Ȃ�

    /*
    res_material.SetShadingParameterHash               (0x0);
    res_material.SetShaderParametersHash               (0x0);
    res_material.SetTextureCoordinatorsHash            (0x0);
    res_material.SetTextureSamplersHash                (0x0);
    res_material.SetTextureMappersHash                 (0x0);
    res_material.SetMaterialColorHash                  (0x0);
    res_material.SetRasterizationHash                  (0x0);
    res_material.SetFragmentLightingHash               (0x0);
    res_material.SetFragmentLightingTableHash          (0x0); 
    res_material.SetFragmentLightingTableParametersHash(0x0);
    res_material.SetAlphaTestHash                      (0x0);
    res_material.SetTextureCombinersHash               (0x0);
    res_material.SetFragmentOperationHash              (0x0);
    */

/*
[����][NW4C] nw::gfx::res::ResMaterial::SetTranslucencyKind() �ɂ���

�J���� : CTR_SDK 1.0.0�ANntendoWare 1.2.33

teabreak 2011-02-11 11:58:19  

�䐢�b�ɂȂ��Ă���܂��B

nw::gfx::res::ResMaterial::SetTranslucencyKind() �ŕ`�惌�C���[��ύX���Ă���̂ł����A
�������ύX����Ȃ��P�[�X���������̂Ō����𒲂ׂĂ��܂��B
GetTranslucencyKind() ���Ăяo���Ă��鑤��
nw::gfx::Material::GetShaderParameterResMaterial()
nw::gfx::Material::GetBuffer()
�ƎQ�Ƃ���}�e���A���̃��\�[�X���܂��܂��ɂȂ��Ă���A
nw::gfx::Model::FLAG_SHADER_PARAMETER ���L���ɂȂ��Ă��Ȃ��ꍇ��
Original �� Buffer �̂ǂ�����Q�Ƃ��邩�킩��Ȃ����Ƃ����̂悤�ł��B
Original �� Buffer �̗�����ύX���邱�ƂŐ������ύX���ꂽ�̂ł����A
���̂悤�ȑΉ����K�v�Ȃ̂ł��傤���H
*/

  }
  else
  {
    GFL_ASSERT_MSG( 0, "�}�e���A���̃C���f�b�N�X�ԍ����}�e���A���̐����z���Ă��܂��B\n" );
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �}�e���A���̃e�N�X�`��
 */
//-----------------------------------------------------------------------------
// material_index�ԃ}�e���A����no�ԃe�N�X�`��(0<=no<=2)��texture_resource_pack�Ŏw���e�N�X�`����ݒ肷��B
// ���X�\���Ă������e�N�X�`���͂��̃��\�[�X����������Ƃ��ɉ�������̂ŁA�����ւ����Ă�Setup���ꂽ�܂܂̏�ԂŎc���Ă��܂��B
void Model::SetMaterialTextureMapperTexture(s32 material_index, s32 no, Resource::TextureResourcePack* texture_resource_pack)
{
  SetMaterialTextureMapperTexture(material_index, no, texture_resource_pack->resource, texture_resource_pack->index_in_resource);
}

// material_index�ԃ}�e���A����no�ԃe�N�X�`��(0<=no<=2)��texture_resource�Ŏw���e�N�X�`����ݒ肷��B
// ���X�\���Ă������e�N�X�`���͂��̃��\�[�X����������Ƃ��ɉ�������̂ŁA�����ւ����Ă�Setup���ꂽ�܂܂̏�ԂŎc���Ă��܂��B
void Model::SetMaterialTextureMapperTexture(s32 material_index, s32 no, Resource* texture_resource, s32 index_in_resource)
{
  nw::gfx::ResGraphicsFile* nw_res_graphics_file = texture_resource->GetNwResGraphicsFile();
  nw::gfx::ResTexture       res_texture          = nw_res_graphics_file->GetTextures(index_in_resource);
  setMaterialTextureMapperTexture(material_index, no, res_texture);
}

// material_index�ԃ}�e���A����no�ԃe�N�X�`��(0<=no<=2)��texture�Ŏw���e�N�X�`����ݒ肷��B
// ���X�\���Ă������e�N�X�`���͂��̃��\�[�X����������Ƃ��ɉ�������̂ŁA�����ւ����Ă�Setup���ꂽ�܂܂̏�ԂŎc���Ă��܂��B
void Model::SetMaterialTextureMapperTexture(s32 material_index, s32 no, Texture* texture)
{
  nw::gfx::ResTexture res_texture = texture->GetNwResTexture();
  setMaterialTextureMapperTexture(material_index, no, res_texture);
}

void Model::setMaterialTextureMapperTexture(s32 material_index, s32 no, nw::gfx::ResTexture res_texture)
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureMapperResMaterial();
  nw::gfx::ResPixelBasedTextureMapper res_texture_mapper = res_material.GetTextureMappers(no);

  // ���X�\���Ă������e�N�X�`���͂��̂܂܂ɂ��Ă���
  //nw::gfx::ResTexture dummy_texture                      = res_texture_mapper.GetTexture().Dereference();
  //dummy_texture.Cleanup();

#if 0
/*[����][NW4C] nw::ut::SafeCleanup() �̌Ăяo����� IsValid() �̒l�ɂ���

�c����NTSC 2011-02-25 09:31:42  
�͂��B�d�l�ł��B
IsValid�͂����܂ł����\�[�X�f�[�^��null���ǂ����̃`�F�b�N�ł��B
�΂���Cleanup��Setup�̋t�ɑ������鏈���ł��̂ŁA
IsValid��Setup�ς݂��ǂ����̃`�F�b�N�ɂ͎g�p���邱�Ƃ͂ł��܂���B*/
#endif

  // �e�N�X�`���������ւ���
  if( res_texture_mapper.IsValid() )
  {
    // �R�}���h�~��
    res_texture_mapper.SetTexture(res_texture);
        // NintendoWare for CTR �֐����t�@�����X�}�j���A��
        // �e�N�X�`���}�b�p�[�Ƀe�N�X�`����ݒ肵�܂��B �e�N�X�`����ݒ肵����ɃR�}���h�̐������s���܂��B

    // �e�N�X�`����ύX�����ꍇ�̓e�N�X�`���}�b�p�[�̃n�b�V�����O�ɐݒ肵�܂��B
    res_material.SetTextureMappersHash(0x0);
  }
  else
  {
    //GFL_ASSERT_MSG( 0, "nw::gfx::ResPixelBasedTextureMapper�̃��\�[�X�ւ̃|�C���^��NULL�ł��B\n" );  // @note �ꎞ�I�ɃR�����g�A�E�g�B�R�����g�A�E�g�͂�߂āA����ASSERT�������悤�ɖ߂��Ă������ق��������B
  }
}


// material_index�ԃ}�e���A����no�ԃe�N�X�`��(0<=no<=2)��U�����J��Ԃ��ݒ���s���B
void Model::SetMaterialTextureMapperWrapU(s32 material_index, s32 no, MaterialTextureWrap wrap)
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureMapperResMaterial();
  nw::gfx::ResPixelBasedTextureMapper res_texture_mapper = res_material.GetTextureMappers(no);

  if( res_texture_mapper.IsValid() )
  {
    // �e�N�X�`���T���v���[
    nw::gfx::ResTextureSampler res_texure_sampler = res_texture_mapper.GetSampler();
    nw::gfx::ResStandardTextureSampler res_standard_texure_sampler = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_texure_sampler);
  
    res_standard_texure_sampler.SetWrapS( static_cast<nw::gfx::res::ResStandardTextureSampler::Wrap>(wrap) );  // �R�}���h�~�ς�?
    // �n�b�V��
    res_material.SetTextureSamplersHash(0x0);
  }
  else
  {
    //GFL_ASSERT_MSG( 0, "nw::gfx::ResPixelBasedTextureMapper�̃��\�[�X�ւ̃|�C���^��NULL�ł��B\n" );  // @note �ꎞ�I�ɃR�����g�A�E�g�B�R�����g�A�E�g�͂�߂āA����ASSERT�������悤�ɖ߂��Ă������ق��������B
  }
}
// material_index�ԃ}�e���A����no�ԃe�N�X�`��(0<=no<=2)��U�����J��Ԃ��ݒ�𓾂�B
Model::MaterialTextureWrap  Model::GetMaterialTextureMapperWrapU(s32 material_index, s32 no) const
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureMapperResMaterial();
  nw::gfx::ResPixelBasedTextureMapper res_texture_mapper = res_material.GetTextureMappers(no);

  if( res_texture_mapper.IsValid() )
  {
    // �e�N�X�`���T���v���[
    nw::gfx::ResTextureSampler res_texure_sampler = res_texture_mapper.GetSampler();
    nw::gfx::ResStandardTextureSampler res_standard_texure_sampler = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_texure_sampler);
  
    return  static_cast<MaterialTextureWrap>( res_standard_texure_sampler.GetWrapS() );
  }
  else
  {
    //GFL_ASSERT_MSG( 0, "nw::gfx::ResPixelBasedTextureMapper�̃��\�[�X�ւ̃|�C���^��NULL�ł��B\n" );  // @note �ꎞ�I�ɃR�����g�A�E�g�B�R�����g�A�E�g�͂�߂āA����ASSERT�������悤�ɖ߂��Ă������ق��������B
    return MATERIAL_TEXTURE_WRAP_CLAMP_TO_EDGE;
  }
}
// material_index�ԃ}�e���A����no�ԃe�N�X�`��(0<=no<=2)��V�����J��Ԃ��ݒ���s���B
void Model::SetMaterialTextureMapperWrapV(s32 material_index, s32 no, MaterialTextureWrap wrap)
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureMapperResMaterial();
  nw::gfx::ResPixelBasedTextureMapper res_texture_mapper = res_material.GetTextureMappers(no);

  if( res_texture_mapper.IsValid() )
  {
    // �e�N�X�`���T���v���[
    nw::gfx::ResTextureSampler res_texure_sampler = res_texture_mapper.GetSampler();
    nw::gfx::ResStandardTextureSampler res_standard_texure_sampler = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_texure_sampler);
  
    res_standard_texure_sampler.SetWrapT( static_cast<nw::gfx::res::ResStandardTextureSampler::Wrap>(wrap) );  // �R�}���h�~�ς�?
    // �n�b�V��
    res_material.SetTextureSamplersHash(0x0);
  }
  else
  {
    //GFL_ASSERT_MSG( 0, "nw::gfx::ResPixelBasedTextureMapper�̃��\�[�X�ւ̃|�C���^��NULL�ł��B\n" );  // @note �ꎞ�I�ɃR�����g�A�E�g�B�R�����g�A�E�g�͂�߂āA����ASSERT�������悤�ɖ߂��Ă������ق��������B
  }
}
// material_index�ԃ}�e���A����no�ԃe�N�X�`��(0<=no<=2)��V�����J��Ԃ��ݒ�𓾂�B
Model::MaterialTextureWrap  Model::GetMaterialTextureMapperWrapV(s32 material_index, s32 no) const
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureMapperResMaterial();
  nw::gfx::ResPixelBasedTextureMapper res_texture_mapper = res_material.GetTextureMappers(no);

  if( res_texture_mapper.IsValid() )
  {
    // �e�N�X�`���T���v���[
    nw::gfx::ResTextureSampler res_texure_sampler = res_texture_mapper.GetSampler();
    nw::gfx::ResStandardTextureSampler res_standard_texure_sampler = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_texure_sampler);
  
    return  static_cast<MaterialTextureWrap>( res_standard_texure_sampler.GetWrapT() );
  }
  else
  {
    //GFL_ASSERT_MSG( 0, "nw::gfx::ResPixelBasedTextureMapper�̃��\�[�X�ւ̃|�C���^��NULL�ł��B\n" );  // @note �ꎞ�I�ɃR�����g�A�E�g�B�R�����g�A�E�g�͂�߂āA����ASSERT�������悤�ɖ߂��Ă������ق��������B
    return MATERIAL_TEXTURE_WRAP_CLAMP_TO_EDGE;
  }
}
// �e�N�X�`���̍��W�́AST�͎g�킸�AUV�œ��ꂵ�Ă����B


// material_index�ԃ}�e���A����no�ԃe�N�X�`���R�[�f�B�l�[�^(0<=no<=2)�̐ݒ�
// texture_mapper_no�ԃe�N�X�`���}�b�p�[��texture_coordinator_no�ԃe�N�X�`���R�[�f�B�l�[�^���������ꍇ�A
// �u2�ԃe�N�X�`���}�b�p�[�Ȃ�2�ԃe�N�X�`���R�[�f�B�l�[�^���g���v�Ƃ����ӂ��ɕK�������Ȃ��Ă���킯�ł͂Ȃ��B
// �u1�ԃe�N�X�`���}�b�p�[��2�ԃe�N�X�`���}�b�p�[��1�ԃe�N�X�`���R�[�f�B�l�[�^�����L���Ďg���v�Ƃ������Ƃ����蓾��B
// �܂�texture_mapper_no=texture_coordinator_no�ƕK���Ȃ�킯�ł͂Ȃ��A�Ƃ������Ƃɒ��ӂ��ĉ������B
void Model::SetMaterialTextureCoordinatorScaleUV(s32 material_index, s32 no, const gfl::math::VEC2& t)
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureCoordinatorResMaterial();
  nw::gfx::ResTextureCoordinator res_texture_coordinator = res_material.GetTextureCoordinators(no);  // �����IsValid�m�F���Ȃ��Ă����v���������A���܂��܃A�N�Z�X�����ԍ��̂��̂��p�ӂ���Ă���������������Ȃ��B

  // �R�}���h�~��
  res_texture_coordinator.SetScale(t);
  // �n�b�V��
  res_material.SetTextureCoordinatorsHash(0x0);
}
void Model::GetMaterialTextureCoordinatorScaleUV(s32 material_index, s32 no, gfl::math::VEC2* t) const
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureCoordinatorResMaterial();
  nw::gfx::ResTextureCoordinator res_texture_coordinator = res_material.GetTextureCoordinators(no);

  *t = res_texture_coordinator.GetScale();
}

void Model::SetMaterialTextureCoordinatorRotateUV(s32 material_index, s32 no, const f32 t)  // [radian]
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureCoordinatorResMaterial();
  nw::gfx::ResTextureCoordinator res_texture_coordinator = res_material.GetTextureCoordinators(no);  // �����IsValid�m�F���Ȃ��Ă����v���������A���܂��܃A�N�Z�X�����ԍ��̂��̂��p�ӂ���Ă���������������Ȃ��B

  // �R�}���h�~��
  res_texture_coordinator.SetRotate(t);
  // �n�b�V��
  res_material.SetTextureCoordinatorsHash(0x0);
}
f32 Model::GetMaterialTextureCoordinatorRotateUV(s32 material_index, s32 no) const  // [radian]
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureCoordinatorResMaterial();
  nw::gfx::ResTextureCoordinator res_texture_coordinator = res_material.GetTextureCoordinators(no); 

  return res_texture_coordinator.GetRotate();
}

void Model::SetMaterialTextureCoordinatorTranslateUV(s32 material_index, s32 no, const gfl::math::VEC2& t)
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureCoordinatorResMaterial();
  nw::gfx::ResTextureCoordinator res_texture_coordinator = res_material.GetTextureCoordinators(no);  // �����IsValid�m�F���Ȃ��Ă����v���������A���܂��܃A�N�Z�X�����ԍ��̂��̂��p�ӂ���Ă���������������Ȃ��B

  // �R�}���h�~��
  res_texture_coordinator.SetTranslate(t);
  // �n�b�V��
  res_material.SetTextureCoordinatorsHash(0x0);
}
void Model::GetMaterialTextureCoordinatorTranslateUV(s32 material_index, s32 no, gfl::math::VEC2* t) const
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureCoordinatorResMaterial();
  nw::gfx::ResTextureCoordinator res_texture_coordinator = res_material.GetTextureCoordinators(no); 

  *t = res_texture_coordinator.GetTranslate();
}

void Model::SetMaterialTextureCoordinatorReferenceSceneCameraIndex(s32 material_index, s32 no, s32 scene_camera_index)
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureCoordinatorResMaterial();
  nw::gfx::ResTextureCoordinator res_texture_coordinator = res_material.GetTextureCoordinators(no); 

  res_texture_coordinator.SetReferenceCamera(scene_camera_index);  // �R�}���h�~�ς�?
  // �n�b�V��
  res_material.SetTextureCoordinatorsHash(0x0);
}
s32  Model::GetMaterialTextureCoordinatorReferenceSceneCameraIndex(s32 material_index, s32 no) const
{
  nw::gfx::Model*       model        = GetNwModel();
  nw::gfx::Material*    material     = model->GetMaterial(material_index);
  nw::gfx::ResMaterial  res_material = material->GetTextureCoordinatorResMaterial();
  nw::gfx::ResTextureCoordinator res_texture_coordinator = res_material.GetTextureCoordinators(no); 

  return res_texture_coordinator.GetReferenceCamera();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ���b�V��
 */
//-----------------------------------------------------------------------------
s32 Model::GetMeshCount() const
{
/*
#if GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD==0
*/
  // ���̕��@�Ő������擾�ł���
  nw::gfx::res::ResMeshArray res_mesh_array = GetNwModel()->GetResMeshes();
  return res_mesh_array.size();

/*
#elif GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD==1
  // ���̕��@�Ő������擾�ł���
  nw::gfx::res::ResModel res_model = GetNwModel()->GetResModel();
  return res_model.GetMeshesCount();

#endif
*/
}
s32 Model::GetMeshIndex(const char* key) const
{
/*
#if GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD==0
*/
/*
  // ���b�V���ɖ��O��t�����f�[�^���Ȃ��̂ŁA���̕��@�Ő������擾�ł��邩�ǂ����s��
  nw::gfx::res::ResMeshArray res_mesh_array = GetNwModel()->GetResMeshes();
  s32 count = res_mesh_array.size();
  for( s32 i=0; i<count; ++i )
  {
    nw::gfx::res::ResMesh res_mesh = res_mesh_array[i];
    if( gfl::std::StringCmp( key, res_mesh.GetName() ) ==  0 ) return i;
  }
  return MESH_INDEX_NOT_FOUND;
*/

/*
#elif GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD==1
  // ���b�V���ɖ��O��t�����f�[�^���Ȃ��̂ŁA���̕��@�Ő������擾�ł��邩�ǂ����s��
  nw::gfx::res::ResModel res_model = GetNwModel()->GetResModel();
  s32 count = res_model.GetMeshesCount();
  for( s32 i=0; i<count; ++i )
  {
    nw::gfx::res::ResMesh res_mesh = res_model.GetMeshes(i);
    if( gfl::std::StringCmp( key, res_mesh.GetName() ) ==  0 ) return i;
  }
  return MESH_INDEX_NOT_FOUND;

#endif
*/


#if 0
/*[����][NW4C] ���b�V�������擾����ɂ́H
�J���� : NintendoWare for CTR 2.2.0 
segR 2012-01-18 21:15:15 

�c����NTSC 2012-01-19 15:39:16  
�\���󂠂�܂���B
ResMesh�Ɋւ��Ă͒��ږ��O���ݒ肳��Ă��Ȃ��悤�ł��B

���L�̂悤�ɂ��ĊԐړI�Ɏ擾������@�ł��Ή����������܂��ł��傤���H
        {
            nw::gfx::Model *pModel = (nw::gfx::Model *)node;
            nw::gfx::ResMeshArray oArray = pModel->GetResModel().GetMeshes();
            for(u32 i = 0; i < oArray.size(); i++){
                s32 nVisIdx = oArray[i].GetMeshNodeVisibilityIndex();
                nw::gfx::ResMeshNodeVisibility oMeshNode = pModel->GetResModel().GetMeshNodeVisibilities(nVisIdx);
                NN_LOG("mesh3 name:%s\n", oMeshNode.GetName());
            }
        }*/
#endif

  {
      nw::gfx::Model *pModel = GetNwModel();
      nw::gfx::ResMeshArray oArray = pModel->GetResModel().GetMeshes();
      for(u32 i = 0; i < oArray.size(); i++){
          s32 nVisIdx = oArray[i].GetMeshNodeVisibilityIndex();
          if(nVisIdx>=0)  // pModel->GetResModel().GetMeshNodeVisibilities(nVisIdx)��Assertion failure at gfx_ResModel.h:155
          {               // NW:Failed assertion 0<= (idx) && static_cast<s32>(idx) < static_cast<s32>(GetMeshNodeVisibilitiesCount())�Ƃ����G���[�ɂȂ����̂ŁA�l���`�F�b�N���邱�Ƃɂ����B  GAME FREAK inc.  Koji Kawada
            nw::gfx::ResMeshNodeVisibility oMeshNode = pModel->GetResModel().GetMeshNodeVisibilities(nVisIdx);
            if( gfl::std::StringCmp( key, oMeshNode.GetName() ) ==  0 ) return i;
          }
      }
      return MESH_INDEX_NOT_FOUND;
  }
}

b32 Model::IsMeshVisible(const s32 mesh_index) const
{
  nw::gfx::res::ResMeshArray res_mesh_array = GetNwModel()->GetResMeshes();
  s32 count = res_mesh_array.size();
  if( mesh_index < count )
  {
    nw::gfx::res::ResMesh res_mesh = res_mesh_array[mesh_index];
    return res_mesh.IsVisible();
  }
  else
  {
    GFL_ASSERT_MSG(0, "mesh_index=%d, count=%d\n", mesh_index, count);
    return false;
  }
}
void Model::SetMeshVisible(const s32 mesh_index, const b32 visible)
{
  nw::gfx::res::ResMeshArray res_mesh_array = GetNwModel()->GetResMeshes();
  s32 count = res_mesh_array.size();
  if( mesh_index < count )
  {
    nw::gfx::res::ResMesh res_mesh = res_mesh_array[mesh_index];
    res_mesh.SetVisible(visible);
  }
  else
  {
    GFL_ASSERT_MSG(0, "mesh_index=%d, count=%d\n", mesh_index, count);
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ���[�U�f�[�^
 */
//-----------------------------------------------------------------------------
// ���f��
s32 Model::GetModelUserDataCount() const
{
  nw::gfx::res::ResModel res_model = GetNwModel()->GetResModel();
  return res_model.GetUserDataCount();
}
s32 Model::GetModelUserDataIndex(const char* key) const
{
#if 0
/*  NintendoWare/CTR/include/nw/ut/
  ut_ResDictionary.h

    // �L�[������̒l�Ŏ������������A�C���f�N�X���擾���܂��B
    // �w��̃L�[��������Ȃ������ꍇ�ɂ́A���̐���Ԃ��܂��B
    s32     GetIndex(const char* key) const;
    s32     GetIndex(const ResName key) const
    {
        if ( (! this->IsValid()) || (! key.IsValid()) ) { return -1; }
        return this->GetIndex(key.GetName());
    }*/
#endif

  nw::gfx::res::ResModel res_model = GetNwModel()->GetResModel();
  s32 index = res_model.GetUserDataIndex(key);
  if( index >= 0 ) return index;                      // ���t����
  else             return USER_DATA_INDEX_NOT_FOUND;  // ���t����Ȃ�����
}
void Model::GetModelUserData(const s32 user_data_index, UserData* user_data) const
{
  GFL_ASSERT_MSG(user_data, "�擾�������[�U�f�[�^���i�[����user_data��NULL�ł��B\n");  //@fix
  if(!user_data) return;
  nw::gfx::res::ResModel res_model = GetNwModel()->GetResModel();
  nw::ut::ResMetaData res_meta_data = res_model.GetUserData(user_data_index);
  user_data->SetNwResMetaData(res_meta_data);
}

// �}�e���A��
s32 Model::GetMaterialUserDataCount(const s32 material_index) const
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Material*        material     = GetNwModel()->GetMaterial(material_index);
    nw::gfx::res::ResMaterial res_material = material->GetOriginal();
    return res_material.GetUserDataCount(); 
  }
  else
  {
    GFL_ASSERT_MSG(0, "�}�e���A���̃C���f�b�N�X%d>=%d���s���ł��B\n", material_index, count);
    return 0;
  }
}
s32 Model::GetMaterialUserDataIndex(const s32 material_index, const char* key) const
{
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Material*        material     = GetNwModel()->GetMaterial(material_index);
    nw::gfx::res::ResMaterial res_material = material->GetOriginal();
    return res_material.GetUserDataIndex(key);  // @note �s���̂Ƃ�USER_DATA_INDEX_NOT_FOUND��Ԃ������B���s���̂Ƃ���-1�Ȃ̂�USER_DATA_INDEX_NOT_FOUND�Ɠ����l�B
  }
  else
  {
    GFL_ASSERT_MSG(0, "�}�e���A���̃C���f�b�N�X%d>=%d���s���ł��B\n", material_index, count);
    return USER_DATA_INDEX_NOT_FOUND; 
  }
}
void Model::GetMaterialUserData(const s32 material_index, const s32 user_data_index, UserData* user_data) const
{
  GFL_ASSERT_MSG(user_data, "�擾�������[�U�f�[�^���i�[����user_data��NULL�ł��B\n");  //@fix
  if(!user_data) return;
  s32 count = GetMaterialCount();
  if( material_index < count )
  {
    nw::gfx::Material*        material     = GetNwModel()->GetMaterial(material_index);
    nw::gfx::res::ResMaterial res_material = material->GetOriginal();
    nw::ut::ResMetaData res_meta_data = res_material.GetUserData(user_data_index);
    user_data->SetNwResMetaData(res_meta_data);
  }
  else
  {
    GFL_ASSERT_MSG(0, "�}�e���A���̃C���f�b�N�X%d>=%d���s���ł��B\n", material_index, count);
  }
}

// ���b�V��
s32 Model::GetMeshUserDataCount(const s32 mesh_index) const
{
/*
#if GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD==0
*/
  // ���̕��@�Ő������擾�ł���
  s32 count = GetMeshCount();
  if( mesh_index < count )
  {
    nw::gfx::res::ResMeshArray res_mesh_array = GetNwModel()->GetResMeshes();
    nw::gfx::res::ResMesh      res_mesh       = res_mesh_array[mesh_index];
    return res_mesh.GetUserDataCount();
  }
  else
  {
    GFL_ASSERT_MSG(0, "���b�V���̃C���f�b�N�X%d>=%d���s���ł��B\n", mesh_index, count);
    return 0; 
  }

/*
#elif GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD==1
  // ���̕��@�Ő������擾�ł���
  s32 count = GetMeshCount();
  if( mesh_index < count )
  {
    nw::gfx::res::ResModel res_model = GetNwModel()->GetResModel();
    nw::gfx::res::ResMesh  res_mesh  = res_model.GetMeshes(mesh_index);
    return res_mesh.GetUserDataCount();
  }
  else
  {
    GFL_ASSERT_MSG(0, "���b�V���̃C���f�b�N�X%d>=%d���s���ł��B\n", mesh_index, count);
    return 0; 
  }

#endif
*/
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool Model::GetModelAABB( gfl::math::AABB *p_dest_aabb ) const 
{
  if ( this->GetModelUserDataCount() == 0 )
    return false;
  
  s32   user_data_index = this->GetModelUserDataIndex("BBoxMinMax");

  if ( user_data_index < 0 )
    return false;
  
  UserData    user_data;
  this->GetModelUserData( user_data_index, &user_data );

  p_dest_aabb->SetMinMax(gfl::math::VEC4( user_data.GetFloatValue(0), user_data.GetFloatValue(1), user_data.GetFloatValue(2), 0.0f), 
                         gfl::math::VEC4( user_data.GetFloatValue(3), user_data.GetFloatValue(4), user_data.GetFloatValue(5), 0.0f) );

  return true;
}

s32 Model::GetMeshUserDataIndex(const s32 mesh_index, const char* key) const
{
#if GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD==0
  // ���̕��@�ł͐������擾�ł��Ȃ�
  s32 count = GetMeshCount();
  if( mesh_index < count )
  {
    nw::gfx::res::ResMeshArray res_mesh_array = GetNwModel()->GetResMeshes();
    nw::gfx::res::ResMesh      res_mesh       = res_mesh_array[mesh_index];
    return res_mesh.GetUserDataIndex(key);  // @note �s���̂Ƃ�USER_DATA_INDEX_NOT_FOUND��Ԃ������B���s���̂Ƃ���-1�Ȃ̂�USER_DATA_INDEX_NOT_FOUND�Ɠ����l�B
  }
  else
  {
    GFL_ASSERT_MSG(0, "���b�V���̃C���f�b�N�X%d>=%d���s���ł��B\n", mesh_index, count);
    return USER_DATA_INDEX_NOT_FOUND; 
  }

#elif GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD==1
  // ���̕��@�Ő������擾�ł���
  s32 count = GetMeshCount();
  if( mesh_index < count )
  {
    nw::gfx::res::ResModel res_model = GetNwModel()->GetResModel();
    nw::gfx::res::ResMesh  res_mesh  = res_model.GetMeshes(mesh_index);
    return res_mesh.GetUserDataIndex(key);  // @note �s���̂Ƃ�USER_DATA_INDEX_NOT_FOUND��Ԃ������B���s���̂Ƃ���-1�Ȃ̂�USER_DATA_INDEX_NOT_FOUND�Ɠ����l�B
  }
  else
  {
    GFL_ASSERT_MSG(0, "���b�V���̃C���f�b�N�X%d>=%d���s���ł��B\n", mesh_index, count);
    return USER_DATA_INDEX_NOT_FOUND; 
  }

#endif
}
void Model::GetMeshUserData(const s32 mesh_index, const s32 user_data_index, UserData* user_data) const
{
#if GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD==0
  // ���̕��@�ł͐������擾�ł��Ȃ�
  GFL_ASSERT_MSG(user_data, "�擾�������[�U�f�[�^���i�[����user_data��NULL�ł��B\n");  //@fix
  if(!user_data) return;
  s32 count = GetMeshCount();
  if( mesh_index < count )
  {
    nw::gfx::res::ResMeshArray res_mesh_array = GetNwModel()->GetResMeshes();
    nw::gfx::res::ResMesh      res_mesh       = res_mesh_array[mesh_index];
    nw::ut::ResMetaData res_meta_data = res_mesh.GetUserData(user_data_index);
    user_data->SetNwResMetaData(res_meta_data);
  }
  else
  {
    GFL_ASSERT_MSG(0, "���b�V���̃C���f�b�N�X%d>=%d���s���ł��B\n", mesh_index, count);
  }

#elif GFL_GRP_G3D_MODEL_MESH_GETTER_METHOD==1
  // ���̕��@�Ő������擾�ł���
  GFL_ASSERT_MSG(user_data, "�擾�������[�U�f�[�^���i�[����user_data��NULL�ł��B\n");  //@fix
  if(!user_data) return;
  s32 count = GetMeshCount();
  if( mesh_index < count )
  {
    nw::gfx::res::ResModel res_model = GetNwModel()->GetResModel();
    nw::gfx::res::ResMesh  res_mesh  = res_model.GetMeshes(mesh_index);
    nw::ut::ResMetaData res_meta_data = res_mesh.GetUserData(user_data_index);
    user_data->SetNwResMetaData(res_meta_data);
  }
  else
  {
    GFL_ASSERT_MSG(0, "���b�V���̃C���f�b�N�X%d>=%d���s���ł��B\n", mesh_index, count);
  }

#endif
}


//-----------------------------------------------------------------------------
/*
 *  @brief         �v�Z�����ɂČĂяo�����R�[���o�b�N
 */
//-----------------------------------------------------------------------------
// �r���ŃR�[���o�b�N���Ȃ��ɂ���ꍇ�́A�߂�l���o���Ă����ĉ������B
// �R�[���o�b�N��ǉ��ł��Ȃ������Ƃ�CALLBACK_INDEX_NOT_ADD��Ԃ��B
s32 Model::AddCalculateCallback(ModelCalculateCallbackFunction function, void* work)
{
  s32 index = CALLBACK_INDEX_NOT_ADD;
  for( u32 i=0; i<m_calculate_callback_num_max; ++i )
  {
    if( !m_calculate_callback_function_array[i] )
    {
      m_calculate_callback_function_array[i] = function;
      m_calculate_callback_work_array[i] = work;
      index = i;
      break;
    }
  }
  GFL_ASSERT_MSG(index>=0, "�R�[���o�b�N������%d���邽�߁A�ǉ��ł��܂���ł����B\n", m_calculate_callback_num_max);  //@check
  return index;
}
// ����calculate_callback_index�ɂ�AddCalculateCallback�̖߂�l��n���ĉ������B
void Model::RemoveCalculateCallback(s32 calculate_callback_index)
{
  if( 0<=calculate_callback_index && calculate_callback_index<m_calculate_callback_num_max )
  {
    m_calculate_callback_function_array[calculate_callback_index] = NULL;
    m_calculate_callback_work_array[calculate_callback_index] = NULL;
  }
  else
  {
    GFL_ASSERT_MSG(0, "calculate_callback_index=%d���s���ł��B�R�[���o�b�N�͍ő�%d�ł��B\n", calculate_callback_index, m_calculate_callback_num_max);
  }
}


#if 0
//������
//-----------------------------------------------------------------------------
/*
 *  @brief         �v�Z�t���O
 *                 true�̂Ƃ��v�Z�����(@note �S���H�ꕔ�H)
 *                 ���������ꂪtrue�̂Ƃ��ł��AGetTreeFlag��false�̂Ƃ��͌v�Z����Ȃ�
 */
//-----------------------------------------------------------------------------
void Model::SetCalculateFlag(b32 flag)
{
  flag;
}
b32  Model::GetCalculateFlag(void) const
{
  return true;
}
#endif

//-----------------------------------------------------------------------------
/*
 *  @brief         �`��t���O
 *                 true�̂Ƃ��`�悳���
 *                 ���������ꂪtrue�̂Ƃ��ł��AGetTreeFlag��false�̂Ƃ��͕`�悳��Ȃ�
 */
//-----------------------------------------------------------------------------
void Model::SetVisible(b32 visible)
{
  nw::gfx::Model* model = GetNwModel();
  model->SetVisible(visible);
}
b32  Model::IsVisible(void) const
{
  nw::gfx::Model* model = GetNwModel();
  return model->IsVisible();
}

#if 0
//SceneNode�ֈڍs����
//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[���c���[�t���O
 *                 true�̂Ƃ�Scene��Add����Ă�����V�[���c���[�ɓ������Afalse�̂Ƃ�Scene��Add����Ă��Ă��V�[���c���[����O�����
 *                 ���ꂪtrue�ł��邱�ƂƁA���[�U��Scene��Add�������Ƃ́A�����ł͂���܂���B
 */
//-----------------------------------------------------------------------------
void Model::SetTreeFlag(b32 flag)
{
  m_tree_flag = flag;
}
b32  Model::GetTreeFlag(void) const
{
  return m_tree_flag;
}
#endif

#if 0
//������
//-----------------------------------------------------------------------------
/*
 *  @brief         �v�Z����邩�ǂ���
 */
//-----------------------------------------------------------------------------
Model::CalculateState Model::GetCalculateState(void) const
{
  return CALCULATE_STATE_NO;
}
#endif

//-----------------------------------------------------------------------------
/*
 *  @brief         �`�悳��邩�ǂ���
 */
//-----------------------------------------------------------------------------
Model::DrawState Model::GetDrawState(void) const
{
  if( IsVisible() && GetTreeFlag() ) return DRAW_STATE_OK;
  return DRAW_STATE_NO;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V���������蓖�Ă�X���b�g�����p�ӂ���Ă��邩
 *
 *  @retval        u32    �p�ӂ���Ă���X���b�g�̌�
 */
//-----------------------------------------------------------------------------
u32 Model::GetAnimationSlotNum(void) const
{
  //u32 slot_num = 0;
  //if( m_animation_manager )
  //{
  //  slot_num = m_animation_manager->GetSlotNum();
  //}
  //return slot_num;
  return m_animation_slot_num;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V���������蓖�Ă�X���b�g�ɃA�j���[�V���������蓖�Ă��Ă��邩
 *
 *  @param[in]     slot_index    �A�j���[�V���������蓖�Ă�X���b�g�̃C���f�b�N�X
 *
 *  @retval        b32    �A�j���[�V���������蓖�Ă��Ă�����true
 */
//-----------------------------------------------------------------------------
b32 Model::IsAnimationSlotHavingAnimation(u32 slot_index) const  // having�͉p��Ƃ��ċC�ɂȂ�
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "�X���b�g�̃C���f�b�N�X%d���s���ł��B\n", slot_index );
    return false;
  }

  return ( m_animation_slot_array[slot_index].GetState() != AnimationSlot::STATE_NONE );
#endif
  if( m_animation_manager )
  {
    if( m_animation_manager->GetSlotAnimationNum(slot_index) > 0 )
    {
      return true;
    }
  }
  return false;
}


//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����X���b�g�̗L��/�������擾����
 *
 *  @param[in]     slot_index    �A�j���[�V���������蓖�Ă�X���b�g�̃C���f�b�N�X
 *
 *  @retval        b32    �A�j���[�V�����X���b�g���L���Ȃ�true
 */
//-----------------------------------------------------------------------------
b32  Model::IsAnimationSlotEnable(u32 slot_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->IsSlotEnable(slot_index);
  }
  return false;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����X���b�g�̗L��/������ݒ肷��
 *
 *  @param[in]     slot_index    �A�j���[�V���������蓖�Ă�X���b�g�̃C���f�b�N�X
 *  @param[in]     enable        �A�j���[�V�����X���b�g��L���ɂ���Ȃ�true
 */
//-----------------------------------------------------------------------------
void Model::SetAnimationSlotEnable(u32 slot_index, b32 enable)
{
  if( m_animation_manager )
  {
    m_animation_manager->SetSlotEnable(slot_index, enable);
  }
}


//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V������ύX����
 *                 slot_index�Ŏw�肵���X���b�g�ɑ΂��ď��߂Ă��̊֐����Ă񂾂Ƃ��́A�ΏۃX���b�g��Animation�𐶐�����B
 *                 slot_index�Ŏw�肵���X���b�g�ɑ΂��ď��߂�change_frame>0�ł��̊֐����Ă񂾂Ƃ����A�ΏۃX���b�g��Animation�𐶐�����B
 *
 *  @param[in]     allocator               �A���P�[�^
 *  @param[in]     res_anim                �A�j���[�V�������\�[�X(NULL��n���ƑS��(�X�P���^���A�}�e���A���A�r�W�r���e�B)�̃A�j���[�V�������������܂�)
 *  @param[in]     change_frame            ���t���[���ŕύX���邩(���̊ԃu�����h�����)
 *  @param[in]     slot_index              �A�j���[�V���������蓖�Ă�X���b�g�̃C���f�b�N�X
 *  @param[in]     max_anim_members        �A�j���[�V�������郁���o�̍ő吔�B(res_anim��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
 *                                         slot_index�Ŏw�肵���X���b�g�ɑ΂��ď��߂Ă��̊֐����Ă񂾂Ƃ��́A�g�p�����B
 *                                         slot_index�Ŏw�肵���X���b�g�ɑ΂��ď��߂�change_frame>0�ł��̊֐����Ă񂾂Ƃ����A�g�p�����B
 *                                         Resource::Get???MemberAnimSetCount�œ����l��n���Ƃ悢�B
 *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST��n���ƁA
 *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X��GetMemberAnimSetCount���g�p���܂��B
 *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERS��n���ƁA
 *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X�̃A�j���[�V�����O���[�v�̃����o���ɁA
 *                                         �����A���̃A�j���[�V�����^�C�v�ɂ�����ő吔�ɂ��܂��B
 *  @param[in]     force                   �����I�ɃA�j���[�V�������֘A�t���܂��B(res_anim��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
 */
//-----------------------------------------------------------------------------
void Model::ChangeAnimation(
  gfl::heap::NwAllocator* allocator,
  ResAnim*                res_anim,
  u32                     change_frame,
  u32                     slot_index,
  s32                     max_anim_members,
  bool                    force
)
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "�X���b�g�̃C���f�b�N�X%d���s���ł��B\n", slot_index );
    return;
  }

  m_animation_slot_array[slot_index].ChangeAnimation(
      this,
      allocator,
      res_anim,
      change_frame,
      max_anim_members,
      force
  );
#endif
  ChangeUnitAnimation(
      allocator,
      allocator,
      slot_index,
      0,
      res_anim,
      change_frame,
      max_anim_members,
      force
  );
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V������ύX����
 *                 slot_index�Ŏw�肵���X���b�g�ɑ΂��ď��߂Ă��̊֐����Ă񂾂Ƃ��́A�ΏۃX���b�g��Animation�𐶐�����B
 *                 slot_index�Ŏw�肵���X���b�g�ɑ΂��ď��߂�change_frame>0�ł��̊֐����Ă񂾂Ƃ����A�ΏۃX���b�g��Animation�𐶐�����B
 *
 *  @param[in]     allocator               �A���P�[�^
 *  @param[in]     resource                ���\�[�X(NULL��n���ƑS��(�X�P���^���A�}�e���A���A�r�W�r���e�B)�̃A�j���[�V�������������܂�)
 *  @param[in]     animation_resource_type �A�j���[�V�������\�[�X�̃^�C�v(resource��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
 *  @param[in]     index_in_resource       ���\�[�X���ɂ�����C���f�b�N�X(resource��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
 *  @param[in]     change_frame            ���t���[���ŕύX���邩(���̊ԃu�����h�����)
 *  @param[in]     slot_index              �A�j���[�V���������蓖�Ă�X���b�g�̃C���f�b�N�X
 *  @param[in]     max_anim_members        �A�j���[�V�������郁���o�̍ő吔�B(resource��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
 *                                         slot_index�Ŏw�肵���X���b�g�ɑ΂��ď��߂Ă��̊֐����Ă񂾂Ƃ��́A�g�p�����B
 *                                         slot_index�Ŏw�肵���X���b�g�ɑ΂��ď��߂�change_frame>0�ł��̊֐����Ă񂾂Ƃ����A�g�p�����B
 *                                         Resource::Get???MemberAnimSetCount�œ����l��n���Ƃ悢�B
 *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST��n���ƁA
 *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X��GetMemberAnimSetCount���g�p���܂��B
 *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERS��n���ƁA
 *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X�̃A�j���[�V�����O���[�v�̃����o���ɁA
 *                                         �����A���̃A�j���[�V�����^�C�v�ɂ�����ő吔�ɂ��܂��B
 *  @param[in]     force                   �����I�ɃA�j���[�V�������֘A�t���܂��B(resource��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
 */
//-----------------------------------------------------------------------------
#if 0
/*  ���̂����������邩���B
   *  @param[in]     resource                ���\�[�X(NULL��n���ƃA�j���[�V�������������܂�)
   *  @param[in]     animation_resource_type �A�j���[�V�������\�[�X�̃^�C�v(resource��NULL�̂Ƃ��̂�ANIMATION_RESOURCE_TYPE_ALL��n���Ă悢�B
   *                                         ANIMATION_RESOURCE_TYPE_ALL�̂Ƃ��S��(�X�P���^���A�}�e���A���A�r�W�r���e�B)�̃A�j���[�V�������������܂�)*/
#endif
void Model::ChangeAnimation(
  gfl::heap::NwAllocator*          allocator,
  Resource*                        resource,
  Resource::AnimationResourceType  animation_resource_type,
  s32                              index_in_resource,
  u32                              change_frame,
  u32                              slot_index,
  s32                              max_anim_members,
  bool                             force
)
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "�X���b�g�̃C���f�b�N�X%d���s���ł��B\n", slot_index );
    return;
  }

  m_animation_slot_array[slot_index].ChangeAnimation(
      this,
      allocator,
      resource,
      animation_resource_type,
      index_in_resource,
      change_frame,
      max_anim_members,
      force
  );
#endif
  ResAnim* res_anim_ptr = NULL;
  ResAnim  res_anim;
  if( resource )
  {
    resource->GetResAnim(animation_resource_type, index_in_resource, &res_anim);
    res_anim_ptr = &res_anim;
  }
  ChangeAnimation(
      allocator,
      res_anim_ptr,
      change_frame,
      slot_index,
      max_anim_members,
      force
  );
}


//-----------------------------------------------------------------------------
/*
 *  @brief         slot_index��unit_index�Ɋ��蓖�Ă�A�j���[�V������ύX����
 *
 *  @param[in]     heap_allocator          �q�[�v�A���P�[�^
 *  @param[in]     device_allocator        �f�o�C�X�A���P�[�^
 *  @param[in]     slot_index              �X���b�g�̃C���f�b�N�X
 *  @param[in]     unit_index              �X���b�g���ɂ��������j�b�g�̃C���f�b�N�X
 *  @param[in]     res_anim                �A�j���[�V�������\�[�X(NULL��n���ƑS��(�X�P���^���A�}�e���A���A�r�W�r���e�B)�̃A�j���[�V�������������܂�)
 *  @param[in]     resource                ���\�[�X(NULL��n���ƑS��(�X�P���^���A�}�e���A���A�r�W�r���e�B)�̃A�j���[�V�������������܂�)
 *  @param[in]     animation_resource_type �A�j���[�V�������\�[�X�̃^�C�v(resource��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
 *  @param[in]     index_in_resource       ���\�[�X���ɂ�����C���f�b�N�X(resource��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
 *  @param[in]     change_frame            ���t���[���ŕύX���邩(���̊ԃu�����h�����)
 *  @param[in]     max_anim_members        �A�j���[�V�������郁���o�̍ő吔�B(res_anim��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
 *                                         Resource::Get???MemberAnimSetCount�œ����l��n���Ƃ悢�B
 *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST��n���ƁA
 *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X��GetMemberAnimSetCount���g�p���܂��B
 *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERS��n���ƁA
 *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X�̃A�j���[�V�����O���[�v�̃����o���ɁA
 *                                         �����A���̃A�j���[�V�����^�C�v�ɂ�����ő吔�ɂ��܂��B
 *  @param[in]     force                   �����I�ɃA�j���[�V�������֘A�t���܂��B(res_anim��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
 */
//-----------------------------------------------------------------------------
void Model::ChangeUnitAnimation(
  gfl::heap::NwAllocator* heap_allocator,
  gfl::heap::NwAllocator* device_allocator,
  u32                     slot_index,
  u32                     unit_index,
  ResAnim*                res_anim,
  u32                     change_frame,
  s32                     max_anim_members,
  b32                     force
)
{
  if(m_animation_slot_num == 0 )
  {
    GFL_ASSERT_MSG(0, "NOT Animatable Model\n");
    return;
  }

  if( !m_animation_manager )
  {
    if( heap_allocator )
    {
      m_animation_manager = GFL_NEW(heap_allocator->GetHeapBase()) AnimationManager;
      m_animation_manager->UseOwn_Create(
          heap_allocator,
          device_allocator,
          this,
          m_animation_slot_num,
          m_animation_blend_unit_num,
          m_animation_num_in_unit
      );
    }
    else
    {
      // �A�j���[�V���������蓖�ĂȂ̂Ƀ�������NULL�ł��Ares_anim��NULL�Ȃ猩�����B
      GFL_ASSERT_MSG(heap_allocator==NULL && res_anim==NULL, "�A�j���[�V���������蓖�Ăł����A��������NULL�̂��ߐ����ł��܂���B\n");  //@check
    }
  }
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_ChangeAnimation(
      heap_allocator,
      device_allocator,
      slot_index,
      unit_index,
      res_anim,
      change_frame,
      max_anim_members,
      force
    );
  }
}
void Model::ChangeUnitAnimation(
  gfl::heap::NwAllocator*          heap_allocator,
  gfl::heap::NwAllocator*          device_allocator,
  u32                              slot_index,
  u32                              unit_index,
  Resource*                        resource,
  Resource::AnimationResourceType  animation_resource_type,
  s32                              index_in_resource,
  u32                              change_frame,
  s32                              max_anim_members,
  b32                              force
)
{
  ResAnim* res_anim_ptr = NULL;
  ResAnim  res_anim;
  if( resource )
  {
    resource->GetResAnim(animation_resource_type, index_in_resource, &res_anim);
    res_anim_ptr = &res_anim;
  }
  ChangeUnitAnimation(
      heap_allocator,
      device_allocator,
      slot_index,
      unit_index,
      res_anim_ptr,
      change_frame,
      max_anim_members,
      force
  );
}


//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̃t���[����ݒ肷��
 *
 *  @param[in]     frame           �t���[��
 *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void Model::SetAnimationFrame(f32 frame, u32 slot_index)
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "�X���b�g�̃C���f�b�N�X%d���s���ł��B\n", slot_index );
    return;
  }

  m_animation_slot_array[slot_index].SetAnimationFrame(frame);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetFrame(slot_index, frame);
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̃t���[�����擾����
 *
 *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
 * 
 *  @retval        �t���[��
 */
//-----------------------------------------------------------------------------
f32 Model::GetAnimationFrame(u32 slot_index) const
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "�X���b�g�̃C���f�b�N�X%d���s���ł��B\n", slot_index );
    return 0.0f;
  }

  return m_animation_slot_array[slot_index].GetAnimationFrame();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->GetFrame(slot_index);
  }
  else
  {
    return 0.0f;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̍X�V�t���[����ݒ肷��
 *                 0.0f�Œ�~�A1.0f�œ��{�A2.0f��2�{���A-1.0f�ŋt���{�Đ�
 *
 *  @param[in]     step_frame      �X�V�t���[��
 *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void Model::SetAnimationStepFrame(f32 step_frame, u32 slot_index)
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "�X���b�g�̃C���f�b�N�X%d���s���ł��B\n", slot_index );
    return;
  }

  m_animation_slot_array[slot_index].SetAnimationStepFrame(step_frame);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetStepFrame(slot_index, step_frame);
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̍X�V�t���[�����擾����
 *
 *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
 * 
 *  @retval        �X�V�t���[��
 */
//-----------------------------------------------------------------------------
f32 Model::GetAnimationStepFrame(u32 slot_index) const
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "�X���b�g�̃C���f�b�N�X%d���s���ł��B\n", slot_index );
    return 0.0f;
  }

  return m_animation_slot_array[slot_index].GetAnimationStepFrame();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->GetStepFrame(slot_index);
  }
  else
  {
    return 0.0f;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̊J�n�t���[����ݒ肷��
 *
 *  @param[in]     frame           �t���[��
 *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void Model::SetAnimationStartFrame(f32 frame, u32 slot_index)
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "�X���b�g�̃C���f�b�N�X%d���s���ł��B\n", slot_index );
    return;
  }

  m_animation_slot_array[slot_index].SetAnimationStartFrame(frame);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetStartFrame(slot_index, frame);
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̊J�n�t���[�����擾����
 *
 *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
 *
 *  @retval        �t���[��
 */
//-----------------------------------------------------------------------------
f32 Model::GetAnimationStartFrame(u32 slot_index) const
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "�X���b�g�̃C���f�b�N�X%d���s���ł��B\n", slot_index );
    return 0.0f;
  }

  return m_animation_slot_array[slot_index].GetAnimationStartFrame();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->GetStartFrame(slot_index);
  }
  else
  {
    return 0.0f;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̏I���t���[����ݒ肷��
 *
 *  @param[in]     frame           �t���[��
 *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void Model::SetAnimationEndFrame(f32 frame, u32 slot_index)
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "�X���b�g�̃C���f�b�N�X%d���s���ł��B\n", slot_index );
    return;
  }

  m_animation_slot_array[slot_index].SetAnimationEndFrame(frame);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetEndFrame(slot_index, frame);
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̏I���t���[�����擾����
 *
 *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
 *
 *  @retval        �t���[��
 */
//-----------------------------------------------------------------------------
f32 Model::GetAnimationEndFrame(u32 slot_index) const
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "�X���b�g�̃C���f�b�N�X%d���s���ł��B\n", slot_index );
    return 0.0f;
  }

  return m_animation_slot_array[slot_index].GetAnimationEndFrame();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->GetEndFrame(slot_index);
  }
  else
  {
    return 0.0f;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̊J�n�t���[���ƏI���t���[����ݒ肷��
 *
 *  @param[in]     start_frame     �J�n�t���[��
 *  @param[in]     end_frame       �I���t���[��
 *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void Model::SetAnimationStartAndEndFrame(f32 start_frame, f32 end_frame, u32 slot_index )
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "�X���b�g�̃C���f�b�N�X%d���s���ł��B\n", slot_index );
    return;
  }

  m_animation_slot_array[slot_index].SetAnimationStartAndEndFrame(start_frame, end_frame);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetStartAndEndFrame(slot_index, start_frame, end_frame);
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̌��݂̃t���[�����J�n�t���[�����ۂ����肷��
 *
 *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
 *
 *  @retval        bool    ���݂̃t���[�����J�n�t���[���̏ꍇture��Ԃ�
 */
//-----------------------------------------------------------------------------
bool Model::IsAnimationFrameStartFrame(u32 slot_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->IsFrameStartFrame(slot_index);
  }
  else
  {
    return false;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̌��݂̃t���[�����I���t���[�����ۂ����肷��
 *
 *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
 *
 *  @retval        bool    ���݂̃t���[�����I���t���[���̏ꍇture��Ԃ�
 */
//-----------------------------------------------------------------------------
bool Model::IsAnimationFrameEndFrame(u32 slot_index) const
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "�X���b�g�̃C���f�b�N�X%d���s���ł��B\n", slot_index );
    return 0.0f;
  }

  return m_animation_slot_array[slot_index].IsAnimationFrameEndFrame();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->IsFrameEndFrame(slot_index);
  }
  else
  {
    return false;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̌��݂̃t���[����
 *                     step_frame>0�̂Ƃ��͏I���t���[���̂Ƃ�true��Ԃ�
 *                     step_frame<0�̂Ƃ��͊J�n�t���[���̂Ƃ�true��Ԃ�
 *                     step_frame==0�̂Ƃ��͊J�n�t���[�����I���t���[���̂Ƃ�true��Ԃ��A����ȊO�̂Ƃ�false��Ԃ�
 *
 *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
 *
 *  @retval        bool    ���݂̃t���[����step_frame���猩�Ē[�t���[���ƂȂ��Ă���ꍇture��Ԃ�
 */
//-----------------------------------------------------------------------------
bool Model::IsAnimationFrameStepTerminalFrame(u32 slot_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->IsFrameStepTerminalFrame(slot_index);
  }
  else
  {
    return false;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V���������[�v�Đ����ۂ��擾����
 *
 *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
 *
 *  @retval        bool            ���[�v�̂Ƃ�true�A���[�v�łȂ��Ƃ�false��Ԃ�
 */
//-----------------------------------------------------------------------------
bool Model::IsAnimationLoop(u32 slot_index) const
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "�X���b�g�̃C���f�b�N�X%d���s���ł��B\n", slot_index );
    return false;
  }

  return m_animation_slot_array[slot_index].IsAnimationLoop();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->IsLoop(slot_index);
  }
  else
  {
    return false;
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����Ƀ��[�v�Đ���ݒ肷��
 *
 *  @param[in]     loop            ���[�v�̂Ƃ�true�A���[�v�łȂ��Ƃ�false��ݒ肷��
 *  @param[in]     slot_index      �A�j���[�V���������蓖�Ă��X���b�g�̃C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void Model::SetAnimationLoop(bool loop, u32 slot_index)
{
#if 0
  if( slot_index >= m_animation_slot_num )
  {
    GFL_ASSERT_MSG( 0, "�X���b�g�̃C���f�b�N�X%d���s���ł��B\n", slot_index );
    return;
  }
  
  m_animation_slot_array[slot_index].SetAnimationLoop(loop);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetLoop(slot_index, loop);
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �t���[������
 */
//-----------------------------------------------------------------------------
void Model::SetUnitFrame(u32 slot_index, u32 unit_index, f32 frame)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitFrame(slot_index, unit_index, frame);
  }
}
f32  Model::GetUnitFrame(u32 slot_index, u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_GetUnitFrame(slot_index, unit_index);
  }
  return 0.0f;
}
void Model::SetUnitStepFrame(u32 slot_index, u32 unit_index, f32 step_frame)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitStepFrame(slot_index, unit_index, step_frame);
  }
}
f32  Model::GetUnitStepFrame(u32 slot_index, u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_GetUnitStepFrame(slot_index, unit_index);
  }
  return 0.0f;
}
void Model::SetUnitStartFrame(u32 slot_index, u32 unit_index, f32 frame)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitStartFrame(slot_index, unit_index, frame);
  }
}
f32  Model::GetUnitStartFrame(u32 slot_index, u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_GetUnitStartFrame(slot_index, unit_index);
  }
  return 0.0f;
}
void Model::SetUnitEndFrame(u32 slot_index, u32 unit_index, f32 frame)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitEndFrame(slot_index, unit_index, frame);
  }
}
f32  Model::GetUnitEndFrame(u32 slot_index, u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_GetUnitEndFrame(slot_index, unit_index);
  }
  return 0.0f;
}
void Model::SetUnitStartAndEndFrame(u32 slot_index, u32 unit_index, f32 start_frame, f32 end_frame)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitStartAndEndFrame(slot_index, unit_index, start_frame, end_frame);
  }
}
b32  Model::IsUnitFrameEndFrame(u32 slot_index, u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_IsUnitFrameEndFrame(slot_index, unit_index);
  }
  return false;
}
b32  Model::IsUnitLoop(u32 slot_index, u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_IsUnitLoop(slot_index, unit_index);
  }
  return false;
}
void Model::SetUnitLoop(u32 slot_index, u32 unit_index, b32 loop)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitLoop(slot_index, unit_index, loop);
  }
}


//-----------------------------------------------------------------------------
/*
 *  @brief         slot_index���ɂ��������j�b�g�̃A�j���[�V�������u�����h����
 *
 *  @param[in]     heap_allocator          �q�[�v�A���P�[�^
 *  @param[in]     device_allocator        �f�o�C�X�A���P�[�^
 *  @param[in]     slot_index              �X���b�g�̃C���f�b�N�X
 *  @param[in]     mode                    AnimationSlot::MODE_NONE                slot_index�̃A�j���[�V�������Ȃ��ɂ���
 *                                         AnimationSlot::MODE_ANIMATION           �u�����h����߂�animation_unit_index�Ŏw�肵���A�j���[�V���������蓖�Ă�
 *                                         AnimationSlot::MODE_BLEND_INTERPOLATOR  ��ԃu�����h
 *                                         AnimationSlot::MODE_BLEND_ADDER         ���Z�u�����h
 *  @param[in]     weight0                 AnimationSlot::MODE_BLEND_INTERPOLATOR��AnimationSlot::MODE_BLEND_ADDER�̂Ƃ��̂ݎg�p�B
 *                                         ���j�b�g0�̃u�����h�d��
 *  @param[in]     weight1                 AnimationSlot::MODE_BLEND_INTERPOLATOR��AnimationSlot::MODE_BLEND_ADDER�̂Ƃ��̂ݎg�p�B
 *                                         ���j�b�g1�̃u�����h�d��
 *  @param[in]     interpolator_ignore_no_anim_member   AnimationSlot::MODE_BLEND_INTERPOLATOR�̂Ƃ��̂ݎg�p�B
 *                                                      false�ɂ���ƁA�A�j���[�V���������݂��Ȃ������o�̓o�C���h���̒l���u�����h����܂��B
 *                                                      true�ɂ���ƁA�A�j���[�V���������݂��Ȃ������o�͏d�� 0 �Ƃ��ău�����h����܂��B
 *  @param[in]     interpolator_normalization_enable    AnimationSlot::MODE_BLEND_INTERPOLATOR�̂Ƃ��̂ݎg�p�B
 *                                                      true�ɂ���ƁA�u�����h�d�݂����v�� 1 �ɂȂ�悤���K�����Ă���u�����h���s���܂��B
 *                                                      false�ɂ���ƁA�ݒ肵���d�݂����̂܂܃u�����h�Ɏg�p�����B
 *  @param[in]     animation_unit_index                 AnimationSlot::MODE_ANIMATION�̂Ƃ��̂ݎg�p�B
 *                                                      �X���b�g���̂ǂ̃��j�b�g�ɂ���A�j���[�V���������蓖�Ă邩���w�肷��B
 */
//-----------------------------------------------------------------------------
void Model::BlendUnitAnimation(
    gfl::heap::NwAllocator*    heap_allocator,
    gfl::heap::NwAllocator*    device_allocator,
    u32                        slot_index,
    AnimationSlot::Mode        mode,
    b32                        interpolator_ignore_no_anim_member,
    b32                        interpolator_normalization_enable,
    u32                        animation_unit_index
)
{
  if(m_animation_slot_num == 0 )
  {
    GFL_ASSERT_MSG(0, "NOT Animatable Model\n");
    return;
  }

  if( !m_animation_manager )
  {
    m_animation_manager = GFL_NEW(heap_allocator->GetHeapBase()) AnimationManager;
    m_animation_manager->UseOwn_Create(
        heap_allocator,
        device_allocator,
        this,
        m_animation_slot_num,
        m_animation_blend_unit_num,
        m_animation_num_in_unit
    );
  }
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_BlendAnimation(
      heap_allocator,
      device_allocator,
      slot_index,
      mode,
      interpolator_ignore_no_anim_member,
      interpolator_normalization_enable,
      animation_unit_index
    );
  }
}
void Model::BlendUnit2Animation(
    gfl::heap::NwAllocator*    heap_allocator,
    gfl::heap::NwAllocator*    device_allocator,
    u32                        slot_index,
    AnimationSlot::Mode        mode,
    f32                        weight0,
    f32                        weight1,
    b32                        interpolator_ignore_no_anim_member,
    b32                        interpolator_normalization_enable,
    u32                        animation_unit_index
)
{
  BlendUnitAnimation(
    heap_allocator,
    device_allocator,
    slot_index,
    mode,
    interpolator_ignore_no_anim_member,
    interpolator_normalization_enable,
    animation_unit_index
  );
  {
    if(    mode == AnimationSlot::MODE_BLEND_INTERPOLATOR
        || mode == AnimationSlot::MODE_BLEND_ADDER )
    {
      SetUnitBlendWeight(slot_index, 0, weight0);
      SetUnitBlendWeight(slot_index, 1, weight1);
    }
  }
}


//-----------------------------------------------------------------------------
/*
 *  @brief         �u�����h�d��
 */
//-----------------------------------------------------------------------------
void Model::SetUnitBlendWeight(u32 slot_index, u32 unit_index, f32 weight)
{
  if( m_animation_manager )
  {
    m_animation_manager->SetBlendWeight(slot_index, unit_index, weight);
  }
}
f32  Model::GetUnitBlendWeight(u32 slot_index, u32 unit_index)
{
  if( m_animation_manager )
  {
    return m_animation_manager->GetBlendWeight(slot_index, unit_index);
  }
  return 0.0f;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ResAnim::Type�̃A�j���[�V�����ŕύX�����|�[�Y�����Z�b�g����
 *
 *                 slot_index�X���b�g�Ɋ��蓖�ĂĂ���A�j���[�V������ResAnim::Type�̃|�[�Y�����Z�b�g����
 *                 �������A���蓖�Ē��̃A�j���[�V�����������������o�̓��Z�b�g����Ȃ�
 *
 *  @param[in]     type          �A�j���[�V�����̃^�C�v
 *  @param[in]     slot_index    �X���b�g�̃C���f�b�N�X
 *
 *  �A�j���[�V���������蓖�Ă��Ă���Ƃ��́A���̃A�j���[�V�����������������o�̓��Z�b�g����Ȃ��B
 *  �A�j���[�V���������蓖�Ă��Ă��Ȃ��Ƃ��́A�S�Ẵ����o�����Z�b�g�����B
 *
 *  slot_index�X���b�g�ɃA�j���[�V���������蓖�Ă��Ă��Ȃ��Ƃ��́A���̃X���b�g�̃A�j���[�V������ResAnim::Type���Ȃ����߁A
 *  �������Z�b�g����Ȃ��̂Œ��ӂ��邱�ƁB
 *
 *  type��ResAnim::TYPE_MATERIAL�̏ꍇ�́A���̃��f���𐶐������Ƃ���
 *  Description�Ŏw�肵�Ă���MaterialBufferOption��
 *  MATERIAL_BUFFER_OPTION_ANIMATABLE_MATERIAL��
 *  MATERIAL_BUFFER_OPTION_ALL_MATERIAL�łȂ���΂Ȃ�Ȃ��B
 */
//-----------------------------------------------------------------------------
void Model::ResetAnimationPose(ResAnim::Type type)
{
  if( m_animation_manager )
  {
    m_animation_manager->Reset(type);
  }
}
void Model::ResetAnimationSlotPose(u32 slot_index)
{
  if( m_animation_manager )
  {
    m_animation_manager->ResetSlot(slot_index);
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ���[���h�}�g���N�X���擾����
 *
 *  @param[out]    matrix          �擾�����}�g���N�X���i�[���ĕԂ�
 *
 *  @retval        bool            ����Ɏ擾�ł�����true
 */
//-----------------------------------------------------------------------------
bool Model::GetWorldMatrix(gfl::math::MTX34* matrix) const
{
  nw::gfx::Model* nw_model = GetNwModel();
  if( nw_model )
  {
    const gfl::math::MTX34& mtx = nw_model->WorldMatrix();
    *matrix = mtx;
    return true;
  }
  return false;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         ���[���h�}�g���N�X��ݒ肷��
 *                 Scene��Calculate�̌�łȂ��Ɣ��f����܂���B
 *
 *  @param[in]    matrix           �ݒ肷��}�g���N�X
 *
 *  @retval        bool            ����ɐݒ�ł�����true
 */
//-----------------------------------------------------------------------------
bool Model::SetWorldMatrix(const gfl::math::MTX34& matrix)  // ���C�g��J�����A�{�[���̂Ȃ����f���Ȃ�A�����ݒ肷��Ό����ڂɒl�����f�����B
                                                            // �{�[���̂��郂�f�����Ƃ����ݒ肵�Ă������ڂɒl�����f����Ă���悤�ɂ͌����Ȃ��B
{
  nw::gfx::Model* nw_model = GetNwModel();
  if( nw_model )
  {
    gfl::math::MTX34& mtx = nw_model->WorldMatrix();
    mtx = matrix;
    
    // ���[���h�s����X�V�����̂ŁA�t�s��̃L���b�V���𖳌������܂��B
    nw_model->InvalidateInverseWorldMatrix();

    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ���[���h�}�g���N�X�̌v�Z�̗L���t���O
 *
 *  @param[in]     enable         �L���ɂ���Ƃ���true�A�����ɂ���Ƃ���false
 */
//-----------------------------------------------------------------------------
void Model::SetWorldMatrixCalculationEnable(bool enable)
{
  nw::gfx::Model* nw_model = GetNwModel();
  if( nw_model )
  {
    if( enable )
    {
      nw_model->Transform().EnableFlags( nw::gfx::CalculatedTransform::FLAG_IS_WORLDMATRIX_CALCULATION_ENABLED );
    }
    else
    {
      nw_model->Transform().DisableFlags( nw::gfx::CalculatedTransform::FLAG_IS_WORLDMATRIX_CALCULATION_ENABLED );
    }
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         ���[���h�}�g���N�X�̌v�Z�̗L���t���O
 *
 *  @retval        bool            �L���̂Ƃ���true�A�����̂Ƃ���false
 */
//-----------------------------------------------------------------------------
bool Model::IsWorldMatrixCalculationEnable(void) const
{
  nw::gfx::Model* nw_model = GetNwModel();
  if( nw_model )
  {
    bool enable = nw_model->Transform().IsEnabledFlags( nw::gfx::CalculatedTransform::FLAG_IS_WORLDMATRIX_CALCULATION_ENABLED );
    return enable; 
  }
  return false;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �{�[���̐����擾����
 *
 *  @retval        s32             �{�[���̐�
 */
//-----------------------------------------------------------------------------
s32  Model::GetBonesCount(void) const
{
  nw::gfx::SkeletalModel* nw_model = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(GetNwSceneNode());
  return nw_model->GetSkeleton()->GetResSkeleton().GetBonesCount();  // @note �s���̂Ƃ�0��Ԃ������B
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �{�[���̃C���f�b�N�X�ԍ����擾����
 *
 *  @param[in]     key             �{�[���̖��O
 *
 *  @retval        s32             �{�[���̃C���f�b�N�X�ԍ�        
 */
//-----------------------------------------------------------------------------
s32  Model::GetBonesIndex(const char* key) const
{
  nw::gfx::SkeletalModel* nw_model = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(GetNwSceneNode());
  return nw_model->GetSkeleton()->GetResSkeleton().GetBonesIndex(key);  // @note �s���̂Ƃ�BONE_INDEX_NOT_FOUND��Ԃ������B���s���̂Ƃ���-1�Ȃ̂�BONE_INDEX_NOT_FOUND�Ɠ����l�B
}
//-----------------------------------------------------------------------------
/*
 *  @brief         ���[���h�}�g���N�X�|�[�Y�̃{�[���̐����擾����
 *
 *  @retval        s32             �{�[���̐�       
 */
//-----------------------------------------------------------------------------
s32  Model::GetBonesCountOfWorldMatrixPose(void)
{
  nw::gfx::SkeletalModel* nw_model = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(GetNwSceneNode());
  nw::gfx::Skeleton::MatrixPose& pose = nw_model->GetSkeleton()->WorldMatrixPose();
  return pose.GetBonesCount();
}
//-----------------------------------------------------------------------------
/*
 *  @brief         ���[���h�}�g���N�X�|�[�Y�̃}�g���N�X���擾����
 *
 *  @param[in]     bone_index      �{�[���̃C���f�b�N�X
 *  @param[out]    matrix          �擾�����}�g���N�X���i�[���ĕԂ�
 *
 *  @retval        bool            ����Ɏ擾�ł�����true
 */
//-----------------------------------------------------------------------------
bool Model::GetBoneMatrixOfWorldMatrixPose(s32 bone_index, gfl::math::MTX34* matrix) const
{
  nw::gfx::SkeletalModel* nw_model = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(GetNwSceneNode());
  nw::gfx::Skeleton::MatrixPose& pose = nw_model->GetSkeleton()->WorldMatrixPose();
  const gfl::math::MTX34* mtx;
  bool ret = pose.GetMatrix( bone_index, &mtx );
  if( ret )
  {
    *matrix = *mtx;
  }
  return ret; 
}
//-----------------------------------------------------------------------------
/*
 *  @brief         ���[���h�}�g���N�X�|�[�Y�̃}�g���N�X��ݒ肷��
 *
 *  @param[in]     bone_index      �{�[���̃C���f�b�N�X
 *  @param[in]     matrix          �ݒ肷��}�g���N�X
 *
 *  @retval        bool            ����ɐݒ�ł�����true
 */
//-----------------------------------------------------------------------------
bool Model::SetBoneMatrixOfWorldMatrixPose(s32 bone_index, const gfl::math::MTX34& matrix)
{
  nw::gfx::SkeletalModel* nw_model = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(GetNwSceneNode());
  nw::gfx::Skeleton::MatrixPose& pose = nw_model->GetSkeleton()->WorldMatrixPose();
  gfl::math::MTX34* mtx = NULL;
  bool ret = pose.GetMatrix( bone_index, &mtx );
  if( ret )
  {
    *mtx = matrix;
  }
  return ret; 
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ���[���h�}�g���N�X�|�[�Y�̃}�g���N�X�̃|�C���^���擾����
 *
 *  @param[in]     bone_index         �{�[���̃C���f�b�N�X
 *
 *  @retval        gfl::math::MTX34*  �}�g���N�X�̃|�C���^��Ԃ��B
 *                                    NULL�̂Ƃ��Ȃ��B
 *                                    ����ɒl��������΃��[���h�}�g���N�X�|�[�Y�̃}�g���N�X�ɔ��f����܂��B
 */
//-----------------------------------------------------------------------------
gfl::math::MTX34* Model::GetBoneMatrixPointerOfWorldMatrixPose(s32 bone_index) const
{
  nw::gfx::SkeletalModel* nw_model = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(GetNwSceneNode());
  nw::gfx::Skeleton::MatrixPose& pose = nw_model->GetSkeleton()->WorldMatrixPose();
#if 0
  return pose.GetMatrix( bone_index );
  // �������bone_index���s�����ǂ����̃`�F�b�N���Ȃ��֐��Ȃ̂ŁA��������イASSERT�Ŏ~�܂�B
#else
  gfl::math::MTX34* mtx = NULL;
  bool ret = pose.GetMatrix( bone_index, &mtx );
  // ret==false�̂Ƃ���mtx�͕ύX����Ȃ��̂ł��̂܂ܕԂ��B 
  return mtx;
#endif
}


//-----------------------------------------------------------------------------
/*
 *  @brief         �����ւ��p�̃{�[���f�[�^���쐬����
 *
 *  @param[in]     heap_allocator  �q�[�v�������̃A���P�[�^
 *  @param[in]     base_model      ��{���f��
 */
//-----------------------------------------------------------------------------
void Model::CreateReplaceBoneData(gfl::heap::NwAllocator* heap_allocator, Model* base_model)
{
  s32 bones_count = this->GetBonesCount();

  m_replace_bone_data_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) ReplaceBoneData[bones_count];

  nw::gfx::Model*            nw_model        = this->GetNwModel();
  nw::gfx::SkeletalModel*    skeletal_model  = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(nw_model);
  nw::gfx::Skeleton*         skeleton        = skeletal_model->GetSkeleton();
  nw::gfx::res::ResSkeleton  res_skeleton    = skeleton->GetResSkeleton();
  
  for( s32 b=0; b<bones_count; ++b )
  {
    nw::gfx::res::ResBone res_bone = res_skeleton.GetBones(b);
    const char* bone_name = res_bone.GetName();

    s32 base_index = base_model->GetBonesIndex(bone_name);
    if( base_index >= 0 )
    {
      m_replace_bone_data_array[b].base_index = base_index;
    }
    else
    {
      m_replace_bone_data_array[b].base_index = gfl::grp::g3d::Model::BONE_INDEX_NONE;
      
      // ��c��T��
      m_replace_bone_data_array[b].base_ancestor_index = gfl::grp::g3d::Model::BONE_INDEX_NONE;  // �����l���
      s32 parent_index = res_bone.GetParentBoneIndex();  // �e
      while( parent_index >= 0 )
      {
        if( parent_index < b )
        {
          // ���ɐ�c���ݒ肳��Ă���{�[�����Q�Ƃ��Ă���
          if( m_replace_bone_data_array[parent_index].base_index >= 0 )
          {
            m_replace_bone_data_array[b].base_ancestor_index = m_replace_bone_data_array[parent_index].base_index;
          }
          else
          {
            m_replace_bone_data_array[b].base_ancestor_index = m_replace_bone_data_array[parent_index].base_ancestor_index;
          }
          break;
        }
        else
        {
          // �܂���c���ݒ肳��Ă��Ȃ��{�[�����Q�Ƃ��Ă���
          nw::gfx::res::ResBone res_bone_parent = res_skeleton.GetBones(parent_index);
          const char* bone_name_parent = res_bone_parent.GetName();
   
          s32 base_ancestor_index = base_model->GetBonesIndex(bone_name_parent);
          if( base_ancestor_index >= 0 )
          {
            m_replace_bone_data_array[b].base_ancestor_index = base_ancestor_index;
            break;
          }
          else
          {
            parent_index = res_bone_parent.GetParentBoneIndex();  // �X�ɐe�ɂ����̂ڂ�
          }
        }
      }
    }
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �����ւ��p�̃{�[���f�[�^��j������
 */
//-----------------------------------------------------------------------------
void Model::DestroyReplaceBoneData(void)
{
  if( m_replace_bone_data_array )
  {
    GFL_DELETE_ARRAY m_replace_bone_data_array;
    m_replace_bone_data_array = NULL;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �����ւ��p�̃{�[���̖����ݒ���s��(�e����)
 *
 *  @param[in]     name   name�̃{�[�����e�̃{�[���͑S�Ė����ݒ�ɂ���(name�̃{�[���͗L���ݒ�̂܂�)
 */
//-----------------------------------------------------------------------------
void Model::IgnoreParentReplaceBone(const char* name)
{
  if( m_replace_bone_data_array )
  {
    s32 bones_count = this->GetBonesCount();
 
    nw::gfx::Model*            nw_model        = this->GetNwModel();
    nw::gfx::SkeletalModel*    skeletal_model  = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(nw_model);
    nw::gfx::Skeleton*         skeleton        = skeletal_model->GetSkeleton();
    nw::gfx::res::ResSkeleton  res_skeleton    = skeleton->GetResSkeleton();
    
    for( s32 b=0; b<bones_count; ++b )
    {
      nw::gfx::res::ResBone res_bone = res_skeleton.GetBones(b);
      const char* bone_name = res_bone.GetName();

      if( strcmp( bone_name, name ) == 0 )
      {
        // �e��T��
        s32 parent_index = res_bone.GetParentBoneIndex();  // �e
        while( parent_index >= 0 )
        {
          // �e�̃{�[���𖳎��ݒ�ɂ���
          m_replace_bone_data_array[parent_index].base_index = gfl::grp::g3d::Model::BONE_INDEX_IGNORE;
          
          // �X�ɐe�ɂ����̂ڂ�
          nw::gfx::res::ResBone res_bone_parent = res_skeleton.GetBones(parent_index);
          parent_index = res_bone_parent.GetParentBoneIndex(); 
        }
        break;
      }
    }
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �����ւ��p�̃{�[���̖����ݒ���s��(�q����)
 *
 *  @param[in]     name   name�̃{�[�����q�̃{�[���͑S�Ė����ݒ�ɂ���(name�̃{�[���͗L���ݒ�̂܂�)
 */
//-----------------------------------------------------------------------------
void Model::IgnoreChildReplaceBone(const char* name)
{
  if( m_replace_bone_data_array )
  {
    s32 bones_count = this->GetBonesCount();
 
    nw::gfx::Model*            nw_model        = this->GetNwModel();
    nw::gfx::SkeletalModel*    skeletal_model  = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(nw_model);
    nw::gfx::Skeleton*         skeleton        = skeletal_model->GetSkeleton();
    nw::gfx::res::ResSkeleton  res_skeleton    = skeleton->GetResSkeleton();
    
    for( s32 b=0; b<bones_count; ++b )
    {
      nw::gfx::res::ResBone res_bone = res_skeleton.GetBones(b);
      const char* bone_name = res_bone.GetName();

      if( strcmp( bone_name, name ) == 0 )
      {
        // �q��T��
        nw::gfx::res::ResBone res_bone_child = res_bone.GetChildBone();  // �q
        while( res_bone_child.IsValid() )
        {
          ignoreSelfChildReplaceBoneRecursive(
            res_bone_child
#if GFL_DEBUG
            , 0
#endif        
          );  // �ċA�Ăяo��

          // �q�{�[���̌Z��{�[��
          res_bone_child = res_bone_child.GetNextSibling();
        }
        break;
      }
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �����ւ��p�̃{�[���̖����ݒ���s��(�q����)
 *
 *  @param[in]     name   name�̃{�[���Ƃ�����q�̃{�[���͑S�Ė����ݒ�ɂ���(name�̃{�[���������ݒ�ɂȂ�)
 */
//-----------------------------------------------------------------------------
void Model::IgnoreSelfChildReplaceBone(const char* name)
{
  if( m_replace_bone_data_array )
  {
    s32 bones_count = this->GetBonesCount();
 
    nw::gfx::Model*            nw_model        = this->GetNwModel();
    nw::gfx::SkeletalModel*    skeletal_model  = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(nw_model);
    nw::gfx::Skeleton*         skeleton        = skeletal_model->GetSkeleton();
    nw::gfx::res::ResSkeleton  res_skeleton    = skeleton->GetResSkeleton();
    
    for( s32 b=0; b<bones_count; ++b )
    {
      nw::gfx::res::ResBone res_bone = res_skeleton.GetBones(b);
      const char* bone_name = res_bone.GetName();

      if( strcmp( bone_name, name ) == 0 )
      {
        // name�̃{�[��
        while( res_bone.IsValid() )
        {
          ignoreSelfChildReplaceBoneRecursive(
            res_bone
#if GFL_DEBUG
            , 0
#endif        
          );  // �ċA�Ăяo��

          // name�̃{�[���̌Z��{�[��
          res_bone = res_bone.GetNextSibling();
        }
        break;
      }
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �ubase�̃{�[���v�̃��[���h�}�g���N�X���upart�̃{�[��(���̃��f���̃{�[��)�v�̃��[���h�}�g���N�X�ɐݒ肷��
 *                 �����Őݒ肵���l��Scene��Calculate���ĂԂƏ㏑������Ă��܂��̂ŁAScene��Calculate�̌�ŌĂԂ��ƁB
 *                 
 *                 // @note ���[���h�}�g���N�X�ɐݒ肷��֐��Ȃ̂ŁAhuman�g�b�v�ɂ��ݒ肵���ق���������������Ȃ����A
 *                          ���͂܂��ݒ肵�Ă��Ȃ��B
 *                 // @note �����������[���h�A���[�J���ƕ������ɁA�S���ɐݒ肷��̂������̂�������Ȃ��B
 *                 // @note ���̊֐��ɂāA�R�[���o�b�N�̂���human�g�b�v��hand�{�[���ɒl��ݒ肵����A
 *                          �R�[���o�b�N���ĂԂ悤�ɂ�����肾���A���͂܂��ĂׂĂ��Ȃ��B
 *
 *  @param[in]     base_model      ��{���f��
 */
//-----------------------------------------------------------------------------
void Model::SetWorldReplaceBone(Model* base_model)
{
  if( m_replace_bone_data_array )
  {
    // ��{���f���̃X�P���g���ŏ㏑������
    nw::gfx::Model*                   base_nw_model             = base_model->GetNwModel();
    nw::gfx::SkeletalModel*           base_skeletal_model       = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(base_nw_model);
    nw::gfx::Skeleton*                base_skeleton             = base_skeletal_model->GetSkeleton();
    nw::gfx::Skeleton::MatrixPose&    base_world_matrix_pose    = base_skeleton->WorldMatrixPose();
    nw::gfx::Skeleton::TransformPose& base_local_transform_pose = base_skeleton->LocalTransformPose();
    nw::gfx::Skeleton::TransformPose& base_world_transform_pose = base_skeleton->WorldTransformPose();

    {
      nw::gfx::Model*                   nw_model             = this->GetNwModel();
      nw::gfx::SkeletalModel*           skeletal_model       = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>(nw_model);
      nw::gfx::Skeleton*                skeleton             = skeletal_model->GetSkeleton();
      nw::gfx::Skeleton::MatrixPose&    world_matrix_pose    = skeleton->WorldMatrixPose();
      nw::gfx::Skeleton::TransformPose& local_transform_pose = skeleton->LocalTransformPose();
      nw::gfx::Skeleton::TransformPose& world_transform_pose = skeleton->WorldTransformPose();
      nw::gfx::res::ResSkeleton         res_skeleton         = skeleton->GetResSkeleton();
   
      s32 bones_count = this->GetBonesCount();
      for( s32 b=0; b<bones_count; ++b )
      {
        if( m_replace_bone_data_array[b].base_index != gfl::grp::g3d::Model::BONE_INDEX_IGNORE )
        {
          nw::gfx::res::ResBone  res_bone              = res_skeleton.GetBones(b);
          s32                    parent_index          = res_bone.GetParentBoneIndex();
 
          nw::math::MTX34*              world_matrix    = world_matrix_pose.GetMatrix(b);
          nw::gfx::CalculatedTransform* local_transform = local_transform_pose.GetTransform(b);
          nw::gfx::CalculatedTransform* world_transform = world_transform_pose.GetTransform(b);
 
          if( m_replace_bone_data_array[b].base_index >= 0 )
          {
            nw::math::MTX34*              base_world_matrix    = base_world_matrix_pose.GetMatrix(m_replace_bone_data_array[b].base_index);
            nw::gfx::CalculatedTransform* base_local_transform = base_local_transform_pose.GetTransform(m_replace_bone_data_array[b].base_index);
            nw::gfx::CalculatedTransform* base_world_transform = base_world_transform_pose.GetTransform(m_replace_bone_data_array[b].base_index);
          
            *world_matrix = *base_world_matrix;
          }
          else  // if( m_replace_bone_data_array[b].base_index == gfl::grp::g3d::Model::BONE_INDEX_NONE )
          {
            if( m_replace_bone_data_array[b].base_ancestor_index >= 0 )
            {
              if( m_replace_bone_data_array[parent_index].base_index >= 0 )
              {
                // ��Ԑe�Ȃ̂ŁA�ċA��������
                nw::math::MTX34* parent_world_matrix = base_world_matrix_pose.GetMatrix(m_replace_bone_data_array[parent_index].base_index);
                SetWorldReplaceBoneRecursive( skeleton, res_bone, parent_world_matrix
#if GFL_DEBUG
                    , 0
#endif 
                ); 
              }
            }
            else if( m_replace_bone_data_array[b].base_ancestor_index == gfl::grp::g3d::Model::BONE_INDEX_NONE )
            {
              // (base_index==gfl::grp::g3d::Model::BONE_INDEX_NONE&&base_ancestor_index==gfl::grp::g3d::Model::BONE_INDEX_NONE)�̂Ƃ��Abase��human�g�b�v�ɒǏ]����
              if( parent_index < 0 )
              {
                // ��Ԑe�Ȃ̂ŁA�ċA��������
                nw::math::MTX34 parent_world_matrix;
                base_model->GetWorldMatrix(&parent_world_matrix);
                SetWorldReplaceBoneRecursive( skeleton, res_bone, &parent_world_matrix
#if GFL_DEBUG
                    , 0
#endif 
                ); 
              }
            }
          }
        }
        // ����if�����^�̂Ƃ��́A
        // �������Ă����{�[��
        // �Ȃ̂ŁA�������Ȃ�
      
      }
    }
  }
}
//void Model::SetLocalReplaceBone(Model* base_model)  // @note ���[�J���}�g���N�X�ł����A�܂��ł��Ă��Ȃ�
                                                // NW4C�̃X�P���g�����L�Ɠ������ʂ𓾂����̊֐��B�X�P���g���A�j���͂��Ă��邪�ꏊ���Ⴄ�Ƃ������́B
//void Model::SetWorldLocalReplaceBone(Model* base_model)  // @note ���[���h�}�g���N�X�ɂ����[�J���}�g���N�X�ɒl��ݒ肷��ł����A�܂��ł��Ă��Ȃ�

//-----------------------------------------------------------------------------
/*
 *  @brief         res_bone�̃��[���h�}�g���N�X��parent_world_matrix�𔽉f�������̂�ݒ肷��B
 *                 ���̌�A�ċA�I�Ɏq�{�[���ɂ��̏������s���Ă����B
 *                 
 *  @param[in]     skeleton  part�̃X�P���g��
 *  @param[in]     res_bone  part�̃{�[��
 *  @param[in]     parent_world_matrix  res_bone�̐e�̃��[���h�}�g���N�X
 */
//-----------------------------------------------------------------------------
void Model::SetWorldReplaceBoneRecursive(
    nw::gfx::Skeleton*     skeleton,
    nw::gfx::res::ResBone  res_bone,
    const nw::math::MTX34* parent_world_matrix
#if GFL_DEBUG
    , const s32            recursive_count 
#endif
)
{
#if GFL_DEBUG
  if( recursive_count >= GFL_GRP_G3D_MODEL_RECURSIVE_COUNT_MAX )
  {
    GFL_PRINT("�x��: �ċA�Ăяo���̉񐔂�%d��ɂȂ�܂����B\n", recursive_count);
  }
  // @note NW4C�̃X�P���g���̃A�b�v�f�[�g������ǂ�ł݂�ƁA�ċA���g���Ă��Ȃ��悤�Ɍ�����B
  // �����炭�e�̍��̂ق����Ⴂ�ԍ��ɂȂ��Ă���K����Ɍv�Z�����悤�Ƀ��\�[�X�̒i�K�łȂ��Ă���̂��낤�B
  // �����M�p�ł���Ȃ�A�����̏������ċA�ɂ��Ȃ��čςނƎv����B
#endif 

  nw::gfx::Skeleton::MatrixPose&    world_matrix_pose    = skeleton->WorldMatrixPose();
  nw::gfx::Skeleton::TransformPose& local_transform_pose = skeleton->LocalTransformPose();
  nw::gfx::Skeleton::TransformPose& world_transform_pose = skeleton->WorldTransformPose();
  nw::gfx::res::ResSkeleton         res_skeleton         = skeleton->GetResSkeleton();

  s32 b = res_bone.GetIndex();
  nw::math::MTX34*              world_matrix    = world_matrix_pose.GetMatrix(b);
  nw::gfx::CalculatedTransform* local_transform = local_transform_pose.GetTransform(b);
  nw::gfx::CalculatedTransform* world_transform = world_transform_pose.GetTransform(b);

  nw::math::MTX34  local_transform_matrix;
  {
    const nw::math::MTX34& local_t_transform_matrix = local_transform->TransformMatrix();
    const nw::math::VEC3&  local_t_scale            = local_transform->Scale();

    internal::CopyTranslate(&local_transform_matrix, local_t_transform_matrix);
    internal::MultScale(&local_transform_matrix, &local_t_transform_matrix, local_t_scale);
  }

  nw::math::MTX34Mult( world_matrix, parent_world_matrix, &local_transform_matrix );

  // �q�{�[��
  nw::gfx::res::ResBone res_bone_child = res_bone.GetChildBone();
  while( res_bone_child.IsValid() )
  {
    if( m_replace_bone_data_array[res_bone_child.GetIndex()].base_index != gfl::grp::g3d::Model::BONE_INDEX_IGNORE )
    {
      SetWorldReplaceBoneRecursive( skeleton, res_bone_child, world_matrix
#if GFL_DEBUG
          , recursive_count +1
#endif        
      );  // �ċA�Ăяo��
    }
    // ����if�����^�̂Ƃ��́A
    // �������Ă����{�[��
    // �Ȃ̂ŁA������q�ɑ΂��Ă͉������Ȃ�

    // �q�{�[���̌Z��{�[��
    res_bone_child = res_bone_child.GetNextSibling();
  }       
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �����ւ��p�̃{�[���̖����ݒ���ċA�I�ɍs��(�q����)
 *
 *  @param[in]     res_bone   res_bone�̃{�[���𖳎��ݒ�ɂ��Ares_bone���q�̃{�[�����S�Ė����ݒ�ɂ���(res_bone�̃{�[���������ݒ�ɂȂ�)
 */
//-----------------------------------------------------------------------------
void Model::ignoreSelfChildReplaceBoneRecursive(
    nw::gfx::res::ResBone  res_bone
#if GFL_DEBUG
    , const s32            recursive_count 
#endif
) 
{
#if GFL_DEBUG
  if( recursive_count >= GFL_GRP_G3D_MODEL_RECURSIVE_COUNT_MAX )
  {
    GFL_PRINT("�x��: �ċA�Ăяo���̉񐔂�%d��ɂȂ�܂����B\n", recursive_count);
  }
  // @note NW4C�̃X�P���g���̃A�b�v�f�[�g������ǂ�ł݂�ƁA�ċA���g���Ă��Ȃ��悤�Ɍ�����B
  // �����炭�e�̍��̂ق����Ⴂ�ԍ��ɂȂ��Ă���K����Ɍv�Z�����悤�Ƀ��\�[�X�̒i�K�łȂ��Ă���̂��낤�B
  // �����M�p�ł���Ȃ�A�����̏������ċA�ɂ��Ȃ��čςނƎv����B
#endif 

  s32 b = res_bone.GetIndex();

  // �{�[���𖳎��ݒ�ɂ���
  m_replace_bone_data_array[b].base_index = gfl::grp::g3d::Model::BONE_INDEX_IGNORE;

  // �q��T��
  nw::gfx::res::ResBone res_bone_child = res_bone.GetChildBone();  // �q
  while( res_bone_child.IsValid() )
  {
    ignoreSelfChildReplaceBoneRecursive(
      res_bone_child
#if GFL_DEBUG
      , recursive_count +1
#endif        
    );  // �ċA�Ăяo��

    // �q�{�[���̌Z��{�[��
    res_bone_child = res_bone_child.GetNextSibling();
  }       
}


//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[���m�[�h������ɍS������
 *                 SceneNode::SetupConstraint����Ă΂��
 *
 *  @param[in]     baton_scene_node    �S�������baton�V�[���m�[�h
 *  @param[in]     hand_scene_index    hand�{�[���̃C���f�b�N�X
 *  @param[in]     inherit_scale       true�̂Ƃ��X�P�[���̒Ǐ]����
 *  @param[in]     inherit_rotate      true�̂Ƃ���]�̒Ǐ]����
 *  @param[in]     inherit_translate   true�̕��s�ړ��̒Ǐ]����
 *
 *  @retval        �ǉ��ł�����true��Ԃ�
 *                 ���X�ǉ�����Ă���V���ɒǉ��ł��Ȃ��Ƃ���GFL_ASSERT_MSG������false��Ԃ�  //@check �����ɂЂ�������Ȃ��悤�Ɋm�F�ς̈������
 */
//-----------------------------------------------------------------------------
b32 Model::AddConstraintBatonSceneNode(
  SceneNode* baton_scene_node,
  const s32  hand_bone_index,
  const b32  inherit_scale,
  const b32  inherit_rotate,
  const b32  inherit_translate
)
{
  if( hand_bone_index >= 0 )
  {
    if( m_callback_constraint_of_skeleton )
    {
      return m_callback_constraint_of_skeleton->AddBatonSceneNode(
          baton_scene_node,
          hand_bone_index,
          inherit_scale,
          inherit_rotate,
          inherit_translate
      );
    }
  }
  else
  {
    if( m_callback_constraint_of_transform_node )
    {
      return m_callback_constraint_of_transform_node->AddBatonSceneNode(
          baton_scene_node,
          inherit_scale,
          inherit_rotate,
          inherit_translate
      );
    }
  }
  return false;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[���m�[�h�̍S���̒Ǐ]��ύX����
 *                 baton_scene_node��hand_bone_index�͕ύX���Ȃ��ŁAinherit�����ύX����
 *                 SceneNode::SetupConstraint����Ă΂��
 *
 *  @param[in]     baton_scene_node    �S�����Ă���baton�V�[���m�[�h
 *  @param[in]     hand_scene_index    baton���������Ă���hand�{�[���̃C���f�b�N�X
 *  @param[in]     inherit_scale       true�̂Ƃ��X�P�[���̒Ǐ]����
 *  @param[in]     inherit_rotate      true�̂Ƃ���]�̒Ǐ]����
 *  @param[in]     inherit_translate   true�̕��s�ړ��̒Ǐ]����
 *
 *  @retval        �ύX�ł�����true��Ԃ�
 *                 ���X�ǉ�����Ă��Ȃ��Ƃ���GFL_ASSERT_MSG������false��Ԃ�  //@check �����ɂЂ�������Ȃ��悤�Ɋm�F�ς̈������
 */
//-----------------------------------------------------------------------------
b32 Model::ChangeConstraintBatonSceneNodeInherit(
  SceneNode* baton_scene_node,
  const s32  hand_bone_index,
  const b32  inherit_scale,
  const b32  inherit_rotate,
  const b32  inherit_translate
)
{
  if( hand_bone_index >= 0 )
  {
    if( m_callback_constraint_of_skeleton )
    {
      return m_callback_constraint_of_skeleton->ChangeBatonSceneNodeInherit(
          baton_scene_node,
          hand_bone_index,
          inherit_scale,
          inherit_rotate,
          inherit_translate
      );
    }
  }
  else
  {
    if( m_callback_constraint_of_transform_node )
    {
      return m_callback_constraint_of_transform_node->ChangeBatonSceneNodeInherit(
          baton_scene_node,
          inherit_scale,
          inherit_rotate,
          inherit_translate
      );
    }
  }
  return false;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[���m�[�h�̍S������������
 *                 SceneNode::SetupConstraint����Ă΂��
 *                 SceneNode::CleanupConstraint����Ă΂��
 *
 *  @param[in]     baton_scene_node    �S������Ă���baton�V�[���m�[�h
 *  @param[in]     hand_scene_index    baton���������Ă���hand�{�[���̃C���f�b�N�X
 *
 *  @retval        �폜�ł��Ȃ��Ƃ���false��Ԃ�
 *                 ���X�ǉ�����Ă��Ȃ��Ƃ���GFL_ASSERT_MSG������false��Ԃ�  //@check �����ɂЂ�������Ȃ��悤�Ɋm�F�ς̈������
 */
//-----------------------------------------------------------------------------
b32 Model::RemoveConstraintBatonSceneNode(
  SceneNode* baton_scene_node,
  const s32  hand_bone_index
)
{
  if( hand_bone_index >= 0 )
  {
    if( m_callback_constraint_of_skeleton )
    {
      return m_callback_constraint_of_skeleton->RemoveBatonSceneNode(
          baton_scene_node,
          hand_bone_index
      );
    }
  }
  else
  {
    if( m_callback_constraint_of_transform_node )
    {
      return m_callback_constraint_of_transform_node->RemoveBatonSceneNode(
          baton_scene_node
      );
    }
  }
  return false;
}


//-----------------------------------------------------------------------------
/*
 *  @brief         �����_�[�L���[�֒ǉ��A�����_�[�L���[����폜
 */
//-----------------------------------------------------------------------------
// UserParameter��RenderQueue�ɂ͎g���Ă��Ȃ����Ƃ����������̂ŁA
// SafeDeleteCheck�p�̎g�킹�Ă��炤�B
void Model::AddAllRenderQueue(void)  // SafeDeleteCheck�p�Ɏg���Ă��܂��B
{
  nw::gfx::Model* nw_model = GetNwModel();
  u32 user_parameter = nw_model->GetUserParameter<u32>();
  u32 flag = USER_PARAMETER_RENDER_QUEUE_BIT_MASK;
  user_parameter |= flag;
  nw_model->SetUserParameter<u32>(user_parameter);
}
void Model::RemoveAllRenderQueue(void)  // SafeDeleteCheck�p�Ɏg���Ă��܂��B
{
  nw::gfx::Model* nw_model = GetNwModel();
  u32 user_parameter = nw_model->GetUserParameter<u32>();
  u32 flag = USER_PARAMETER_RENDER_QUEUE_BIT_MASK;
  user_parameter &= ~flag;
  nw_model->SetUserParameter<u32>(user_parameter);
}
void Model::AddRenderQueue(s32 render_queue_index)  // SafeDeleteCheck�p�Ɏg���Ă��܂��B
{
  nw::gfx::Model* nw_model = GetNwModel();
  u32 user_parameter = nw_model->GetUserParameter<u32>();
  u32 flag = 0x1 << (USER_PARAMETER_RENDER_QUEUE_BIT_SHIFT+render_queue_index);
  user_parameter |= flag;
  nw_model->SetUserParameter<u32>(user_parameter);
}
void Model::RemoveRenderQueue(s32 render_queue_index)  // SafeDeleteCheck�p�Ɏg���Ă��܂��B
{
  nw::gfx::Model* nw_model = GetNwModel();
  u32 user_parameter = nw_model->GetUserParameter<u32>();
  u32 flag = 0x1 << (USER_PARAMETER_RENDER_QUEUE_BIT_SHIFT+render_queue_index);
  user_parameter &= ~flag;
  nw_model->SetUserParameter<u32>(user_parameter);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ���[�U�[�p�����[�^�̃`�F�b�N
 */
//-----------------------------------------------------------------------------
// ���̃��f�������̃����_�[�L���[�ŕ`�悷�邩
// UserParameter��RenderQueue�ɂ͎g���Ă��Ȃ����Ƃ����������̂ŁA
// SafeDeleteCheck�p�̎g�킹�Ă��炤�B
bool Model::IsUserParameterRenderQueue( const nw::gfx::Model* nw_model, s32 /*render_queue_index*/ )  // SafeDeleteCheck�p�Ɏg���Ă��܂��B
{
  if( !( nw_model->IsVisible() && nw_model->IsEnabledResults(nw::gfx::SceneNode::FLAG_IS_VISIBLE) ) ) return false;
    // SetVisible�ł͕\��OFF�ɂł��Ȃ������̂ŁAgfx_Model.h��IsVisibleModelDefaultFunctor���Q�l�ɂ��Ĕ����ǉ������B

#if 0
  u32 user_parameter = nw_model->GetUserParameter<u32>();
  u32 flag = 0x1 << (USER_PARAMETER_RENDER_QUEUE_BIT_SHIFT+render_queue_index);
  return ( (user_parameter & flag) != 0 );
#else
  return true;
#endif
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �{�N���X�̃}�e���A���̃o�b�t�@�̐����I�v�V������NintendoWare�̃o�b�t�@�̐����I�v�V�����ɕύX����
 */
//-----------------------------------------------------------------------------
bit32 Model::ConvertMaterialBufferOptionToNwBufferOption(
    const MaterialBufferOption  material_buffer_option
)
{
  bit32 nw_buffer_option = nw::gfx::Model::FLAG_BUFFER_NOT_USE;
  static const bit32 material_buffer_option_to_nw_buffer_option[MATERIAL_BUFFER_OPTION_MAX] =
  {
    nw::gfx::Model::FLAG_BUFFER_NOT_USE,
    nw::gfx::Model::FLAG_BUFFER_MATERIAL_COLOR,
    nw::gfx::Model::FLAG_BUFFER_MATERIAL_COLOR | nw::gfx::Model::FLAG_BUFFER_SHADING_PARAMETER,
    nw::gfx::Model::MULTI_FLAG_ANIMATABLE_MATERIAL,
    nw::gfx::Model::MULTI_FLAG_BUFFER_MATERIAL,
  };
  if(    0 <= material_buffer_option
      && material_buffer_option < MATERIAL_BUFFER_OPTION_MAX )
  {
    nw_buffer_option = material_buffer_option_to_nw_buffer_option[material_buffer_option];
  }
  return nw_buffer_option;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �������ɃX�P���g�����L���`�F�b�N���A�K�؂ɏ�������
 */
//-----------------------------------------------------------------------------
Model::ShareType Model::CheckSkeletonShareSourceModel(
    Model*               skeleton_share_source_model,
    nw::gfx::Skeleton**  nw_skeleton 
)
{
  // �`�F�b�N
  if( !skeleton_share_source_model )
  {
    *nw_skeleton = NULL;
    return SHARE_TYPE_RESOURCE;
  }

  if(    ( skeleton_share_source_model->m_skeleton_share_type != SHARE_TYPE_RESOURCE     )
      && ( skeleton_share_source_model->m_skeleton_share_type != SHARE_TYPE_SHARE_SOURCE ) )
  {
    GFL_ASSERT_MSG( 0, "�X�P���g���̋��L���̃��f�����A�X�P���g���̋��L���ɂȂ邱�Ƃ��ł��܂���B\n" );
    *nw_skeleton = NULL;
    return SHARE_TYPE_RESOURCE;
  }

  // ���f
  nw::gfx::SkeletalModel* skeleton_share_source_skeletal_model = nw::ut::DynamicCast<nw::gfx::SkeletalModel*> \
                                                                 ( skeleton_share_source_model->GetNwSceneNode() );
  nw::gfx::Skeleton*      skeleton_share_source_skeleton       = skeleton_share_source_skeletal_model->GetSkeleton();

  nw::gfx::res::ResSkeleton skeleton_share_source_res_skeleton = skeleton_share_source_skeleton->GetResSkeleton();
  if( skeleton_share_source_model->m_skeleton_share_type == SHARE_TYPE_RESOURCE )
  {
    skeleton_share_source_res_skeleton.EnableFlags(nw::gfx::ResSkeletonData::FLAG_MODEL_COORDINATE);
    skeleton_share_source_model->m_skeleton_share_type = SHARE_TYPE_SHARE_SOURCE;
  }

  *nw_skeleton = skeleton_share_source_skeleton;
  return SHARE_TYPE_SHARE_DESTINATION;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �������Ƀ}�e���A�����L���`�F�b�N���A�K�؂ɏ�������
 */
//-----------------------------------------------------------------------------
Model::ShareType Model::CheckMaterialShareSourceModel(
    Model*                material_share_source_model,
    MaterialBufferOption  material_buffer_option,
    nw::gfx::Model**      nw_model
)
{
  // �`�F�b�N
  if( !material_share_source_model )
  {
    *nw_model = NULL;
    return SHARE_TYPE_RESOURCE;
  }

  if( material_buffer_option != MATERIAL_BUFFER_OPTION_NOT_USE )
  {
    GFL_ASSERT_MSG( 0, "material_share_source_model��NULL�łȂ��Ƃ��Amaterial_buffer_option��MATERIAL_BUFFER_OPTION_NOT_USE�łȂ���΂Ȃ�Ȃ��B\n" );
    *nw_model = NULL;
    return SHARE_TYPE_RESOURCE;
  }
  if(    ( material_share_source_model->m_material_share_type != SHARE_TYPE_RESOURCE     )
      && ( material_share_source_model->m_material_share_type != SHARE_TYPE_SHARE_SOURCE ) )
  {
    GFL_ASSERT_MSG( 0, "material_share_source_model���}�e���A���̋��L�����Q�Ƃ��Ă��܂��B\n" );
    *nw_model = NULL;
    return SHARE_TYPE_RESOURCE;
  }

  // ���f
  nw::gfx::Model* material_share_source_nw_model = material_share_source_model->GetNwModel();

  if( material_share_source_model->m_material_share_type != SHARE_TYPE_RESOURCE )
  {
    material_share_source_model->m_material_share_type = SHARE_TYPE_SHARE_SOURCE;
  }

  *nw_model = material_share_source_nw_model;
  return SHARE_TYPE_SHARE_DESTINATION;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �������Ƀ��b�V���m�[�h�r�W�r���e�B���L���`�F�b�N���A�K�؂ɏ�������
 */
//-----------------------------------------------------------------------------
Model::ShareType Model::CheckMeshNodeVisibilityShareSourceModel(
    Model*                mesh_node_visibility_share_source_model,
    nw::gfx::Model**      nw_model
)
{
  // �`�F�b�N
  if( !mesh_node_visibility_share_source_model )
  {
    *nw_model = NULL;
    return SHARE_TYPE_RESOURCE;
  }

  if(    ( mesh_node_visibility_share_source_model->m_mesh_node_visibility_share_type != SHARE_TYPE_RESOURCE     )
      && ( mesh_node_visibility_share_source_model->m_mesh_node_visibility_share_type != SHARE_TYPE_SHARE_SOURCE ) )
  {
    GFL_ASSERT_MSG( 0, "mesh_node_visibility_share_source_model�����b�V���m�[�h�r�W�r���e�B�̋��L�����Q�Ƃ��Ă��܂��B\n" );
    *nw_model = NULL;
    return SHARE_TYPE_RESOURCE;
  }

  // ���f
  nw::gfx::Model* mesh_node_visibility_share_source_nw_model = mesh_node_visibility_share_source_model->GetNwModel();

  if( mesh_node_visibility_share_source_model->m_mesh_node_visibility_share_type != SHARE_TYPE_RESOURCE )
  {
    mesh_node_visibility_share_source_model->m_mesh_node_visibility_share_type = SHARE_TYPE_SHARE_SOURCE;
  }
  
  *nw_model = mesh_node_visibility_share_source_nw_model;
  return SHARE_TYPE_SHARE_DESTINATION;
}


#if GFL_DEBUG
void Model::SetSafeDeleteCheckInfo(const char* pName)
{
  // UserParameter��RenderQueue�ɂ͎g���Ă��Ȃ����Ƃ����������̂ŁA
  // SafeDeleteCheck�p�̎g�킹�Ă��炤�B
  nw::gfx::Model* nw_model = GetNwModel();
  nw_model->SetUserParameter<u32>(reinterpret_cast<u32>(this));

  m_DrawFrameCount = 0;
  SetModelName(pName);
}
void Model::SetModelName(const char* pName)
{
  memset( m_ModelName, 0, sizeof(m_ModelName) );
  if(pName)
  {
    s32   len = strlen( pName );
    
    if ( len >= sizeof(m_ModelName) )
      len = sizeof(m_ModelName) - 1;//���[null�����L�[�v
    
    memcpy( m_ModelName, pName, len );
  }
}
#endif


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

