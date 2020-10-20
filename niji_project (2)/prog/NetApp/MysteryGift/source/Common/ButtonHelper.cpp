
//==============================================================================
/**
  *  @file   ButtonHelper.cpp
  *  @brief  
  *  @date   2015.05.20 Wed.
  *  @author Yu Muto
  */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./ButtonHelper.h"



namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func    CreateButton
  *  @brief   ButtonHelperInfoからのボタン生成
  *  @note    複数のLayoutへは対応していない
  */
//------------------------------------------------------------------
void  CreateButton(
  app::util::Heap*                pHeap,
  app::ui::UIResponder*           pResponder,
  gfl2::lyt::LytWk*               pLayout,
  const gfl2::lyt::LytMultiResID& rLayoutMultiResourceID,
  const ButtonHelperInfo*         pHelperInfo,
  const u32                       infoNum
)
{
  app::ui::ButtonInfoEx*  pInfos  = GFL_NEW_LOW_ARRAY(pHeap->GetDeviceHeap()) app::ui::ButtonInfoEx[infoNum];
  app::ui::ButtonInfoEx*  pInfo   = pInfos;

  for(u32 num = 0; num < infoNum; ++num, ++pInfo)  {
    pHelperInfo[num].assignToButtonInfoEx(*pInfo);
    if(pHelperInfo[num].isPartsPicture)  {
      /*  partsのとき  */
      gfl2::lyt::LytParts*  pButtonParts  = NULL;
      pInfo->picture_pane  = 
      pButtonParts         = pLayout->GetPartsPane(pHelperInfo[num].picturePaneIndex);
      pInfo->bound_pane    = pLayout->GetBoundingPane( pButtonParts, pHelperInfo[num].boundingPaneIndex, &rLayoutMultiResourceID );
    }else {
      /*  そうでないとき  */
      pInfo->picture_pane  = pLayout->GetPane(pHelperInfo[num].picturePaneIndex);
      pInfo->bound_pane    = pLayout->GetBoundingPane(pHelperInfo[num].boundingPaneIndex);
    }
  }


  pResponder->CreateButtonManager( pHeap, pLayout, pInfos,  infoNum);
  GFL_SAFE_DELETE_ARRAY(pInfos);
}



//------------------------------------------------------------------
/**
  *  @func    CreateButton
  *  @brief   ButtonHelperInfoからのボタン生成
  *  @note    複数のLayoutへは対応していない
  */
//------------------------------------------------------------------
void  CreateButton(
  app::util::Heap*          pHeap,
  app::ui::UIView*          pUIView,
  const u32                 layoutWorkID,
  const u32                 layoutResourceID,
  const ButtonHelperInfo*   pHelperInfo,
  const u32                 infoNum
) {
  app::util::G2DUtil*             pG2DUtil                = pUIView->GetG2DUtil();
  gfl2::lyt::LytWk*               pLayout                 = pG2DUtil->GetLayoutWork( layoutWorkID );
  const gfl2::lyt::LytMultiResID& rLayoutMultiResourceID  = pG2DUtil->GetLayoutResourceID( layoutResourceID );

  CreateButton(
    pHeap,
    pUIView,
    pLayout,
    rLayoutMultiResourceID,
    pHelperInfo,
    infoNum
  );
}


} /*  namespace Gift    */
} /*  namespace NetApp  */

