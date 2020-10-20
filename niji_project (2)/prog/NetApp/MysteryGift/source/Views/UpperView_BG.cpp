//==============================================================================
/**
 * @file   UpperView_BG.cpp
 * @date   2016/03/09 Wed. 18:50:09
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "AppLib/include/Util/app_util_GetPaneHelper.h"
#include  "../ApplicationSystem/GiftWork.h"

#include  "../GiftResource.h"
#include  "./UpperView_BG.h"

// =============================================================================
/**/
//==============================================================================
namespace
{
}


namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================
UpperView_BG::UpperView_BG(GiftWork* pWork)
  : NetAppLib::UI::NetAppEmptyView(pWork)
  , m_pWork(pWork)
{
  Create2DOneLayout(
    m_pWork,
    RESOURCE_Layout,
    LYTRES_GIFT_GIFT_BG_UP_000_BFLYT,
    LA_GIFT_GIFT_BG_UP_000___NUM,
    app::util::G2DUtil::SETUP_UPPER,
    &(m_pWork->GetMessageUtility()),
    GIFT_MESSAGE_GRAC_ID
  );


  GetG2DUtil()->StartAnime(0, LA_GIFT_GIFT_BG_UP_000_KEEP);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
UpperView_BG::~UpperView_BG()
{
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  UpperView_BG::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pWork->GetAppRenderingManager(), displayNo, 0);
}


} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
