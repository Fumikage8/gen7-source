
#if !defined(NUMBERCONTROLLER_RESULTTRICK_H_INCLUDED)
#define NUMBERCONTROLLER_RESULTTRICK_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   NumberController_ResultTrick.h
 * @date   2016/11/17 Thu. 16:26:22
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
  *  @date   2016/11/17 Thu. 16:26:40
  */
//==============================================================================
class NumberController_ResultTrick
  : public NumberController
{
  GFL_FORBID_COPY_AND_ASSIGN(NumberController_ResultTrick);
public:
  //------------------------------------------------------------------
  /**
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  NumberController_ResultTrick(const u32 digits)
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
  virtual ~NumberController_ResultTrick(){}

  void  SetupTrickNameAnimID(const u32 animID)  {m_AnimID[ANIMID_TrickName] = animID;}
  void  SetupInAnimID(const u32 animID)         {m_AnimID[ANIMID_In]        = animID;}
  void  SetTrickID(const u32 trickID)           { if(m_AnimID[ANIMID_TrickName] != ANIMID_INVALID) { m_pG2DUtil->SetAnimeFrame(m_LayoutID, m_AnimID[ANIMID_TrickName], trickID); } }
  void  Show(void)                              {m_pG2DUtil->StartAnime(m_LayoutID, m_AnimID[ANIMID_In]);}

//  virtual void  Update(void);

protected:
  enum
  {
    ANIMID_TrickName,
    ANIMID_In,

    ANIMID_MAX
  };

  static const u32  ANIMID_INVALID = UINT_MAX;
  u32   m_AnimID[ANIMID_MAX];
};


} /*  namespace UI  */
} /*  namespace MantainSurf  */
} /*  namespace App  */
#endif  /*  #if !defined(NUMBERCONTROLLER_RESULTTRICK_H_INCLUDED)  */
