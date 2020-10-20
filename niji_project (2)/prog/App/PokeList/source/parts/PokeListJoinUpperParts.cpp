//==============================================================================
/**
 * @file    PokeListJoinUpperParts.cpp
 * @author  fukushima_yuya
 * @date    2015.05.13
 * @brief   ポケモンリスト参加選択上画面パーツ
 */
//==============================================================================

// niji
#include "App/PokeList/source/parts/PokeListJoinUpperParts.h"
#include "App/PokeList/source/parts/PokeListDrawerBase.h"
#include "App/PokeList/source/work/PokeListLocalWork.h"
#include "App/PokeList/source/work/PokeListStaticWork.h"

#include <AppLib/include/Ui/UIView.h>
#include <AppLib/include/Tool/app_tool_PokeIcon.h>

// arc
#include <niji_conv_header/app/common_pokeList/common_pokeList.h>
#include <niji_conv_header/app/common_pokelist/common_pokelist_pane.h>
#include <niji_conv_header/app/common_pokelist/common_pokeList_anim_list.h>

#include <niji_conv_header/message/msg_pokelist.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//==============================================================================
// @brief   制限時間
//==============================================================================

  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   pBase   描画共通処理クラスポインタ
   */
  //----------------------------------------------------------------------------
  JoinTimeLimit::JoinTimeLimit( DrawerBase* pBase )
    : m_pBase( pBase )
  {
    // ペインのセット
    {
      m_pTitleTextBox = m_pBase->GetTextBox( PANENAME_POKELIST_OPP_UPP_000_PANE_TEXTBOX_00 );
      m_pTimeTextBox  = m_pBase->GetTextBox( PANENAME_POKELIST_OPP_UPP_000_PANE_TEXTBOX_01 );

      gfl2::lyt::LytPane* timeWnd = m_pBase->GetPane( PANENAME_POKELIST_OPP_UPP_000_PANE_WINDOW_00 );

      StaticWork* staticWork = m_pBase->GetStaticWork();
      {
        bool timeLimit = (staticWork->GetTimeLimitClass() != NULL);

        m_pBase->SetVisiblePane( m_pTitleTextBox, timeLimit );
        m_pBase->SetVisiblePane( m_pTimeTextBox, timeLimit );
        m_pBase->SetVisiblePane( timeWnd, timeLimit );
      }
    }

    app::ui::UIView* pUIView = m_pBase->GetUIView();
    pUIView->SetTextboxPaneMessage( m_pTitleTextBox, mes_pokelist_13_05 );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //----------------------------------------------------------------------------
  JoinTimeLimit::~JoinTimeLimit( void )
  {
    ;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   制限時間の表示を更新
   */
  //----------------------------------------------------------------------------
  void JoinTimeLimit::UpdateTime( void )
  {
    StaticWork* staticWork = m_pBase->GetStaticWork();

    if( staticWork->GetTimeLimitClass() )
    {
      u32 time = staticWork->GetTimeLimit();
      u32 min = time / 60;
      u32 sec = time - (min * 60);

      print::WordSet* pWordSet = m_pBase->GetWordSet();
      gfl2::str::MsgData* pMsgData = m_pBase->GetMsgData();
      gfl2::str::StrBuf* pStrBuf = m_pBase->GetStrBuf( 0 );
      gfl2::str::StrBuf* pTmpStrBuf = m_pBase->GetStrBuf( 1 );

      // 分
      pWordSet->RegisterNumber( 0, min, 2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );

      // 秒
      pWordSet->RegisterNumber( 1, sec, 2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );

      pMsgData->GetString( mes_pokelist_13_06, *pTmpStrBuf );
      pWordSet->Expand( pStrBuf, pTmpStrBuf );

      // テキストボックスに反映
      app::ui::UIView* pUIView = m_pBase->GetUIView();
      pUIView->SetTextboxPaneString( m_pTimeTextBox, pStrBuf );
    }
  }



//==============================================================================
// @brief   手持ちプレート
//==============================================================================

  //----------------------------------------------------------------------------
  // @brief   定数
  //----------------------------------------------------------------------------
  namespace {
    enum PLATE_KIND
    {
      PLATE_KIND_FRIEND,
      PLATE_KIND_ENEMY_0,
      PLATE_KIND_ENEMY_1,
      PLATE_KIND_NUM
    };

    //! @brief  プレートパーツ
    const u32 c_PlatePartsIndex[] = {
      PANENAME_POKELIST_OPP_UPP_000_PANE_PARTSMAINPLT_00,
      PANENAME_POKELIST_OPP_UPP_000_PANE_PARTSMAINPLT_01,
      PANENAME_POKELIST_OPP_UPP_000_PANE_PARTSMAINPLT_02,
    };

    //! @brief  ポケモンセット
    const u32 c_PokeSetIndex[] = {
      PANENAME_POKELIST_PLT_LOW_001_PANE_POKESET_00,
      PANENAME_POKELIST_PLT_LOW_001_PANE_POKESET_01,
      PANENAME_POKELIST_PLT_LOW_001_PANE_POKESET_02,
      PANENAME_POKELIST_PLT_LOW_001_PANE_POKESET_03,
      PANENAME_POKELIST_PLT_LOW_001_PANE_POKESET_04,
      PANENAME_POKELIST_PLT_LOW_001_PANE_POKESET_05,
    };

    //! @brief  ポケモンアイコンパーツ
    const u32 c_PokeIconPartsIndex[] = {
      PANENAME_POKELIST_PLT_LOW_001_PANE_POKEICON_00,
      PANENAME_POKELIST_PLT_LOW_001_PANE_POKEICON_01,
      PANENAME_POKELIST_PLT_LOW_001_PANE_POKEICON_02,
      PANENAME_POKELIST_PLT_LOW_001_PANE_POKEICON_03,
      PANENAME_POKELIST_PLT_LOW_001_PANE_POKEICON_04,
      PANENAME_POKELIST_PLT_LOW_001_PANE_POKEICON_05,
    };

    //! @brief  性別アイコン
    const u32 c_GenderPaneIndex[] = {
      PANENAME_POKELIST_PLT_LOW_001_PANE_GENDER_00,
      PANENAME_POKELIST_PLT_LOW_001_PANE_GENDER_01,
      PANENAME_POKELIST_PLT_LOW_001_PANE_GENDER_02,
      PANENAME_POKELIST_PLT_LOW_001_PANE_GENDER_03,
      PANENAME_POKELIST_PLT_LOW_001_PANE_GENDER_04,
      PANENAME_POKELIST_PLT_LOW_001_PANE_GENDER_05,
    };

    //! @brief  レベルメッセージ
    const u32 c_LvMsgIndex[] = {
      PANENAME_POKELIST_PLT_LOW_001_PANE_TEXTLV_00,
      PANENAME_POKELIST_PLT_LOW_001_PANE_TEXTLV_01,
      PANENAME_POKELIST_PLT_LOW_001_PANE_TEXTLV_02,
      PANENAME_POKELIST_PLT_LOW_001_PANE_TEXTLV_03,
      PANENAME_POKELIST_PLT_LOW_001_PANE_TEXTLV_04,
      PANENAME_POKELIST_PLT_LOW_001_PANE_TEXTLV_05,
    };

    //! @brief  レベル数値テキスト
    const u32 c_LvValueIndex[] = {
      PANENAME_POKELIST_PLT_LOW_001_PANE_POKELV_00,
      PANENAME_POKELIST_PLT_LOW_001_PANE_POKELV_01,
      PANENAME_POKELIST_PLT_LOW_001_PANE_POKELV_02,
      PANENAME_POKELIST_PLT_LOW_001_PANE_POKELV_03,
      PANENAME_POKELIST_PLT_LOW_001_PANE_POKELV_04,
      PANENAME_POKELIST_PLT_LOW_001_PANE_POKELV_05,
    };

    //! @brief  性別変更アニメ(男)
    const u32 c_GenderAnimMale[PLATE_KIND_NUM][PLATE_NUM] = {
      {
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_00__GENDER_00_ICON_MALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_00__GENDER_01_ICON_MALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_00__GENDER_02_ICON_MALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_00__GENDER_03_ICON_MALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_00__GENDER_04_ICON_MALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_00__GENDER_05_ICON_MALE,
      },
      {
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_01__GENDER_00_ICON_MALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_01__GENDER_01_ICON_MALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_01__GENDER_02_ICON_MALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_01__GENDER_03_ICON_MALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_01__GENDER_04_ICON_MALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_01__GENDER_05_ICON_MALE,
      },
      {
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_02__GENDER_00_ICON_MALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_02__GENDER_01_ICON_MALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_02__GENDER_02_ICON_MALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_02__GENDER_03_ICON_MALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_02__GENDER_04_ICON_MALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_02__GENDER_05_ICON_MALE,
      },
    };
    //! @brief  性別変更アニメ(女)
    const u32 c_GenderAnimFemale[PLATE_KIND_NUM][PLATE_NUM] = {
      {
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_00__GENDER_00_ICON_FEMALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_00__GENDER_01_ICON_FEMALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_00__GENDER_02_ICON_FEMALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_00__GENDER_03_ICON_FEMALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_00__GENDER_04_ICON_FEMALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_00__GENDER_05_ICON_FEMALE,
      },
      {
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_01__GENDER_00_ICON_FEMALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_01__GENDER_01_ICON_FEMALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_01__GENDER_02_ICON_FEMALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_01__GENDER_03_ICON_FEMALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_01__GENDER_04_ICON_FEMALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_01__GENDER_05_ICON_FEMALE,
      },
      {
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_02__GENDER_00_ICON_FEMALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_02__GENDER_01_ICON_FEMALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_02__GENDER_02_ICON_FEMALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_02__GENDER_03_ICON_FEMALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_02__GENDER_04_ICON_FEMALE,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_02__GENDER_05_ICON_FEMALE,
      },
    };

    const u32 c_PlateType[PLATE_KIND_NUM][2] = {
      {
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_00_POS_00,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_00_POS_01,
      },
      {
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_01_POS_00,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_01_POS_01,
      },
      {
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_02_POS_00,
        LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_02_POS_01,
      },
    };
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   pBase   描画共通処理クラスポインタ
   */
  //----------------------------------------------------------------------------
  JoinPokePlate::JoinPokePlate( DrawerBase* pBase, PartyID partyID, IntermediateData* pParty, u8 index, JoinUppMode uppMode )
    : m_pBase( pBase )
    , m_PartyID( partyID )
    , m_pPartyData( pParty )
    , m_PlateIndex( index )
    , m_MemberCount( pml::PokeParty::MAX_MEMBERS )
    , m_pPlayerName( NULL )
  {
    for( u8 i=0; i<PLATE_NUM; ++i )
    {
      m_pPokeIconParts[i] = NULL;
      m_pPokeIconPane[i] = NULL;
      m_pItemIconPane[i] = NULL;
      m_pGenderIcon[i] = NULL;
      m_pLvMsgTextBox[i] = NULL;
      m_pLvNumTextBox[i] = NULL;
    }

    // 初期化
    Init();

    if( uppMode == JOIN_UPP_MODE_SINGLE )
    {
      m_pBase->StartAnim( c_PlateType[index][0] );
    }
    else {
      m_pBase->StartAnim( c_PlateType[index][1] );
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //----------------------------------------------------------------------------
  JoinPokePlate::~JoinPokePlate( void )
  {
    ;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //----------------------------------------------------------------------------
  void JoinPokePlate::Init( void )
  {
    StaticWork* staticWork = m_pBase->GetStaticWork();

    // パーティ情報の取得
    PartyID partyID = m_PartyID;
    IntermediateData* pParty = m_pPartyData;

    if( pParty == NULL ) return;

    // メンバーの数を取得
    m_MemberCount = staticWork->GetMemberCount( partyID );

    // プレートパーツ
    gfl2::lyt::LytParts* pPlateParts = m_pBase->GetParts( c_PlatePartsIndex[m_PlateIndex] );

    // プレイヤー名
    m_pPlayerName = m_pBase->GetTextBox( PANENAME_POKELIST_PLT_LOW_001_PANE_TEXTBOX_00, pPlateParts );

    for( u8 i=0; i<PLATE_NUM; ++i )
    {
      // ポケモンセット
      m_pPokeSet[i]       = m_pBase->GetPane( c_PokeSetIndex[i], pPlateParts );
      // ポケモンアイコン
      m_pPokeIconParts[i] = m_pBase->GetParts( c_PokeIconPartsIndex[i], pPlateParts );
      m_pPokeIconPane[i]  = m_pBase->GetPicture( PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON, m_pPokeIconParts[i] );      
      // 道具アイコン
      m_pItemIconPane[i]  = m_pBase->GetPicture( PANENAME_COMMON_BTN_LOW_001_PANE_ICON_ITEM, m_pPokeIconParts[i] );
      // 性別アイコン
      m_pGenderIcon[i]    = m_pBase->GetPane( c_GenderPaneIndex[i], pPlateParts );
      // レベル
      m_pLvMsgTextBox[i]  = m_pBase->GetTextBox( c_LvMsgIndex[i], pPlateParts );
      m_pLvNumTextBox[i]  = m_pBase->GetTextBox( c_LvValueIndex[i], pPlateParts );
    }

    // QRアイコン
    m_pQRIconPane = m_pBase->GetPane( PANENAME_POKELIST_PLT_LOW_001_PANE_POKELIST_ICON_QR, pPlateParts );

    // データのセット
    {
      // プレイヤー名
      SetPlayerName( partyID );

      for( u8 i=0; i<PLATE_NUM; ++i )
      {
        bool vis = (i < m_MemberCount);

        m_pBase->SetVisiblePane( m_pPokeSet[i], vis );

        if( vis )
        {
          const pml::pokepara::PokemonParam* pp = pParty->GetPokeConst( i );
          bool isEgg = false;

          bool fastmode_flag = pp->StartFastMode();
          {
            // タマゴかどうかを取得
            isEgg = pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG );

            // ポケモンアイコン
            SetPokeIcon( pp, i );

            // 性別アイコン
            SetGenderIcon( pp, i, isEgg );

            // 道具アイコン
            SetItemIcon( pp, i, isEgg );

            // レベル
            SetLevel( pp, i, isEgg );
          }
          pp->EndFastMode( fastmode_flag );
        }
      }

      // QRアイコン
      SetQRIcon();
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   プレイヤー名のセット
   *
   * @param   partyID   パーティID
   */
  //----------------------------------------------------------------------------
  void JoinPokePlate::SetPlayerName( PartyID partyID )
  {
    // プレイヤー名の取得
    app::ui::UIView* pUIView = m_pBase->GetUIView();
    StaticWork* pStaticWork = m_pBase->GetStaticWork();

    Savedata::MyStatus* pMyStatus = pStaticWork->GetMyStatus( partyID );
    gfl2::str::StrBuf* pNameBuf = m_pBase->GetStrBuf(0);

    if( pMyStatus )
    {
      pMyStatus->GetNameString( pNameBuf );
      pUIView->SetTextboxPaneString( m_pPlayerName, pNameBuf );
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ポケモンアイコンのセット
   *
   * @param   pp          ポケモンデータ
   * @param   memberIdx   メンバーインデックス
   */
  //----------------------------------------------------------------------------
  void JoinPokePlate::SetPokeIcon( const pml::pokepara::PokemonParam* pp, u8 memberIdx )
  {
    app::tool::PokeIconToolEx* pPokeIcon = m_pBase->GetPokeIcon();

    u8 plateID = m_PlateIndex;
    u32 iconID = (plateID * POKE_ICON_NUM) + memberIdx;
    pPokeIcon->SetPokeIcon( iconID, pp, m_pPokeIconPane[memberIdx] );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   道具アイコンのセット
   *
   * @param   pp          ポケモンデータ
   * @param   memberIdx   メンバーインデックス
   * @param   isEgg       タマゴフラグ
   */
  //----------------------------------------------------------------------------
  void JoinPokePlate::SetItemIcon( const pml::pokepara::PokemonParam* pp, u8 memberIdx, bool isEgg )
  {
    if( !isEgg )
    {
      u16 itemNo = pp->GetItem();

      m_pBase->SetVisiblePane( m_pItemIconPane[memberIdx], (itemNo != ITEM_DUMMY_DATA) );
    }
    else {
      m_pBase->SetVisiblePane( m_pItemIconPane[memberIdx], false );
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   性別アイコンのセット
   *
   * @param   pp          ポケモンデータ
   * @param   memberIdx   メンバーインデックス
   * @param   isEgg       タマゴフラグ
   */
  //----------------------------------------------------------------------------
  void JoinPokePlate::SetGenderIcon( const pml::pokepara::PokemonParam* pp, u8 memberIdx, bool isEgg )
  {
    if( !isEgg )
    {
      //pml::Sex sex = PokeTool::GetDrawSex( pp );
      pml::Sex sex = pp->GetSex();  //!< @fix NMCat[252]：ニックネームが表示されないので直接性別を参照する

      m_pBase->SetVisiblePane( m_pGenderIcon[memberIdx], (sex != pml::SEX_UNKNOWN) );

      if( sex != pml::SEX_UNKNOWN )
      {
        const u32* animArray = NULL;

        if( sex == pml::SEX_MALE )
        {
          animArray = c_GenderAnimMale[m_PlateIndex];
        }
        else {
          animArray = c_GenderAnimFemale[m_PlateIndex];
        }

        m_pBase->StartAnim( animArray[memberIdx] );
      }
    }
    else {
      m_pBase->SetVisiblePane( m_pGenderIcon[memberIdx], false );
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ポケモンのレベルのセット
   * @param   pp          ポケモンデータ
   * @param   memberIdx   メンバーインデックス
   * @param   isEgg       タマゴフラグ
   */
  //----------------------------------------------------------------------------
  void JoinPokePlate::SetLevel( const pml::pokepara::PokemonParam* pp, u8 memberIdx, bool isEgg )
  {
    if( !isEgg )
    {
      u32 level = pp->GetLevel();

      app::ui::UIView* view = m_pBase->GetUIView();
      {
        // [Lv]
        view->SetTextboxPaneMessage( m_pLvMsgTextBox[memberIdx], mes_pokelist_01_02 );

        // 数値
        view->SetTextboxPaneNumber( m_pLvNumTextBox[memberIdx], mes_pokelist_01_03, level, 3 );

      }
    }

    m_pBase->SetVisiblePane( m_pLvMsgTextBox[memberIdx], !isEgg );
    m_pBase->SetVisiblePane( m_pLvNumTextBox[memberIdx], !isEgg );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   QRアイコンのセット
   */
  //----------------------------------------------------------------------------
  void JoinPokePlate::SetQRIcon( void )
  {
    ModeContext context = m_pBase->GetStaticWork()->GetModeContext();
    {
      m_pBase->SetVisiblePane( m_pQRIconPane, GFL_BOOL_CAST(context.join.qrcode[m_PartyID]) );
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   プレートの色の設定
   * @param   color   PlateColorを指定(青 or 赤)
   */
  //----------------------------------------------------------------------------
  void JoinPokePlate::SetPlateColor( PlateColor color )
  {
    if( color >= COL_NUM )
    {
      GFL_ASSERT(0);
      color = COL_BLUE;
    }

    const u32 c_PlateColorAnim[] = {
      LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_00_COLOR_00,
      LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_01_COLOR_00,
      LA_COMMON_POKELIST_POKELIST_OPP_UPP_000__PARTSMAINPLT_02_COLOR_00,
    };
    u32 anm = c_PlateColorAnim[m_PlateIndex] + color;

    m_pBase->StartAnim( anm );
  }




GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )
