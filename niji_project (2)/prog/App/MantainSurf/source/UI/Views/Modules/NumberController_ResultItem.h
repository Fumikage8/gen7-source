#if !defined(NUMBERCONTROLLER_RESULTITEM_H_INCLUDED)
#define NUMBERCONTROLLER_RESULTITEM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   NumberController_ResultItem.h
 * @date   2016/11/18 Fri. 16:54:00
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./NumberController.h"

namespace App  {
namespace MantainSurf  {
namespace UI  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/11/16 Wed. 15:18:48
  */
//==============================================================================
class NumberController_ResultItem
  : public NumberController
{
  GFL_FORBID_COPY_AND_ASSIGN(NumberController_ResultItem);
public:
  //------------------------------------------------------------------
  /**
    *  @brief    ctor
    *  @param    [in] pHeap               heap
    *  @param    [in] pRenderingManager   renderingManager
    */
  //------------------------------------------------------------------
  explicit NumberController_ResultItem(const u32 digits=1)
    : NumberController(digits)
    , m_pNumberGroupPane(NULL)
    , m_AppearAnimID(UINT_MAX)
    , m_SuspendedTargetValue(0)
    , m_bTargetValueSuspended(false)
  {}

  /**
    dtor
  */
  virtual ~NumberController_ResultItem(){}


  virtual void  Update(void);

  void  SetAppearAnimID(const u32 animID)               {m_AppearAnimID = animID;}
  void  SetNumberGroupPane(gfl2::lyt::LytPane* pPane)   {m_pNumberGroupPane = pPane;}
  void  SetTargetValueAfterAnim(const u32 targetValue);
  void  ClearAfterAnimRequest(void)                     {m_bTargetValueSuspended = false; }
  bool  IsAfterAnimRequested(void) const                {return m_bTargetValueSuspended;}

protected:
  static const u32 DefaultFrame = 5;

  gfl2::lyt::LytPane* m_pNumberGroupPane;
  u32                 m_AppearAnimID;
  u32                 m_SuspendedTargetValue;
  bool                m_bTargetValueSuspended;
};


} /*  namespace UI  */
} /*  namespace MantainSurf  */
} /*  namespace App  */
#endif  /*  #if !defined(NUMBERCONTROLLER_RESULTITEM_H_INCLUDED)  */
