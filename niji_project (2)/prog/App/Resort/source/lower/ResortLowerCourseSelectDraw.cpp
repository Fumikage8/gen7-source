//======================================================================
/**
 * @file    ResortLowerCourseSelectDraw.cpp
 * @date    2015/10/02 12:09:01
 * @author  fukushima_yuya
 * @brief   ポケリゾート：コース選択画面(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortLowerCourseSelectDraw.h"

#include <App/Resort/source/ResortDrawerBase.h>
#include <App/Resort/source/ResortDrawListener.h>
#include <App/Resort/source/ResortInitLogic.h>
#include <App/Resort/source/ResortUtility.h>
#include <App/Resort/source/parts/ResortTalkMessage.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/ResortSave.h>
#include <Savedata/include/BoxPokemonSave.h>
#include <sound/include/Sound.h>

// arc
#include <niji_conv_header/app/resort/resort.h>
#include <niji_conv_header/app/resort/resort_pane.h>
#include <niji_conv_header/app/resort/resort_anim_list.h>
#include <niji_conv_header/message/msg_pokeresort_system.h>
#include <niji_conv_header/message/script/msg_pokeresort_talk.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
  #define COURSE_ANIM_NULL    (LA_RESORT_RESORT_COURSESELECT_LOW_000___NUM)

  enum {
    BTN_ID_CANCEL,
    BTN_ID_DECIDE,
    BTN_ID_ARROW_L,
    BTN_ID_ARROW_R,
    BTN_ID_NUM,
  };

  enum {
    GIM_COURSE_LV = 0,
    GIM_COURSE_HP,
    GIM_COURSE_ATK,
    GIM_COURSE_DEF,
    GIM_COURSE_SPATK,
    GIM_COURSE_SPDEF,
    GIM_COURSE_AGI,
    GIM_COURSE_MAX,

    DEV_COURSE_FUREAI = 0,
    DEV_COURSE_KINOMI,
    DEV_COURSE_TANKEN,
    DEV_COURSE_UNDOU,
    DEV_COURSE_ONSEN,
    DEV_COURSE_MAX,
  };
  
  static const app::ui::ButtonInfoEx c_DefInfo[] = {
    {
      BTN_ID_CANCEL, NULL, NULL,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__BACKBUTTON_TOUCH,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__BACKBUTTON_RELEASE,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__BACKBUTTON_CANSEL,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__BACKBUTTON_ACTIVE,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__BACKBUTTON_PASSIVE,
    },
    {
      BTN_ID_DECIDE, NULL, NULL,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__SELECTBUTTON_TOUCH,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__SELECTBUTTON_RELEASE,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__SELECTBUTTON_CANSEL,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__SELECTBUTTON_TOUCH_RELEASE,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__SELECTBUTTON_ACTIVE,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__SELECTBUTTON_PASSIVE,
    },
    {
      BTN_ID_ARROW_L, NULL, NULL,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__PAGECURSOR_L_TOUCH,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__PAGECURSOR_L_RELEASE,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__PAGECURSOR_L_CANSEL,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__PAGECURSOR_L_TOUCH_RELEASE,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__PAGECURSOR_L_ACTIVE,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__PAGECURSOR_L_PASSIVE,
    },
    {
      BTN_ID_ARROW_R, NULL, NULL,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__PAGECURSOR_R_TOUCH,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__PAGECURSOR_R_RELEASE,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__PAGECURSOR_R_CANSEL,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__PAGECURSOR_R_TOUCH_RELEASE,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__PAGECURSOR_R_ACTIVE,
      LA_RESORT_RESORT_COURSESELECT_LOW_000__PAGECURSOR_R_PASSIVE,
    },
  };

  static const u32 c_ButtonBaseIndex[BTN_ID_NUM] = {
    PANENAME_RESORT_COURSESELECT_LOW_000_PANE_BACKBUTTON,
    PANENAME_RESORT_COURSESELECT_LOW_000_PANE_SELECTBUTTON,
    PANENAME_RESORT_COURSESELECT_LOW_000_PANE_PAGECURSOR_L,
    PANENAME_RESORT_COURSESELECT_LOW_000_PANE_PAGECURSOR_R,
  };
  static const u32 c_ButtonBoundIndex[BTN_ID_NUM] = {
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_021_PANE_BOUND_00,
    PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00,
    PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00,
  };
  static const u32 c_DevelopImage[] = {
    LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGEMAIN_00,
    LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGEMAIN_01,
    LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGEMAIN_02,
  };
}

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   param   生成パラメータ
 */
