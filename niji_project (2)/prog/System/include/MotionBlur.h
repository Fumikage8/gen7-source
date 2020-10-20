
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		MotionBlur.h
 *	@brief  モーションブラー
 *	@author	tomoya takahashi
 *	@date		2014.03.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __MOTIONBLUR_H__
#define __MOTIONBLUR_H__
#pragma once


#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <model/include/gfl2_BaseModel.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>
#include <gfx/include/gfl2_types.h>
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNode.h>

#include "System/include/nijiAllocator.h"

#include <Fade/include/gfl2_FadeManager.h>

namespace System
{

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
class MotionBlur : public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
  GFL_FORBID_COPY_AND_ASSIGN(MotionBlur); //コピーコンストラクタ＋代入禁止

public:

  enum BlendType
  {
    BLEND_PLANE,
    BLEND_CIRCLE,
    BLEND_NEAR,
    BLEND_MAX,
  };
  enum AnimeType
  {
    ANIM_NONE,
    ANIM_SCALE,
    ANIM_MAX,
  };

public:

  // コンストラクタ・デストラクタ
  MotionBlur(void);
  virtual ~MotionBlur();

  /**
   *  @brief  初期化 (SceneRenderPath::Initializeを実行する必要はありません。)
   *  @param  p_device    デバイスヒープ
   *  @param  p_fileread  ファイルリードマネージャー
   *  @param  useTextureTarget  テンポラリのカラーバッファとして使用するFadeManagerの面(gfl2::Fade::FadeManager::COLOR_BUFFER_UPPER or  gfl2::Fade::FadeManager::COLOR_BUFFER_LOWER)
   */
  void InitializeMotionBlur( gfl2::gfx::IGLAllocator*  p_deviceAllocator, gfl2::heap::HeapBase* p_device, gfl2::fs::AsyncFileManager* p_fileread, u32 useTextureTarget );
  bool InitializeMotionBlurUpdate( void );
  // 破棄
  void Terminate(void);


  // 生成ずみか？
  b32 IsInitialized(void) const { return m_IsInitialized; }

  // 描画
  virtual void Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
  virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

  // 
  //----------------------------------------------------------------------------
  /**
   *	@brief  表示ON
   *	@param	blend       ブレンドタイプ
   *	@param	anime       アニメーションタイプ
   *	@param	cpTargetSurface 描画ターゲットとなるサーフェース
   *  @param  is_Loop     ループ再生するか？
   */
  //-----------------------------------------------------------------------------
  void RenderStart( BlendType blend, AnimeType anime, const gfl2::gfx::Surface* cpTargetSurface, bool is_loop );

  //----------------------------------------------------------------------------
  /**
   *	@brief    アニメーションの完了を待つ
   *
   *	@retval true  完了
   *	@retval false 途中
   */
  //-----------------------------------------------------------------------------
  b32 IsAnimeEnd( void );

  // 表示OFFかチェック
  bool IsRenderOff(void);

  // 表示OFF (Finalizeの1フレーム前に必ずコール）
  void RenderOff(void);


  //----------------------------------------------------------------------------
  /**
   *	ブレンド比をコントロール
   */
  //-----------------------------------------------------------------------------
  enum Type
  {
    TYPE_SET,     // endの値をセットする
    TYPE_LINEAR,  // 線形補間
    TYPE_SPLINE,  // スプライン補間
    TYPE_QUAD,     // 2次関数グラフで補間
    TYPE_LOG10,   // 常用対数で補間
    TYPE_END, 
  };

  struct Param
  {
    Type type;    // 補間タイプ
    f32 end;      // 終了値 -0.5 ～ +0.5
    s32 frame;    // アニメーションフレーム　SETのときは1を入れてね
  };
  //----------------------------------------------------------------------------
  /**
   *	@brief  ブレンド比をコントロールする。 
   */
  //-----------------------------------------------------------------------------
  void SetBlendControl( const Param& param );
  bool IsEndBlendControl( void ) const; // ブレンド比のアニメーション完了チェック
  f32 GetBlendControl( void ) const;    // 今のブレンド比を取得

