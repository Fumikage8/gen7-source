#if !defined(NUMBERCONTROLLER_TRICKSCORE_H_INCLUDED)
#define NUMBERCONTROLLER_TRICKSCORE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   NumberController_TrickScore.h
 * @date   2017/01/18 Wed. 13:01:12
 * @author muto_yu
 * @brief  上画面技名表示に付随するスコア表示
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
  *  @date   2017/01/18 Wed. 13:01:39
  */
//==============================================================================
class NumberController_TrickScore
  : public NumberController
{
  GFL_FORBID_COPY_AND_ASSIGN(NumberController_TrickScore);
public:
  //------------------------------------------------------------------
  /**
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  NumberController_TrickScore(void)
   : NumberController()
  {}


  NumberController_TrickScore(const u32 digits)
    : NumberController(digits)
  {
    for(u32 index = 0; index < GFL_NELEMS(m_AnimID); ++index)
    {
      m_AnimID[index] = ANIMID_INVALID;
    }
  }

  /**
    dtor
  */
  virtual ~NumberController_TrickScore(){}

  virtual void  Update(void);

  void  SetupAnimID_PlusSign(const u32 animID)  {m_AnimID[ANIMID_PlusSign] = animID;}

//  virtual void  Update(void);

protected:
  enum
  {
    ANIMID_PlusSign,

    ANIMID_MAX
  };

  void  UpdatePlusSign(void);


  static const u32  ANIMID_INVALID = UINT_MAX;
  u32   m_AnimID[ANIMID_MAX];
};


} /*  namespace UI  */
} /*  namespace MantainSurf  */
} /*  namespace App  */
#endif  /*  #if !defined(NUMBERCONTROLLER_TRICKSCORE_H_INCLUDED)  */
