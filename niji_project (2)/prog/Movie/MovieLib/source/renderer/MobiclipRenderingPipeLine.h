#if !defined(NIJI_PROJECT_MOVIE_RENDERER_MOBICLIPRENDERINGPIPELINE_H_INCLUDED)
#define NIJI_PROJECT_MOVIE_RENDERER_MOBICLIPRENDERINGPIPELINE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    MobiclipRenderingPipeLine.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.10.18
 @brief   mobiclip用の描画パイプライン
 @note    gf内製コード。mobiclipとは無関係。
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>

// nijiのインクルード
#include "MobiclipRenderPath.h"




GFL_NAMESPACE_BEGIN(movie)
GFL_NAMESPACE_BEGIN(renderer)


//==============================================================================
/**
 @class     MobiclipRenderingPipeLine
 @brief     mobiclip用の描画パイプライン
 */
//==============================================================================
class MobiclipRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
  GFL_FORBID_COPY_AND_ASSIGN(MobiclipRenderingPipeLine);

public:
  MobiclipRenderingPipeLine(void)
    : gfl2::renderingengine::renderer::RenderingPipeLine(),
      m_RenderPathCnt(0), m_TargetRenderPath(NULL)
  {}

  virtual ~MobiclipRenderingPipeLine()
  {}

private:
  virtual b32 StartRenderPath()
  {
    m_RenderPathCnt = 0;
    return true;
  }

  virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath()
  {
    return m_TargetRenderPath;
  }

  virtual b32 NextRenderPath()
  {
    m_RenderPathCnt++;
    if ( m_RenderPathCnt == 1 )
      return false;
    return true;
  }

public:
  MobiclipRenderPath* GetMobiclipRenderPath(void)
  {
    return m_MobiclipRenderPath;
  }

  int NumMobiclipRenderPath()
  {
    return GFL_NELEMS(m_MobiclipRenderPath);
  }

  void SetDrawBuffer( gfl2::gfx::CtrDisplayNo   displayNo )
  {
    m_TargetRenderPath = NULL;
    switch(displayNo.Get())
    {
    case gfl2::gfx::CtrDisplayNo::LEFT:
      m_TargetRenderPath = &m_MobiclipRenderPath[0];
      break;
    case gfl2::gfx::CtrDisplayNo::DOWN:
      m_TargetRenderPath = &m_MobiclipRenderPath[1];
      break;
    }


  }

public:
  //! @brief  表示する絵が描かれたバッファを設定する
  void SetBufForTexture(
      gfl2::gfx::CtrDisplayNo   displayNo,
      gfl2::gfx::IGLAllocator*  glAllocator,     // デバイスメモリのglアロケータ
      const void*               bufForTexture,   // バッファのアドレス
      gfl2::gfx::Pool           pool,            // メモリ配置
      u32                       width,           // 幅
      u32                       height,          // 高さ
      gfl2::gfx::Format         format,          // ピクセルフォーマット
      float                     imageClockwiseDegreeRotationZ
  )
  {
    switch(displayNo.Get())
    {
    case gfl2::gfx::CtrDisplayNo::LEFT:
      m_MobiclipRenderPath[0].SetBufForTexture(
        glAllocator,    
        bufForTexture,  
        pool,           
        width,          
        height,         
        format,
        imageClockwiseDegreeRotationZ
      );
      break;
    case gfl2::gfx::CtrDisplayNo::DOWN:
      m_MobiclipRenderPath[1].SetBufForTexture(
        glAllocator,    
        bufForTexture,  
        pool,           
        width,          
        height,         
        format,
        imageClockwiseDegreeRotationZ
      );
      break;
    }
  }
  //! @brief  表示する絵が描かれたバッファを解除する
  void UnsetBufForTexture(gfl2::gfx::CtrDisplayNo   displayNo)
  {
    switch(displayNo.Get())
    {
    case gfl2::gfx::CtrDisplayNo::LEFT:
      m_MobiclipRenderPath[0].UnsetBufForTexture();
      break;
    case gfl2::gfx::CtrDisplayNo::DOWN:
      m_MobiclipRenderPath[1].UnsetBufForTexture();
      break;
    }
  }

private:
  MobiclipRenderPath  m_MobiclipRenderPath[2]; //上下で２つ
  u32                 m_RenderPathCnt;
  MobiclipRenderPath* m_TargetRenderPath;
};


GFL_NAMESPACE_END(renderer)
GFL_NAMESPACE_END(movie)


#endif  // NIJI_PROJECT_MOVIE_RENDERER_MOBICLIPRENDERINGPIPELINE_H_INCLUDED