  //----------------------------------------------------------------------------
  /**
   *	@brief  カラーfilterをかける/コントロールする。 
   */
  //-----------------------------------------------------------------------------
  struct ColorParam
  {
    Type type;    // 補間タイプ
    gfl2::math::Vector3 end;      // 色の加算
    s32 frame;    // アニメーションフレーム　SETのときは1を入れてね
  };
  void SetColorFilter( const ColorParam& param );
  bool IsEndColorFilter( void ) const; // ブレンド比のアニメーション完了チェック

private:


  // アニメーション更新処理
   void UpdateMotionBlur( void );

	virtual const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride();
	virtual const gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* CameraOverride(void);
  // コマンドの再利用モード(使用できない）
	virtual void CallDisplayList( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

  // クリアレンダーターゲット
	virtual void ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );

  bool IsRendering( gfl2::gfx::CtrDisplayNo type ) const;

  // ブレンド比コントロール
  void BlendControl(void);
  void ColorControl(void);
  // 各タイプの関数
  static f32 CalcSet( f32 end, f32 start, f32 now, s32 frame, s32 frame_max );
  static f32 CalcLinear( f32 end, f32 start, f32 now, s32 frame, s32 frame_max );
  static f32 CalcSpline( f32 end, f32 start, f32 now, s32 frame, s32 frame_max );
  static f32 CalcQuad( f32 end, f32 start, f32 now, s32 frame, s32 frame_max );
  static f32 CalcLog10( f32 end, f32 start, f32 now, s32 frame, s32 frame_max );


private:

  gfl2::gfx::IGLAllocator* m_pDevice;
  gfl2::heap::HeapBase* m_pDeviceHeap;

  gfl2::fs::AsyncFileManager * m_pFileReadManager;

  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera m_camera;

  bool m_RenderOff;
  BlendType m_BlendType;
  AnimeType m_AnimeType;
  const gfl2::gfx::Surface* m_cpTargetSurface;
  // GFSBTS[1683] : 立体視時に画像がずれる。
  b32    m_DrawMsk; // 描画マスク
  bool m_IsLoop;
  bool m_IsInitialized;   ///< 初期化済みフラグ
  bool m_IsSceneRenderPathInitialized; ///< シーンレンダリングパスが生成済みか？ 

  // 1つ前のレンダリング結果　保持テクスチャ
  u32 m_UseTextureTarget;
  gfl2::gfx::Texture*  m_pOldRenderBuffer;
  gfl2::renderingengine::scenegraph::resource::TextureResourceNode* m_pOldRenderBufferResource;

  // モーションブラーマテリアル
  void * m_pResourceBuff;
  gfl2::fs::BinLinkerAccessor m_BinLinker;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pResource;
  gfl2::renderingengine::scenegraph::instance::TransformNode                  *m_node;
  poke_3d::model::BaseModel* m_pModel;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode  * m_pModelInstanceNode;
  gfl2::renderingengine::scenegraph::instance::DrawableNode  *      m_pModelInstanceNodeDrawableNode;
	gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>	m_DrawableNodeContainer;


  gfl2::animation::AnimationPackList m_AnimPackList;

  // ブレンド比コントロール値アニメーション
  Param m_BlendControlParam;
  f32   m_BlendControlNowBlend;
  f32   m_BlendControlStartBlend;
  s32   m_BlendControlFrame;

  // カラーコントロール値アニメーション
  ColorParam m_ColorControlParam;
  gfl2::math::Vector3   m_ColorControlNowColor;
  gfl2::math::Vector3   m_ColorControlStartColor;
  s32   m_ColorControlFrame;

  // アニメーションインデックス
  static const u32 BlendAnimationIndex[ BLEND_MAX ];
  static const u32 AnimationIndex[ ANIM_MAX ];

};
  

} // namespace System

#endif	// __MOTIONBLUR_H__


