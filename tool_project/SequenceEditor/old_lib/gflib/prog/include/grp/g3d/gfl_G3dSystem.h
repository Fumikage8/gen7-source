#ifndef __GFL_G3DSYSTEM_H__
#define __GFL_G3DSYSTEM_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dSystem.h
 *	@brief  3D�V�X�e��
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/ut.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>
#include <gfl_Base.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>
#include <grp/g3d/gfl_G3dObjectManager.h>
#include <grp/g3d/gfl_G3dResource.h>
#include <grp/g3d/gfl_G3dH3dResource.h>
#include <grp/g3d/gfl_G3dTexture.h>
#include <grp/g3d/gfl_G3dH3dModelMemoryManager.h>


namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�I�u�W�F�N�g�̍ő吔
//=====================================
enum
{
  SYSTEM_SCENE_ENVIRONMENT_SETTING_OBJECT_NUM_MAX     =  64,  // SceneEnvironmentSetting�̍ő吔
  SYSTEM_SCENE_OBJECT_NUM_MAX                         =   8,  // Scene�̍ő吔
  SYSTEM_MODEL_OBJECT_NUM_MAX                         = 256,  // Model�̍ő吔
  SYSTEM_TEXTURE_OBJECT_NUM_MAX                       = 128,  // Texture�̍ő吔
  SYSTEM_FOG_OBJECT_NUM_MAX                           =  64,  // Fog�̍ő吔
  SYSTEM_CAMERA_OBJECT_NUM_MAX                        =  64,  // Camera�̍ő吔
  SYSTEM_STEREO_CAMERA_OBJECT_NUM_MAX                 =  64,  // StereoCamera�̍ő吔
  SYSTEM_LIGHT_OBJECT_NUM_MAX                         = 256,  // Light�̍ő吔
  SYSTEM_RES_ANIM_OBJECT_NUM_MAX                      = 256,  // ResAnim�̍ő吔
  SYSTEM_ANIMATION_OBJECT_NUM_MAX                     = 256,  // Animation�̍ő吔
  SYSTEM_UNIT_ANIMATION_OBJECT_NUM_MAX                = 256,  // UnitAnimation�̍ő吔
  SYSTEM_RESOURCE_OBJECT_NUM_MAX                      = 256,  // Resource�̍ő吔
  SYSTEM_PARTICLE_OBJECT_NUM_MAX                      = 256,  // Particle�̍ő吔
  SYSTEM_H3D_MODEL_OBJECT_NUM_MAX                     = 256   // H3dModel�̍ő吔
};


//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	3D�V�X�e���N���X
//=====================================
class System : public gfl::base::Singleton<System>
{
  GFL_FORBID_COPY_AND_ASSIGN(System);


public:
  // @brief  ���ʃf�[�^�̎��������^�C�v
  enum AutoCommonDataDeal
  {
    AUTO_COMMON_DATA_DEAL_NONE,  // ���ʃf�[�^�������I�ɓǂݍ��܂Ȃ��������Ȃ�
    AUTO_COMMON_DATA_DEAL_ALL    // ���ʃf�[�^�������I�ɓǂݍ��ݐ�������(���ʃf�[�^���풓���܂�)
  };

  //#######################################################################################
  //
  //            �V�X�e���̐ݒ���e
  // 
  //#######################################################################################
  //---------------------------------------------------------------------------
  /**
   *  @brief �V�X�e���̐ݒ���e�\����
   */
  //---------------------------------------------------------------------------
  struct Description  // max�ƕt���Ă���̂͊Ǘ�����ő吔�A�t���Ă��Ȃ��̂͂��̌�����
  {
    u32  resource_object_num_max;
    u32  scene_object_num_max;
    u32  model_object_num_max;
    u32  light_object_num_max;
    u32  camera_object_num_max;
    u32  stereo_camera_object_num_max;
    u32  fog_object_num_max;
    u32  scene_environment_setting_object_num_max;
    u32  res_anim_object_num_max; 
    u32  animation_object_num_max; 
    u32  unit_animation_object_num_max; 
    u32  texture_object_num_max;
    u32  particle_object_num_max;
    u32  h3d_model_object_num_max;
    AutoCommonDataDeal  auto_common_data_deal;  // ���ʃf�[�^�̎��������^�C�v

