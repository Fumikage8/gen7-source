#ifndef __APP_RENDERING_PIPELINE_H__
#define __APP_RENDERING_PIPELINE_H__
//===================================================================
/**
 * @file    AppRenderingPipeline.h
 * @brief   アプリレンダリングパイプライン
 * @author  k.ohno
 */
//===================================================================
#pragma once


#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <Layout/include/gfl2_LayoutRenderPath.h>


GFL_NAMESPACE_BEGIN(gfl2) 
GFL_NAMESPACE_BEGIN(util)
class GLHeapAllocator;
GFL_NAMESPACE_END(util) 
GFL_NAMESPACE_END(gfl2) 



class AppRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
public:

  
	//----------------------------------------------------------------------------
	/**
	*  @brief  コンストラクタ
	*/
	//-----------------------------------------------------------------------------
	AppRenderingPipeLine( gfl2::heap::HeapBase* heap, f32 screanWidth, f32 screanHeight, u32 lytWkMax = 32 );
	
	//----------------------------------------------------------------------------
	/**
	*  @brief  デストラクタ
	*/
	//-----------------------------------------------------------------------------
	virtual ~AppRenderingPipeLine();

	//----------------------------------------------------------------------------
	/**
	*  @brief  最初のパスから描画できる状態にする
	*/
	//-----------------------------------------------------------------------------
	virtual b32 StartRenderPath(void);

	//----------------------------------------------------------------------------
	/**
	*  @brief  現状のパスにあったシーンを返す
	*  @return シーン
	*/
	//-----------------------------------------------------------------------------
	virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath(void);

	//----------------------------------------------------------------------------
	/**
	*  @brief  次のパスへ遷移
	*  @return trueなら次のパス、falseなら描画終了
	*/
	//-----------------------------------------------------------------------------
	virtual b32 NextRenderPath(void);

public:

	//----------------------------------------------------------------------------
	/**
	*  @brief  レイアウトのレンダーパスを取得する
	*/
	//-----------------------------------------------------------------------------
	gfl2::lyt::LayoutRenderPath* GetLayoutRenderPath( void ) { return m_LayoutRenderPath; }



private:

  static const int APP_MAX_PATH_NUM = 1;  //一個
      
  gfl2::util::GLHeapAllocator*  m_pGLHeapAllocator;

  u32                           m_RenderPathCnt;
  gfl2::lyt::LayoutRenderPath*  m_LayoutRenderPath;
};

#endif
