//==============================================================================
/**
 * @file   LowerView_Info.cpp
 * @date   2017/02/07 Tue. 15:05:56
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <AppLib/include/Util/app_util_GetPaneHelper.h>
/*  kisekae  */
#include  <KisekaeData/include/KisekaeDataBase.h>
#include  <Savedata/include/FashionSave.h>

#include  <App/FinderStudioStatic/include/FinderStudioBGDataAccessor.h>

#include  "../ApplicationSystem/PhotoViewWork.h"
#include  "../PhotoViewResource.h"
#include  "./LowerView_Info.h"


namespace NetApp {
namespace PhotoView {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_LowerView_Info
namespace FILELOCAL {
  enum
  {
    BUTTON_Back,

    BUTTON_MAX
  };


  /*  button  */
  void  CreateButton(app::util::Heap* pHeap, app::ui::UIResponder* pResponder, gfl2::lyt::LytWk* pLayoutWork, gfl2::lyt::LytMultiResID& rLytMultiResID)
  {
    app::util::GetPaneHelper  helper(pLayoutWork, &rLytMultiResID);
    
    app::ui::ButtonInfoEx buttonInfo[] =
    {
      {
        BUTTON_Back,
        helper.Clear().Push(PANENAME_JFPL_PHOTODESC_LOW_000_PANE_BACKBUTTON).Peek(),
        helper.GetBoundingPane(PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00),
        LA_JOIN_FESTA_PLAYERLIST_JFPL_PHOTODESC_LOW_000__BACKBUTTON_TOUCH,
        LA_JOIN_FESTA_PLAYERLIST_JFPL_PHOTODESC_LOW_000__BACKBUTTON_RELEASE,
        LA_JOIN_FESTA_PLAYERLIST_JFPL_PHOTODESC_LOW_000__BACKBUTTON_CANSEL,
        LA_JOIN_FESTA_PLAYERLIST_JFPL_PHOTODESC_LOW_000__BACKBUTTON_TOUCH_RELEASE,
        LA_JOIN_FESTA_PLAYERLIST_JFPL_PHOTODESC_LOW_000__BACKBUTTON_ACTIVE,
        LA_JOIN_FESTA_PLAYERLIST_JFPL_PHOTODESC_LOW_000__BACKBUTTON_PASSIVE,
      },
    };

    pResponder->CreateButtonManager( pHeap, pLayoutWork, buttonInfo, GFL_NELEMS(buttonInfo) );

    /*  SE  */
    {
      app::tool::ButtonManager* pButtonManager  = pResponder->GetButtonManager();

      pButtonManager->SetButtonSelectSE(BUTTON_Back, NetApp::PhotoView::SEID_CANCEL);
    }
  }
}


// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
LowerView_Info::LowerView_Info(PhotoViewWork* pWork)
  : NetAppLib::UI::NetAppEmptyView(pWork)
  , m_pWork(pWork)
  , m_pHandler(NULL)
{
  Create2DOneLayout(
    pWork
    , RESOURCE_Layout
    , LYTRES_JOIN_FESTA_PLAYERLIST_JFPL_PHOTODESC_LOW_000_BFLYT
    , LA_JOIN_FESTA_PLAYERLIST_JFPL_PHOTODESC_LOW_000___NUM
    , app::util::G2DUtil::SETUP_LOWER
    , &(pWork->GetMessageUtility())
    , MESSAGE_GRAC_ID
  );

  app::util::G2DUtil*           pG2DUtil        = GetG2DUtil();
  gfl2::lyt::LytWk*             pLayoutWork     = pG2DUtil->GetLayoutWork(0);
  gfl2::lyt::LytMultiResID&     rLytMultiResID  = pG2DUtil->GetLayoutResourceID(0);
  gfl2::heap::HeapBase*         pHeapBase       = m_pWork->GetAppHeap()->GetDeviceHeap();
  app::util::GetPaneHelper      helper(pG2DUtil, 0, 0);

  /*  create button  */
  FILELOCAL::CreateButton(m_pWork->GetAppHeap(), this, pLayoutWork, rLytMultiResID);
  SetInputListener( this );

  {
    app::tool::ButtonManager* pBM = GetButtonManager();

    pBM->SetButtonBindKey( FILELOCAL::BUTTON_Back, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
    pBM->SetButtonSelectSE(FILELOCAL::BUTTON_Back, SEID_CANCEL );
  }


  /*  pane  */
  {
    pTextBoxes[TEXTBOX_TrainerName]   = helper.GetTextBoxPane(PANENAME_JFPL_PHOTODESC_LOW_000_PANE_PLAYERNAME_00);
    pTextBoxes[TEXTBOX_Wear0]         = helper.GetTextBoxPane(PANENAME_JFPL_PHOTODESC_LOW_000_PANE_WEARPARAM_00);
    pTextBoxes[TEXTBOX_Wear1]         = helper.GetTextBoxPane(PANENAME_JFPL_PHOTODESC_LOW_000_PANE_WEARPARAM_01);
    pTextBoxes[TEXTBOX_Wear2]         = helper.GetTextBoxPane(PANENAME_JFPL_PHOTODESC_LOW_000_PANE_WEARPARAM_02);
    pTextBoxes[TEXTBOX_Wear3]         = helper.GetTextBoxPane(PANENAME_JFPL_PHOTODESC_LOW_000_PANE_WEARPARAM_03);
    pTextBoxes[TEXTBOX_Wear4]         = helper.GetTextBoxPane(PANENAME_JFPL_PHOTODESC_LOW_000_PANE_WEARPARAM_04);
    pTextBoxes[TEXTBOX_Wear5]         = helper.GetTextBoxPane(PANENAME_JFPL_PHOTODESC_LOW_000_PANE_WEARPARAM_05);
    pTextBoxes[TEXTBOX_Wear6]         = helper.GetTextBoxPane(PANENAME_JFPL_PHOTODESC_LOW_000_PANE_WEARPARAM_06);
    pTextBoxes[TEXTBOX_Wear7]         = helper.GetTextBoxPane(PANENAME_JFPL_PHOTODESC_LOW_000_PANE_WEARPARAM_07);
    pTextBoxes[TEXTBOX_LocationName]  = helper.GetTextBoxPane(PANENAME_JFPL_PHOTODESC_LOW_000_PANE_BGPARAM_00);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
LowerView_Info::~LowerView_Info()
{
  Delete2D();
  DeleteGraphicsSystem();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Info::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pWork->GetAppRenderingManager(), displayNo, 0);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult  LowerView_Info::OnLayoutPaneEvent( const u32 buttonID )
{
  switch(buttonID)
  {
    case  FILELOCAL::BUTTON_Back:
      Event(IEventHandler::EVENT_Exit);
      break;
  }

  return app::ui::UIInputListener::DISABLE_ACCESS;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Info::Event(const IEventHandler::EventCode event)
{
  if(m_pHandler)
  {
    m_pHandler->LowerView_Info_OnEvent(this, event);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Info::SetData(const gfl2::str::STRCODE* const pName, const App::FinderStudio::Viewer::PhotoParam& rPhotoParam)
{
  const u32 wearNameTextBoxIDs[] =
  {
    TEXTBOX_Wear0,
    TEXTBOX_Wear1,
    TEXTBOX_Wear2,
    TEXTBOX_Wear3,
    TEXTBOX_Wear4,
    TEXTBOX_Wear5,
    TEXTBOX_Wear6,
    TEXTBOX_Wear7,
  };

  app::util::G2DUtil*                     pG2DUtil        = GetG2DUtil();
  gfl2::str::StrBuf*                      pTempStr        = pG2DUtil->GetTempStrBuf(1);
  print::WordSet*                         pWordSet        = pG2DUtil->GetWordSet();
  NetAppLib::Message::MessageUtility&     rMessageUtil    = m_pWork->GetMessageUtility();
  const poke_3d::model::DressUpParam::Sex trainerGender   = static_cast<poke_3d::model::DressUpParam::Sex>(rPhotoParam.sexTrainer);
  const bool                              bWithTrainer    = (rPhotoParam.captureMode == App::FinderStudio::Viewer::CAPTURE_MODE_BOTH);
  const s32                               rideSuitsIndex  = static_cast<s32>(rPhotoParam.situ) - 1;   /*  -1:bias解除  */
  const bool                              bRideSuits      = 0 <= rideSuitsIndex;
  u32                                     wearIndex       = 0;



  /*  name  */
  {
    pTempStr->SetStr(pName);
    pG2DUtil->SetRegisterWord(0, *pTempStr);
    pG2DUtil->SetTextBoxPaneStringExpand(pTextBoxes[TEXTBOX_TrainerName], rMessageUtil.GetExpandedString(MESSAGE_GRAC_ID, jf_playerlist_cap_01));
  }
/*
struct table_data_GoodsData {
	short no;
	const char* data_id;
	short item_id;
	char sex;
	short goods_txt_id;
	short color_txt_id;
	int sell_num;
	char color_change;
	short fes_coin_num;
	char brand;
	short tex_id;
};
*/


  if(bWithTrainer)
  {
    /*  トレーナー情報  */
    if(bRideSuits)
    {
      s32 messageID = -1;
      
      /*  ライド服  */
      if(trainerGender == poke_3d::model::DressUpParam::SEX_MALE)
      {
        if(rideSuitsIndex == DRESSUP_ITEM_ID_P1_SITU_RIDESEA01)     messageID = jf_photo_dress_part_ride01;     /*  海  */
        else if(rideSuitsIndex == DRESSUP_ITEM_ID_P1_SITU_RIDELS01) messageID = jf_photo_dress_part_ride02;     /*  陸  */
      }
      else if(trainerGender == poke_3d::model::DressUpParam::SEX_FEMALE)
      {
        if(rideSuitsIndex == DRESSUP_ITEM_ID_P2_SITU_RIDESEA01)     messageID = jf_photo_dress_part_ride01;     /*  海  */
        else if(rideSuitsIndex == DRESSUP_ITEM_ID_P2_SITU_RIDELS01) messageID = jf_photo_dress_part_ride02;     /*  陸  */
      }
      
      if(0 < messageID)
      {
        pWordSet->RegisterWord(0, rMessageUtil.GetString(MESSAGE_GRAC_ID, messageID));
        pG2DUtil->SetTextBoxPaneStringExpand( pTextBoxes[wearNameTextBoxIDs[wearIndex++]], jf_photo_dress_part_01 );
      }
      else
      {
        GFL_ASSERT_MSG(false, "invalid params: trainerGender[%d], rideSuitsIndex[%d]\n", trainerGender, rideSuitsIndex);
      }
    }
    else
    {
      /*  通常服  */
#define NAMESPACE_FASHIONITEM Savedata::Fashion
#if PM_DEBUG
  #define FASHION_CATEGORY(_category) #_category, NAMESPACE_FASHIONITEM::_category
#else
  #define FASHION_CATEGORY(_category) NAMESPACE_FASHIONITEM::_category
#endif
      const struct
      {
#if PM_DEBUG
        const char* const           pCategoryName;
#endif
        Savedata::Fashion::Category category;
        s16                         itemID;
      
        bool isAvailable(void) const  {return (0 <= itemID);}
      } dataTable[] = {
#define toItemID(_photoItemID)    (static_cast<s16>(_photoItemID) - 1)    /*  FSViewerで施しているbiasの解除  */
        {FASHION_CATEGORY(CATEGORY_HAT),     toItemID(rPhotoParam.hat)},
        {FASHION_CATEGORY(CATEGORY_ACCHAIR), toItemID(rPhotoParam.hairAccessory)},
        {FASHION_CATEGORY(CATEGORY_ACCEYE),  toItemID(rPhotoParam.eyeAccessory)},
        {FASHION_CATEGORY(CATEGORY_TOPS),    toItemID(rPhotoParam.tops)},
        {FASHION_CATEGORY(CATEGORY_BOTTOMS), toItemID(rPhotoParam.bottoms)},
        {FASHION_CATEGORY(CATEGORY_LEGS),    toItemID(rPhotoParam.leg)},
        {FASHION_CATEGORY(CATEGORY_SHOES),   toItemID(rPhotoParam.shoes)},
        {FASHION_CATEGORY(CATEGORY_BAG),     toItemID(rPhotoParam.bag)},
#undef  toItemID
      };
#undef  FASHION_CATEGORY
#undef  NAMESPACE_FASHIONITEM

      const table_data_GoodsData* pData = NULL;

      for(u32 index = 0; index < GFL_NELEMS(dataTable); ++index)
      {
        if(!dataTable[index].isAvailable()) continue;
        
        pData = app::util::Kisekae::KisekaeDataBase::GetGoodsDataSearchItemId(trainerGender, dataTable[index].category,  dataTable[index].itemID);
        if(pData)
        {
          pWordSet->RegisterKisekaeItemName(0, pData->goods_txt_id, 1);
          pG2DUtil->SetTextBoxPaneStringExpand( pTextBoxes[wearNameTextBoxIDs[wearIndex++]], jf_photo_dress_part_01 );
        }
        else
        {
          GFL_WARNING("%s, gender:%s, #%d の情報を取得できません\n", dataTable[index].pCategoryName, trainerGender ? "f" : "m", dataTable[index].itemID);
        }
      }
    }
  }

  pTempStr->Clear();
  pWordSet->RegisterWord(0, *pTempStr);
  for(u32 index = wearIndex; index < GFL_NELEMS(wearNameTextBoxIDs); ++index)
  {
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBoxes[wearNameTextBoxIDs[index]], jf_photo_dress_part_01 );
  }


  /*  location  */
  {
    const u32 locationNameMessageID = App::FinderStudio::BGDataAccessor::GetUIData(rPhotoParam.bgSetIndex).msgID;

    pG2DUtil->SetRegisterWord(0, rMessageUtil.GetString(LOCATION_GRAC_ID, locationNameMessageID));
    pG2DUtil->SetTextBoxPaneStringExpand( pTextBoxes[TEXTBOX_LocationName], jf_photo_haikei );
  }
}




}  /*  namespace PhotoView  */
}  /*  namespace NetApp  */
