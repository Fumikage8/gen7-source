#ifndef GFLIB2_UTIL_UTILTEXTRENDERINGPIPELINE_H_INCLUDED
#define GFLIB2_UTIL_UTILTEXTRENDERINGPIPELINE_H_INCLUDED


//gflib2
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <util/include/gfl2_UtilTextRenderPath.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(util)

class UtilTextRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
public:

  //----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  UtilTextRenderingPipeLine( gfl2::gfx::IGLAllocator* allocator, f32 screanWidth, f32 screanHeight );
  
  //----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~UtilTextRenderingPipeLine();

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
  *  @brief  DrawUtilTextのレンダーパスを取得する
  */
  //-----------------------------------------------------------------------------
  gfl2::util::UtilTextRenderPath* GetUtilTextRenderPath( void ) { return &m_UtilTextRenderPath; }

private:
  u32                                    m_RenderPathCnt;
  gfl2::util::UtilTextRenderPath        m_UtilTextRenderPath;
};

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gfl2)

#endif  // GFLIB2_UTIL_UTILTEXTRENDERINGPIPELINE_H_INCLUDED
