//=============================================================================
/**
 * @file ConfigLowerUIDraw.h
 * @brief コンフィグの上UI画面
 * @author yuto_uchida
 * @date 2015.05.08
 */
//=============================================================================
#include "App/config/source/parts/ConfigLowerUIDraw.h"

#include "niji_conv_header/app/config/res2d/lyt.h"
#include "niji_conv_header/app/config/res2d/lyt_anim_list.h"
#include "niji_conv_header/app/config/res2d/lyt_pane.h"

#include "niji_conv_header/message/msg_option.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( config )

  const u32 errorTextId = 0xffffffff;

  //  リスト項目のデータリスト
  const ConfigLowerUIDraw::LIST_ITEM_CREATE_DATA         ConfigLowerUIDraw::m_sListItemCreateDataTable[ ConfigLowerUIDraw::CONFIG_TYPE_MAX ]  =
  {
    //  ConfigLowerUIDraw::CONFIG_TYPE_TALK_SPEED
    {
      msg_option_01,
      msg_option_01_info,

      ConfigLowerUIDraw::BUTTON_ID_LEFT_ARROW_ITEM_00,
      ConfigLowerUIDraw::BUTTON_ID_RIGHT_ARROW_ITEM_00,

      {
        msg_option_01_01,
        msg_option_01_02,
        msg_option_01_03
      },
    },

    //  ConfigLowerUIDraw::CONFIG_TYPE_TEXT_MODE
    {
      msg_option_04,
      msg_option_04_info,

      ConfigLowerUIDraw::BUTTON_ID_LEFT_ARROW_ITEM_01,
      ConfigLowerUIDraw::BUTTON_ID_RIGHT_ARROW_ITEM_01,

      {
        msg_option_04_01,
        msg_option_04_02,
        errorTextId
      },
    },

    //  ConfigLowerUIDraw::CONFIG_TYPE_BATTLE_ANIM_SHOW
    {
      msg_option_02,
      msg_option_02_info,

      ConfigLowerUIDraw::BUTTON_ID_LEFT_ARROW_ITEM_02,
      ConfigLowerUIDraw::BUTTON_ID_RIGHT_ARROW_ITEM_02,

      {
        msg_option_02_01,
        msg_option_02_02,
        errorTextId
      },
    },

    //  ConfigLowerUIDraw::CONFIG_TYPE_BATTLE_RULE
    {
      msg_option_03,
      msg_option_03_info,

      ConfigLowerUIDraw::BUTTON_ID_LEFT_ARROW_ITEM_03,
      ConfigLowerUIDraw::BUTTON_ID_RIGHT_ARROW_ITEM_03,

      {
        msg_option_03_01,
        msg_option_03_02,
        errorTextId
      },
    },

    //  ConfigLowerUIDraw::CONFIG_TYPE_BUTTOM_MODE
    {
      msg_option_05,
      msg_option_05_info,

      ConfigLowerUIDraw::BUTTON_ID_LEFT_ARROW_ITEM_04,
      ConfigLowerUIDraw::BUTTON_ID_RIGHT_ARROW_ITEM_04,

      {
        msg_option_05_01,
        msg_option_05_02,
        errorTextId
      },
    },

    //  ConfigLowerUIDraw::CONFIG_TYPE_BOX_STATUS
    {
      msg_option_06,
      msg_option_06_info,

      ConfigLowerUIDraw::BUTTON_ID_LEFT_ARROW_ITEM_05,
      ConfigLowerUIDraw::BUTTON_ID_RIGHT_ARROW_ITEM_05,

      {
        msg_option_06_01,
        msg_option_06_02,
        errorTextId
      },
    },
  };

  //  ボタン作成データ固定値専用
  const ConfigLowerUIDraw::BUTTON_CREATE_DATA  ConfigLowerUIDraw::m_sCreateButtonInfoTable[ BUTTON_ID_MAX ] =
  {
/* start -- リスト項目1 -- */
    //  左矢印ボタン
    {
      {
        ConfigLowerUIDraw::BUTTON_ID_LEFT_ARROW_ITEM_00,
        NULL,
        NULL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_00__CURSOR_L_TOUCH,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_00__CURSOR_L_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_00__CURSOR_L_CANSEL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_00__CURSOR_L_TOUCH_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_00__CURSOR_L_ACTIVE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_00__CURSOR_L_PASSIVE
      },

      SMID_NULL
    },

    //  右矢印ボタン
    {
      {
        ConfigLowerUIDraw::BUTTON_ID_RIGHT_ARROW_ITEM_00,
        NULL,
        NULL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_00__CURSOR_R_TOUCH,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_00__CURSOR_R_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_00__CURSOR_R_CANSEL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_00__CURSOR_R_TOUCH_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_00__CURSOR_R_ACTIVE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_00__CURSOR_R_PASSIVE
      },

      SMID_NULL
    },
/* end -- リスト項目1 -- */

/* start -- リスト項目2 -- */
    //  左矢印ボタン
    {
      {
        ConfigLowerUIDraw::BUTTON_ID_LEFT_ARROW_ITEM_01,
        NULL,
        NULL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_01__CURSOR_L_TOUCH,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_01__CURSOR_L_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_01__CURSOR_L_CANSEL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_01__CURSOR_L_TOUCH_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_01__CURSOR_L_ACTIVE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_01__CURSOR_L_PASSIVE
      },

      SMID_NULL
    },

    //  右矢印ボタン
    {
      {
        ConfigLowerUIDraw::BUTTON_ID_RIGHT_ARROW_ITEM_01,
        NULL,
        NULL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_01__CURSOR_R_TOUCH,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_01__CURSOR_R_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_01__CURSOR_R_CANSEL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_01__CURSOR_R_TOUCH_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_01__CURSOR_R_ACTIVE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_01__CURSOR_R_PASSIVE
      },

      SMID_NULL
    },
/* end -- リスト項目2 -- */

/* start -- リスト項目3 -- */
    //  左矢印ボタン
    {
      {
        ConfigLowerUIDraw::BUTTON_ID_LEFT_ARROW_ITEM_02,
        NULL,
        NULL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_02__CURSOR_L_TOUCH,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_02__CURSOR_L_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_02__CURSOR_L_CANSEL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_02__CURSOR_L_TOUCH_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_02__CURSOR_L_ACTIVE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_02__CURSOR_L_PASSIVE
      },

      SMID_NULL
    },

    //  右矢印ボタン
    {
      {
        ConfigLowerUIDraw::BUTTON_ID_RIGHT_ARROW_ITEM_02,
        NULL,
        NULL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_02__CURSOR_R_TOUCH,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_02__CURSOR_R_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_02__CURSOR_R_CANSEL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_02__CURSOR_R_TOUCH_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_02__CURSOR_R_ACTIVE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_02__CURSOR_R_PASSIVE
      },

      SMID_NULL
    },
/* end -- リスト項目3-- */

/* start -- リスト項目4 -- */
    //  左矢印ボタン
    {
      {
        ConfigLowerUIDraw::BUTTON_ID_LEFT_ARROW_ITEM_03,
        NULL,
        NULL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_03__CURSOR_L_TOUCH,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_03__CURSOR_L_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_03__CURSOR_L_CANSEL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_03__CURSOR_L_TOUCH_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_03__CURSOR_L_ACTIVE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_03__CURSOR_L_PASSIVE
      },

      SMID_NULL
    },

    //  右矢印ボタン
    {
      {
        ConfigLowerUIDraw::BUTTON_ID_RIGHT_ARROW_ITEM_03,
        NULL,
        NULL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_03__CURSOR_R_TOUCH,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_03__CURSOR_R_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_03__CURSOR_R_CANSEL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_03__CURSOR_R_TOUCH_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_03__CURSOR_R_ACTIVE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_03__CURSOR_R_PASSIVE
      },

      SMID_NULL
    },
/* end -- リスト項目4 -- */

/* start -- リスト項目5 -- */
    //  左矢印ボタン
    {
      {
        ConfigLowerUIDraw::BUTTON_ID_LEFT_ARROW_ITEM_04,
        NULL,
        NULL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_04__CURSOR_L_TOUCH,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_04__CURSOR_L_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_04__CURSOR_L_CANSEL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_04__CURSOR_L_TOUCH_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_04__CURSOR_L_ACTIVE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_04__CURSOR_L_PASSIVE
      },

      SMID_NULL
    },

    //  右矢印ボタン
    {
      {
        ConfigLowerUIDraw::BUTTON_ID_RIGHT_ARROW_ITEM_04,
        NULL,
        NULL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_04__CURSOR_R_TOUCH,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_04__CURSOR_R_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_04__CURSOR_R_CANSEL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_04__CURSOR_R_TOUCH_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_04__CURSOR_R_ACTIVE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_04__CURSOR_R_PASSIVE
      },

      SMID_NULL
    },
/* end -- リスト項目5 -- */

/* start -- リスト項目6 -- */
    //  左矢印ボタン
    {
      {
        ConfigLowerUIDraw::BUTTON_ID_LEFT_ARROW_ITEM_05,
        NULL,
        NULL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_05__CURSOR_L_TOUCH,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_05__CURSOR_L_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_05__CURSOR_L_CANSEL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_05__CURSOR_L_TOUCH_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_05__CURSOR_L_ACTIVE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_05__CURSOR_L_PASSIVE
      },

      SMID_NULL
    },

    //  右矢印ボタン
    {
      {
        ConfigLowerUIDraw::BUTTON_ID_RIGHT_ARROW_ITEM_05,
        NULL,
        NULL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_05__CURSOR_R_TOUCH,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_05__CURSOR_R_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_05__CURSOR_R_CANSEL,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_05__CURSOR_R_TOUCH_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_05__CURSOR_R_ACTIVE,
        LA_LYT_CONFIG_MIN_LOW_000__BUTTON_05__CURSOR_R_PASSIVE
      },

      SMID_NULL
    },
/* end -- リスト項目6 -- */

    //  リスト項目1
    {
      {
        ConfigLowerUIDraw::BUTTON_ID_LIST_ITEM_00,
        NULL,
        NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL
      },

      -1
    },

    //  リスト項目2
    {
      {
        ConfigLowerUIDraw::BUTTON_ID_LIST_ITEM_01,
        NULL,
        NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL
      },

      -1
    },

    //  リスト項目3
    {
      {
        ConfigLowerUIDraw::BUTTON_ID_LIST_ITEM_02,
        NULL,
        NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL
      },

      -1
    },

    //  リスト項目4
    {
      {
        ConfigLowerUIDraw::BUTTON_ID_LIST_ITEM_03,
        NULL,
        NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL
      },

      -1
    },

    //  リスト項目5
    {
      {
        ConfigLowerUIDraw::BUTTON_ID_LIST_ITEM_04,
        NULL,
        NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL
      },

      -1
    },

    //  リスト項目6
    {
      {
        ConfigLowerUIDraw::BUTTON_ID_LIST_ITEM_05,
        NULL,
        NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL
      },
    },

    //  もどるボタン
    {
      {
        ConfigLowerUIDraw::BUTTON_ID_BACK,
        NULL,
        NULL,
        LA_LYT_CONFIG_MIN_LOW_000__BACKBUTTON_TOUCH,
        LA_LYT_CONFIG_MIN_LOW_000__BACKBUTTON_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BACKBUTTON_CANSEL,
        LA_LYT_CONFIG_MIN_LOW_000__BACKBUTTON_TOUCH_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__BACKBUTTON_ACTIVE,
        LA_LYT_CONFIG_MIN_LOW_000__BACKBUTTON_PASSIVE
      },

      SEQ_SE_CANCEL1,
    },

    //  決定ボタン
    {
      {
        ConfigLowerUIDraw::BUTTON_ID_ENTER,
        NULL,
        NULL,
        LA_LYT_CONFIG_MIN_LOW_000__OKBUTTON_TOUCH,
        LA_LYT_CONFIG_MIN_LOW_000__OKBUTTON_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__OKBUTTON_CANSEL,
        LA_LYT_CONFIG_MIN_LOW_000__OKBUTTON_TOUCH_RELEASE,
        LA_LYT_CONFIG_MIN_LOW_000__OKBUTTON_ACTIVE,
        LA_LYT_CONFIG_MIN_LOW_000__OKBUTTON_PASSIVE
      },

      SEQ_SE_COMPLETE1
    },
  };

  //  選択リストの初期化データテーブル
  const ConfigLowerUIListItemObj::INIT_PARAM ConfigLowerUIDraw::m_sSelectListItemInitParamTable[ SELECT_LIST_MAX ] =
  {
    //  0
    {
      PANENAME_CONFIG_MIN_LOW_000_PANE_BUTTON_00,
      
      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_00__CURSOR_L_PATERN_BLUE,
      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_00__CURSOR_R_PATERN_BLUE,

      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_00__CURSOR_L_PATERN_GRAY,
      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_00__CURSOR_R_PATERN_GRAY,

      //  項目ボタン
      m_sCreateButtonInfoTable[ ConfigLowerUIDraw::BUTTON_ID_LIST_ITEM_00 ].infoEx,
      //  左矢印ボタン
      m_sCreateButtonInfoTable[ ConfigLowerUIDraw::BUTTON_ID_LEFT_ARROW_ITEM_00 ].infoEx,
      //  右矢印ボタン
      m_sCreateButtonInfoTable[ ConfigLowerUIDraw::BUTTON_ID_RIGHT_ARROW_ITEM_00 ].infoEx
    },

    //  1
    {
      PANENAME_CONFIG_MIN_LOW_000_PANE_BUTTON_01,

      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_01__CURSOR_L_PATERN_BLUE,
      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_01__CURSOR_R_PATERN_BLUE,

      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_01__CURSOR_L_PATERN_GRAY,
      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_01__CURSOR_R_PATERN_GRAY,

      //  項目ボタン
      m_sCreateButtonInfoTable[ ConfigLowerUIDraw::BUTTON_ID_LIST_ITEM_01 ].infoEx,
      //  左矢印ボタン
      m_sCreateButtonInfoTable[ ConfigLowerUIDraw::BUTTON_ID_LEFT_ARROW_ITEM_01 ].infoEx,
      //  右矢印ボタン
      m_sCreateButtonInfoTable[ ConfigLowerUIDraw::BUTTON_ID_RIGHT_ARROW_ITEM_01 ].infoEx
    },

    //  2
    {
      PANENAME_CONFIG_MIN_LOW_000_PANE_BUTTON_02,

      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_02__CURSOR_L_PATERN_BLUE,
      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_02__CURSOR_R_PATERN_BLUE,

      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_02__CURSOR_L_PATERN_GRAY,
      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_02__CURSOR_R_PATERN_GRAY,

      //  項目ボタン
      m_sCreateButtonInfoTable[ ConfigLowerUIDraw::BUTTON_ID_LIST_ITEM_02 ].infoEx,
      //  左矢印ボタン
      m_sCreateButtonInfoTable[ ConfigLowerUIDraw::BUTTON_ID_LEFT_ARROW_ITEM_02 ].infoEx,
      //  右矢印ボタン
      m_sCreateButtonInfoTable[ ConfigLowerUIDraw::BUTTON_ID_RIGHT_ARROW_ITEM_02 ].infoEx
    },

    //  3
    {
      PANENAME_CONFIG_MIN_LOW_000_PANE_BUTTON_03,

      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_03__CURSOR_L_PATERN_BLUE,
      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_03__CURSOR_R_PATERN_BLUE,

      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_03__CURSOR_L_PATERN_GRAY,
      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_03__CURSOR_R_PATERN_GRAY,

      //  項目ボタン
      m_sCreateButtonInfoTable[ ConfigLowerUIDraw::BUTTON_ID_LIST_ITEM_03 ].infoEx,
      //  左矢印ボタン
      m_sCreateButtonInfoTable[ ConfigLowerUIDraw::BUTTON_ID_LEFT_ARROW_ITEM_03 ].infoEx,
      //  右矢印ボタン
      m_sCreateButtonInfoTable[ ConfigLowerUIDraw::BUTTON_ID_RIGHT_ARROW_ITEM_03 ].infoEx
    },

    //  4
    {
      PANENAME_CONFIG_MIN_LOW_000_PANE_BUTTON_04,

      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_04__CURSOR_L_PATERN_BLUE,
      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_04__CURSOR_R_PATERN_BLUE,

      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_04__CURSOR_L_PATERN_GRAY,
      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_04__CURSOR_R_PATERN_GRAY,

      //  項目ボタン
      m_sCreateButtonInfoTable[ ConfigLowerUIDraw::BUTTON_ID_LIST_ITEM_04 ].infoEx,
      //  左矢印ボタン
      m_sCreateButtonInfoTable[ ConfigLowerUIDraw::BUTTON_ID_LEFT_ARROW_ITEM_04 ].infoEx,
      //  右矢印ボタン
      m_sCreateButtonInfoTable[ ConfigLowerUIDraw::BUTTON_ID_RIGHT_ARROW_ITEM_04 ].infoEx
    },

    //  5
    {
      PANENAME_CONFIG_MIN_LOW_000_PANE_BUTTON_05,

      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_05__CURSOR_L_PATERN_BLUE,
      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_05__CURSOR_R_PATERN_BLUE,

      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_05__CURSOR_L_PATERN_GRAY,
      LA_LYT_CONFIG_MIN_LOW_000__BUTTON_05__CURSOR_R_PATERN_GRAY,

      //  項目ボタン
      m_sCreateButtonInfoTable[ ConfigLowerUIDraw::BUTTON_ID_LIST_ITEM_05 ].infoEx,
      //  左矢印ボタン
      m_sCreateButtonInfoTable[ ConfigLowerUIDraw::BUTTON_ID_LEFT_ARROW_ITEM_05 ].infoEx,
      //  右矢印ボタン
      m_sCreateButtonInfoTable[ ConfigLowerUIDraw::BUTTON_ID_RIGHT_ARROW_ITEM_05 ].infoEx
    }
  };

  //  各リスト作成データを取得
  const ConfigLowerUIDraw::LIST_ITEM_CREATE_DATA&  ConfigLowerUIDraw::GetCreateListItemData( const ConfigType type )
  {
    return  m_sListItemCreateDataTable[ type ];
  }

  //  各リスト作成データの個数
  u32 ConfigLowerUIDraw::GetCreateListItemDataNum()
  {
    return  GFL_NELEMS( m_sListItemCreateDataTable );
  }

  //  テキストエラーid
  u32 ConfigLowerUIDraw::GetTextErrorId()
  {
    return  errorTextId;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  ConfigLowerUIDraw::ConfigLowerUIDraw( app::util::Heap* pUtilHeap ) :
    UIView( pUtilHeap, pUtilHeap->GetDeviceAllocator(), pUtilHeap->GetDeviceAllocator() )
  {
    _Clear();

    m_pUtilHeap = pUtilHeap;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  ConfigLowerUIDraw::~ConfigLowerUIDraw()
  {
    GFL_SAFE_DELETE( m_pMenuCursor );

    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    */
  //------------------------------------------------------------------
  void ConfigLowerUIDraw::Initialize( const INIT_PARAM& param )
  {
    m_initParam = param;

    _Initialize2D( m_initParam.pLytBuff );

    app::util::G2DUtil*       pG2DUtil   = UIView::GetG2DUtil();
    gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );
    gfl2::lyt::LytWk*         pLayoutWork = UIView::GetLayoutWork( LYT_WK_TYPE_SCREEN );

    //  選択リスト初期化
    {
      u32 loopNum = GFL_NELEMS( m_aSelectListItem );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_aSelectListItem[ i ].Initialize( m_sSelectListItemInitParamTable[ i ], this, LYT_WK_TYPE_SCREEN, &rMultiResId );
      }
    }

    //  ボタン登録
    {
      app::ui::ButtonInfoEx aButtonDataTable[ BUTTON_ID_MAX ];
      gfl2::std::MemClear( aButtonDataTable, sizeof( aButtonDataTable ) );

      u32 buttonIdx = 0;

      //  リストのボタンデータを設定
      {
        u32 loopNum = GFL_NELEMS( m_aSelectListItem );
        for( u32 i = 0; i < loopNum; ++i )
        {
          GFL_ASSERT( buttonIdx < BUTTON_ID_MAX );

          const ConfigLowerUIListItemObj::INIT_PARAM&  rInitParam  = m_aSelectListItem[ i ].GetInitParam();
          {
            //  リストの左矢印のボタン設定
            {
              GFL_ASSERT( buttonIdx < BUTTON_ID_MAX );

              aButtonDataTable[ buttonIdx ] = rInitParam.left_arrow_button_data;
              ++buttonIdx;
            }

            //  リストの右矢印のボタン設定
            {
              GFL_ASSERT( buttonIdx < BUTTON_ID_MAX );

              aButtonDataTable[ buttonIdx ] = rInitParam.right_arrow_button_data;

              ++buttonIdx;
            }
          }
        }

        //  リストの項目ボタン設定
        for( u32 i = 0; i < loopNum; ++i )
        {
          GFL_ASSERT( buttonIdx < BUTTON_ID_MAX );

          const ConfigLowerUIListItemObj::INIT_PARAM&  rInitParam  = m_aSelectListItem[ i ].GetInitParam();

          aButtonDataTable[ buttonIdx ] = rInitParam.item_button_data;
          ++buttonIdx;
        }
      }

      app::ui::ButtonInfoEx*  pSetActiveButtonInfo  = NULL;

      //  もどるボタン
      {
        GFL_ASSERT( buttonIdx == BUTTON_ID_BACK );

        pSetActiveButtonInfo    = &aButtonDataTable[ buttonIdx ];
        *pSetActiveButtonInfo   = m_sCreateButtonInfoTable[ buttonIdx ].infoEx;

        gfl2::lyt::LytParts*  pPartsPane  = pLayoutWork->GetPartsPane( PANENAME_CONFIG_MIN_LOW_000_PANE_BACKBUTTON );
        GFL_ASSERT( pPartsPane );

        pSetActiveButtonInfo->picture_pane  = pLayoutWork->GetPane( pPartsPane, PANENAME_COMMON_BTN_LOW_000_PANE_MAIN, &rMultiResId );
        GFL_ASSERT( pSetActiveButtonInfo->picture_pane );

        pSetActiveButtonInfo->bound_pane    = pLayoutWork->GetBoundingPane( pPartsPane, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &rMultiResId );
        GFL_ASSERT( pSetActiveButtonInfo->bound_pane );

        ++buttonIdx;
      }

      //  決定ボタン
      {
        GFL_ASSERT( buttonIdx == BUTTON_ID_ENTER );

        pSetActiveButtonInfo    = &aButtonDataTable[ buttonIdx ];
        *pSetActiveButtonInfo   = m_sCreateButtonInfoTable[ buttonIdx ].infoEx;

        gfl2::lyt::LytParts*  pPartsPane  = pLayoutWork->GetPartsPane( PANENAME_CONFIG_MIN_LOW_000_PANE_OKBUTTON );
        GFL_ASSERT( pPartsPane );

        pSetActiveButtonInfo->picture_pane  = pLayoutWork->GetPane( pPartsPane, PANENAME_CONFIG_BTN_LOW_001_PANE_MAIN, &rMultiResId );
        GFL_ASSERT( pSetActiveButtonInfo->picture_pane );

        pSetActiveButtonInfo->bound_pane    = pLayoutWork->GetBoundingPane( pPartsPane, PANENAME_CONFIG_BTN_LOW_001_PANE_BOUND_00, &rMultiResId );
        GFL_ASSERT( pSetActiveButtonInfo->bound_pane );

        ++buttonIdx;
      }

      UIView::CreateButtonManager( m_pUtilHeap, pLayoutWork, aButtonDataTable, GFL_NELEMS( aButtonDataTable ) );

      //  SE設定
      {
        app::tool::ButtonManager* pButtonManager  = UIView::GetButtonManager();
        u32 loopNum = GFL_NELEMS( m_sCreateButtonInfoTable );
        for( u32 i = 0; i < loopNum; ++i )
        {
          if( m_sCreateButtonInfoTable[ i ].seId != -1 )
          {
            pButtonManager->SetButtonSelectSE( m_sCreateButtonInfoTable[ i ].infoEx.button_id, (u32)m_sCreateButtonInfoTable[ i ].seId );
          }
        }
      }
    }

    //  選択リストの項目を一旦すべて非表示
    {
      u32 loopNum = GFL_NELEMS( m_aSelectListItem );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_aSelectListItem[ i ].SetViaible( false );
      }
    }

    //  表示項目にデータを設定
    {
      for( u32 i = 0; i < m_initParam.listMax; ++i )
      {
        m_aSelectListItem[ i ].SetData( m_initParam.aListItemParam[ i ].itemData );
        m_aSelectListItem[ i ].SetViaible( true );
      }
    }

    //  カーソル作成
    {
      m_pMenuCursor = GFL_NEW( m_pUtilHeap->GetDeviceHeap() ) app::tool::MenuCursor( m_pUtilHeap, &m_initParam.pMenuCursorLytDataBuffer );
      m_pMenuCursor->CreateLayoutWork( UIView::GetLayoutSystem(), UIView::GetLytSetup( gfl2::lyt::DISPLAY_LOWER ), pG2DUtil->GetAppLytAccessor(), app::tool::MenuCursor::SIZE_SMALL, false );
    }

    //  表示しているテキスト一括更新
    {
      UpdateText( m_initParam.pMsgData );
    }

    //  BGのループアニメ
    {
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_LYT_CONFIG_MIN_LOW_000__BG_00_BG_LOOP_00 );
    }

    //  初回カーソル位置を指定
    SetListItemCurrentPos( 0 );

    UIView::SetVisible( false );
  }

  //------------------------------------------------------------------
  /**
    * @brief   終了処理
    */
  //------------------------------------------------------------------
  void ConfigLowerUIDraw::Terminate()
  {
    u32 loopNum = GFL_NELEMS( m_aSelectListItem );
    for( u32 i = 0; i < loopNum; ++i )
    {
      m_aSelectListItem[ i ].Terminate();
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   クラス破棄可能か
    */
  //------------------------------------------------------------------
  bool ConfigLowerUIDraw::IsDelete()
  {
    if( m_pMenuCursor->DeleteLayoutWork() == true )
    {
      return  true;
    }

    return  false;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 更新処理
    */
  //-------------------------------------------------------------------------
  void ConfigLowerUIDraw::Update()
  {
    _UpdateState();

    UIView::Update();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 描画
    */
  //-------------------------------------------------------------------------
  void ConfigLowerUIDraw::Draw(  gfl2::gfx::CtrDisplayNo displayNo )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    if( pG2DUtil == NULL ) { return; }

    for( s32 i = 0; i < LYT_WK_TYPE_MAX; ++i )
    {
      pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderingManager, displayNo, i );
    }

    m_pMenuCursor->Draw( m_initParam.pRenderingManager, displayNo, gfl2::lyt::DISPLAY_LOWER );
  }

  //------------------------------------------------------------------
  /**
    * @brief   指定したリスト項目のデータ位置を変える
    * @param   activeListItemIdx : 設定対象のリストアイテムidx
    * @param   addMoveDataNum    : データ移動数
    * @return  変更したデータ結果を取得
    */
  //------------------------------------------------------------------
  const ConfigLowerUIDraw::LIST_ITEM_PARAM* ConfigLowerUIDraw::MoveListItemSelectDataPos( u32 activeListItemIdx, s32 addMoveDataNum )
  {
    GFL_ASSERT( activeListItemIdx < m_initParam.listMax );

    //  項目内容を変更
    const ConfigLowerUIListItemObj::DATA_PARAM* pListItemDataParam  = m_aSelectListItem[ activeListItemIdx ].MoveSelectData( addMoveDataNum );
    if( pListItemDataParam == NULL )
    {
      return  NULL;
    }

    //  変更データ更新
    LIST_ITEM_PARAM*  pActiveListItemParam  = &m_initParam.aListItemParam[ activeListItemIdx ];
     
    pActiveListItemParam->itemData = *pListItemDataParam;

    return  pActiveListItemParam;
  }

  //------------------------------------------------------------------
  /**
    * @brief   指定したリスト項目の押しアニメ再生
    * @param   idx       : リストidx
    * @param   pushType  : 押すアニメ種類
    */
  //------------------------------------------------------------------
  void  ConfigLowerUIDraw::PlayPushAnimeListItem( const u32 idx, const PushButtonListItem pushType )
  {
    GFL_ASSERT( idx < GFL_NELEMS( m_aSelectListItem ) );

    if( pushType == PUSH_BUTTON_LIST_ITEM_LEFT_ARROW )
    {
      UIResponder::GetButtonManager()->StartSelectedAct( m_aSelectListItem[ idx ].GetInitParam().item_button_data.button_id );
    }
    else
    if( pushType == PUSH_BUTTON_LIST_ITEM_RIGHT_ARROW )
    {
      UIResponder::GetButtonManager()->StartSelectedAct( m_aSelectListItem[ idx ].GetInitParam().item_button_data.button_id );
    }
    else
    {
      GFL_ASSERT( 0 );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   指定ボタンを押す
    * @param   id : 押すボタンidを指定
    * @return  ボタンを押した -> true / ボタンを押せなかった -> false
    */
  //------------------------------------------------------------------
  bool ConfigLowerUIDraw::PushButton( const ButtonId id )
  {
    UIView::GetButtonManager()->StartSelectedAct( id );
    return  true;
  }

  //------------------------------------------------------------------
  /**
    * @brief   カレントのリスト項目のボタンプッシュ
    * @param   pushId : 項目にボタン押しid
    * @return  押すのに成功 -> true / 押すのに失敗 -> false
    */
  //------------------------------------------------------------------
  bool ConfigLowerUIDraw::PushButtonCurrentListItem( const PushButtonListItem pushId )
  {
    if( PUSH_BUTTON_LIST_ITEM_MAX <= pushId )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    GFL_ASSERT( m_nowListItemCurrentPos <= GFL_NELEMS( m_aSelectListItem ) );
    GFL_ASSERT( (u32)m_nowListItemCurrentPos <= m_initParam.listMax );

    const ConfigLowerUIListItemObj::INIT_PARAM& rListItemObjInitParam = m_aSelectListItem[ m_nowListItemCurrentPos ].GetInitParam();

    const u32 aPushDataTable[ PUSH_BUTTON_LIST_ITEM_MAX ][ 2 ] =
    {
      { PUSH_BUTTON_LIST_ITEM_LEFT_ARROW,  rListItemObjInitParam.left_arrow_button_data.button_id },
      { PUSH_BUTTON_LIST_ITEM_RIGHT_ARROW, rListItemObjInitParam.right_arrow_button_data.button_id },
    };

    return  PushButton( (ButtonId)aPushDataTable[ pushId ][ 1 ] );
  }

  //------------------------------------------------------------------
  /**
    * @brief   ボタンidからリスト項目のカレント設定
    * @param   id : ボタンid
    * @return  変更成功 -> true / 変更失敗 -> false
    */
  //------------------------------------------------------------------
  bool ConfigLowerUIDraw::SetListItemCurrentPosFormButtonId( const ButtonId id )
  {
    s32 loopNum = (s32)m_initParam.listMax;
    GFL_ASSERT( loopNum <= GFL_NELEMS( m_sSelectListItemInitParamTable ) );

    s32   newCurrentPos = -1;

    app::tool::ButtonManager::ButtonId  chkButtonId = (app::tool::ButtonManager::ButtonId)id;
    for( s32 i = 0; i < loopNum; ++i )
    {
      const ConfigLowerUIListItemObj::INIT_PARAM&  rActiveListItemInitParam = m_aSelectListItem[ i ].GetInitParam();

      //  選択項目と左右矢印をタッチしたらカレント移動
      if( rActiveListItemInitParam.item_button_data.button_id == chkButtonId )
      {
        newCurrentPos = i;
        break;
      }

      if( rActiveListItemInitParam.left_arrow_button_data.button_id == chkButtonId )
      {
        newCurrentPos = i;
        break;
      }

      if( rActiveListItemInitParam.right_arrow_button_data.button_id == chkButtonId )
      {
        newCurrentPos = i;
        break;
      }
    }

    if( newCurrentPos != -1 )
    {
      s32 oldCurrentPos = m_nowListItemCurrentPos;
      if( SetListItemCurrentPos( newCurrentPos ) != -1 )
      {
        //  変更しているか
        return  ( oldCurrentPos != m_nowListItemCurrentPos );
      }
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief   現在のリスト項目のカレント位置を指定
    * @param   pos : カレント位置指定
    * @return  設定したカレント位置を返す / 設定失敗の場合は -1
    */
  //------------------------------------------------------------------
  s32 ConfigLowerUIDraw::SetListItemCurrentPos( const s32 pos )
  {
    if( m_initParam.listMax <= (u32)pos )
    {
      return  -1;
    }

    m_nowListItemCurrentPos = pos;
    m_pMenuCursor->PutNonVisible( UIView::GetLayoutWork( LYT_WK_TYPE_SCREEN ), m_aSelectListItem[ pos ].GetCursorPosPane() );

    //  カーソルが設定している項目以外は非アクティブに
    {
      for( u32 i = 0; i < m_initParam.listMax; ++i )
      {
        if( i == (u32)pos )
        {
          m_aSelectListItem[ i ].SetActive( true );
        }
        else
        {
          m_aSelectListItem[ i ].SetActive( false );
        }
      }
    }

    return  m_nowListItemCurrentPos;
  }

  //------------------------------------------------------------------
  /**
    * @brief   現在のリスト項目のカレントデータを取得
    */
  //------------------------------------------------------------------
  const ConfigLowerUIDraw::LIST_ITEM_PARAM* ConfigLowerUIDraw::GetListItemCurrentData() const
  {
    GFL_ASSERT( (u32)m_nowListItemCurrentPos < m_initParam.listMax );

    //  変更データ更新
    return &m_initParam.aListItemParam[ m_nowListItemCurrentPos ];
  }

  //------------------------------------------------------------------
  /**
    * @brief   現在のリスト項目データを取得
    * @parma   idx : リストidx
    */
  //------------------------------------------------------------------
  const ConfigLowerUIDraw::LIST_ITEM_PARAM* ConfigLowerUIDraw::GetListItemData( const u32 idx ) const
  {
    GFL_ASSERT( idx < m_initParam.listMax );

    //  変更データ更新
    return &m_initParam.aListItemParam[ idx ];
  }

  //------------------------------------------------------------------
  /**
    * @brief   テキスト更新
    */
  //------------------------------------------------------------------
  void ConfigLowerUIDraw::UpdateText( gfl2::str::MsgData* pMsgData )
  {
    GFL_ASSERT( pMsgData );

    app::util::G2DUtil*       pG2DUtil    = UIView::GetG2DUtil();
    gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );
    gfl2::lyt::LytWk*         pLayoutWork = UIView::GetLayoutWork( LYT_WK_TYPE_SCREEN );

    pG2DUtil->SetMessageData( pMsgData, m_initParam.pWordSet );
    m_initParam.pMsgData  = pMsgData;

    //  決定テキスト
    {
      gfl2::lyt::LytParts*  pPartsPane  = pLayoutWork->GetPartsPane( PANENAME_CONFIG_MIN_LOW_000_PANE_OKBUTTON );
      GFL_ASSERT( pPartsPane );

      pG2DUtil->SetTextBoxPaneString( pLayoutWork->GetTextBoxPane( pPartsPane, PANENAME_CONFIG_BTN_LOW_001_PANE_TEXTBOX_00, &rMultiResId ), (u32)msg_option_00 );
    }

    //  リスト項目のテキスト更新
    {
      u32 loopNum = GFL_NELEMS( m_aSelectListItem );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_aSelectListItem[ i ].UpdateText();
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   モード設定
    */
  //------------------------------------------------------------------
  void ConfigLowerUIDraw::SetDrawMode( const DrawMode nowMode, const bool bInput )
  {
    m_drawMode  = nowMode;

    UIView::GetButtonManager()->SetInputEnable( bInput );

    if( m_drawMode == DRAW_MODE_MESSAGE_CHANGE_DATA )
    {
      m_pMenuCursor->SetVisible( false );
    }
    else if( m_drawMode == DRAW_MODE_SELECT_LIST )
    {
      m_pMenuCursor->SetVisible( true );
    }
  }

  /**
    * @brief レイアウトワークを生成する
    */
  void ConfigLowerUIDraw::_Initialize2D( void* pAppLytBuff )
  {
    //  2D画面作成
    {
      // アーカイブIDテーブル
      const app::util::G2DUtil::LytResData resTbl[] =
      {
        {  pAppLytBuff, 32, (u32)app::util::G2DUtil::ATTACH_ROOT },
      };

      //  作成するレイアウトワーク
      const app::util::G2DUtil::LytwkData setupData[ LYT_WK_TYPE_MAX ] =
      {
        //  選択画面
        {
          0,
          LYTRES_LYT_CONFIG_MIN_LOW_000_BFLYT,
          LA_LYT_CONFIG_MIN_LOW_000___NUM,
          pAppLytBuff,
          true,
          ::app::util::G2DUtil::SETUP_LOWER,
          true
        }
      };

      UIView::Create2D( m_pUtilHeap, NULL, 32, resTbl, GFL_NELEMS(resTbl), setupData, GFL_NELEMS(setupData), m_initParam.pMsgData, m_initParam.pWordSet );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void ConfigLowerUIDraw::_UpdateState()
  {
    if( m_state == STATE_TYPE_IN )
    {
      _SetState( STATE_TYPE_UPDATE );
    }

    if( m_state == STATE_TYPE_UPDATE )
    {
    }

    if( m_state == STATE_TYPE_OUT )
    {
      _SetState( STATE_TYPE_END );
    }

    {
      for( u32 i = 0; i < m_initParam.listMax; ++i )
      {
        m_aSelectListItem[ i ].Update();
      }

      m_pMenuCursor->Update();
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void ConfigLowerUIDraw::_SetState( const StateType newStateType )
  {
    GFL_ASSERT( m_state != newStateType );

    m_state = newStateType;

    if( m_state == STATE_TYPE_NONE )
    {
    }
    else if( m_state == STATE_TYPE_IN )
    {
      UIView::SetVisible( true );
    }
    else if( m_state == STATE_TYPE_UPDATE )
    {
      gfl2::lyt::LytPane* pActiveCursorPosPane  = m_aSelectListItem[ m_nowListItemCurrentPos ].GetCursorPosPane();
      m_pMenuCursor->Put( UIView::GetLayoutWork( LYT_WK_TYPE_SCREEN ), pActiveCursorPosPane );
      m_pMenuCursor->StartAnime();
    }
    else if( m_state == STATE_TYPE_OUT )
    {
      m_pMenuCursor->SetVisible( false );
    }
    else if( m_state ==  STATE_TYPE_END )
    {
      UIView::SetVisible( false );
    }
  }

GFL_NAMESPACE_END( config )
GFL_NAMESPACE_END( app )
