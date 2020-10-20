#pragma once
#if !defined( __GFL_G3DFAKERENDERQUEUE_H__ )
#define __GFL_G3DFAKERENDERQUEUE_H__

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dFakeRenderQueue.h
 *	@brief  3D偽レンダーキュー
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
 *  @brief  g3d非公開記述
 */
namespace internal {

// nw::gfx::RenderQueueのprivateメンバ変数m_Listへアクセスするためのクラス

// nw::gfx::RenderQueueとオブジェクトのメンバ変数のレイアウトが同じクラスを用意し
//     nw::gfx::RenderQueue* render_queue_A = m_scene_draw_group_array[scene_draw_group_index_A]->GetRenderQueue();
//     nw::gfx::RenderQueue::ElementListType* list_A = (reinterpret_cast<FakeRenderQueue*>(render_queue_A))->GetList();
// というふうにキャストして使用する。
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
  u8 dummy[4];  // nw::gfx::GfxObjectのメンバ変数の合計バイト数分確保する。
                // u8 dummy[sizeof(nw::gfx::GfxObject)];ではダメ。
                // nw::gfx::GfxObjectにvirtualメンバ関数があるので、ポインタvptrが存在し、その分の4バイトが余計に取られるので。
                // vptrの分のバイトはFakeRenderQueueがvirtualメンバ関数を持っておりそれで取っているので、
                // nw::gfx::GfxObjectの派生クラスであるnw::gfx::RenderQueueが持っているvptrの分のバイトと、それで一致する。
  
  nw::gfx::RenderQueue::ElementListType m_List;
public:
  void* m_OpacityKeyFactory;      // sizeofでnw::gfx::RenderQueueの変更に気付けるように、nw::gfx::RenderQueueと同じメンバ変数を持っておくことにする。
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
/*メモ
(1) offsetofについて
offsetofはただのdefineマクロなので、privateメンバ変数へのアクセスには使えない。

(2) 型のサイズを調べた結果
GFL_PRINT("size_t=%d, nw::os::IAllocator&=%d, nw::gfx::GfxObject=%d\n", sizeof(size_t), sizeof(nw::os::IAllocator&), sizeof(nw::gfx::GfxObject));
出力結果
size_t=4, nw::os::IAllocator&=4, nw::gfx::GfxObject=8*/
#endif

}  // namespace internal


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DFAKERENDERQUEUE_H__
