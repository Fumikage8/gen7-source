// ============================================================================
/*
 * @file KisekaeLowerItemSelectView.cpp
 * @brief きせかえアプリのアイテム選択の下画面を表示するクラスです。
 * @date  2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Kisekae/source/View/KisekaeLowerItemSelectView.h"

#include "App/Kisekae/source/KisekaeResourceID.h"
#include "App/Kisekae/source/System/KisekaeWork.h"

#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

//  nijiのインクルード
#include "AppLib/include/util/app_util_GetPaneHelper.h"
#include "AppLib/include/util/app_util_KeyRepeatController.h"
#include "AppLib/include/Tool/CommonMessageObject.h"
#include "AppLib/include/Tool/AppCommonGrpIconData.h"

#include "Savedata/include/MiscSave.h"

//  リソースのインクルード
#include <arc_index/message.gaix>
#include <niji_conv_header/app/shop_kisekae/res2d/ShopKisekaeLytData.h>
#include <niji_conv_header/app/shop_kisekae/res2d/ShopKisekaeLytData_anim_list.h>
#include <niji_conv_header/app/shop_kisekae/res2d/ShopKisekaeLytData_pane.h>

#include <niji_conv_header/message/msg_kisekae.h>
#include <niji_conv_header/sound/SoundMiddleID.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(View)

  LowerItemSelectView::LowerItemSelectView( App::Kisekae::System::Work* pWork ) :
    NetAppLib::System::NetApplicationViewBase(
      pWork,
      App::Kisekae::RESOURCE_ID_LAYOUT,
      LYTRES_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000_BFLYT,
      LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000___NUM,
      app::util::G2DUtil::SETUP_LOWER,
      pWork->GetMessageUtility(),
      GARC_message_kisekae_DAT )
  {
    _Clear();

    GFL_ASSERT( pWork );
    m_pWork = pWork;

    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    app::util::Heap*  pUseAppHeap = m_pWork->GetAppHeap();

    gfl2::lyt::LytMultiResID& rResourceID = pG2DUtil->GetLayoutResourceID( 0 );
    gfl2::lyt::LytWk* pLayoutWork = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
    GFL_ASSERT( pLayoutWork );

    //  メニューカーソル作成
    {
      m_pMenuCursor = GFL_NEW( pUseAppHeap->GetDeviceHeap() ) NetAppLib::UI::NetAppMenuCursor( m_pWork, GetG2DUtil(), NetAppLib::System::COMMON_RESOURCE_ID_MENU_CURSOR, 0, LAYOUT_WORK_ID_LOWER );
    }

    //  ペインリスト作成
    {
      //  PaneListViewを作成
      {
        //  PaneLisstView専用のヒープを作成
        //  なぜなら何度も読み変えるから
        {
          m_pPaneListViewHeap = GFL_NEW( pUseAppHeap->GetDeviceHeap() ) app::util::Heap();
          m_pPaneListViewHeap->LocalHeapCreate( pUseAppHeap->GetSystemHeap(), pUseAppHeap->GetDeviceHeap(), 1024 * 100, 1024 * 500 );
        }

        m_pPaneListView = GFL_NEW( pUseAppHeap->GetDeviceHeap() ) app::tool::PaneListView( m_pPaneListViewHeap, true );
      }

      // -----------------------------------------------------------------------
      { // m_aPaneListData 初期化 ここから
        const SETUP_ITEM_DATA aSetupItemData[ ITEM_LIST_MAX ]  =
        {
          //  menu01
          {
            PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_BUTTON_00,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_00_TOUCH,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_00_RELEASE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_00_CANSEL,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_00_TOUCH_RELEASE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_00_ACTIVE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_00_PASSIVE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_00__NEWICON_LOOP,
          },

          //  menu02
          {
            PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_BUTTON_01,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_01_TOUCH,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_01_RELEASE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_01_CANSEL,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_01_TOUCH_RELEASE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_01_ACTIVE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_01_PASSIVE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_01__NEWICON_LOOP,
          },

          //  menu03
          {
            PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_BUTTON_02,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_02_TOUCH,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_02_RELEASE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_02_CANSEL,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_02_TOUCH_RELEASE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_02_ACTIVE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_02_PASSIVE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_02__NEWICON_LOOP,
          },

          //  menu04
          {
            PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_BUTTON_03,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_03_TOUCH,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_03_RELEASE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_03_CANSEL,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_03_TOUCH_RELEASE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_03_ACTIVE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_03_PASSIVE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_03__NEWICON_LOOP,
          },

          //  menu05
          {
            PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_BUTTON_04,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_04_TOUCH,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_04_RELEASE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_04_CANSEL,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_04_TOUCH_RELEASE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_04_ACTIVE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_04_PASSIVE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_04__NEWICON_LOOP,
          },

          //  menu06
          {
            PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_BUTTON_05,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_05_TOUCH,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_05_RELEASE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_05_CANSEL,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_05_TOUCH_RELEASE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_05_ACTIVE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_05_PASSIVE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_05__NEWICON_LOOP,
          },

          //  menu07
          {
            PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_BUTTON_06,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_06_TOUCH,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_06_RELEASE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_06_CANSEL,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_06_TOUCH_RELEASE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_06_ACTIVE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_06_PASSIVE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_06__NEWICON_LOOP,
          },
        };

        u32 loopNum = GFL_NELEMS( aSetupItemData );
        for( u32 i = 0; i < loopNum; ++i )
        {
          const SETUP_ITEM_DATA*  pSetupItemData  = &aSetupItemData[ i ];

          app::util::GetPaneHelper  paneHelper( pLayoutWork, &rResourceID );

          paneHelper.Push( pSetupItemData->partsPaneId );

          //  paneList用のデータ
          {
            m_aPaneListData[i].base       = paneHelper.Peek();
            m_aPaneListData[i].text       = NULL;
            m_aPaneListData[i].bound      = paneHelper.GetBoundingPane( PANENAME_SHOPKISEKAE_BTN_LOW_000_PANE_BOUND_00 );
            m_aPaneListData[i].cursor_pos = paneHelper.GetPane( PANENAME_SHOPKISEKAE_BTN_LOW_000_PANE_NULL_CUR );

            m_aPaneListData[i].holdAnime    = pSetupItemData->holdAnimeId;
            m_aPaneListData[i].releaseAnime = pSetupItemData->releaseAnimeId;
            m_aPaneListData[i].cancelAnime  = pSetupItemData->cancelAnimeId;
            m_aPaneListData[i].selectAnime  = pSetupItemData->selectAnimeId;
          }

          //  選択項目個別のデータ
          {
            m_aSelectItem[ i ].Setup( this, m_pWork, *pSetupItemData, m_aPaneListData[ i ], pUseAppHeap->GetDeviceHeap() );
          }
        }
      } // m_aPaneListData 初期化 ここまで
      // -----------------------------------------------------------------------

      //  ボタン作成
      {
        struct SETUP_BTN_DATA
        {
          gfl2::lyt::LytParts*  pPartsPane;
          u32 seId;
          //  カテゴリー用のセットアップデータ
          const CATEGORY_SETUP_DATA*  pCategorySetupData;
        };

        //  リストの順序が変わるとここを並べなおさないといけない
        const CATEGORY_SETUP_DATA aCategorySetupData[ CATEGORY_ITEM_MAX ]  =
        {
          //  トップス
          {
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_00_ITEM_01,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_00_ITEM_00,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_00_SELECT_00,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_00_UNSELECT_00,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_00_SELECT_KEEP_00,

            kisekae_02_04
          },

          //  ボトムス
          {
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_01_ITEM_01,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_01_ITEM_00,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_01_SELECT_01,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_01_UNSELECT_01,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_01_SELECT_KEEP_00,

            kisekae_02_05
          },

          //  くつした
          {
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_02_ITEM_01,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_02_ITEM_00,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_02_SELECT_02,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_02_UNSELECT_02,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_02_SELECT_KEEP_00,

            kisekae_02_06
          },

          //  くつ
          {
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_03_ITEM_01,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_03_ITEM_00,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_03_SELECT_03,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_03_UNSELECT_03,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_03_SELECT_KEEP_00,

            kisekae_02_07
          },

          //  バッグ
          {
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_04_ITEM_01,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_04_ITEM_00,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_04_SELECT_04,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_04_UNSELECT_04,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_04_SELECT_KEEP_00,

            kisekae_02_08
          },

          //  ぼうし
          {
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_05_ITEM_01,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_05_ITEM_00,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_05_SELECT_05,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_05_UNSELECT_05,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_05_SELECT_KEEP_00,

            kisekae_02_01
          },

          //  アイウェア
          {
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_06_ITEM_01,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_06_ITEM_00,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_06_SELECT_06,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_06_UNSELECT_06,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_06_SELECT_KEEP_00,

            kisekae_02_03
          },

          //  かみかざり
          {
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_07_ITEM_01,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_07_ITEM_00,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_07_SELECT_07,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_07_UNSELECT_07,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__CATEGORYBUTTON_07_SELECT_KEEP_00,

            kisekae_02_02
          }
        };

        const SETUP_BTN_DATA  aSetupBtnData[ BUTTON_ID_MAX ]  =
        {
          { pLayoutWork->GetPartsPane( PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_BACKBUTTON ), SEQ_SE_CANCEL1, NULL },
          { pLayoutWork->GetPartsPane( PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_BUTTON_X ), SEQ_SE_DECIDE1, NULL },
          { pLayoutWork->GetPartsPane( PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_BUTTON_Y ), SEQ_SE_COMPLETE1, NULL },

          //  カテゴリー順にする
          { pLayoutWork->GetPartsPane( PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_CATEGORYBUTTON_00 ), SEQ_SE_PAGE2, &aCategorySetupData[ 0 ] },
          { pLayoutWork->GetPartsPane( PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_CATEGORYBUTTON_01 ), SEQ_SE_PAGE2, &aCategorySetupData[ 1 ] },
          { pLayoutWork->GetPartsPane( PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_CATEGORYBUTTON_02 ), SEQ_SE_PAGE2, &aCategorySetupData[ 2 ] },
          { pLayoutWork->GetPartsPane( PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_CATEGORYBUTTON_03 ), SEQ_SE_PAGE2, &aCategorySetupData[ 3 ] },
          { pLayoutWork->GetPartsPane( PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_CATEGORYBUTTON_04 ), SEQ_SE_PAGE2, &aCategorySetupData[ 4 ] },
          { pLayoutWork->GetPartsPane( PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_CATEGORYBUTTON_05 ), SEQ_SE_PAGE2, &aCategorySetupData[ 5 ] },
          { pLayoutWork->GetPartsPane( PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_CATEGORYBUTTON_06 ), SEQ_SE_PAGE2, &aCategorySetupData[ 6 ] },
          { pLayoutWork->GetPartsPane( PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_CATEGORYBUTTON_07 ), SEQ_SE_PAGE2, &aCategorySetupData[ 7 ] }
        };

        const app::ui::ButtonInfoEx aButtonPaneIdxList[ BUTTON_ID_MAX ] =
        {
        /*
          app::tool::ButtonManager::ButtonId button_id;
          gfl2::lyt::LytPane* picture_pane;
          gfl2::lyt::LytPane* bound_pane;
          u32                 touch_anime_index;        //!< タッチ時のアニメ
          u32                 release_anime_index;      //!< タッチ決定時のアニメ
          u32                 cancel_anime_index;       //!< タッチ状態からのキャンセルアニメ
          u32                 key_select_anime_index;   //!< キー決定時のアニメ ( touch => release の連続アニメ )
          u32                 active_anime_index;       //!< アクティブアニメ
          u32                 passive_anime_index;      //!< パッシブアニメ
        */
          //  Bキャンセル
          {
            BUTTON_ID_CANCEL,
            aSetupBtnData[ BUTTON_ID_CANCEL ].pPartsPane,
            pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_CANCEL ].pPartsPane, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &rResourceID ),
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BACKBUTTON_TOUCH,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BACKBUTTON_RELEASE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BACKBUTTON_CANSEL,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BACKBUTTON_TOUCH_RELEASE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BACKBUTTON_ACTIVE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BACKBUTTON_PASSIVE,
          },

          //  すがたを見る
          {
            BUTTON_ID_VIEWR,
            aSetupBtnData[ BUTTON_ID_VIEWR ].pPartsPane,
            pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_VIEWR ].pPartsPane, PANENAME_COMMON_BTN_LOW_015_PANE_BOUND_00, &rResourceID ),
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_X_TOUCH,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_X_RELEASE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_X_CANSEL,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_X_TOUCH_RELEASE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_X_ACTIVE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_X_PASSIVE,
          },

          //  きがえる
          {
            BUTTON_ID_DRESS_UP,
            aSetupBtnData[ BUTTON_ID_DRESS_UP ].pPartsPane,
            pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_DRESS_UP ].pPartsPane, PANENAME_COMMON_BTN_LOW_015_PANE_BOUND_00, &rResourceID ),
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_Y_TOUCH,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_Y_RELEASE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_Y_CANSEL,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_Y_TOUCH_RELEASE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_Y_ACTIVE,
            LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__BUTTON_Y_PASSIVE,
          },

          //  カテゴリー
          {
            BUTTON_ID_CATEGORY_00,
            aSetupBtnData[ BUTTON_ID_CATEGORY_00 ].pPartsPane,
            pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_CATEGORY_00 ].pPartsPane, PANENAME_SHOPKISEKAE_BTN_LOW_001_PANE_BOUND_00, &rResourceID ),
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL
          },

          //  カテゴリー
          {
            BUTTON_ID_CATEGORY_01,
            aSetupBtnData[ BUTTON_ID_CATEGORY_01 ].pPartsPane,
            pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_CATEGORY_01 ].pPartsPane, PANENAME_SHOPKISEKAE_BTN_LOW_001_PANE_BOUND_00, &rResourceID ),
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL
          },

          //  カテゴリー
          {
            BUTTON_ID_CATEGORY_02,
            aSetupBtnData[ BUTTON_ID_CATEGORY_02 ].pPartsPane,
            pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_CATEGORY_02 ].pPartsPane, PANENAME_SHOPKISEKAE_BTN_LOW_001_PANE_BOUND_00, &rResourceID ),
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL
          },

          //  カテゴリー
          {
            BUTTON_ID_CATEGORY_03,
            aSetupBtnData[ BUTTON_ID_CATEGORY_03 ].pPartsPane,
            pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_CATEGORY_03 ].pPartsPane, PANENAME_SHOPKISEKAE_BTN_LOW_001_PANE_BOUND_00, &rResourceID ),
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL
          },

          //  カテゴリー
          {
            BUTTON_ID_CATEGORY_04,
            aSetupBtnData[ BUTTON_ID_CATEGORY_04 ].pPartsPane,
            pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_CATEGORY_04 ].pPartsPane, PANENAME_SHOPKISEKAE_BTN_LOW_001_PANE_BOUND_00, &rResourceID ),
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL
          },

          //  カテゴリー
          {
            BUTTON_ID_CATEGORY_05,
            aSetupBtnData[ BUTTON_ID_CATEGORY_05 ].pPartsPane,
            pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_CATEGORY_05 ].pPartsPane, PANENAME_SHOPKISEKAE_BTN_LOW_001_PANE_BOUND_00, &rResourceID ),
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL
          },

          //  カテゴリー
          {
            BUTTON_ID_CATEGORY_06,
            aSetupBtnData[ BUTTON_ID_CATEGORY_06 ].pPartsPane,
            pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_CATEGORY_06 ].pPartsPane, PANENAME_SHOPKISEKAE_BTN_LOW_001_PANE_BOUND_00, &rResourceID ),
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL
          },

          //  カテゴリー
          {
            BUTTON_ID_CATEGORY_07,
            aSetupBtnData[ BUTTON_ID_CATEGORY_07 ].pPartsPane,
            pLayoutWork->GetBoundingPane( aSetupBtnData[ BUTTON_ID_CATEGORY_07 ].pPartsPane, PANENAME_SHOPKISEKAE_BTN_LOW_001_PANE_BOUND_00, &rResourceID ),
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL,
            app::tool::ButtonManager::ANIMATION_NULL
          },
        };

        UIResponder::CreateButtonManager( pUseAppHeap, pLayoutWork, aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );
 
        //  入力コールバック設定
        UIView::SetInputListener( this );

        //  ボタンSEの設定
        {
          app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();

          u32 loopNum = GFL_NELEMS( aSetupBtnData );
          for( u32 i = 0; i < loopNum; ++i )
          {
            pButtonManager->SetButtonSelectSE( i, aSetupBtnData[ i ].seId );
          }
        }

        //  カテゴリーボタン生成
        {
          //  カテゴリー数が一致していないとアウト
          GFL_ASSERT( Savedata::Fashion::CATEGORY_MAX == GFL_NELEMS( m_aCategoryItem ) );

          const App::Kisekae::View::LowerItemSelectView::ButtonId aCategoryBtnId[]  =
          {
            App::Kisekae::View::LowerItemSelectView::BUTTON_ID_CATEGORY_00,
            App::Kisekae::View::LowerItemSelectView::BUTTON_ID_CATEGORY_01,
            App::Kisekae::View::LowerItemSelectView::BUTTON_ID_CATEGORY_02,
            App::Kisekae::View::LowerItemSelectView::BUTTON_ID_CATEGORY_03,
            App::Kisekae::View::LowerItemSelectView::BUTTON_ID_CATEGORY_04,
            App::Kisekae::View::LowerItemSelectView::BUTTON_ID_CATEGORY_05,
            App::Kisekae::View::LowerItemSelectView::BUTTON_ID_CATEGORY_06,
            App::Kisekae::View::LowerItemSelectView::BUTTON_ID_CATEGORY_07,
          };

          u32 loopNum = GFL_NELEMS( aCategoryBtnId );
          for( u32 i = 0; i < loopNum; ++i )
          {
            GFL_ASSERT( aSetupBtnData[ aCategoryBtnId[ i ] ].pCategorySetupData != NULL );
            m_aCategoryItem[ i ].Init( this, aButtonPaneIdxList[ aCategoryBtnId[ i ] ], *aSetupBtnData[ aCategoryBtnId[ i ] ].pCategorySetupData );
          }
        }
      }
    }

    //  固定テキスト設定
    {
      //  すがたをみる
      {
        app::util::GetPaneHelper  paneHelper( pLayoutWork, &rResourceID );
        paneHelper.Push( PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_BUTTON_X );

        pG2DUtil->SetTextBoxPaneString( paneHelper.GetTextBoxPane( PANENAME_COMMON_BTN_LOW_015_PANE_TEXTBOX_00 ), kisekae_01_05 );
      }

      //  きがえる
      {
        app::util::GetPaneHelper  paneHelper( pLayoutWork, &rResourceID );
        paneHelper.Push( PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_BUTTON_Y );

        pG2DUtil->SetTextBoxPaneString( paneHelper.GetTextBoxPane( PANENAME_COMMON_BTN_LOW_015_PANE_TEXTBOX_00 ), kisekae_01_06 );
      }
    }

    //  下側のウィンドウ作成
    {
      m_pLowerCommonMessageObject = GFL_NEW( pUseAppHeap->GetDeviceHeap() ) app::tool::CommonMessageObject();

      app::util::GetPaneHelper  paneHelper( pLayoutWork, &rResourceID );
      paneHelper.Push( PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_MESSAGE_L1 );

      app::tool::CommonMessageObject::INIT_PARAM  initParam;
      {
        initParam.lytId         = LAYOUT_WORK_ID_LOWER;
        initParam.pG2DUtil      = pG2DUtil;
        initParam.inAnimeId     = LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__MESSAGE_L1_IN;
        initParam.outAnimeId    = LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000__MESSAGE_L1_OUT;
        initParam.pWindowPane   = paneHelper.Peek();
        initParam.showWaitFrame = 0;

        //  テキストペイン設定
        {
          initParam.pTextBoxPaneArray[ 0 ]  = paneHelper.GetTextBoxPane( PANENAME_COMMON_MSG_LOW_001_PANE_TEXTBOX_00 );
          initParam.textBoxPaneNum  = 1;
        }
      }

      m_pLowerCommonMessageObject->InitFunc( initParam, pUseAppHeap->GetDeviceHeap() );
    }

    //  ペイン取得
    {
      app::util::GetPaneHelper  paneHelper( pLayoutWork, &rResourceID );
      m_pErrorMessageTextPane = paneHelper.GetTextBoxPane( PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_ERRORMESSAGE );
      GFL_ASSERT( m_pErrorMessageTextPane );
    }

    //  カテゴリーキー操作
    {
      m_pCategoryKeyRepatCtrl = GFL_NEW( pUseAppHeap->GetDeviceHeap() ) app::util::KeyRepeatController( 0, CATEGORY_ITEM_MAX - 1 );
    }
  }

  LowerItemSelectView::~LowerItemSelectView()
  {
    UIView::RemoveAllSubView();

    if( m_pPaneListView != NULL )
    {
      m_pPaneListView->DeleteScrollBar();
      m_pPaneListView->DeletePaneList();
    }

    m_pLowerCommonMessageObject->EndFunc();

    m_pMenuCursor->Destroy();

    GFL_SAFE_DELETE( m_pCategoryKeyRepatCtrl );
    GFL_SAFE_DELETE( m_pMenuCursor );
    GFL_SAFE_DELETE( m_pLowerCommonMessageObject );
    GFL_SAFE_DELETE( m_pPaneListView )
    GFL_SAFE_DELETE( m_pPaneListViewHeap );
  }

  bool LowerItemSelectView::InitFunc()
  {
    bool  bNext = true;

    u32 loopNum = GFL_NELEMS( m_aSelectItem );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( m_aSelectItem[ i ].InitFunc() == false )
      {
        bNext = false;
      }
    }

    if( bNext == true )
    {
      return  true;
    }

    return  false;
  }

  bool LowerItemSelectView::EndFunc()
  {
    bool  bNext = true;

    u32 loopNum = GFL_NELEMS( m_aSelectItem );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( m_aSelectItem[ i ].EndFunc() == false )
      {
        bNext = false;
      }
    }

    if( bNext == true )
    {
      return  true;
    }

    return  false;
  }

  bool LowerItemSelectView::IsDrawing(void) const
  {
    bool result = false;
  
    if( m_pPaneListView->IsDrawing() == true )
    {
      result  = true;
    }
    else
    if( UIView::IsDrawing() == true )
    {
      result = true;
    }
    else
    if( m_pMenuCursor->CanDestroy() == false )
    {
      result  = true;
    }

    return result;
  }

  //  開始イベント実行
  void LowerItemSelectView::StartEvent()
  {
    _SetState( STATE_START_EVENT );
  }

  //  リストアイテムを取得
  LowerItemSelectView::SelectItem* LowerItemSelectView::GetListItem( const u32 paneListIdx )
  {
    if( GFL_NELEMS( m_aSelectItem ) <= paneListIdx )
    {
      GFL_ASSERT( 0 );
      return  NULL;
    }

    return  &m_aSelectItem[ paneListIdx ];
  }

  //  カテゴリアイテムを取得
  LowerItemSelectView::CategoryItem* LowerItemSelectView::GetCategoryItem( const u32 categoryListIdx )
  {
    if( GFL_NELEMS( m_aCategoryItem ) <= categoryListIdx )
    {
      GFL_ASSERT( 0 );
      return  NULL;
    }

    return  &m_aCategoryItem[ categoryListIdx ];
  }

  //  リスト作成
  void LowerItemSelectView::CreateList( App::Kisekae::System::ItemList* pItemList, const u32 listDrawPriority, const Mode mode, const bool bVisibleMoney, const bool bFirst )
  {
    GFL_ASSERT( pItemList );
    m_pItemList = pItemList;

    //  リストアイテムモード設定
    {
      u32 loopNum = GFL_NELEMS( m_aSelectItem );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_aSelectItem[ i ].SetMode( mode );
        m_aSelectItem[ i ].SetVisibleMoney( bVisibleMoney );
      }
    }

    u32 itemListNum = m_pItemList->GetDataMax();
    _CreatePaneList( itemListNum, m_pItemList->GetListScroll(), m_pItemList->GetListPos(), listDrawPriority );

    //  カテゴリータブ更新
    {
      u32 category      = m_pItemList->GetActiveCategory();
      m_acitveCategoryIdx = category;
    }

    CategoryItem* pActiveCategoryItem = &m_aCategoryItem[ m_acitveCategoryIdx ];

    if( bFirst == true )
    {
      //  全カテゴリーボタン更新
      u32 loopNum = GFL_NELEMS( m_aCategoryItem );
      for( u32 i = 0; i < loopNum; ++i )
      {
        if( pItemList->GetDataMax( i ) <= 0 )
        {
          m_aCategoryItem[ i ].SelectItem( false );
        }
        else
        {
          m_aCategoryItem[ i ].SelectItem( true );
        }

        if( i != m_acitveCategoryIdx )
        {
          m_aCategoryItem[ i ].SelectBtn( false, true );
        }
      }

      pActiveCategoryItem->SelectBtn( true, bFirst );
    }
    else
    {
      u32 oldCategory   = m_pItemList->GetOldCategory();

      if( m_acitveCategoryIdx != oldCategory )
      {
        //  カテゴリーに変化があった場合は切り替え
        pActiveCategoryItem->SelectBtn( true );
        m_aCategoryItem[ oldCategory ].SelectBtn( false );
      }
      else
      {
      }

      if( 0 < itemListNum )
      {
        pActiveCategoryItem->SelectItem( true );
      }
      else
      {
        pActiveCategoryItem->SelectItem( false );
      }
    }

    //  リストが0ならエラーメッセージを表示
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    if( itemListNum <= 0 )
    {
      u32 strId = pActiveCategoryItem->GetNonItemMsgId();
      if( mode == MODE_COLOR_DRESS_UP )
      {
        strId = kisekae_12_01;
      }

      pG2DUtil->SetPaneVisible( m_pErrorMessageTextPane, true );
      pG2DUtil->SetTextBoxPaneString( m_pErrorMessageTextPane, strId );

      // @fix 吉田さん要望　リストが０の時は専用ペイン表示
      pG2DUtil->SetPaneVisible( LAYOUT_WORK_ID_LOWER, PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_BG_ERROR, true );
    }
    else
    {
      pG2DUtil->SetPaneVisible( m_pErrorMessageTextPane, false );

      // @fix 吉田さん要望　リストがある時は専用ペイン非表示
      pG2DUtil->SetPaneVisible( LAYOUT_WORK_ID_LOWER, PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_BG_ERROR, false );
    }
  }

  //  リストのカーソルスクロールと位置を取得
  f32 LowerItemSelectView::GetListScroll()
  {
    f32 listScroll  = 0.f;
    m_pPaneListView->GetPaneList()->GetCursorData( NULL, &listScroll );

    return  listScroll;
  }

  u32 LowerItemSelectView::GetListPos()
  {
    u32 listPos = 0;
    m_pPaneListView->GetPaneList()->GetCursorData( &listPos, NULL );

    return  listPos;
  }

  void LowerItemSelectView::Update( void )
  {
    if( m_bRefresh == true )
    {
      //  リストを再更新
      m_pPaneListView->GetPaneList()->InitListPut();
      m_bRefresh = false;
    }

    if( m_state == STATE_START_EVENT )
    {
      app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
      if( pG2DUtil->IsAnimeEnd( LAYOUT_WORK_ID_LOWER, LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000_INANIM ) == true )
      {
        _SetState( STATE_IDLE );
      }
    }

    if( m_state == STATE_IDLE )
    {
      //  リスト移動をしていない状態の場合はカーソルヒットのコールバックを呼び続ける
      {
        if( ( m_pPaneListView->IsInputEnabled() == false ) || m_pPaneListView->GetUpdateResult() != app::tool::PaneList::RET_NONE )
        {
          //  リストが動いている
        }
        else
        {
          //  リストが静止状態
          if( m_pDelegate != NULL )
          {
            u32  localCursorPos  = 0;
            f32  scroll = 0.0f;
            m_pPaneListView->GetPaneList()->GetCursorData( &localCursorPos, &scroll );

            //  ローカルカーソルとスクロール値で全リストデータのidxになる
            App::Kisekae::System::ItemDataBase* pItemData = m_pItemList->GetData( localCursorPos + (u32)scroll );

            //  画面に表示する選択アイテムを取得
            SelectItem*   pSelectItem = GetListItem( m_pPaneListView->GetPaneList()->GetPosPaneIndex( localCursorPos ) );

            m_pDelegate->LowerItemSelectViewDelegate_OnCursorHitItem( pItemData, pSelectItem );
          }
        }
      }
    }

    if( m_state == STATE_INPUT_EVENT_WAIT )
    {
      if( ( UIView::IsButtonAnimation() == false )
      &&  ( m_pPaneListView->IsButtonAnimation() == false ) )
      {
        _SetState( STATE_INPUT_STOP );
      }
    }

    NetAppLib::System::NetApplicationViewBase::Update();

    {
      u32 loopNum = GFL_NELEMS( m_aSelectItem );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_aSelectItem[ i ].UpdateFunc();
      }
    }

    m_pLowerCommonMessageObject->Update();
    m_pMenuCursor->Update();
  }

  void LowerItemSelectView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    app::util::AppRenderingManager* pAppRenderingManager = m_pWork->GetAppRenderingManager();
    pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );

    m_pMenuCursor->Draw( displayNo );
  }

  //  アイドリングを開始
  void LowerItemSelectView::StartIdling()
  {
    //  開始イベント中は変更できない
    if( m_state != STATE_START_EVENT )
    {
      _SetState( STATE_IDLE );
    }
  }

  //  ボタンアクティブ設定
  void LowerItemSelectView::SetActionButton( const LowerItemSelectView::ButtonId buttonId, const bool bFlg )
  {
    if( BUTTON_ID_MAX <= buttonId )
    {
      GFL_ASSERT( 0 );
      return;
    }

    if( bFlg == true )
    {
      UIView::GetButtonManager()->SetButtonActive( buttonId, true );
    }
    else
    {
      UIView::GetButtonManager()->SetButtonPassive( buttonId, true );
    }
  }

  void LowerItemSelectView::ShowMessageLower( gfl2::str::StrBuf* pStr, const bool bIsImmediateMessage )
  {
    GFL_ASSERT( pStr );

    m_pLowerCommonMessageObject->ShowMessage( *pStr, bIsImmediateMessage );

    //  ボタンを消す
    {
      UIView::GetButtonManager()->SetButtonInvisible( BUTTON_ID_VIEWR );
      UIView::GetButtonManager()->SetButtonInvisible( BUTTON_ID_DRESS_UP );
    }
  }

  void LowerItemSelectView::HideMessageLower()
  {
    m_pLowerCommonMessageObject->HideMessage();

    //  ボタンを出す
    {
      UIView::GetButtonManager()->SetButtonVisible( BUTTON_ID_VIEWR );
      UIView::GetButtonManager()->SetButtonVisible( BUTTON_ID_DRESS_UP );
    }
  }

  //  画面リフレッシュ
  void LowerItemSelectView::Refresh()
  {
    // @fix NMCat[555] すぐに実行しない、viewのupdateでやらないとアニメ設定が即時反映しないので表示が１フレームおかしくなる。
    //                 フラグを立ててUpdate関数で実行
    m_bRefresh = true;
  }

  void LowerItemSelectView::CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )
  {
    UCHIDA_PRINT( "CallBack_CursorMove DataList ->pos %d\n", pos );

    if( m_pDelegate != NULL )
    {
      //!< コールバックへ引き渡すペインが存在しない
      SelectItem* pSelectItem = NULL;
      if( pane_index != app::tool::PaneList::CALLBACK_PANE_NONE )
      {
        pSelectItem = GetListItem( pane_index );
      }
      
      m_pDelegate->LowerItemSelectViewDelegate_OnCursorItem( m_pItemList->GetData( pos ), pSelectItem );
    }
  }

  void LowerItemSelectView::CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
  {
  }

  void LowerItemSelectView::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
  {
    GFL_ASSERT( pane );
    
    UCHIDA_PRINT( "CallBack_ListWrite DataList ->pos %d\n", pos );

    //  パラメータを参照して表示
    SelectItem* pSelectItem = GetListItem( pane_index );
    if( pSelectItem == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

    App::Kisekae::System::ItemDataBase*  data  = m_pItemList->GetData( pos );

    app::tool::PaneList*  pPaneList = m_pPaneListView->GetPaneList();

    bool  bActive  = true;
    if( m_pDelegate != NULL )
    {
      bActive  = m_pDelegate->LowerItemSelectViewDelegate_OnIsDressupOk( data );
    }

    if( bActive == true )
    {
      //  アクティブ状態で購入済みかどうかをチェックして購入していれば非アクティブにする
      if( data->IsBuy() == true )
      {
        bActive = false;
      }

      // @fix NMCat[2633] 押せるので決定SEに
      pSelectItem->SetPushSEId( SEQ_SE_DECIDE1 );
    }
    else
    {
      // @fix NMCat[2633] 押せないのでビープSEに
      pSelectItem->SetPushSEId( SEQ_SE_BEEP );
    }

    // @fix NMCat[2633] 決定SEを設定
    if( pPaneList != NULL )
    {
      pPaneList->SetButtonSEPaneIndex( pane_index, pSelectItem->GetPushSEId() );
    }

    //  アイテム内容更新
    pSelectItem->SetData( data, m_pWork->GetMessageUtility() );
    pSelectItem->SetActive( bActive );

    if( m_pDelegate != NULL )
    {
      m_pDelegate->LowerItemSelectViewDelegate_OnUpdateItem( data, pSelectItem );
    }
  }

  void LowerItemSelectView::CallBack_ItemChange( void * work, u32 pos1, u32 pos2 )
  {
  }

  void LowerItemSelectView::PaneListItemSelectListener( u32 pos )
  {
    UCHIDA_PRINT( "Push DataList\n" );

    SelectItem* pSelectItem = NULL;
    {
      u32  cursorPos  = 0;
      m_pPaneListView->GetPaneList()->GetCursorData( &cursorPos, NULL );
      m_pushPaneListIdx = m_pPaneListView->GetPaneList()->GetPosPaneIndex( cursorPos );

      pSelectItem = GetListItem( m_pushPaneListIdx );
    }

    if( pSelectItem == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

    if( m_pDelegate != NULL )
    {
      App::Kisekae::System::ItemDataBase* pItemData = m_pItemList->GetData( pos );
      m_pDelegate->LowerItemSelectViewDelegate_OnCursorItem( pItemData, pSelectItem );

      if( m_pDelegate->LowerItemSelectViewDelegate_OnPushItem( pItemData, pSelectItem ) == false )
      {
        return;
      }
    }

    m_tmpPushId = BUTTON_ID_ITEM;
    m_pushItemIdx = pos;

    _SetState( STATE_INPUT_EVENT_WAIT );
  }

  //------------------------------------------------------------------
  /**
    * @brief   ペインタッチ時を検知
    * @param[in] buttonId ボタンid
    * @return 入力処理を続けるかを返す
    */
  //------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult LowerItemSelectView::OnLayoutPaneEvent( const u32 id )
  {
    //  idle状態でないと入力は受け付けない
    if( m_state != STATE_IDLE )
    {
      return  ENABLE_ACCESS;
    }

    m_tmpPushId = (ButtonId)id;

    _SetState( STATE_INPUT_EVENT_WAIT );

    return  DISABLE_ACCESS;
  }

  //------------------------------------------------------------------
  /**
    * @brief   キー入力通知用リスナ
    * @param[in] pButton  ボタン入力の情報
    * @param[in] pKey     キー入力の情報
    * @param[in] pStick   スティック入力の情報
    * @return 入力処理を続けるかを返す
    */
  //------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult LowerItemSelectView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    //  idle状態でないと入力は受け付けない
    if( m_state != STATE_IDLE )
    {
      return  ENABLE_ACCESS;
    }

    m_pCategoryKeyRepatCtrl->UpdateInfo( pKey, 0, m_acitveCategoryIdx );

    if( m_pDelegate != NULL )
    {
      m_pDelegate->LowerItemSelectViewDelegate_OnKeyAction( pButton, pKey, pStick );
    }

    //  きがえるボタン
    if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) == true )
    {
      UIView::GetButtonManager()->StartSelectedAct( BUTTON_ID_DRESS_UP );
      return  DISABLE_ACCESS;
    }
    //  すがたをみるボタン
    else
    if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) == true )
    {
      UIView::GetButtonManager()->StartSelectedAct( BUTTON_ID_VIEWR );
      return  DISABLE_ACCESS;
    }
    //  キャンセルボタン
    else
    if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) == true )
    {
      UIView::GetButtonManager()->StartSelectedAct( BUTTON_ID_CANCEL );
      return  DISABLE_ACCESS;
    }
    else
    {
      if( m_pCategoryKeyRepatCtrl->IsRepeat( gfl2::ui::DIR_LEFT ) == true )
      {
        m_acitveCategoryIdx = ( m_acitveCategoryIdx - 1 ) % CATEGORY_ITEM_MAX;
        UIView::GetButtonManager()->StartSelectedAct( m_aCategoryItem[ m_acitveCategoryIdx ].m_buttonInfo.button_id );

        return  DISABLE_ACCESS;
      }
      else
      if( m_pCategoryKeyRepatCtrl->IsRepeat( gfl2::ui::DIR_RIGHT ) == true )
      {
        m_acitveCategoryIdx = ( m_acitveCategoryIdx + 1 ) % CATEGORY_ITEM_MAX;
        UIView::GetButtonManager()->StartSelectedAct( m_aCategoryItem[ m_acitveCategoryIdx ].m_buttonInfo.button_id );

        return  DISABLE_ACCESS;
      }
    }

    return  ENABLE_ACCESS;
  }

  //------------------------------------------------------------------
  /**
    * @brief   ペインリスト作成
    */
  //------------------------------------------------------------------
  void LowerItemSelectView::_CreatePaneList( const u32 listNum, const u32 listScroll, const f32 listPos, const u32 priority )
  {
    app::util::G2DUtil* pG2DUtil          = UIView::GetG2DUtil();
    gfl2::lyt::LytWk* pLayoutWork         = UIView::GetLayoutWork( LAYOUT_WORK_ID_LOWER );
    gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

    u32 dataNum = m_listItemNum = listNum;

    //  ペインリストデータを作成
    app::tool::PaneList::SETUP_PARAM list_setup;
    {
      list_setup.heap           = m_pPaneListViewHeap->GetDeviceHeap();  // ヒープ

      // レイアウトワーク
      list_setup.pLytWk         = pLayoutWork;
      // ペインデータ
      list_setup.paneList       = m_aPaneListData;
      // ペインデータ数
      list_setup.paneListMax    = ITEM_LIST_MAX;

      // 実際に使用するリストの項目数（ペインの数ではない）
      list_setup.valMax         = dataNum;

      // カーソル動作範囲（画面にはみ出る部分を除いた実際に入るリストの数）
      list_setup.listPosMax     = 3;

      // 初期カーソル位置 ( 0 ～ listPosMax-1 の範囲 )
      list_setup.listPos        = listPos;
      // 初期スクロール位置 ( 0 ～ valMax-listPosMax-1 の範囲 )
      list_setup.listScroll     = listScroll;
                              //   一番下にカーソルを位置させた状態で始めたいのであれば、listScroll+listposが
                              //   valMax-1になるようにする設定する。

      //  新しいリストは下にはみ出している
      if( ( dataNum - 1 ) <= ( list_setup.listPos + list_setup.listScroll ) )
      {
        list_setup.listScroll  = (f32)( (s32)dataNum - (s32)list_setup.listPosMax );
        if( list_setup.listScroll <= 0.0f )
        {
          list_setup.listScroll = 0.0f;
          list_setup.listPos  = dataNum - 1;
        }
        else
        {
          list_setup.listPos  = (u32)( (f32)dataNum - list_setup.listScroll - 1.0f );
        }
      }

      list_setup.defPY          = 55.0f;    // ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標 )

      //   この座標で指定した場所にあるペインが描画開始位置のペインになる
      list_setup.defSX          = 318.0f;   // ペインのXサイズ(レイアウトデディターで厳密に確認）
      list_setup.defSY          = 58.0f;    // ペインのYサイズ(レイアウトデディターで厳密に確認）

      // 上方向の表示限界(リストが半分はみ出ているなどの場合は-1, きっちり画面に入っている場合は0)
      list_setup.writeOffsetY   = -1;

      // 項目基準位置のペイン ( NULLの場合、動作時にプライオリティ変更を行わない )
      list_setup.listStartPane  = pLayoutWork->GetPane( PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_ITEM_CHANGE_ON );

      list_setup.cbFunc         = this;   // コールバック関数
      list_setup.cbWork         = NULL;   // コールバック関数に渡すワーク

      list_setup.uiDevMan       = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager(); // UIデバイスマネージャ
      list_setup.menuCursor     = m_pMenuCursor->GetAppToolMenuCursor();  // カーソル
    }

    //  スクロールデータを作成
    app::util::ScrollBar::SETUP_PARAM scroll_bar_setup =
    {
      // @note  以下、[set1]を設定
      m_pPaneListViewHeap->GetDeviceHeap(),
      app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL,
      list_setup.valMax - list_setup.listPosMax,

      pLayoutWork,
      rMultiResId,
      PANENAME_SHOPKISEKAE_LST_LOW_000_PANE_SCROLL,
      PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER,
      PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP,
      PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM,

      NULL,
      NULL,
      NULL,
      NULL,
    };
    
    //  スクロールバー作成
    {
      //  いったん削除
      m_pPaneListView->DeleteScrollBar();

      //  新しく作成
      m_pPaneListView->CreateScrollBar( &scroll_bar_setup );
    }

    //  リストの削除と生成を同時にやっている
    m_pPaneListView->ResetPaneList( &list_setup );

    m_pPaneListView->SetListener( this );

    //  リストの初期設定
    app::tool::PaneList*  pPaneList = m_pPaneListView->GetPaneList();
    GFL_ASSERT( pPaneList );

    {
      // @fix 大田哲司さん要望　回転をＹボタンからＬＲボタンに切り替えたためにリストのＬＲボタンのスキップを無効に
      pPaneList->SetConfig( app::tool::PaneList::PAGE_SKIP_NONE, true, false, true, false );
      pPaneList->SetSE( SEQ_SE_SELECT1, SEQ_SE_DECIDE1, SEQ_SE_CANCEL1 );
    }

    // @fix NMCat[2633] 押すＳＥを設定
    {
      SelectItem* pSelectItem = NULL;
      u32 loopNum = GFL_NELEMS( m_aSelectItem );
      for( u32 i = 0; i < loopNum; ++i )
      {
        pSelectItem = GetListItem( i );
        if( pSelectItem == NULL )
        {
          GFL_ASSERT( 0 );
        }        
        else
        {
          pPaneList->SetButtonSEPaneIndex( i, pSelectItem->GetPushSEId() );
        }
      }
    }


    AddSubView( m_pPaneListView );
  }

  void LowerItemSelectView::_SetState( const State newState )
  {
    m_state = newState;

    if( m_state == STATE_START_EVENT )
    {
      app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
      pG2DUtil->StartAnime( LAYOUT_WORK_ID_LOWER, LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_LST_LOW_000_INANIM );

      //  最初は非表示に開始演出中に表示していると位置がおかしくなるから
      m_pMenuCursor->SetVisible( false );

      //  リスト入力は停止
      {
        app::tool::PaneList*  pPaneList = m_pPaneListView->GetPaneList();
        pPaneList->SetInputEnabled( false );
        UIView::SetInputEnabledAllView( false );
      }
    }
    else
    if( m_state == STATE_IDLE )
    {
      app::tool::PaneList*  pPaneList = m_pPaneListView->GetPaneList();

      //  カーソル更新
      if( 0 < m_listItemNum )
      {
        m_pMenuCursor->SetVisible( true );

        //  リスト演出でタッチ範囲がずれている可能性があるので再設定
        pPaneList->MakeTouchArea();

        pPaneList->PutCursor();
        pPaneList->SetInputEnabled( true );
      }

      m_pushId  = m_tmpPushId = BUTTON_ID_NONE;

      //  入力可能
      {
        UIView::SetInputEnabledAllView( true );
        pPaneList->SetInputEnabled( true );
      }
    }
    else
    if( m_state == STATE_INPUT_EVENT_WAIT )
    {
      //  入力不可
      {
        UIView::SetInputEnabledAllView( false );
      }
    }
    else
    if( m_state == STATE_INPUT_STOP )
    {
      m_pushId  = m_tmpPushId;
      m_tmpPushId = BUTTON_ID_NONE;

      //  入力不可
      {
        app::tool::PaneList*  pPaneList = m_pPaneListView->GetPaneList();

        UIView::SetInputEnabledAllView( false );
        pPaneList->SetInputEnabled( false );
      }

      m_pMenuCursor->SetVisible( false );
    }
  }

