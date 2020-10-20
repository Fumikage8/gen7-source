//==============================================================================
/**
 * @file    PokeListMenuWindowDraw.h
 * @author  fukushima_yuya
 * @date    2015.03.19
 * @brief   ポケモンリストメニューウィンドウ
 */
//==============================================================================


// niji
#include "App/PokeList/source/parts/PokeListDrawerBase.h"
#include "App/PokeList/source/parts/PokeListMenuWindowDraw.h"
#include "App/PokeList/source/work/PokeListLocalWork.h"
#include "App/PokeList/source/work/PokeListStaticWork.h"

#include <AppLib/include/Tool/app_tool_MenuWindow.h>
#include <AppLib/include/Ui/UIView.h>

// arc
#include <niji_conv_header/app/common_pokeList/common_pokeList_pane.h>
#include <niji_conv_header/app/common_pokeList/common_pokeList_anim_list.h>
#include <niji_conv_header/message/msg_pokelist.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )


//------------------------------------------------------------------------------
// @brief   変数
//------------------------------------------------------------------------------
//! @brief  選択肢ごとの設定
const MenuWindowDraw::AltDesc MenuWindowDraw::ALT_DESC[ALT_NUM] = {
  { mes_pokelist_01_10,     false },   //!< ALT_POKE_STATUS
  { mes_pokelist_01_11,     false },   //!< ALT_POKE_REPAIR
  { mes_pokelist_01_12,     false },   //!< ALT_POKE_ITEM
  { mes_pokelist_01_14,     true  },   //!< ALT_POKE_RETURN
  { mes_pokelist_01_15,     false },   //!< ALT_ITEM_USE
  { mes_pokelist_01_16,     false },   //!< ALT_ITEM_STORE
  { mes_pokelist_01_17,     false },   //!< ALT_ITEM_HAND
  { mes_pokelist_01_14,     true  },   //!< ALT_ITEM_RETURN
  { mes_pokelist_01_18,     false },   //!< ALT_JOIN_JOIN
  { mes_pokelist_01_19,     false },   //!< ALT_JOIN_CANCEL
  { mes_pokelist_01_10,     false },   //!< ALT_JOIN_STATUS
  { mes_pokelist_01_14,     true  },   //!< ALT_JOIN_RETURN
  { mes_pokelist_01_20,     false },   //!< ALT_SODATEYA_DECIDE
  { mes_pokelist_01_10,     false },   //!< ALT_SODATEYA_STATUS
  { mes_pokelist_01_14,     true  },   //!< ALT_SODATEYA_RETURN
  { mes_pokelist_yes,       false },   //!< ALT_YESNO_YES
  { mes_pokelist_no,        true  },   //!< ALT_YESNO_NO
  { mes_pokelist_01_14,     false },   //!< ALT_PROMOTION_DECIDE
  { mes_pokelist_01_10,     false },   //!< ALT_PROMOTION_STATUS
  { mes_pokelist_01_14,     true  },   //!< ALT_PROMOTION_RETURN
  { mes_pokelist_01_11,     false },   //!< ALT_POKE_FLY
};

//! @brief  サブウィンドウインアニメ
const u32 c_SubWndAnim[] = {
  LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_COMMANDSELECT_IN,
  LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_COMMANDSELECT_OUT,
};

//! @brief  
const s32 c_ItemExplainMsgID[] = {
  mes_pokelist_03_01,
  mes_pokelist_03_02,
  mes_pokelist_03_03,
  -1,
};