//------------------------------------------------------------------------------
LowerCourseSelectDraw::LowerCourseSelectDraw( CreateParam& param )
  : LowerBaseDraw( param )
  , m_pTalkMessage(NULL)
  , m_CourseSelectID(COURSE_SELECT_NUM)
  , m_CourseIndex(0)
  , m_CourseMax(0)
  , m_PrevCourseImageAnim(COURSE_ANIM_NULL)
  , m_pHeaderMain(NULL)
  , m_pCourseSubImage(NULL)
  , m_pCourseName(NULL)
  , m_pCourseDesc(NULL)
  , m_pPokeCount(NULL)
  , m_pMameCount(NULL)
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void LowerCourseSelectDraw::Initialize2D( InitParam& param )
{
  const app::util::G2DUtil::LytResData resTbl[] = {
    { param.lytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_RESORT_RESORT_COURSESELECT_LOW_000_BFLYT,
      LA_RESORT_RESORT_COURSESELECT_LOW_000___NUM,
      param.lytBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  // 2Dの生成
  Create2D(
    m_pLytHeap,
    NULL,
    1,
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    param.msgSys,
    param.wordSet );

  this->SetVisible( false );
  this->SetInputEnabled( false );
}

//------------------------------------------------------------------------------
/**
 * @brief   クラスの初期化
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void LowerCourseSelectDraw::InitializeClass( InitParam& param )
{
  gfl2::heap::HeapBase* heap = m_pLytHeap->GetDeviceHeap();
  {
    // 描画共通処理
    m_pDrawerBase = GFL_NEW(heap) DrawerBase( this, m_pLytHeap, 0, param.wordSet, param.msgSys );
    {
      GFL_ASSERT( m_pDrawerBase );
    }
    
    // ボタンマネージャの生成
    CreateButtonManager( c_DefInfo, c_ButtonBaseIndex, c_ButtonBoundIndex, BTN_ID_NUM );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインの初期化
 */
//------------------------------------------------------------------------------
void LowerCourseSelectDraw::InitializePane( void )
{
  m_pHeaderMain     = m_pDrawerBase->GetTextBox( PANENAME_RESORT_COURSESELECT_LOW_000_PANE_HEADDER_MAIN );
  m_pCourseSubImage = m_pDrawerBase->GetPane( PANENAME_RESORT_COURSESELECT_LOW_000_PANE_SELECTIMAGE_01 );
  m_pCourseName     = m_pDrawerBase->GetTextBox( PANENAME_RESORT_COURSESELECT_LOW_000_PANE_HEADDER_L3 );
  m_pCourseDesc     = m_pDrawerBase->GetTextBox( PANENAME_RESORT_COURSESELECT_LOW_000_PANE_TEXTBOXL3 );
  m_pPokeCount      = m_pDrawerBase->GetTextBox( PANENAME_RESORT_COURSESELECT_LOW_000_PANE_TEXTNUM_01 );
  m_pMameCount      = m_pDrawerBase->GetTextBox( PANENAME_RESORT_COURSESELECT_LOW_000_PANE_TEXTNUM_00 );
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト
 */
//------------------------------------------------------------------------------
void LowerCourseSelectDraw::InitializeText( void )
{
  gfl2::lyt::LytTextBox* textBox = m_pDrawerBase->GetTextBox( PANENAME_RESORT_COURSESELECT_LOW_000_PANE_TEXTBOX_04 );
  {
    this->SetTextboxPaneMessage( textBox, msg_prsys_low_00_03 );
  }

  gfl2::lyt::LytParts* btnParts = m_pDrawerBase->GetParts( PANENAME_RESORT_COURSESELECT_LOW_000_PANE_SELECTBUTTON );
  {
    gfl2::lyt::LytTextBox* btnText = m_pDrawerBase->GetTextBox( PANENAME_COMMON_BTN_LOW_021_PANE_TEXTBOX_00, btnParts );
    {
      this->SetTextboxPaneMessage( btnText, msg_prsys_button_01 );
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   開く
 */
//------------------------------------------------------------------------------
void LowerCourseSelectDraw::Open( void )
{
  m_CourseIndex    = 0;
  m_CourseMax      = GetCourseMax();

  // コース選択のセットアップ
  SetupCourse();

  // リザルトの初期化
  this->InitResult();

  this->SetVisible( true );
  this->SetInputEnabled( true );
}

//------------------------------------------------------------------------------
/**
 * @brief   閉じる
 */
//------------------------------------------------------------------------------
void LowerCourseSelectDraw::Close( void )
{
  // レイアウトアニメの停止
  for( u32 i=0; i<GFL_NELEMS(c_DevelopImage); ++i )
  {
    m_pDrawerBase->StopAnim( c_DevelopImage[i] );
  }

  this->SetInputEnabled( false );
  this->SetVisible( false );
}


//------------------------------------------------------------------------------
/**
 * @brief   コース数の取得
 */
//------------------------------------------------------------------------------
u8 LowerCourseSelectDraw::GetCourseMax( void )
{
  u8 courseNum = 1;

  switch( m_CourseSelectID )
  {
  case COURSE_SELECT_ADV:
    {
      m_DevelopLevel[RESORT_ID_ADVENTURE] = util::GetResortLevel( m_pResortSave, RESORT_ID_ADVENTURE );
      u8 level  = m_DevelopLevel[RESORT_ID_ADVENTURE];

      if( level == RESORT_LV_2 )
      {
        courseNum = 3;
      }
      else if( level == RESORT_LV_3 )
      {
        courseNum = 4;
      }
    } break;

  case COURSE_SELECT_GIM:
    {
      courseNum = 7;
    } break;

  case COURSE_SELECT_DEVELOP:
    {
      static const char* c_ResortName[] = {
        "FureaiResort",
        "KinomiResort",
        "TankenResort",
        "UndouResort",
        "OnsenResort",
      };
      static const char* c_ResortLevelName[] = {
        "Level_0",
        "Level_1",
        "Level_2",
        "Level_3",
      };

      courseNum = 0;

      for( u8 i=0; i<RESORT_ID_MAX; ++i )
      {
        u8 level = util::GetResortLevel( m_pResortSave, i );

        if( level < RESORT_LV_3 )
        {
          m_DevelopResort[courseNum] = i;
          m_DevelopLevel[courseNum]  = level;
          courseNum++;

          FUKUSHIMA_PRINT( "DevelopResort Name[%s], Level[%s]\n"
            , c_ResortName[i], c_ResortLevelName[level] );
        }
      }
    } break;
  }

  return courseNum;
}

//------------------------------------------------------------------------------
/**
 * @brief   各コースの表示
 */
//------------------------------------------------------------------------------
void LowerCourseSelectDraw::SetupCourse( void )
{
  // コース選択の設定
  SetCourseSelect();
  // コース選択タイトルの設定
  SetTitleText();
  // イメージの変更
  SetCourseImage();
  // テキストのセット
  SetCourseText();
  // 必要なアイテムの表示
  SetNeedItemCount();
  // 決定ボタンの設定
  SetDecideButton();
  // ページ切り替えボタンの設定
  SetArrowButton();
}

//------------------------------------------------------------------------------
/**
 * @brief   コース選択の設定
 */
//------------------------------------------------------------------------------
void LowerCourseSelectDraw::SetCourseSelect( void )
{
  static const u32 c_TypeAnim[] = {
    LA_RESORT_RESORT_COURSESELECT_LOW_000_COMMON_PAGESELECT_00,
    LA_RESORT_RESORT_COURSESELECT_LOW_000_COMMON_PAGESELECT_00,
    LA_RESORT_RESORT_COURSESELECT_LOW_000_IKADA_00,
  };

  m_pDrawerBase->StartAnim( c_TypeAnim[m_CourseSelectID] );
}

//------------------------------------------------------------------------------
/**
 * @brief   コース選択タイトルの設定
 */
//------------------------------------------------------------------------------
void LowerCourseSelectDraw::SetTitleText( void )
{
  static const u32 c_TitleMsg[] = {
    msg_prsys_low_menu_title_03,
    msg_prsys_low_menu_title_04,
    msg_prsys_low_menu_title_00,
  };

  this->SetTextboxPaneMessage( m_pHeaderMain, c_TitleMsg[m_CourseSelectID] );
}

//------------------------------------------------------------------------------
/**
 * @brief   コースイメージの切り替え
 */
//------------------------------------------------------------------------------
void LowerCourseSelectDraw::SetCourseImage( void )
{
  // サブイメージを非表示
  m_pDrawerBase->SetVisiblePane( m_pCourseSubImage, false );

  switch( m_CourseSelectID )
  {
  case COURSE_SELECT_ADV:
    {
      u32 mainAnimID = GetAdvMainImageAnim();
      {
        m_pDrawerBase->StartAnim( mainAnimID );
      }
      u32 subAnimID = GetAdvSubImageAnim( m_CourseIndex );
      {
        m_pDrawerBase->StartAnim( subAnimID );
      }
      // サブイメージを表示
      m_pDrawerBase->SetVisiblePane( m_pCourseSubImage, true );
    } break;

  case COURSE_SELECT_GIM:
    {
      static const u32 c_UndouCourseImage[] = {
        LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGEMAIN_03,
        LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGEMAIN_09,
        LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGEMAIN_04,
        LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGEMAIN_05,
        LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGEMAIN_06,
        LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGEMAIN_07,
        LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGEMAIN_08,
      };

      m_pDrawerBase->StartAnim( c_UndouCourseImage[m_CourseIndex] );
    } break;

  case COURSE_SELECT_DEVELOP:
    {
      if( m_PrevCourseImageAnim != COURSE_ANIM_NULL )
      {
        m_pDrawerBase->StopAnim( m_PrevCourseImageAnim );
      }

      u8 resort = m_DevelopResort[m_CourseIndex];
      u8 level  = m_DevelopLevel[m_CourseIndex];
      m_PrevCourseImageAnim = c_DevelopImage[level];

      m_pDrawerBase->StartAnim( m_PrevCourseImageAnim, false );
      m_pDrawerBase->SetAnimFrame( m_PrevCourseImageAnim, static_cast<f32>( resort ) );

      FUKUSHIMA_PRINT( "CourseImage[%d] : resort[%d], level[%d]\n", m_CourseIndex, resort, level );
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   たんけんリゾートのメインイメージアニメの取得
 *
 * @return  アニメID
 */
//------------------------------------------------------------------------------
u32 LowerCourseSelectDraw::GetAdvMainImageAnim( void )
{
  static const u32 c_TankenLvImage[] = {
    LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGEMAIN_10,
    LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGEMAIN_11,
    LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGEMAIN_12,
  };

  u8 level = util::GetResortLevel( m_pResortSave, RESORT_ID_ADVENTURE );
  {
    return c_TankenLvImage[level - 1];
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   たんけんリゾートのサブイメージアニメの取得
 *
 * @param   index   コースインデックス
 *
 * @return  アニメID
 */
//------------------------------------------------------------------------------
u32 LowerCourseSelectDraw::GetAdvSubImageAnim( u32 index )
{
  static const u32 c_AdventureSubImageLevel1[] = {
    LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGESUB_00,
    LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGESUB_00,
    LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGESUB_00,
    LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGESUB_00,
  };
  static const u32 c_AdventureSubImageLevel2[] = {
    LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGESUB_00,
    LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGESUB_01,
    LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGESUB_03,
    LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGESUB_03,
  };
  static const u32 c_AdventureSubImageLevel3[] = {
    LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGESUB_00,
    LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGESUB_01,
    LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGESUB_02,
    LA_RESORT_RESORT_COURSESELECT_LOW_000_IMAGECHANGESUB_03,
  };
  static const u32* c_AdventureSubImage[] = {
    c_AdventureSubImageLevel1,
    c_AdventureSubImageLevel2,
    c_AdventureSubImageLevel3,
  };

  // たんけんリゾートのレベルを取得
  u8 level = util::GetResortLevel( m_pResortSave, RESORT_ID_ADVENTURE );
  {
    const u32* animTable = c_AdventureSubImage[level - 1];
    {
      return animTable[index];
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   コーステキストの切り替え
 */
//------------------------------------------------------------------------------
void LowerCourseSelectDraw::SetCourseText( void )
{
  u32 nameIndex = 0;
  u32 descIndex = 0;

  switch( m_CourseSelectID )
  {
  case COURSE_SELECT_ADV:
    {
      // たんけんリゾートのテキストを取得
      GetAdvCourseText( m_CourseIndex, &nameIndex, &descIndex );
    } break;

  case COURSE_SELECT_GIM:
    {
      static const u32 c_UndouName[] = {
        msg_prsys_low_menu_04_01_01,
        msg_prsys_low_menu_04_01_07,
        msg_prsys_low_menu_04_01_02,
        msg_prsys_low_menu_04_01_03,
        msg_prsys_low_menu_04_01_04,
        msg_prsys_low_menu_04_01_05,
        msg_prsys_low_menu_04_01_06,
      };
      static const u32 c_UndouDesc[] = {
        msg_prsys_low_menu_04_02_01,
        msg_prsys_low_menu_04_02_07,
        msg_prsys_low_menu_04_02_02,
        msg_prsys_low_menu_04_02_03,
        msg_prsys_low_menu_04_02_04,
        msg_prsys_low_menu_04_02_05,
        msg_prsys_low_menu_04_02_06,
      };
      nameIndex = c_UndouName[m_CourseIndex];
      descIndex = c_UndouDesc[m_CourseIndex];
    } break;

  case COURSE_SELECT_DEVELOP:
    {
      static const u32 c_DevelopName[RESORT_ID_MAX][RESORT_DEV_LV_NUM] = {
        {
          msg_prsys_low_menu_00_01_01,
          msg_prsys_low_menu_00_01_02,
          msg_prsys_low_menu_00_01_03,
        },
        {
          msg_prsys_low_menu_00_01_04,
          msg_prsys_low_menu_00_01_05,
          msg_prsys_low_menu_00_01_06,
        },
        {
          msg_prsys_low_menu_00_01_07,
          msg_prsys_low_menu_00_01_08,
          msg_prsys_low_menu_00_01_09,
        },
        {
          msg_prsys_low_menu_00_01_10,
          msg_prsys_low_menu_00_01_11,
          msg_prsys_low_menu_00_01_12,
        },
        {
          msg_prsys_low_menu_00_01_13,
          msg_prsys_low_menu_00_01_14,
          msg_prsys_low_menu_00_01_15,
        },
      };

      static const u32 c_DevelopDesc[RESORT_ID_MAX][RESORT_DEV_LV_NUM] = {
        {
          msg_prsys_low_menu_00_02_01,
          msg_prsys_low_menu_00_02_02,
          msg_prsys_low_menu_00_02_03,
        },
        {
          msg_prsys_low_menu_00_02_04,
          msg_prsys_low_menu_00_02_05,
          msg_prsys_low_menu_00_02_06,
        },
        {
          msg_prsys_low_menu_00_02_07,
          msg_prsys_low_menu_00_02_08,
          msg_prsys_low_menu_00_02_09,
        },
        {
          msg_prsys_low_menu_00_02_10,
          msg_prsys_low_menu_00_02_11,
          msg_prsys_low_menu_00_02_12,
        },
        {
          msg_prsys_low_menu_00_02_13,
          msg_prsys_low_menu_00_02_14,
          msg_prsys_low_menu_00_02_15,
        },
      };

      u8 resort = m_DevelopResort[m_CourseIndex];
      u8 level  = m_DevelopLevel[m_CourseIndex];

      nameIndex = c_DevelopName[resort][level];
      descIndex = c_DevelopDesc[resort][level];
    } break;
  }

  // メッセージのセット
  this->SetTextboxPaneMessage( m_pCourseName, nameIndex );
  this->SetTextboxPaneMessage( m_pCourseDesc, descIndex );
}

//------------------------------------------------------------------------------
/**
 * @brief   コース数の取得
 *
 * @param   index     [in]コースインデックス
 * @param   nameMsg   [out]名前テキストID
 * @param   descMsg   [out]説明テキストID
 */
//------------------------------------------------------------------------------
void LowerCourseSelectDraw::GetAdvCourseText( u32 index, u32* nameMsg, u32* descMsg )
{
  static const u32 c_AdventureLevel1NameText[] = {
    msg_prsys_low_menu_03_01_01,
    msg_prsys_low_menu_03_01_01,
    msg_prsys_low_menu_03_01_01,
    msg_prsys_low_menu_03_01_01,
  };
  static const u32 c_AdventureLevel2NameText[] = {
    msg_prsys_low_menu_03_01_01,
    msg_prsys_low_menu_03_01_02,
    msg_prsys_low_menu_03_01_04,
    msg_prsys_low_menu_03_01_04,
  };
  static const u32 c_AdventureLevel3NameText[] = {
    msg_prsys_low_menu_03_01_01,
    msg_prsys_low_menu_03_01_02,
    msg_prsys_low_menu_03_01_03,
    msg_prsys_low_menu_03_01_04,
  };
  static const u32* c_AdventureNameText[] = {
    c_AdventureLevel1NameText,
    c_AdventureLevel2NameText,
    c_AdventureLevel3NameText,
  };

  static const u32 c_AdventureLevel1DescText[] = {
    msg_prsys_low_menu_03_02_01,
    msg_prsys_low_menu_03_02_01,
    msg_prsys_low_menu_03_02_01,
    msg_prsys_low_menu_03_02_01,
  };
  static const u32 c_AdventureLevel2DescText[] = {
    msg_prsys_low_menu_03_02_01,
    msg_prsys_low_menu_03_02_02,
    msg_prsys_low_menu_03_02_04,
    msg_prsys_low_menu_03_02_04,
  };
  static const u32 c_AdventureLevel3DescText[] = {
    msg_prsys_low_menu_03_02_01,
    msg_prsys_low_menu_03_02_02,
    msg_prsys_low_menu_03_02_03,
    msg_prsys_low_menu_03_02_04,
  };
  static const u32* c_AdventureDescText[] = {
    c_AdventureLevel1DescText,
    c_AdventureLevel2DescText,
    c_AdventureLevel3DescText,
  };

  // たんけんリゾートのレベルを取得
  u8 level = util::GetResortLevel( m_pResortSave, RESORT_ID_ADVENTURE );
  {
    const u32* nameTable = c_AdventureNameText[level - 1];
    {
      *nameMsg = nameTable[index];
    }
    const u32* descTable = c_AdventureDescText[level - 1];
    {
      *descMsg = descTable[index];
    }
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   必要なアイテムの表示
 */
//------------------------------------------------------------------------------
void LowerCourseSelectDraw::SetNeedItemCount( void )
{
  if( m_CourseSelectID != COURSE_SELECT_DEVELOP ) return;

  u8 resort = m_DevelopResort[m_CourseIndex];
  u8 level  = m_DevelopLevel[m_CourseIndex];

  this->SetTextboxPaneNumber( m_pPokeCount, msg_prsys_common_04, c_NeedItemNum[resort][level][0], 3 );
  this->SetTextboxPaneNumber( m_pMameCount, msg_prsys_common_04, c_NeedItemNum[resort][level][1], 3 );
}

//------------------------------------------------------------------------------
/**
 * @brief   決定ボタンの設定
 */
//------------------------------------------------------------------------------
void LowerCourseSelectDraw::SetDecideButton( void )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();

  GFL_ASSERT( m_pTalkMessage );

  // 開発
  if( m_CourseSelectID == COURSE_SELECT_DEVELOP )
  {
    u8 resort = m_DevelopResort[m_CourseIndex];
    u8 level  = m_DevelopLevel[m_CourseIndex];

    u32 pokeCount = util::GetBoxPokeAndResortPokeNum( m_pResortSave, m_pBoxPokeSave, m_pLytHeap );
    u32 beansCount[3] = { 0, 0, 0 };
    {
      util::GetRankBeansCount( m_pResortSave, beansCount );
    }

    u32 needPokeCount  = c_NeedItemNum[resort][level][0];
    u32 needBeansCount = c_NeedItemNum[resort][level][1];

    if( needPokeCount <= pokeCount && needBeansCount <= beansCount[0] )
    {
      m_pTalkMessage->TalkStart( msg_prwin_sys_00_11, gfl2::str::StrWin::FINISH_NONE );
      btnMan->SetButtonActive( BTN_ID_DECIDE );
    }
    else if( needPokeCount > pokeCount )
    {
      m_pTalkMessage->TalkStart( msg_prwin_sys_00_07, gfl2::str::StrWin::FINISH_NONE );
      btnMan->SetButtonPassive( BTN_ID_DECIDE );
    }
    else {
      m_pTalkMessage->TalkStart( msg_prwin_sys_00_08, gfl2::str::StrWin::FINISH_NONE );
      btnMan->SetButtonPassive( BTN_ID_DECIDE );
    }
  }
  else {
    btnMan->SetButtonActive( BTN_ID_DECIDE );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ページ切り替えボタンの設定
 */
//------------------------------------------------------------------------------
void LowerCourseSelectDraw::SetArrowButton( void )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    // １ページ分しかない時はパッシブにしておく
    if( m_CourseMax == 1 )
    {
      btnMan->SetButtonPassive( BTN_ID_ARROW_L );
      btnMan->SetButtonPassive( BTN_ID_ARROW_R );
    }
    else {
      btnMan->SetButtonActive( BTN_ID_ARROW_L );
      btnMan->SetButtonActive( BTN_ID_ARROW_R );
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   次のコースへ
 */
//------------------------------------------------------------------------------
void LowerCourseSelectDraw::NextCourse( void )
{
  u8 course = m_CourseIndex;

  course = (course + 1) % m_CourseMax;

  if( m_CourseIndex != course )
  {
    m_CourseIndex = course;

    // イメージの変更
    SetCourseImage();

    // テキストのセット
    SetCourseText();

    // 必要なアイテムの表示
    SetNeedItemCount();

    // 決定ボタンの設定
    SetDecideButton();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   前のコースへ
 */
//------------------------------------------------------------------------------
void LowerCourseSelectDraw::PrevCourse( void )
{
  u8 course = m_CourseIndex;

  if( course == 0 )
  {
    course = m_CourseMax - 1;
  }
  else {
    course--;
  }

  if( m_CourseIndex != course )
  {
    m_CourseIndex = course;

    // イメージの変更
    SetCourseImage();

    // テキストのセット
    SetCourseText();

    // 必要なアイテムの表示
    SetNeedItemCount();

    // 決定ボタンの設定
    SetDecideButton();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   コースを決定
 */
//------------------------------------------------------------------------------
void LowerCourseSelectDraw::DecideCourse( void )
{
  switch( m_CourseSelectID )
  {
  case COURSE_SELECT_ADV:
    {
      // コースの設定
      SetCourseID_Adventure();
      // たんけんリゾートの開始
      //m_pInitLogic->StartAdventure();
      // たんけんリゾート上画面の更新
      m_uiListener->UpdateUpper( RESORT_ID_ADVENTURE );
    } break;

  case COURSE_SELECT_GIM:
    {
      // 運動の種類を設定
      Savedata::ResortSave::PokeResortGimKind kind = static_cast<Savedata::ResortSave::PokeResortGimKind>( m_CourseIndex );
      m_pResortSave->SetGimKind( m_GroupIndex, kind );
      // ポケモンデータ設定
      u32 start = m_GroupIndex * Savedata::ResortSave::RESORT_GIM_KIND_NUM_MAX;
      pml::pokepara::CoreParam * cp = GFL_NEW_LOW(m_pLytHeap->GetDeviceHeap()) pml::pokepara::CoreParam( m_pLytHeap->GetDeviceHeap() );
      if( kind == Savedata::ResortSave::RESORT_GIM_KIND_LV )
      {
        for( u32 i=0; i<Savedata::ResortSave::RESORT_GIM_KIND_NUM_MAX; i++ )
        {
          m_pResortSave->GetGimPokemon( start+i, cp );
          m_pResortSave->SetGimStartValue( start+i, cp->GetLevel() );
          cp->SetResortEventStatus( pml::pokepara::RESORTEVENT_PLAY_STATE );
          m_pResortSave->SetGimPokemon( start+i, cp );
        }
      }
      else
      {
        pml::pokepara::PowerID eff = m_pResortSave->GetGimKindPowerID( kind );
        for( u32 i=0; i<Savedata::ResortSave::RESORT_GIM_KIND_NUM_MAX; i++ )
        {
          m_pResortSave->GetGimPokemon( start+i, cp );
          m_pResortSave->SetGimStartValue( start+i, cp->GetEffortPower(eff) );
          cp->SetResortEventStatus( pml::pokepara::RESORTEVENT_PLAY_STATE );
          m_pResortSave->SetGimPokemon( start+i, cp );
        }
      }
      GFL_DELETE cp;
    } break;

  case COURSE_SELECT_DEVELOP:
    {
      u8 resort = m_DevelopResort[m_CourseIndex];
      u8 level  = m_DevelopLevel[m_CourseIndex];

      u32 useBeansNum = c_NeedItemNum[resort][level][1];

      m_DevResort = resort;
      //m_DevNeedNum = useBeansNum;

      // ポケマメの消費
      {
        m_pResortSave->SubPokeBeansNum( useBeansNum, 0, 0 );
      }
      // リゾートのレベルアップ
      {
        Savedata::ResortSave::PokeResortKind resortID =
          static_cast<Savedata::ResortSave::PokeResortKind>( resort );
        u8 resLevel = m_pResortSave->GetResortLevel( resortID );
        m_pResortSave->SetResortLevel( resortID, (resLevel + 1) );
      }
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   コースの設定：たんけんリゾート
 */
//------------------------------------------------------------------------------
void LowerCourseSelectDraw::SetCourseID_Adventure( void )
{
  static const u8 c_AdvCourseID_Level1[] = {
    Savedata::ResortSave::RESORT_ADV_COURSE_KAKERA,
    Savedata::ResortSave::RESORT_ADV_COURSE_KAKERA,
    Savedata::ResortSave::RESORT_ADV_COURSE_KAKERA,
    Savedata::ResortSave::RESORT_ADV_COURSE_KAKERA,
  };
  static const u8 c_AdvCourseID_Level2[] = {
    Savedata::ResortSave::RESORT_ADV_COURSE_KAKERA,
    Savedata::ResortSave::RESORT_ADV_COURSE_ISHI,
    Savedata::ResortSave::RESORT_ADV_COURSE_DOUGU1,
    Savedata::ResortSave::RESORT_ADV_COURSE_DOUGU1,
  };
  static const u8 c_AdvCourseID_Level3[] = {
    Savedata::ResortSave::RESORT_ADV_COURSE_KAKERA,
    Savedata::ResortSave::RESORT_ADV_COURSE_ISHI,
    Savedata::ResortSave::RESORT_ADV_COURSE_OTAKARA,
    Savedata::ResortSave::RESORT_ADV_COURSE_DOUGU2,
  };
  static const u8* c_AdvCourseID[] = {
    c_AdvCourseID_Level1,
    c_AdvCourseID_Level2,
    c_AdvCourseID_Level3,
  };

  u8 level = util::GetResortLevel( m_pResortSave, RESORT_ID_ADVENTURE );
  {
    const u8* courseTable = c_AdvCourseID[level - 1];
    {
      const u8 courseID = courseTable[m_CourseIndex];
      {
        GFL_PRINT( "DecideCourse[Adventure] : ID[%d]\n", courseID );

        m_pResortSave->SetAdventureCoursePattern( courseID );
      }
    }
  }
}







//==============================================================================
/**
 *      app::ui::UIInputListener
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 *
 * @param   paneID
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult LowerCourseSelectDraw::OnLayoutPaneEvent( const u32 paneID )
{
  if( !m_uiListener ) return ENABLE_ACCESS;

  if( paneID == BTN_ID_CANCEL )
  {
    m_Result = RESULT_CANCEL;
  }
  else if( paneID == BTN_ID_DECIDE )
  {
    DecideCourse();

    m_Result = RESULT_DECIDE;
  }
  else if( paneID == BTN_ID_ARROW_L )
  {
    PrevCourse();
  }
  else if( paneID == BTN_ID_ARROW_R )
  {
    NextCourse();
  }

  return ENABLE_ACCESS;
}






//==============================================================================
/**
 *      app::tool::ButtonManager
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ボタンの個別セットアップ
 *
 * @param   btnMan    ボタンマネージャ
 */
//------------------------------------------------------------------------------
void LowerCourseSelectDraw::SetupButton( app::tool::ButtonManager* btnMan )
{
  btnMan->SetButtonBindKey( BTN_ID_ARROW_L, gfl2::ui::BUTTON_L );
  btnMan->SetButtonBindKey( BTN_ID_ARROW_R, gfl2::ui::BUTTON_R );
  
  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    if( i == BTN_ID_CANCEL )
    {
      btnMan->SetButtonSelectSE( i, SEQ_SE_CANCEL1 );
    }
    else {
      btnMan->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
    }
  }
}

GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )
