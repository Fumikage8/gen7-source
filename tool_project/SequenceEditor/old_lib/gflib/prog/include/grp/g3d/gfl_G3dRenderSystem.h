#ifndef __GFL_G3DRENDERSYSTEM_H__
#define __GFL_G3DRENDERSYSTEM_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dRenderSystem.h
 *	@brief  3D�`��V�X�e��
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>
#include <nw/gfx/gfx_SceneUpdateHelper.h>  // #include <nw/gfx.h> �ł�gfx_SceneUpdateHelper.h�̓C���N���[�h����Ȃ��悤���B

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>

// gflib grp
#include <grp/gfl_GraphicsSystem.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dSceneSystem.h>
#include <grp/g3d/gfl_G3dSceneCalculateGroup.h>
#include <grp/g3d/gfl_G3dSceneDrawGroup.h>
#include <grp/g3d/gfl_G3dModel.h>

#if GFL_GRP_G3D_H3D_USE
#include <grp/g3d/gfl_G3dH3dModel.h>
#endif  // GFL_GRP_G3D_H3D_USE


namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	3D�`��V�X�e���N���X
//=====================================
class RenderSystem
{
  GFL_FORBID_COPY_AND_ASSIGN(RenderSystem);


  // ���L�N���X���炵���A�N�Z�X���Ȃ�
  friend class Scene;


public:
  //---------------------------------------------------------------------------
  /**
   *           �\���̐錾
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	�V�[���V�X�e���̐ݒ���e
  //=====================================
  struct Description
  {
    // m_render_context�p�̐ݒ�
    s32  max_cameras;        // �J�����̍ő吔
    s32  max_light_sets;     // ���C�g�Z�b�g�̍ő吔
    s32  max_fogs;           // �t�H�O�̍ő吔
    s32  max_vertex_lights;  // ���_���C�g�̍ő吔
 
    //-----------------------------------------------------------------------------
    /**
     *  @brief         �R���X�g���N�^
     */
    //-----------------------------------------------------------------------------
    Description(void)
      : max_cameras        (nw::gfx::RenderContext::DEFAULT_MAX_CAMERAS),
        max_light_sets     (nw::gfx::RenderContext::DEFAULT_MAX_LIGHT_SETS),
        max_fogs           (nw::gfx::RenderContext::DEFAULT_MAX_FOGS),
        max_vertex_lights  (nw::gfx::LightSet::DEFAULT_MAX_VERTEX_LIGHTS)
    {}
  };



  //-----------------------------------------------------------------------------
  /**
   *    �{�b�N�X�e�X�g�̐ݒ�,�p�����[�^�擾
   */
  //-----------------------------------------------------------------------------
  static void SetBoxTest( bool flag );
  static bool IsBoxTest( void );
  static s32 GetBoxTestCutCount( void );
  static s32 GetBoxTestThroughCount( void );


  // �����_�����O�������b�V�������J�E���g�A�b�v
  inline void ClearRenderMeshCount(void){ m_render_mesh_count = 0; }
  inline void AddRenderMeshCount(void){ ++m_render_mesh_count; }
  inline s32 GetRenderMeshCount( void ) const { return m_render_mesh_count; }
