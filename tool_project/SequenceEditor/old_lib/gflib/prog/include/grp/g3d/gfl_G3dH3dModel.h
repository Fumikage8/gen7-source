#ifndef __GFL_G3DH3DMODEL_H__
#define __GFL_G3DH3DMODEL_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dModel.h
 *	@brief  H3D���f��
 *	@author	Koji Kawada
 *	@date		2012.04.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#if 0
/*����
����H3dModel�ł�
nw::h3d::res::ModelContent
��e��X�e�[�g�������܂��B*/
#endif


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
#include <grp/g3d/gfl_G3dH3dResource.h>
#include <grp/g3d/gfl_G3dH3dEnvironment.h>
#include <grp/g3d/gfl_G3dH3dCommand.h>
#include <grp/g3d/gfl_G3dH3dResTexture.h>
#include <grp/g3d/gfl_G3dH3dDynamicTexture.h>
#include <grp/g3d/gfl_G3dH3dResShader.h>
#include <grp/g3d/gfl_G3dH3dAnimationManager.h>
#include <grp/g3d/gfl_G3dH3dEx.h>

#include "gfl_G3dH3dModelMemoryManager.h"




namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

// �O���錾
class Scene;
class H3dUserData;
class H3dModel;

extern bool G3DH3DMODEL_DEBUG_PRINT;


//-------------------------------------
///	H3D���b�V���֘A�̃C�x���g���ώ@���郊�X�i�[�N���X
//=====================================
class H3dMeshListener
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dMeshListener);

  friend class H3dModel;


public:
  // @brief �R���X�g���N�^
  H3dMeshListener(void)
    : next(NULL)
  {}
  // @brief �f�X�g���N�^
  virtual ~H3dMeshListener(){}

public:
  // @brief �C�x���g���N�����Ƃ��ɌĂ΂��֐�
  // @param[in]  model           ���b�V�����������Ă��郂�f��
  // @param[in]  material_index  ���b�V���Ɋ��蓖�Ă��Ă���}�e���A���̃C���f�b�N�X
  // @param[in]  command         �R�}���h
  // @param[in]  env             ��
  virtual void Invoke(H3dModel* model, s32 material_index, H3dCommand* command, H3dEnvironment* env){}


  ///////////////////////////////////////////////////////////////////////////////
  //
  // friend�ɂ͌��J���Ă������J�����o
  //
  ///////////////////////////////////////////////////////////////////////////////
private:
  H3dMeshListener* next;  // ���̃��X�i�[�̃|�C���^
};




//-------------------------------------
///	H3D���f���N���X
//=====================================
class H3dModel : public Object
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dModel);


  friend class H3dAnimationSlot;
  friend class Scene;
  friend class RenderSystem;
  friend struct RenderSceneInternalFunctor;
  friend struct RenderStereoSceneInternalFunctor;


public:
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �萔
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  enum
  {
    INDEX_NONE          = -1,  // �C���f�b�N�X�����t����Ȃ��Ƃ�
    INDEX_IGNORE        = -2,  // �������Ă����{�[���Ƃ����Ӗ��Ŏg���C���f�b�N�X�ԍ�

    CONSTANT_COLOR_NUM  = 6    // �R���X�^���g�J���[�̌�
  };

  // �e�N�X�`���R�[�f�B�l�[�^  �}�b�s���O���@
  enum TextureMappingMethod
  {
    TEXTURE_MAPPING_METHOD_UV_COORDINATE     = nw::h3d::TextureMappingType::UV_COORDINATE,  // �e�N�X�`�����W�ł��B 
    TEXTURE_MAPPING_METHOD_CAMERA_CUBE_ENV   = nw::h3d::TextureMappingType::CUBE_ENV,       // �J�����L���[�u���W�ł��B 
    TEXTURE_MAPPING_METHOD_CAMERA_SPHERE_ENV = nw::h3d::TextureMappingType::SPHERE_ENV,     // �J�����X�t�B�A���W�ł��B 
    TEXTURE_MAPPING_METHOD_PROJECTION        = nw::h3d::TextureMappingType::PROJECTION,     // ���e���܂��B 
    TEXTURE_MAPPING_METHOD_SHADOW            = nw::h3d::TextureMappingType::SHADOW,         // �V���h�E�p�̓��e�ł��B 
    TEXTURE_MAPPING_METHOD_SHADOW_CUBE       = nw::h3d::TextureMappingType::SHADOW_CUBE,    // �V���h�E�L���[�u�p�̓��e�ł��B
  };

  // �}�e���A���̕`�惌�C���[
  enum MaterialTranslucencyKind
  {
    MATERIAL_TRANSLUCENCY_KIND_LAYER0       = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_LAYER0,       // ���C���[ 0 ( �s�������b�V���Ƃ��ĕ`�� ) �ł��B 
    MATERIAL_TRANSLUCENCY_KIND_LAYER1       = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_LAYER1,       // ���C���[ 2 ( ���������b�V���Ƃ��ĕ`�� ) �ł��B 
    MATERIAL_TRANSLUCENCY_KIND_LAYER2       = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_LAYER2,       // ���C���[ 3 ( ���Z�������b�V���Ƃ��ĕ`�� ) �ł� 
    MATERIAL_TRANSLUCENCY_KIND_LAYER3       = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_LAYER3,       // ���C���[ 4 ( ���Z�������b�V���Ƃ��ĕ`�� ) �ł� 
    MATERIAL_TRANSLUCENCY_KIND_OPAQUE       = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_OPAQUE,       // TRANSLUCENCY_KIND_LAYER0 �Ɠ����ł��B 
    MATERIAL_TRANSLUCENCY_KIND_TRANSLUCENT  = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_TRANSLUCENT,  // TRANSLUCENCY_KIND_LAYER1 �Ɠ����ł��B 
    MATERIAL_TRANSLUCENCY_KIND_SUBTRACTIVE  = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_SUBTRACTIVE,  // TRANSLUCENCY_KIND_LAYER2 �Ɠ����ł��B 
    MATERIAL_TRANSLUCENCY_KIND_ADDITIVE     = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_ADDITIVE,     // TRANSLUCENCY_KIND_LAYER3 �Ɠ����ł��B 
    MATERIAL_TRANSLUCENCY_KIND_END          = nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_END           // �Ō�̕`�惌�C���[�ł��B                       
  };

  // �f�X�N���v�V����
  struct Description
  {
    b32 delete_memory_control;  ///< true ���ƁA���f���j�����̃������j�����P�t���[���x��Ď��s����B �}�e���A���̃L���b�V�����������R�}���h�o�b�t�@�ɐς܂�Ă��邽�߁B
    s32 maxFragmentLightNum;    ///< �V�[���Ŏg�p���郉�C�g�̍ő吔
    
    Description(void);
  };


