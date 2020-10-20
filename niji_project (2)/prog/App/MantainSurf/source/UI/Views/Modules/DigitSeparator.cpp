//==============================================================================
/**
 * @file   DigitSeparator.cpp
 * @date   2017/01/20 Fri. 19:59:16
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./DigitSeparator.h"


namespace App {
namespace MantainSurf {
namespace UI {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_DigitSeparator
namespace FILELOCAL {

  static const u32  LocaleAnimFrame[DigitSeparator::Info::Languages] =
  {
    DigitSeparator::Info::AnimFarme_Comma,  //  POKEMON_LANG_NULL   
    DigitSeparator::Info::AnimFarme_Comma,  //  POKEMON_LANG_JAPAN  
    DigitSeparator::Info::AnimFarme_Comma,  //  POKEMON_LANG_ENGLISH
    DigitSeparator::Info::AnimFarme_Space,  //  POKEMON_LANG_FRANCE 
    DigitSeparator::Info::AnimFarme_Dot,    //  POKEMON_LANG_ITALY  
    DigitSeparator::Info::AnimFarme_Space,  //  POKEMON_LANG_GERMANY
    DigitSeparator::Info::AnimFarme_Space,  //  POKEMON_LANG_SPAIN  
    DigitSeparator::Info::AnimFarme_Comma,  //  POKEMON_LANG_KOREA  
    DigitSeparator::Info::AnimFarme_Comma,  //  POKEMON_LANG_CHINA  
    DigitSeparator::Info::AnimFarme_Comma,  //  POKEMON_LANG_TAIWAN 
  };
}


// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DigitSeparator::SetupLocale(void)
{
  const bool  isValidID = (m_Info.languageID < Info::Languages);
  
  
  GFL_ASSERT(m_Info.pG2DUtil);
  GFL_ASSERT(isValidID);
  

  if(m_Info.pG2DUtil && isValidID)
  {
    const u32 localeAnimFrame = FILELOCAL::LocaleAnimFrame[m_Info.languageID];
    
    if(m_Info.pG2DUtil->IsAnimeEnd(m_Info.layoutID, m_Info.animID))
    {
      m_Info.pG2DUtil->StartAnimeFrame(m_Info.layoutID, m_Info.animID, localeAnimFrame, false);
    }
    else
    {
      m_Info.pG2DUtil->SetAnimeFrame(m_Info.layoutID, m_Info.animID, localeAnimFrame);
    }
  }
  
  

}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DigitSeparator::UpdateByValue(const u32 value)
{
  if(m_Info.pSeparatorPane)
  {
    m_Info.pSeparatorPane->SetVisible(value >= m_Info.threshold);
  }
}




}  /*  namespace UI  */
}  /*  namespace MantainSurf  */
}  /*  namespace App  */
