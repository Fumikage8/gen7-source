//===========================================================================
/**
 * @file    gfl2_DrawTag.h
 * @brief   描画単位オブジェクト
 * @author  GAMEFREAK inc.
 * @author  ishiguro_masateru
 * @date    2014

 DrawManagerが扱うatomicな描画単位の仮想クラス。
 DrawableNode（描画可能Node)は必ずDrawTagを継承したクラスのオブジェクトを保持し、それを扱うAPIを持つ。
 */
//===========================================================================
#pragma once
#ifndef GFLIB2_RENDERINGENGINE_RENDERER_DRAWTAG_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_RENDERER_DRAWTAG_H_INCLUDED


#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_GLMemory.h>
#include <RenderingEngine/include/renderer/gfl2_RenderState.h>

namespace gfl2 { namespace renderingengine { namespace renderer { 

class DrawTag
{
public:

  //!< @brief レンダーキー(描画順のソートに使う情報)
  union RenderKey
  {
    struct
    {
      u64 value;              //!< @brief ソートに使用する値
    };
    struct
    {
      // 下に行くほど優先度が高い情報となる
      u64 materialHash : 24;  //!< @brief マテリアルのハッシュ
      u64 depth : 24;         //!< @brief 深度情報
//      u64 reserved : 1;       //!< @brief 予約領域(1ビットの空き領域, 拡張用)
      u64 priority : 5;       //!< @brief 優先度(sangoまで0〜7, 念のため1ビットの空き領域を確保)
      u64 drawLayer : 3;          //!< @brief 描画レイヤー(デザイナーが指定する優先順位)
      u64 programmableLayer : 8;  //!< @brief プログラマレイヤー(プログラマーが指定する優先順位)
    };
    enum {
      PRIORITY_MARGIN = 8       //!< プライオリティに対してコード側から操作する余地
    };
  };

	class IDrawListener
	{
	public:
		virtual void PreDraw(RenderState&) = 0;
		virtual void PostDraw(RenderState&) = 0;

		virtual ~IDrawListener(){}
	protected:
		IDrawListener(){}
	};

	virtual ~DrawTag();

  inline void SetVisible(bool isVisible) { m_IsVisible = isVisible; }
  inline bool IsVisible() { return m_IsVisible; }

	inline void SetWorldMatrix( const math::Matrix34 *pmat ){ GFL_ASSERT(pmat);m_pWorldMatrix = pmat; }
	inline const math::Matrix34* GetWorldMatrix() const { return m_pWorldMatrix; }

	inline void SetCullingAABB( const math::AABB* aabb ) { m_pWorldAABB = aabb; }
	inline const math::AABB* GetCullingWorldAABB() const { GFL_ASSERT(m_pWorldAABB);return m_pWorldAABB; }

	inline u32 GetDrawLayerNo() const { return m_DrawLayerNo; }
	inline s32 GetLightSetNo() const { return m_LightSetNo; }
  inline void SetDrawLayerNo(const u32 no) {  m_DrawLayerNo = no; }
	inline void SetLightSetNo(const u32 no) { m_LightSetNo = no; }

  inline void SetRenderKey(const u64 renderKey) { m_RenderKey = renderKey; }
  inline u64 GetRenderKey() const { return m_RenderKey; }

	inline void SetDrawListener( IDrawListener *pObj ){ m_pDrawListener = pObj; }
	inline const IDrawListener* GetDrawListener() const { return m_pDrawListener; }

	inline void PreDraw(RenderState &state){ if ( m_pDrawListener ) m_pDrawListener->PreDraw(state); }
	virtual void Draw(RenderState&)=0;
	inline void PostDraw(RenderState &state){ if ( m_pDrawListener ) m_pDrawListener->PostDraw(state); }

protected:
	DrawTag();

  bool                      m_IsVisible;
	u32												m_DrawLayerNo;
	s32												m_LightSetNo;
	const math::Matrix34			*m_pWorldMatrix;
	const math::AABB					*m_pWorldAABB;
  u64                       m_RenderKey;      //!< @brief ソート用のレンダーキー
	IDrawListener							*m_pDrawListener;
};

}}}

#endif

