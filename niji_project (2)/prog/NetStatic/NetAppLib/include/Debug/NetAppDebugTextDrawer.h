#if !defined(NETAPPDEBUGTEXTDRAWER_H_INCLUDED)
#define NETAPPDEBUGTEXTDRAWER_H_INCLUDED
#pragma once
//==============================================================================
/**
  *  @file   NetAppDebugTextDrawer.h
  *  @brief  簡易テキスト描画（デバッグ用）
  *  @date   2015.06.16 Tue.
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


namespace NetAppLib  {
namespace Debug {
// =============================================================================
/**/
//==============================================================================
class NetAppDebugTextDrawerImplBase;

//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @note   !PM_DEBUG時には実装がすべて空になります（関数コール自体がなくなるわけではなく、先頭で抜けることになります）
  *  @date   2015.06.11 Thu.
  */
//==============================================================================
class NetAppDebugTextDrawer
{
  GFL_FORBID_COPY_AND_ASSIGN( NetAppDebugTextDrawer );

public:
  NetAppDebugTextDrawer(void);
  virtual ~NetAppDebugTextDrawer();


  void  Initialize(app::util::Heap* pHeap);
  void  Finalize(void);


  void  CLS(void);
  void  LOC(const s32 x, const s32 y);
  void  CRLF(void);
  void  COLOR(const u8 r, const u8 g, const u8 b);

  void  Scale(const f32 scaleH, const f32 scaleV);

  void  Print(const wchar_t* pFormat, ...);
  void  Print(const wchar_t* pFormat, va_list valist);
  void  PrintLn(const wchar_t* pFormat, ...);


  void  Draw(gfl2::gfx::CtrDisplayNo displayNo);

private:
  NetAppDebugTextDrawerImplBase*  m_pImpl;
};


} /*  namespace Debug */
} /*  namespace NetApp  */
#endif  /*  #if !defined(NETAPPDEBUGTEXTDRAWER_H_INCLUDED)  */
