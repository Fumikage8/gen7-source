#ifndef __GFL_G3DUNITANIMATION_H__
#define __GFL_G3DUNITANIMATION_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dUnitAnimation.h
 *	@brief  3D�A�j���[�V�����̒P�ʂƂȂ����
 *	@author	Koji Kawada
 *	@date		2011.10.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#if 0
/*

�p����


����
  �uAnimation�v�ƂȂ��Ă���Ƃ��́uAnimation�N���X�v�̂��Ƃ��w���A
  �u�A�j���[�V�����v�ƂȂ��Ă���Ƃ��́u��ʓI�ȓ���⃂�[�V�����v�̂��Ƃ��w���Ă���B
  ����������邽�߂ɁuAnimation�v�́uAnimation�N���X�v�Ɩ��L���邱�Ƃɂ��邪�A
  �uAnimation�N���X�v�Ə����āuAnimation�N���X�̃C���X�^���X�v�̂��Ƃ�\���Ă��邱�Ƃ�����B


IAnimation�N���X
  Animaton�N���X��UnitAnimation�N���X�̊��N���X
  ���ۃN���X�A�C���X�^���X�N���X


Animation�N���X
  �ȉ��̂ǂꂩ
  �E���\�[�X���̂܂܂̃A�j���[�V����
  �E������Animation�N���X���u�����h�����A�j���[�V����
  �E������Animation�N���X���u�����h�����A�j���[�V�������u�����h�����A�j���[�V����


UnitAnimation�N���X
  Animation�N���X��1��������
  �����Ă���Animation�N���X��ύX����Ƃ��́A���t���[���������đO��Animation�N���X�Əd�ݍ��v1�ƂȂ��ԃu�����h���s���Ȃ���ύX����
  (�u�����h���s���̂ŁA�O��Animation�N���X�A�O�̑O��Animation�N���X�A�O�̑O�̑O��Animation�N���X�Ƃ����ӂ���
   ����������Animation�N���X�������Ă���̂����A�O���猩����1��Animation�N���X�ɂ��������Ȃ��̂ŒP��(Unit)�ƌĂ�ł���)
  ��ԃu�����h�ł̕ύX���I����Ă���Ȃ�Animation�N���X���̂��̂ɂȂ��Ă���
  ��ԃu�����h�ŕύX���Ȃ��Ă������ꍇ�͏��Animation�N���X���̂��̂ɂȂ�


AnimationSlot�N���X
  ������UnitAnimation�N���X��C�ӂ̕����Ńu�����h�����A�j���[�V������1��������
  �u�����h���Ă��Ȃ��Ȃ�UnitAnimation�N���X���̂��̂ɂȂ��Ă���

  Animation�N���X�ƂقƂ�Ǔ����Ȃ̂ŁA
  �uAnimation�N���X��IAnimation�N���X�Ńu�����h�ł���悤�ɑ���UnitAnimation�N���X�̃u�����h���ł���悤�Ɋg���v���āA
  ���́u�g������Animation�N���X�v��AnimationSlot�N���X�̒��g�Ƃ��Ď��悤�ɂ�����Animation�N���X�Ǝ��ʂ����L�q���Ȃ��Ȃ�̂����A
  �������Ȃ������B�Ȃ��Ȃ炻�����Ă��܂��ƁA
  �uUnitAnimation�N���X���u�g������Animation�N���X�v�𕡐������Ƃ��ł��鑦��UnitAnimation�N���X�̕��i��UnitAnimation�N���X�����邱�Ƃ��ł��Ă��܂��v
  ���߁A�l�������G�ɂȂ��Ă��܂�����B�ǂ����G�ɂȂ邩���������ƁA
  ���w�u��������p���`�ɕύX����UnitAnimation�N���X�v���u��������L�b�N�ɕύX����UnitAnimation�N���X�v�ɕύX����UnitAnimation�N���X�x��
  �w�������Ă���Animation�N���X�x�ɕύX����UnitAnimation�N���X���Ƃ������Ƃ������ł��Ă��܂��B
  ���ꂭ�炢���G�ł����������Ƃ����C�ɂȂ�����A
  �uAnimation�N���X��AnimationPack�̃����o��Animation�N���X����IAnimation�N���X�ɕύX���AAnimation�N���X��Calculate���������A
  �Ăяo�����͖��t���[��GetNwAnimObject�̌��ʂŃ|�C���^��ύX����v�Ƃ������Ƃ����Ȃ���΂Ȃ�Ȃ��B

  AnimationSlot�N���X�́Anw::gfx::Model�̃����o�֐��ł���
  SetSkeletalAnimObject, SetMaterialAnimObject, SetVisibilityAnimObject
  �ɓn��objectIndex���A�Œ�l��1���蓖�Ă��Ă���B
  objectIndex�͑SAnimationSlot�ł̒ʂ��ԍ��ɂ��Ă���B
  �����A�j���[�V�����O���[�v(��FSkeletal��Skeletal)�ŕ����A�j���[�V����(PartialAnimation)���s���ۂɂ�
  objectIndex��ʂ̒l�ɂ���K�v�����邪�A
  �قȂ�A�j���[�V�����O���[�v(��FSkeletal��Material)�̏ꍇ�͓���objectIndex���g���Ă����Ȃ��B
  �������ASkeletal�̉��ԖځAMaterial�̉��ԖڂƂ��̂����������̂ŁA
  Skeletal��Material��Visibility���܂߂Ēʂ��ԍ��ɂ��邱�Ƃɂ����B


AnimationManager�N���X
  AnimationSlot�𕡐�����
  AnimationSlot�N���X�͂��݂��Ɋ����Ȃ��̂ŁA�����A�j���[�V����(PartialAnimation)���ł���
  AnimationSlot�N���X�̓Ɨ�������������ۂ��߂ɂ��A���������𑀍삷��悤�ȃA�j���[�V�����𕡐���AnimationSlot�N���X�Ɋ��蓖�ĂȂ��悤��
  ���[�U�͒��ӂ��ĉ�����




�ݒ���

  nw::gfx::AnimInterpolator::Builder()
  nw::gfx::TransformAnimInterpolator::Builder()
      .IgnoreNoAnimMember(false)  // false��
      IgnoreNoAnimMember��true�ɂ���ƁA�m�[�hA�̓����̂���A�j��b�ƃm�[�hA�̓����̂Ȃ��A�j��c���u�����h�����
      �m�[�hA�ɂ̓A�j��b�̓����݂̂��t���B�����A�����̂Ȃ��A�j��c�Ƃ̓u�����h����Ȃ��B
      IgnoreNoAnimMember��false�ɂ���ƁA�m�[�hA�̓����̂���A�j��b�ƃm�[�hA�̓����̂Ȃ��A�j��c���u�����h�����
      �m�[�hA�ɂ̓A�j��b�̓�����OriginalValue�i���f���̃��[�h���̏�ԁj�Ńu�����h�����������t���B

  nw::gfx::AnimAdder
  nw::gfx::TransformAnimAdder
      IgnoreNoAnimMember�̐ݒ�͂Ȃ����Atrue���Ǝv���Ă���΂悢�B

  nw::gfx::AnimInterpolator::
  nw::gfx::TransformAnimInterpolator::
      SetNormalizationEnabled(true);  // true��
      �u�����h�d�݂̍��v���K��1�ɂȂ�悤�ɏd�݂̒l�����[�U���n���Ă����Ȃ�  
      false�ł����̂����A�����Ƃ͌���Ȃ����낤����true�ɂ��Ă����B
      �u�����h�d�݂̍��v��1�ȏ��1�����ɂ����u�����h���Ӑ}�I�ɍs�������A�Ƃ����v�]�����邩������Ȃ����A
      ���ʂ𗝉����ɂ���(IgnoreNoAnimMember�Ƒg�ݍ��킹��Ɠ��ɗ������ɂ���)�����Ȃ̂ł�߂Ă����B
      Builder::IsOldMethod �� true ���w�肵�Ă���ƁA���̐ݒ�͖�������܂��B
      �f�t�H���g�l�� true �ł��B
*/
#endif


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dIAnimation.h>
#include <grp/g3d/gfl_G3dAnimation.h>
#include <grp/g3d/gfl_G3dResAnim.h>
#include <grp/g3d/gfl_G3dResource.h>


namespace gfl {
namespace grp {
namespace g3d {


//-------------------------------------
///	�A�j���[�V�����̒P�ʂƂȂ����
//=====================================
class UnitAnimation : public IAnimation
{
  GFL_FORBID_COPY_AND_ASSIGN(UnitAnimation);


  friend class AnimationSlot;
  friend class AnimationManager;


  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
  //
  // friend�N���X����̂݃A�N�Z�X���Ă����������o�֐�
  //
  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
private:
  // �R���X�g���N�^
  UnitAnimation(void);
  // �f�X�g���N�^
  virtual ~UnitAnimation();

  // ����
  virtual void Create(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    ResAnim::Type           type,
    s32                     num_max
  );
  void ChangeAnimation(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    s32                     num_max,
    Animation*              animation,
    u32                     change_frame
  );
  void ChangeAnimation(
    gfl::heap::NwAllocator* device_allocator,
    Animation*              animation,
    u32                     change_frame
  );

  // �j��
  virtual void Destroy(void);

  // �v�Z����
  // �K�����t���[���Ă�ŉ������B
  virtual void Calculate(void);

  // ���݂̃A�j���[�V�����𓾂�
  // �K�����t���[��Calculate�̌�ŌĂсA�Ăяo�����Ŏg�p���Ă���A�j���[�V�����̃|�C���^��ύX���ĉ������B
  // Calculate�ł��̃A�j���[�V�����̃|�C���^��������Ă��܂���������Ȃ��̂ŁACalculate�̑O�ɂ͌Ă΂Ȃ����ƁB
  virtual nw::gfx::AnimObject* GetNwAnimObject(void) const;

  b32 BindBlend(nw::gfx::AnimGroup* nw_anim_group);

  // �A�j���[�V������ύX����
  void ChangeAnimation(
      Animation*  animation,
      u32         change_frame
  );

  // �A�j���[�V�����̃^�C�v�𓾂�
  virtual ResAnim::Type GetType(void) const;

  // ���݂������Ă���A�j���[�V�����̐�(�d��0�ł��������Ă���ΐ��ɓ����)
  virtual s32 GetAnimationNum(void) const;

  // �t���[������
  virtual void SetFrame(const f32 frame);
  virtual f32  GetFrame(void) const;
  
  virtual void SetStepFrame(const f32 step_frame);
  virtual f32  GetStepFrame(void) const;
  
  virtual void SetStartFrame(const f32 frame);
  virtual f32  GetStartFrame(void) const;
  virtual void SetEndFrame(const f32 frame);
  virtual f32  GetEndFrame(void) const;
  virtual void SetStartAndEndFrame(const f32 start_frame, const f32 end_frame);
  virtual b32  IsFrameStartFrame(void) const;
  virtual b32  IsFrameEndFrame(void) const;
  virtual b32  IsFrameStepTerminalFrame(void) const;

  virtual void SetLoop(b32 loop);
  virtual b32  IsLoop(void) const;


  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
  //
  // friend�N���X������A�N�Z�X���Ă͂Ȃ�Ȃ��萔�A�^
  //
  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
private:
  // m_type, m_num_max, m_blend��ݒ�A��������
  b32 CreateBlend( 
    gfl::heap::NwAllocator* device_allocator,
    ResAnim::Type           type,
    s32                     num_max
  );

  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
  //
  // friend�N���X������A�N�Z�X���Ă͂Ȃ�Ȃ��萔�A�^
  //
  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
private:
  // ���
  enum State
  {
    STATE_NONE,           // �A�j���[�V�������Ȃ�
    STATE_ONE,            // m_pack_array[0]�̃A�j���[�V�����ɂȂ��Ă���
    STATE_OTHERS_BLEND,   // nw::gfx::AnimInterpolator�ɂȂ��Ă���
    STATE_SKELETAL_BLEND  // nw::gfx::TransformAnimInterpolator�ɂȂ��Ă���
  };

  // �A�j���[�V�����̏����܂Ƃ߂��\����
  struct AnimationPack
  {
    u32               frame_count;  // �u�����h�t���[���̃J�E���g
    u32               frame_max;    // ���t���[���u�����h���邩
          // (frame_count>=frame_max)���^�ɂȂ����炱�̃A�j���[�V�����ɂȂ�
          // frame_max=0�̂Ƃ��́A�ύX�����t���[���ł��̃A�j���[�V�����ɂȂ�(0�t���[����Ɋ��S�ɂ���ɂȂ�)
          // frame_max=1�̂Ƃ��́A�ύX�����t���[���ł͉����ς�炸�A���̃t���[���ł��̃A�j���[�V�����ɂȂ�(1�t���[����Ɋ��S�ɂ���ɂȂ�)
          // frame_max=2�̂Ƃ��́A�ύX�����t���[���ł͉����ς�炸�A���̃t���[���Œ��Ԃ̃A�j���[�V�����ɂȂ�A����Ɏ��̃t���[�����̃A�j���[�V�����ɂȂ�(2�t���[����Ɋ��S�ɂ���ɂȂ�)
    Animation*        animation;    // ���̂Ƃ���Ő��������A�j���[�V�������o���Ă��邾���̃|�C���^
    AnimationPack(void)
      : frame_count(0),
        frame_max(0),
        animation(NULL)
    {}
  };


  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
  //
  // friend�N���X������A�N�Z�X���Ă͂Ȃ�Ȃ������o�ϐ�
  //
  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
private:
  State          m_state;
  ResAnim::Type  m_type;

  s32             m_num_max;  // �ő吔
  s32             m_num;      // ���݂̌�
  AnimationPack*  m_pack_array;  // �ŐV[0][1]...[m_num_max-2][m_num_max-1]�Ō�

  // �u�����h����m_num��n�Ƃ����
  // 
  // n=2�̂Ƃ�
  // [0].weight=([0].frame_count)/([0].frame_max);
  // [1].weight=1-[0].weight;
  //
  // n=3�̂Ƃ�
  // [0].weight=([0].frame_count)/([0].frame_max);
  // [1].weight=(1-[0].weight)*(([1].frame_count)/([1].frame_max));
  // [2].weight=(1-[0].weight)*(1-([1].frame_count)/([1].frame_max))=1-[0].weight-[1].weight;
  //
  // [n].weight=wn, ([n].frame_count)/([n].frame_max)=fn �Ƃ����
  // 
  // n=2�̂Ƃ�
  // w0=f0
  // w1=1-f0=1-w0
  //
  // n=3�̂Ƃ�
  // w0=f0
  // w1=(1-f0)*f1=(1-w0)*f1
  // w2=(1-f0)*(1-f1)=(1-w0)*(1-f1)=1-w0-w1
  //
  // n=4�̂Ƃ�
  // w0=f0
  // w1=(1-f0)*f1
  // w2=(1-f0)*(1-f1)*f2
  // w3=(1-f0)*(1-f1)*(1-f2)=1-w0-w1-w2
  //
  // n=m�̂Ƃ�
  // w0=f0
  // 1<=y<m�Ƃ����
  // wy=(1-f0)*(1-f1)*(1-f2)*...*(1-f[y-1])*fy
  // fy��n=m-1�̂Ƃ��K��1�Ƃ���
  // 
  // �ƂȂ�
 
  nw::gfx::AnimInterpolator*           m_blend;                  // �ő�m_num_max�̃A�j���[�V�������u�����h�����A�j���[�V����
  nw::gfx::AnimGroup*                  m_blend_bind_anim_group;  // �o�C���h���Ă���A�j���[�V�����O���[�v���o���Ă����BNULL�̂Ƃ��o�C���h���Ă��Ȃ��B
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DUNITANIMATION_H__
