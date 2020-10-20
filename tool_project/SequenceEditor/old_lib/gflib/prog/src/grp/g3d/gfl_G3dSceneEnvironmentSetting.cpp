//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dSceneEnvironmentSetting.cpp
 *	@brief  3D�V�[�����ݒ�
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
#include <grp/g3d/gfl_G3dSystem.h>
#include <grp/g3d/gfl_G3dResource.h>

#include <grp/g3d/gfl_G3dSceneEnvironmentSetting.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X��`
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3D�V�[�����ݒ�N���X
//=====================================

//-----------------------------------------------------------------------------
/*
 *  @brief         ����
 *
 *  @param[in]     allocator          �A���P�[�^
 *  @param[in]     allocator          �f�o�C�X�������̃A���P�[�^
 *  @param[in]     resource           ���\�[�X
 *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void SceneEnvironmentSetting::Create(
    gfl::heap::NwAllocator*           allocator, 
    gfl::heap::NwAllocator*           device_allocator, 
    Resource*                         resource,
    s32                               index_in_resource
)
{
  if( resource == NULL )
  {
    GFL_ASSERT_MSG( 0, "���\�[�X���s���̂��߃V�[���I�u�W�F�N�g�������ł��܂���ł����B\n" );
    return;
  }

  nw::gfx::ResGraphicsFile* nw_res_graphics_file = resource->GetNwResGraphicsFile();
  nw::gfx::ResSceneEnvironmentSetting res = nw_res_graphics_file->GetSceneEnvironmentSettings(index_in_resource);
  nw::gfx::SceneObject* scene_object = gfl::grp::g3d::Resource::CreateNwSceneObject(
      allocator,
      device_allocator,
      res,
      false,  // �A�j���[�V�����Ȃ�
      nw::gfx::Model::FLAG_BUFFER_NOT_USE,
      0
  );
  GFL_ASSERT_MSG( scene_object, "index_in_resource=%d�̃V�[���I�u�W�F�N�g�������ł��܂���ł����B\n", index_in_resource );

  nw::gfx::SceneEnvironmentSetting* scene_environment_setting =
      nw::ut::DynamicCast<nw::gfx::SceneEnvironmentSetting*>(scene_object);
  SetNwSceneEnvironmentSetting(scene_environment_setting);
  SetResourceIdAndIndexInResource(resource->GetId(), index_in_resource);
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �j��
 *                 ����Ă�ł����v�B�f�X�g���N�^�͌Ă΂�܂���B 
 */
//-----------------------------------------------------------------------------
void SceneEnvironmentSetting::Destroy(void)
{
  nw::gfx::SafeDestroy(m_nw_scene_environment_setting);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �R���X�g���N�^
 *                 Create�͌Ă΂�܂���B
 */
//-----------------------------------------------------------------------------
SceneEnvironmentSetting::SceneEnvironmentSetting(void)
  : m_nw_scene_environment_setting(NULL)
{
  GFL_SINGLETON_INSTANCE(System)->GetSceneEnvironmentSettingManager()->AddObject(this);
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �f�X�g���N�^
 *                 Destroy���Ăт܂��B
 */
//-----------------------------------------------------------------------------
SceneEnvironmentSetting::~SceneEnvironmentSetting()
{
  Destroy();
  GFL_SINGLETON_INSTANCE(System)->GetSceneEnvironmentSettingManager()->RemoveObject(this);
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

