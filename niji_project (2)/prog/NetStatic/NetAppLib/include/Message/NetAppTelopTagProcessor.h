#if !defined(NETAPPTELOPTAGPROCESSOR_H_INCLUDED)
#define NETAPPTELOPTAGPROCESSOR_H_INCLUDED
#pragma once
//==============================================================================
/**
  *  @file   NetAppTelopTagProcessor.h
  *  @brief  
  *  @date   2015.06.19 Fri.
  *  @author Yu Muto
  */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <str/include/gfl2_Str.h>


namespace NetAppLib  {
namespace Message  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015.06.19 Fri.
  */
//==============================================================================
class TelopTagProcessor
  : public nw::font::TagProcessorBase<gfl2::str::STRCODE>
{
  GFL_FORBID_COPY_AND_ASSIGN( TelopTagProcessor );
public:
  TelopTagProcessor(void)
    : gfl2::str::TagProcBase()
    , m_ScrollOffsetPerFrame(1)
    , m_ScrollOffset(0)
    , m_ScrollAreaWidth(0)
    , m_isScrollFinish(false)
  {}

  virtual ~TelopTagProcessor()
  {}


  void  SetRectWidth(const f32 w) {m_ScrollAreaWidth  = w;}
  void  SetScrollSpeed(const f32 px, f32 frames)  {
          if(frames < FLT_EPSILON)  frames  = 1.0f;

          m_ScrollOffsetPerFrame  = px / frames;
          
          if(m_ScrollOffsetPerFrame < 0.0f)
          {
            m_ScrollOffsetPerFrame  = .0f;
          }
          m_isScrollFinish = false;
        }

  void  ResetScroll(void) {m_ScrollOffset = 0.0f;}
  void  Update(void);


  virtual void BeginPrint(nw::font::PrintContext< gfl2::str::STRCODE > * pContext);
  virtual void EndPrint(nw::font::PrintContext< gfl2::str::STRCODE > * pContext);

  bool  IsScrollFinish(void) const  { return m_isScrollFinish; }
  void  ResetFinishFlag(void)       {m_isScrollFinish = false;}

protected:
  f32   m_ScrollOffsetPerFrame;
  f32   m_ScrollOffset;
  f32   m_ScrollAreaWidth;

  bool  m_isScrollFinish; //! 1回でもスクロール終了したらtrueになる
};



} /*  namespace Message */
} /*  namespace NetAppLib  */
#endif  /*  #if !defined(NETAPPTELOPTAGPROCESSOR_H_INCLUDED)  */