    // @brief �R���X�g���N�^
    Description(void)
      : resource_object_num_max                  (SYSTEM_RESOURCE_OBJECT_NUM_MAX),
        scene_object_num_max                     (SYSTEM_SCENE_OBJECT_NUM_MAX),
        model_object_num_max                     (SYSTEM_MODEL_OBJECT_NUM_MAX),
        light_object_num_max                     (SYSTEM_LIGHT_OBJECT_NUM_MAX),
        camera_object_num_max                    (SYSTEM_CAMERA_OBJECT_NUM_MAX),
        stereo_camera_object_num_max             (SYSTEM_STEREO_CAMERA_OBJECT_NUM_MAX),
        fog_object_num_max                       (SYSTEM_FOG_OBJECT_NUM_MAX),
        scene_environment_setting_object_num_max (SYSTEM_SCENE_ENVIRONMENT_SETTING_OBJECT_NUM_MAX),
        res_anim_object_num_max                  (SYSTEM_RES_ANIM_OBJECT_NUM_MAX),
        animation_object_num_max                 (SYSTEM_ANIMATION_OBJECT_NUM_MAX),
        unit_animation_object_num_max            (SYSTEM_UNIT_ANIMATION_OBJECT_NUM_MAX),
        texture_object_num_max                   (SYSTEM_TEXTURE_OBJECT_NUM_MAX),
        particle_object_num_max                  (SYSTEM_PARTICLE_OBJECT_NUM_MAX),
        h3d_model_object_num_max                 (SYSTEM_H3D_MODEL_OBJECT_NUM_MAX),
        auto_common_data_deal                    (AUTO_COMMON_DATA_DEAL_ALL)
    {}
  };


public:
  //#######################################################################################
  //
  //           ���ʃf�[�^
  //
  //#######################################################################################
  //---------------------------------------------------------------------------
  /**
   *  @brief  ���ʃf�[�^��`
   */
  //---------------------------------------------------------------------------
  enum CommonDataType
  {
    COMMON_DATA_TYPE_NONE,                     // ����`�B�����̃o�b�t�@��������Ȃ����Agfl::grp::g3d::Camera�Ƃ���������Ȃ����B
    
    // ���ʃf�[�^ 
    COMMON_DATA_TYPE_DEFAULT_SHADER,           // gfl�f�t�H���g�V�F�[�_
    COMMON_DATA_TYPE_PARTICLE_DEFAULT_SHADER,  // gfl�p�[�e�B�N���f�t�H���g�V�F�[�_
    COMMON_DATA_TYPE_NW_DEFAULT_SHADER,        // NW�f�t�H���g�V�F�[�_
    COMMON_DATA_TYPE_SMALL_TEXTURE,            // �������e�N�X�`��
#if GFL_GRP_G3D_H3D_USE
    COMMON_DATA_TYPE_H3D_DEFAULT_SHADER,       // gfl h3d�f�t�H���g�V�F�[�_
    COMMON_DATA_TYPE_H3D_SMALL_TEXTURE,        // h3d�������e�N�X�`��
#endif  // GFL_GRP_G3D_H3D_USE

    // Ex���ʃf�[�^
    EX_COMMON_DATA_TYPE_BUFFER_RESOURCE_SHADER,   // �o�b�t�@���烊�\�[�X�𐶐�����V�F�[�_
    EX_COMMON_DATA_TYPE_BUFFER_RESOURCE_TEXTURE,  // �o�b�t�@���烊�\�[�X�𐶐�����e�N�X�`��
    EX_COMMON_DATA_TYPE_RESOURCE_SHADER,          // ���\�[�X�ɂȂ��Ă���V�F�[�_
    EX_COMMON_DATA_TYPE_RESOURCE_TEXTURE          // ���\�[�X�ɂȂ��Ă���e�N�X�`��
  };

  #if 0
/*    ���ʃf�[�^   = �V�X�e���Ŏ����Ă��鋤�ʃf�[�^�BCOMMON_DATA, CommonData, common_data�Ƃ������O���t���Ă���B
                   ���ʃ��\�[�X(�uCOMMON_RESOURCE_BIT_���O�v)�ƂȂ��Ă�����̂�����A������Resource��Setup�Ŏg�p�\�B
    Ex���ʃf�[�^ = ���[�U���ǉ��������ʃf�[�^�BEX_COMMON_DATA, ExCommonData, ex_common_data�Ƃ������O���t���Ă���B
                   ���ʃ��\�[�X(�uEX_COMMON_RESOURCE_BIT_���O�v)�ɂ��邱�Ƃ��ł�(Ex���ʃ��\�[�X�ƌĂ�)�A���������ꍇ��Resource��Setup�Ŏg�p�\�B
                   CommonData�Ɠ��l�̕��@�Ő����A�j���A�A�N�Z�X�ł���̂ŁACommonData�Ƃ������O�̕t�����\���̂�֐��ɂ����ڂ��邱�ƁB*/
  #endif

  //---------------------------------------------------------------------------
  /**
  // @brief  ���ʃf�[�^�����ɕK�v�Ȉ������܂Ƃ߂��\����
   */
  //---------------------------------------------------------------------------
  struct CommonDataCreateArg
  {
    CommonDataType type;
    void*          ptr;
    u32            param0;
    u32            param1;

  #if 0
    /*+---------------------------------------------+----------+---------------------------------------+-------------------+----------+
    | type                                        | ptr      | param0                                | param1            | ������   |
    +---------------------------------------------+----------+---------------------------------------+-------------------+----------+
    | COMMON_DATA_TYPE_NONE                       | NULL     | PARAM_NONE                            | PARAM_NONE        | NULL     |
    +---------------------------------------------+----------+---------------------------------------+-------------------+----------+
    | COMMON_DATA_TYPE_DEFAULT_SHADER             | buffer   | index_in_resource                     | PARAM_NONE        | Resource |
    | COMMON_DATA_TYPE_PARTICLE_DEFAULT_SHADER    | buffer   | index_in_resource                     | PARAM_NONE        | Resource |
    | COMMON_DATA_TYPE_NW_DEFAULT_SHADER          | buffer   | index_in_resource                     | PARAM_NONE        | Resource |
    | COMMON_DATA_TYPE_SMALL_TEXTURE              | NULL     | PARAM_NONE                            | PARAM_NONE        | Texture  |
    | COMMON_DATA_TYPE_H3D_DEFAULT_SHADER         | buffer   | ?                                     | ?                 | Resource |
    | COMMON_DATA_TYPE_H3D_SMALL_TEXTURE          | NULL     | PARAM_NONE                            | PARAM_NONE        | Texture  |
    +---------------------------------------------+----------+---------------------------------------+-------------------+----------+
    | EX_COMMON_DATA_TYPE_BUFFER_RESOURCE_SHADER  | buffer   | Resource::EX_COMMON_RESOURCE_BIT_�ԍ� | index_in_resource | Resource |
    | EX_COMMON_DATA_TYPE_BUFFER_RESOURCE_TEXTURE | buffer   | Resource::EX_COMMON_RESOURCE_BIT_�ԍ� | index_in_resource | Resource |
    | EX_COMMON_DATA_TYPE_RESOURCE_SHADER         | Resource | Resource::EX_COMMON_RESOURCE_BIT_�ԍ� | index_in_resource | Resource |
    | EX_COMMON_DATA_TYPE_RESOURCE_TEXTURE        | Resource | Resource::EX_COMMON_RESOURCE_BIT_�ԍ� | index_in_resource | Resource |
    +---------------------------------------------+----------+---------------------------------------+-------------------+----------+*/
  #endif
    
    // @brief  �萔
    enum
    {
      PARAM_NONE   = 0xFFFFFFFF
    };

    // @brief  �R���X�g���N�^
    CommonDataCreateArg(void)
      : type(COMMON_DATA_TYPE_NONE),
        ptr(NULL),
        param0(PARAM_NONE),
        param1(PARAM_NONE)
    {}

    // @brief  �R���X�g���N�^
    CommonDataCreateArg(
        CommonDataType a_type,
        void*          a_ptr,
        u32            a_param0,
        u32            a_param1
    )
      : type(a_type),
        ptr(a_ptr),
        param0(a_param0),
        param1(a_param1)
    {}
  };

  //---------------------------------------------------------------------------
  /**
   *  @brief  ���ʃf�[�^�𐶐�����
   *  @retval  �����ł����Ƃ�true��Ԃ�
   */
  //---------------------------------------------------------------------------
  b32 CreateCommonData(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      CommonDataCreateArg&    common_data_create_arg
  );
  
  //---------------------------------------------------------------------------
  /**
   *  @brief  ���ʃf�[�^��j������
   *          ptr���w�����̂��̂��̂̉���͌Ăяo�����ōs���ĉ������B
   *  @param[in]  ptr  CreateCommonData�ŗ^����CommonDataCreateArg��ptr
   */
  //---------------------------------------------------------------------------
  void DestroyCommonData(
      void* ptr
  );

