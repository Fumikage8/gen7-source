#if !defined(SEPLAYER_H_INCLUDED)
#define SEPLAYER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   SEPlayer.h
 * @date   2017/04/10 Mon. 18:25:34
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>

//  #include  "../../../../include/MantainSurfAppData.h"
#include  "../../../SurfSound/SurfSoundManager.h"


namespace App {
namespace MantainSurf {
namespace UI {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2017/04/10 Mon. 18:25:36
  */
//==============================================================================
class SEPlayer
{
  GFL_FORBID_COPY_AND_ASSIGN(SEPlayer);
public:
  SEPlayer(void)
#if PM_DEBUG
    : m_bEnableSE(true)
#endif
  {}

  virtual ~SEPlayer() {}

  void  PlaySE(const SurfSound::SurfSoundManager::SEIndex seID, const u32 fadeInFrame = 0, const f32 pitch = 1.0f)
  {
#if PM_DEBUG
    if(m_bEnableSE)
#endif
    {
      SurfSound::SurfSoundManager::PlaySE(seID, fadeInFrame, pitch);
    }
  }


  void  StopSE(const SurfSound::SurfSoundManager::SEIndex seID, const u32 fadeOutFrame = 0)
  {
    SurfSound::SurfSoundManager::StopSE(seID, fadeOutFrame);
  }



#if PM_DEBUG
  void  Debug_EnableSE(const bool bEnableSE)  {m_bEnableSE  = bEnableSE;}
#endif




protected:
#if PM_DEBUG
  bool  m_bEnableSE;
#endif
};



}  /*  namespace UI  */
}  /*  namespace MantainSurf  */
}  /*  namespace App  */
#endif  /*  #if !defined(SEPLAYER_H_INCLUDED)  */
