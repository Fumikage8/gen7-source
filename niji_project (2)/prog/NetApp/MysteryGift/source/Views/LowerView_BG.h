#if !defined(LOWERVIEW_BG_H_INCLUDED)
#define LOWERVIEW_BG_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LowerView_BG.h
 * @date   2016/03/09 Wed. 19:08:25
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"




namespace NetApp  {
namespace Gift    {
  class GiftWork;
// =============================================================================
/**/
//==============================================================================
class LowerView_BG
  : public  NetAppLib::UI::NetAppEmptyView
{
  GFL_FORBID_COPY_AND_ASSIGN( LowerView_BG );
public:
  LowerView_BG(GiftWork* pWork);
  virtual ~LowerView_BG();

  /*  implement UIView I/F  */
//  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
//  virtual bool  IsDrawing(void) const;
  /*  endimplement UIView I/F  */


protected:
  /*
    members
  */
  GiftWork* m_pWork;
};


} /*  namespace Gift    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(LOWERVIEW_BG_H_INCLUDED)  */
