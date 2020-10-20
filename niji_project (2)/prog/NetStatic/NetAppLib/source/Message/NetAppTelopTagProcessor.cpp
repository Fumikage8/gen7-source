//==============================================================================
/**
  *  @file   NetAppTelopTagProcessor.cpp
  *  @brief  
  *  @date   2015.06.19 Fri.
  *  @author Yu Muto
  */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <gfx/include/gfl2_GFGL.h>

#include  "NetStatic/NetAppLib/include/Message/NetAppTelopTagProcessor.h"


namespace NetAppLib  {
namespace Message  {
// =============================================================================
/**/
//==============================================================================

void  TelopTagProcessor::Update(void)
{
  m_ScrollOffset  += m_ScrollOffsetPerFrame;
}



void TelopTagProcessor::BeginPrint(nw::font::PrintContext< gfl2::str::STRCODE > * pContext)
{
  nw::ut::Rect  rect;
  f32           rectW;


  /*  cursorはoriginXに設定された状態でくる  */


  pContext->writer->CalcStringRect(&rect, pContext->str);
  rectW = rect.right - rect.left;

  /*  最大スクロール量はスクロール幅+textRect幅  */
  const f32 maxScrollDist = m_ScrollAreaWidth + rectW;


  if(rectW > 0.0f)
  {
    while(m_ScrollOffset >= maxScrollDist)
    {
      m_isScrollFinish = true;
      m_ScrollOffset  -= maxScrollDist;
    }
  }

  /*  スクロール開始位置は（基点が中央にあるとして）+スクロール幅/2  */
  const f32 scrollStart = m_ScrollAreaWidth/2;

  pContext->writer->MoveCursorX(scrollStart-m_ScrollOffset);
}


void TelopTagProcessor::EndPrint(nw::font::PrintContext< gfl2::str::STRCODE > * pContext)
{
}






} /*  namespace Message     */
} /*  namespace NetAppLib   */