//==============================================================================
// @brief   メニューウィンドウクラス
//==============================================================================

  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   param   メニュー生成パラメータ
   */
  //----------------------------------------------------------------------------
  MenuWindowDraw::MenuWindowDraw( CreateMenuParam param )
    : m_pBase( param.pBase )
    , m_ActMenuID( MENU_NULL )
    , m_ActMenuCPos( CPOS_NULL )
    , m_ActPokeIndex( 0 )
    , m_PrevPos( 0 )
  {
    // メニューの生成
    CreateMenu( param );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //----------------------------------------------------------------------------
  MenuWindowDraw::~MenuWindowDraw( void )
  {
    // メニューの破棄
    //DeleteMenu();
  }



  //------------------------------------------------------------------------------
  /**
   * @brief   生成
   * @param   param   メニュー生成パラメータ
   */
  //------------------------------------------------------------------------------
  void MenuWindowDraw::CreateMenu( CreateMenuParam param )
  {
    StaticWork* pStaticWork = m_pBase->GetStaticWork();
    IntermediateData* pData = pStaticWork->GetPokeParty( PARTY_SELF );

    for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
    {
      pml::pokepara::PokemonParam* pPoke = pData->GetPoke( i );
      bool fastmode_flg = pPoke->StartFastMode();

      switch( pStaticWork->GetProcMode() )
      {
      case MODE_FIELD:      //!< フィールド
        {
          m_pMenu[i][MENU_POKE] = CreateMenu_POKE( param, pPoke, i );
          m_pMenu[i][MENU_ITEM] = CreateMenu_ITEM( param, i );
        } break;

      case MODE_JOIN:       //!< 参加選択
        {
          m_pMenu[i][MENU_JOIN]         = CreateMenu_JOIN( param, i );
          m_pMenu[i][MENU_JOIN_ALREADY] = CreateMenu_JOIN_ALREADY( param, i );
          m_pMenu[i][MENU_JOIN_DISABLE] = CreateMenu_JOIN_DISABLE( param, i );
        } break;

      case MODE_SODATEYA:   //!< 育て屋
        {
          m_pMenu[i][MENU_SODATEYA] = CreateMenu_SODATEYA( param, pPoke, i );
        } break;
      }

      pPoke->EndFastMode( fastmode_flg );
    }
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   破棄
   */
  //------------------------------------------------------------------------------
  bool MenuWindowDraw::DeleteMenu( void )
  {
    bool result = true;

    for( u8 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
    {
      for( u8 j=0; j<MENU_NUM; ++j )
      {
        if( m_pMenu[i][j] )
        {
          //if( m_pMenu[i][j]->IsModuleFree() )
          if( m_pMenu[i][j]->IsCreate() )
          {
            m_pMenu[i][j]->RemoveSubView();
            GFL_SAFE_DELETE( m_pMenu[i][j] );
          }
          else {
            m_pMenu[i][j]->Create();
            result = false;
          }
        }
      }
    }

    return result;
  }



  //------------------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------------------
  void MenuWindowDraw::Update( void )
  {
    MenuID menu_id = GetActMenuID();

    if( menu_id == MENU_ITEM )
    {
      CursorPos cpos = GetCursorPos();
      if( cpos < CPOS_DECIDE )
      {
        app::tool::MenuWindow* pWindow = m_pMenu[cpos][menu_id];
        u32 pos = pWindow->GetCursorPos();

        if( m_PrevPos != pos )
        {
          //m_SubWindow.SetMessage( MSG_TYPE_MSG_ONLY, c_ItemExplainMsgID[pos] );
          m_PrevPos = pos;
        }
      }
    }
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   メニューウィンドウを取得
   * @param   poke_index  メンバーID(0〜5)
   * @param   menu_id     メニューID
   * @return  メニューウィンドウ
   */
  //------------------------------------------------------------------------------
  app::tool::MenuWindow* MenuWindowDraw::GetMenuWindow( u32 poke_index, MenuID menu_id ) const
  {
    if( poke_index >= pml::PokeParty::MAX_MEMBERS )
    {
      GFL_ASSERT(0);
      poke_index = 0;
    }

    if( menu_id >= MENU_NUM )
    {
      GFL_ASSERT(0);
      menu_id = MENU_POKE;
    }

    return m_pMenu[poke_index][menu_id];
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   準備完了か？
   * @return  true == 完了
   */
  //------------------------------------------------------------------------------
  bool MenuWindowDraw::IsReady( void )
  {
    for( u8 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
    {
      for( u8 j=0; j<MENU_NUM; ++j )
      {
        app::tool::MenuWindow* pMenu = m_pMenu[i][j];

        if( pMenu == NULL ) continue;

        // メニューウィンドウの生成チェック
        if( !pMenu->IsCreate() )
        {
          return false;   //!< 準備中！
        }
      }
    }

    return true;
  }



  //------------------------------------------------------------------------------
  /**
   * @brief   ポケモンのメニューを生成する
   * @param   param   生成パラメータ
   * @param   pPoke   対象のポケモン
   * @param   id      メンバーID(0〜5)
   * @return  ポケモンのメニューウィンドウ
   */
  //------------------------------------------------------------------------------
  app::tool::MenuWindow* MenuWindowDraw::CreateMenu_POKE(
    CreateMenuParam param, const pml::pokepara::PokemonParam* pPoke, u32 id )
  {
    const MenuAltID alts_all[] = {  // 全表示
      ALT_POKE_STATUS,  //!< つよさをみる
      ALT_POKE_REPAIR,  //!< かいふくする
      ALT_POKE_ITEM,    //!< もちもの
      ALT_POKE_RETURN,  //!< やめる
    };
    const MenuAltID alts_egg[] = {
      ALT_POKE_STATUS,  //!< つよさをみる
      ALT_POKE_RETURN,  //!< やめる
    };

    const MenuAltID* pAltArray = NULL;
    u8 alt_cnt = 0;

    // タマゴ
    if( pPoke->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )
    {
      pAltArray = alts_egg;
      alt_cnt = GFL_NELEMS( alts_egg );
    }
    else {
      pAltArray = alts_all;
      alt_cnt = GFL_NELEMS( alts_all );
    }

    return CreateMenu_COMMON( param, MENU_POKE, pAltArray, alt_cnt, id );
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   道具メニューを生成する
   * @param   param   生成パラメータ
   * @param   id      メンバーID(0〜5)
   * @return  メニューウィンドウ
   */
  //------------------------------------------------------------------------------
  app::tool::MenuWindow* MenuWindowDraw::CreateMenu_ITEM( CreateMenuParam param, u32 id )
  {
    const MenuAltID alts[] = {
      ALT_ITEM_USE,
      ALT_ITEM_STORE,
      ALT_ITEM_HAND,
      ALT_ITEM_RETURN,
    };

    return CreateMenu_COMMON( param, MENU_ITEM, alts, GFL_NELEMS(alts), id );
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   参加メニュー( 未登録 )を生成する
   * @param   param   生成パラメータ
   * @param   id      メンバーID(0〜5)
   * @return  メニューウィンドウ
   */
  //------------------------------------------------------------------------------
  app::tool::MenuWindow* MenuWindowDraw::CreateMenu_JOIN( CreateMenuParam param, u32 id )
  {
    const MenuAltID alts[] = {
      ALT_JOIN_JOIN,
      ALT_JOIN_STATUS,
      ALT_JOIN_RETURN,
    };

    return CreateMenu_COMMON( param, MENU_JOIN, alts, GFL_NELEMS(alts), id );
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   参加メニュー( 登録済み )を生成する
   * @param   param   生成パラメータ
   * @param   pPoke   対象のポケモン
   * @param   id      メンバーID(0〜5)
   * @return  メニューウィンドウ
   */
  //------------------------------------------------------------------------------
  app::tool::MenuWindow* MenuWindowDraw::CreateMenu_JOIN_ALREADY( CreateMenuParam param, u32 id )
  {
    const MenuAltID alts[] = {
      ALT_JOIN_CANCEL,
      ALT_JOIN_STATUS,
      ALT_JOIN_RETURN,
    };

    return CreateMenu_COMMON( param, MENU_JOIN_ALREADY, alts, GFL_NELEMS(alts), id );
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   参加メニュー( 登録できない )を生成する
   * @param   param   生成パラメータ
   * @param   id      メンバーID(0〜5)
   * @return  メニューウィンドウ
   */
  //------------------------------------------------------------------------------
  app::tool::MenuWindow* MenuWindowDraw::CreateMenu_JOIN_DISABLE( CreateMenuParam param, u32 id )
  {
    const MenuAltID alts[] = {
      ALT_JOIN_STATUS,
      ALT_JOIN_RETURN,
    };

    return CreateMenu_COMMON( param, MENU_JOIN_DISABLE, alts, GFL_NELEMS(alts), id );
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   育て屋メニューを生成する
   * @param   param   生成パラメータ
   * @param   pPoke   対象のポケモン
   * @param   id      メンバーID(0〜5)
   * @return  メニューウィンドウ
   */
  //------------------------------------------------------------------------------
  app::tool::MenuWindow* MenuWindowDraw::CreateMenu_SODATEYA(
    CreateMenuParam param, const pml::pokepara::PokemonParam* pPoke, u32 id )
  {
    // 預けられる
    const MenuAltID alts_enable[] = {
      ALT_SODATEYA_DECIDE,    //!< あずける
      ALT_SODATEYA_STATUS,    //!< つよさをみる
      ALT_SODATEYA_RETURN,    //!< やめる
    };

    // 預けられない
    const MenuAltID alts_disable[] = {
      ALT_SODATEYA_STATUS,    //!< つよさをみる
      ALT_SODATEYA_RETURN,    //!< やめる
    };

    // フィールド技の有無で選択肢の構成を変更する
    const MenuAltID* pAltArray = NULL;
    u8 alt_cnt = 0;

    if( IsPokemonSodateyaEnable( pPoke ) )
    {
      pAltArray = alts_enable;
      alt_cnt = GFL_NELEMS( alts_enable );
    }
    else {
      pAltArray = alts_disable;
      alt_cnt = GFL_NELEMS( alts_disable );
    }

    return CreateMenu_COMMON( param, MENU_SODATEYA, pAltArray, alt_cnt, id );
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   二択メニューを生成する
   * @param   param   生成パラメータ
   * @param   id      メンバーID(0〜5)
   * @return  メニューウィンドウ
   */
  //------------------------------------------------------------------------------
  app::tool::MenuWindow* MenuWindowDraw::CreateMenu_YESNO( CreateMenuParam param, u32 id )
  {
    const MenuAltID alts[] = {
      ALT_YESNO_YES,
      ALT_YESNO_NO,
    };

    return CreateMenu_COMMON( param, MENU_YESNO, alts, GFL_NELEMS(alts), id );
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   プロモ選択メニューを生成する
   * @param   param   生成パラメータ
   * @param   pPoke   対象のポケモン
   * @param   id      メンバーID(0〜5)
   * @return  メニューウィンドウ
   */
  //------------------------------------------------------------------------------
  app::tool::MenuWindow* MenuWindowDraw::CreateMenu_PROMOTION(
    CreateMenuParam param, const pml::pokepara::PokemonParam* pPoke, u32 id )
  {
  #if 0 // プロモ選択メニューは必要か？
    const MenuAltID alts_enable[] = {
      ALT_PROMOTION_DECIDE,
      ALT_PROMOTION_STATUS,
      ALT_PROMOTION_RETURN,
    };
    const MenuAltID alts_disable[] = {
      ALT_PROMOTION_STATUS,
      ALT_PROMOTION_RETURN,
    };

    app::tool::MenuWindow* pMenu = GFL_NEW( pHeap ) app::tool::MenuWindow( pHeap, MENU_PARAM[ MENU_PROMOTION ] );

    if( this->IsPokemonSodateyaEnable( pPokeParam ) )
    {
      this->AddAltsToMenu( pHeap, alts_enable, GFL_NELEMS(alts_enable), pMenu );
    }
    else {
      this->AddAltsToMenu( pHeap, alts_disable, GFL_NELEMS(alts_disable), pMenu );
    }

    return pMenu;
  #endif
    return NULL;
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   ウィンドウの生成(共通処理)
   * @param   param     生成パラメータ
   * @param   menu_id   メニューID
   * @param   pAlts     メニュー項目IDのリスト
   * @param   alt_cnt   メニュー項目数
   * @param   id        メンバーID(0〜5)
   * @return  メニューウィンドウ
   */
  //------------------------------------------------------------------------------
  app::tool::MenuWindow* MenuWindowDraw::CreateMenu_COMMON(
    CreateMenuParam param, MenuID menu_id, const MenuAltID* pAlts, u32 alt_cnt, u32 id )
  {
    app::ui::UIView* pUIView = m_pBase->GetUIView();
    app::util::Heap* pHeap = m_pBase->GetHeap();

    // メニューの生成
    app::tool::MenuWindow* pMenu = GFL_NEW(pHeap->GetDeviceHeap())
      app::tool::MenuWindow( pHeap, param.pRenderingManager, param.pCursorBuf );

    // リスナーのセット
    pMenu->SetListener( param.pListner );

    // 項目の追加
    for( u8 i=0; i<alt_cnt; ++i )
    {
      MenuAltID alt_id = pAlts[i];
      u32 mes_id = ALT_DESC[alt_id].strID;
      bool link = ALT_DESC[alt_id].BButtonLink;

      pMenu->AddItem( GetMessageID(param, mes_id), link, link );

      m_alts[id][menu_id][i] = alt_id;
    }

    // ウィンドウの登録
    pUIView->AddSubView( pMenu );

    return pMenu;
  }



  //------------------------------------------------------------------------------
  /**
   * @brief   メニューウィンドウを開く
   */
  //------------------------------------------------------------------------------
  void MenuWindowDraw::OpenWindow( void )
  {
    StaticWork* pStaticWork = m_pBase->GetStaticWork();

    switch( pStaticWork->GetProcMode() )
    {
    case MODE_FIELD:  //!< フィールド
      {
        OpenWindow( MENU_POKE );
      } break;

    case MODE_JOIN:   //!< 参加選択
      {
        CursorPos cpos = pStaticWork->GetCursorPos( CID_POKE );
        //u8 poke_index = m_pStaticWork->GetJoinPokeIndex( cpos );

        if( !pStaticWork->IsJoinEnable( cpos ) )  //!< 参加できない
        {
          OpenWindow( MENU_JOIN_DISABLE );
        }
        else if( pStaticWork->IsJoinPoke( cpos ) )
        {
          OpenWindow( MENU_JOIN_ALREADY );  //!< 参加済み
        }
        else {
          OpenWindow( MENU_JOIN );  //!< 未参加
        }
      } break;

    case MODE_SODATEYA: //!< 育て屋
      {
        OpenWindow( MENU_SODATEYA );
      } break;
    }
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   メニューウィンドウを開く
   * @param   menuID        メニューID
   * @param   fade_enable   差分ウィンドウのフェード許可フラグ
   */
  //------------------------------------------------------------------------------
  void MenuWindowDraw::OpenWindow( MenuID menuID, bool fade_enable )
  {
    StaticWork* pStaticWork = m_pBase->GetStaticWork();

    CursorPos cpos = pStaticWork->GetCursorPos( CID_POKE );
    u8 pokeIndex = pStaticWork->GetPokeIndex( cpos );

    if( cpos <= CPOS_PLATE_BR )
    {
      StartMenu( menuID, cpos, pokeIndex );
    }
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   メニューウィンドウを閉じる
   * @param   sub_close   サブウィンドウを閉じるフラグ(デフォルトtrue)
   * @param   item_close  アイテム欄を閉じるフラグ(デフォルトfalse)
   */
  //------------------------------------------------------------------------------
  void MenuWindowDraw::CloseWindow( bool sub_close, bool item_close )
  {
    // メニューウィンドウを閉じる
    EndMenu();
  }


  //------------------------------------------------------------------------------
  /**
   * @brief   メニューの開始
   * @param   menuID    メニューID
   * @param   cpos      カーソル位置(開くポケモンの位置)
   * @param   pokeIndex カーソル位置のポケモンのインデックス
   */
  //------------------------------------------------------------------------------
  bool MenuWindowDraw::StartMenu( MenuID menuID, CursorPos cpos, u8 pokeIndex )
  {
    app::tool::MenuWindow* pWindow = m_pMenu[pokeIndex][menuID];

    if( pWindow == NULL ) return false;

    m_PrevPos = 0;
    bool ret = pWindow->StartMenu( m_PrevPos, app::tool::MenuWindow::PUT_RD, 165, -115 );

    // 成功した時には開始したメニューの情報を保持する
    // EndMenuの解放で使用
    if( ret )
    {
      m_ActMenuID = menuID;
      m_ActMenuCPos = cpos;
      m_ActPokeIndex = pokeIndex;
    }

    return ret;
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   メニューの終了
   */
  //------------------------------------------------------------------------------
  void MenuWindowDraw::EndMenu( void )
  {
    if( m_ActMenuID != MENU_NULL && m_ActPokeIndex < PLATE_NUM )
    {
      app::tool::MenuWindow* pWindow = m_pMenu[m_ActPokeIndex][m_ActMenuID];

      pWindow->EndMenu( false );
    }
  }



  //------------------------------------------------------------------------------
  /**
   * @brief   アクティブなメニューウィンドウのID
   * @return  MENU_NULL == メニューを開いていない
   */
  //------------------------------------------------------------------------------
  MenuID MenuWindowDraw::GetActMenuID( void )
  {
    return m_ActMenuID;
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   アクティブなメニューのメンバーID
   * @return  CPOS_NULL == メニューを開いていない
   */
  //------------------------------------------------------------------------------
  CursorPos MenuWindowDraw::GetCursorPos( void )
  {
    return m_ActMenuCPos;
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   アクティブなメニューのポケモンのインデックス
   * @return  6以上はメニューを開いていない
   */
  //------------------------------------------------------------------------------
  u8 MenuWindowDraw::GetPokeIndex( void )
  {
    return m_ActPokeIndex;
  }

  //------------------------------------------------------------------------------
  /**
    * @brief   選択したメニュー項目IDを取得
    * @param   select_id 選択した項目
    */
  //------------------------------------------------------------------------------
  MenuAltID MenuWindowDraw::GetSelectAltID( u32 select_id )
  {
    GFL_ASSERT( select_id < 8 );

    return m_alts[m_ActPokeIndex][m_ActMenuID][select_id];
  }



  //------------------------------------------------------------------------------
  /**
   * @brief   ポケモンが育て屋に預けられるかどうかを調べる
   * @param   pPokeParam    チェック対象のポケモン
   * @return  true    預けられる
   * @return  false   預けられない
   */
  //------------------------------------------------------------------------------
  bool MenuWindowDraw::IsPokemonSodateyaEnable( const pml::pokepara::PokemonParam* pPokeParam ) const
  {
    return !pPokeParam->IsEgg( pml::pokepara::CHECK_BOTH_EGG );
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   ポケモンがプロモに出られるかどうかを調べる
   * @param   pPokeParam    チェック対象のポケモン
   * @return  true    出られる
   * @return  false   出られない
   */
  //------------------------------------------------------------------------------
  bool MenuWindowDraw::IsPokemonPromotioneEnable( const pml::pokepara::PokemonParam* pPokeParam ) const
  {
    return !pPokeParam->IsEgg( pml::pokepara::CHECK_BOTH_EGG );
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   メッセージの取得
   * @param   param       生成パラメータ
   * @param   message_id  メッセージID
   * @return  gfl2::str::StrBuf
   */
  //------------------------------------------------------------------------------
  gfl2::str::StrBuf& MenuWindowDraw::GetMessageID( CreateMenuParam param, u32 message_id )
  {
    app::util::G2DUtil* pG2D = m_pBase->GetG2D();
    gfl2::str::StrBuf* pStrBuf = m_pBase->GetStrBuf(0);

    pG2D->GetStringExpand( pStrBuf, message_id );

    return *pStrBuf;
  }

  //------------------------------------------------------------------------------
  /**
   * @brief   メッセージの取得
   * @param   param     生成パラメータ
   * @param   waza_id   技ID
   * @return  gfl2::str::StrBuf
   */
  //------------------------------------------------------------------------------
  gfl2::str::StrBuf& MenuWindowDraw::GetWazaMessage( CreateMenuParam param, u32 waza_id )
  {
    print::WordSet* pWordSet = m_pBase->GetWordSet();
    gfl2::str::MsgData* pMsgData = m_pBase->GetMsgData();
    gfl2::str::StrBuf* pStrBuf = m_pBase->GetStrBuf(0);
    gfl2::str::StrBuf* pTmpStrBuf = m_pBase->GetStrBuf(1);

    // 技名
    pWordSet->RegisterWazaName( 0, waza_id );
    pMsgData->GetString( mes_pokelist_waza_name, *pTmpStrBuf );
    pWordSet->Expand( pStrBuf, pTmpStrBuf );

    return *pStrBuf;
  }


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )
