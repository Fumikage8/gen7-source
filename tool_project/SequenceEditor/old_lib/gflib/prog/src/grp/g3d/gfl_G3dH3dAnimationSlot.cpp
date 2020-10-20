//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dAnimationSlot.cpp
 *	@brief  H3D�A�j���[�V�����X���b�g
 *	@author	Koji Kawada
 *	@date		2012.04.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>
#include <gfl_fs.h>

// gflib grp
#include <grp/gfl_GraphicsType.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>



#if GFL_GRP_G3D_H3D_USE

// NW4C
#include <nw/h3d.h>
// gflib grp g3dh3d
#include <grp/g3d/gfl_G3dH3dModel.h>
#include <grp/g3d/gfl_G3dH3dAnimationSlot.h>



namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					�N���X����
*/
//-----------------------------------------------------------------------------

H3dAnimationSlot::H3dAnimationSlot(void)
  : m_buff(NULL),
    m_model(NULL),
    m_type(ANIMATION_TYPE_NONE),
    m_anim_elem_num_max(0),
    m_res_anim(NULL),
    m_list_num(0),
    m_res_anim_list(NULL),
    m_frame(0.0f),
    m_step_frame(1.0f),
    m_start_frame(0.0f),
    m_end_frame(0.0f)
{
  initializeFrame();
}

H3dAnimationSlot::~H3dAnimationSlot()
{
  Destroy();
}

b32 H3dAnimationSlot::Create(gfl::heap::NwAllocator* heap_allocator, H3dModel* model, const Description* desc)
{
  if( ( desc->type != ANIMATION_TYPE_NONE ) && ( desc->anim_elem_num_max > 0 ) )
  {
    return CreateFromAnimElementNumMax(heap_allocator, model, desc->type, desc->anim_elem_num_max);
  }
  else if( desc->res_anim )
  {
    return CreateFromResAnim(heap_allocator, model, desc->res_anim);
  }
  else if( desc->list_max > 0 && desc->res_anim_list )
  {
    return CreateFromResAnimList(heap_allocator, model, desc->list_max, desc->res_anim_list);
  }
  return false;
}

b32 H3dAnimationSlot::CreateFromAnimElementNumMax(gfl::heap::NwAllocator* heap_allocator, H3dModel* model, AnimationType type, u32 anim_elem_num_max)
{
  // �o�C���h����A�j���[�V�����̃A�j���[�V�����v�f���� Init() �Ŏw�肷�� maxAnimElementCount �̒l�ȉ��ł���K�v������܂��B ����𒴂���A�j���[�V�������o�C���h����Ƃ��́A �ēx Init() �ŏ��������s���K�v������܂��B

  m_buff = GflHeapAllocMemoryAlign(
      heap_allocator->GetHeapBase(),
      nw::h3d::AnimBindState::CalcSize(anim_elem_num_max), 4);
  m_bind_state.Init(m_buff, anim_elem_num_max);

  m_model = model;
  m_type = type;
  m_anim_elem_num_max = anim_elem_num_max;

  return true;
}
b32 H3dAnimationSlot::CreateFromResAnim(gfl::heap::NwAllocator* heap_allocator, H3dModel* model, const H3dResAnim* res_anim)
{
  return CreateFromAnimElementNumMax(heap_allocator, model, res_anim->GetAnimationType(), res_anim->GetAnimElementNum());
}
b32 H3dAnimationSlot::CreateFromResAnimList(
    gfl::heap::NwAllocator* heap_allocator,
    H3dModel* model, 
    s32 list_num,
    H3dResAnim** res_anim_list
)
{
  AnimationType type = ANIMATION_TYPE_NONE;
  u32 anim_elem_num_max  = 0;
  for(s32 i=0; i<list_num; ++i)
  {
    if(res_anim_list[i])
    {
      // �A�j���[�V�����^�C�v
      AnimationType curr_type = res_anim_list[i]->GetAnimationType();
      if( curr_type == ANIMATION_TYPE_NONE )
      {
        GFL_ASSERT_MSG(0, "�A�j���[�V�����^�C�v���s���ł��B\n");
      }

      if( type != ANIMATION_TYPE_NONE )
      {
        if( curr_type != type )
        {
          GFL_ASSERT_MSG(0, "�A�j���[�V�����^�C�v���قȂ�܂��B\n");
        }
      }
      else
      {
        type = curr_type;
      }

      // �v�f��
      u32 num = res_anim_list[i]->GetAnimElementNum();
      if( num > anim_elem_num_max ) anim_elem_num_max = num;     
    }
  }

  m_list_num      = list_num;
  m_res_anim_list = res_anim_list;
  
  return CreateFromAnimElementNumMax(heap_allocator, model, type, anim_elem_num_max);
}

void H3dAnimationSlot::Destroy(void)
{
  if(m_buff) GflHeapFreeMemory(m_buff);

  // �R���X�g���N�^�Ɠ���������
  m_buff = NULL;
  m_model = NULL;
  m_type = ANIMATION_TYPE_NONE;
  m_anim_elem_num_max  = 0;
  m_frame = 0.0f;
  m_step_frame = 1.0f;
  m_start_frame = 0.0f;
  m_end_frame = 0.0f;
  m_res_anim = NULL;
  m_list_num = 0;
  m_res_anim_list = NULL;
}

void H3dAnimationSlot::Calculate(void)
{
#if 0
��Calculate������O�̏���

  if( m_res_anim )
  {
    switch(m_type)
    {
    case ANIMATION_TYPE_SKELETAL:
      {
        m_res_anim->GetNwSkeletalAnimContent()->CalcAndApply( m_model->GetNwSkeletonState(), &m_bind_state, m_calc_frame );
        break;
      }
    case ANIMATION_TYPE_MATERIAL:
      {
        const H3dResMaterialAnim* res_mat_anim = static_cast<H3dResMaterialAnim*>(m_res_anim);
        m_res_anim->GetNwMaterialAnimContent()->\
          CalcAndApply( m_model->GetNwMaterialStateArray(), &m_bind_state, res_mat_anim->GetNwTexturePatternAnimBindState(), m_calc_frame );
        break;
      }
    case ANIMATION_TYPE_VISIBILITY:
      {
        m_res_anim->GetNwVisibilityAnimContent()->CalcAndApply( m_model->GetNwVisibilityState(), &m_bind_state, m_calc_frame );
        break;
      }
    }

    // �t���[����i�߂Ă���
    stepFrame();
  }

#else
  CalculateLocalSkeletalAnimation();
  CalculateVisibilityAnimation();
  CalculateMaterialAnimation();
  CalculateAnimationFrame();
  CalculateWorldSkeletalAnimation();
#endif
}


void H3dAnimationSlot::CalculateLocalSkeletalAnimation(void)
{
  if( m_res_anim )
  {
    switch(m_type)
    {
    case ANIMATION_TYPE_SKELETAL:
      {
        m_res_anim->GetNwSkeletalAnimContent()->CalcAndApply( m_model->GetNwSkeletonState(), &m_bind_state, m_calc_frame );
        break;
      }
    }
  }
}
void H3dAnimationSlot::CalculateVisibilityAnimation(void)
{
  if( m_res_anim )
  {
    switch(m_type)
    {
    case ANIMATION_TYPE_VISIBILITY:
      {
        m_res_anim->GetNwVisibilityAnimContent()->CalcAndApply( m_model->GetNwVisibilityState(), &m_bind_state, m_calc_frame );
        break;
      }
    }
  }
}
void H3dAnimationSlot::CalculateMaterialAnimation(void)
{
  if( m_res_anim )
  {
    switch(m_type)
    {
    case ANIMATION_TYPE_MATERIAL:
      {
        const H3dResMaterialAnim* res_mat_anim = static_cast<H3dResMaterialAnim*>(m_res_anim);
        m_res_anim->GetNwMaterialAnimContent()->\
          CalcAndApply( m_model->GetNwMaterialStateArray(), &m_bind_state, res_mat_anim->GetNwTexturePatternAnimBindState(), m_calc_frame );
        break;
      }
    }
  }
}
void H3dAnimationSlot::CalculateAnimationFrame(void)
{
  if( m_res_anim )
  {
    // �t���[����i�߂Ă���
    stepFrame();
  }
}
void H3dAnimationSlot::CalculateWorldSkeletalAnimation(void)
{
}




H3dAnimationSlot::ChangeAnimationResult H3dAnimationSlot::ChangeByResAnim(H3dResAnim* res_anim)
{
  if( !res_anim )
  {
    unbindAnimation();
    return CHANGE_ANIMATION_RESULT_OK;
  }
  else if( res_anim->GetAnimationType() != m_type )
  {
    unbindAnimation();
    return CHANGE_ANIMATION_RESULT_DIFFERENT_TYPE;
  }
  else if( res_anim->GetAnimElementNum() > m_anim_elem_num_max )
  {
    unbindAnimation();
    return CHANGE_ANIMATION_RESULT_OVER_ELEMENT_NUM;
  }
  else
  {
    nw::h3d::Result::Description res = bindAnimation(res_anim);
    if( res == nw::h3d::Result::RESULT_ANIM_ANY_ELEMENT_NOT_BOUND )
    {
      unbindAnimation();
      return CHANGE_ANIMATION_RESULT_ANY_ELEMENT_NOT_BOUND;
    }
    else if( res == nw::h3d::Result::RESULT_ANIM_SOME_ELEMENT_NOT_BOUND )
    {
      return CHANGE_ANIMATION_RESULT_SOME_ELEMENT_NOT_BOUND;
    }
  }
  
  return CHANGE_ANIMATION_RESULT_OK;
}
H3dAnimationSlot::ChangeAnimationResult H3dAnimationSlot::ChangeByResAnimListIndex(s32 list_index)
{
  if( 0 <= list_index && list_index < m_list_num )
  {
    return ChangeByResAnim(m_res_anim_list[list_index]);
  }
  else if( list_index == LIST_INDEX_NONE )
  {
    return ChangeByResAnim(NULL);
  }
  else
  {
    GFL_ASSERT_MSG(0, "CHANGE_ANIMATION_RESULT_LIST_INDEX_ERROR");
    ChangeByResAnim(NULL);
    return CHANGE_ANIMATION_RESULT_LIST_INDEX_ERROR;
  }
}

nw::h3d::Result::Description H3dAnimationSlot::bindAnimation(H3dResAnim* res_anim)
{
  // maxAnimElementCount �𒴂��Ȃ��ꍇ�́A �ēx BindSkeletalAnim() / BindMaterialAnim() / BindVisibilityAnim() ���ĂԂ��Ƃōăo�C���h���ł��܂��B

  nw::h3d::Result result(nw::h3d::Result::RESULT_ANIM_ANY_ELEMENT_NOT_BOUND);  // �������Ȃ������Ƃ���Result�ŏ��������Ă���
  const nw::h3d::res::ModelContent* model = m_model->GetNwModelContent();
  switch(m_type)
  {
  case ANIMATION_TYPE_SKELETAL:
    {
      if( m_model->GetBonesCount() > 0 )
        result = m_bind_state.BindSkeletalAnim(res_anim->GetNwSkeletalAnimContent(), model);
      break;
    }
  case ANIMATION_TYPE_MATERIAL:
    {
      result = m_bind_state.BindMaterialAnim(res_anim->GetNwMaterialAnimContent(), model);
      break;
    }
  case ANIMATION_TYPE_VISIBILITY:
    {
      result = m_bind_state.BindVisibilityAnim(res_anim->GetNwVisibilityAnimContent(), model);
      break;
    }
  }

  // �o�C���h����
  nw::h3d::Result::Description res_desc = static_cast<nw::h3d::Result::Description>(result.GetDescription());
  //GFL_ASSERT_MSG(res_desc==nw::h3d::Result::RESULT_OK, "nw::h3d::AnimBindState::Bind_%d_Anim Result=%d\n", m_type, res_desc);  //@check �����ɂЂ�������Ȃ��悤�Ɋm�F�ς̈�����Ă����B
 
  m_res_anim = res_anim;
  initializeFrame();

  // �}�e���A���A�j���[�V�����̑Ώۂ�Mutable�t���O��ݒ肷��
  if( m_type == ANIMATION_TYPE_MATERIAL )
  {
    // ���ꂪ�Ȃ��ƃe�N�X�`��UV�A�j���[�V���������f����Ȃ������B
    m_res_anim->GetNwMaterialAnimContent()->\
      SetMaterialMutableFlag( m_model->GetNwMaterialStateArray(), &m_bind_state );
        // @note �A�j����t���ւ����Ƃ��ɁA�O�̃A�j���ŗ��Ă��t���O�����ɖ߂��Ȃ��Ă������낤���H ���ʂȃt���O���������܂܂��Ə������x���Ȃ����肵�Ȃ����H
        //       ���X�g�C���f�b�N�X�ŃA�j����t���ւ���Ƃ��́A���X�g�ɓo�^����Ă���S�A�j���ōŏ��Ƀt���O�𗧂ĂĈȍ~�ς��Ȃ��̂��悳���������B
        //       �����t�@�����X�J�E���^�ł��p�ӂ��Ȃ�����A�t���O�����ɖ߂��Ȃ��B�Ȃ��Ȃ�A�}�e���A���A�j���𕡐��t���Ă���\�������邩��B
        //         �}�e���A���A�j���[�V����0�ŐF��ς��A�}�e���A���A�j���[�V����0���O���A�}�e���A���A�j���[�V����1�ł͂��̐F��ς��Ȃ��B
        //         ���̂Ƃ����[�U�̓}�e���A���A�j���[�V����0���ς����F�͕ς���O�̐F�ɖ߂��ė~�������낤���H
        //         �����炭�߂��ė~�����Ǝv���̂ŁA�ǂ����悤�B�B�B
        //         MaterialState���ύX��̐F���o���Ă���̂ŁA�}�e���A���A�j���[�V����0�̍Ō�̏�Ԃ̐F��ۂ��Ƃ��ł���B
        //         ���f���ɂ��Ă���S�Ẵ}�e���A��or�C�ӂ̃}�e���A���̃t���O���f�t�H���g�ɖ߂��֐���p�ӂ���̂ŁA
        //         ��U���ɖ߂��������[�U�ɂ́A
        //         �}�e���A���A�j���[�V����0���O���A�}�e���A���̃t���O���f�t�H���g�ɖ߂��A�}�e���A���A�j���[�V����1������A�Ƃ����菇�𓥂�ł��炨���Ǝv���B
  }

  return res_desc;
}

void H3dAnimationSlot::unbindAnimation(void)
{
  // NintendoWare/CTR/sources/libraries/h3d/h3d_AnimStateImpl.h
  // template <class IndexGetter>
  // Result BindAnim(AnimBindState* state, const res::AnimContent* animContent, const res::ModelContent* model)

  m_res_anim = NULL;
  initializeFrame();

  u32 num = m_bind_state.GetMaxAnimElementCount();
  for(s32 i=0; i<num; ++i)
  {
    m_bind_state.SetBindTarget(i, nw::h3d::AnimBindState::UNBOUND);
  }
}


H3dResAnim* H3dAnimationSlot::GetAnimationAsResAnim(void) const
{
  return m_res_anim;
}



void H3dAnimationSlot::SetFrame(f32 frame)
{
  if( m_res_anim )
  {
    m_frame = frame;
    normalizeFrame();
  }
}
f32 H3dAnimationSlot::GetFrame(void) const
{
  if( m_res_anim )
  {
    return m_frame;
  }
  return 0.0f;
}

void H3dAnimationSlot::SetStepFrame(f32 step_frame)
{
  if( m_res_anim )
  {
    m_step_frame = step_frame;
  }
}
f32 H3dAnimationSlot::GetStepFrame(void) const
{
  if( m_res_anim )
  {
    return m_step_frame;
  }
  return 0.0f;
}

void H3dAnimationSlot::SetStartFrame(f32 start_frame)
{
  if( m_res_anim )
  {
    m_start_frame = start_frame;
    normalizeFrame();
  }
}
f32 H3dAnimationSlot::GetStartFrame(void) const
{
  if( m_res_anim )
  {
    return m_start_frame;
  }
  return 0.0f;
}

void H3dAnimationSlot::SetEndFrame(f32 end_frame)
{
  if( m_res_anim )
  {
    // �A�j���[�V���������W�𒴂��Ă��Ȃ����`�F�b�N
    if( m_res_anim->GetFrameSize() < end_frame ){
      GFL_ASSERT_MSG( m_res_anim->GetFrameSize() >= end_frame, "SetEndFrame Size Over" );
      end_frame = m_res_anim->GetFrameSize();
    }
    m_end_frame = end_frame;
    normalizeFrame();
  }
}
f32 H3dAnimationSlot::GetEndFrame(void) const
{
  if( m_res_anim )
  {
    return m_end_frame;
  }
  return 0.0f;
}

void H3dAnimationSlot::SetStartAndEndFrame(f32 start_frame, f32 end_frame)
{
  if( m_res_anim )
  {
    m_start_frame = start_frame;
    m_end_frame = end_frame;
    normalizeFrame();
  }
}

b32 H3dAnimationSlot::IsFrameStartFrame(void) const
{
  if( m_res_anim )
  {
    if( COMPARE(m_frame, m_start_frame, EPSILON)==0 ) return true;
  }
  return false;
}
b32 H3dAnimationSlot::IsFrameEndFrame(void) const
{
  if( m_res_anim )
  {
    if( COMPARE(m_frame, m_end_frame, EPSILON)==0 ) return true;
  }
  return false;
}
b32 H3dAnimationSlot::IsFrameStepTerminalFrame(void) const
{
  b32 ret = false;
  if( m_res_anim )
  {
    if( m_step_frame > 0.0f )
    {
      ret = IsFrameEndFrame();
    }
    else if( m_step_frame < 0.0f )
    {
      ret = IsFrameStartFrame();
    }
    else
    {
      if( IsFrameStartFrame() || IsFrameEndFrame() ) ret = true;
    }
  }
  return ret;
}

b32 H3dAnimationSlot::IsLoop(void) const
{
  if( m_res_anim )
  {
    return m_res_anim->IsLoop();
  }
  return false;
}
void H3dAnimationSlot::SetLoop(b32 loop)
{
  if( m_res_anim )
  {
    m_res_anim->SetLoop(loop);
  }
}

void H3dAnimationSlot::initializeFrame(void)
{
  m_frame = 0.0f;
  m_calc_frame = 0.0f;
  m_step_frame = 1.0f;
  m_start_frame = 0.0f;

  if( m_res_anim )
  {
    m_end_frame = m_res_anim->GetFrameSize();
  }
  else
  {
    m_end_frame = 0.0f;
  }
}

void H3dAnimationSlot::stepFrame(void)
{
  if( m_res_anim )
  {
    m_frame = staticStepFrame(m_frame, m_step_frame, m_start_frame, m_end_frame, m_res_anim->IsLoop());
    normalizeFrame();
  }
}

void H3dAnimationSlot::normalizeFrame(void)
{
  if( m_res_anim )
  {
    m_frame = staticNormalizeFrame(m_frame, m_start_frame, m_end_frame, m_res_anim->IsLoop());
    m_calc_frame = m_res_anim->NormalizeFrame(m_frame);
  }
}

f32 H3dAnimationSlot::staticStepFrame(f32 curr_frame, f32 step_frame, f32 start_frame, f32 end_frame, b32 loop)
{
  f32 next_frame = curr_frame + step_frame;
  next_frame = staticNormalizeFrame(next_frame, start_frame, end_frame, loop); 
  return next_frame;
}

f32 H3dAnimationSlot::staticNormalizeFrame(f32 frame, f32 start_frame, f32 end_frame, b32 loop)
{
  f32 nor_frame = frame;
  if( loop )
  {
    // ��F
    // start_frame = 5.0f
    // end_frame = 10.0f
    // ���̂P
    // frame = 10.0f �̂Ƃ� 5.0f�ɂȂ�
    // ���̂Q
    // frame = 11.0f �̂Ƃ� 6.0f�ɂȂ�
    while(nor_frame >= end_frame)
    {
      nor_frame -= end_frame;
    }
    while(nor_frame < start_frame)
    {
      nor_frame += start_frame;
    }
    // �ǂ������_���Ȃ�X�^�[�g�t���[���ɂȂ�悤�ȋL�q���ɂ��Ă���
  }
  else
  {
    if(nor_frame > end_frame)   nor_frame = end_frame;
    if(nor_frame < start_frame) nor_frame = start_frame;
    // �ǂ������_���Ȃ�X�^�[�g�t���[���ɂȂ�悤�ȋL�q���ɂ��Ă���
  }
  return nor_frame;
}

s32 H3dAnimationSlot::COMPARE(f32 a, f32 b, f32 e)
{
  s32 ret = 0;
  //if( b < a+e || a-e < b ) ret = 0;  ���܂���
  //if( a-e < b && b < a+e ) ret = 0;  ���܂��߂Q
  if( nn::math::FAbs( a- b ) <= e ) ret = 0;
  else if( a > b ) ret = 1;
  else ret = -1;
  return ret;
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE

