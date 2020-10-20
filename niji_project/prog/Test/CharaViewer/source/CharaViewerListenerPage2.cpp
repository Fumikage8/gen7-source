//======================================================================
/**
 * @file CharaViewerListenerPage2.cpp
 * @date 2015/07/24 18:59:53
 * @author araki_syo
 * @brief CharaViewer2ページ目のリスナー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include <macro/include/gfl2_Macros.h>

#include "../include/CharaViewerListener.h"

GFL_NAMESPACE_BEGIN(test)
  GFL_NAMESPACE_BEGIN(chara_viewer)


  /**
   * @fn
   * ここに関数の説明を書く
   * @brief 要約説明
   * @param[in] var1 var1の説明
   * @param[out] var2 var2の説明
   * @param[in,out] var3 var3の説明
   * @return 戻り値の説明
   * @sa 参照すべき関数を書けばリンクが貼れる
   * @detail 詳細な説明
   */
app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_Page2( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  // めりこみ確認用パーツロード
  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_MATERIAL_CHECK_PARTS_LOAD_OF_CHECK_PARTS_LOAD))
  {
    m_view->LoadCollisionCheckParts();
    return app::ui::UIInputListener::ENABLE_ACCESS;
  }

  // データサイズ確認用パーツロード
  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_MATERIAL_DATASIZE_PARTS_LOAD_OF_DATASIZE_PARTS_LOAD))
  {
    m_view->LoadDataSizeCheckParts();
    return app::ui::UIInputListener::ENABLE_ACCESS;
  }

  // 色替え用のパーツ選択
  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_HEAD_W_SELECTOR_TITLE))
  {
    m_view->SetClothColorHat();
  }
  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_HAIR_ACCE_SELECTOR_TITLE))
  {
    m_view->SetClothColorAccHair();
  }
  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_EYE_W_SELECTOR_TITLE))
  {
    m_view->SetClothColorAccEye();
  }
  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_TOPS_SELECTOR_TITLE))
  {
    m_view->SetClothColorTops();
  }
  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_BOTTOMS_SELECTOR_TITLE))
  {
    m_view->SetClothColorBottoms();
  }
  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_SOCKS_SELECTOR_TITLE))
  {
    m_view->SetClothColorLegs();
  }
  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_SHOES_SELECTOR_TITLE))
  {
    m_view->SetClothColorShoes();
  }
  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_BAG_SELECTOR_TITLE))
  {
    m_view->SetClothColorBag();
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_PARTSSELECTORBASE_00))
  {
    return OnTouchEvent_PartsSelector( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_COLORPALLETBASE_00))
  {
    return OnTouchEvent_ColorPalletSelector( pTouchPanel, isTouch );
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_PartsSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_SKINSELECTORBASE))
  {
    return OnTouchEvent_SkinSelector( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_HAIRSTYLESELECTORBASE))
  {
    return OnTouchEvent_HairStyleSelector( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_HEADWEARSELECTORBASE))
  {
    return OnTouchEvent_HeadWearSelector( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_HAIRACCESSORYSELECTORBAS))
  {
    return OnTouchEvent_HairAccessorySelector( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_EYEWEARSELECTORBASE))
  {
    return OnTouchEvent_EyeWearSelector( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_TOPSSELECTORBASE))
  {
    return OnTouchEvent_TopsSelector( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_BOTTOMSSELECTORBASE))
  {
    return OnTouchEvent_BottomsSelector( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_SOCKSSELECTORBASE))
  {
    return OnTouchEvent_SocksSelector( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_SHOESSELECTORBASE))
  {
    return OnTouchEvent_ShoesSelector( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_BAGSELECTORBASE))
  {
    return OnTouchEvent_BagSelector( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_BANGLESELECTORBASE))
  {
    return OnTouchEvent_BangleSelector( pTouchPanel, isTouch );
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_SITUATIONSELECTORBASE))
  {
    return OnTouchEvent_SituationSelector( pTouchPanel, isTouch );
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_SkinSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_SKIN_SELECTOR_BTN_L))
  {
    m_view->SlideSkinSelector(false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_SKIN_SELECTOR_BTN_R))
  {
    m_view->SlideSkinSelector(true);
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_HairStyleSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_HAIR_STY_SELECTOR_BTN_L))
  {
    m_view->SlideHairStyleSelector(false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_HAIR_STY_SELECTOR_BTN_R))
  {
    m_view->SlideHairStyleSelector(true);
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_HeadWearSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_HEAD_W_SELECTOR_BTN_L))
  {
    m_view->SlideHeadWearSelector(false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_HEAD_W_SELECTOR_BTN_R))
  {
    m_view->SlideHeadWearSelector(true);
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_HairAccessorySelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_HAIR_ACCE_SELECTOR_BTN_L))
  {
    m_view->SlideHairAccessorySelector(false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_HAIR_ACCE_SELECTOR_BTN_R))
  {
    m_view->SlideHairAccessorySelector(true);
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_EyeWearSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_EYE_W_SELECTOR_BTN_L))
  {
    m_view->SlideEyeWearSelector(false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_EYE_W_SELECTOR_BTN_R))
  {
    m_view->SlideEyeWearSelector(true);
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_TopsSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_TOPS_SELECTOR_BTN_L))
  {
    m_view->SlideTopsSelector(false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_TOPS_SELECTOR_BTN_R))
  {
    m_view->SlideTopsSelector(true);
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_OnepieceSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  return app::ui::UIInputListener::ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_BottomsSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_BOTTOMS_SELECTOR_BTN_L))
  {
    m_view->SlideBottomsSelector(false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_BOTTOMS_SELECTOR_BTN_R))
  {
    m_view->SlideBottomsSelector(true);
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_SocksSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_SOCKS_SELECTOR_BTN_L))
  {
    m_view->SlideSocksSelector(false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_SOCK_SELECTOR_BTN_R))
  {
    m_view->SlideSocksSelector(true);
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_ShoesSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_SHOES_SELECTOR_BTN_L))
  {
    m_view->SlideShoesSelector(false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_SHOES_SELECTOR_BTN_R))
  {
    m_view->SlideShoesSelector(true);
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_BagSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_BAG_SELECTOR_BTN_L))
  {
    m_view->SlideBagSelector(false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_BAG_SELECTOR_BTN_R))
  {
    m_view->SlideBagSelector(true);
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_BangleSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_BANGLE_SELECTOR_BTN_L))
  {
    m_view->SlideBangleSelector(false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_BANGLE_SELECTOR_BTN_R))
  {
    m_view->SlideBangleSelector(true);
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_CharmSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  return app::ui::UIInputListener::ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_SituationSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_SITUATION_BTN_L))
  {
    m_view->SlideSituationSelector(false);
  }

  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_SITUATION_BTN_R))
  {
    m_view->SlideSituationSelector(true);
  }

  return app::ui::UIInputListener::ENABLE_ACCESS;
}