private:
  //-----------------------------------------------------------------------------
  /**
   *					�N���X�錾
   */
  //-----------------------------------------------------------------------------
  //-------------------------------------
  ///	���f���̃��[�U�[�p�����[�^���`�F�b�N���āA�`��v�f�ɉ����邩�ۂ��𔻒肷��֐��I�u�W�F�N�g
  //=====================================
  class ModelUserParameterCheckFunctor : public nw::gfx::ISceneUpdater::IsVisibleModelFunctor
  {
    GFL_FORBID_COPY_AND_ASSIGN(ModelUserParameterCheckFunctor);
  public:
    ModelUserParameterCheckFunctor(void)
      : m_render_queue_index(0)
    {
      // nw::gfx::ISceneUpdater::IsVisibleModelFunctor �ɂ̓R���X�g���N�^���f�X�g���N�^���Ȃ�
      // �������Ȃ�
    }
    ModelUserParameterCheckFunctor(s32 render_queue_index)
      : m_render_queue_index(render_queue_index)
    {
      // nw::gfx::ISceneUpdater::IsVisibleModelFunctor �ɂ̓R���X�g���N�^���f�X�g���N�^���Ȃ�
      // �������Ȃ�
    }
    virtual ~ModelUserParameterCheckFunctor()
    {
      // nw::gfx::ISceneUpdater::IsVisibleModelFunctor �ɂ̓R���X�g���N�^���f�X�g���N�^���Ȃ�
      // �������Ȃ�
    }
    void SetRenderQueueIndex(u32 render_queue_index)
    {
      m_render_queue_index = render_queue_index;
    }
    u32 GetRenderQueueIndex(void) const
    {
      return m_render_queue_index;
    }
  public:
    virtual bool IsVisible(const nw::gfx::Model* model)
    {
      return gfl::grp::g3d::Model::IsUserParameterRenderQueue(model, m_render_queue_index);
    }

  private:
    s32 m_render_queue_index;
  };

private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   *
   *  @param[in]     allocator  �A���P�[�^
   */
  //-----------------------------------------------------------------------------
  RenderSystem(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, Description* description = NULL);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  virtual ~RenderSystem(); 

  void InitializeSceneDrawGroup(SceneDrawGroup* scene_draw_group);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[�����ݒ肩��V�[������K�p����
   *
   *  @param[in]     scene_environment_setting  �K�p����V�[����
   *                                            (nw::gfx::SceneEnvironmentSetting::ResolveReference���ς�ł��邱��)
   */
  //-----------------------------------------------------------------------------
  void SetSceneEnvironmentSetting(const nw::gfx::SceneEnvironmentSetting& scene_environment_setting);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �����_�[�R���e�L�X�g�ɐݒ肳�ꂽ�V�[�������擾����
   */
  //-----------------------------------------------------------------------------
  const nw::gfx::SceneEnvironment& GetSceneEnvironment(s32* max_cameras, s32* max_light_sets, s32* max_fogs, s32* max_vertex_lights) const
  {
    *max_cameras       = m_max_cameras_plus_system_camera;
    *max_light_sets    = m_max_light_sets;
    *max_fogs          = m_max_fogs;
    *max_vertex_lights = m_max_vertex_lights;

    return m_render_context->GetSceneEnvironment();
  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[�����ɃJ������ݒ肷��
   *
   *  @param[in]     camera_index   �J�����̃C���f�b�N�X
   *  @param[in]     camera         �J����(camera_index�ɐݒ肷��)
   * 
   *  �ucamera=NULL, camera_index=�C���f�b�N�X�v��n����camera_index�Ɋ��蓖�ĂĂ����J�������Ȃ��ɂ���
   */
  //-----------------------------------------------------------------------------
  void SetCamera(s32 camera_index, nw::gfx::Camera* camera);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[�����̃J�����z�񂩂�J�������擾����
   *
   *  @param[in]     camera_index   �J�����̃C���f�b�N�X
   *
   *  @retval        �z��̂��̃C���f�b�N�X�ɓn���Ă������J�����̃|�C���^�𓾂�B
   *                 NULL�̂Ƃ����̃C���f�b�N�X�ɂ̓J������n���Ă��Ȃ������B
   */
  //-----------------------------------------------------------------------------
  nw::gfx::Camera* GetCamera(const s32 camera_index) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[�����̃J�����z�񂩂�J�����̃C���f�b�N�X���擾����
   *
   *  @param[in]     camera   �J����
   *
   *  @retval        �z��ɂ��̃J����������ꍇ���̃C���f�b�N�X�𓾂�B
   *                 �Ȃ��ꍇCAMERA_INDEX_NONE��Ԃ��B
   */
  //-----------------------------------------------------------------------------
  s32              GetCameraIndex(const nw::gfx::Camera* camera) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[�����̃J�����z�񂩂��ԍŏ��ɂ���J�����̃C���f�b�N�X���擾����
   *
   *  @retval        �z��Ɉ���J�������Ȃ��ꍇCAMERA_INDEX_NONE��Ԃ��B
   */
  //-----------------------------------------------------------------------------
  s32              GetFirstCameraIndex(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[�����ɃJ�������J�����z��ɐݒ肵�Ă��Ȃ��J������ݒ肷��
   *
   *  @param[in]     camera         �J����(GetNotSetCameraIndex�ɐݒ肷��)
   * 
   *  �ucamera=NULL�v��n����GetNotSetCameraIndex�Ɋ��蓖�ĂĂ����J�������Ȃ��ɂ���
   */
  //-----------------------------------------------------------------------------
  void SetNotSetCamera(nw::gfx::Camera* camera);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �J�������J�����z��ɐݒ肵�Ă��Ȃ��Ƃ��Ɏg���J�����̃C���f�b�N�X���擾����
   *
   *  @retval        m_max_cameras+SYSTEM_CAMERA_NOT_SET��Ԃ��B
   */
  //-----------------------------------------------------------------------------
  s32 GetNotSetCameraIndex(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[�����Ƀ��C�g�Z�b�g��ݒ肷��
   *
   *  @param[in]     light_set_index   �ݒ��ƂȂ郉�C�g�Z�b�g�̃C���f�b�N�X 
   *  @param[in]     light_set         ���C�g�Z�b�g�B
   *                                   �Ăяo�����͂��̃��C�g�Z�b�g��ێ��������ĉ������B
   *                                   light_set_index�ɐݒ肵�Ă��郉�C�g�Z�b�g���O���ꍇ��NULL��n���ĉ������B
   */
  //-----------------------------------------------------------------------------
  void SetLightSet(s32 light_set_index, nw::gfx::LightSet* light_set);

#if GFL_GRP_G3D_H3D_USE
  //-----------------------------------------------------------------------------
  /**
   *  @brief       h3d����gfx���������
   */
  //-----------------------------------------------------------------------------
  void SetH3dEnvironmentFromSceneEnvironment(const nw::gfx::Camera& camera);
#endif  // GFL_GRP_G3D_H3D_USE

  //-----------------------------------------------------------------------------
  /**
   *  @brief       �`���������������
   */
  //-----------------------------------------------------------------------------
  void ResetState(void)
  {
    m_render_context->ResetState();

    // �R���e�L�X�g��L�������܂�
    m_render_context->ActivateContext();
  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief        �����_�[�L���[�Ƀ����_�[�G�������g��ǉ�
   *
   *  @param[in]    camera         �����_�[�L���[�����Ɏg�p����J����
   *  @param[in]    scene_system   �`�悷��V�[���R���e���c���܂ރV�[���V�X�e��
   */
  //-----------------------------------------------------------------------------
  void BeginSubmitView(SceneDrawGroup* scene_draw_group);
  void SubmitViewModel(
      nw::gfx::Camera*       camera,
      SceneSystem*           scene_system,
      nw::gfx::Model*        model,
      SceneDrawGroup*        scene_draw_group );
#if GFL_GRP_G3D_H3D_USE
  void SubmitViewH3dModel(
      nw::gfx::Camera*       camera,
      SceneSystem*           scene_system,
      H3dModel*              model,
      SceneDrawGroup*        scene_draw_group );
#endif  // GFL_GRP_G3D_H3D_USE
  void SubmitView(
      nw::gfx::Camera*       camera,
      SceneSystem*           scene_system,
      SceneCalculateGroup*   scene_calculate_group,
      SceneDrawGroup*        scene_draw_group );
  void EndSubmitView(SceneDrawGroup* scene_draw_group);

  //-----------------------------------------------------------------------------
  /**
   *  @brief        �V�[����`��
   *
   *  @param[in]    camera              ���̃V�[����`�悷��J����
   *  @param[in]    command_cache_dump  �R�}���h�L���b�V�������O�o�͂���
   */
  //-----------------------------------------------------------------------------
  void RenderScene(
    gfl::grp::GraphicsSystem* graphics,
    gfl::grp::RenderTarget*  render_target,
    nw::gfx::Camera*         camera,
    SceneDrawGroup*          scene_draw_group,
    bool                     command_cache_dump = false);

  void RenderScene(
    //nw::gfx::IRenderTarget*  render_target,
    gfl::grp::RenderTarget*  render_target,
    nw::gfx::Camera*         camera,
    SceneDrawGroup*          scene_draw_group,
    bool                     command_cache_dump = false);

  //-----------------------------------------------------------------------------
  /**
   *  @brief        �X�e���I�f�B�X�v���C�ɑΉ������V�[���̕`��
   *                ���ʂɂ̂ݑΉ�
   * 
   *  @param[in]   
   *  @param[in]   
   *  @param[in]  
   */
  //-----------------------------------------------------------------------------
  void RenderStereoScene(
    gfl::grp::GraphicsSystem*     graphics_system,
    gfl::grp::RenderTarget*       render_target_left,
    nw::gfx::Camera*              camera_left,
    gfl::grp::RenderTarget*       render_target_right,
    nw::gfx::Camera*              camera_right,
    SceneDrawGroup*               scene_draw_group,
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
    nw::gfx::Camera*              camera_base,
#endif
    bool                          command_cache_dump = false);

#if 0
  //h3d�ŃT�|�[�g������Ȃ��̂ŃR�����g�A�E�g�B
  //-----------------------------------------------------------------------------
  /**
   *  @brief        �R�}���h�Z�[�u�������ăV�[���̕`��͂��Ȃ�
   */
  //-----------------------------------------------------------------------------
  void CommandSaveNotRenderScene(
    gfl::grp::GraphicsSystem*     graphics_system,
    nw::gfx::IRenderTarget*       render_target,
    SceneDrawGroup*               scene_draw_group,
    bool                          command_cache_dump = false);
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief        �R�}���h�Z�[�u���Ă�����̂��g���ăV�[���̕`�������
   */
  //-----------------------------------------------------------------------------
  void ReuseCommandSaveRenderScene(
    gfl::grp::GraphicsSystem*     graphics_system,
    gfl::grp::RenderTarget*       render_target,
    nw::gfx::Camera*              camera);
#endif

  //-----------------------------------------------------------------------------
  /**
   *  @brief        �{�b�N�X�e�X�g�@���ʃJ�E���^�̒l�N���A
   */
  //-----------------------------------------------------------------------------
  void ClearBoxTestCount( void );


#if GFL_GRP_G3D_H3D_USE
public:  // private�ɂ����ق���������
  inline H3dCommand* GetH3dCommand(void) const { return m_h3d_command; }
  inline H3dEnvironment* GetH3dEnvironment(void) const { return m_h3d_environment; }
  
  b32 IsFragmentAndWBufferDirty() const { return m_fragmentAndWBufferDirty; }

private:
  void createH3dSystem(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator);
  void destroyH3dSystem(void);
#endif  // GFL_GRP_G3D_H3D_USE

private:

    // gfx_SceneUpdater.h
    // gfx_SceneUpdater.cpp
    //     class SceneUpdater 
    //         void SubmitView
    // gfx_SceneUpdateHelper.h
    //     class SceneUpdateHelper
    //         class SubmitViewFunctor
    //             void operator() (Model* model)
    // ���Q�l�ɂ����B

    //! @brief        �J�����̎��E�Ɋ�Â��ăV�[�����X�V���A�`��L���[���\�z���܂��B
    //!
    //! @tparam       IsVisibleModelFunctor �\�����郂�f�������ʂ��邽�߂̊֐��I�u�W�F�N�g�̌^�ł��B
    //! @param[in]    skeletonUpdater �X�P���g���A�b�v�f�[�^�ł��B
    //! @param[in]    billboardUpdater �r���{�[�h�A�b�v�f�[�^�ł��B
    //! @param[in]    renderQueue �`��ΏۂƂȂ�v�f���W�߂��L���[�ł��B
    //! @param[in]    model ���f���ł��B
    //! @param[in]    camera �J�����ł��B
    //! @param[in]    layerId �`��v�f�̃\�[�g���ɍŗD��ɋ敪����� ID �ł��B���C���[��r���[�|�[�g�̕`��𐧌䂷��̂ɗp���܂��B
    //! @param[in]    particleLayerId �p�[�e�B�N���`��p�� layerId �ł��B
    //! @param[in]    isVisibleModel �\�����郂�f�������ʂ��邽�߂̊֐��I�u�W�F�N�g�ł��B
    //! @param[in]    renderSortMode �`��\�[�g���[�h�ł��B
    //!
    template<typename IsVisibleModelFunctor>
    static void SceneUpdateHelperSubmitViewModel(
        nw::gfx::SkeletonUpdater* skeletonUpdater,
        nw::gfx::BillboardUpdater* billboardUpdater,
        nw::gfx::RenderQueue* renderQueue,
        nw::gfx::Model* model,
        const nw::gfx::Camera& camera,
        u8 layerId,
        u8 particleLayerId,
        nw::gfx::ISceneUpdater::RenderSortMode renderSortMode,
        nw::gfx::ISceneUpdater::DepthSortMode depthSortMode,
        IsVisibleModelFunctor* isVisibleModel)
    {
        if( model == NULL ) return;
        
        if (depthSortMode == nw::gfx::ISceneUpdater::SORT_DEPTH_OF_TRANSLUCENT_MESH)
        {
            // �������̃��b�V���̂ݐ[�x�v�Z���s���܂��B
            // �s�����Ȃǂ̃��b�V���͐[�x�ɂ��\�[�g������Ȃ��Ȃ�܂����A
            // �[�x�v�Z����CPU�̏������ׂ��팸���܂��B
            
            typedef nw::gfx::SceneUpdateHelper::SubmitViewFunctor<
                nw::gfx::RenderQueue::FastEnqueueModelFunctor,
                nw::gfx::RenderQueue::FastEnqueueSkeletalModelFunctor,
                IsVisibleModelFunctor> FastMeshBaseSubmitViewFunctor;
            
            typedef nw::gfx::SceneUpdateHelper::SubmitViewFunctor<
                nw::gfx::RenderQueue::EnqueueModelTranslucentModelBaseFunctor,
                nw::gfx::RenderQueue::EnqueueSkeletalModelTranslucentModelBaseFunctor,
                IsVisibleModelFunctor> FastTranslucentModelBaseSubmitViewFunctor;
            
            if (renderSortMode == nw::gfx::ISceneUpdater::OPAQUE_MESH_BASE_AND_TRANSLUCENT_MODEL_BASE_SORT)
            {
                    FastTranslucentModelBaseSubmitViewFunctor(
                            skeletonUpdater, billboardUpdater,
                            renderQueue, &camera, layerId, particleLayerId, isVisibleModel)(model);
            }
            else
            {
                    FastMeshBaseSubmitViewFunctor(
                            skeletonUpdater, billboardUpdater,
                            renderQueue, &camera, layerId, particleLayerId, isVisibleModel)(model);
            }
        }
        else
        {
            // ���ׂẴ��b�V���̐[�x�v�Z���s���܂��B
            
            typedef nw::gfx::SceneUpdateHelper::SubmitViewFunctor<
                nw::gfx::RenderQueue::EnqueueModelFunctor,
                nw::gfx::RenderQueue::EnqueueSkeletalModelFunctor,
                IsVisibleModelFunctor> MeshBaseSubmitViewFunctor;
            
            typedef nw::gfx::SceneUpdateHelper::SubmitViewFunctor<
                nw::gfx::RenderQueue::EnqueueModelTranslucentModelBaseFunctor,
                nw::gfx::RenderQueue::EnqueueSkeletalModelTranslucentModelBaseFunctor,
                IsVisibleModelFunctor> TranslucentModelBaseSubmitViewFunctor;
            
            if (renderSortMode == nw::gfx::ISceneUpdater::OPAQUE_MESH_BASE_AND_TRANSLUCENT_MODEL_BASE_SORT)
            {
                    TranslucentModelBaseSubmitViewFunctor(
                            skeletonUpdater, billboardUpdater,
                            renderQueue, &camera, layerId, particleLayerId, isVisibleModel)(model);
            }
            else
            {
                    MeshBaseSubmitViewFunctor(
                            skeletonUpdater, billboardUpdater,
                            renderQueue, &camera, layerId, particleLayerId, isVisibleModel)(model);
            }
        }
    }


private:
  // �萔
  // �V�X�e���Ŏg���J����
  enum
  {
    // �J�����z��ɑ΂��Ċ��蓖�Ă�ꍇ�́Am_max_cameras����̃I�t�Z�b�g�Ƃ��Ĉ���
    SYSTEM_CAMERA_NOT_SET,  // �J�������J�����z��ɐݒ肵�Ă��Ȃ��Ƃ��Ɏg��
    SYSTEM_CAMERA_MAX
  };

private:
  nw::gfx::RenderContext*                 m_render_context;
  s32                                     m_max_cameras;                     // �J�����̍ő吔
  s32                                     m_max_cameras_plus_system_camera;  // m_max_cameras�ɃV�X�e���Ŏg���J����������������
  s32                                     m_max_light_sets;
  s32                                     m_max_fogs;
  s32                                     m_max_vertex_lights;
  nw::gfx::RenderKeyFactory*              m_prior_material_render_key_factory;
  nw::gfx::RenderKeyFactory*              m_prior_depth_render_key_factory;
  nw::gfx::MeshRenderer*                  m_mesh_renderer;
  ModelUserParameterCheckFunctor*         m_model_user_parameter_check_functor;
  nw::gfx::ISceneUpdater::RenderSortMode  m_render_sort_mode;
  nw::gfx::ISceneUpdater::DepthSortMode   m_depth_sort_mode;  // SORT_DEPTH_OF_TRANSLUCENT_MESH�̂Ƃ��A�������ȊO�̐[�x�\�[�g���ȗ�����B
  s32                                     m_render_mesh_count;  // �Ō�̃����_�����O�̃��b�V����
  b32                                     m_fragmentAndWBufferDirty;

#if GFL_GRP_G3D_H3D_USE
  H3dCommand*               m_h3d_command;
  H3dEnvironment*           m_h3d_environment;
#endif  // GFL_GRP_G3D_H3D_USE

#if GFL_GRP_G3D_H3D_USE
public:
  struct RenderSceneParam
  {
    enum ElementType
    {
      ELEMENT_TYPE_NONE,
      ELEMENT_TYPE_GFX,  // gfx��command��mesh
      ELEMENT_TYPE_H3D,
      ELEMENT_TYPE_MAX
    };
 
    gfl::grp::GraphicsSystem*               graphics_system;  // Stereo�`��ł����g��Ȃ�
    RenderSystem*                           render_system;
    gfl::grp::RenderTarget*                 render_target;
    nw::gfx::Camera*                        nw_camera;
    gfl::grp::RenderTarget*                 render_target_extension;  // Stereo�`��ł����g��Ȃ�
    nw::gfx::Camera*                        nw_camera_extension;  // Stereo�`��ł����g��Ȃ�
    SceneDrawGroup*                         scene_draw_group;
    nw::gfx::internal::CommandCacheBuilder* command_cache_builder;
    ElementType                             element_type;         
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
    gfl::math::MTX34                        view_mtx;
    gfl::math::MTX34                        inv_view_mtx;
    u32                                     inv_exist;
#endif

    RenderSceneParam(void)
    {
      graphics_system         = NULL;
      render_system           = NULL;
      render_target           = NULL;
      nw_camera               = NULL;
      render_target_extension = NULL;
      nw_camera_extension     = NULL;
      scene_draw_group        = NULL;
      command_cache_builder   = NULL;
      element_type            = ELEMENT_TYPE_NONE;
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
      inv_exist               = 0;
#endif
    }
  };

  static void BeginRenderScene( nw::gfx::RenderContext* render_context, RenderSceneParam* param );
  static void BeginGfxRenderScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param);
  static void EndGfxRenderScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param);
  static void BeginH3dRenderScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param);
  static void EndH3dRenderScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param);

  static void BeginGfxRenderStereoScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param);
  static void EndGfxRenderStereoScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param);
  static void BeginH3dRenderStereoScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param);
  static void EndH3dRenderStereoScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param);
  static void endRenderStereoScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param);
#endif  // GFL_GRP_G3D_H3D_USE
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DRENDERSYSTEM_H__
