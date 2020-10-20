#ifndef __GFL_G3DPARTICLE_H__
#define __GFL_G3DPARTICLE_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   gfl_G3dParticle.h
 *  @brief  3D�p�[�e�B�N��
 *  @author Koji Kawada
 *  @date   2011.05.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/*
�L�[���[�h
    ## �p�[�e�B�N�����\������S�Ă̂��̂ɑ΂��ē����l��ݒ肵�܂� ##
        �p�[�e�B�N���̓g�b�v��nw::gfx::TransformNode�A
        ���̉��ɕ����t�����Ƃ��ł���nw::gfx::ParticleModel�Anw::gfx::ParticleEmitter����Ȃ�B
        �����S�Ă̂��̂ɑ΂��ē����l��ݒ肷��̂ŁA
        �f�U�C�i�[���񃊃\�[�X�̒i�K�ł̓o���o���̒l�������Ƃ��Ă�
        ���̒��ӏ����̂���֐����g�p����Ɠ����l�ɂȂ��Ă��܂��B
*/


// CTR_SDK

// NW4C
#include <nw/gfx.h>
#include <nw/ut.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dSceneNode.h>
#include <grp/g3d/gfl_G3dResource.h>


namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *     �N���X�錾
 */
//-----------------------------------------------------------------------------
//-------------------------------------
/// 3D�p�[�e�B�N���N���X
//=====================================
class Particle : public SceneNode
{
  GFL_FORBID_COPY_AND_ASSIGN(Particle);


  // ���L�N���X�����private�֐��ɂ��A�N�Z�X���Ă悢
  friend class SceneCalculateGroup;
  friend class Scene;




public:
  //---------------------------------------------------------------------------
  /**
   *           �\���̐錾
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  /// �p�[�e�B�N���̐ݒ���e
  //=====================================
  struct Description
  {
    // nw::gfx::TransformNode
    //�R�[���o�b�N�Ȃ�s32 max_callbacks;         // �Ǘ��ł���R�[���o�b�N�̍ő吔��ݒ肵�܂��B
    s32 max_children;          // �q�̍ő吔��ݒ肵�܂��B
    //�����������������m�ۂ��Ȃ�b32 is_fixed_size_memory;  // �������ȊO�ɂ����������m�ۂ��邩�ǂ����̃t���O��ݒ肵�܂��Btrue ���w�肷��ƁA�������̂݌Œ�T�C�Y�̃������m�ۂ��s���܂��B

    // ���f��  // ���f���X�̐ݒ�͂ł��܂���B�S�Ẵ��f�������̐ݒ�ɂȂ�܂��B
    s32 model_max_children;

    // �G�~�b�^�[  // �G�~�b�^�[�X�̐ݒ�͂ł��܂���B�S�ẴG�~�b�^�[�����̐ݒ�ɂȂ�܂��B
    s32 emitter_max_children;

    Description(void)
      : max_children(16),
        model_max_children(16),
        emitter_max_children(16)
    {
      // �������Ȃ�
    }
  };
  static void InitializeDescriptionForDefault(
      Description*  description
  );
/*
  �֐��Őݒ肷��̂͂�����߂�J�����̂悤�ȃ^�C�v�̂�����̂����֐��ݒ�ɂ��鑼�͒��ڑ����
  static void InitializeDescription(
      Description*  description,
      s32 max_children,
      s32 model_max_children,
      s32 emitter_max_children
  );*/

  //---------------------------------------------------------------------------
  /**
   *           �萔�錾
   */
  //---------------------------------------------------------------------------
/*
  ������
  // �v�Z����邩�ǂ���
  enum CalculateState
  {
    CALCULATE_STATE_NO,  // �v�Z����Ȃ�
    CALCULATE_STATE_OK   // �v�Z�����
      // �ꕔ�v�Z������ԂȂǂ�����ǉ������\��
  };
*/
  // �`�悳��邩�ǂ���
  enum DrawState
  {
    DRAW_STATE_NO,  // �`�悳��Ȃ�
    DRAW_STATE_OK   // �`�悳���
  };

public:
  Particle(void);
  virtual ~Particle();

  // resource�Ɋ܂܂��S�Ă�Model�A�S�Ă�Emitter���g�p���ăp�[�e�B�N���𐶐�����
  void Create(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    Resource*               resource,
    const Description*      description = NULL
  );

/*
  // resource�Ɋ܂܂��name_in_resource_array��Model, Emitter���g�p���ăp�[�e�B�N���𐶐�����
  // ��
  // static const char* NAME_ARRAY[4] =
  // {
  //   "FireModel",
  //   "FireEmitter",
  //   "SmokeModel",
  //   "SmokeEmitter",
  // };
  void Create(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    Resource*               resource,
    u32                     name_num,
    const char**            name_in_resource_array,
    const Description*      description = NULL
  );

  // resource�Ɋ܂܂��model_index_in_resource_array, emitter_index_in_resource_array���g�p���ăp�[�e�B�N���𐶐�����
  // ��
  // static const s32 MODEL_INDEX_ARRAY[2] =
  // {
  //   0,
  //   1,
  // }
  // static const s32 EMITTER_INDEX_ARRAY[2] =
  // {
  //   0,
  //   1,
  // }
  void Create(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    Resource*               resource,
    u32                     model_num,
    const s32*              model_index_in_resource_array,
    u32                     emitter_num,
    const s32*              emitter_index_in_resource_array,
    const Description*      description = NULL
  );
*/

  virtual void Destroy(void);


public:
  // @note GetNwTransformNode�͍������̊֐��Ȃ̂ŁA�g�p���Ȃ��ŉ������B���R�����g�A�E�g���Ďg���Ȃ������B
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���̃p�[�e�B�N���̃g�b�v�m�[�h���擾
   * 
   *  @retval        nw::gfx::TransformNode*    ���̃p�[�e�B�N���̃g�b�v�m�[�h
   */
  //-----------------------------------------------------------------------------
  //nw::gfx::TransformNode* GetNwTransformNode(void) const { return SceneNode::GetNwTransformNode(); }


public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���s�ړ�
   */
  //-----------------------------------------------------------------------------
  void SetTranslate(f32 x, f32 y, f32 z);
  void SetTranslate(const gfl::math::VEC3& t);
  void GetTranslate(gfl::math::VEC3* t) const;
  gfl::math::VEC3 GetTranslate(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ��]
   *                 �P�ʂ̓��W�A���B
   */
  //-----------------------------------------------------------------------------
  void SetRotateXYZ(f32 x, f32 y, f32 z);
  void SetRotateXYZ(const gfl::math::VEC3& t);
  template <typename TMatrix> void SetRotateMatrix(const TMatrix& m)
  {
    nw::gfx::TransformNode* transform_node = GetNwTransformNode();
    transform_node->Transform().SetRotateMatrix(m);

    {
      nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note �h���N���X�ŃA�N�Z�X���āA�X�̔h���N���X�ŏ�������̂ł͂Ȃ��ASceneNode�ŏ����������B
      constraint_calculated_transform.SetRotateMatrix(m);
    }
  }
  void SetRotateMatrix(
      f32 m00,
      f32 m01,
      f32 m02,
      f32 m10,
      f32 m11,
      f32 m12,
      f32 m20,
      f32 m21,
      f32 m22
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �X�P�[��
   */
  //-----------------------------------------------------------------------------
  void SetScale(f32 x, f32 y, f32 z);
  void SetScale(const gfl::math::VEC3& t);
  void GetScale(gfl::math::VEC3* t) const;
  gfl::math::VEC3 GetScale(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �`�揇��ݒ肷��
   *                 �v���O���}�������ݒ�ł��郂�f���̃��C���[ID���p�[�e�B�N�����f���ɐݒ肷��
   *                 �ŏ��ɕ`�� 0 <= u8 <= 127 �Ō�ɕ`��
   *                 �ŗD�悳���`�揇 ModelLayerId > MaterialTranslucencyKind > ���̑� �ŗ�コ���`�揇
   *
   *                 **** Model��Particle�ɂ���S�������ɕ��ׂ�����ԍ��ł��B�f�t�H���g0000000�ł��B ****
   *
   *  ## �p�[�e�B�N�����\������S�Ă̂��̂ɑ΂��ē����l��ݒ肵�܂� ##
   *
   *  @param[in]    model_layer_id    ���f���̃��C���[ID
   */
  //-----------------------------------------------------------------------------
  void SetModelLayerId(u8 model_layer_id);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �`�揇���擾����
   *                 �v���O���}�������ݒ�ł��郂�f���̃��C���[ID���p�[�e�B�N������擾����
   *                 �ŏ��ɕ`�� 0 <= u8 <= 127 �Ō�ɕ`��
   *                 �ŗD�悳���`�揇 ModelLayerId > MaterialTranslucencyKind > ���̑� �ŗ�コ���`�揇
   *
   *                 **** Model��Particle�ɂ���S�������ɕ��ׂ�����ԍ��ł��B�f�t�H���g0000000�ł��B ****
   *
   *  ## �p�[�e�B�N�����\������S�Ă̂��̂̒��Ő擪�̂��̂̒l���擾���܂�(�擪�Ɛ擪�ȊO�ňقȂ�l���ݒ肳��Ă���Ƃ����������ƂɂȂ邩������܂���) ##
   *
   *  @retval       u8    ���f���̃��C���[ID
   */
  //-----------------------------------------------------------------------------
  u8 GetModelLayerId(void) const;

public:
#if 0
  //������
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �v�Z�t���O
   *                 true�̂Ƃ��v�Z�����(@note �S���H�ꕔ�H)
   *                 ���������ꂪtrue�̂Ƃ��ł��AGetTreeFlag��false�̂Ƃ��͌v�Z����Ȃ�
   */
  //-----------------------------------------------------------------------------
  void SetCalculateFlag(b32 flag);
  b32  GetCalculateFlag(void) const;
#endif
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �`��t���O
   *                 true�̂Ƃ��`�悳���
   *                 ���������ꂪtrue�̂Ƃ��ł��AGetTreeFlag��false�̂Ƃ��͕`�悳��Ȃ�
   */
  //-----------------------------------------------------------------------------
  virtual void SetVisible(b32 visible);
  virtual b32  IsVisible(void) const;

#if 0
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���c���[�t���O
   *                 true�̂Ƃ�Scene��Add����Ă�����V�[���c���[�ɓ������Afalse�̂Ƃ�Scene��Add����Ă��Ă��V�[���c���[����O�����
   *                 ���ꂪtrue�ł��邱�ƂƁA���[�U��Scene��Add�������Ƃ́A�����ł͂���܂���B
   */
  //-----------------------------------------------------------------------------
  void SetTreeFlag(b32 flag);
  b32  GetTreeFlag(void) const;
#endif

#if 0
  //������
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �v�Z����邩�ǂ���
   */
  //-----------------------------------------------------------------------------
  CalculateState GetCalculateState(void) const;
#endif

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �`�悳��邩�ǂ���
   */
  //-----------------------------------------------------------------------------
  DrawState GetDrawState(void) const;


  //-----------------------------------------------------------------------------
  /**
   *  @brief   �p�[�e�B�N�����f���̌��𓾂�
   *
   *  @retval    �p�[�e�B�N�����f���̌�
   */
  //-----------------------------------------------------------------------------
  s32 GetModelNum(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief   �p�[�e�B�N�����f���𓾂�
   *
   *  @param[in] index  �C���f�b�N�X
   *  @retval    �p�[�e�B�N�����f��
   */
  //-----------------------------------------------------------------------------
  nw::gfx::ParticleModel* GetModel(s32 index) const;

public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief  �X�V�t���[����ݒ肷��
   *
   *  ## �p�[�e�B�N�����\������S�Ă̂��̂ɑ΂��ē����l��ݒ肵�܂� ##
   * 
   *  0.0f�Œ�~�A1.0f�œ��{�A2.0f��2�{��
   *  �p�[�e�B�N���͋t�Đ��͂ł��Ȃ��̂ŁA-1.0f�Ȃǂ̕��̒l�͐ݒ�ł��܂���B
   * 
   *  @param[in]     step_frame      �X�V�t���[��
   */
  //-----------------------------------------------------------------------------
  void SetStepFrame(const f32 step_frame);
#if 0
  //�ċA�͎~�߂��B
  // friend�N���X����A�N�Z�X���Ă͂����Ȃ�private�����o�֐�
private:
  void SetStepFrameRecursively(
      nw::gfx::SceneNode*   nw_scene_node,
      const f32             step_frame
#if GFL_DEBUG
      , const s32           recursive_count 
#endif
  );
#endif

  //-----------------------------------------------------------------------------
  /**
   *  @brief  �t���[����ݒ肷��
   *
   *  ## �p�[�e�B�N�����\������S�Ă̂��̂ɑ΂��ē����l��ݒ肵�܂� ##
   * 
   *  @param[in]     frame      �t���[��
   */
  //-----------------------------------------------------------------------------
  void SetFrame(const f32 frame);

  //-----------------------------------------------------------------------------
  /**
   *  @brief  �t���[�����擾����
   * 
   *  @retval      �t���[��
   */
  //-----------------------------------------------------------------------------
  f32 GetFrame( void ) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief  ���Z�b�g���s���A������Ԃɖ߂�
   *          GPU�������̓R�[���ł��܂���B
   *          ���̂ǂ��炩�𖞂�����GPU�������ł͂���܂���B
   *          (1) �V�[���o�^���������Ă���B(�R�}���h�o�b�t�@���_�u���̂Ƃ��̓V�[���o�^���������Ă���1�t���[���ȏ�o�߂��Ă���)
   *          (2) �I����Ԃł���B(�R�}���h�o�b�t�@���_�u���̂Ƃ��͏I����ԂɂȂ��Ă���1�t���[���ȏ�o�߂��Ă���)
   */
  //-----------------------------------------------------------------------------
  void Reset(void);
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  �G�~�b�^�[�݂̂̃��Z�b�g
   */
  //-----------------------------------------------------------------------------
  void ResetEmitter(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief   ���o�ʂ�ݒ肷��
   *
   *  ## �p�[�e�B�N�����\������S�Ă̂��̂ɑ΂��ē����l��ݒ肵�܂� ##
   *
   *  @param[in]     ratio  ���o��(0.0f�őS�����o����Ȃ�)
   */
  //-----------------------------------------------------------------------------
  void SetEmitterEmissionRatio(f32 ratio);

  //-----------------------------------------------------------------------------
  /**
   *  @brief   ���o�ʂ�ݒ肷��
   *           name�Ƃ������O���t�����S�ẴG�~�b�^�[�ɕ��o�ʂ�ݒ肷��
   *           name�Ƃ������O���t�����G�~�b�^�[���Ȃ��ꍇ�͉������Ȃ�
   *
   *  @param[in]     name   �G�~�b�^�[�̖��O
   *  @param[in]     ratio  ���o��(0.0f�őS�����o����Ȃ�)
   */
  //-----------------------------------------------------------------------------
  void SetEmitterEmissionRatio(const char* name, f32 ratio);

  //-----------------------------------------------------------------------------
  /**
   *  @brief   ���o�ʂ��擾����
   *           �ŏ��Ɍ��t����name�Ƃ������O���t�����G�~�b�^�[�̕��o�ʂ�Ԃ�
   *           name�Ƃ������O���t�����G�~�b�^�[���Ȃ��ꍇ��0.0f��Ԃ�
   *
   *  @param[in]     name   �G�~�b�^�[�̖��O
   *
   *  @retval        f32    ���o��(0.0f�őS�����o����Ȃ�)
   */
  //-----------------------------------------------------------------------------
  f32 GetEmitterEmissionRatio(const char* name) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief   ���݂̃p�[�e�B�N���̌����擾����
   *           �p�[�e�B�N�����\������S�Ă̂��̂Ō��ݑ��݂�����̂̌��̘a��Ԃ��܂�
   *
   *  @retval  s32  ���݂̃p�[�e�B�N���̌�
   */
  //-----------------------------------------------------------------------------
  s32  GetParticleCount(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief   �I����Ԃ��ǂ������擾����
   *           �p�[�e�B�N�����\������S�Ă̂��̂��I����Ԃł����true��Ԃ��܂�
   *           1�ł��I����ԂłȂ����false��Ԃ��܂�
   *
   *  @retval  b32  �I����Ԃ̂Ƃ�true
   */
  //-----------------------------------------------------------------------------
  b32 IsDone(void) const; 

  //-----------------------------------------------------------------------------
  /**
   *  @brief   �������o��ݒ肷��
   *           �G�~�b�^�[����̕��o�ɏI��肪���邩�ۂ��̐ݒ�ł��B1��1�̗��q�̎����̐ݒ�ł͂���܂���B
   * 
   *  ## �p�[�e�B�N�����\������S�Ă̂��̂ɑ΂��ē����l��ݒ肵�܂� ##
   *
   *  @param[in]  infinity  �������o�ɂ���Ƃ�true
   */
  //-----------------------------------------------------------------------------
  void SetEmitterEmissionSpanInfinity(b32 infinity);

  //-----------------------------------------------------------------------------
  /**
   *  @brief   �������o���ǂ������擾����
   *           �p�[�e�B�N�����\������S�Ă̂��̂��������o�łȂ����false��Ԃ��܂�
   *           1�ł��������o�ł����true��Ԃ��܂�
   *
   *  @retval  b32  �������o�̂Ƃ�true
   */
  //-----------------------------------------------------------------------------
  b32 IsEmitterEmissionSpanInfinity(void) const;

  // friend�N���X����̂݃A�N�Z�X���Ă��悢private�����o�֐�
private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief   �Z�b�g�A�b�v
   *
   *  @param[in] particle_context �p�[�e�B�N���R���e�L�X�g
   */
  //-----------------------------------------------------------------------------
  void Setup(nw::gfx::ParticleContext* particle_context);

  //-----------------------------------------------------------------------------
  /**
   *  @brief   �w�萔�����t���[������
   *
   *  ## �p�[�e�B�N�����\������S�Ă̂��̂ɑ΂��ē����l��ݒ肵�܂� ##
   *
   *  @param[in] times            �t���[�����񂷉�
   *  @param[in] particle_context �p�[�e�B�N���R���e�L�X�g
   */
  //-----------------------------------------------------------------------------
  void AddTimes(u32 times, nw::gfx::ParticleContext* particle_context);

private:
  //b32             m_tree_flag;  // true�̂Ƃ�Scene��Add����Ă�����V�[���c���[�ɓ������Afalse�̂Ƃ�Scene��Add����Ă��Ă��V�[���c���[����O�����
  
  b32 m_is_setup;                                                 // �Z�b�g�A�b�v���������Ă�����true
  //nw::ut::MoveArray<nw::gfx::SceneNode*>* m_scene_node_array;     // ���������V�[���m�[�h���Ǘ����Ă����z��
  nw::gfx::SceneNodeArray*                m_scene_node_array;     // ���������V�[���m�[�h���Ǘ����Ă����z��

  //nw::ut::MoveArray<nw::gfx::ParticleModel*>*   m_model_array;    // ���������p�[�e�B�N�����f�����Ǘ����Ă����z��
  nw::gfx::ParticleModelArray*                  m_model_array;    // ���������p�[�e�B�N�����f�����Ǘ����Ă����z��
      // m_scene_node_array�Ɠ������̂��w���z��Ȃ̂ŁA����/�j����m_scene_node_array�ɔC����B
  //nw::ut::MoveArray<nw::gfx::ParticleEmitter*>* m_emitter_array;  // ���������p�[�e�B�N���G�~�b�^�[���Ǘ����Ă����z��
  nw::gfx::ParticleEmitterArray*                m_emitter_array;  // ���������p�[�e�B�N���G�~�b�^�[���Ǘ����Ă����z��
      // m_scene_node_array�Ɠ������̂��w���z��Ȃ̂ŁA����/�j����m_scene_node_array�ɔC����B
  
#if GFL_DEBUG    
  public:
    //----------------------------------------------------------------
    /**
     * @brief ���S�Ƀ��f��������ł��������m�F����@�\�̗L�������ݒ�
     * @param fEnable
     */
    //----------------------------------------------------------------
    static void SafeDeleteCheckEnable( b32 fEnable )
    {
      s_SafeDeleteCheckEnable        = fEnable;
      s_SafeDeleteCheckEnableUserSet = fEnable;
      if(s_SafeDeleteCheckForce) s_SafeDeleteCheckEnable = true;
    }
    static b32 IsSafeDeleteCheckEnable(){ return s_SafeDeleteCheckEnable; }

    static void SafeDeleteCheckForce(b32 fForce)
    {
      s_SafeDeleteCheckForce = fForce;
      if(s_SafeDeleteCheckForce)
      {
        s_SafeDeleteCheckEnable = true;
      }
      else
      {
        s_SafeDeleteCheckEnable = s_SafeDeleteCheckEnableUserSet;
      }
    }
    static b32 IsSafeDeleteCheckForce(void) { return s_SafeDeleteCheckForce; }

    static void SafeDeleteCheckAssert(b32 fAssert){ s_SafeDeleteCheckAssert = fAssert; }
    static b32  IsSafeDeleteCheckAssert(void) { return s_SafeDeleteCheckAssert; }

    void SetDrawFrameCount( u32 cnt ){ m_DrawFrameCount = cnt; }
    u32 GetDrawFrameCount(){ return m_DrawFrameCount; }
    
    void SetModelName( const char* pName );
    
  private:
    static b32                  s_SafeDeleteCheckEnable;
    static b32                  s_SafeDeleteCheckEnableUserSet;  // Force�I����Ƀ��[�U�ݒ�l�ɖ߂���悤�ɋL�����Ă����B
    static b32                  s_SafeDeleteCheckForce;          // true�̂Ƃ�s_SafeDeleteCheckEnable��true�ŌŒ肷��B
    static b32                  s_SafeDeleteCheckAssert;         // true�̂Ƃ�ASSERT�Ŏ~�߂�Bfalse�̂Ƃ�PRINT�ōς܂���B

    u32                         m_DrawFrameCount;
    char                        m_ModelName[32];
#endif  
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // #ifndef __GFL_G3DPARTICLE_H__
