#if !defined(TEXTDRAWER_H_INCLUDED)
#define TEXTDRAWER_H_INCLUDED
#pragma once

#if PM_DEBUG
//==============================================================================
/**
  *  @file   TextDrawer.h
  *  @brief  
  *  @date   2015.06.11 Thu.
  *  @author Yu Muto
  */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <stdarg.h>
#include  <math/include/gfl2_math.h>
#include  <util/include/gfl2_DrawUtilText.h>
#include  <util/include/gfl2_UtilTextRenderingPipeLine.h>


namespace app {
namespace util  {
  class Heap;
}
}


namespace Test  {
namespace MutoTest {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015.06.11 Thu.
  */
//==============================================================================
class TextDrawer
{
  GFL_FORBID_COPY_AND_ASSIGN( TextDrawer );

public:
  TextDrawer(void);
  virtual ~TextDrawer();


  void  Initialize(app::util::Heap* pHeap);
  void  Finalize(void);


  void  CLS(void);
  void  LOC(const s32 x, const s32 y)   {m_Pos.x  = x * m_FontH;  m_Pos.y = y * m_FontH;}
  void  CRLF(void)                      {LOC(0, (m_Pos.y / m_FontH)+1);}

  int   Print(const wchar_t* pFormat, ...);
  int   Print(const wchar_t* pFormat, va_list valist);
  int   PrintLn(const wchar_t* pFormat, ...);

  void  Scale(const f32 scale);

  void  Draw(gfl2::gfx::CtrDisplayNo displayNo);


protected:
  
  gfl2::util::UtilTextRenderingPipeLine*  m_pTextRenderPipe;
  gfl2::util::DrawUtilText*               m_pTextUtil;
  struct{s32 x, y;}                       m_Pos;
  gfl2::math::Vector4                     m_Color;
  int                                     m_FontH;
};







} /*  namespace MutoTest */
} /*  namespace Test      */
#endif  /*  #if PM_DEBUG                          */
#endif  /*  #if !defined(TEXTDRAWER_H_INCLUDED)   */
