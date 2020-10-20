#ifndef __GFL_G3D_DEPTHOFFIELSSYSTEM_H__
#define __GFL_G3D_DEPTHOFFIELSSYSTEM_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		G3DDepthOfFieldSystem.h
 *	@brief  被写界深度システム
 *	@author	Masateru Ishiguro
 *	@date		2011.06.28
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


// CTR_SDK

// NW4C
#include <nw/gfx.h>
#include <nw/gfx/gfx_Model.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>

//  gflib grp
#include <grp/gfl_RenderTarget.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dModel.h>
#include <grp/g3d/gfl_G3dCamera.h>
#include <grp/g3d/gfl_G3dShadowGroup.h>
#include <base/gfl_KeyContainer.h>


#define DECLARE_ENUM_BEGIN( CLASSNAME )                           \
	class CLASSNAME {                                               \
	public:                                                         \
		enum Enum {
#define DECLARE_ENUM_END( CLASSNAME )                             \
		};                                                            \
	public:                                                         \
		CLASSNAME(){}                                                 \
		explicit CLASSNAME( int value ) : value_( Enum( value ) ) {}	\
		CLASSNAME( Enum value ) : value_( value ) {}                  \
		operator Enum () const { return value_; }                     \
	private:                                                        \
		Enum value_;                                                  \
	};


namespace gfl {
namespace grp {
namespace g3d {
  
//! 被写界深度の種類
DECLARE_ENUM_BEGIN( DofType )
  Front = 0,    //! 手前ピンボケ
  Back,         //! 奥行きピンボケ
DECLARE_ENUM_END( DofType )  
  
//-------------------------------------
///	被写界深度システムクラス
//=====================================
class G3DDepthOfFieldSystem
{
  GFL_FORBID_COPY_AND_ASSIGN(G3DDepthOfFieldSystem);
  friend class Scene;
  
public:
  static const s32          DOF_LAYER_MAX = 4;
  
private:
  
  G3DDepthOfFieldSystem();
  ~G3DDepthOfFieldSystem();
  
  /**
   * @brief 被写界深度システムワーク確保
   * @param heap_allocator      メモリ確保先
   * @param device_allocator    メモリ確保先
   * @param p_desc              レンダーターゲット設定内容
   */
  void CreateWork(gfl::grp::GraphicsSystem* graphics_system, gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, gfl::grp::RenderTarget::Description* p_desc);

  /**
   * @brief 被写界深度ワーク破棄
   */
  void DestroyWork(void);
  
  /**
   * @brief 被写界深度のタイプ設定
   * @param type
   */
  inline void SetDofType( DofType::Enum type );
  
  /**
   * @brief フォーカス指定
   * @param focus　距離
   */
  inline void SetFocusLength( f32 focus );
  
  /**
   * @brielf    ぼやけレイヤー設定（最大３段階）
   * @param layer             ぼやけレイヤー番号
   * @param rangeDistance     影響範囲
   * @param blurred           ぼやけ具合( 最小0.0f ? 1.0f最大 )
   */
  void SetFocusRange( u32 layer, f32 rangeDistance, f32 blurred );
  
  /**
   * @brief   ぼやけ設定リセット
   */
  void ReSetFocusRange();
  
  /**
   * @broef ぼかし幅のスケールを設定（Frontに有効）
   * @param blurScale
   */
  inline void SetBlurScale( f32 blurScale );
  
  inline void DrawDepthOfField( const gfl::grp::DisplayType display_type, gfl::grp::GraphicsSystem* graphics_system, gfl::grp::FrameBufferManager* frame_buffer_manager, gfl::grp::g3d::Camera* pCamera );
  
  void MakeBlurImage( const gfl::grp::DisplayType display_type, gfl::grp::GraphicsSystem* graphics_system, gfl::grp::FrameBufferManager* frame_buffer_manager, gfl::grp::g3d::Camera* pCamera );
  void DrawBlurImage( const gfl::grp::DisplayType display_type, gfl::grp::GraphicsSystem* graphics_system, gfl::grp::FrameBufferManager* frame_buffer_manager, gfl::grp::g3d::Camera* pCamera );
  
  struct BlurLayerInfo{
    f32 rangeDistance;
    f32 blurred;
    b32 enable;
  };
  
  gfl::grp::RenderTarget*   m_render_target[DISPLAY_BOTH_MAX];
  gfl::grp::RenderTarget*   m_small_target[DISPLAY_BOTH_MAX];
  
  gfl::grp::GraphicsSystem  *m_graphics_system;
  void*                     m_pHalfImage;
  f32                       m_FocusLength;
  DofType::Enum             m_Type;
  BlurLayerInfo             m_LayerInfo[DOF_LAYER_MAX];
  f32                       m_BlurScale;
};

inline void G3DDepthOfFieldSystem::DrawDepthOfField( const gfl::grp::DisplayType display_type, gfl::grp::GraphicsSystem* graphics_system, gfl::grp::FrameBufferManager* frame_buffer_manager, gfl::grp::g3d::Camera* pCamera )
{
  MakeBlurImage( display_type, graphics_system, frame_buffer_manager, pCamera );
  DrawBlurImage( display_type, graphics_system, frame_buffer_manager, pCamera );
}

inline void G3DDepthOfFieldSystem::SetDofType( DofType::Enum type )
{
  m_Type = type;
}

inline void G3DDepthOfFieldSystem::SetFocusLength( f32 focus )
{
  m_FocusLength = focus;
}

inline void G3DDepthOfFieldSystem::SetBlurScale( f32 blurScale )
{
  m_BlurScale = blurScale;
}


#undef DECLARE_ENUM_BEGIN
#undef DECLARE_ENUM_END

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DEDGEMAPSYSTEM_H__
