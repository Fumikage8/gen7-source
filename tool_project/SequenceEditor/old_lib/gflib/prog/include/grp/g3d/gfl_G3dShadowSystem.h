#ifndef __GFL_G3DSHADOWSYSTEM_H__
#define __GFL_G3DSHADOWSYSTEM_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dShadowSystem.h
 *	@brief  3D�e�V�X�e��
 *	@author	Koji Kawada
 *	@date		2011.03.24
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>

//  gflib grp
#include <grp/gfl_RenderTarget.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dModel.h>
#include <grp/g3d/gfl_G3dCamera.h>
#include <grp/g3d/gfl_G3dShadowGroup.h>


namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3D�e�V�X�e���N���X
//=====================================
class ShadowSystem
{
  GFL_FORBID_COPY_AND_ASSIGN(ShadowSystem);

  
  // ���L�N���X�����private�֐��ɂ��A�N�Z�X���Ă悢
  friend class Scene;


public:
  //---------------------------------------------------------------------------
  /**
   *           �萔�錾
   */
  //---------------------------------------------------------------------------
  enum ReceiverType
  {
    RECEIVER_TYPE_UNKNOWN,       // �s��
    RECEIVER_TYPE_LAMBERT,       // Lambert
    RECEIVER_TYPE_BLINN,         // Blinn
    RECEIVER_TYPE_VERTEX_COLOR,  // ���_�J���[
    RECEIVER_TYPE_ADD,           // ��Z����}�e���A���ݒ肪���ꂽ���V�[�o�[  // @note RECEIVER_TYPE_ADD�������̊o�������H
  };


