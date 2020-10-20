#if !defined(NIJI_PROJECT_APPLIB_UTIL_APPOUTLINEPOSTRENDERPATH_H_INCLUDED)
#define NIJI_PROJECT_APPLIB_UTIL_APPOUTLINEPOSTRENDERPATH_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    AppOutLinePostRenderPath.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2016.01.29
 @brief   AppOutLinePostRenderPath
 */
//==============================================================================

// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>


// nijiのインクルード
#include <System/include/nijiAllocator.h>
#include <System/include/RenderPath/OutLinePostRenderPath.h>




#define NIJI_APPLIB_UTIL_APP_OUT_LINE_POST_RENDER_PATH_DEBUG_TEX_SRT (0)  // テクスチャのSRT調整を行う際は1。必ず0でコミットするように！




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)


//! @brief  AppRenderingManagerで使うポケモンモデルレンダリングパイプラインで使うOutLinePostRenderPath
class AppOutLinePostRenderPath : public System::NijiOutLinePostSceneRenderPath
{
  GFL_FORBID_COPY_AND_ASSIGN(AppOutLinePostRenderPath);

public:
  AppOutLinePostRenderPath(gfl2::gfx::IGLAllocator *allocator);
  virtual ~AppOutLinePostRenderPath();

private:
  virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

private:
#if NIJI_APPLIB_UTIL_APP_OUT_LINE_POST_RENDER_PATH_DEBUG_TEX_SRT
  void debugControlTexSRT(void);
#endif  // NIJI_APPLIB_UTIL_APP_OUT_LINE_POST_RENDER_PATH_DEBUG_TEX_SRT

private:
  static const u32 TEX_NUM = 3;

#if NIJI_APPLIB_UTIL_APP_OUT_LINE_POST_RENDER_PATH_DEBUG_TEX_SRT
  bool                m_debugTexTranslateOriginalValGetFlag[TEX_NUM];
  gfl2::math::Vector2 m_debugTexTranslateOriginalVal[TEX_NUM];
  gfl2::math::Vector2 m_debugTexScaleVal;
  gfl2::math::Vector2 m_debugTexTranslateVal;
#endif  // NIJI_APPLIB_UTIL_APP_OUT_LINE_POST_RENDER_PATH_DEBUG_TEX_SRT
};


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APPLIB_UTIL_APPOUTLINEPOSTRENDERPATH_H_INCLUDED

