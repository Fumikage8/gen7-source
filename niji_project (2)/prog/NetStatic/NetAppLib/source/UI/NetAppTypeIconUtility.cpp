//==============================================================================
/**
 * @file   NetAppTypeIconUtility.cpp
 * @date   2017/01/26 Thu. 19:36:27
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <AppLib/include/Tool/AppCommonGrpIconData.h>

#include  "../../include/UI/NEtAppTypeIconUtility.h"

namespace NetAppLib {
namespace UI {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_NetAppTypeIconUtility
namespace FILELOCAL {
}


// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
NetAppTypeIconUtility::NetAppTypeIconUtility(void)
  : m_pAppCommonGrpIconData(NULL)
  , m_pPicturePane(NULL)

{
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
NetAppTypeIconUtility::~NetAppTypeIconUtility()
{
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  NetAppTypeIconUtility::SetType(const PokeType_tag type)
{
  bool  isSuccess = false;

  if(m_pAppCommonGrpIconData && m_pPicturePane)
  {
    isSuccess = m_pAppCommonGrpIconData->ReplacePaneTextureByWazaTypeIcon(type, m_pPicturePane);
    
    SetVisibility(isSuccess);
  }

  return isSuccess;
}




}  /*  namespace UI  */
}  /*  namespace NetAppLib  */
