//==============================================================================
/**
 * @file   app_util_GetPaneHelper.cpp
 * @date   2015/07/15 Wed. 17:05:08
 * @date   2015/08/03 Mon. 18:19:22 NetAppLibより移動
 * @author muto_yu
 * @brief  LayoutWorkから各種Paneを取得するためのヘルパ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <AppLib/include/Util/app_util_2d.h>

#include  "AppLib/include/Util/app_util_GetPaneHelper.h"


namespace app   {
namespace util  {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
GetPaneHelper::GetPaneHelper(gfl2::lyt::LytWk* pLayoutWork, gfl2::lyt::LytMultiResID* pMultiResID)
  : m_pLayout(pLayoutWork)
  , m_pLayoutMultiResourceID(pMultiResID)
{
  Clear();
}

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
GetPaneHelper::GetPaneHelper(G2DUtil* pG2D, const u32 layoutWorkID, const u32 resourceIndex)
  : m_pLayout(pG2D->GetLayoutWork(layoutWorkID))
  , m_pLayoutMultiResourceID(&(pG2D->GetLayoutResourceID(resourceIndex)))
{
  Clear();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
GetPaneHelper::~GetPaneHelper()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
GetPaneHelper&  GetPaneHelper::Clear(void)
{
  m_StackDepth = 0;
  for(u32 num = 0; num < GFL_NELEMS(m_pPartsStack); ++num)
  {
    m_pPartsStack[num]  = NULL;
  }

  return *this;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
GetPaneHelper&  GetPaneHelper::Push(const gfl2::lyt::LytPaneIndex paneID)
{
  GFL_ASSERT_MSG(m_StackDepth < PARTS_STACK_DEPTH_MAX, "capacity overflow");
  if(m_StackDepth < PARTS_STACK_DEPTH_MAX)
  {
    gfl2::lyt::LytParts*  pPushedParts    = NULL;


    if(!m_StackDepth)
    {
      pPushedParts  = m_pLayout->GetPartsPane(paneID);
    }
    else
    {
      gfl2::lyt::LytParts*  pStackTopParts  = m_pPartsStack[m_StackDepth-1];
      
      if(pStackTopParts)
      {
        pPushedParts  = m_pLayout->GetPartsPane(pStackTopParts, paneID, m_pLayoutMultiResourceID);
      }
    }
    
    
    GFL_ASSERT_MSG(pPushedParts, "partsPane not found: id[%d]\n", paneID);
    if(pPushedParts)
    {
      m_pPartsStack[m_StackDepth] = pPushedParts;
      ++m_StackDepth;
    }
  }

  return *this;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
GetPaneHelper&  GetPaneHelper::Pop(void)
{
  GFL_ASSERT_MSG(m_StackDepth > 0, "capacity underflow");

  if(m_StackDepth > 0)
  {
    m_pPartsStack[m_StackDepth]  = NULL;
    --m_StackDepth;
  }

  return *this;
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
gfl2::lyt::LytParts*  GetPaneHelper::Peek(void)
{
  return (m_StackDepth ? m_pPartsStack[m_StackDepth-1] : NULL);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
gfl2::lyt::LytPane*       GetPaneHelper::GetPane(const gfl2::lyt::LytPaneIndex paneID)         {return GetPaneCore<gfl2::lyt::LytPane>(paneID);}
gfl2::lyt::LytPicture*    GetPaneHelper::GetPicturePane(const gfl2::lyt::LytPaneIndex paneID)  {return GetPaneCore<gfl2::lyt::LytPicture>(paneID);}
gfl2::lyt::LytTextBox*    GetPaneHelper::GetTextBoxPane(const gfl2::lyt::LytPaneIndex paneID)  {return GetPaneCore<gfl2::lyt::LytTextBox>(paneID);}
gfl2::lyt::LytWindow*     GetPaneHelper::GetWindowPane(const gfl2::lyt::LytPaneIndex paneID)   {return GetPaneCore<gfl2::lyt::LytWindow>(paneID);}
gfl2::lyt::LytBounding*   GetPaneHelper::GetBoundingPane(const gfl2::lyt::LytPaneIndex paneID) {return GetPaneCore<gfl2::lyt::LytBounding>(paneID);}
gfl2::lyt::LytParts*      GetPaneHelper::GetPartsPane(const gfl2::lyt::LytPaneIndex paneID)    {return GetPaneCore<gfl2::lyt::LytParts>(paneID);}


} /*  namespace UI        */
} /*  namespace NetAppLib */
