#if !defined(NUMBERCONTROLLER_RESULTTOTAL_H_INCLUDED)
#define NUMBERCONTROLLER_RESULTTOTAL_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   NumberController_ResultTotal.h
 * @date   2016/11/16 Wed. 17:06:11
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <System/include/PokemonVersion.h>
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
class NumberController_ResultTotal
  : public NumberController
{
  GFL_FORBID_COPY_AND_ASSIGN(NumberController_ResultTotal);
public:
  //------------------------------------------------------------------
  /**
    *  @brief    ctor
    *  @param    [in] pHeap               heap
    *  @param    [in] pRenderingManager   renderingManager
    */
  //------------------------------------------------------------------
  explicit NumberController_ResultTotal(const u32 digits = 1);

  /**
    dtor
  */
  virtual ~NumberController_ResultTotal(){}

  virtual void  Update(void);

protected:
};


} /*  namespace UI  */
} /*  namespace MantainSurf  */
} /*  namespace App  */
#endif  /*  #if !defined(NUMBERCONTROLLER_RESULTTOTAL_H_INCLUDED)  */
