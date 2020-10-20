//==============================================================================
/**
 * @file   NetAppWazaUtility.cpp
 * @date   2017/01/27 Fri. 11:17:10
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../../include/UI/NetAppWazaUtility.h"


namespace NetAppLib {
namespace UI {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_NetAppWazaUtility
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
NetAppWazaUtility::NetAppWazaUtility(void)
  : m_TypeIconUtility()
  , m_pG2DUtil(NULL)
  , m_LayoutID(0)
  , m_pControlPane(NULL)
  , m_pTextBox(NULL)
  , m_pFormatString(NULL)
{
  for(u32 index = 0; index < GFL_NELEMS(m_TypeAnimIDs); ++index)
  {
    m_TypeAnimIDs[index]  = INVALID_TYPEANIM;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppWazaUtility::SetWaza(const WazaNo wazano)
{
  const PokeType_tag  type    = NetAppTypeIconUtility::GetWazaType(wazano);
  const bool          isEmpty = (wazano == WAZANO_NULL);

  if(m_pControlPane)  m_pControlPane->SetVisible(!isEmpty);
  if(isEmpty)         return;

  if(m_pG2DUtil)
  {
    /*  技名表示  */
    if(m_pTextBox && m_pFormatString)
    {
      m_pG2DUtil->SetRegisterWazaName(0, wazano);
      m_pG2DUtil->SetTextBoxPaneStringExpand(m_pTextBox, *m_pFormatString);
    }

    /*  台座アニメ  */
    if(m_TypeAnimIDs[type] != INVALID_TYPEANIM)
    {
      m_pG2DUtil->StartAnime(m_LayoutID, m_TypeAnimIDs[type]);
    }
  }

  /*  タイプアイコン  */
  m_TypeIconUtility.SetTypeFromWazaNo(wazano);
}




}  /*  namespace UI  */
}  /*  namespace NetAppLib  */
