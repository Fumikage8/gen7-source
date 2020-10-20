#include "../include/gfl2_UtilTextRenderingPipeLine.h"

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(util)

//----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
UtilTextRenderingPipeLine::UtilTextRenderingPipeLine( gfl2::gfx::IGLAllocator* allocator, f32 screanWidth, f32 screanHeight )
  : m_RenderPathCnt(0),
    m_UtilTextRenderPath(allocator)
{

}

//----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
UtilTextRenderingPipeLine::~UtilTextRenderingPipeLine(void)
{
}

//----------------------------------------------------------------------------
/**
*  @brief  最初のパスから描画できる状態にする
*/
//-----------------------------------------------------------------------------
b32 UtilTextRenderingPipeLine::StartRenderPath(void)
{
  m_RenderPathCnt = 0;
  return true;
}

//----------------------------------------------------------------------------
/**
*  @brief  現状のパスにあったシーンを返す
*  @return シーン
*/
//-----------------------------------------------------------------------------
RenderingPipeLine::IRenderPath* UtilTextRenderingPipeLine::GetRenderPath(void)
{
  switch( m_RenderPathCnt )
  {
  case 0:
    return &m_UtilTextRenderPath;
//    break;
  }
  return NULL;
}

//----------------------------------------------------------------------------
/**
*  @brief  次のパスへ遷移
*  @return trueなら次のパス、falseなら描画終了
*/
//-----------------------------------------------------------------------------
b32 UtilTextRenderingPipeLine::NextRenderPath(void)
{
  m_RenderPathCnt++;
  if ( m_RenderPathCnt == 1 )
  {
    return false;
  }
  return true;
}

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gfl2)
