#ifndef __GFL_G3DSCENENODE_H__
#define __GFL_G3DSCENENODE_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dSceneNode.h
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
#include <gfl_Macros.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dObject.h>
#include <grp/g3d/gfl_G3dSceneCalculateGroup.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�N���X�̑O���錾
//=====================================
class Model;

namespace internal {

class CallbackConstraintOfSkeleton;       // ���ꂪ�Ȃ��ƁASceneNode�N���X����friend�N���X�Ƃ��ď������Ƃ��ł��Ȃ�
class CallbackConstraintOfTransformNode;

}  // namespace internal


//-------------------------------------
///	3D�V�[���m�[�h�N���X
//=====================================
class SceneNode : public Object
{
  GFL_FORBID_COPY_AND_ASSIGN(SceneNode);


  friend class Scene;
  friend class SceneSystem;
  //friend class Model::AnimationSlot;  // ���������\�L�ł�omake���ʂ�Ȃ�
  friend class AnimationSlot;
  friend class internal::CallbackConstraintOfSkeleton;       // friend class CallbackConstraintOfSkeleton;�ł�omake���ʂ�Ȃ�
  friend class internal::CallbackConstraintOfTransformNode;


public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   */
  //-----------------------------------------------------------------------------
  virtual void Create(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �j��
   *                 ����Ă�ł����v�B�f�X�g���N�^�͌Ă΂�܂���B 
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���c���[�t���O
   *                 true�̂Ƃ�Scene��Add����Ă�����V�[���c���[�ɓ������Afalse�̂Ƃ�Scene��Add����Ă��Ă��V�[���c���[����O�����
   *                 ���ꂪtrue�ł��邱�ƂƁA���[�U��Scene��Add�������Ƃ́A�����ł͂���܂���B
   */
  //-----------------------------------------------------------------------------
  virtual void SetTreeFlag(b32 flag);
  virtual b32  GetTreeFlag(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �`��t���O
   *                 true�̂Ƃ��`�悳���
   *                 ���������ꂪtrue�̂Ƃ��ł��AGetTreeFlag��false�̂Ƃ��͕`�悳��Ȃ�
   */
  //-----------------------------------------------------------------------------
  virtual void SetVisible(b32 visible);
  virtual b32  IsVisible(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[����Add����Ă�����true��Ԃ�(�V�[���c���[�t���O���ǂ��Ȃ��Ă��邩�͒��ׂĂ��Ȃ��̂ŁAAdd����Ă��Ă��V�[���c���[�ɂ͓����Ă��Ȃ���������Ȃ�)
   *                 ���̃V�[���m�[�h�̐e��SceneCalculateGroup�łȂ��Ă��A�e�̐e�́E�E�E�e�̐e��SceneCalculateGroup��������AAdd����Ă���Ɣ��肷��
   */
  //-----------------------------------------------------------------------------
  b32 IsAddedToScene(void) const { return (m_parent_borrow != NULL); }


public:
  //-----------------------------------------------------------------------------
  /**
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
  b32 SetupConstraint(
      gfl::grp::g3d::Model* human_scene_node,
      const s32             hand_bone_index,
      const b32             inherit_scale     = true,
      const b32             inherit_rotate    = true,
      const b32             inherit_translate = true
  );
  b32 SetupConstraint(
      gfl::grp::g3d::Model* human_scene_node,
      const char*           hand_bone_key,
      const b32             inherit_scale     = true,
      const b32             inherit_rotate    = true,
      const b32             inherit_translate = true
  );

  //-----------------------------------------------------------------------------
  /**
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
  b32 CleanupConstraint(void);


protected:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���̃N���X���ێ����Ă���nw::gfx::SceneNode*���擾
   *
   *  @retval        nw::gfx::SceneNode*    ���̃N���X���ێ����Ă���nw::gfx::SceneNode
   */
  //-----------------------------------------------------------------------------
  virtual nw::gfx::SceneNode* GetNwSceneNode(void) const  {  return m_nw_scene_node;  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���̃N���X���ێ����Ă���nw::gfx::SceneNode*��nw::gfx::TransformNode*�ɃL���X�g���Ď擾
   *
   *  @retval        nw::gfx::TransformNode*    ���̃N���X���ێ����Ă���nw::gfx::SceneNode
   */
  //-----------------------------------------------------------------------------
  virtual nw::gfx::TransformNode* GetNwTransformNode(void) const
  {
    return nw::ut::DynamicCast<nw::gfx::TransformNode*>( m_nw_scene_node );
  }


protected:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   *                 Create�͌Ă΂�܂���B
   */
  //-----------------------------------------------------------------------------
  SceneNode(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   *                 Destroy���Ăт܂��B
   */
  //-----------------------------------------------------------------------------
  virtual ~SceneNode();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���̃N���X�ŕێ�����悤nw::gfx::SceneNode*��ݒ�
   *
   *  @param[in]     nw_scene_node    �V�[���m�[�h
   */
  //-----------------------------------------------------------------------------
  void SetNwSceneNode(nw::gfx::SceneNode* nw_scene_node)  {  m_nw_scene_node = nw_scene_node;  }


protected:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���C���ŏ㏑�������O�̒l
   *                 �h���N���X��m_constraint_calculated_transform�����������邽�߂̃A�N�Z�X�֐�
   *
   *                 @note �h���N���X�ŃA�N�Z�X���āA�X�̔h���N���X�ŏ�������̂ł͂Ȃ��ASceneNode�ŏ����������B
   */
  //-----------------------------------------------------------------------------
  nw::gfx::CalculatedTransform& ConstraintNwCalculatedTransform(void)
  {
    return m_constraint_calculated_transform;
  }
  const nw::gfx::CalculatedTransform& ConstraintNwCalculatedTransform(void) const  // @note �h���N���X��const�֐�����ĂԂ��߂̊֐������A
                                                                                   // �X�̔h���N���X�ŏ�������̂ł͂Ȃ��ASceneNode�ŏ�������悤��
                                                                                   // �Ȃ�����s�v�ɂȂ�̂ŁA�폜����\��̊֐��B
  {
    return m_constraint_calculated_transform;
  }
private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���C���ŏ㏑�������O�̒l
   *                 friend�N���X��m_constraint_calculated_transform�����邽�߂̃A�N�Z�X�֐�
   */
  //-----------------------------------------------------------------------------
  const nw::gfx::CalculatedTransform* GetConstraintNwCalculatedTransform(void) const
  {
    return &m_constraint_calculated_transform;
  }


private:
  // �e��ݒ肷��
  void SetParent(SceneCalculateGroup* parent)
  {
    // NULL��ݒ肷��Ƃ��́AUnsetParent���ĂԂ��ƁB
    GFL_ASSERT_MSG(parent, "parent=NULL\n");  //@check
    
    if( !m_parent_borrow )
    {
      m_parent_borrow = parent;
    }
    else
    {
      GFL_ASSERT_MSG(0, "���ɐe0x%p�����܂��B\n", m_parent_borrow);
    }
  }
  void UnsetParent(void)
  {
    // NULL��ݒ肷��
    GFL_ASSERT_MSG(m_parent_borrow, "m_parent_borrow=NULL\n");  //@check

    m_parent_borrow = NULL;
  }
  // GetParent�͂Ȃ�(�Ȃ��̗��R�͓��ɂȂ����A���ՂɎg���ė~�����Ȃ��̂�)
  

public:
  //�e�X�g�p�ɋً}�p�ӂ�������b32 IsParentExist(void) const { return (m_parent_borrow != NULL); }


private:
  // �V�[���m�[�h(�Ƃ��̔h���N���X)�𐧌䂷��r�b�g�t���O
  enum
  {
    // �Ȃ�
    BIT_FLAG_NONE         = 0,  // �������Ȃ�

    // ��
    BIT_FLAG_TREE_FLAG    = 0x1 <<  0,  // �V�[���c���[�t���O
                                        // 1�̂Ƃ�Scene��Add����Ă�����V�[���c���[�ɓ������A
                                        // 0�̂Ƃ�Scene��Add����Ă��Ă��V�[���c���[����O�����B

    // ����
    BIT_FLAG_DEFAULT      = BIT_FLAG_TREE_FLAG   // �f�t�H���g
  };

private:
  nw::gfx::SceneNode* m_nw_scene_node;

  // �e
  SceneCalculateGroup*          m_parent_borrow;  // ���̂Ƃ���Ő����������̂��؂�Ċo���Ă���  // @note Model��Light���e�ɂȂ邱�Ƃ�����̂ŁA�e�Ɛ��蓾����̂̌^�����肽���B  // @note ������ς�����IsAddedToScene���ύX���邱��

  // �r�b�g�t���O
  bit32                         m_bit_flag;

  // �R���X�g���C��
  gfl::grp::g3d::Model*         m_constraint_human_scene_node;      // NULL�łȂ��Ƃ��A���̃V�[���m�[�h�͍S������baton�ɂȂ��Ă���B
  s32                           m_constraint_hand_bone_index;
  nw::gfx::CalculatedTransform  m_constraint_calculated_transform;  // �R���X�g���C���Ń��[�U���ݒ肵���l���㏑������Ă��܂��̂ŁA�㏑�������O�̒l���Ƃ��Ă����B

protected:
  gfl::math::VEC3  m_rotate;  // XYZ�̏��Ԃɉ�]����B�P�ʂ̓��W�A���B
                              // �y���Ӂz��p��SetRotateXYZ�֐��ȊO�ŉ�]���s���Ă���ꍇ�́A�����ڂƈقȂ�l�������Ă���B
                              // @note SceneNode�ł͂Ȃ�TransNode�Ƃ��������Ă�����Ɏ����������B
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DSCENENODE_H__