//  ここから先は SelectItem のクラス定義
  //  選択アイテムクラス
  LowerItemSelectView::SelectItem::SelectItem()
  {
    _Clear();
  }

  LowerItemSelectView::SelectItem::~SelectItem()
  {
  }

  void LowerItemSelectView::SelectItem::Setup( LowerItemSelectView* pRootView, App::Kisekae::System::Work* pWork, const SETUP_ITEM_DATA& rSetup, const app::tool::PaneList::PANE_DATA& rPaneData, gfl2::heap::HeapBase* pHeap )
  {
    GFL_ASSERT( pHeap );
    GFL_ASSERT( pRootView );
    GFL_ASSERT( pWork );

    m_pRootView     = pRootView;
    m_setupItemData = rSetup;

    app::util::G2DUtil* pG2DUtil          = m_pRootView->GetG2DUtil();
    gfl2::lyt::LytWk* pLayoutWork         = m_pRootView->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
    gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

    app::util::GetPaneHelper  paneHelpaer( pLayoutWork, &rMultiResId );
    paneHelpaer.Push( rSetup.partsPaneId );

    m_pItemNameTextPane   = paneHelpaer.GetTextBoxPane( PANENAME_SHOPKISEKAE_BTN_LOW_000_PANE_TEXTBOX_00 );
    GFL_ASSERT( m_pItemNameTextPane );

    m_pPriceNumTextPane   = paneHelpaer.GetTextBoxPane( PANENAME_SHOPKISEKAE_BTN_LOW_000_PANE_TEXTBOX_01 );
    GFL_ASSERT( m_pPriceNumTextPane );

    m_pColorNameTextPane  = paneHelpaer.GetTextBoxPane( PANENAME_SHOPKISEKAE_BTN_LOW_000_PANE_TEXTBOX_02 );
    GFL_ASSERT( m_pColorNameTextPane );

    m_pBoughtNameTextPane = paneHelpaer.GetTextBoxPane( PANENAME_SHOPKISEKAE_BTN_LOW_000_PANE_TEXTBOX_03 );
    GFL_ASSERT( m_pBoughtNameTextPane );

    m_pChkMarkBGPane      = paneHelpaer.GetPane( PANENAME_SHOPKISEKAE_BTN_LOW_000_PANE_CHECKMARK_BG );
    GFL_ASSERT( m_pChkMarkBGPane );

    m_pChkMarkPane        = paneHelpaer.GetPane( PANENAME_SHOPKISEKAE_BTN_LOW_000_PANE_CHECKMARK );
    GFL_ASSERT( m_pChkMarkPane );

    m_pNewIconPartsPane   = paneHelpaer.GetPane( PANENAME_SHOPKISEKAE_BTN_LOW_000_PANE_NEWICON );
    GFL_ASSERT( m_pNewIconPartsPane );

    m_pBoughtMarkPane     = paneHelpaer.GetPane( PANENAME_SHOPKISEKAE_BTN_LOW_000_PANE_BOUGHTMARK );
    GFL_ASSERT( m_pBoughtMarkPane );

    //  newアイコンの画像ペインを取得
    {
      paneHelpaer.Push( PANENAME_SHOPKISEKAE_BTN_LOW_000_PANE_NEWICON );

      m_pNewIconPic         = paneHelpaer.GetPicturePane( PANENAME_COMMON_ICN_013_PANE_NEW_ICON_01 );
      GFL_ASSERT( m_pNewIconPic );

      //  グラフィックフォント対応
      pWork->GetCommonGrpIconData()->ReplacePaneTextureByNewIcon( m_pNewIconPic );
    }

    //  固定テキスト設定
    {
      //  購入済み
      pG2DUtil->SetTextBoxPaneString( m_pBoughtNameTextPane, kisekae_01_07 );
    }
  }

  bool LowerItemSelectView::SelectItem::InitFunc()
  {
    return  true;
  }

  bool LowerItemSelectView::SelectItem::EndFunc()
  {
    return  true;
  }

  void LowerItemSelectView::SelectItem::UpdateFunc()
  {
  }

  //  モード指定
  void LowerItemSelectView::SelectItem::SetMode( const Mode mode )
  {
    app::util::G2DUtil* pG2DUtil          = m_pRootView->GetG2DUtil();

    m_mode  = mode;

    if( mode == MODE_SHOP )
    {
      //  購入モード
      pG2DUtil->SetPaneVisible( m_pChkMarkPane, false );
      pG2DUtil->SetPaneVisible( m_pChkMarkBGPane, false );
      pG2DUtil->SetPaneVisible( m_pNewIconPartsPane, false );
      pG2DUtil->SetPaneVisible( m_pNewIconPic, false );
    }
    else
    if( mode == MODE_DRESS_UP )
    {
      //  きせかえモード
      pG2DUtil->SetPaneVisible( m_pChkMarkBGPane, true );
    }
    else
    if( mode == MODE_COLOR_DRESS_UP )
    {
      //  いろそめモード
      pG2DUtil->SetPaneVisible( m_pChkMarkPane, false );
      pG2DUtil->SetPaneVisible( m_pChkMarkBGPane, false );
      pG2DUtil->SetPaneVisible( m_pNewIconPartsPane, false );
      pG2DUtil->SetPaneVisible( m_pNewIconPic, false );
    }
    else
    {
      GFL_ASSERT( 0 );
    }
  }

  //  チェックボックスの有効化
  void LowerItemSelectView::SelectItem::SetEnableChkMark( const bool bFlg )
  {
    app::util::G2DUtil* pG2DUtil          = m_pRootView->GetG2DUtil();

    //  チェックボックスをつけるか
    pG2DUtil->SetPaneVisible( m_pChkMarkPane, bFlg );
  }

  //  NEWマークをつける
  void LowerItemSelectView::SelectItem::SetEnableNewMark( const bool bFlg )
  {
    app::util::G2DUtil* pG2DUtil          = m_pRootView->GetG2DUtil();

    pG2DUtil->SetPaneVisible( m_pNewIconPartsPane, bFlg );

    if( bFlg == true )
    {
      //  ループアニメをする
      pG2DUtil->StartAnime( LAYOUT_WORK_ID_LOWER, m_setupItemData.newIconLoopAnimeId );
    }
    else
    {
      //  ループアニメを停止
      pG2DUtil->StopAnime( LAYOUT_WORK_ID_LOWER, m_setupItemData.newIconLoopAnimeId );
    }
  }

  //  項目データ設定
  void LowerItemSelectView::SelectItem::SetData( App::Kisekae::System::ItemDataBase* pData, NetAppLib::Message::MessageUtility* pMessageUtility )
  {
    if( pData == NULL )
    {
      return;
    }

    app::util::G2DUtil* pG2DUtil          = m_pRootView->GetG2DUtil();

    if( pData->IsBuy() == true )
    {
      //  購入済み
      pG2DUtil->SetPaneVisible( m_pBoughtMarkPane, true );
    }
    else
    {
      //  購入していない
      pG2DUtil->SetPaneVisible( m_pBoughtMarkPane, false );
    }

    //  アイテム名表示
    {
      u32 txtId = pData->GetItemTxtID();
      print::WordSet* pWordSet  = pG2DUtil->GetWordSet();
      if( pWordSet == NULL )
      {
        GFL_ASSERT( 0 );
      }
      else
      {
        //  きせかえの購入個数は常に１つなので第３引数の購入個数値は１でよい
        pWordSet->RegisterKisekaeItemName( 0, txtId, 1 );
        pG2DUtil->SetTextBoxPaneStringExpand( m_pItemNameTextPane, kisekae_01_01 );
      }
    }

    //  色情報名表示
    {
      u32 txtId = pData->GetColorTxtID();
      if( 0 <= (s32)txtId )
      {
        pG2DUtil->SetPaneVisible( m_pColorNameTextPane, true );
        pG2DUtil->SetRegisterWord( 0, pMessageUtility->GetString( App::Kisekae::System::ItemList::GetColorTextMsTxtArcDatId(), txtId ) );

        pG2DUtil->SetTextBoxPaneStringExpand( m_pColorNameTextPane, kisekae_01_08 );
      }
      else
      {
        pG2DUtil->SetPaneVisible( m_pColorNameTextPane, false );
      }
    }

    //  newフラグがあるか
    SetEnableNewMark( pData->IsNew() );

    //  金額表示
    // @fix 大田さんさん要望　フィッティングルーム時には金額表示はoff
    pG2DUtil->SetPaneVisible( m_pPriceNumTextPane, m_bVisibleMoney );
    if( m_bVisibleMoney == true )
    {
      u32 txtId = kisekae_01_03;
      if( m_mode == MODE_COLOR_DRESS_UP )
      {
        txtId = kisekae_10_02;
      }

      pG2DUtil->SetTextBoxPaneNumber( m_pPriceNumTextPane, txtId, pData->GetMoney(), Savedata::Misc::MY_GOLD_KETA, 0, print::NUM_DISP_SPACE, print::NUM_CODE_DEFAULT );
    }
  }

  //  アクティブにするかどうか
  void LowerItemSelectView::SelectItem::SetActive( const bool bActive )
  {
    m_bActive = bActive;

    {
      app::util::G2DUtil* pG2DUtil = m_pRootView->GetG2DUtil();

      // @fix GFNMCat[555]  アニメフレーム設定すると自動アニメがなくなるので手動で停止が必要
      pG2DUtil->StopAnime( LAYOUT_WORK_ID_LOWER, m_setupItemData.activeAnimeId );
      pG2DUtil->StopAnime( LAYOUT_WORK_ID_LOWER, m_setupItemData.passiveAnimeId );

      // @fix GFNMCat[555]  アニメフレーム設定して最終フレームに設定。
      //                    リストが切り替わるとアクティブ表示でもアクティブアニメをしまうのを防ぐため
      //                    そもそもアクティブからパッシブはあるが、パッシブからアクティブはないパッシブアニメは１フレームで終らせているので最終フレーム設定でいい
      if( m_bActive == true )
      {
        pG2DUtil->StartAnime( LAYOUT_WORK_ID_LOWER, m_setupItemData.activeAnimeId );
        pG2DUtil->SetAnimeFrame( LAYOUT_WORK_ID_LOWER, m_setupItemData.activeAnimeId, pG2DUtil->GetAnimeMaxFrame( LAYOUT_WORK_ID_LOWER, m_setupItemData.activeAnimeId ) );
      }
      else
      {
        pG2DUtil->StartAnime( LAYOUT_WORK_ID_LOWER, m_setupItemData.passiveAnimeId );
        pG2DUtil->SetAnimeFrame( LAYOUT_WORK_ID_LOWER, m_setupItemData.passiveAnimeId, pG2DUtil->GetAnimeMaxFrame( LAYOUT_WORK_ID_LOWER, m_setupItemData.passiveAnimeId ) );
      }
    }
  }

