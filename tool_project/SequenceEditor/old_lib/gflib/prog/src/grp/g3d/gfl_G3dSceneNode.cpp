//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dSceneNode.cpp
 *	@brief  3D�V�[���m�[�h
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dSceneNode.h>
#include <grp/g3d/gfl_G3dModel.h>
#include <grp/g3d/gfl_G3dSceneCalculateGroup.h>

namespace gfl {
namespace grp {
namespace g3d {


namespace internal {

static const nw::math::Transform3 SCENE_NODE_CONSTRAINT_CALCULATED_TRANSFORM_DEFAULT(
    nw::math::VEC3( 1.0f, 1.0f, 1.0f ),
    nw::math::VEC3( 0.0f, 0.0f, 0.0f ),
    nw::math::VEC3( 0.0f, 0.0f, 0.0f )
);

}  // namespace internal


//-----------------------------------------------------------------------------
/*
 *  @brief         �R���X�g���N�^
 *                 Create�͌Ă΂�܂���B
 */
//-----------------------------------------------------------------------------
SceneNode::SceneNode(void)
  : m_nw_scene_node(NULL),
    m_parent_borrow(NULL),
    m_bit_flag(BIT_FLAG_DEFAULT),
    m_constraint_human_scene_node(NULL),
    m_constraint_hand_bone_index(gfl::grp::g3d::Model::BONE_INDEX_NONE),
    m_rotate(0.0f, 0.0f, 0.0f)
{
  m_constraint_calculated_transform.SetTransform( internal::SCENE_NODE_CONSTRAINT_CALCULATED_TRANSFORM_DEFAULT );
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �f�X�g���N�^
 *                 Destroy���Ăт܂��B
 */
//-----------------------------------------------------------------------------
SceneNode::~SceneNode()
{
  Destroy();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ����
 */
//-----------------------------------------------------------------------------
void SceneNode::Create(void)
{
  m_bit_flag = BIT_FLAG_DEFAULT;

  m_rotate = gfl::math::VEC3(0.0f, 0.0f, 0.0f);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �j��
 *                 ����Ă�ł����v�B�f�X�g���N�^�͌Ă΂�܂���B 
 */
//-----------------------------------------------------------------------------
void SceneNode::Destroy(void)
{
  //GFL_ASSERT_MSG(m_parent_borrow==NULL, "�e����O���O�ɔj�����Ă΂�܂����B\n");  // �e����O��Ă��Ȃ��Ƃ��͌x�����o�����A�������Ȃ��B  //@check
                                                                                    // ���܂�ɂ��̌x���Ɉ����|����̂ŁA�O���Ă����邱�Ƃɂ���B
                                                                                    // Scene��Destroy�ł��O���Y����O���Ă���̂ŁA
                                                                                    // �e��NULL�ɂȂ��Ă���Ƃ������Ƃ͂Ȃ��B
  if( m_parent_borrow )
  {
    m_parent_borrow->RemoveSceneNode(this);
  }


  nw::gfx::SafeDestroy(m_nw_scene_node);  // SafeDestroy��NULL�������Ă����

  m_parent_borrow = NULL;  // �e����O��Ă��Ȃ��Ă��������Ȃ��B

  m_bit_flag = BIT_FLAG_DEFAULT;

  if( m_constraint_human_scene_node )
  {
    CleanupConstraint();
  }

  m_constraint_human_scene_node     = NULL;
  m_constraint_hand_bone_index      = gfl::grp::g3d::Model::BONE_INDEX_NONE;
  m_constraint_calculated_transform.SetTransform( internal::SCENE_NODE_CONSTRAINT_CALCULATED_TRANSFORM_DEFAULT );
  
  m_rotate = gfl::math::VEC3(0.0f, 0.0f, 0.0f);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[���c���[�t���O
 *                 true�̂Ƃ�Scene��Add����Ă�����V�[���c���[�ɓ������Afalse�̂Ƃ�Scene��Add����Ă��Ă��V�[���c���[����O�����
 *                 ���ꂪtrue�ł��邱�ƂƁA���[�U��Scene��Add�������Ƃ́A�����ł͂���܂���B
 */
//-----------------------------------------------------------------------------
void SceneNode::SetTreeFlag(b32 flag)
{
  if( GetTreeFlag() != flag )
  {
    SetBitFlag(&m_bit_flag, BIT_FLAG_TREE_FLAG, flag);
    if( m_parent_borrow )
    {
      m_parent_borrow->RefreshSceneNodeTreeFlag(this);
    }
  }
}
b32  SceneNode::GetTreeFlag(void) const
{
  return ( IsBitFlagOnOne(m_bit_flag, BIT_FLAG_TREE_FLAG) != 0 );  // @note ������y�߂�l���Ӂz�������Ă����Ȃ�
                                                                   // u32�^�Ły0�ȊO(1�Ƃ͌���Ȃ��������Ⴄ�l�ɂȂ�)�z���y0�z��Ԃ��悤�ɂ��Ă����������B
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �`��t���O
 *                 true�̂Ƃ��`�悳���
 *                 ���������ꂪtrue�̂Ƃ��ł��AGetTreeFlag��false�̂Ƃ��͕`�悳��Ȃ�
 */
//-----------------------------------------------------------------------------
void SceneNode::SetVisible(b32 /*visible*/)
{
  // �h���N���X�Ŏ������ĉ������B
  // @note ModelNode�N���X�������Ă�����Ɉڍs������
}
b32  SceneNode::IsVisible(void) const
{
  // �h���N���X�Ŏ������ĉ������B
  // @note ModelNode�N���X�������Ă�����Ɉڍs������
  return false;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �R���X�g���C���̃Z�b�g�A�b�v������
 *                 ���̃V�[���m�[�h�͍S������baton�ɂȂ�
 *
 *                 ���ɂ��̃V�[���m�[�h��baton�Ƃ��ăZ�b�g�A�b�v����Ă���Ƃ��ɂ��̊֐����ĂԂƁA�V�����Z�b�g�A�b�v�ŏ㏑�������B
 *                 ������A���߂ẴZ�b�g�A�b�v�������łȂ��Ainherit��ύX�������Ƃ���hand�{�[����ύX�������Ƃ��ɂ����̊֐����Ă�ł悢�B
 *
 *  @param[in]     human_scene_node    human�V�[���m�[�h  // @note ����Model�Ɍ��肵�Ă��܂����ALight��Camera�ɂ��S���ł���悤�ɂ��邩���B
 *  @param[in]     hand_bone_index     hand�{�[���̃C���f�b�N�X
 *                                     gfl::grp::g3d::Model::BONE_INDEX_NONE�̂Ƃ��Ahand�{�[���ł͂Ȃ�human�g�b�v�ɂ���
 *  @param[in]     hand_bone_key       hand�{�[���̖��O
 *                                     NULL�̂Ƃ��Ahand�{�[���ł͂Ȃ�human�g�b�v�ɂ���
 *  @param[in]     inherit_scale       true�̂Ƃ��X�P�[���̒Ǐ]����
 *  @param[in]     inherit_rotate      true�̂Ƃ���]�̒Ǐ]����
 *  @param[in]     inherit_translate   true�̕��s�ړ��̒Ǐ]����
 *
 *  @retval        b32                 ����ɃZ�b�g�A�b�v�ł�����true
 *
 *  @note �S�������Ƃ��ɁA�S������O�̃g�����X�t�H�[����ύX���Ȃ����ύX���邩��I�ׂ�悤�ɂ������B
 *  �I�ׂȂ��Ȃ�A�ǂ��炩�ɓ��ꂵ�Ă������ƁI
 */
//-----------------------------------------------------------------------------
b32 SceneNode::SetupConstraint(
    gfl::grp::g3d::Model* human_scene_node,
    const s32             hand_bone_index,
    const b32             inherit_scale,
    const b32             inherit_rotate,
    const b32             inherit_translate
)
{
  if( m_constraint_human_scene_node )
  {
    // ���ɁA���̃V�[���m�[�h�͍S������Ă���baton�ɂȂ��Ă���
    if(    m_constraint_human_scene_node != human_scene_node
        || m_constraint_hand_bone_index  != hand_bone_index )
    {
      // �Ⴄ�Ƃ���ɂ�������
      b32 ret_rem = m_constraint_human_scene_node->RemoveConstraintBatonSceneNode(
          this,
          m_constraint_hand_bone_index
      );
      if( ret_rem )
      {
        m_constraint_human_scene_node = NULL;
        m_constraint_hand_bone_index  = gfl::grp::g3d::Model::BONE_INDEX_NONE;

        b32 ret_add = human_scene_node->AddConstraintBatonSceneNode(
            this,
            hand_bone_index,
            inherit_scale,
            inherit_rotate,
            inherit_translate
        );
        if( ret_add )
        {
          m_constraint_human_scene_node = human_scene_node;
          m_constraint_hand_bone_index  = hand_bone_index;
        }
        else
        {
          GFL_ASSERT_MSG( 0, "�ǉ��ł��܂���ł����B\n" );
        }
        return ret_add;
      }
      else
      {
        GFL_ASSERT_MSG( 0, "�t���������߂Ɉ�U�폜���悤�Ƃ��܂������A�폜�ł��܂���ł����B\n" );
        return ret_rem;
      }
    }
    else
    {
      // �����Ƃ���Ȃ̂�inherit�����ύX����
      b32 ret_cha = human_scene_node->ChangeConstraintBatonSceneNodeInherit(
          this,
          hand_bone_index,
          inherit_scale,
          inherit_rotate,
          inherit_translate
      );
      GFL_ASSERT_MSG( ret_cha, "inherit��ύX���悤�Ƃ��܂������A�ύX�ł��܂���ł����B\n" );  //@check
      return ret_cha;
    }
  }
  else
  {
    // ���߂Ă��̃V�[���m�[�h���S����baton�ɂ���
    b32 ret_add = human_scene_node->AddConstraintBatonSceneNode(
        this,
        hand_bone_index,
        inherit_scale,
        inherit_rotate,
        inherit_translate
    );
    if( ret_add )
    {
      m_constraint_human_scene_node = human_scene_node;
      m_constraint_hand_bone_index  = hand_bone_index;
    }
    else
    {
      GFL_ASSERT_MSG( 0, "�ǉ��ł��܂���ł����B\n" );  // @note ���X�ǉ�����Ă���Ƃ� or �v�f�����z���Ēǉ��ł��Ȃ��Ƃ� ��2�ʂ��false�����邽�߁A
                                                        //       ���͑O�̂������Y��Ă��đO�҂�false�ɂȂ��Ă���Ƃ������Ƃ����蓾��B�������ʂ������B
    }
    return ret_add;
  }
}
b32 SceneNode::SetupConstraint(
    gfl::grp::g3d::Model* human_scene_node,
    const char*           hand_bone_key,
    const b32             inherit_scale,
    const b32             inherit_rotate,
    const b32             inherit_translate
)
{
  return SceneNode::SetupConstraint(
    human_scene_node,
    human_scene_node->GetBonesIndex(hand_bone_key),
    inherit_scale,
    inherit_rotate,
    inherit_translate
  );
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �R���X�g���C���̃N���[���A�b�v������
 *                 ���̃V�[���m�[�h�̍S��������baton�łȂ�����
 *
 *  @retval        b32                 ����ɃN���[���A�b�v�ł�����true
 *                                     ���X�R���X�g���C������Ă��Ȃ��Ƃ���false��Ԃ��B
 *
 *  @note �S���������Ƃ��ɁA�S�����鎞�̃g�����X�t�H�[����ύX���Ȃ����ύX���邩��I�ׂ�悤�ɂ������B
 *  �I�ׂȂ��Ȃ�A�ǂ��炩�ɓ��ꂵ�Ă������ƁI
 */
//-----------------------------------------------------------------------------
b32 SceneNode::CleanupConstraint(void)
{
  if( m_constraint_human_scene_node )
  {
    // ���̃V�[���m�[�h�͍S������Ă���baton�ɂȂ��Ă���
    b32 ret_rem = m_constraint_human_scene_node->RemoveConstraintBatonSceneNode(
        this,
        m_constraint_hand_bone_index
    );
    if( ret_rem )
    {
      m_constraint_human_scene_node = NULL;
      m_constraint_hand_bone_index  = gfl::grp::g3d::Model::BONE_INDEX_NONE;
    }
    else
    {
      GFL_ASSERT_MSG( 0, "�폜�ł��܂���ł����B\n" );
    }
    return ret_rem;
  }
  else
  {
    // ���̃V�[���m�[�h�͍S������Ă��Ȃ��̂�baton�ɂȂ��Ă��Ȃ�
    return false;
  }
}

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

