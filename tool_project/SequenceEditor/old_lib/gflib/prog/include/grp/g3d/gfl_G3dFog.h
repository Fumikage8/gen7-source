#ifndef __GFL_G3DFOG_H__
#define __GFL_G3DFOG_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dFog.h
 *	@brief  3D�t�H�O
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#if 0
/*[�Q�ƃe�[�u�����g�p�����t�H�O�̐���]

�Q�ƃe�[�u���̒l�͎��̂悤�Ɏg����B

���� : 0���t�H�O�̊J�n�f�v�X�̏ꏊ�ɑ�������
       1���t�H�O�̏I���f�v�X�̏ꏊ�ɑ�������
       (NW4C���ԃt�@�C���ł͔͈͂�0<= <=1�łȂ�0<= <=255�ɂȂ��Ă���)

�c�� : 0�̂Ƃ��t�H�O�J���[���S��������Ȃ�
       1�̂Ƃ����S�Ƀt�H�O�J���[�ɂȂ�

�� :

1 |       /
  |  ----
  | /
  +--------
0         1

�J�n�f�v�X�̏ꏊ�ł́A�t�H�O�J���[���S��������Ȃ�
�J�n�f�v�X�ƏI���f�v�X�̒��Ԃ̏ꏊ�ł́A�t�H�O�J���[�������������Ă���
�I���f�v�X�̏ꏊ�ł́A���S�Ƀt�H�O�J���[�ɂȂ�*/
#endif


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dSceneNode.h>
#include <grp/g3d/gfl_G3dResource.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3D�t�H�O�N���X
//=====================================
class Fog : public SceneNode
{
  GFL_FORBID_COPY_AND_ASSIGN(Fog);


public:
  //-------------------------------------
  ///	�t�H�O�e�[�u���\����
  //=====================================
  static const u32 FOG_TABLE_VALUE_NUM = 128;  // �t�H�O�e�[�u���̗v�f��
  struct FogTable
  {
    f32 value[FOG_TABLE_VALUE_NUM];
        // index : 0���t�H�O�̊J�n�f�v�X�̏ꏊ�ɑ�������
        //         FOG_TABLE_VALUE_NUM-1���t�H�O�̏I���f�v�X�̏ꏊ�ɑ�������
        // value[index] : 0�̂Ƃ��t�H�O�J���[���S��������Ȃ�
        //                1�̂Ƃ����S�Ƀt�H�O�J���[�ɂȂ�
  };
  static void InitializeFogTableForDefault(
      FogTable*    fog_table
  );
  static void SetupFogTableUsingLookupTable(  // ���̊֐�����߂��Ă�����Ăяo������lookup_table_resource���폜���Ă��悢
      FogTable*    fog_table,                 // �Ăяo������fog_table�̃������͊m�ۂ��Ă�������
      Resource*    lookup_table_resource,
      s32          lookup_table_set_index_in_resource,
      s32          lookup_table_index_in_lookup_table_set
  );


public:
  // �萔
  enum
  {
    LOOKUP_TABLE_INDEX_NONE    // ���g�p�ŃC���f�b�N�X���ݒ肳��Ă��Ȃ��ꍇ�̒l
  };
  // �֐�
  enum Function
  {
    FUNCTION_LINEAR             = nw::gfx::res::ResFogUpdater::FOG_UPDATER_TYPE_LINEAR,           // ���`�֐�
    FUNCTION_EXPONENT           = nw::gfx::res::ResFogUpdater::FOG_UPDATER_TYPE_EXPONENT,         // �w���֐�
    FUNCTION_EXPONENT_SQUARE    = nw::gfx::res::ResFogUpdater::FOG_UPDATER_TYPE_EXPONENT_SQUARE   // ���̎w���֐�
  };


public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   *                 Create�͌Ă΂�܂���B
   */
  //-----------------------------------------------------------------------------
  Fog(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   *                 Destroy���Ăт܂��B
   */
  //-----------------------------------------------------------------------------
  virtual ~Fog();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   *
   *  @param[in]     allocator          �A���P�[�^
   *  @param[in]     device_allocator   �f�o�C�X�A���P�[�^
   *  @param[in]     resource           ���\�[�X
   *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*           allocator,
      gfl::heap::NwAllocator*           device_allocator,
      Resource*                         resource,
      s32                               index_in_resource
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �Q�ƃe�[�u�����g�p�����t�H�O�̐���
   *
   *  @param[in]     allocator          �A���P�[�^
   *  @param[in]     device_allocator   �f�o�C�X�A���P�[�^
   *  @param[in]     resource           ���\�[�X
   *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
   *  @param[in]     lookup_table_resource                   �Q�ƃe�[�u���̃��\�[�X
   *  @param[in]     lookup_table_set_index_in_resource      �Q�ƃe�[�u���Z�b�g�̃��\�[�X���ɂ�����C���f�b�N�X
   *  @param[in]     lookup_table_index_in_lookup_table_set  �Q�ƃe�[�u���̎Q�ƃe�[�u���Z�b�g���ɂ�����C���f�b�N�X
   */
  //-----------------------------------------------------------------------------
  virtual void CreateFogUsingLookupTable(
      gfl::heap::NwAllocator*  allocator,
      gfl::heap::NwAllocator*  device_allocator,
      Resource*                resource,
      s32                      index_in_resource,
      Resource*                lookup_table_resource,
      s32                      lookup_table_set_index_in_resource,
      s32                      lookup_table_index_in_lookup_table_set
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �t�H�O�e�[�u�����g�p�����t�H�O�̐���
   *
   *  @param[in]     allocator          �A���P�[�^
   *  @param[in]     device_allocator   �f�o�C�X�A���P�[�^
   *  @param[in]     resource           ���\�[�X
   *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
   *  @param[in]     fog_table          �t�H�O�e�[�u��(���e���R�s�[�������̂�ێ�����̂ŁA���̊֐�����߂��Ă�����A�Ăяo������fog_table���폜���Ă��悢)
   */
  //-----------------------------------------------------------------------------
  virtual void CreateFogUsingFogTable(
      gfl::heap::NwAllocator*  allocator,
      gfl::heap::NwAllocator*  device_allocator,
      Resource*                resource,
      s32                      index_in_resource,
      const FogTable&          fog_table
  );
  virtual void CreateFogUsingFogTable(
      gfl::heap::NwAllocator*  allocator,
      gfl::heap::NwAllocator*  device_allocator,
      Resource*                resource,
      s32                      index_in_resource
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �j��
   *                 ����Ă�ł����v�B�f�X�g���N�^�͌Ă΂�܂���B 
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �J���[
   *                 u8�̂Ƃ���0����&�����A255����&�s����
   *                 f32�̂Ƃ���0.0f����&�����A1.0f����&�s����
   */
  //-----------------------------------------------------------------------------
  void SetColor(f32 r, f32 g, f32 b);
  void SetColor(const gfl::grp::ColorF32& c);  // a�͂ǂ��g���邩�s��
  void SetColor(u8 r, u8 g, u8 b);
  void SetColor(const gfl::grp::ColorU8& c);   // a�͂ǂ��g���邩�s��
  gfl::grp::ColorF32 GetColor(void) const;     // a�͂ǂ��g���邩�s��
  void GetColor(gfl::grp::ColorF32* c) const;  // a�͂ǂ��g���邩�s��
  void GetColor(gfl::grp::ColorU8*  c) const;  // a�͂ǂ��g���邩�s��
 
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   */
  //-----------------------------------------------------------------------------
  void     SetFunction(Function function);
  Function GetFunction(void) const;
  void SetDensity(f32 value);
  f32  GetDensity(void) const;
  void SetMinDepth(f32 min);
  f32  GetMinDepth(void) const;
  void SetMaxDepth(f32 max);
  f32  GetMaxDepth(void) const;
  void SetDepth(f32 min, f32 max);
  void SetIntensity(const f32 intensity);  // m_type��TYPE_CREATE_FOG_TABLE��TYPE_USE_LOOKUP_TABLE_RESOURCE�̂Ƃ��̂ݗL���B  // 0.0f�őS���t�H�O���o�Ȃ��A1.0f�Ńe�[�u���̒ʂ�Ƀt�H�O���o��(0.0f<= <=1.0f)
  f32  GetIntensity(void) const;  // m_type��TYPE_CREATE_FOG_TABLE��TYPE_USE_LOOKUP_TABLE_RESOURCE�̂Ƃ��̂ݗL���B                         
  void SetFogTable(
      const FogTable&          fog_table  // �t�H�O�e�[�u��(���e���R�s�[�������̂�ێ�����̂ŁA���̊֐�����߂��Ă�����A�Ăяo������fog_table���폜���Ă��悢)
  );  // m_type��TYPE_CREATE_FOG_TABLE�̂Ƃ��̂ݗL���B


  //-----------------------------------------------------------------------------
  /**
   *  @brief         �L���t���O
   */
  //-----------------------------------------------------------------------------
  void SetEnable(bool enable);
  b32 IsEnable(void) const;
  // @note �t�H�O�𖳌��ɂ���Ƃ���nw::gfx::res::ResFogUpdater::FOG_UPDATER_TYPE_NONE���g�������Ǝv�������A
  //       ������g�p�����NW4C�̃A�T�[�gNW_ASSERTMSG(false, "Unknown FogUpdater type.");�Ɉ����|�����Ă��܂��B
  //       ������A�Ǝ��̕��@�Ŗ����ɂ���悤�ɂ���(���m�ɂ̓t�H�O���L�������|����Ȃ��悤�ɂ��Ă���)�B


//protected:
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   */
  //-----------------------------------------------------------------------------
  nw::gfx::Fog*          GetNwFog(void) const            {  return nw::ut::DynamicCast<nw::gfx::Fog*>( GetNwSceneNode() );  }
  nw::gfx::ResFog        GetNwResFog(void) const         {  return GetNwFog()->GetResFog();  }
  nw::gfx::ResFogUpdater GetNwResFogUpdater(void) const  {  return GetNwResFog().GetFogUpdater();  }
  void                   Update(Camera* camera);

  void                   UpdateUsingLookupTable(Camera* camera);
  void                   UpdateUsingFogTable(Camera* camera);
  void                   updateUsingFogTable(Camera* camera, const FogTable& fog_table );


private:
  enum Type
  {
    TYPE_NONE,                      // �܂��t�H�O�𐶐����Ă��Ȃ�
    TYPE_NORMAL,                    // ���ʂ�NW4C�̋@�\�������g�����t�H�O
    TYPE_CREATE_FOG_TABLE,          // �t�H�O�e�[�u��FogTable������g���Ă���t�H�O
    TYPE_USE_LOOKUP_TABLE_RESOURCE  // �Q�ƃe�[�u���̃��\�[�X���g�����t�H�O
  };
  static const f32 DISABLE_MAX_DEPTH = 100000000.0f;
  static const f32 DISABLE_MIN_DEPTH = DISABLE_MAX_DEPTH - 0.01f;

private:
  b32  m_enable_flag;
  f32  m_enable_min_depth;
  f32  m_enable_max_depth;

  Type       m_type;
  FogTable*  m_fog_table;

  Resource*  m_lookup_table_resource;
  s32        m_lookup_table_set_index_in_resource;
  s32        m_lookup_table_index_in_lookup_table_set;

  f32        m_intensity;    // m_type��TYPE_CREATE_FOG_TABLE��TYPE_USE_LOOKUP_TABLE_RESOURCE�̂Ƃ��̂ݗL���B  // 0.0f�őS���t�H�O���o�Ȃ��A1.0f�Ńe�[�u���̒ʂ�Ƀt�H�O���o��(0.0f<= <=1.0f)
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DFOG_H__