//  ここから先は CategoryItem のクラス定義
  //  カテゴリーアイテムクラス
  void LowerItemSelectView::CategoryItem::Init( LowerItemSelectView* pRootView, const app::ui::ButtonInfoEx buttonInfo, const CATEGORY_SETUP_DATA& rSetupData )
  {
    m_buttonInfo    = buttonInfo;
    m_categorySetup = rSetupData;
    m_pRootView     = pRootView;
  }

  void LowerItemSelectView::CategoryItem::SelectItem( const bool bFlg )
  {
    app::util::G2DUtil* pG2DUtil = m_pRootView->GetG2DUtil();

    if( bFlg == true )
    {
      pG2DUtil->StartAnime( LAYOUT_WORK_ID_LOWER, m_categorySetup.selectItemAnimeId );
    }
    else
    {
      pG2DUtil->StartAnime( LAYOUT_WORK_ID_LOWER, m_categorySetup.nonSelectItemAnimeId );
    }
  }

  void LowerItemSelectView::CategoryItem::SelectBtn( const bool bFlg, const bool bFirst )
  {
    app::util::G2DUtil* pG2DUtil = m_pRootView->GetG2DUtil();

    if( bFlg == true )
    {
      if( bFirst == true )
      {
        //  初回時はアニメ最大に
        pG2DUtil->BindAnime( LAYOUT_WORK_ID_LOWER, m_categorySetup.selectBtnAnimeId );
        pG2DUtil->StartAnimeFrame( LAYOUT_WORK_ID_LOWER, m_categorySetup.selectBtnAnimeId, pG2DUtil->GetAnimeMaxFrame( LAYOUT_WORK_ID_LOWER, m_categorySetup.selectBtnAnimeId ) );
      }
      else
      {
        pG2DUtil->StopAnime( LAYOUT_WORK_ID_LOWER, m_categorySetup.selectBtnAnimeId, false );
        pG2DUtil->StartAnime( LAYOUT_WORK_ID_LOWER, m_categorySetup.selectBtnAnimeId );
      }
    }
    else
    {
      if( bFirst == true )
      {
        //  初回時はアニメ最大に
        pG2DUtil->BindAnime( LAYOUT_WORK_ID_LOWER, m_categorySetup.nonSelecBtntAnimeId );
        pG2DUtil->StartAnimeFrame( LAYOUT_WORK_ID_LOWER, m_categorySetup.nonSelecBtntAnimeId, pG2DUtil->GetAnimeMaxFrame( LAYOUT_WORK_ID_LOWER, m_categorySetup.nonSelecBtntAnimeId ) );
      }
      else
      {
        pG2DUtil->StopAnime( LAYOUT_WORK_ID_LOWER, m_categorySetup.nonSelecBtntAnimeId, false );
        pG2DUtil->StartAnime( LAYOUT_WORK_ID_LOWER, m_categorySetup.nonSelecBtntAnimeId );
      }
    }
  }

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)