public:
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �R���X�g���N�^ / �f�X�g���N�^
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   */
  //-----------------------------------------------------------------------------
  H3dModel(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  virtual ~H3dModel();



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // ���� / �j��
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   */
  //-----------------------------------------------------------------------------
  virtual b32 Create(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      const H3dResource* resource,
      const char* name,
      const Description* description = NULL
  );
  virtual b32 Create(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      const H3dResource* resource,
      s32 index_in_resource,
      const Description* description = NULL
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �j��
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �ݒ� / �擾
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
private:  // friend�ɂ͌��J���Ă������J�����o
  //-----------------------------------------------------------------------------
  /**
   *  @brief         Scene�ւ̓o�^���ς�����Ƃ��ɌĂсA�o�^���Ă���Scene��ݒ肷��
   *
   *  @param[in]     scene  �o�^���Ă���Scene�BScene����O�����Ƃ���NULL��n���B
   */
  //-----------------------------------------------------------------------------
  inline void SetScene(Scene* scene)
  {
    m_scene = scene;
  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �o�^���Ă���Scene���擾����
   *
   *  @retval        �o�^���Ă���Scene�BScene����O��Ă���Ƃ���NULL���Ԃ�B
   */
  //-----------------------------------------------------------------------------
  inline Scene* GetScene(void) const
  {
    return m_scene;
  }



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �v�Z
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �v�Z����
   */
  //-----------------------------------------------------------------------------
 
#if 0
/*������͍l������gfx�ƍ���Ȃ�

  ///////////////////////////
  // �S������
  ///////////////////////////
protected:
  virtual void Calculate(void);
private:
  // �A�j���[�V�����}�l�[�W���v�Z
  void calculateAnimationManager(void);

  ////////////////////////////////////////////////////////////////////////////////
  // �y���Ӂz�S������ƌʂ�1�t���[���ɂǂ��炩�����g��Ȃ��悤�ɂ��ĉ������B
  ////////////////////////////////////////////////////////////////////////////////
  
  ///////////////////////////
  // ��
  ///////////////////////////
protected:
  // �Ǝ��X�V
  virtual void CalculateBeforeCulling(void);
  virtual void CalculateAfterCulling(void);

  // ���[���h�}�g���N�X�X�V
  virtual void CalculateWorldMatrix(void);
  // ���[�J���X�P���^���A�j���[�V�����X�V
  virtual void CalculateLocalSkeletalAnimation(void);
  // �r�W�r���e�B�A�j���[�V�����X�V
  virtual void CalculateVisibilityAnimation(void);
  // �}�e���A���A�j���[�V�����X�V
  virtual void CalculateMaterialAnimation(void);
  // �A�j���[�V�����t���[���X�V
  virtual void CalculateAnimationFrame(void);
  // ���[���h�X�P���^���A�j���[�V�����X�V
  virtual void CalculateWorldSkeletalAnimation(void);
*/
#endif


  
  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  // �h���N���X������֐��Ag3d�ł������֐�
  // gfl::grp::g3d::Model::Calculate�ɑ�������֐�
  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
protected:
  virtual void Calculate(void);
  ///////////////////////////////////////////////////


  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  // �h���N���X������Ȃ��AScene���X�V����֐��Agfx������ɂ���Ă����֐�
  // nw::gfx�ɂ����ăV�[���c���[�ɓo�^���Ă��������̂��A�b�v�f�[�g���邱�Ƃɑ�������֐�
  // CalculateScene���S���ЂƂ܂Ƃߊ֐��ŁA����ȊO������̏������֐��B
  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
private:
  virtual void CalculateScene(void);
  
  // �Ǝ��X�V
  virtual void CalculateBeforeCulling(void);
  virtual void CalculateAfterCulling(void);

// GFBTS[2579] TreeFlag��False�̎��ɁA�A�j���[�V�����̃t���[�����i�܂Ȃ������C��
#ifdef BUGFIX_GFBTS2579_20130723
protected:
#endif

  // ���[���h�}�g���N�X�X�V
  virtual void CalculateWorldMatrix(void);
  // ���[�J���X�P���^���A�j���[�V�����X�V
  virtual void CalculateLocalSkeletalAnimation(void);
  // �r�W�r���e�B�A�j���[�V�����X�V
  virtual void CalculateVisibilityAnimation(void);
  // �}�e���A���A�j���[�V�����X�V
  virtual void CalculateMaterialAnimation(void);
  // �A�j���[�V�����t���[���X�V
  virtual void CalculateAnimationFrame(void);
  // ���[���h�X�P���^���A�j���[�V�����X�V
  virtual void CalculateWorldSkeletalAnimation(void);
  ///////////////////////////////////////////////////


  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  // ���̃N���X�����ł����g�p����֐�
  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
private:
  // �A�j���[�V�����}�l�[�W���v�Z
  void calculateAnimationManager(void);
  ///////////////////////////////////////////////////



private:  // friend�ɂ͌��J���Ă������J�����o
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // SubmitView
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         SubmitView
   */
  //-----------------------------------------------------------------------------
  void SubmitView(
    nw::gfx::RenderQueue::ElementListType* list,
    const nw::gfx::Camera& camera
  );



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �`��
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
private:  // friend�ɂ͌��J���Ă������J�����o
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �s�����`��
   */
  //-----------------------------------------------------------------------------
  /*void DrawOpa(
      H3dCommand* command,
      H3dEnvironment* env,
      const gfl::math::MTX34* view_mtx,
      bool fragmentAndWBufferDirty
  );*/

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �������`��
   */
  //-----------------------------------------------------------------------------
  /*void DrawXlu(
      H3dCommand* command,
      H3dEnvironment* env,
      const gfl::math::MTX34* view_mtx,
      bool fragmentAndWBufferDirty
  );*/ 

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���b�V���`��
   */
  //-----------------------------------------------------------------------------
#if 0
  void DrawMesh(
      const nw::h3d::res::MeshData& mesh,
      bool fragmentAndWBufferDirty
  );
#endif

  void DrawMesh(
      H3dCommand* command,
      H3dEnvironment* env,
      const nw::h3d::res::MeshData& mesh,
      bool fragmentAndWBufferDirty
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
      ,
      const gfl::math::MTX34&  view_mtx,
      const gfl::math::MTX34&  inv_view_mtx,
      const u32                inv_exist
#endif
  );



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �`�揇
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �`�揇
   *                 �������̎��(�`�惌�C���[)
   *                 �f�U�C�i���G�f�B�^��Őݒ�ł���}�e���A���̕`�惌�C���[
   *                 �ŏ��ɕ`�� MATERIAL_TRANSLUCENCY_KIND_LAYER0 <= MaterialTranslucencyKind <= MATERIAL_TRANSLUCENCY_KIND_LAYER3 �Ō�ɕ`��
   *                 �ŗD�悳���`�揇 ModelLayerId > MaterialTranslucencyKind > ���̑� �ŗ�コ���`�揇
   */
  //-----------------------------------------------------------------------------
  MaterialTranslucencyKind GetMaterialTranslucencyKind(s32 material_index) const;
  void SetMaterialTranslucencyKind(s32 material_index, MaterialTranslucencyKind kind);
private:
  void initializeMaterialTranslucencyKind(void);

public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �`�揇
   *                 �v���O���}�������ݒ�ł��郂�f���̃��C���[ID
   *                 �ŏ��ɕ`�� 0 <= u8 <= 127 �Ō�ɕ`��
   *                 �ŗD�悳���`�揇 ModelLayerId > MaterialTranslucencyKind > ���̑� �ŗ�コ���`�揇
   *
   *                 **** Model��Particle�ɂ���S�������ɕ��ׂ�����ԍ��ł��B�f�t�H���g0000000�ł��B ****
   */
  //-----------------------------------------------------------------------------
  void SetModelLayerId(u8 model_layer_id);
  u8 GetModelLayerId(void) const;



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // ���[�U�f�[�^
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  /////////////////////////
  // ���f��
  /////////////////////////
  s32 GetModelUserDataCount() const;
  // �y���Ӂz���[�U�[�f�[�^�͐擪�� $ (�_���[) ��t���������O�̃��^�f�[�^�Ƃ��ĕێ�����܂��B
  //         $�����Y��Ȃ��悤�ɁB
  s32 GetModelUserDataIndex(const char* name) const;
  void GetModelUserData(const s32 user_data_index, H3dUserData* user_data) const;
  
  bool GetModelAABB( gfl::math::AABB *p_dest_aabb ) const;
  
  /////////////////////////
  // �}�e���A��
  /////////////////////////
  s32 GetMaterialUserDataCount(const s32 material_index) const;
  // �y���Ӂz���[�U�[�f�[�^�͐擪�� $ (�_���[) ��t���������O�̃��^�f�[�^�Ƃ��ĕێ�����܂��B
  //         $�����Y��Ȃ��悤�ɁB
  s32 GetMaterialUserDataIndex(const s32 material_index, const char* name) const;
  void GetMaterialUserData(const s32 material_index, const s32 user_data_index, H3dUserData* user_data) const;
  
  /////////////////////////
  // ���b�V��
  /////////////////////////
  s32 GetMeshUserDataCount(const s32 mesh_index) const;
  // �y���Ӂz���[�U�[�f�[�^�͐擪�� $ (�_���[) ��t���������O�̃��^�f�[�^�Ƃ��ĕێ�����܂��B
  //         $�����Y��Ȃ��悤�ɁB
  s32 GetMeshUserDataIndex(const s32 mesh_index, const char* name) const;
  void GetMeshUserData(const s32 mesh_index, const s32 user_data_index, H3dUserData* user_data) const;

  u32 GetMaterialProjData( const s32 material_index );
  void SetMaterialProjData( const s32 material_index, const u32 value );


private:
  nw::h3d::fnd::PatriciaMap<nw::h3d::res::MetaData>* getModelMetaDataMap(void) const;
  nw::h3d::fnd::PatriciaMap<nw::h3d::res::MetaData>* getMaterialMetaDataMap(const s32 material_index) const;
  nw::h3d::fnd::PatriciaMap<nw::h3d::res::MetaData>* getMeshMetaDataMap(const s32 mesh_index) const;



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // ���W�ϊ�
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  // �X�P�[��
  void SetScale(f32 x, f32 y, f32 z);
  void SetScale(const gfl::math::VEC3& t);
  void GetScale(gfl::math::VEC3* t) const;

  // ��]
  void SetRotateXYZ(const gfl::math::VEC3& t);  // [radian]

  void GetRotateXYZ(gfl::math::VEC3* t) const;  // [radian]  // �y���ӁzSetRotateXYZ�œn���ꂽ�l��Ԃ������ł��BSetRotateXYZ�ȊO�ŉ�]���w�肵�Ă����ꍇ��
                                                             //         �����ڂƍ���Ȃ��l���Ԃ��Ă��܂��B
 
  template <typename TMatrix> void SetRotateMatrix(const TMatrix& m)
  {
    m_rotate_mtx.f._00 = m.f._00;
    m_rotate_mtx.f._01 = m.f._01;
    m_rotate_mtx.f._02 = m.f._02;
    m_rotate_mtx.f._03 = 0;  // @note m_translate_vec���������ɓ���������
    m_rotate_mtx.f._10 = m.f._10;
    m_rotate_mtx.f._11 = m.f._11;
    m_rotate_mtx.f._12 = m.f._12;
    m_rotate_mtx.f._13 = 0;
    m_rotate_mtx.f._20 = m.f._20;
    m_rotate_mtx.f._21 = m.f._21;
    m_rotate_mtx.f._22 = m.f._22;
    m_rotate_mtx.f._23 = 0;
    
    SetBitFlagOff(&m_bit_flag, BIT_FLAG_ENABLE_ROTATE_VEC);
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

  // ���s�ړ�
  void SetTranslate(const gfl::math::VEC3& t);
  void GetTranslate(gfl::math::VEC3* t) const;

  virtual bool GetWorldMatrix(gfl::math::MTX34* matrix) const
  {
    *matrix = m_world_mtx;
    return true;
  }
  
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �t���O
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���c���[�t���O
   *                 true�̂Ƃ�Scene��Add����Ă�����V�[���c���[�ɓ������B
   *                 false�̂Ƃ�Scene��Add����Ă��Ă��V�[���c���[����O�����B
   *                 ���ꂪtrue�ł��邱�ƂƁA���[�U��Scene��Add�������Ƃ́A�����ł͂���܂���B
   *                 false�ŃV�[���c���[����O����Ă���Ƃ��́A�v�Z���`�������Ȃ��B
   */
  //-----------------------------------------------------------------------------
  virtual void SetTreeFlag(b32 flag);
  virtual b32 GetTreeFlag(void) const;
 
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �`��t���O
   *                 true�̂Ƃ��`�悳���B
   *                 ���������ꂪtrue�̂Ƃ��ł��AGetTreeFlag��false�̂Ƃ��͕`�悳��Ȃ��B
   *                 �V�[���c���[�ɓ�����Ă��Ă��ꂪfalse�̂Ƃ��́A�v�Z�͂���邪�`��͂���Ȃ��B
   */
  //-----------------------------------------------------------------------------
  virtual void SetVisible(b32 visible);
  virtual b32  IsVisible(void) const;



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �}�e���A��
  //
  // u8�̂Ƃ���0����&�����A255����&�s����
  // f32�̂Ƃ���0.0f����&�����A1.0f����&�s����
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  ///////////////////////////////////////////
  ///////////////////////////////////////////
  //
  // �}�e���A���S��
  //
  ///////////////////////////////////////////
  ///////////////////////////////////////////
  s32 GetMaterialCount(void) const;
  s32 GetMaterialIndex(const char* name) const;
  const char* GetMaterialName(s32 material_index) const;

  // �}�e���A���A�j���[�V�����ȂǂŕύX�t���O�����������̂��f�t�H���g�̗����Ă��Ȃ���Ԃɖ߂��B
  void ResetMaterialState(s32 material_index);
  void ResetAllMaterialState(void);

  // �}�e���A���̃e�N�X�`��0,1,2��UV��SRT�̕ύX�t���O�����Z�b�g����
  void ResetMaterialStateForTextureCoordinatorUV(s32 material_index);
  void ResetAllMaterialStateForTextureCoordinatorUV(void);



  ///////////////////////////////////////////
  ///////////////////////////////////////////
  //
  // �J���[
  //
  ///////////////////////////////////////////
  ///////////////////////////////////////////
  // �A���r�G���g�J���[
  void SetMaterialColorAmbient(s32 material_index, const gfl::grp::ColorF32& c);
  void GetMaterialColorAmbient(s32 material_index, gfl::grp::ColorF32* c) const;

  // �f�B�t���[�Y�J���[
  void SetMaterialColorDiffuse(s32 material_index, const gfl::grp::ColorF32& c);
  void GetMaterialColorDiffuse(s32 material_index, gfl::grp::ColorF32* c) const;
  
  // �X�y�L�����J���[
  void GetMaterialColorSpecular0(s32 material_index, gfl::grp::ColorF32* c) const;
  void GetMaterialColorSpecular1(s32 material_index, gfl::grp::ColorF32* c) const;

  // �R���X�^���g�J���[
  // no�̓f�U�C�i�[����c�[���̃R���X�^���g�J���["�ԍ�"
  virtual void SetMaterialColorConstant(s32 material_index, s32 no, const gfl::grp::ColorF32& c);
  virtual void GetMaterialColorConstant(s32 material_index, s32 no, gfl::grp::ColorF32* c) const;


  ///////////////////////////////////////////
  ///////////////////////////////////////////
  //
  // �e�N�X�`���}�b�p�[
  //
  ///////////////////////////////////////////
  ///////////////////////////////////////////
  // no  �e�N�X�`���}�b�p�[�ԍ�(0,1,2,Proc)(�e�N�X�`���R�[�f�B�l�[�^�ԍ��ł͂Ȃ�)
  // �e�N�X�`����ݒ肷��
  void SetMaterialTextureMapperTexture(s32 material_index, s32 no, const H3dResTexture& res_texture);
  void SetMaterialTextureMapperTexture(s32 material_index, s32 no, const H3dDynamicTexture& dynamic_texture);
  void SetMaterialTextureMapperTexture(s32 material_index, s32 no, const H3dModel& src_model, s32 src_material_index, s32 src_no);

  // @brief  �e�N�X�`���̐ݒ�����ɖ߂�
  // @attention VRAM�ɔz�u����Delete����Section(DeleteSomeSection�Ȃ�)���Ă���ƌ��ɖ߂��Ȃ���������܂���B
  void ResetMaterialTextureMapperTexture(s32 material_index, s32 no);
  
  //////////////////////
  // �ݒ� / �擾
  //////////////////////
  // @retval �e�N�X�`�����\���Ă���Ƃ�true�A�\���ĂȂ��Ƃ�false
  b32 IsMaterialTextureMapperTextureValid(s32 material_index, s32 no) const;
  
  // @attention VRAM�ɔz�u����Delete����Section(DeleteSomeSection�Ȃ�)���Ă���ƒl���擾�ł��܂���B
  void* GetMaterialTextureMapperTexturePhysicalAddress(s32 material_index, s32 no) const;
  
  // @attention VRAM�ɔz�u����Delete����Section(DeleteSomeSection�Ȃ�)���Ă���ƒl���擾�ł��܂���B
  u16   GetMaterialTextureMapperTextureWidth(s32 material_index, s32 no) const;
  
  // @attention VRAM�ɔz�u����Delete����Section(DeleteSomeSection�Ȃ�)���Ă���ƒl���擾�ł��܂���B
  u16   GetMaterialTextureMapperTextureHeight(s32 material_index, s32 no) const;
  
  gfl::grp::g3d::TextureFormat GeMaterialTextureMapperTexturetForamat(s32 material_index, s32 no) const;
  
  u8    GetMaterialTextureMapperTextureMipmap(s32 material_index, s32 no) const;

  // @brief  �ŏ��ɓ\���Ă������e�N�X�`���̖��O�𓾂�
  const char* GetMaterialTextureMapperTextureOriginalName(s32 material_index, s32 no) const;
  // @brief  �ŏ��ɓ\���Ă������e�N�X�`���̖��O��^���A���ꂪ�ǂ̃}�e���A���̉��Ԃ̃e�N�X�`�����������𓾂�
  // @param[out]  material_index  �}�e���A���ԍ������ĕԂ�
  // @param[out]  no              �e�N�X�`���}�b�p�[�ԍ������ĕԂ�
  // @retval     true�̂Ƃ��ŏ��ɓ\���Ă������e�N�X�`���̒���name�Ƃ������O�������̂�����B
  //             true�̂Ƃ���material_index��no�ɗL���Ȓl�����ĕԂ��Bfalse�̂Ƃ��͕s��B
  b32 GetMaterialTextureMapperTextureOriginalIndex(s32* material_index, s32* no, const char* name) const;

private:
  void initializeModelTextureInfo(s32 material_index, s32 no);
  void* getModelTexturePhysicalAddress(s32 material_index, s32 no) const;
  u16   getModelTextureWidth(s32 material_index, s32 no) const;
  u16   getModelTextureHeight(s32 material_index, s32 no) const;
  gfl::grp::g3d::TextureFormat getModelTextureFormat(s32 material_index, s32 no) const;
  u8    getModelTextureMipmap(s32 material_index, s32 no) const;

  // TEXTURE_CLASS_TYPE_???�𓾂�
  s32 getMaterialTextureClassType(s32 material_index, s32 no) const;

  // SetMaterialTextureMapperTexture��H3dModel���g���łɓn�������𓾂�֐�
  const nw::h3d::res::TextureContent* getMaterialTextureMapperTextureAsModelTexture(s32 material_index, s32 no) const;
  const H3dResTexture*                getMaterialTextureMapperTextureAsResTexture(s32 material_index, s32 no) const;
  const H3dDynamicTexture*            getMaterialTextureMapperTextureAsDynamicTexture(s32 material_index, s32 no) const;

public:
  // no  �e�N�X�`���}�b�p�[�ԍ�(0,1,2,Proc)(�e�N�X�`���R�[�f�B�l�[�^�ԍ��ł͂Ȃ�)
  // U�����J��Ԃ��ݒ���s���B
  void SetMaterialTextureMapperWrapU(s32 material_index, s32 no, H3dTextureSamplerExConst::Wrap wrap);
  // U�����J��Ԃ��ݒ�𓾂�B
  H3dTextureSamplerExConst::Wrap  GetMaterialTextureMapperWrapU(s32 material_index, s32 no) const;
  // V�����J��Ԃ��ݒ���s���B
  void SetMaterialTextureMapperWrapV(s32 material_index, s32 no, H3dTextureSamplerExConst::Wrap wrap);
  // V�����J��Ԃ��ݒ�𓾂�B
  H3dTextureSamplerExConst::Wrap  GetMaterialTextureMapperWrapV(s32 material_index, s32 no) const;
  // �e�N�X�`���̍��W�́AST�͎g�킸�AUV�œ��ꂵ�Ă����B

  // no  �e�N�X�`���}�b�p�[�ԍ�(0,1,2,Proc)(�e�N�X�`���R�[�f�B�l�[�^�ԍ��ł͂Ȃ�)
  // �e�N�X�`���T���v���[�^�C�v
  void SetMaterialTextureMapperSamplerType(s32 material_index, s32 no, H3dTextureSamplerExConst::SamplerType sampler_type);
  H3dTextureSamplerExConst::SamplerType GetMaterialTextureMapperSamplerType(s32 material_index, s32 no) const;
  //res_tex_sampler.SetSamplerType( nw::gfx::ResTextureSampler::SAMPLERTYPE_PROJECTION );  // �e�N�X�`���T���v���[�^�C�v��ݒ肵�܂��B �e�N�X�`���T���v���[�^�C�v���ݒ�\�Ȃ̂͂O�Ԗڂ̃e�N�X�`���}�b�p�[�݂̂ł��B �P�A�Q�Ԗڂɐݒ肷��Ɛ��������삵�܂���B

  // no  �e�N�X�`���}�b�p�[�ԍ�(0,1,2,Proc)(�e�N�X�`���R�[�f�B�l�[�^�ԍ��ł͂Ȃ�)
  // �g�厞�̕�ԏ���
  void SetMaterialTextureMapperMagFilter(s32 material_index, s32 no, H3dTextureSamplerExConst::MagFilter mag_filter);
  H3dTextureSamplerExConst::MagFilter GetMaterialTextureMapperMagFilter(s32 material_index, s32 no) const;
  // �k�����̕�ԏ���
  void SetMaterialTextureMapperMinFilter(s32 material_index, s32 no, H3dTextureSamplerExConst::MinFilter min_filter);
  H3dTextureSamplerExConst::MinFilter GetMaterialTextureMapperMinFilter(s32 material_index, s32 no) const;
  
  // �~�b�v�}�b�v�ݒ�
  void SetTextureMipMapLodMinLevel( s32 material_index, s32 no, s32 level );
  void SetTextureMipMapLodBias( s32 material_index, s32 no, s32 bias );

  // no  �e�N�X�`���}�b�p�[�ԍ�(0,1,2,Proc)(�e�N�X�`���R�[�f�B�l�[�^�ԍ��ł͂Ȃ�)
  // �{�[�_�[�J���[
  void SetMaterialTextureMapperBorderColor(s32 material_index, s32 no, const gfl::grp::ColorF32& c);
  void GetMaterialTextureMapperBorderColor(s32 material_index, s32 no, gfl::grp::ColorF32* c) const;

  
  ///////////////////////////////////////////
  ///////////////////////////////////////////
  //
  // �e�N�X�`���R�[�f�B�l�[�^
  //
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  // material_index�ԃ}�e���A����no�ԃe�N�X�`���R�[�f�B�l�[�^(0<=no<=2)�̐ݒ�
  // texture_mapper_no�ԃe�N�X�`���}�b�p�[��texture_coordinator_no�ԃe�N�X�`���R�[�f�B�l�[�^���������ꍇ�A
  // �u2�ԃe�N�X�`���}�b�p�[�Ȃ�2�ԃe�N�X�`���R�[�f�B�l�[�^���g���v�Ƃ����ӂ��ɕK�������Ȃ��Ă���킯�ł͂Ȃ��B
  // �u1�ԃe�N�X�`���}�b�p�[��2�ԃe�N�X�`���}�b�p�[��1�ԃe�N�X�`���R�[�f�B�l�[�^�����L���Ďg���v�Ƃ������Ƃ����蓾��B
  // �܂�texture_mapper_no=texture_coordinator_no�ƕK���Ȃ�킯�ł͂Ȃ��A�Ƃ������Ƃɒ��ӂ��ĉ������B

  // �e�N�X�`�����W
  void SetMaterialTextureCoordinatorScaleUV(s32 material_index, s32 no, const gfl::math::VEC2& t);
  void GetMaterialTextureCoordinatorScaleUV(s32 material_index, s32 no, gfl::math::VEC2* t) const;
  
  void SetMaterialTextureCoordinatorRotateUV(s32 material_index, s32 no, const f32 t);  // [radian]
  f32 GetMaterialTextureCoordinatorRotateUV(s32 material_index, s32 no) const;  // [radian]
  
  void SetMaterialTextureCoordinatorTranslateUV(s32 material_index, s32 no, const gfl::math::VEC2& t);
  void GetMaterialTextureCoordinatorTranslateUV(s32 material_index, s32 no, gfl::math::VEC2* t) const;

  void SetMaterialTextureCoordinatorReferenceSceneCameraIndex(s32 material_index, s32 no, s32 scene_camera_index);
  s32  GetMaterialTextureCoordinatorReferenceSceneCameraIndex(s32 material_index, s32 no) const;

  // �}�b�s���O���@
  void SetMaterialTextureCoordinatorMappingMethod(s32 material_index, s32 no, TextureMappingMethod mapping_method);
  TextureMappingMethod GetMaterialTextureCoordinatorMappingMethod(s32 material_index, s32 no) const;

private:
  nw::h3d::fnd::TextureCoord& getMaterialTextureCoordinator(s32 material_index, s32 no);
  const nw::h3d::fnd::TextureCoord& getMaterialTextureCoordinator(s32 material_index, s32 no) const;
  void enableMaterialTextureCoordinatorFlag(s32 material_index, s32 no);








  ///////////////////////////////////////////
  ///////////////////////////////////////////
  // 
  // �e�N�X�`���R���o�C�i
  // 
  ///////////////////////////////////////////
  ///////////////////////////////////////////
public: 
  // step�i��
  void SetMaterialTextureCombinerCombineRgb(s32 material_index, s32 step, H3dTextureCombinerExConst::Combine combine);
  H3dTextureCombinerExConst::Combine  GetMaterialTextureCombinerCombineRgb(s32 material_index, s32 step);

  // step�i��
  void SetMaterialTextureCombinerCombineAlpha(s32 material_index, s32 step, H3dTextureCombinerExConst::Combine combine);
  H3dTextureCombinerExConst::Combine  GetMaterialTextureCombinerCombineAlpha(s32 material_index, s32 step);



  // step�i��
  void SetMaterialTextureCombinerScaleRgb(s32 material_index, s32 step, H3dTextureCombinerExConst::Scale scale);
  H3dTextureCombinerExConst::Scale  GetMaterialTextureCombinerScaleRgb(s32 material_index, s32 step);
  
  // step�i��
  void SetMaterialTextureCombinerScaleAlpha(s32 material_index, s32 step, H3dTextureCombinerExConst::Scale scale);
  H3dTextureCombinerExConst::Scale  GetMaterialTextureCombinerScaleAlpha(s32 material_index, s32 step);



  // step�i��
  // no �\�[�Xno  A:�\�[�X0  B:�\�[�X1  C:�\�[�X2
  void SetMaterialTextureCombinerOperandRgb(s32 material_index, s32 step, s32 no, H3dTextureCombinerExConst::OperandRgb ope);
  H3dTextureCombinerExConst::OperandRgb  GetMaterialTextureCombinerOperandRgb(s32 material_index, s32 step, s32 no);
  
  // step�i��
  // no �\�[�Xno  A:�\�[�X0  B:�\�[�X1  C:�\�[�X2
  void SetMaterialTextureCombinerOperandAlpha(s32 material_index, s32 step, s32 no, H3dTextureCombinerExConst::OperandAlpha ope);
  H3dTextureCombinerExConst::OperandAlpha  GetMaterialTextureCombinerOperandAlpha(s32 material_index, s32 step, s32 no);



  // step�i��
  // no �\�[�Xno  A:�\�[�X0  B:�\�[�X1  C:�\�[�X2
  void SetMaterialTextureCombinerSourceRgb(s32 material_index, s32 step, s32 no, H3dTextureCombinerExConst::Source source);
  H3dTextureCombinerExConst::Source  GetMaterialTextureCombinerSourceRgb(s32 material_index, s32 step, s32 no);
  
  // step�i��
  // no �\�[�Xno  A:�\�[�X0  B:�\�[�X1  C:�\�[�X2
  void SetMaterialTextureCombinerSourceAlpha(s32 material_index, s32 step, s32 no, H3dTextureCombinerExConst::Source source);
  H3dTextureCombinerExConst::Source  GetMaterialTextureCombinerSourceAlpha(s32 material_index, s32 step, s32 no);


  // step�i��
  // no�̓f�U�C�i�[����c�[���̃R���X�^���g�J���["�ԍ�"
  // �f�U�C�i�[����c�[���̃R���X�^���g�J���["�ԍ�"��ݒ肷��
  void SetMaterialTextureCombinerConstant(s32 material_index, s32 step, s32 no);
  // �f�U�C�i�[����c�[���̃R���X�^���g�J���["�ԍ�"���擾����
  s32 GetMaterialTextureCombinerConstant(s32 material_index, s32 step) const;


  ///////////////////////////////////////////
  ///////////////////////////////////////////
  // 
  // �t���O�����g�V�F�[�_
  // 
  ///////////////////////////////////////////
  ///////////////////////////////////////////
  
  // step�i�ڂ͑O�i�̉����g����(step�i�ڂ͌�i�ɉ���n�����ł͂Ȃ��̂Œ��ӂ��邱��)
  void SetMaterialFragmentShaderBufferInputRgb(s32 material_index, s32 step, H3dFragmentShaderExConst::BufferInput input);
  H3dFragmentShaderExConst::BufferInput  GetMaterialFragmentShaderBufferInputRgb(s32 material_index, s32 step);

  // step�i�ڂ͑O�i�̉����g����(step�i�ڂ͌�i�ɉ���n�����ł͂Ȃ��̂Œ��ӂ��邱��)
  void SetMaterialFragmentShaderBufferInputAlpha(s32 material_index, s32 step, H3dFragmentShaderExConst::BufferInput input);
  H3dFragmentShaderExConst::BufferInput  GetMaterialFragmentShaderBufferInputAlpha(s32 material_index, s32 step);


  ///////////////////////////////////////////
  ///////////////////////////////////////////
  // 
  // �V�F�[�_(���_�V�F�[�_���W�I���g���V�F�[�_)
  // 
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  // �V�F�[�_��ݒ肷��
  void SetMaterialShader(s32 material_index, const H3dResShader& res_shader);
  // �V�F�[�_�̐ݒ�����ɖ߂�
  void ResetMaterialShader(s32 material_index);


  ///////////////////////////////////////////
  ///////////////////////////////////////////
  // 
  // �X�e���V���e�X�g
  // 
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  // StencilOperation
  b32 IsMaterialStencilTestEnable(s32 material_index) const;
  void SetMaterialStencilTestEnable(s32 material_index, b32 enable);
  H3dStencilOperationExConst::TestFunc GetMaterialStencilTestFunction(s32 material_index) const;
  void SetMaterialStencilTestFunction(s32 material_index, H3dStencilOperationExConst::TestFunc func_func);

  void SetMaterialStencilTestReference(s32 material_index, u8 ref);
  u8 GetMaterialStencilTestReference(s32 material_index) const;

  // @note GetMaterialStencilTestMask��SetMaterialStencilTestMask�́AMask��FuncMask�ԈႦ�Ă��Ȃ������M�Ȃ��ł��B
  //       �I�[�T�����O�c�[���̃X�e���V���̃}�X�N�͎g���Ă��Ȃ��炵���̂ŁA�ǂ��炪����ɊY������̂����؂��Ă��܂���B
  //       ��ID�G�b�W�Ō��؂����̂Ŏg���đ��v�ł��B
  u8 GetMaterialStencilTestMask(s32 material_index) const;  // @note Mask��FuncMask�ԈႦ�Ă��Ȃ������M�Ȃ��H
  void SetMaterialStencilTestMask(s32 material_index, u8 mask);  // @note Mask��FuncMask�ԈႦ�Ă��Ȃ������M�Ȃ��H

  H3dStencilOperationExConst::StencilOp GetMaterialStencilTestFailOperation(s32 material_index) const;
  void SetMaterialStencilTestFailOperation(s32 material_index, H3dStencilOperationExConst::StencilOp op_fail);
  H3dStencilOperationExConst::StencilOp GetMaterialStencilTestZFailOperation(s32 material_index) const;
  void SetMaterialStencilTestZFailOperation(s32 material_index, H3dStencilOperationExConst::StencilOp op_zfail);
  H3dStencilOperationExConst::StencilOp GetMaterialStencilTestPassOperation(s32 material_index) const;
  void SetMaterialStencilTestPassOperation(s32 material_index, H3dStencilOperationExConst::StencilOp op_zpass);


  ///////////////////////////////////////////
  ///////////////////////////////////////////
  // 
  // �A���t�@�e�X�g
  // 
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  // AlphaTest
  b32 IsMaterialAlphaTestEnable(s32 material_index) const;
  void SetMaterialAlphaTestEnable(s32 material_index, b32 enable);  // true�̂Ƃ��A���t�@�e�X�g�L��
  H3dAlphaTestExConst::TestFunc GetMaterialAlphaTestFunction(s32 material_index) const;
  void SetMaterialAlphaTestFunction(s32 material_index, H3dAlphaTestExConst::TestFunc func);

  void SetMaterialAlphaTestReference(s32 material_index, f32 ref);
  f32 GetMaterialAlphaTestReference(s32 material_index) const;

  void SetMaterialAlphaTestReferenceU8(s32 material_index, u8 ref);
  u8 GetMaterialAlphaTestReferenceU8(s32 material_index) const;


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �f�v�X�e�X�g
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  // DepthTest
  b32 IsMaterialDepthTestEnable(s32 material_index) const;
  void SetMaterialDepthTestEnable(s32 material_index, b32 enable);  // true�̂Ƃ��f�v�X�e�X�g�L��
  b32 IsMaterialDepthTestMaskEnable(s32 material_index) const;
  void SetMaterialDepthTestMaskEnable(s32 material_index, b32 enable);  // true�̂Ƃ��f�v�X�}�X�N�L��(true�̂Ƃ��f�v�X�o�b�t�@���X�V����)
  H3dDepthTestExConst::TestFunc GetMaterialDepthTestFunction(s32 material_index) const;
  void SetMaterialDepthTestFunction(s32 material_index, H3dDepthTestExConst::TestFunc func);


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �u�����_�[
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  
  // @attention �y���Ӂz�u�����h���[�h�ɂ���Đݒ�ł����肵�Ȃ������肷��悤�ɐ��䂵�Ă���̂ŁA�܂��ŏ��Ƀu�����h���[�h��ύX���ĉ������B
  void SetMaterialBlendMode(s32 material_index, H3dBlendExConst::Mode mode);
  H3dBlendExConst::Mode GetMaterialBlendMode(s32 material_index) const;

  H3dBlendExConst::LogicOperation GetMaterialBlendLogicOperation(s32 material_index) const;
  void SetMaterialBlendLogicOperation(s32 material_index, H3dBlendExConst::LogicOperation operation);

  H3dBlendExConst::Factor GetMaterialBlendFunctionSourceRgb(s32 material_index) const;
  void SetMaterialBlendFunctionSourceRgb(s32 material_index, H3dBlendExConst::Factor factor);
  H3dBlendExConst::Factor GetMaterialBlendFunctionSourceAlpha(s32 material_index) const;
  void SetMaterialBlendFunctionSourceAlpha(s32 material_index, H3dBlendExConst::Factor factor);

  H3dBlendExConst::Factor GetMaterialBlendFunctionDestinationRgb(s32 material_index) const;
  void SetMaterialBlendFunctionDestinationRgb(s32 material_index, H3dBlendExConst::Factor factor);
  H3dBlendExConst::Factor GetMaterialBlendFunctionDestinationAlpha(s32 material_index) const;
  void SetMaterialBlendFunctionDestinationAlpha(s32 material_index, H3dBlendExConst::Factor factor);

  H3dBlendExConst::Equation GetMaterialBlendEquationRgb(s32 material_index) const;
  void SetMaterialBlendEquationRgb(s32 material_index, H3dBlendExConst::Equation equation);
  H3dBlendExConst::Equation GetMaterialBlendEquationAlpha(s32 material_index) const;
  void SetMaterialBlendEquationAlpha(s32 material_index, H3dBlendExConst::Equation equation);


  ///////////////////////////////////////////
  ///////////////////////////////////////////
  // 
  // ���X�^���C�[�[�V����
  // 
  ///////////////////////////////////////////
  ///////////////////////////////////////////

  // �J�����O���[�h(CreativeStudio�̃}�e���A���S�ʂŐݒ肷��|���S���̕\���`��ݒ�̂��Ƃł�)
  void SetMaterialRasterizationCullingMode(s32 material_index, H3dRasterizationExConst::CullingMode cull);
  H3dRasterizationExConst::CullingMode  GetMaterialRasterizationCullingMode(s32 material_index) const;


  ///////////////////////////////////////////
  ///////////////////////////////////////////
  //
  // �}�e���A�����e�����󂯂�t�H�O
  //
  ///////////////////////////////////////////
  ///////////////////////////////////////////
  void SetMaterialFogEnable(s32 material_index, b32 enable);
  b32 IsMaterialFogEnable(s32 material_index) const;
  void SetMaterialFogIndex(s32 material_index, s32 index);
  s32  GetMaterialFogIndex(s32 material_index) const;


  ///////////////////////////////////////////
  ///////////////////////////////////////////
  //
  // �}�e���A�����e�����󂯂郉�C�g�Z�b�g
  //
  ///////////////////////////////////////////
  ///////////////////////////////////////////
  void SetMaterialLightSetIndex(s32 material_index, s32 index);
  s32  GetMaterialLightSetIndex(s32 material_index) const;


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �{�[���ƃ{�[���}�g���N�X
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  virtual s32 GetBonesCount(void) const;
  virtual s32 GetBonesIndex(const char* key) const;
  virtual const char* GetBonesName(s32 bone_index) const;
private:
  // �g�p�֎~
  //virtual s32 GetBonesCountOfWorldMatrixPose(void) const;  // GetBonesCount�Ɠ����l���Ԃ��Ă���Ǝv�����A�����łȂ��Ƃ��͂���̂��낤���H
public:
  virtual b32 GetBoneMatrixOfWorldMatrixPose(s32 bone_index, gfl::math::MTX34* matrix) const;
  virtual gfl::math::MTX34* GetBoneMatrixPointerOfWorldMatrixPose(s32 bone_index);
  virtual const gfl::math::MTX34* GetBoneMatrixPointerOfWorldMatrixPose(s32 bone_index) const;
  //virtual gfl::math::MTX34& GetBoneMatrixReferenceOfWorldMatrixPose(s32 bone_index);
  virtual b32 SetBoneMatrixOfWorldMatrixPose(s32 bone_index, const gfl::math::MTX34& matrix);

public:
  // ���[�J���}�g���N�X(�e�X�g���Ă��Ȃ��ł�)(�X�P�[�����~�����Ƃ��͕ʓr���k���ĉ�����)
  virtual b32 GetBoneMatrixOfLocalMatrixPose(s32 bone_index, gfl::math::MTX34* matrix) const;
  virtual gfl::math::MTX34* GetBoneMatrixPointerOfLocalMatrixPose(s32 bone_index);  // ����Ŏ擾�����}�g���N�X��ύX�����Ƃ��́A�ύX�̓x��SetBoneDirtyFlagOfLocalMatrixPose(s32 bone_index)���Ă�ŉ������B
  virtual void SetBoneDirtyFlagOfLocalMatrixPose(s32 bone_index);
  virtual const gfl::math::MTX34* GetBoneMatrixPointerOfLocalMatrixPose(s32 bone_index) const;
  virtual b32 SetBoneMatrixOfLocalMatrixPose(s32 bone_index, const gfl::math::MTX34& matrix);


  




  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �{�[�������ւ�
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  void CreateReplaceBoneData(gfl::heap::NwAllocator* heap_allocator, H3dModel* base_model);
  void DestroyReplaceBoneData(void);
  void SetWorldReplaceBone(H3dModel* base_model);

  void IgnoreParentReplaceBone(const char* name); 
  void IgnoreChildReplaceBone(const char* name); 
  void IgnoreSelfChildReplaceBone(const char* name);
private:
  // @brief  self_index�{�[���̐�c��ancestor_index�{�[��������ꍇtrue��Ԃ��A�Ȃ��ꍇfalse��Ԃ��Bancestor_index��self_index�������Ƃ���false��Ԃ��B
  b32  isAncestorBone(s32 ancestor_index, s32 self_index);

  //-----------------------------------------------------------------------------------------------------------------------------
  // xy_project��CBoneEffectModel�N���X�Ŏg�p(�����ł��邱�Ƃ��ŗD��Ƃ��č���Ă��邽�߃G���[�`�F�b�N�͑S���s���Ă��Ȃ�)
  //-----------------------------------------------------------------------------------------------------------------------------
public:
  inline int GetBoneNumberBEM() const { return m_skel_state.GetBoneStateSize(); } ///< �������擾����
  inline const gfl::math::MTX34& GetWorldMatrixBEM() const { return m_cont->worldMtx; } ///< ���[���h�}�g���N�X���擾����
  inline int GetBoneIndexByNameBEM(const char* sName) const { return m_cont->GetBoneIdx(sName); } ///< ���̖��O����C���f�b�N�X���擾����
  inline const char* GetBoneNameByIndexBEM(const int nIndex) const { return m_cont->bones[nIndex].name; } ///< ���̃C���f�b�N�X���疼�O���擾����
  inline const gfl::math::MTX34& GetBoneMatrixByIndexBEM(const int nIndex) const { return m_skel_state.GetWorldMtx(nIndex); } ///< ���̃C���f�b�N�X����}�g���N�X���擾����
  inline int GetParentBoneIndexByIndexBEM(const int nIndex) const { return m_cont->bones[nIndex].parentIdx; } ///< �C���f�b�N�X����e���̃C���f�b�N�X���擾����
  inline void SetBoneMatrixByIndexNotTransmitBEM(const int nIndex, const gfl::math::MTX34& inMat) { gfl::math::MTX34Copy(&(m_skel_state.GetWorldMtx(nIndex)), &inMat); } ///< �C���f�b�N�X�ɑΉ����鍜�Ƀ}�g���N�X��ݒ肷��(�q���ւ̓`���͍s���Ȃ�)

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // ���b�V��
  // (���b�V���m�[�h�Ƃ͈Ⴄ�̂Œ���)
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  virtual s32 GetMeshCount(void) const;
  virtual s32 GetMeshIndex(const char* name) const;  // ���b�V���̖��O�����Ȃ��̂łȂ�
  virtual const char* GetMeshName(s32 mesh_index) const;  // ���b�V���̖��O�����Ȃ��̂łȂ�

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���b�V�����\������Ă��邩
   *  @param[in]     mesh_index  ���b�V���̃C���f�b�N�X
   *  @retval        ���b�V�����\������Ă���Ƃ�true
   */
  //-----------------------------------------------------------------------------
  virtual b32 IsMeshVisible(const s32 mesh_index) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���b�V���̕\��/��\����؂�ւ���
   *
   *                 ���b�V���m�[�h�r�W�r���e�B�A�j���[�V�����������Ă���t���O�Ɠ����t���O�𑀍삷��̂ŁA
   *                 �A�j���[�V�����ŕ\��/��\����؂�ւ��Ă���ꍇ�͒��ӂ��ĉ������B
   *
   *  @param[in]     mesh_index  ���b�V���̃C���f�b�N�X
   *  @param[in]     visible     ���b�V����\������Ƃ�true
   */
  //-----------------------------------------------------------------------------
  virtual void SetMeshVisible(const s32 mesh_index, const b32 visible);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���b�V���̃}�e���A���𓾂�
   *
   *  @param[in]     mesh_index  ���b�V���̃C���f�b�N�X
   *  @retval        �}�e���A���C���f�b�N�X
   */
  //-----------------------------------------------------------------------------
  virtual s32 GetMaterialIndexOfMesh(const s32 mesh_index) const;



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // ���b�V���m�[�h
  // (���b�V���Ƃ͈Ⴄ�̂Œ���)
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  virtual s32 GetMeshNodeCount(void) const;
  virtual s32 GetMeshNodeDicCount(void) const;  // ���b�V���m�[�h�̖��O�e�[�u���̗v�f���BGetMeshNodeCount�̖߂�l�Ƃ͈قȂ邱�Ƃ�����̂Œ��ӁB
  virtual s32 GetMeshNodeIndex(const char* name) const;  // ���b�V���m�[�h�̖��O�͎���
  virtual const char* GetMeshNodeName(s32 mesh_node_index) const;  // ���b�V���m�[�h�̖��O�͎���
  virtual b32 IsMeshNodeVisible(const s32 mesh_node_index) const;
  virtual void SetMeshNodeVisible(const s32 mesh_node_index, const b32 visible);



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �V�F�[�_�V���{��
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  b32 CreateShaderExState(
      gfl::heap::NwAllocator* heap_allocator, 
      gfl::heap::NwAllocator* device_allocator, 
      s32 material_index,
      const H3dShaderExState::Description* desc
  );

  // value�͂��̊֐�����߂�����Ăяo�����͔j�����Ă��悢
  void SetShaderSymbolVSFloat(s32 material_index, const char* name, const gfl::math::VEC4& value);  // �Ȃ�ׂ�index�ł��g���ĉ������B
  void SetShaderSymbolVSFloat(s32 material_index, s32 index, const gfl::math::VEC4& value);  // CreateShaderExState��Description�œn����shader_symbol_vs_float_name_list�̃C���f�b�N�X

  void GetShaderSymbolVSFloat(s32 material_index, const char* name, gfl::math::VEC4* value) const;  // �Ȃ�ׂ�index�ł��g���ĉ������B
  void GetShaderSymbolVSFloat(s32 material_index, s32 index, gfl::math::VEC4* value) const;  // CreateShaderExState��Description�œn����shader_symbol_vs_float_name_list�̃C���f�b�N�X
  
  s32 GetShaderSymbolIndex(s32 material_index, const char* name) const;
  
  void SetVSBool(s32 material_index, s32 boolIndex, b32 value );

  // �m�F�p
  void DumpShaderSymbolName(s32 material_index) const;



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �A�j���[�V����
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����}�l�[�W������
   */
  //-----------------------------------------------------------------------------
  b32 CreateAnimationManagerAndAnimationSlot(gfl::heap::NwAllocator* heap_allocator, s32 slot_num, const H3dAnimationSlot::Description* slot_desc_array);
  b32 CreateAnimationManager(gfl::heap::NwAllocator* heap_allocator, s32 slot_num);
  virtual b32 CreateAnimationSlot(gfl::heap::NwAllocator* heap_allocator, s32 slot_index, const H3dAnimationSlot::Description* slot_desc);
  virtual b32 IsAnimationSlotCreated(s32 slot_index) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����}�l�[�W���j��
   */
  //-----------------------------------------------------------------------------
  void DestroyAnimationManager(void);
  void DestroyAnimationSlot(s32 slot_index);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����ύX
   */
  //-----------------------------------------------------------------------------
  virtual H3dAnimationSlot::ChangeAnimationResult ChangeAnimationByResAnim(s32 slot_index, H3dResAnim* res_anim);
  virtual H3dAnimationSlot::ChangeAnimationResult ChangeAnimationByResAnimListIndex(s32 slot_index, s32 list_index);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���ݒ肳��Ă���A�j���[�V�����𓾂�
   *  @param[in]     slot_index  �X���b�g
   *  @retval        �A�j���[�V�������ݒ肳��Ă��Ȃ��Ƃ���NULL��Ԃ�
   *                 H3dResAnim�łȂ��A�j���[�V�������ݒ肳��Ă���Ƃ���NULL��Ԃ�
   */
  //-----------------------------------------------------------------------------
  virtual H3dResAnim* GetAnimationAsResAnim(s32 slot_index) const;
  // @note �����A�j���[�V�����N���X��ResAnim�ȊO�ɂ���������A
  // ���̊֐��ɂ���ăA�j���̗L����������Ă���Ƃ���ɉe�����o��B
  // ������A�A�j���[�V�����N���X�𑝂₵����A�g���Ă���ӏ����悭�m�F���邱�ƁB

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̃t���[����ݒ肷��
   *  @param[in]     frame           �t���[��
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationFrame(f32 frame, s32 slot_index = 0);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̃t���[�����擾����
   *  @retval        �t���[��
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationFrame(s32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̍X�V�t���[����ݒ肷��
   *                 0.0f�Œ�~�A1.0f�œ��{�A2.0f��2�{���A-1.0f�ŋt���{�Đ�
   *
   *  @param[in]     step_frame      �X�V�t���[��
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationStepFrame(f32 step_frame, s32 slot_index = 0);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̍X�V�t���[�����擾����
   *  @retval        �X�V�t���[��
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationStepFrame(s32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̊J�n�t���[����ݒ肷��
   *  @param[in]     frame           �t���[��
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationStartFrame(f32 start_frame, s32 slot_index = 0);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̊J�n�t���[�����擾����
   *  @retval        �t���[��
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationStartFrame(s32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̏I���t���[����ݒ肷��
   *  @param[in]     frame           �t���[��
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationEndFrame(f32 end_frame, s32 slot_index = 0);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̏I���t���[�����擾����
   *  @retval        �t���[��
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationEndFrame(s32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̊J�n�t���[���ƏI���t���[����ݒ肷��
   *  @param[in]     start_frame     �J�n�t���[��
   *  @param[in]     end_frame       �I���t���[��
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationStartAndEndFrame(f32 start_frame, f32 end_frame, s32 slot_index = 0);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̌��݂̃t���[�����J�n�t���[�����ۂ����肷��
   *  @retval        bool    ���݂̃t���[�����J�n�t���[���̏ꍇture��Ԃ�
   */
  //-----------------------------------------------------------------------------
  virtual b32 IsAnimationFrameStartFrame(s32 slot_index = 0) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̌��݂̃t���[�����I���t���[�����ۂ����肷��
   *  @retval        bool    ���݂̃t���[�����I���t���[���̏ꍇture��Ԃ�
   */
  //-----------------------------------------------------------------------------
  virtual b32 IsAnimationFrameEndFrame(s32 slot_index = 0) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̌��݂̃t���[����
   *                     step_frame>0�̂Ƃ��͏I���t���[���̂Ƃ�true��Ԃ�
   *                     step_frame<0�̂Ƃ��͊J�n�t���[���̂Ƃ�true��Ԃ�
   *                     step_frame==0�̂Ƃ��͊J�n�t���[�����I���t���[���̂Ƃ�true��Ԃ��A����ȊO�̂Ƃ�false��Ԃ�
   *
   *  @retval        bool    ���݂̃t���[����step_frame���猩�Ē[�t���[���ƂȂ��Ă���ꍇture��Ԃ�
   */
  //-----------------------------------------------------------------------------
  virtual b32 IsAnimationFrameStepTerminalFrame(s32 slot_index = 0) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V���������[�v�Đ����ۂ��擾����
   *                 ��slot_index�ɐݒ肳��Ă���H3dResAnim�̃��[�v��Ԃ��擾���邾���ł��B
   *  @retval        bool            ���[�v�̂Ƃ�true�A���[�v�łȂ��Ƃ�false��Ԃ�
   */
  //-----------------------------------------------------------------------------
  virtual b32 IsAnimationLoop(s32 slot_index = 0) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����Ƀ��[�v�Đ���ݒ肷��
   *                 ��slot_index�ɐݒ肳��Ă���H3dResAnim�̃��[�v��Ԃ�ύX���܂��B
   *  @param[in]     loop            ���[�v�̂Ƃ�true�A���[�v�łȂ��Ƃ�false��ݒ肷��
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationLoop(b32 loop, s32 slot_index = 0);



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �R�[���o�b�N
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  // @brief   ���b�V���`��J�n���̃C�x���g���ώ@���郊�X�i�[��ǉ�����
  // @retval  �ǉ��ł����Ƃ�true��Ԃ�
  b32 AddPreMeshDrawListener(H3dMeshListener* listener);
  // @brief  ���b�V���`��J�n���̃C�x���g���ώ@���郊�X�i�[����菜��
  void RemovePreMeshDrawListener(H3dMeshListener* listener);
  
  // @brief   ���b�V���`��I�����̃C�x���g���ώ@���郊�X�i�[��ǉ�����
  // @retval  �ǉ��ł����Ƃ�true��Ԃ�
  b32 AddPostMeshDrawListener(H3dMeshListener* listener);
  // @brief  ���b�V���`��I�����̃C�x���g���ώ@���郊�X�i�[����菜��
  void RemovePostMeshDrawListener(H3dMeshListener* listener);

private:
  b32 addMeshListener(H3dMeshListener*& head, H3dMeshListener* listener);
  void removeMeshListener(H3dMeshListener*& head, H3dMeshListener* listener);
  void callMeshListenerInvoke(H3dMeshListener*& head, s32 material_index, H3dCommand* command, H3dEnvironment* env);



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �R�}���h���s
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  // @brief  �}�e���A���R�}���h�𔭍s����
  //         �R�}���h�o�b�t�@�W�����v�ł͂Ȃ����ۂɔ��s����
  //         �R�}���h�o�b�t�@�W�����v�ł͗p�ӂ��Ă��Ȃ�
  void MakeMaterialCommand(
      H3dCommand* command,
      H3dEnvironment* env,
      s32 material_index,
      bool fragmentAndWBufferDirty = false
  );
  
  //----------------------------------------------------------------------
  /**
   * @brief MaterialCommand�̃L���b�V���쐬
   * @param device_allocator
   */
  //----------------------------------------------------------------------
  void MakeMaterialCommandCache(gfl::heap::NwAllocator* device_allocator);
  
  //----------------------------------------------------------------------
  /**
   * @brief MaterialCommand�̃L���b�V������B�����I�ɍs���ꍇ�́A�P�t���x�����ɋC��t���āB
   */
  //----------------------------------------------------------------------
  void ReleaseMaterialCommandCache();

private:
  // @brief  �V�F�[�_�R�}���h�𔭍s����
  void makeShaderCommand(
      H3dCommand* command,
      H3dEnvironment* env,
      s32 material_index
  );


public:
  const nw::h3d::res::ModelContent* GetNwModelContent(void) const
  {
    return m_cont;
  }  
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // friend�ɂ͌��J���Ă������J�����o
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
private:
  
  nw::h3d::SkeletonState* GetNwSkeletonState(void) //������const��t����Ɖ��̂��R���p�C���G���[�ɂȂ�
  {
    return &m_skel_state;
  }
  nw::h3d::MaterialStateArray* GetNwMaterialStateArray(void) //������const��t����Ɖ��̂��R���p�C���G���[�ɂȂ�
  {
    return &m_mate_state_array;
  }
  nw::h3d::VisibilityState* GetNwVisibilityState(void) //������const��t����Ɖ��̂��R���p�C���G���[�ɂȂ�
  {
    return &m_visi_state;
  }



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // friend�ɂ����J���Ă��Ȃ�����J�����o
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
private:
  //-----------------------------------------------------------------------------
  /// @brief �����o������
  //-----------------------------------------------------------------------------
  void initParam();
  
  //-----------------------------------------------------------------------------
  /// @brief ���b�V���`��
  //-----------------------------------------------------------------------------
  void drawMesh(
      H3dCommand* command,
      H3dEnvironment* env,
      const nw::h3d::res::MeshData& mesh,
      const nn::math::MTX34& normal
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
      ,
      const gfl::math::MTX34&  view_mtx,
      const gfl::math::MTX34&  inv_view_mtx,
      const u32                inv_exist
#endif
  );

  // �V�F�[�_�[�V���{���̃o�C���h��ݒ肷��
  void setShaderBind(s32 material_index, const nw::h3d::res::ShaderContent* shader_content);

  // ���W�ϊ��v�Z
  void calculateTransform(void);

  // �X�P���^�����f���̃��b�V����Z�[�x�f�v�X�����߂�
  f32 calculateDepthOfSkeletalModel(
    const nw::h3d::res::MeshData& mesh,
    const nw::gfx::Camera&        camera
  );
  // (�X�P���^�����f���łȂ�)���f���̃��b�V����Z�[�x�f�v�X�����߂�
  f32 calculateDepth(
    const nw::h3d::res::MeshData& mesh,
    const nw::gfx::Camera&        camera
  );

  // �����_�[�L���[�Ƀ��f���̃��b�V����ς�
  void enqueue(
    nw::gfx::RenderQueue::ElementListType* list,
    const nw::gfx::Camera& camera
  );

  //! @brief �t�H�O�̃R�}���h�𐶐����܂��B
  //!
  //! ���ݐݒ肳��Ă���t�H�O����ύX���Ȃ��ꍇ�́A��������K�v�͂���܂���B
  //!
  //! �쐬�����R�}���h�̃t�H�O ID �l��Ԃ��܂��B
  u16 makeFogCommand(
      nn::gr::CTR::CommandBufferJumpHelper& dcs,
      const nw::h3d::MaterialState& materialState,
      const nw::h3d::SceneState& sceneState,
      u16 currentFogId,
      const H3dEnvironment& env) const;
  
  void SetFragmentAndWBufferDirty( b32 f ){ m_FragmentAndWBufferDirty = f; }

private:
  // �V�F�[�_�[�V���{���̃o�C���h
  struct ShaderBind
  {
    nn::gr::BindSymbolVSFloat worldMtx;
    nn::gr::BindSymbolVSFloat normalMtx;
    nn::gr::BindSymbolVSFloat projMtx;
    nn::gr::BindSymbolVSFloat viewMtx;
    nn::gr::BindSymbolVSFloat univMtx;
    nn::gr::BindSymbolVSInteger lightCt;//@fix ���삳��v�]�F�o�g���w�i�̃��C�e�B���O�Ή�

    // ���_���C�e�B���O
    nn::gr::BindSymbolVSFloat hslGCol;
    nn::gr::BindSymbolVSFloat hslSCol;
    nn::gr::BindSymbolVSFloat hslSDir;

    nn::gr::BindSymbolVSFloat matAmbi;
    nn::gr::BindSymbolVSFloat matDiff;
  };
  typedef nw::h3d::fnd::DataArray<ShaderBind> ShaderBindArray;

  // �r�b�g�t���O
  enum
  {
    BIT_FLAG_NONE              = 0,
    BIT_FLAG_VISIBLE           = 1 <<  0,
    BIT_FLAG_TREE_FLAG         = 1 <<  1,
    BIT_FLAG_ENABLE_ROTATE_VEC = 1 <<  2,
    BIT_FLAG_MAKE_DRAW_CASH    = 1 <<  3,       ///< �`�掞�ɃL���b�V���𐶐�
    BIT_FLAG_DEL_MEM_CONTROL   = 1 <<  4,       ///< �j�����Ƀ������}�l�[�W���Ƀ�������o�^���邩

    BIT_FLAG_DEFAULT           = BIT_FLAG_VISIBLE | BIT_FLAG_TREE_FLAG | BIT_FLAG_ENABLE_ROTATE_VEC
  };

  // �e�N�X�`�����
  enum
  {
    TEXTURE_CLASS_TYPE_NONE     = 0,  // �e�N�X�`���Ȃ�
    TEXTURE_CLASS_TYPE_MODEL    = 1,  // ���f�����������Ƃ��ɓ����Ă����e�N�X�`��  // ���f�����������Ƃ��ɓ����Ă����Ƃ������Ƃ�Model�Ƃ��Ă���(ModelTexture)
    TEXTURE_CLASS_TYPE_RES      = 2,  // H3dResTexture
    TEXTURE_CLASS_TYPE_DYNAMIC  = 3,  // H3dDynamicTexture

    TEXTURE_CLASS_TYPE_SHIFT  = 8  // 8�r�b�g���V�t�g����
  };
  struct TextureInfo
  {
    u32   texture_class_type;   // TEXTURE_CLASS_TYPE_SHIFT����TEXTURE_CLASS_TYPE_???�����Ă����B|��|2|1|0|
    const void* ptr[H3dTextureSamplerExConst::TEXTURE_NUM];
        // TEXTURE_CLASS_TYPE_NONE�̂Ƃ�NULL
        // TEXTURE_CLASS_TYPE_MODEL�̂Ƃ�nw::h3d::res::TextureContent�ւ̃|�C���^
        // TEXTURE_CLASS_TYPE_RES�̂Ƃ�H3dResTexture�C���X�^���X�ւ̃|�C���^
        // TEXTURE_CLASS_TYPE_DYNAMIC�̂Ƃ�H3dDynamicTexture�C���X�^���X�ւ̃|�C���^

    TextureInfo(void)
      : texture_class_type(0)
    {
      for(s32 no=0; no<H3dTextureSamplerExConst::TEXTURE_NUM; ++no)
      {
        ptr[no] = NULL;
      }
    }
    // no  �e�N�X�`���}�b�p�[�ԍ�(0,1,2,Proc)(�e�N�X�`���R�[�f�B�l�[�^�ԍ��ł͂Ȃ�)
    void SetTextureClassTypeAndPtr(s32 no, s32 l_type, const void* l_ptr)
    {
      texture_class_type &= (~(0xFF<<(no*TEXTURE_CLASS_TYPE_SHIFT)));  // �܂�0�����Ă���
      texture_class_type |= (l_type<<(no*TEXTURE_CLASS_TYPE_SHIFT));   // ���ۂ̒l������
      ptr[no] = l_ptr;
    }
    s32 GetTextureClassType(s32 no) const
    {
      return ( ( texture_class_type >> (no*TEXTURE_CLASS_TYPE_SHIFT) ) & 0xFF );
    }
    const nw::h3d::res::TextureContent* GetPtrAsModel(s32 no) const  // ���f�����������Ƃ��ɓ����Ă����Ƃ������Ƃ�Model�Ƃ��Ă���(ModelTexture)
    {
      return reinterpret_cast<const nw::h3d::res::TextureContent*>(ptr[no]);
    }
    const H3dResTexture* GetPtrAsRes(s32 no) const
    {
      return reinterpret_cast<const H3dResTexture*>(ptr[no]);
    }
    const H3dDynamicTexture* GetPtrAsDynamic(s32 no) const
    {
      return reinterpret_cast<const H3dDynamicTexture*>(ptr[no]);
    }
  };


private:
  // �����ւ��p�̃{�[���̃f�[�^
  struct ReplaceBoneData
  {
    s32 base_index;           // ����Ɠ����{�[����base�ł̃C���f�b�N�X
    s32 base_ancestor_index;  // base_index��gfl::grp::g3d::H3dModel::INDEX_NONE�̂Ƃ��̂ݗL���Ȓl�������Ă���A�����base�̃{�[���̒��Ő�c�ɓ�����{�[���̃C���f�b�N�X
                              // base_index��0�ȏ�̒l��������gfl::grp::g3d::H3dModel::INDEX_IGNORE�̂Ƃ��Abase_ancestor_index�ɂ͉��������Ă��邩�s��
        // (base_index==gfl::grp::g3d::H3dModel::INDEX_NONE&&base_ancestor_index==gfl::grp::g3d::H3dModel::INDEX_NONE)�̂Ƃ��Abase��human�g�b�v�ɒǏ]����
        
    ReplaceBoneData(void)
      : base_index(gfl::grp::g3d::H3dModel::INDEX_NONE),
        base_ancestor_index(gfl::grp::g3d::H3dModel::INDEX_NONE)
    {}
  };

public:
  static void ResetBeforeMaterialSetting()
  {
    s_LastMaterialData.Reset();
    s_GeometryShaderSetUpCheck = false;
  } 
  
  void SetWorldMatrix( const nn::math::MTX34 &mat ){ m_world_mtx = mat; }
  void SetNormalMatrix( const nn::math::MTX34 &mat ){ m_normal_mtx = mat; }
  
  nn::math::MTX34 GetWorldMatrix(){ return m_world_mtx; }
  nn::math::MTX34 GetNormalMatrix(){ return m_normal_mtx; }
  
  b32 IsSkeltalModel()
  {
    return (m_cont->flags & nw::h3d::ModelFlag::IS_SKELETAL_MODEL) ? true : false;
  }
  
  static bool                                 s_DebugFlag;
private:
  struct LastMaterialData{
    void Reset()
    {
      m_Content = NULL;
      m_LightSetId = -1;
      m_FogId = -1;
    }
    const nw::h3d::res::MaterialContent   *m_Content;
    s32                                   m_LightSetId;
    s32                                   m_FogId;
  };
  
  static LastMaterialData     s_LastMaterialData;
  static b32                  s_GeometryShaderSetUpCheck;
  
  const H3dResource*                m_res;         // ���Ő����������\�[�X�̃|�C���^���o���Ă�������
  s32                               m_cont_index;  // m_res���̃R���e���g�̃C���f�b�N�X
  const nw::h3d::res::ModelContent* m_cont;        // m_res���̃R���e���g

  class MemoryObject : public H3dModelMemoryManager::IMemortObject
  {
    friend class H3dModel;
  public:
    MemoryObject();
    virtual ~MemoryObject();

#if GFL_DEBUG    
    void SetDrawFrameCount( u32 cnt ){ m_DrawFrameCount = cnt; }
    u32 GetDrawFrameCount(){ return m_DrawFrameCount; }
    
    void SetModelName( const char* pName );
    const char* GetModelName(void) const { return m_ModelName; }
    
  private:
    u32                                         m_DrawFrameCount;
    u32                                         m_HeapSize;
    u32                                         m_DeviceSize;
    char                                        m_ModelName[32];
#endif
  private:
    void* m_state_heap_mem;
    void* m_state_device_mem;
    gfl::grp::util::DrawUtil::CacheCommand      *m_MaterialCommandChaches;
    
  };
  
  MemoryObject                 *m_pMemoryObject;
  
#if GFL_DEBUG
private:
  static b32 s_SafeDeleteCheckEnable;
  static b32 s_SafeDeleteCheckEnableUserSet;  // Force�I����Ƀ��[�U�ݒ�l�ɖ߂���悤�ɋL�����Ă����B
  static b32 s_SafeDeleteCheckForce;          // true�̂Ƃ�s_SafeDeleteCheckEnable��true�ŌŒ肷��B
  static b32 s_SafeDeleteCheckAssert;         // true�̂Ƃ�ASSERT�Ŏ~�߂�Bfalse�̂Ƃ�PRINT�ōς܂���B
  
public:  
  void SetDrawFrameCount( u32 cnt ){ m_pMemoryObject->SetDrawFrameCount(cnt); }
  u32 GetDrawFrameCount(){ return m_pMemoryObject->GetDrawFrameCount(); }

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
#endif    


#if GFL_DEBUG
private:
  static b32 s_DeleteMemoryControlEnable;  // false�̂Ƃ��A�O������delete_memory_control��true�ɐݒ肵�Ă��A�K��delete_memory_control��false�Ƃ���B
  static b32 s_DeleteMemoryControlAssert;   // true�̂Ƃ��A�O������delete_memory_control��true�ɐݒ肵�Ă�����ASSERT�Ŏ~�߂�Bfalse�̂Ƃ�PRINT�ōς܂���B
public:
  static void DeleteMemoryControlEnable(b32 fEnable) { s_DeleteMemoryControlEnable = fEnable; }
  static b32  IsDeleteMemoryControlEnable(void) { return s_DeleteMemoryControlEnable; }
  static void DeleteMemoryControlAssert(b32 fAssert) { s_DeleteMemoryControlAssert = fAssert; }
  static b32  IsDeleteMemoryControlAssert(void) { return s_DeleteMemoryControlAssert; }
#endif    
  
private:  
  nw::h3d::MaterialStateArray  m_mate_state_array;
  nw::h3d::SkeletonState       m_skel_state;
  nw::h3d::VisibilityState     m_visi_state;

  gfl::grp::ColorU8** m_mate_constant_color_array;  // �R���X�^���g�J���[�Bm_mate_constant_color_array[�}�e���A����][CONSTANT_COLOR_NUM]�ƂȂ�悤�ɂ��Ă����B
                                                    // nw::h3d::MaterialState::SetCombiner"�ԍ�"Constant���A�R���o�C�i"�ԍ�"�p�R���X�^���g�J���[�ł���A
                                                    // �f�U�C�i�[����c�[���̃R���X�^���g�J���["�ԍ�"�ł͂Ȃ��̂ŁA
                                                    // �f�U�C�i�[����c�[���Ɠ��������̃R���X�^���g�J���["�ԍ�"��p�ӂ��Ă����B
  
  H3dMaterialExState*  m_mate_ex_state_array;

  ShaderBindArray m_shader_bind_array;

  MaterialTranslucencyKind* m_mate_translucency_kind_array;  // �}�e���A���̕`�惌�C���[�Bm_mate_translucency_kind_array[�}�e���A����]�ƂȂ�悤�ɂ��Ă����B
  const nw::h3d::res::ShaderContent** m_shader_cont_array;  // ���ݎg���Ă���V�F�[�_�Bm_shader_cont_array[�}�e���A����]�ƂȂ�悤�ɂ��Ă����B
  TextureInfo* m_texture_info_array;  // m_texture_info_array[�}�e���A����]�ƂȂ�悤�ɂ��Ă����B 
      // @note ����܂�ׂ��������ă���������炸�ɁA�}�e���A�����Ŏ����ĕ������Ă�����͍̂\���̂ɂ��Ă܂Ƃ߂ĂƂ��Ă��܂������B

  nn::math::MTX34 m_world_mtx;
  nn::math::MTX34 m_normal_mtx;

  bit32  m_bit_flag;

  gfl::math::VEC3  m_scale_vec;
  gfl::math::VEC3  m_rotate_vec;  // [radian]
  gfl::math::MTX34 m_rotate_mtx;
  gfl::math::VEC3  m_translate_vec;

  H3dAnimationManager*  m_animation_manager;
  
  Scene* m_scene;  // �������Ă���Scene�̃|�C���^���o���Ă�������

  H3dMeshListener* m_pre_mesh_draw_listener_head;
  H3dMeshListener* m_post_mesh_draw_listener_head;

  u8                  m_model_layer_id;  // 0 <= u8 <= 127 (�����o�ϐ��ɂ͂��͈̔͂̐����Ŏ����Ă����B�g���Ƃ���1�r�b�g���炷)
  b32                 m_FragmentAndWBufferDirty;

  // �{�[�������ւ�
  ReplaceBoneData*  m_replace_bone_data_array;  // m_replace_bone_data_array[�{�[����]

  u32* m_mate_proj_array;
  b32           m_fVisible;
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE



#endif // __GFL_G3DH3DMODEL_H__