  //---------------------------------------------------------------------------
  /**
   *  @brief  �S�Ă̋��ʃf�[�^��j������
   *          ptr���w�����̂��̂��̂̉���͌Ăяo�����ōs���ĉ������B
   *          CreateCommonData�Ő����������̂����j�����܂��BAutoCommonDataDeal�Ő����������͔̂j�����܂���B
   */
  //---------------------------------------------------------------------------
  void DestroyAllCommonData(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief  ���ʃf�[�^���擾����
   *          CreateCommonData�Ő����������̂��AAutoCommonDataDeal�Ő����������̂��A�ǂ�����擾�ł��܂��B
   *  @retval       
   */
  //-----------------------------------------------------------------------------
  inline Resource*     GetCommonDataDefaultShaderResource(void) const;
  inline Resource*     GetCommonDataParticleDefaultShaderResource(void) const;
  inline Resource*     GetCommonDataNwDefaultShaderResource(void) const;
  //�ύX���܂���inline Resource*     GetCommonDataSmallTextureResource(void) const;  // L4 8x8 ����F�̃e�N�X�`��
  inline Texture*      GetCommonDataSmallTexture(void) const;  // L4 8x8 ����F�̃e�N�X�`��
#if GFL_GRP_G3D_H3D_USE
  inline H3dResource*        GetCommonDataH3dDefaultShaderResource(void) const;
  inline H3dDynamicTexture*  GetCommonDataH3dSmallTexture(void) const;  // L4 8x8 ����F�̃e�N�X�`��
#endif  // GFL_GRP_G3D_H3D_USE
  //�Ȃ����܂���inline Resource*     GetCommonDataMiddleTextureResource(void) const;  // 


  //-----------------------------------------------------------------------------
  /**
   *  @brief  ���ʃf�[�^�𓾂�
   *  @param[in]  type  CreateCommonData�ŗ^����CommonDataCreateArg��type�ł��B����type�̃f�[�^����������ꍇ�́A��ԍŏ��ɐ����������̂�Ԃ��܂��B
   *  @retval  �������ꂽ�f�[�^�Btype�ɉ����ăL���X�g���Ďg�p���ĉ������B
   */
  //-----------------------------------------------------------------------------
  inline void* GetCommonData(CommonDataType type) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief  ���ʃf�[�^�𓾂�
   *  @param[in]  ptr  CreateCommonData�ŗ^����CommonDataCreateArg��ptr
   *  @retval  �������ꂽ�f�[�^�Btype�ɉ����ăL���X�g���Ďg�p���ĉ������B
   */
  //-----------------------------------------------------------------------------
  inline void* GetCommonData(void* ptr) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief  �uEX_COMMON_DATA_TYPE_BUFFER_RESOURCE_���O�v�𓾂�
   *  @param[in]  param0  �uCommonDataCreateArg�v�́uparam0�v�ɓn�����uResource::EX_COMMON_RESOURCE_BIT_�ԍ��v
   *  @retval  Resource
   */
  //-----------------------------------------------------------------------------
  inline Resource* GetExCommonDataExCommonResource(u32 param0) const;





  //#######################################################################################
  //
  //   g3d���\�[�X�f�[�^
  //   ���L���\�[�X�����ׂĊO���w�肷�邱�ƂŁAg3d���C�u���������ł̃t�@�C���ǂݍ��݂��J�b�g���邱�Ƃ��ł���B
  //
  //#######################################################################################
  enum ResourceEnum{
    GFL_DEFAULT_SHADER_BCSDR,           // gfl�f�t�H���g�V�F�[�_
    NWGFX_PARTICLEDEFAULTSHADER_BCSDR,  // gfl�p�[�e�B�N���f�t�H���g�V�F�[�_
    NWGFX_DEFAULTSHADER_BCSDR,          // NW�f�t�H���g�V�F�[�_
#if GFL_GRP_G3D_H3D_USE
    GFL_DEFAULTSHADER_BCHSDR,           // gfl h3d�f�t�H���g�V�F�[�_
#endif  // GFL_GRP_G3D_H3D_USE
    BLOOM_FILTER_HIGH_LUMINANCE_BOARD_BCRES,  // �u���[���t�B���^�[�f��
    BLOOM_FILTER_BLOOM_BOARD_BCRES,           // �u���[���t�B���^�[�f��
    TEXTURE_CHECK_BOARD_NO_ALPHA_BCRES,       // �u���[���t�B���^�[�f��
    EDGE_FILLTER_BCRES,                       // �G�b�W�t�B���^�[�f��
    VCOLOR_EDGE_FILLTER_BCRES,                // �G�b�W�t�B���^�[�f��
    IDEDGE_FILLTER_BCRES,                     // �G�b�W�t�B���^�[�f��
    EDGE_FILLTER_NORMAL_BCRES,                // �G�b�W�t�B���^�[�f��
    INSIDE_EDGE_FILLTER_BCRES,                // �G�b�W�t�B���^�[�f��
    NORMAL_TEXTURE_BCTEX,                     // �G�b�W�t�B���^�[�f��
    NORMAL_TO_UV_SHADER_BCSDR,                // �G�b�W�t�B���^�[�f��
    EDGE_MAP_VCOLA_BCMDL,                     // �G�b�W�t�B���^�[�f��
    EDGE_MAP_ALPHAVCOLA_BCMDL,                // �G�b�W�t�B���^�[�f��
    EDGE_MAP_VCOLA_DISABLE_NORMAL_BCMDL,      // �G�b�W�t�B���^�[�f��
    EDGE_MAP_ALPHA_VCOLA_DISABLE_NORMAL_BCMDL,// �G�b�W�t�B���^�[�f��
    EDGE_MAP_DISABLE_NORMAL_VCOLA_BCMDL,      // �G�b�W�t�B���^�[�f��
    EDGE_MAP_ALPHA_DISABLE_NORMAL_VCOLA_BCMDL,// �G�b�W�t�B���^�[�f��
#if GFL_GRP_G3D_H3D_USE
    NORMAL_TEXTURE_BCTEX_H3D,                     // �G�b�W�t�B���^�[�f��
    NORMAL_TO_UV_SHADER_BCSDR_H3D,                // �G�b�W�t�B���^�[�f��
    EDGE_MAP_VCOLA_BCMDL_H3D,                 // �G�b�W�t�B���^�[�f��
    EDGE_MAP_ALPHAVCOLA_BCMDL_H3D,            // �G�b�W�t�B���^�[�f��
    EDGE_MAP_VCOLA_DISABLE_NORMAL_BCMDL_H3D,  // �G�b�W�t�B���^�[�f��
    EDGE_MAP_ALPHA_VCOLA_DISABLE_NORMAL_BCMDL_H3D,  // �G�b�W�t�B���^�[�f��
    EDGE_MAP_DISABLE_NORMAL_VCOLA_BCMDL_H3D,        // �G�b�W�t�B���^�[�f��
    EDGE_MAP_ALPHA_DISABLE_NORMAL_VCOLA_BCMDL_H3D,  // �G�b�W�t�B���^�[�f��
#endif // GFL_GRP_G3D_H3D_USE
#if GFL_DEBUG
    EDGE_DEBUG_RGB_BCRES,                     // �G�b�W�t�B���^�[�f�ށi�f�o�b�O���̂݁j
    EDGE_DEBUG_A_BCRES,                       // �G�b�W�t�B���^�[�f�ށi�f�o�b�O���̂݁j
    EDGE_DEBUG_R_BCRES,                       // �G�b�W�t�B���^�[�f�ށi�f�o�b�O���̂݁j
#endif // GFL_DEBUG

    RESOURCE_MAX,   // g3d�V�X�e�����v�����郊�\�[�X��
  };

  //----------------------------------------------------------------------------
  /**
   *	@brief  g3d���C�u�����Ŏg�p���郊�\�[�X���O���w��
   */
  //-----------------------------------------------------------------------------
  // �ݒ�
  static void SetStaticResource( void * buff, ResourceEnum resource_type );
  // �擾
  static void* GetStaticResource( ResourceEnum resource_type );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �O���w�肳�ꂽg3d���C�u�����Ŏg�p���郊�\�[�X��ێ��Ă���e�[�u�������ׂăN���A
   *	*�������̉���͊O���ŁB
   */
  //-----------------------------------------------------------------------------
  static void ClearStaticResourceAll( void );



public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   *
   *  @param[in]     heap_allocator    �q�[�v�������̃A���P�[�^
   *  @param[in]     device_allocator  �f�o�C�X�������̃A���P�[�^
   *  @param[in]     description       �ݒ���e
   */
  //-----------------------------------------------------------------------------
  System( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Description* description = NULL );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  ~System();  // �p���͍l���Ă��Ȃ�


  //----------------------------------------------------------------------------
  /**
   *	@brief  �X�V����  �iScene��Calculate��SubmitView�̑O�ɓǂ�ł��������j
   */
  //-----------------------------------------------------------------------------
  void Update(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief        �}�l�[�W���[���擾����
   *
   *  @retval       ObjectManager*       �}�l�[�W���[
   */
  //-----------------------------------------------------------------------------
  ObjectManager*   GetSceneEnvironmentSettingManager(void)  {  return m_scene_environment_setting_manager;  }
  ObjectManager*   GetSceneManager(void)                    {  return m_scene_manager;  }
  ObjectManager*   GetModelManager(void)                    {  return m_model_manager;  }
  ObjectManager*   GetParticleManager(void)                 {  return m_particle_manager;  }
  ObjectManager*   GetTextureManager(void)                  {  return m_texture_manager;  }
  ObjectManager*   GetFogManager(void)                      {  return m_fog_manager;  }
  ObjectManager*   GetCameraManager(void)                   {  return m_camera_manager;  }
  ObjectManager*   GetStereoCameraManager(void)             {  return m_stereo_camera_manager;  }
  ObjectManager*   GetLightManager(void)                    {  return m_light_manager;  }
  ObjectManager*   GetResAnimManager(void)                  {  return m_res_anim_manager;  }
  ObjectManager*   GetAnimationManager(void)                {  return m_animation_manager;  }
  ObjectManager*   GetUnitAnimationManager(void)            {  return m_unit_animation_manager;  }
  ObjectManager*   GetResourceManager(void)                 {  return m_resource_manager;  }
  ObjectManager*   GetH3dModelManager(void)                 {  return m_h3d_model_manager;  }

#if GFL_GRP_G3D_H3D_USE
  //-----------------------------------------------------------------------------
  /**
   *  @brief        H3d�������}�l�[�W�����擾����
   */
  //-----------------------------------------------------------------------------
  inline H3dModelMemoryManager* GetH3dModelMemoryManager(void) const { return m_h3d_model_memory_manager; }
#endif  // GFL_GRP_G3D_H3D_USE

  //-----------------------------------------------------------------------------
  /**
   *  @brief        �`���������������
   */
  //-----------------------------------------------------------------------------
  //�g�p�֎~�ɂ��Ă���void ResetRenderState(void);

private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����������
   */
  //-----------------------------------------------------------------------------
  void initialize(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator); 

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �I������
   */
  //-----------------------------------------------------------------------------
  void finalize(void); 

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���ʃf�[�^�������I�ɓǂݍ��ݐ�������A�j������
   */
  //-----------------------------------------------------------------------------
  void autoCreateCommonData(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator);
  void autoDestroyCommonData(void);

  void autoCreateCommonDataDefaultShaderResource(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator);
  void autoDestoryCommonDataDefaultShaderResource(void);
  void autoCreateCommonDataParticleDefaultShaderResource(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator);
  void autoDestroyCommonDataParticleDefaultShaderResource(void);
  void autoCreateCommonDataNwDefaultShaderResource(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator);
  void autoDestoryCommonDataNwDefaultShaderResource(void);
  void autoCreateCommonDataSmallTexture(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator);  // L4 8x8 ����F�̃e�N�X�`��
  void autoDestoryCommonDataSmallTexture(void);  // L4 8x8 ����F�̃e�N�X�`��
#if GFL_GRP_G3D_H3D_USE
  void autoCreateCommonDataH3dDefaultShaderResource(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator);
  void autoDestroyCommonDataH3dDefaultShaderResource(void);
  void autoCreateCommonDataH3dSmallTexture(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator);  // L4 8x8 ����F�̃e�N�X�`��
  void autoDestoryCommonDataH3dSmallTexture(void);  // L4 8x8 ����F�̃e�N�X�`��
#endif  // GFL_GRP_G3D_H3D_USE

  
private:
  // ���ʃf�[�^�̎��������t���O
  enum
  {
    AUTO_COMMON_DATA_DEAL_BIT_NONE                             = 0,
    AUTO_COMMON_DATA_DEAL_BIT_DEFAULT_SHADER_RESOURCE          = 1 <<  0,
    AUTO_COMMON_DATA_DEAL_BIT_PARTICLE_DEFAULT_SHADER_RESOURCE = 1 <<  1,
    AUTO_COMMON_DATA_DEAL_BIT_NW_DEFAULT_SHADER_RESOURCE       = 1 <<  2,
    AUTO_COMMON_DATA_DEAL_BIT_SMALL_TEXTURE                    = 1 <<  3
#if GFL_GRP_G3D_H3D_USE
    , AUTO_COMMON_DATA_DEAL_BIT_H3D_DEFAULT_SHADER_RESOURCE      = 1 <<  4 
    , AUTO_COMMON_DATA_DEAL_BIT_H3D_SMALL_TEXTURE                = 1 <<  5
#endif  // GFL_GRP_G3D_H3D_USE
  };

  // ���ʃf�[�^�v�f
  struct CommonDataElement
  {
    CommonDataCreateArg arg;
    void*               data;  // ������

    CommonDataElement*  prev;  // �擪�v�f�̂Ƃ���NULL
    CommonDataElement*  next;  // �����v�f�̂Ƃ���NULL

    CommonDataElement(void)
      : arg(),
        data(NULL),
        prev(NULL),
        next(NULL)
    {}
  };


private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���ʃf�[�^�̐����A�j��
   *                 CommonDataElement�̘g�́A�Ăяo�����Ő����A�j�����ĉ������B
   */
  //-----------------------------------------------------------------------------
  static void* createCommonDataSmallTextureBuffer(gfl::heap::NwAllocator* device_allocator);  // L4 8x8 ����F�̃e�N�X�`���o�b�t�@����
  static void  destroyCommonDataSmallTextureBuffer(void* buffer);  // L4 8x8 ����F�̃e�N�X�`���o�b�t�@�j��
  
  static Texture* createCommonDataSmallTexture(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, void* buffer);  // L4 8x8 ����F�̃e�N�X�`��
#if GFL_GRP_G3D_H3D_USE
  static H3dDynamicTexture* createCommonDataH3dSmallTexture(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, void* buffer);  // L4 8x8 ����F�̃e�N�X�`��
#endif  // GFL_GRP_G3D_H3D_USE
  b32 createCommonData(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      CommonDataElement* elem
  );
  void destroyCommonData(CommonDataElement* elem);


private:
  
  ObjectManager*   m_scene_environment_setting_manager;
  ObjectManager*   m_scene_manager;
  ObjectManager*   m_model_manager;
  ObjectManager*   m_particle_manager;
  ObjectManager*   m_texture_manager;
  ObjectManager*   m_fog_manager;
  ObjectManager*   m_camera_manager;
  ObjectManager*   m_stereo_camera_manager;
  ObjectManager*   m_light_manager;
  ObjectManager*   m_res_anim_manager;
  ObjectManager*   m_animation_manager;
  ObjectManager*   m_unit_animation_manager;
  ObjectManager*   m_resource_manager;
  ObjectManager*   m_h3d_model_manager;

  b32    m_is_initialized;  // �������ς݂Ȃ�true
  bit32  m_auto_common_data_deal_bit;

  void*         m_small_texture_buffer;  // L4 8x8 ����F�̃e�N�X�`���o�b�t�@
  
  Resource*     m_default_shader_resource;
  Resource*     m_particle_default_shader_resource;
  Resource*     m_nw_default_shader_resource;
  //Resource*     m_small_texture_resource;  // L4 8x8 ����F�̃e�N�X�`��
  Texture*      m_small_texture;  // L4 8x8 ����F�̃e�N�X�`��

#if GFL_GRP_G3D_H3D_USE
  void*               m_h3d_default_shader_buffer;
  b8                  m_h3d_default_shader_buffer_release_flag;
  H3dResource*        m_h3d_default_shader_resource;
  H3dDynamicTexture*  m_h3d_small_texture;  // L4 8x8 ����F�̃e�N�X�`��
  H3dModelMemoryManager* m_h3d_model_memory_manager;  // H3d�̃��f���������}�l�[�W��
#endif  // GFL_GRP_G3D_H3D_USE

  CommonDataElement*  m_common_data_head_elem;  // �擪�v�f���w��


  // ���[�U�[�w�胊�\�[�X
  static void * m_pStaticResourceTbl[RESOURCE_MAX];
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DSYSTEM_H__