  //---------------------------------------------------------------------------
  /**
   *           �\���̐錾
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	�V���h�E�̐ݒ���e
  //=====================================
  struct Description
  {
    u32                        shadow_group_num;
    ShadowGroup::Description*  shadow_group_description_array;  // shadow_group_num���̔z���������NULL��n���ĉ������B
        // ��
        // gfl::grp::g3d::Shadow::Description         shadow_description;
        // gfl::grp::g3d::ShadowGroup::Description    shadow_group_description_array[3];
        // shadow_description.shadow_group_num               = 3;
        // shadow_description.shadow_group_description_array = shadow_group_description_array;
    
    //-----------------------------------------------------------------------------
    /**
     *  @brief         �R���X�g���N�^
     */
    //-----------------------------------------------------------------------------
    Description(void)
      : shadow_group_num(1),
        shadow_group_description_array(NULL)
    {}
  };


  //---------------------------------------------------------------------------
  /**
   *           static�֐��錾
   */
  //---------------------------------------------------------------------------
  static ReceiverType CheckShadowReceiverModel(
      const Model*                      model,
      const s32                         material_index
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief    ���\�[�X�ɂ���e�N�X�`�����e�̃e�N�X�`���Ƃ��Ďg�����v���W�F�N�V�����V���h�E��receiver���Z�b�g�A�b�v����
   *            ���̉e�V�X�e���ȊO������Ăяo���Ă��悢����ɂȂ��Ă���B
   *
   *  @param[in]  model
   *  @param[in]  material_index
   *  @param[in]  receiver_type
   *  @param[in]  shadow_intensity               0�`1  // 1-( (1-res_shadow_texture)*shadow_intensity ) ���e�̐F�ɂȂ�B
   *                                             shadow_intensity=1�̂Ƃ�res_shadow_texture�̐F�̂܂܂̉e�ɂȂ�B
   *                                             shadow_intensity=0�̂Ƃ��e�͏o�Ȃ��B
   *  @param[in]  projection_scene_camera_index
   *  @param[in]  texture_resource_pack          �e�̂Ƃ���͔��łȂ��F(���Ƃ�)�A�e�łȂ��Ƃ���͔��ɂȂ��Ă���B
   *  @param[in]  texture_resource               �e�̂Ƃ���͔��łȂ��F(���Ƃ�)�A�e�łȂ��Ƃ���͔��ɂȂ��Ă���B
   *  @param[in]  index_in_resource
   */
  //-----------------------------------------------------------------------------
  static void SetupProjectionShadowReceiver(
      Model*       model,
      s32          material_index,
      ReceiverType receiver_type,
      f32          shadow_intensity,
      s32          projection_scene_camera_index,
      Resource::TextureResourcePack* texture_resource_pack
  );
  static void SetupProjectionShadowReceiver(
      Model*       model,
      s32          material_index,
      ReceiverType receiver_type,
      f32          shadow_intensity,
      s32          projection_scene_camera_index,
      Resource*    texture_resource,
      s32          index_in_resource
  );

#if 0
  //-----------------------------------------------------------------------------
  /**
   *  @brief    ���\�[�X�ɂ���e�N�X�`�����e�̃e�N�X�`���Ƃ��Ďg�����v���W�F�N�V�����V���h�E��receiver��On/Off����
   *            ���̉e�V�X�e���ȊO������Ăяo���Ă��悢����ɂȂ��Ă���B
   *
   *  @param[in]  model
   *  @param[in]  material_index
   *  @param[in]  shadow_intensity               0�`1  // 1-( (1-texture_resource)*shadow_intensity ) ���e�̐F�ɂȂ�B
   *                                             shadow_intensity=1�̂Ƃ�texture_resource�̐F�̂܂܂̉e�ɂȂ�B
   *                                             shadow_intensity=0�̂Ƃ��e�͏o�Ȃ��B
   *  @param[in]  projection_camera_index
   *  @param[in]  texture_resource_pack          �e�̂Ƃ���͔��łȂ��F(���Ƃ�)�A�e�łȂ��Ƃ���͔��ɂȂ��Ă���B
   *  @param[in]  texture_resource               �e�̂Ƃ���͔��łȂ��F(���Ƃ�)�A�e�łȂ��Ƃ���͔��ɂȂ��Ă���B
   *  @param[in]  index_in_resource
   */
  //-----------------------------------------------------------------------------
  static void SwitchOnProjectionShadowReceiver(
      Model*       model,
      s32          material_index,
      f32          shadow_intensity,
      s32          projection_camera_index,
      Resource::TextureResourcePack* texture_resource_pack
  );
  static void SwitchOnProjectionShadowReceiver(
      Model*       model,
      s32          material_index,
      f32          shadow_intensity,
      s32          projection_camera_index,
      Resource*    texture_resource,
      s32          index_in_resource
  );
  static void SwitchOffProjectionShadowReceiver(
      Model*       model,
      s32          material_index
  );
#endif



  //-----------------------------------------------------------------------------
  /*
   * 
   * 
   * 
   * 
   * 
   * 
   * 
   * 
   * 
   * 
   * 
   * 
   * 
   * 
   * 
   * 
   */
  //-----------------------------------------------------------------------------
  // gfx
  static b32 SetupProjectionShadowReceiver(
      Model*       model,
      s32          material_index,
      const gfl::grp::ColorF32&  shadow_intensity,
      s32          projection_camera_index,
      Resource::TextureResourcePack* texture_resource_pack
  );
  static b32 SetupProjectionShadowReceiver(
      Model*       model,
      s32          material_index,
      const gfl::grp::ColorF32&  shadow_intensity,
      s32          projection_camera_index,
      Resource*    texture_resource,
      s32          index_in_resource
  );
  static b32 CleanupProjectionShadowReceiver(
      Model*       model,
      s32          material_index
  );

  static b32 SwitchOnProjectionShadowReceiver(
      Model*       model,
      s32          material_index,
      const gfl::grp::ColorF32&  shadow_intensity,
      s32          projection_camera_index,
      Resource::TextureResourcePack* texture_resource_pack
  );
  static b32 SwitchOnProjectionShadowReceiver(
      Model*       model,
      s32          material_index,
      const gfl::grp::ColorF32&  shadow_intensity,
      s32          projection_camera_index,
      Resource*    texture_resource,
      s32          index_in_resource
  );
  static b32 SwitchOffProjectionShadowReceiver(
      Model*       model,
      s32          material_index
  );


#if GFL_GRP_G3D_H3D_USE
  // h3d
  static b32 SetupProjectionShadowReceiver(
      H3dModel*       model,
      s32          material_index,
      const gfl::grp::ColorF32&  shadow_intensity,
      s32          projection_camera_index,
      H3dResTexture*    texture_resource
  );
  static b32 CleanupProjectionShadowReceiver(
      H3dModel*       model,
      s32          material_index
  );

  static b32 SwitchOnProjectionShadowReceiver(
      H3dModel*       model,
      s32          material_index,
      const gfl::grp::ColorF32&  shadow_intensity,
      s32          projection_camera_index,
      H3dResTexture*    texture_resource
  );
  static b32 SwitchOffProjectionShadowReceiver(
      H3dModel*       model,
      s32          material_index
  );

#endif // GFL_GRP_G3D_H3D_USE

  //-----------------------------------------------------------------------------
  /**
   *  @brief    ���\�[�X�ɂ���e�N�X�`�������̃e�N�X�`���Ƃ��Ďg�����v���W�F�N�V�������C�g��receiver��On/Off����
   *            ���̉e�V�X�e���ȊO������Ăяo���Ă��悢����ɂȂ��Ă���B
   *            Setup�̓f�t�H���g�̐ݒ���s��On�ɂ���
   *            Cleanup�͍Œ���̐ݒ�߂����s��Off�ɂ���(���S�ɐݒ��߂����Ƃ͂ł��܂���)
   *            SwitchOn�͐ݒ��ύX����On�ɂ���
   *            SwitchOff�͐ݒ��ύX������Off�ɂ���
   *
   *  @param[in]  model
   *  @param[in]  material_index
   *  @param[in]  light_intensity                0�`1  // texture_resource*light_intensity �����̐F�ɂȂ�B
   *                                             light_intensity=1�̂Ƃ�texture_resource�̐F�̂܂܂̌��ɂȂ�B
   *                                             light_intensity=0�̂Ƃ����͏o�Ȃ��B
   *  @param[in]  projection_camera_index
   *  @param[in]  texture_resource_pack          ���Z����Ƃ���͍��łȂ��F(���Ƃ�)�A���Z���Ȃ��Ƃ���͍��ɂȂ��Ă���B
   *  @param[in]  texture_resource               ���Z����Ƃ���͍��łȂ��F(���Ƃ�)�A���Z���Ȃ��Ƃ���͍��ɂȂ��Ă���B
   *  @param[in]  index_in_resource
   *
   *  @retval     b32   On/Off�ł�����true��Ԃ��BOn/Off�ł��Ȃ�������false��Ԃ�(���[�U�f�[�^��ProjectionType���Ԉ���Ă���ꍇ�Ȃ�false�ɂȂ�)
   */
  //-----------------------------------------------------------------------------
  /// gfx��
  static b32 SetupProjectionLightReceiver(
      Model*       model,
      s32          material_index,
      //f32          light_intensity,
      const gfl::grp::ColorF32&  light_intensity,
      s32          projection_camera_index,
      Resource::TextureResourcePack* texture_resource_pack
  );
  static b32 SetupProjectionLightReceiver(
      Model*       model,
      s32          material_index,
      //f32          light_intensity,
      const gfl::grp::ColorF32&  light_intensity,
      s32          projection_camera_index,
      Resource*    texture_resource,
      s32          index_in_resource
  );
  
  static b32 CleanupProjectionLightReceiver(
      Model*       model,
      s32          material_index
  );

  static b32 SwitchOnProjectionLightReceiver(
      Model*       model,
      s32          material_index,
      //f32          light_intensity,
      const gfl::grp::ColorF32&  light_intensity,
      s32          projection_camera_index,
      Resource::TextureResourcePack* texture_resource_pack
  );
  static b32 SwitchOnProjectionLightReceiver(
      Model*       model,
      s32          material_index,
      //f32          light_intensity,
      const gfl::grp::ColorF32&  light_intensity,
      s32          projection_camera_index,
      Resource*    texture_resource,
      s32          index_in_resource
  );
  static b32 SwitchOffProjectionLightReceiver(
      Model*       model,
      s32          material_index
  );

#if GFL_GRP_G3D_H3D_USE
  /// H3d��
  static b32 SetupProjectionLightReceiver(
      H3dModel*       model,
      s32          material_index,
      //f32          light_intensity,
      const gfl::grp::ColorF32&  light_intensity,
      s32          projection_camera_index,
      H3dResTexture*    texture_resource
  );
  
  static b32 CleanupProjectionLightReceiver(
      H3dModel*       model,
      s32          material_index
  );

  static b32 SwitchOnProjectionLightReceiver(
      H3dModel*       model,
      s32          material_index,
      //f32          light_intensity,
      const gfl::grp::ColorF32&  light_intensity,
      s32          projection_camera_index,
      H3dResTexture*    texture_resource
  );
  static b32 SwitchOffProjectionLightReceiver(
      H3dModel*       model,
      s32          material_index
  );

#endif // GFL_GRP_G3D_H3D_USE

  // friend�N���X����̂݃A�N�Z�X���Ă��悢private�����o�֐�
private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   *                 Create�͌Ă΂�܂���B
   */
  //-----------------------------------------------------------------------------
  ShadowSystem(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   *                 Destroy���Ăт܂��B
   */
  //-----------------------------------------------------------------------------
  virtual ~ShadowSystem();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���� / �j��
   */
  //-----------------------------------------------------------------------------
  void Create(
      gfl::heap::NwAllocator*         heap_allocator,
      gfl::heap::NwAllocator*         device_allocator,
      const Description*              description = NULL );
  void Destroy(void);
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �e��receiver�ƂȂ郂�f�����Z�b�g�A�b�v����
   */
  //-----------------------------------------------------------------------------
  void SetupShadowReceiverModel(Model* model, s32 material_index, s32 shadow_group_index, ReceiverType receiver_type, f32 shadow_intensity);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �e��receiver�Ƃ��ăZ�b�g�A�b�v�ς݂̃��f��(SetupShadowReceiverModel���������Ă��郂�f��)��On/Off
   */
  //-----------------------------------------------------------------------------
  void SwitchShadowReceiverModel(Model* model, s32 material_index, s32 shadow_group_index, ReceiverType receiver_type, b32 receive_on);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �e��receiver�Ƃ��ăZ�b�g�A�b�v�ς݂̃��f��(SetupShadowReceiverModel���������Ă��郂�f��)�̉e�̋��x��ݒ肷��
   */
  //-----------------------------------------------------------------------------
  void SetShadowReceiverModelShadowIntensity(Model* model, s32 material_index, s32 shadow_group_index, ReceiverType receiver_type, f32 shadow_intensity);
      // @note �g���Ă��Ȃ�����������̂ŁA�����𐸍������ق��������B

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[���`��O���[�v���e�p�ɃZ�b�g�A�b�v����
   */
  //-----------------------------------------------------------------------------
  void  SetupSceneDrawGroupForShadow(const gfl::grp::g3d::ShadowGroup::Type type, gfl::grp::g3d::SceneDrawGroup* scene_draw_group) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �e�O���[�v�̐����擾
   */
  //-----------------------------------------------------------------------------
  u32  GetShadowGroupNum(void) const  { return m_shadow_group_num; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �e�O���[�v�̏����擾
   *
   *  @param[in]     shadow_group_index  m_shadow_group_array�̃C���f�b�N�X
   */
  //-----------------------------------------------------------------------------
  gfl::grp::g3d::ShadowGroup::Type  GetShadowGroupType(const s32 shadow_group_index) const
  {
    return m_shadow_group_array[shadow_group_index]->GetType();
  }
  s32                               GetShadowGroupSceneDrawGroupIndex(const s32 shadow_group_index) const
  {
    return m_shadow_group_array[shadow_group_index]->GetSceneDrawGroupIndex();
  }
  s32                               GetShadowGroupCameraIndex(const s32 shadow_group_index) const
  {
    return m_shadow_group_array[shadow_group_index]->GetCameraIndex();
  }
  gfl::grp::g3d::Camera*            GetShadowGroupCamera(const s32 shadow_group_index) const
  {
    return m_shadow_group_array[shadow_group_index]->GetCamera();
  }
  gfl::grp::RenderTarget*           GetShadowGroupRenderTarget(const s32 shadow_group_index) const
  {
    return m_shadow_group_array[shadow_group_index]->GetRenderTarget();
  }




private:
  enum
  {
    SHADOW_SETTING_RESOURCE_MODEL_PROJECTION_SHADOW   = 0,
    SHADOW_SETTING_RESOURCE_MODEL_SHADOW_MAP,
    SHADOW_SETTING_RESOURCE_MAX,
    // �e�p�̃V�F�[�_��2�ɂȂ�̂͂��������Ȃ��̂ŁA
    // 1���f����2�}�e���A��1�V�F�[�_��
    // 1���f����1�}�e���A��0�V�F�[�_��2�ƃV�F�[�_������1�ɂ��邩

    SHADOW_SETTING_MODEL_PROJECTION_SHADOW   = 0,
    SHADOW_SETTING_MODEL_SHADOW_MAP,
    SHADOW_SETTING_MODEL_MAX
  };

private:
  Resource** m_shadow_setting_resource_array;
  Model**    m_shadow_setting_model_array;

  u32                             m_shadow_group_num;
  gfl::grp::g3d::ShadowGroup**    m_shadow_group_array;




  // friend�N���X������A�N�Z�X���Ă͂����Ȃ�private�����o�֐�
  // �֗��֐��⏈�����e�𒊏o�����֐�
private:
  static void SetupShadowReceiverMaterial(
      nw::gfx::Material*                      material,
      const nw::gfx::ResTexture               shadow_res_texture,  // �e�̂Ƃ���͔��łȂ��F(���Ƃ�)�A�e�łȂ��Ƃ���͔��ɂȂ��Ă���B
      const gfl::grp::g3d::ShadowGroup::Type  type,
      const f32                               shadow_intensity,  // 0�`1  // 1-( (1-res_shadow_texture)*shadow_intensity ) ���e�̐F�ɂȂ�B
                                                                 // shadow_intensity=1�̂Ƃ�res_shadow_texture�̐F�̂܂܂̉e�ɂȂ�B
                                                                 // shadow_intensity=0�̂Ƃ��e�͏o�Ȃ��B
      const ReceiverType                      receiver_type,
      const s32                               shadow_camera_index
  );
  static ReceiverType CheckShadowReceiverMaterial(
      const nw::gfx::Material*                material
  );
  static void SwitchOnShadowReceiverMaterial(
      nw::gfx::Material*                      material,
      const nw::gfx::ResTexture               shadow_res_texture,  // �e�̂Ƃ���͔��łȂ��F(���Ƃ�)�A�e�łȂ��Ƃ���͔��ɂȂ��Ă���B
      const gfl::grp::g3d::ShadowGroup::Type  type,
      const ReceiverType                      receiver_type
  );
  static void SwitchOffShadowReceiverMaterial(
      nw::gfx::Material*                      material,
      const nw::gfx::ResTexture               dummy_res_texture,  // �e�̂Ƃ���͔��łȂ��F(���Ƃ�)�A�e�łȂ��Ƃ���͔��ɂȂ��Ă���B
      const gfl::grp::g3d::ShadowGroup::Type  type,
      const ReceiverType                      receiver_type
  );
  static void SetShadowReceiverMaterialShadowIntensity(
      nw::gfx::Material*                      material,
      const gfl::grp::g3d::ShadowGroup::Type  type,
      const f32                               shadow_intensity,  // 0�`1  // 1-( (1-res_shadow_texture)*shadow_intensity ) ���e�̐F�ɂȂ�B
                                                                 // shadow_intensity=1�̂Ƃ�res_shadow_texture�̐F�̂܂܂̉e�ɂȂ�B
                                                                 // shadow_intensity=0�̂Ƃ��e�͏o�Ȃ��B
      const ReceiverType                      receiver_type
  );  // @note �g���Ă��Ȃ�����������̂ŁA�����𐸍������ق��������B

  //-----------------------------------------------------------------------------
  /**
   *  @brief            �v���W�F�N�V�����V���h�E�p�̃}�e���A���ƃV�F�[�_�[���܂ރ��f���̃Z�b�g�A�b�v
   *
   *  @param[in]        shadow_setting_model    �Z�b�g�A�b�v���郂�f��
   */
  //-----------------------------------------------------------------------------
  static void SetupProjectionShadowSettingModel( nw::gfx::Model* shadow_setting_model );
  //-----------------------------------------------------------------------------
  /**
   *  @brief            �V���h�E�}�b�v�p�̃}�e���A���ƃV�F�[�_�[���܂ރ��f���̃Z�b�g�A�b�v
   *
   *  @param[in]        shadow_setting_model    �Z�b�g�A�b�v���郂�f��
   */
  //-----------------------------------------------------------------------------
  static void SetupShadowMapSettingModel( nw::gfx::Model* shadow_setting_model );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �e�̓��e�̃Z�b�g�A�b�v
   */
  //-----------------------------------------------------------------------------
  /// gfx��
  static void SetupProjectionShadowModel( gfl::grp::g3d::Model* model, s32 material_index, const gfl::grp::ColorF32&  shadow_intensity, s32 projection_camera_index, Resource* texture_resource, s32 index_in_resource );
  static void CleanUpProjectionShadowModel( gfl::grp::g3d::Model* model, s32 material_index, const nw::ut::FloatColor& border_color );

#if GFL_GRP_G3D_H3D_USE
  /// H3d��
  static void SetupProjectionShadowModel( H3dModel* model, s32 material_index, const gfl::grp::ColorF32&  shadow_intensity, s32 projection_camera_index, H3dResTexture* texture_resource );
  static void CleanUpProjectionShadowModel( H3dModel* model, s32 material_index, const nw::ut::FloatColor& border_color );
#endif // GFL_GRP_G3D_H3D_USE
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  ���̓��e�̃Z�b�g�A�b�v
   */
  //-----------------------------------------------------------------------------
  /// gfx��
  static void SetupProjectionLighModel( gfl::grp::g3d::Model* model, s32 material_index, const gfl::grp::ColorF32&  light_intensity, s32 projection_camera_index, Resource* texture_resource, s32 index_in_resource );
  static void CleanUpProjectionLightModel( gfl::grp::g3d::Model* model, s32 material_index, const nw::ut::FloatColor& border_color );

#if GFL_GRP_G3D_H3D_USE
  /// H3d��
  static void SetupProjectionLighModel( H3dModel* model, s32 material_index, const gfl::grp::ColorF32&  light_intensity, s32 projection_camera_index, H3dResTexture* texture_resource );
  static void CleanUpProjectionLightModel( H3dModel* model, s32 material_index, const nw::ut::FloatColor& border_color );
#endif // GFL_GRP_G3D_H3D_USE
  
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DSHADOWSYSTEM_H__