app::ui::UIInputListener::ListenerResult CharaViewerListener::OnTouchEvent_ColorPalletSelector( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::util::G2DUtil* pG2DUtil = m_view->GetG2DUtil();

  // 肌
  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_SKINPALLET))
  {
    static const u32 sc_skinPaneList[] =
    {
      PANENAME_DOWN1_PANE_SKIN_PALLET_00,
      PANENAME_DOWN1_PANE_SKIN_PALLET_01,
      PANENAME_DOWN1_PANE_SKIN_PALLET_02,
      PANENAME_DOWN1_PANE_SKIN_PALLET_03,
    };
    for (u32 i = 0; i < sizeof(sc_skinPaneList) / sizeof(sc_skinPaneList[0]); ++i)
    {
      u32 pane = sc_skinPaneList[i];
      if(pG2DUtil->IsPaneTouchTrigger(0, pane))
      {
        m_view->SelectSkinColor(i);
        return app::ui::UIInputListener::ENABLE_ACCESS;
      }
    }
  }

  // 髪
  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_HAIRPALLET))
  {
    static const u32 sc_hairPaneList[] =
    {
      PANENAME_DOWN1_PANE_HAIR_PALLET_00,
      PANENAME_DOWN1_PANE_HAIR_PALLET_01,
      PANENAME_DOWN1_PANE_HAIR_PALLET_02,
      PANENAME_DOWN1_PANE_HAIR_PALLET_03,
      PANENAME_DOWN1_PANE_HAIR_PALLET_04,
      PANENAME_DOWN1_PANE_HAIR_PALLET_05,
      PANENAME_DOWN1_PANE_HAIR_PALLET_06,
      PANENAME_DOWN1_PANE_HAIR_PALLET_07,
      PANENAME_DOWN1_PANE_HAIR_PALLET_08,
    };
    for (u32 i = 0; i < sizeof(sc_hairPaneList) / sizeof(sc_hairPaneList[0]); ++i)
    {
      u32 pane = sc_hairPaneList[i];
      if(pG2DUtil->IsPaneTouchTrigger(0, pane))
      {
        m_view->SelectHairColor(i);
        return app::ui::UIInputListener::ENABLE_ACCESS;
      }
    }
  }

  // 服
  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CLOTHPALLET))
  {
    static const u32 sc_clothPaneList[] =
    {
      PANENAME_DOWN1_PANE_CLOTH_PALLET_00,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_01,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_02,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_03,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_04,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_05,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_06,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_07,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_08,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_09,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_10,

      PANENAME_DOWN1_PANE_CLOTH_PALLET_11,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_12,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_13,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_14,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_15,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_16,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_17,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_18,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_19,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_20,

      PANENAME_DOWN1_PANE_CLOTH_PALLET_21,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_22,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_23,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_24,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_25,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_26,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_27,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_28,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_29,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_30,

      PANENAME_DOWN1_PANE_CLOTH_PALLET_31,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_32,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_33,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_34,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_35,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_36,
    };
    for (u32 i = 0; i < sizeof(sc_clothPaneList) / sizeof(sc_clothPaneList[0]); ++i)
    {
      u32 pane = sc_clothPaneList[i];
      if(pG2DUtil->IsPaneTouchTrigger(0, pane))
      {
        m_view->SelectClothColor(i);
        return app::ui::UIInputListener::ENABLE_ACCESS;
      }
    }
  }

  // コンタクト
  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_CONTACTPALLET))
  {
    static const u32 sc_contactPaneList[] =
    {
      PANENAME_DOWN1_PANE_CONTACT_PALLET_00,
      PANENAME_DOWN1_PANE_CONTACT_PALLET_01,
      PANENAME_DOWN1_PANE_CONTACT_PALLET_02,
      PANENAME_DOWN1_PANE_CONTACT_PALLET_03,
      PANENAME_DOWN1_PANE_CONTACT_PALLET_04,
      PANENAME_DOWN1_PANE_CONTACT_PALLET_05,
      PANENAME_DOWN1_PANE_CONTACT_PALLET_06,
      PANENAME_DOWN1_PANE_CONTACT_PALLET_07,
      PANENAME_DOWN1_PANE_CONTACT_PALLET_08,
    };

    for (u32 i = 0; i < sizeof(sc_contactPaneList) / sizeof(sc_contactPaneList[0]); ++i)
    {
      u32 pane = sc_contactPaneList[i];
      if(pG2DUtil->IsPaneTouchTrigger(0, pane))
      {
        m_view->SelectContactColor(i);
        return app::ui::UIInputListener::ENABLE_ACCESS;
      }
    }
  }

  // リップ
  if(pG2DUtil->IsPaneTouchTrigger(0, PANENAME_DOWN1_PANE_LIPPALLET))
  {
    static const u32 sc_lipPaneList[] =
    {
      PANENAME_DOWN1_PANE_LIP_PALLET_00,
      PANENAME_DOWN1_PANE_LIP_PALLET_01,
      PANENAME_DOWN1_PANE_LIP_PALLET_02,
      PANENAME_DOWN1_PANE_LIP_PALLET_03,
      PANENAME_DOWN1_PANE_LIP_PALLET_04,
      PANENAME_DOWN1_PANE_LIP_PALLET_05,
      PANENAME_DOWN1_PANE_LIP_PALLET_06,
    };

    for (u32 i = 0; i < sizeof(sc_lipPaneList) / sizeof(sc_lipPaneList[0]); ++i)
    {
      u32 pane = sc_lipPaneList[i];
      if(pG2DUtil->IsPaneTouchTrigger(0, pane))
      {
        m_view->SelectLipColor(i);
        return app::ui::UIInputListener::ENABLE_ACCESS;
      }
    }
  }


  return app::ui::UIInputListener::ENABLE_ACCESS;
}

GFL_NAMESPACE_END(chara_viewer)
  GFL_NAMESPACE_END(test)

#endif // PM_DEBUG
