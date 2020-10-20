#ifndef __GFL_G3DSCENEENVIRONMENTSETTING_H__
#define __GFL_G3DSCENEENVIRONMENTSETTING_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dSceneEnvironmentSetting.h
 *	@brief  3D�V�[�����ݒ�
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dObject.h>
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
///	3D�V�[�����ݒ�N���X
//=====================================
class SceneEnvironmentSetting : public Object
{
  GFL_FORBID_COPY_AND_ASSIGN(SceneEnvironmentSetting);

public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   *
   *  @param[in]     allocator          �A���P�[�^
   *  @param[in]     device_allocator   �f�o�C�X�������̃A���P�[�^
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
   *  @brief         �j��
   *                 ����Ă�ł����v�B�f�X�g���N�^�͌Ă΂�܂���B 
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���̃N���X���ێ����Ă���nw::gfx::SceneEnvironmentSetting*���擾
   *
   *  @retval        nw::gfx::SceneEnvironmentSetting*    ���̃N���X���ێ����Ă���nw::gfx::SceneEnvironmentSetting
   */
  //-----------------------------------------------------------------------------
  nw::gfx::SceneEnvironmentSetting* GetNwSceneEnvironmentSetting(void)  {  return m_nw_scene_environment_setting;  }

public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   *                 Create�͌Ă΂�܂���B
   */
  //-----------------------------------------------------------------------------
  SceneEnvironmentSetting(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   *                 Destroy���Ăт܂��B
   */
  //-----------------------------------------------------------------------------
  virtual ~SceneEnvironmentSetting();

protected:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���̃N���X�ŕێ�����悤nw::gfx::SceneEnvironmentSetting*��ݒ�
   *
   *  @param[in]     nw_scene_environment_setting    �V�[�����ݒ�
   */
  //-----------------------------------------------------------------------------
  void SetNwSceneEnvironmentSetting(nw::gfx::SceneEnvironmentSetting* nw_scene_environment_setting)
  {
    m_nw_scene_environment_setting = nw_scene_environment_setting;
  }

private:
  nw::gfx::SceneEnvironmentSetting* m_nw_scene_environment_setting;
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DSCENEENVIRONMENTSETTING_H__
