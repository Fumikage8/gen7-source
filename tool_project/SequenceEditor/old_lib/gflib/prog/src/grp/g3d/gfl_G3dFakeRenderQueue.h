#pragma once
#if !defined( __GFL_G3DFAKERENDERQUEUE_H__ )
#define __GFL_G3DFAKERENDERQUEUE_H__

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dFakeRenderQueue.h
 *	@brief  3D�U�����_�[�L���[
 *	@author	Koji Kawada
 *	@date		2012.07.20
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>


namespace gfl {
namespace grp {
namespace g3d {


/**
 *  @brief  g3d����J�L�q
 */
namespace internal {

// nw::gfx::RenderQueue��private�����o�ϐ�m_List�փA�N�Z�X���邽�߂̃N���X

// nw::gfx::RenderQueue�ƃI�u�W�F�N�g�̃����o�ϐ��̃��C�A�E�g�������N���X��p�ӂ�
//     nw::gfx::RenderQueue* render_queue_A = m_scene_draw_group_array[scene_draw_group_index_A]->GetRenderQueue();
//     nw::gfx::RenderQueue::ElementListType* list_A = (reinterpret_cast<FakeRenderQueue*>(render_queue_A))->GetList();
// �Ƃ����ӂ��ɃL���X�g���Ďg�p����B
class FakeRenderQueue
{
public:
  FakeRenderQueue(void)
    : m_List()
  {}
  virtual ~FakeRenderQueue()
  {}

  nw::gfx::RenderQueue::ElementListType* GetList(void)
  {
    return &m_List;
  }

private:
  u8 dummy[4];  // nw::gfx::GfxObject�̃����o�ϐ��̍��v�o�C�g�����m�ۂ���B
                // u8 dummy[sizeof(nw::gfx::GfxObject)];�ł̓_���B
                // nw::gfx::GfxObject��virtual�����o�֐�������̂ŁA�|�C���^vptr�����݂��A���̕���4�o�C�g���]�v�Ɏ����̂ŁB
                // vptr�̕��̃o�C�g��FakeRenderQueue��virtual�����o�֐��������Ă��肻��Ŏ���Ă���̂ŁA
                // nw::gfx::GfxObject�̔h���N���X�ł���nw::gfx::RenderQueue�������Ă���vptr�̕��̃o�C�g�ƁA����ň�v����B
  
  nw::gfx::RenderQueue::ElementListType m_List;
public:
  void* m_OpacityKeyFactory;      // sizeof��nw::gfx::RenderQueue�̕ύX�ɋC�t����悤�ɁAnw::gfx::RenderQueue�Ɠ��������o�ϐ��������Ă������Ƃɂ���B
  void* m_TranslucentKeyFactory;
  void* m_AdditiveKeyFactory;
  void* m_SubtractionKeyFactory;

  enum KeyCachingState
  {
      UNUSE_CACHED_KEY,
      USE_CACHED_KEY
  };
  
  KeyCachingState m_KeyCachingState;
};

#if 0
/*����
(1) offsetof�ɂ���
offsetof�͂�����define�}�N���Ȃ̂ŁAprivate�����o�ϐ��ւ̃A�N�Z�X�ɂ͎g���Ȃ��B

(2) �^�̃T�C�Y�𒲂ׂ�����
GFL_PRINT("size_t=%d, nw::os::IAllocator&=%d, nw::gfx::GfxObject=%d\n", sizeof(size_t), sizeof(nw::os::IAllocator&), sizeof(nw::gfx::GfxObject));
�o�͌���
size_t=4, nw::os::IAllocator&=4, nw::gfx::GfxObject=8*/
#endif

}  // namespace internal


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DFAKERENDERQUEUE_H__
