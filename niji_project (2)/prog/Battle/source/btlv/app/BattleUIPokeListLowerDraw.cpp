//=============================================================================
/**
 * @file BattleUIPokeListLowerDraw.h
 * @brief バトル下画面 >> 「ポケモンリスト」画面
 * @author yuto_uchida
 * @date 2015.04.09
 */
//=============================================================================
#include "BattleUIPokeListLowerDraw.h"
#include "BattleUIViewDrawOrder.h"

#include "niji_conv_header/app/battle_pokelist/poke_list_low.h"
#include "niji_conv_header/app/battle_pokelist/poke_list_low_pane.h"
#include "niji_conv_header/app/battle_pokelist/poke_list_low_anim_list.h"

#include "niji_conv_header/message/msg_btl_app.h"

#include "Battle/source/btlv/app/Parts/BattleUIKoukaIconPartsLayout.h"
#include "Battle/source/btlv/app/Parts/BattleUISkillButtonPartsLayout.h"

#include "AppLib/include/Tool/AppToolGaugePartsLayout.h"
#include "AppLib/include/Tool/AppCommonGrpIconData.h"

#include "AppLib/include/Util/app_util_Common.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

//
//  ここからは BattleUIPokeListSubViewLowerDrawBase クラスの定義
//
  //  サブViewの基本クラス
  class  BattleUIPokeListLowerSubViewBase : public ::app::ui::UIView
  {
  public:
    //  コンストラクタ
    BattleUIPokeListLowerSubViewBase( BattleUIPokeListLowerDraw* pParentView, const s32 useLytWkId, ::app::tool::MenuCursor* pBtnMenuCursor )
      : UIView( pParentView->GetInitParam().pUtilHeap, pParentView->GetInitParam().pUtilHeap->GetDeviceAllocator(), pParentView->GetInitParam().pUtilHeap->GetDeviceAllocator() )
    {
      GFL_ASSERT( pBtnMenuCursor != NULL );
      GFL_ASSERT( pParentView != NULL );

      _Clear();

      UIView::SetVisible( false );

      m_pParentView = pParentView;
      m_useLytWkId  = useLytWkId;

      //  メニューカーソル
      m_pMenuCursor = pBtnMenuCursor;
      m_pMenuCursor->SetVisible( false );
    }

    //  デストラクタ
    virtual ~BattleUIPokeListLowerSubViewBase()
    {
      m_pMenuCursor = NULL;

      UIView::ReleaseButtonManager();
      UIView::Delete2D();
    }

    //------------------------------------------------------------------
    /**
      * @brief   初期化
      */
    //------------------------------------------------------------------
    virtual void Initialize() = 0;

    //------------------------------------------------------------------
    /**
      * @brief   開始
      */
    //------------------------------------------------------------------
    virtual void Start( BattleUIPokeListLowerButtonCtrl* pButtonCtrl )
    {
      GFL_ASSERT( pButtonCtrl );
      m_pButtonCtrl = pButtonCtrl;

      UIView::SetVisible( true );
    }

    //------------------------------------------------------------------
    /**
      * @brief   終了
      */
    //------------------------------------------------------------------
    virtual void End( void ) { UIView::SetVisible( false ); }

    //------------------------------------------------------------------
    /**
      * @brief   更新処理
      */
    //------------------------------------------------------------------
    virtual void Update( void )
    {
      m_pMenuCursor->Update();

      UIView::Update();
    }

    //------------------------------------------------------------------
    /**
      * @brief   描画
      */
    //------------------------------------------------------------------
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo )
    {
      UIView* pParentView = m_pParentView;
      GFL_ASSERT( pParentView );

      //  親のビューが入力できないのなら子はカーソル描画をしない
      if( pParentView->IsInputEnabled() )
      {
        BattleUIPokeListLowerDraw::INIT_PARAM&  rInitParam  = m_pParentView->GetInitParam();

        if( UIView::IsInputEnabled() == true )
        {
          m_pMenuCursor->Draw( rInitParam.pRenderingManager, displayNo, gfl2::lyt::DISPLAY_LOWER );
        }
      }
    }

    //  メニューカーソルポインタ取得
    ::app::tool::MenuCursor* GetMenuCursorPtr() const { return m_pMenuCursor; }

    //------------------------------------------------------------------
    /**
      * @brief   ポケモン情報を設定
      */
    //------------------------------------------------------------------
    virtual void SetPokeInfoData( const BattleUIPokeListLowerDraw::PokeObject* pPokeObject ) = 0;

  protected:
    BattleUIPokeListLowerDraw*  m_pParentView;
    s32                         m_useLytWkId;
    //  入力制御
    BattleUIPokeListLowerButtonCtrl*        m_pButtonCtrl;

  private:
    void _Clear()
    {
      m_pParentView = NULL;
      m_pMenuCursor = NULL;
      m_pButtonCtrl = NULL;
      m_useLytWkId  = -1;
    }

    ::app::tool::MenuCursor*          m_pMenuCursor;
  };

//
//  ここから先は BattleUIPokeListLowerSimpleStatusSubView クラスの定義
//
  //  簡易ステータス表示状態のView
  //  簡易ステータスと選択ボタンを表示する
  class  BattleUIPokeListLowerSimpleStatusSubView : public BattleUIPokeListLowerSubViewBase
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleUIPokeListLowerSimpleStatusSubView );  // コピーを禁止

  public:
    // ステート
    enum StateType
    {
      STATE_TYPE_NONE         = 0,
      STATE_TYPE_UPDATE,
    };

    //------------------------------------------------------------------
    /**
      * @brief   コンストラクタ
      */
    //------------------------------------------------------------------
    BattleUIPokeListLowerSimpleStatusSubView( BattleUIPokeListLowerDraw* pParentView, const s32 useLytWkId, ::app::tool::MenuCursor* pBtnMenuCursor )
      : BattleUIPokeListLowerSubViewBase( pParentView, useLytWkId, pBtnMenuCursor )
    {
      _Clear();
    }

    //------------------------------------------------------------------
    /**
      * @brief   デストラクタ
      */
    //------------------------------------------------------------------
    virtual ~BattleUIPokeListLowerSimpleStatusSubView()
    {
      {
        u32 loopNum = GFL_NELEMS( m_pSkillItemList );
        for( u32 i = 0; i < loopNum; ++i )
        {
          GFL_SAFE_DELETE( m_pSkillItemList[ i ] );
        }
      }

      GFL_SAFE_DELETE( m_pHpGaugePartsLayout );
      GFL_SAFE_DELETE( m_pExpGaugePartsLayout );
    }

    //------------------------------------------------------------------
    /**
      * @brief   初期化
      */
    //------------------------------------------------------------------
    virtual void Initialize()
    {
      GFL_ASSERT( m_pParentView );

      ::app::util::G2DUtil* pG2DUtil  = m_pParentView->GetG2DUtil();
      m_pLayoutWork   = pG2DUtil->GetLayoutWork( m_useLytWkId );

      const BattleUIPokeListLowerDraw::INIT_PARAM&  rParentViewInitParam  = m_pParentView->GetInitParam();

      const gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

      //  スキルオブジェクト項目一覧を作成
      {
        SkillItemObject::INIT_PARAM                     skillItemObjectInitParam;
        parts::BattleUIKoukaIconPartsLayout::INIT_PARAM koukaIconPartsLayoutInitParam;

        u32 loopNum = GFL_NELEMS( m_pSkillItemList );
        for( u32 i = 0; i < loopNum; ++i )
        {
          //  必要データを出力
          _OutputSkillItemObjectInitParam( &skillItemObjectInitParam, &koukaIconPartsLayoutInitParam, i );
          m_pSkillItemList[ i ] = GFL_NEW( rParentViewInitParam.pUtilHeap->GetDeviceHeap() ) SkillItemObject( skillItemObjectInitParam, koukaIconPartsLayoutInitParam, rParentViewInitParam.pUtilHeap->GetDeviceHeap() );
        }
      }

      //  ＨＰゲージ作成
      {
        gfl2::lyt::LytParts*  pPartsPaneHpGauge = m_pLayoutWork->GetPartsPane( PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_PARTS_HP );
        GFL_ASSERT( pPartsPaneHpGauge );

        ::app::tool::AppToolHPGaugePartsLayout::INIT_PARAM  initParam =
        {
          pG2DUtil,
          m_useLytWkId,

          //  数字の表示用のメッセージID
          STR_VALUE_3,
          //  数字の桁数
          3,

          m_pLayoutWork->GetTextBoxPane( pPartsPaneHpGauge, PANENAME_COMMON_GAG_000_PANE_TEXTBOX_00, &rMultiResId ),
          m_pLayoutWork->GetTextBoxPane( pPartsPaneHpGauge, PANENAME_COMMON_GAG_000_PANE_TEXTBOX_01, &rMultiResId ),
          m_pLayoutWork->GetTextBoxPane( pPartsPaneHpGauge, PANENAME_COMMON_GAG_000_PANE_TEXTBOX_02, &rMultiResId ),

          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTS_HP_GAUGE_HP,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTS_HP_COLOR_NORMAL,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTS_HP_COLOR_HALF,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTS_HP_COLOR_FEW,
        };

        m_pHpGaugePartsLayout = GFL_NEW( rParentViewInitParam.pUtilHeap->GetDeviceHeap() ) ::app::tool::AppToolHPGaugePartsLayout( initParam, rParentViewInitParam.pUtilHeap->GetDeviceHeap() );
      }

      //  EXPゲージ作成
      {
        ::app::tool::AppToolGaugePartsLayout::INIT_PARAM  initParam = 
        {
          pG2DUtil,
          m_useLytWkId,
          m_pLayoutWork->GetPartsPane( PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_PARTS_EXP ),
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTS_EXP_GAUGE_00,
        };

        m_pExpGaugePartsLayout  = GFL_NEW( rParentViewInitParam.pUtilHeap->GetDeviceHeap() ) ::app::tool::AppToolGaugePartsLayout( initParam, rParentViewInitParam.pUtilHeap->GetDeviceHeap() );
      }

      //  道具名ペイン取得
      {
        m_pHaveItemTextBoxPane  = m_pLayoutWork->GetTextBoxPane( PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TEXTSKILL_04 );
        GFL_ASSERT( m_pHaveItemTextBoxPane );
      }
    }

    //------------------------------------------------------------------
    /**
      * @brief   開始
      */
    //------------------------------------------------------------------
    virtual void Start( BattleUIPokeListLowerButtonCtrl* pButtonCtrl )
    {
      BattleUIPokeListLowerSubViewBase::Start( pButtonCtrl );
      _SetState( STATE_TYPE_UPDATE );
    }

    //------------------------------------------------------------------
    /**
      * @brief   終了
      */
    //------------------------------------------------------------------
    virtual void End( void )
    {
      BattleUIPokeListLowerSubViewBase::End();
    }

    //------------------------------------------------------------------
    /**
      * @brief   更新処理
      */
    //------------------------------------------------------------------
    virtual void Update( void )
    {
      m_pExpGaugePartsLayout->Update();
      m_pHpGaugePartsLayout->Update();

      BattleUIPokeListLowerSubViewBase::Update();
    }

    //------------------------------------------------------------------
    /**
      * @brief   ポケモン情報を設定
      */
    //------------------------------------------------------------------
    virtual void SetPokeInfoData( const BattleUIPokeListLowerDraw::PokeObject* pPokeObject )
    {
      GFL_ASSERT( pPokeObject );

      const BattleUIPokeListLowerDraw::PokeObject::INIT_PARAM&  rPokeObjectParam  = pPokeObject->GetInitParam();
      const BTL_POKEPARAM*  pPokeParam  = rPokeObjectParam.pBtlParam;
      GFL_ASSERT( pPokeParam );

      const pml::pokepara::PokemonParam*  pOrgPokeParam = pPokeParam->GetSrcData();
      ::app::util::G2DUtil* pG2DUtil  = m_pParentView->GetG2DUtil();

      //  卵であれば名前以外はすべて非表示
      if( pOrgPokeParam->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )
      {
        // @fix GFNMCat[1054] 初回ポケモンカーソルがタマゴの場合にはペインの非表示のアニメ再生を止める
        pG2DUtil->StopAnime( m_useLytWkId, LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000_PAGE_00 );
        pG2DUtil->SetPaneVisible( m_useLytWkId, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_NULL_00, false );

        //  入力関連はすべてOFF
        m_pButtonCtrl->SetEnableButtonAll( false );
        return;
      }

      pG2DUtil->SetPaneVisible( m_useLytWkId, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_NULL_00, true );
      //  入力をON
      m_pButtonCtrl->SetEnableButtonAll( true );

      //  HPゲージ
      {
        GFL_ASSERT( m_pHpGaugePartsLayout );

        m_pHpGaugePartsLayout->SetNumber( pPokeParam->GetValue( BTL_POKEPARAM::BPP_HP ), pPokeParam->GetValue( BTL_POKEPARAM::BPP_MAX_HP ) );
      }

      //  EXPゲージ
      {
        GFL_ASSERT( m_pExpGaugePartsLayout );

        //  次のLVのExpを取得
        const u32 nextLvExp = ::app::util::GetPokeNextLvExpNum( pPokeParam->GetSrcData() );
        const u32 nowLvExp  = ::app::util::GetPokeNowLvExpNum( pPokeParam->GetSrcData() );
        m_pExpGaugePartsLayout->SetNumber( nowLvExp, nextLvExp );
      }

      //  所持アイテム
      {
        u16 itemNo  = pPokeParam->GetItem();
        if( itemNo == ITEM_DUMMY_DATA )
        {
          pG2DUtil->SetPaneVisible( m_pHaveItemTextBoxPane, false );
        }
        else
        {
          //  アイテム名設定
          pG2DUtil->SetRegisterItemName( 0, itemNo );
          pG2DUtil->SetTextBoxPaneStringExpand( m_pHaveItemTextBoxPane, STR_ITEM_NAME );

          pG2DUtil->SetPaneVisible( m_pHaveItemTextBoxPane, true );
        }
      }

      //  わざ一覧を表示(技はバトルではなくオリジナルのデータを参照、変身状態だと変身した相手の技が取得してしまうのを避けるため)
      {
        //  一旦すべての項目を非表示
        {
          u32 loopNum = GFL_NELEMS( m_pSkillItemList );
          for( u32 i = 0; i < loopNum; ++i )
          {
            m_pSkillItemList[ i ]->SetVisible( false );
          }
        }

        //  オリジナルの技一覧から項目設定
        WazaNo  wazaNo          = WAZANO_NULL;
        pml::PokeType wazaType  = 0;

        const BattleUIPokeListLowerDraw::INIT_PARAM&  rParentViewInitParam  = m_pParentView->GetInitParam();

        //  すでにポケモンオブジェクトにオリジナル技一覧が作成されているのでデータを参照して表示
        GFL_ASSERT( rPokeObjectParam.wazaCount <= GFL_NELEMS( m_pSkillItemList ) );
        for( u8 i = 0; i < rPokeObjectParam.wazaCount; ++i )
        {
          //  わざがあるので表示
          m_pSkillItemList[ i ]->SetVisible( true );

          wazaNo  = rPokeObjectParam.aWazaParam[ i ].id;
          wazaType  = pml::wazadata::GetType( wazaNo );

          //  技名指定
          // @fix NMCat[496] 技情報はバトル用のポケパラメータでないと反映できない
          m_pSkillItemList[ i ]->SetName( rPokeObjectParam.aWazaParam[ i ].pWazaNameBuffer, rPokeObjectParam.aWazaParam[ i ].nowPP, rPokeObjectParam.aWazaParam[ i ].maxPP );

          //  技タイプ指定
          m_pSkillItemList[ i ]->SetType( wazaType );

          //  技の効果指定
          if( rParentViewInitParam.pDelegate != NULL )
          {
            bool bPutFlg  = false;
            BtlTypeAffAbout type  = rParentViewInitParam.pDelegate->OnGetKoukaType( &bPutFlg, wazaNo, pPokeParam );
            m_pSkillItemList[ i ]->SetKoukaType( bPutFlg, type );
          }
        }
      }
    }

  private:
    //  技選択オブジェクト
    class SkillItemObject
    {
    public:
      //  初期化パラメータ
      typedef struct
      {
        ::app::util::G2DUtil*               p_g2d_util;
        ::app::tool::AppCommonGrpIconData*  p_common_grp_icon_data;
        gfl2::lyt::LytTextBox*              p_waza_name_text_box_pane;
        gfl2::lyt::LytPicture*              p_waza_type_icon_pic_pane;

      } INIT_PARAM;

      SkillItemObject( const INIT_PARAM& rInitParam, const parts::BattleUIKoukaIconPartsLayout::INIT_PARAM& rKoukaPartsInitParam, gfl2::heap::HeapBase* pHeap )
      {
        m_initParam = rInitParam;

        GFL_ASSERT( pHeap );
        GFL_ASSERT( m_initParam.p_g2d_util );
        GFL_ASSERT( m_initParam.p_waza_name_text_box_pane );
        GFL_ASSERT( m_initParam.p_waza_type_icon_pic_pane );

        m_pKoukaIconParts = GFL_NEW_LOW( pHeap ) parts::BattleUIKoukaIconPartsLayout( rKoukaPartsInitParam );
      }

      ~SkillItemObject()
      {
        GFL_SAFE_DELETE( m_pKoukaIconParts );
      }

      //  技タイプ設定
      void SetType( pml::PokeType type )
      {
        //  テクスチャ張替え
        GFL_ASSERT( m_initParam.p_common_grp_icon_data != NULL );
        m_initParam.p_common_grp_icon_data->ReplacePaneTextureByWazaTypeIcon( (PokeType_tag)type, m_initParam.p_waza_type_icon_pic_pane );

      }

      //  技名設定
      void SetName( gfl2::str::StrBuf* pWazaName, const u32 nowPPNum, const u32 maxPPNum )
      {
        GFL_ASSERT( pWazaName );

        m_initParam.p_g2d_util->SetTextBoxPaneString( m_initParam.p_waza_name_text_box_pane, pWazaName );

        //  テキスト色指定
        {
          //  デフォルトはテキスト黒色に
          gfl2::lyt::ColorU8  color = ::app::util::GetWazaTextColor( nowPPNum, maxPPNum, gfl2::lyt::ColorU8( 0, 0, 0, 255 ) );
          m_initParam.p_g2d_util->SetTextBoxPaneColor( m_initParam.p_waza_name_text_box_pane, color, color );
        }
      }

      //  効果タイプ設定
      void SetKoukaType( const bool bPutFlg, const BtlTypeAffAbout type )
      {
        m_pKoukaIconParts->SetVisible( bPutFlg );
        m_pKoukaIconParts->SetType( type );
      }

      //  表示設定
      void SetVisible( const bool bFlg )
      {
        //  このペインの下階層に効果、タイプアイコンのペインがあるので一緒に表示設定できる
        m_initParam.p_g2d_util->SetPaneVisible( m_initParam.p_waza_name_text_box_pane, bFlg );
      }

    private:
      parts::BattleUIKoukaIconPartsLayout*  m_pKoukaIconParts;
      INIT_PARAM                            m_initParam;
    };

    enum
    {
      SKILL_LIST_MAX  = 4,
    };

    //  ステート設定
    void _SetState( const StateType newStateType )
    {
      ::app::util::G2DUtil* pG2DUtil  = m_pParentView->GetG2DUtil();

      m_state = newStateType;

      if( newStateType == STATE_TYPE_UPDATE )
      {
        pG2DUtil->StartAnime( m_useLytWkId, LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000_PAGE_00 );
        pG2DUtil->SetPaneVisible( m_useLytWkId, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_NULL_00, true );
        pG2DUtil->SetPaneVisible( m_useLytWkId, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_NULL_01, false );

        // 強制アニメをして画面反映
#if defined( GF_PLATFORM_CTR )
        m_pLayoutWork->GetLayoutCore()->Animate();
#elif defined( GF_PLATFORM_WIN32 )
        m_pLayoutWork->GetLayoutCore()->AnimateAndUpdateAnimFrame();
#endif
      }
    }

    //  SkillItemObjectクラスに必要な初期化パラメータを出力する
    void _OutputSkillItemObjectInitParam( SkillItemObject::INIT_PARAM* pOutputSkillItemObjectInitParam, parts::BattleUIKoukaIconPartsLayout::INIT_PARAM* pOutKoukaIconPartsLayoutInitParam, const u32 itemIdx )
    {
      GFL_ASSERT( pOutputSkillItemObjectInitParam );
      GFL_ASSERT( pOutKoukaIconPartsLayoutInitParam );
      GFL_ASSERT( itemIdx < SKILL_LIST_MAX );

      ::app::util::G2DUtil* pG2DUtil              = m_pParentView->GetG2DUtil();
      gfl2::lyt::LytWk* pLayoutWork               = pG2DUtil->GetLayoutWork( m_useLytWkId );
      const gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

      //  SkillItemObject用の初期化パラメータ作成
      {
        typedef struct
        {
          u32 waza_text_pane_id;
          u32 waza_type_icon_parts_pane_id;
        } SETUP_PANE_DATA;

        const SETUP_PANE_DATA aSetupPaneData[ SKILL_LIST_MAX ] =
        {
          { PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TEXTSKILL_00, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TYPE_00 },
          { PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TEXTSKILL_01, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TYPE_01 },
          { PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TEXTSKILL_02, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TYPE_02 },
          { PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TEXTSKILL_03, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TYPE_03 }
        };

        pOutputSkillItemObjectInitParam->p_g2d_util                 = pG2DUtil;
        pOutputSkillItemObjectInitParam->p_waza_name_text_box_pane  = pLayoutWork->GetTextBoxPane( aSetupPaneData[ itemIdx ].waza_text_pane_id );
        pOutputSkillItemObjectInitParam->p_common_grp_icon_data     = m_pParentView->GetInitParam().pAppCommonGrpIconData;

        {
          gfl2::lyt::LytParts*  pLytParts = pLayoutWork->GetPartsPane( aSetupPaneData[ itemIdx ].waza_type_icon_parts_pane_id );
          GFL_ASSERT( pLytParts );

          pOutputSkillItemObjectInitParam->p_waza_type_icon_pic_pane  = pLayoutWork->GetPicturePane( pLytParts, PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE, &rMultiResId );
          GFL_ASSERT( pOutputSkillItemObjectInitParam->p_waza_type_icon_pic_pane );
        }
      }

      //  効果アイコンパーツレイアウトの初期化パラメータ作成
      {
        const s32 aKoukaIconPartsPaneId[ SKILL_LIST_MAX ] =
        {
          PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_KOUKA_00,
          PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_KOUKA_01,
          PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_KOUKA_02,
          PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_KOUKA_03,
        };

        //  各効果アイコンのアニメidx一覧
        const s32 aKoukaIconAnimeIdTable[ SKILL_LIST_MAX ][ 6 ]  =
        {
          {
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_00_ACTIVE,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_00_PASSIVE,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_00_KOUKA_00,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_00_KOUKA_01,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_00_KOUKA_02,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_00_KOUKA_03,
          },

          {
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_01_ACTIVE,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_01_PASSIVE,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_01_KOUKA_00,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_01_KOUKA_01,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_01_KOUKA_02,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_01_KOUKA_03,
          },

          {
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_02_ACTIVE,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_02_PASSIVE,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_02_KOUKA_00,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_02_KOUKA_01,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_02_KOUKA_02,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_02_KOUKA_03,
          },

          {
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_03_ACTIVE,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_03_PASSIVE,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_03_KOUKA_00,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_03_KOUKA_01,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_03_KOUKA_02,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__KOUKA_03_KOUKA_03
          },
        };

        pOutKoukaIconPartsLayoutInitParam->p_g2d_util                 = m_pParentView->GetG2DUtil();
        pOutKoukaIconPartsLayoutInitParam->p_parts_pane               = pLayoutWork->GetPartsPane( aKoukaIconPartsPaneId[ itemIdx ] );
        pOutKoukaIconPartsLayoutInitParam->layout_id                  = m_useLytWkId;
        pOutKoukaIconPartsLayoutInitParam->active_anime_idx           = aKoukaIconAnimeIdTable[ itemIdx ][ 0 ];
        pOutKoukaIconPartsLayoutInitParam->passive_anime_idx          = aKoukaIconAnimeIdTable[ itemIdx ][ 1 ];
        pOutKoukaIconPartsLayoutInitParam->kouka_batsugun_anime_idx   = aKoukaIconAnimeIdTable[ itemIdx ][ 2 ];
        pOutKoukaIconPartsLayoutInitParam->kouka_imahitotsu_anime_idx = aKoukaIconAnimeIdTable[ itemIdx ][ 3 ];
        pOutKoukaIconPartsLayoutInitParam->kouka_none_anime_idx       = aKoukaIconAnimeIdTable[ itemIdx ][ 4 ];
        pOutKoukaIconPartsLayoutInitParam->kouka_normal_anime_idx     = aKoukaIconAnimeIdTable[ itemIdx ][ 5 ];
      }
    }

    //  変数の初期化
    void _Clear()
    {
      m_pHpGaugePartsLayout   = NULL;
      m_pExpGaugePartsLayout  = NULL;
      m_pLayoutWork           = NULL;
      m_state                 = STATE_TYPE_NONE;
      m_pHaveItemTextBoxPane  = NULL;

      gfl2::std::MemFill( m_pSkillItemList, NULL, sizeof( m_pSkillItemList ) );
    }

    StateType                               m_state;

    gfl2::lyt::LytWk*                       m_pLayoutWork;

    ::app::tool::AppToolHPGaugePartsLayout* m_pHpGaugePartsLayout;
    ::app::tool::AppToolGaugePartsLayout*   m_pExpGaugePartsLayout;

    gfl2::lyt::LytTextBox*                  m_pHaveItemTextBoxPane;

    SkillItemObject*                        m_pSkillItemList[ SKILL_LIST_MAX ];
  };

//
//  ここから先は BattleUIPokeListLowerDetailStatusSubView クラスの定義
//
  //  詳細ステータスView
  //  技一覧、所持特性、所持アイテムの表示
  class  BattleUIPokeListLowerDetailStatusSubView : public BattleUIPokeListLowerSubViewBase
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleUIPokeListLowerDetailStatusSubView );  // コピーを禁止

  public:
    // ステート
    enum StateType
    {
      STATE_TYPE_NONE         = 0,
      STATE_TYPE_UPDATE,
    };

    //------------------------------------------------------------------
    /**
      * @brief   コンストラクタ
      */
    //------------------------------------------------------------------
    BattleUIPokeListLowerDetailStatusSubView( BattleUIPokeListLowerDraw* pParentView, const s32 useLytWkId, ::app::tool::MenuCursor* pBtnMenuCursor )
      : BattleUIPokeListLowerSubViewBase( pParentView, useLytWkId, pBtnMenuCursor )
    {
      _Clear();
    }

    //------------------------------------------------------------------
    /**
      * @brief   デストラクタ
      */
    //------------------------------------------------------------------
    virtual ~BattleUIPokeListLowerDetailStatusSubView()
    {
      {
        u32 loopNum = GFL_NELEMS( m_pSkillItemButtonList );
        for( u32 i = 0; i < loopNum; ++i )
        {
          GFL_SAFE_DELETE( m_pSkillItemButtonList[ i ] );
        }
      }

      GFL_SAFE_DELETE( m_pHpGaugePartsLayout );
      GFL_SAFE_DELETE( m_pExpGaugePartsLayout );
    }

    //------------------------------------------------------------------
    /**
      * @brief   初期化
      */
    //------------------------------------------------------------------
    virtual void Initialize()
    {
      GFL_ASSERT( m_pParentView );

      ::app::util::G2DUtil* pG2DUtil  = m_pParentView->GetG2DUtil();
      m_pLayoutWork   = pG2DUtil->GetLayoutWork( m_useLytWkId );

      const BattleUIPokeListLowerDraw::INIT_PARAM&  rParentViewInitParam  = m_pParentView->GetInitParam();

      const gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

      //  スキルオブジェクト項目一覧を作成
      {
        parts::BattleUISkillButtonPartsLayout::INIT_PARAM       skillItemObjectInitParam;
        parts::BattleUISkillButtonPartsLayout::INIT_PANE_PARAM  skillItemObjectInitPaneParam;

        u32 loopNum = GFL_NELEMS( m_pSkillItemButtonList );
        for( u32 i = 0; i < loopNum; ++i )
        {
          m_pSkillItemButtonList[ i ] = GFL_NEW( rParentViewInitParam.pUtilHeap->GetDeviceHeap() ) parts::BattleUISkillButtonPartsLayout( pG2DUtil, m_useLytWkId );

           //  必要データを出力
          _OutputSkillItemObjectInitParam( &skillItemObjectInitParam, &skillItemObjectInitPaneParam, i );
          m_pSkillItemButtonList[ i ]->Initialize( skillItemObjectInitParam, skillItemObjectInitPaneParam, rParentViewInitParam.pAppCommonGrpIconData, rParentViewInitParam.pUtilHeap->GetDeviceHeap() );
        }
      }

      //  各種パラメータ項目名
      {
        //  攻撃
        pG2DUtil->SetTextBoxPaneString( m_useLytWkId, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TEXLIST_00, STR_ATK );

        //  防御
        pG2DUtil->SetTextBoxPaneString( m_useLytWkId, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TEXLIST_01, STR_DEF );

        //  とくこう
        pG2DUtil->SetTextBoxPaneString( m_useLytWkId, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TEXLIST_02, STR_SPATK );

        //  とくぼう
        pG2DUtil->SetTextBoxPaneString( m_useLytWkId, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TEXLIST_03, STR_SPDEF );

        //  すばやさ
        pG2DUtil->SetTextBoxPaneString( m_useLytWkId, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TEXLIST_04, STR_AGI );
      }

      //  ＨＰゲージ作成
      {
        gfl2::lyt::LytParts*  pPartsPaneHpGauge = m_pLayoutWork->GetPartsPane( PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_PARTS_HP_00 );
        GFL_ASSERT( pPartsPaneHpGauge );

        ::app::tool::AppToolHPGaugePartsLayout::INIT_PARAM  initParam =
        {
          pG2DUtil,
          m_useLytWkId,

          //  数字の表示用のメッセージID
          STR_VALUE_3,
          //  数字の桁数
          3,

          m_pLayoutWork->GetTextBoxPane( pPartsPaneHpGauge, PANENAME_COMMON_GAG_000_PANE_TEXTBOX_00, &rMultiResId ),
          m_pLayoutWork->GetTextBoxPane( pPartsPaneHpGauge, PANENAME_COMMON_GAG_000_PANE_TEXTBOX_01, &rMultiResId ),
          m_pLayoutWork->GetTextBoxPane( pPartsPaneHpGauge, PANENAME_COMMON_GAG_000_PANE_TEXTBOX_02, &rMultiResId ),

          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTS_HP_00_GAUGE_HP,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTS_HP_00_COLOR_NORMAL,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTS_HP_00_COLOR_HALF,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTS_HP_00_COLOR_FEW,
        };

        m_pHpGaugePartsLayout = GFL_NEW( rParentViewInitParam.pUtilHeap->GetDeviceHeap() ) ::app::tool::AppToolHPGaugePartsLayout( initParam, rParentViewInitParam.pUtilHeap->GetDeviceHeap() );
      }

      //  EXPゲージ作成
      {
        ::app::tool::AppToolGaugePartsLayout::INIT_PARAM  initParam = 
        {
          pG2DUtil,
          m_useLytWkId,
          m_pLayoutWork->GetPartsPane( PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_PARTS_EXP_00 ),
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTS_EXP_00_GAUGE_00,
        };

        m_pExpGaugePartsLayout  = GFL_NEW( rParentViewInitParam.pUtilHeap->GetDeviceHeap() ) ::app::tool::AppToolGaugePartsLayout( initParam, rParentViewInitParam.pUtilHeap->GetDeviceHeap() );
      }

      //  道具名ペイン取得
      {
        m_pHaveItemTextBoxPane  = m_pLayoutWork->GetTextBoxPane( PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TEXT_ITEM );
        m_pItemInfoIconBtnParts = m_pLayoutWork->GetPartsPane( PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_INFO_01 );

        GFL_ASSERT( m_pHaveItemTextBoxPane );
      }

      //  特性項目ペイン取得
      {
        m_pHaveTokuseiTextBoxPane   = m_pLayoutWork->GetTextBoxPane( PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TEXT_TOKUSEI );
        m_pTokuseiInfoIconBtnParts  = m_pLayoutWork->GetPartsPane( PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_INFO_00 );
        
        GFL_ASSERT( m_pHaveTokuseiTextBoxPane );
      }
    }

    //------------------------------------------------------------------
    /**
      * @brief   開始
      */
    //------------------------------------------------------------------
    virtual void Start( BattleUIPokeListLowerButtonCtrl* pButtonCtrl )
    {
      BattleUIPokeListLowerSubViewBase::Start( pButtonCtrl );
      _SetState( STATE_TYPE_UPDATE );
    }

    //------------------------------------------------------------------
    /**
      * @brief   終了
      */
    //------------------------------------------------------------------
    virtual void End( void )
    {
      BattleUIPokeListLowerSubViewBase::End();
    }

    //------------------------------------------------------------------
    /**
      * @brief   更新処理
      */
    //------------------------------------------------------------------
    virtual void Update( void )
    {
      m_pExpGaugePartsLayout->Update();
      m_pHpGaugePartsLayout->Update();

      BattleUIPokeListLowerSubViewBase::Update();
    }

    //------------------------------------------------------------------
    /**
      * @brief   ポケモン情報を設定
      */
    //------------------------------------------------------------------
    virtual void SetPokeInfoData( const BattleUIPokeListLowerDraw::PokeObject* pPokeObject )
    {
      GFL_ASSERT( pPokeObject );

      const BattleUIPokeListLowerDraw::PokeObject::INIT_PARAM&  rPokeObjectParam  = pPokeObject->GetInitParam();
      const BTL_POKEPARAM*  pPokeParam  = rPokeObjectParam.pBtlParam;
      GFL_ASSERT( pPokeParam );

      ::app::util::G2DUtil* pG2DUtil  = m_pParentView->GetG2DUtil();

      const pml::pokepara::PokemonParam*  pOrgPokeParam = pPokeParam->GetSrcData();

      if( pOrgPokeParam->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )
      {
        pG2DUtil->SetPaneVisible( m_useLytWkId, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_NULL_01, false );

        //  入力をOFF
        m_pButtonCtrl->SetEnableButtonAll( false );

        return;
      }

      pG2DUtil->SetPaneVisible( m_useLytWkId, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_NULL_01, true );
      //  入力をON
      m_pButtonCtrl->SetEnableButtonAll( true );

      //  HPゲージ
      {
        GFL_ASSERT( m_pHpGaugePartsLayout );
        m_pHpGaugePartsLayout->SetNumber( pPokeParam->GetValue( BTL_POKEPARAM::BPP_HP ), pPokeParam->GetValue( BTL_POKEPARAM::BPP_MAX_HP ) );
      }

      //  EXPゲージ
      //  経験値のゲージ設定
      const u32 maxExp  = ::app::util::GetPokeNextLvExpNum( pPokeParam->GetSrcData() );
      const u32 nowExp  = ::app::util::GetPokeNowLvExpNum( pPokeParam->GetSrcData() );

      {
        GFL_ASSERT( m_pExpGaugePartsLayout );
        m_pExpGaugePartsLayout->SetNumber( nowExp, maxExp );
      }

      //  所持特性とアイテムidを設定
      {
        m_activeHaveItemId    = pPokeParam->GetItem();
        // @fix NMCat[234] 変更前の元の特性を表示
        // @fix NMCat[1845] イリュージョンしたデータから特性を取得していたのを元ポケモンパラメータからに変更
        m_activeHaveTokuseiId = pOrgPokeParam->GetTokuseiNo();
      }

      //  EXP値
      {
        //  現在値
        // @fix GFNMCat[2467] 数字は０埋めにしない
        pG2DUtil->SetTextBoxPaneNumber( m_useLytWkId, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TEXEXP_00, str_btl_poke_exp, nowExp, 5, 0 );

        //  /の記号
        pG2DUtil->SetTextBoxPaneString( m_useLytWkId, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TEXEXP_01, str_btl_poke_hp_slash );

        //  つぎのレベルに必要な経験値
        // @fix GFNMCat[2467] 数字は０埋めにしない
        pG2DUtil->SetTextBoxPaneNumber( m_useLytWkId, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TEXEXP_02, str_btl_poke_exp_s, maxExp, 5, 0 );
      }

      //  特性
      {
        if( rPokeObjectParam.pTokuseiName == NULL )
        {
          pG2DUtil->SetPaneVisible( m_pHaveTokuseiTextBoxPane, false );
          pG2DUtil->SetPaneVisible( m_pTokuseiInfoIconBtnParts, false );
        }
        else
        {
          //  特性名表示
          {
            pG2DUtil->SetTextBoxPaneString( m_pHaveTokuseiTextBoxPane, rPokeObjectParam.pTokuseiName );
          }

          pG2DUtil->SetPaneVisible( m_pHaveTokuseiTextBoxPane, true );
          pG2DUtil->SetPaneVisible( m_pTokuseiInfoIconBtnParts, true );
        }
      }

      //  所持アイテム
      {
        if( rPokeObjectParam.pHaveItemName == NULL )
        {
          pG2DUtil->SetPaneVisible( m_pHaveItemTextBoxPane, false );
          pG2DUtil->SetPaneVisible( m_pItemInfoIconBtnParts, false );
        }
        else
        {
          //  アイテム名設定
          pG2DUtil->SetTextBoxPaneString( m_pHaveItemTextBoxPane, rPokeObjectParam.pHaveItemName );

          pG2DUtil->SetPaneVisible( m_pHaveItemTextBoxPane, true );
          pG2DUtil->SetPaneVisible( m_pItemInfoIconBtnParts, true );
        }

        u16 itemNo  = pPokeParam->GetItem();
        if( itemNo == ITEM_DUMMY_DATA )
        {
          pG2DUtil->SetPaneVisible( m_pHaveItemTextBoxPane, false );
        }
        else
        {
          //  アイテム名設定
          pG2DUtil->SetRegisterItemName( 0, itemNo );
          pG2DUtil->SetTextBoxPaneStringExpand( m_pHaveItemTextBoxPane, STR_ITEM_NAME );

          pG2DUtil->SetPaneVisible( m_pHaveItemTextBoxPane, true );
        }
      }

      //  各パラメータ値
      {
        //  攻撃
        pG2DUtil->SetTextBoxPaneNumber( m_useLytWkId, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TEXPARAM_00, STR_VALUE_3, pOrgPokeParam->GetPower( pml::pokepara::POWER_ATK ), 3, print::NUM_DISP_LEFT );

        //  防御
        pG2DUtil->SetTextBoxPaneNumber( m_useLytWkId, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TEXPARAM_01, STR_VALUE_3, pOrgPokeParam->GetPower( pml::pokepara::POWER_DEF ), 3, print::NUM_DISP_LEFT );

        //  とくこう
        pG2DUtil->SetTextBoxPaneNumber( m_useLytWkId, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TEXPARAM_02, STR_VALUE_3, pOrgPokeParam->GetPower( pml::pokepara::POWER_SPATK ), 3, print::NUM_DISP_LEFT );

        //  とくぼう
        pG2DUtil->SetTextBoxPaneNumber( m_useLytWkId, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TEXPARAM_03, STR_VALUE_3, pOrgPokeParam->GetPower( pml::pokepara::POWER_SPDEF ), 3, print::NUM_DISP_LEFT );

        //  すばやさ
        pG2DUtil->SetTextBoxPaneNumber( m_useLytWkId, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TEXPARAM_04, STR_VALUE_3, pOrgPokeParam->GetPower( pml::pokepara::POWER_AGI ), 3, print::NUM_DISP_LEFT );
      }

      //  わざ一覧を表示(技はバトルではなくオリジナルのデータを参照、変身状態だと変身した相手の技が取得してしまうのを避けるため)
      {
        //  一旦すべての項目を非表示
        {
          u32 loopNum = GFL_NELEMS( m_pSkillItemButtonList );
          for( u32 i = 0; i < loopNum; ++i )
          {
            m_pSkillItemButtonList[ i ]->SetVisible( false );
            //  インフォアイコンは表示不要
            m_pSkillItemButtonList[ i ]->SetInfoIconVisible( false );
          }
        }

        //  オリジナルの技一覧から項目設定
        WazaNo  wazaNo          = WAZANO_NULL;

        const BattleUIPokeListLowerDraw::INIT_PARAM&  rParentViewInitParam  = m_pParentView->GetInitParam();

        for( u8 i = 0; i < rPokeObjectParam.wazaCount; ++i )
        {
          //  わざがあるので表示
          m_pSkillItemButtonList[ i ]->SetVisible( true );

          wazaNo  = rPokeObjectParam.aWazaParam[ i ].id;
 
          //  技id指定
          m_pSkillItemButtonList[ i ]->SetId( wazaNo, rPokeObjectParam.aWazaParam[ i ].pWazaNameBuffer );

          //  PP値の指定
          // @fix NMCat[496] 技情報はバトル用のポケパラメータでないと反映できない
          m_pSkillItemButtonList[ i ]->SetPPNum( rPokeObjectParam.aWazaParam[ i ].nowPP, rPokeObjectParam.aWazaParam[ i ].maxPP );

          //  技の効果指定
          if( rParentViewInitParam.pDelegate != NULL )
          {
            bool bPutFlg  = false;
            BtlTypeAffAbout type  = rParentViewInitParam.pDelegate->OnGetKoukaType( &bPutFlg, wazaNo, pPokeParam );
            m_pSkillItemButtonList[ i ]->SetKoukaType( type );
            m_pSkillItemButtonList[ i ]->SetKoukaTypeVisible( bPutFlg );
          }
        }

        //  わざの個数を保持
        m_skillListMax = rPokeObjectParam.wazaCount;
      }
    }

    //  技ボタンのデータポインタ取得
    parts::BattleUISkillButtonPartsLayout*  GetSkillButtonPtr( const u8 itemIdx )
    {
      GFL_ASSERT( itemIdx < SKILL_LIST_MAX );

      return  m_pSkillItemButtonList[ itemIdx ];
    }

    //  わざリスト最大数
    u8 GetSkillListNum() const { return m_skillListMax; }

    //  所持特性id
    u16 GetHaveTokuseiId() const { return m_activeHaveTokuseiId; }

    //  所持アイテムid
    s32 GetHaveItemId() const { return m_activeHaveItemId; }

    //  アイテムインフォアイコンの部品ペイン
    gfl2::lyt::LytParts*  GetItemInfoIconPartsPane() { return m_pItemInfoIconBtnParts; }

    //  特性インフォアイコンの部品ペイン
    gfl2::lyt::LytParts*  GetTokuseiInfoIconPartsPane() { return m_pTokuseiInfoIconBtnParts; }

  private:

    enum
    {
      SKILL_LIST_MAX  = 4,
    };

    //  ステート設定
    void _SetState( const StateType newStateType )
    {
      ::app::util::G2DUtil* pG2DUtil  = m_pParentView->GetG2DUtil();

      m_state = newStateType;

      if( newStateType == STATE_TYPE_UPDATE )
      {
        pG2DUtil->SetPaneVisible( m_useLytWkId, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_NULL_00, false );
        pG2DUtil->SetPaneVisible( m_useLytWkId, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_NULL_01, true );
        pG2DUtil->StartAnime( m_useLytWkId, LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000_PAGE_01 );
      }
    }

    //  parts::BattleUISkillButtonPartsLayoutクラスに必要な初期化パラメータを出力する
    void _OutputSkillItemObjectInitParam( parts::BattleUISkillButtonPartsLayout::INIT_PARAM* pOutputInitParam, parts::BattleUISkillButtonPartsLayout::INIT_PANE_PARAM* pOutPanePartsInitParam, const u32 itemIdx )
    {
      GFL_ASSERT( pOutputInitParam );
      GFL_ASSERT( pOutPanePartsInitParam );
      GFL_ASSERT( itemIdx < SKILL_LIST_MAX );

      ::app::util::G2DUtil* pG2DUtil  = m_pParentView->GetG2DUtil();
      gfl2::lyt::LytWk* pLayoutWork  = pG2DUtil->GetLayoutWork( m_useLytWkId );

      const gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

      //  初期パラメータ
      {
        const parts::BattleUISkillButtonPartsLayout::INIT_PARAM aSkillItemInitParam[ SKILL_LIST_MAX ]  =
        {
          /*
            //  リストidx( 0 〜 3 )
            s32 list_idx;

            //  ボタンID
            s32 button_id;

            //  アニメidリスト
            s32 a_anim_id_list[ ANIM_TYPE_MAX ];
          */
          //  1列目
          {
            BattleUIPokeListLowerButtonCtrl::BUTTON_ID_SKILL_ITEM_00,
            -1,

            {
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_TOUCH,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_RELEASE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_CANSEL,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_PASSIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_ACTIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_TOUCH_RELEASE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_COLOR_00,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_COLOR_01,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_COLOR_02,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_COLOR_03,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_COLOR_04,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_COLOR_05,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_COLOR_06,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_COLOR_07,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_COLOR_08,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_COLOR_09,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_COLOR_10,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_COLOR_11,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_COLOR_12,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_COLOR_13,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_COLOR_14,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_COLOR_15,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_COLOR_16,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_COLOR_17,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_TEXCOLOR_00,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_TEXCOLOR_01,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_TEXCOLOR_02,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00_TEXCOLOR_03,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00__PARTS_INFO_TOUCH,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00__PARTS_INFO_RELEASE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00__PARTS_INFO_CANSEL,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00__PARTS_INFO_PASSIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00__PARTS_INFO_ACTIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00__PARTS_INFO_TOUCH_RELEASE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00__PARTS_KOUKA_ACTIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00__PARTS_KOUKA_PASSIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00__PARTS_KOUKA_KOUKA_00,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00__PARTS_KOUKA_KOUKA_01,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00__PARTS_KOUKA_KOUKA_02,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_00__PARTS_KOUKA_KOUKA_03,
            }
          },

          //  2列目
          {
            BattleUIPokeListLowerButtonCtrl::BUTTON_ID_SKILL_ITEM_01,
            -1,

            {
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_TOUCH,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_RELEASE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_CANSEL,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_PASSIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_ACTIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_TOUCH_RELEASE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_COLOR_00,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_COLOR_01,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_COLOR_02,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_COLOR_03,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_COLOR_04,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_COLOR_05,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_COLOR_06,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_COLOR_07,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_COLOR_08,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_COLOR_09,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_COLOR_10,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_COLOR_11,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_COLOR_12,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_COLOR_13,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_COLOR_14,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_COLOR_15,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_COLOR_16,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_COLOR_17,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_TEXCOLOR_00,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_TEXCOLOR_01,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_TEXCOLOR_02,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01_TEXCOLOR_03,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01__PARTS_INFO_TOUCH,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01__PARTS_INFO_RELEASE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01__PARTS_INFO_CANSEL,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01__PARTS_INFO_PASSIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01__PARTS_INFO_ACTIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01__PARTS_INFO_TOUCH_RELEASE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01__PARTS_KOUKA_ACTIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01__PARTS_KOUKA_PASSIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01__PARTS_KOUKA_KOUKA_00,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01__PARTS_KOUKA_KOUKA_01,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01__PARTS_KOUKA_KOUKA_02,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_01__PARTS_KOUKA_KOUKA_03,
            }
          },

          //  3列目
          {
            BattleUIPokeListLowerButtonCtrl::BUTTON_ID_SKILL_ITEM_02,
            -1,

            {
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_TOUCH,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_RELEASE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_CANSEL,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_PASSIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_ACTIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_TOUCH_RELEASE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_COLOR_00,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_COLOR_01,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_COLOR_02,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_COLOR_03,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_COLOR_04,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_COLOR_05,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_COLOR_06,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_COLOR_07,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_COLOR_08,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_COLOR_09,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_COLOR_10,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_COLOR_11,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_COLOR_12,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_COLOR_13,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_COLOR_14,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_COLOR_15,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_COLOR_16,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_COLOR_17,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_TEXCOLOR_00,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_TEXCOLOR_01,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_TEXCOLOR_02,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02_TEXCOLOR_03,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02__PARTS_INFO_TOUCH,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02__PARTS_INFO_RELEASE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02__PARTS_INFO_CANSEL,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02__PARTS_INFO_PASSIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02__PARTS_INFO_ACTIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02__PARTS_INFO_TOUCH_RELEASE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02__PARTS_KOUKA_ACTIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02__PARTS_KOUKA_PASSIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02__PARTS_KOUKA_KOUKA_00,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02__PARTS_KOUKA_KOUKA_01,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02__PARTS_KOUKA_KOUKA_02,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_02__PARTS_KOUKA_KOUKA_03,
            }
          },

          //  4列目
          {
            BattleUIPokeListLowerButtonCtrl::BUTTON_ID_SKILL_ITEM_03,
            -1,

            {
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_TOUCH,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_RELEASE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_CANSEL,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_PASSIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_ACTIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_TOUCH_RELEASE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_COLOR_00,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_COLOR_01,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_COLOR_02,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_COLOR_03,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_COLOR_04,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_COLOR_05,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_COLOR_06,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_COLOR_07,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_COLOR_08,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_COLOR_09,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_COLOR_10,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_COLOR_11,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_COLOR_12,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_COLOR_13,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_COLOR_14,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_COLOR_15,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_COLOR_16,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_COLOR_17,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_TEXCOLOR_00,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_TEXCOLOR_01,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_TEXCOLOR_02,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03_TEXCOLOR_03,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03__PARTS_INFO_TOUCH,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03__PARTS_INFO_RELEASE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03__PARTS_INFO_CANSEL,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03__PARTS_INFO_PASSIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03__PARTS_INFO_ACTIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03__PARTS_INFO_TOUCH_RELEASE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03__PARTS_KOUKA_ACTIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03__PARTS_KOUKA_PASSIVE,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03__PARTS_KOUKA_KOUKA_00,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03__PARTS_KOUKA_KOUKA_01,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03__PARTS_KOUKA_KOUKA_02,
              LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSSKILLPLT_03__PARTS_KOUKA_KOUKA_03,
            }
          }
        };

        *pOutputInitParam = aSkillItemInitParam[ itemIdx ];
      }

      //  ペイン初期パラメータ
      {
        const s32 aRootPaneId[ SKILL_LIST_MAX ] =
        {
          PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_PARTSSKILLPLT_00,
          PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_PARTSSKILLPLT_01,
          PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_PARTSSKILLPLT_02,
          PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_PARTSSKILLPLT_03,
        };

        //  パーツルートペイン
        pOutPanePartsInitParam->pRootPartsPane  = pLayoutWork->GetPartsPane( aRootPaneId[ itemIdx ] );
        GFL_ASSERT( pOutPanePartsInitParam->pRootPartsPane );

        //  カーソルNULLペイン
        pOutPanePartsInitParam->pCursorNullPane  = pLayoutWork->GetPane( pOutPanePartsInitParam->pRootPartsPane, PANENAME_BATTLE_PLT_LOW_002_PANE_NULL_CUR, &rMultiResId );

        //  ボタンのヒット判定のボックスペイン
        pOutPanePartsInitParam->pButtonHitBoxPane  = pLayoutWork->GetBoundingPane( pOutPanePartsInitParam->pRootPartsPane, PANENAME_BATTLE_PLT_LOW_002_PANE_BOUND_00, &rMultiResId );

        //  インフォアイコンのパーツペイン
        pOutPanePartsInitParam->pInfoIconPartsPane = pLayoutWork->GetPartsPane( pOutPanePartsInitParam->pRootPartsPane, PANENAME_BATTLE_PLT_LOW_002_PANE_PARTS_INFO, &rMultiResId );

        //  インフォアイコンのヒット判定のボックスペイン
        pOutPanePartsInitParam->pInfoIconHitBoxPane  = pLayoutWork->GetBoundingPane( pOutPanePartsInitParam->pInfoIconPartsPane, PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00, &rMultiResId );

        //  現在PP値のテキストペイン
        pOutPanePartsInitParam->pNowPPTextBoxPane  = pLayoutWork->GetTextBoxPane( pOutPanePartsInitParam->pRootPartsPane, PANENAME_BATTLE_PLT_LOW_002_PANE_TEXTBOX_05, &rMultiResId );

        //  最大PP値のテキストペイン
        pOutPanePartsInitParam->pMaxPPNumTextBoxPane = pLayoutWork->GetTextBoxPane( pOutPanePartsInitParam->pRootPartsPane, PANENAME_BATTLE_PLT_LOW_002_PANE_TEXTBOX_03, &rMultiResId );

        /** @fix  MMCat[171]   つよさをみる画面のPP値の文字色に関する質問:  "PP","/"も色変更の対象とする  */
        //  "PP"
        pOutPanePartsInitParam->pPPLavelTextBoxPane = pLayoutWork->GetTextBoxPane( pOutPanePartsInitParam->pRootPartsPane, PANENAME_BATTLE_PLT_LOW_002_PANE_TEXTBOX_01, &rMultiResId );
        //  "/"
        pOutPanePartsInitParam->pPPSlashTextBoxPane = pLayoutWork->GetTextBoxPane( pOutPanePartsInitParam->pRootPartsPane, PANENAME_BATTLE_PLT_LOW_002_PANE_TEXTBOX_04, &rMultiResId );

        //  技名のテキストペイン
        pOutPanePartsInitParam->pSkillNameTextBoxPane  = pLayoutWork->GetTextBoxPane( pOutPanePartsInitParam->pRootPartsPane, PANENAME_BATTLE_PLT_LOW_002_PANE_TEXTBOX_00, &rMultiResId );

        //  効果アイコンペイン
        pOutPanePartsInitParam->pKoukaIconPartsPane  = pLayoutWork->GetPartsPane( pOutPanePartsInitParam->pRootPartsPane, PANENAME_BATTLE_PLT_LOW_002_PANE_PARTS_KOUKA, &rMultiResId );

        //  PP表記テキストペイン
        pOutPanePartsInitParam->pPPTitleTextPane  = pLayoutWork->GetTextBoxPane( pOutPanePartsInitParam->pRootPartsPane, PANENAME_BATTLE_PLT_LOW_002_PANE_TEXTBOX_01, &rMultiResId );

        //  わざタイプアイコンペイン
        {
          gfl2::lyt::LytParts*  pPartsPane  = pLayoutWork->GetPartsPane( pOutPanePartsInitParam->pRootPartsPane, PANENAME_BATTLE_PLT_LOW_002_PANE_PARTS_TYPE, &rMultiResId );
          GFL_ASSERT( pPartsPane );

          gfl2::lyt::LytPicture*  pPicPane  = pLayoutWork->GetPicturePane( pPartsPane, PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE, &rMultiResId );
          GFL_ASSERT( pPicPane );
          pOutPanePartsInitParam->pTypeIconPicPane     = pPicPane;
        }
      }
    }

    //  変数の初期化
    void _Clear()
    {
      m_pHpGaugePartsLayout     = NULL;
      m_pExpGaugePartsLayout    = NULL;
      m_pLayoutWork             = NULL;
      m_state                   = STATE_TYPE_NONE;
      m_pHaveItemTextBoxPane    = NULL;
      m_pHaveTokuseiTextBoxPane = NULL;
      m_pItemInfoIconBtnParts   = NULL;
      m_pTokuseiInfoIconBtnParts  = NULL;
      m_skillListMax            = 0;
      m_activeHaveTokuseiId     = 0;
      m_activeHaveItemId        = 0;

      gfl2::std::MemFill( m_pSkillItemButtonList, NULL, sizeof( m_pSkillItemButtonList ) );
    }

    StateType                               m_state;

    u8                                      m_skillListMax;
    u16                                     m_activeHaveTokuseiId;
    s32                                     m_activeHaveItemId;

    gfl2::lyt::LytWk*                       m_pLayoutWork;

    ::app::tool::AppToolHPGaugePartsLayout* m_pHpGaugePartsLayout;
    ::app::tool::AppToolGaugePartsLayout*   m_pExpGaugePartsLayout;

    gfl2::lyt::LytTextBox*                  m_pHaveItemTextBoxPane;
    gfl2::lyt::LytTextBox*                  m_pHaveTokuseiTextBoxPane;

    gfl2::lyt::LytParts*                    m_pItemInfoIconBtnParts;
    gfl2::lyt::LytParts*                    m_pTokuseiInfoIconBtnParts;

    parts::BattleUISkillButtonPartsLayout*  m_pSkillItemButtonList[ SKILL_LIST_MAX ];
  };

//
//  ここから先は BattleUIPokeListLowerSimpleStatusSubButtonCtrl クラスの定義
//
  //  簡易ステータス表示の入力制御クラス
  class BattleUIPokeListLowerSimpleStatusSubButtonCtrl : public BattleUIPokeListLowerButtonCtrl
  {
  public:
    BattleUIPokeListLowerSimpleStatusSubButtonCtrl()
    {
      _Clear();
    }

    //  初期化
    virtual void Initialize( BattleUIPokeListLowerDraw* pRootView, ::app::ui::UIView* pActiveView, const s32 useLytWkId, gfl2::lyt::LytMultiResID* pMultiResId, ::app::util::Heap* pUtilHeap, ::app::tool::MenuCursor* pMenuCursor )
    {
      BattleUIPokeListLowerButtonCtrl::_SetBaseParam( pRootView, pActiveView, useLytWkId, pMultiResId, pUtilHeap );
      GFL_ASSERT( pMenuCursor );

      m_pMenuCursor = pMenuCursor;
      m_pMenuCursor->StartAnime();

      ::app::util::G2DUtil* pG2DUtil  = pRootView->GetG2DUtil();
      pG2DUtil->StartAnime( 0, LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000_POS_SELECT3 );

      //  ボタン作成
      //  入れ替えるボタンに常に必要なデータを設定
      gfl2::lyt::LytParts*  pChangeButtonPartsPane  = NULL;
      {
        BUTTON_PANE_DATA* pPaneData = &m_aButtonPaneData[ BUTTON_OBJ_POKE_CHANGE ];

        pChangeButtonPartsPane = m_pLayoutWork->GetPartsPane( PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_PARTSMSGBTN_00 );
        GFL_ASSERT( pChangeButtonPartsPane );

        pPaneData->buttonId     = BUTTON_ID_CHANGE;
        pPaneData->seID         = SEQ_SE_DECIDE1;
        pPaneData->pCursorPane  = m_pLayoutWork->GetPane( pChangeButtonPartsPane, PANENAME_POKELIST_BTN_LOW_000_PANE_NULL_CUR, pMultiResId );
        GFL_ASSERT( pPaneData->pCursorPane );
      }

      //  かいふくボタンに常に必要なデータを設定
      gfl2::lyt::LytParts*  pCureButtonPartsPane  = NULL;
      {
        BUTTON_PANE_DATA* pPaneData = &m_aButtonPaneData[ BUTTON_OBJ_POKE_CURE ];

        pCureButtonPartsPane = m_pLayoutWork->GetPartsPane( PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_PARTSMSGBTN_01 );
        GFL_ASSERT( pCureButtonPartsPane );

        pPaneData->buttonId     = BUTTON_ID_CURE;
        pPaneData->seID         = SEQ_SE_DECIDE1;
        pPaneData->pCursorPane  = m_pLayoutWork->GetPane( pCureButtonPartsPane, PANENAME_POKELIST_BTN_LOW_000_PANE_NULL_CUR, pMultiResId );
        GFL_ASSERT( pPaneData->pCursorPane );
      }

      //  つよさをみるボタンに常に必要なデータを設定
      gfl2::lyt::LytParts*  pStatusButtonPartsPane  = NULL;
      {
        BUTTON_PANE_DATA* pPaneData = &m_aButtonPaneData[ BUTTON_OBJ_POKE_STATUS ];

        pStatusButtonPartsPane = m_pLayoutWork->GetPartsPane( PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_PARTSMSGBTN_02 );
        GFL_ASSERT( pStatusButtonPartsPane );

        pPaneData->buttonId     = BUTTON_ID_STATUS;
        pPaneData->seID         = SEQ_SE_DECIDE1;
        pPaneData->pCursorPane  = m_pLayoutWork->GetPane( pStatusButtonPartsPane, PANENAME_POKELIST_BTN_LOW_000_PANE_NULL_CUR, pMultiResId );
        GFL_ASSERT( pPaneData->pCursorPane );
      }

      //  ボタン設定データ定義一覧
      const ::app::ui::ButtonInfoEx    aButtonPaneIdxList[ BUTTON_OBJ_MAX ] =
      {
        //  入れ替えるボタン
        {
          m_aButtonPaneData[ BUTTON_OBJ_POKE_CHANGE ].buttonId,
          pChangeButtonPartsPane,
          m_pLayoutWork->GetBoundingPane( pChangeButtonPartsPane, PANENAME_POKELIST_BTN_LOW_000_PANE_BOUND_00, pMultiResId ),
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_00_TOUCH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_00_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_00_CANSEL,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_00_TOUCH_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_00_ACTIVE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_00_PASSIVE,
        },

        //  かいふくする
        {
          m_aButtonPaneData[ BUTTON_OBJ_POKE_CURE ].buttonId,
          pCureButtonPartsPane,
          m_pLayoutWork->GetBoundingPane( pCureButtonPartsPane, PANENAME_POKELIST_BTN_LOW_000_PANE_BOUND_00, pMultiResId ),
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_01_TOUCH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_01_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_01_CANSEL,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_01_TOUCH_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_01_ACTIVE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_01_PASSIVE,
        },

        //  つよさを見る
        {
          m_aButtonPaneData[ BUTTON_OBJ_POKE_STATUS ].buttonId,
          pStatusButtonPartsPane,
          m_pLayoutWork->GetBoundingPane( pStatusButtonPartsPane, PANENAME_POKELIST_BTN_LOW_000_PANE_BOUND_00, pMultiResId ),
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_02_TOUCH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_02_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_02_CANSEL,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_02_TOUCH_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_02_ACTIVE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_02_PASSIVE,
        },
      };

      //  ボタン登録
      pActiveView->ReleaseButtonManager();
      pActiveView->CreateButtonManager( pUtilHeap, m_pLayoutWork, aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );

      //  ボタンテキスト設定
      {
        //  入れ替えるボタン
        m_aButtonPaneData[ BUTTON_OBJ_POKE_CHANGE ].pNameTextBoxPane  = m_pLayoutWork->GetTextBoxPane( pChangeButtonPartsPane, PANENAME_POKELIST_BTN_LOW_000_PANE_TEXTBOX_00, pMultiResId );
        GFL_ASSERT( m_aButtonPaneData[ BUTTON_OBJ_POKE_CHANGE ].pNameTextBoxPane );

        //  回復するボタン
        m_aButtonPaneData[ BUTTON_OBJ_POKE_CURE ].pNameTextBoxPane  = m_pLayoutWork->GetTextBoxPane( pCureButtonPartsPane, PANENAME_POKELIST_BTN_LOW_000_PANE_TEXTBOX_00, pMultiResId );
        GFL_ASSERT( m_aButtonPaneData[ BUTTON_OBJ_POKE_CURE ].pNameTextBoxPane );

        //  つよさを見るボタン
        m_aButtonPaneData[ BUTTON_OBJ_POKE_STATUS ].pNameTextBoxPane  = m_pLayoutWork->GetTextBoxPane( pStatusButtonPartsPane, PANENAME_POKELIST_BTN_LOW_000_PANE_TEXTBOX_00, pMultiResId );
        GFL_ASSERT( m_aButtonPaneData[ BUTTON_OBJ_POKE_STATUS ].pNameTextBoxPane );
      }

      //  SE設定
      if( m_bUseInputSE )
      {
        u32  loopNum = GFL_NELEMS( m_aButtonPaneData );
        ::app::tool::ButtonManager* pButtonManager  = pActiveView->GetButtonManager();
        for( u32 i = 0; i < loopNum; ++i )
        {
          pButtonManager->SetButtonSelectSE( m_aButtonPaneData[ i ].buttonId, m_aButtonPaneData[ i ].seID );
        }
      }

      m_pMenuCursor->SetVisible( false );
    }

    //  更新処理
    virtual void Update()
    {
      if( m_bFirstCursorUpdate )
      {
        //  ボタン位置変更アニメが終わった後に設定する
        //  そうしないとペイン位置が正しく取得できないので、カーソル位置がおかしくなる。
        if( m_pLayoutWork->IsAnimeEnd( LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000_POS_SELECT3 ) )
        {
          //  入力コールバック設定
          m_pActiveView->SetInputListener( this );

          // @fix GFNMCat[1054] 初回ポケモンカーソルがタマゴの場合にはカーソルを表示にしないといけない
          m_pMenuCursor->SetVisible( m_bInputEnable );

          //  初回カーソル位置を指定
          if( m_cursorIdx == -1 )
          {
            SetCursorMoveIdx( 0 );
          }
          else
          {
            SetCursorMoveIdx( m_cursorIdx );
          }

          m_bFirstCursorUpdate  = false;
        }
      }
    }

    //------------------------------------------------------------------
    /**
      * @brief   ボタンの有効設定
      */
    //------------------------------------------------------------------
    virtual void SetEnableButtonAll( const bool bFlg )
    {
      BattleUIPokeListLowerButtonCtrl::SetEnableButtonAll( bFlg );

      m_pMenuCursor->SetVisible( bFlg );
    }

    //------------------------------------------------------------------
    /**
      * @brief   カレントカーソル項目を決定
      */
    //------------------------------------------------------------------
    virtual bool PushBtnCurrentCursor()
    {
      if( PushButtonId( m_aButtonPaneData[ m_cursorIdx ].buttonId ) )
      {
        return  true;
      }

      return false;
    }

    //  カレントカーソルを設定(押せるButtonId値で判別、一致しない場合は何も処理しない)
    virtual void SetCurrentCursor( const ButtonId id, const bool bUseMoveSE )
    {
      s32 loopNum = GFL_NELEMS( m_aButtonPaneData );
      for( s32 i = 0; i < loopNum; ++i )
      {
        if( m_aButtonPaneData[ i ].buttonId == id )
        {
          //  カーソル移動
          SetCursorMoveIdx( i );

          return;
        }
      }
    }

    //  ボタンのテキスト設定
    virtual void SetNameButton( const ButtonId id, const s32 textId )
    {
      s32 buttonDataIdx = -1;

      //  指定idからdataIdxを探す
      {
        s32 dataMax = GFL_NELEMS( m_aButtonPaneData );
        for( s32 i = 0; i < dataMax; ++i )
        {
          if( m_aButtonPaneData[ i ].buttonId == id )
          {
            buttonDataIdx = i;
            break;
          }
        }
      }

      if( ( 0 <= buttonDataIdx ) && ( buttonDataIdx < GFL_NELEMS( m_aButtonPaneData ) )  )
      {
        m_pG2DUtil->SetTextBoxPaneString( m_aButtonPaneData[ buttonDataIdx ].pNameTextBoxPane, textId );
      }
      else
      {
        GFL_ASSERT( 0 );
      }
    }

    //------------------------------------------------------------------
    /**
      * @brief   カーソル移動
      */
    //------------------------------------------------------------------
    virtual bool MoveCursorUp( const bool bLoop )
    {
      if( m_bInputEnable == false )
      {
        return  false;
      }

      //  入れ替えボタン位置なら移動しない
      if( m_cursorIdx == BUTTON_OBJ_POKE_CHANGE )
      {
        return  false;
      }

      SetCursorMoveIdx( BUTTON_OBJ_POKE_CHANGE );
      return  true;
    }

    virtual bool MoveCursorDown( const bool bLoop )
    {
      if( m_bInputEnable == false )
      {
        return  false;
      }

      //  入れ替えボタン位置なら移動する
      if( m_cursorIdx == BUTTON_OBJ_POKE_CHANGE )
      {
        s32 nextCursorIdx = m_cursorOldIdx;
        if( nextCursorIdx == BUTTON_OBJ_POKE_CHANGE )
        {
          nextCursorIdx = BUTTON_OBJ_POKE_CURE;
        }

        //  移動できないのであれば、絶対に移動できるつよさを見るに移動させる
        if( SetCursorMoveIdx( nextCursorIdx ) == false )
        {
          SetCursorMoveIdx( BUTTON_OBJ_POKE_STATUS );
        }

        return  true;
      }

      return  false;
    }

    virtual bool MoveCursorLeft( const bool bLoop )
    {
      if( m_bInputEnable == false )
      {
        return  false;
      }

      //  強さを見る項目なら移動できる
      if( m_cursorIdx == BUTTON_OBJ_POKE_STATUS )
      {
        SetCursorMoveIdx( BUTTON_OBJ_POKE_CURE );
        return  true;
      }

      return false;
    }

    virtual bool MoveCursorRight( const bool bLoop )
    {
      if( m_bInputEnable == false )
      {
        return  false;
      }

      //  かいふくする項目なら移動できる
      if( m_cursorIdx == BUTTON_OBJ_POKE_CURE )
      {
        SetCursorMoveIdx( BUTTON_OBJ_POKE_STATUS );
        return  true;
      }

      return false;
    }

    //  カーソル移動idxを設定
    virtual bool SetCursorMoveIdx( const s32 cursorIdx )
    {
      if( m_pActiveView->GetButtonManager()->IsButtonVisible( m_aButtonPaneData[ cursorIdx ].buttonId ) == false )
      {
        return  false;
      }

      m_cursorOldIdx  = m_cursorIdx;
      m_cursorIdx     = cursorIdx;

      m_pMenuCursor->PutNonVisible( m_pLayoutWork, m_aButtonPaneData[ cursorIdx ].pCursorPane );

      return  true;
    }

    //  空関数にして、連続でキーアクションが呼ばれないように対応
    virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick ) { return ENABLE_ACCESS; }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief ペインタッチ時を検知
    * @note 主な用途  RELEASE判定だが、決定前時にカーソルを移動したい、とか
    */
    //--------------------------------------------------------------------------------------------
    virtual void OnLayoutPaneTouchTriggerEvent( const u32 buttonId )
    {
      u32 max = GFL_NELEMS( m_aButtonPaneData );
      for( u32 i = 0; i < max; ++i )
      {
        if( m_aButtonPaneData[ i ].buttonId == (s32)buttonId )
        {
          SetCursorMoveIdx( i );
          return;
        }
      }
    }

  private:
    enum ButtonObj
    {
      BUTTON_OBJ_POKE_CHANGE  = 0,
      BUTTON_OBJ_POKE_CURE,
      BUTTON_OBJ_POKE_STATUS,

      BUTTON_OBJ_MAX
    };

    //  ボタンペインデータ
    typedef struct _tag_button_pane_data_
    {
      //  ボタンid
      ButtonId buttonId;

      //  ボタン名のテキストペイン
      gfl2::lyt::LytTextBox*  pNameTextBoxPane;

      //  カーソルペイン
      gfl2::lyt::LytPane*     pCursorPane;

      //  SEid
      u32                     seID;
    } BUTTON_PANE_DATA;

    //  変数初期化
    void _Clear()
    {
      m_pMenuCursor = NULL;

      {
        u32 loopNum = GFL_NELEMS( m_aButtonPaneData );
        for( u32 i = 0; i < loopNum; ++i )
        {
          m_aButtonPaneData[ i ].buttonId = BUTTON_ID_MAX;
          m_aButtonPaneData[ i ].pNameTextBoxPane = NULL;
          m_aButtonPaneData[ i ].pCursorPane  = NULL;
          m_aButtonPaneData[ i ].seID = SMID_NULL;
        }
      }
    }

    BUTTON_PANE_DATA  m_aButtonPaneData[ BUTTON_OBJ_MAX ];
    ::app::tool::MenuCursor*  m_pMenuCursor;
  };

//
//  ここから先は BattleUIPokeListLowerDetailStatusSubButtonCtrl クラスの定義
//
  //  詳細ステータス画面の入力制御クラス
  //  わざ選択一覧状態中に処理する
  class BattleUIPokeListLowerDetailStatusSubButtonCtrl : public BattleUIPokeListLowerButtonCtrl
  {
  public:
    BattleUIPokeListLowerDetailStatusSubButtonCtrl() { _Clear(); }

    //  初期化
    virtual void Initialize( BattleUIPokeListLowerDraw* pRootView, ::app::ui::UIView* pActiveView, const s32 useLytWkId, gfl2::lyt::LytMultiResID* pMultiResId, ::app::util::Heap* pUtilHeap, ::app::tool::MenuCursor* pMenuCursor )
    {
      BattleUIPokeListLowerButtonCtrl::_SetBaseParam( pRootView, pActiveView, useLytWkId, pMultiResId, pUtilHeap );
      GFL_ASSERT( pMenuCursor );

      m_pMenuCursor = pMenuCursor;
      m_pMenuCursor->StartAnime();
    }

    //------------------------------------------------------------------
    /**
      * @brief   ステータス詳細Viewとの接続
      * @note
      *   接続する時にボタン生成もしている
      */
    //------------------------------------------------------------------
    void ConectDetailStatusView( BattleUIPokeListLowerDetailStatusSubView* pStatusView )
    {
      GFL_ASSERT( pStatusView );

      m_pDetailStatusView = pStatusView;

      //  ボタン管理が作成していなのであれば作成する
      if( m_pActiveView->GetButtonManager() == NULL )
      {
        gfl2::lyt::LytParts* pTokuseiInfoIconBtnParts = pStatusView->GetTokuseiInfoIconPartsPane();
        GFL_ASSERT( pTokuseiInfoIconBtnParts );

        gfl2::lyt::LytParts*  pItemInfoIconBtnParts = pStatusView->GetItemInfoIconPartsPane();
        GFL_ASSERT( pItemInfoIconBtnParts );

        //  ボタンオブジェクトのポインタ取得
        {
          gfl2::std::MemFill( m_pSkillButtonList, NULL, sizeof( m_pSkillButtonList ) );
          u32  loopNum = GFL_NELEMS( m_pSkillButtonList );
          for( u32 i = 0; i < loopNum; ++i )
          {
            m_pSkillButtonList[ i ] = pStatusView->GetSkillButtonPtr( i );
            GFL_ASSERT( m_pSkillButtonList[ i ] );
          }
        }

        //  ボタン設定データ定義一覧
        const ::app::ui::ButtonInfoEx    aButtonPaneIdxList[] =
        {
          //  各わざ項目のボタン
          m_pSkillButtonList[ 0 ]->GetPanelButtonInfoEx(),
          m_pSkillButtonList[ 1 ]->GetPanelButtonInfoEx(),
          m_pSkillButtonList[ 2 ]->GetPanelButtonInfoEx(),
          m_pSkillButtonList[ 3 ]->GetPanelButtonInfoEx(),

          //  特性インフォアイコンボタン
          {
            BUTTON_ID_TOKUSEI_INFO_ICON,
            pTokuseiInfoIconBtnParts,
            m_pLayoutWork->GetBoundingPane( pTokuseiInfoIconBtnParts, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, m_pMultiResId ),
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__INFO_00_TOUCH,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__INFO_00_RELEASE,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__INFO_00_CANSEL,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__INFO_00_TOUCH_RELEASE,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__INFO_00_ACTIVE,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__INFO_00_PASSIVE,
          },

          //  アイテムインフォアイコンボタン
          {
            BUTTON_ID_ITEM_INFO_ICON,
            pItemInfoIconBtnParts,
            m_pLayoutWork->GetBoundingPane( pItemInfoIconBtnParts, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, m_pMultiResId ),
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__INFO_01_TOUCH,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__INFO_01_RELEASE,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__INFO_01_CANSEL,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__INFO_01_TOUCH_RELEASE,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__INFO_01_ACTIVE,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__INFO_01_PASSIVE,
          },
        };

        //  ボタン登録
        m_pActiveView->ReleaseButtonManager();
        m_pActiveView->CreateButtonManager( m_pUtilHeap, m_pLayoutWork, aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );

        //  入力コールバック設定
        m_pActiveView->SetInputListener( this );

        //  SE設定
        if( m_bUseInputSE )
        {
          ::app::tool::ButtonManager* pButtonManager  = m_pActiveView->GetButtonManager();
          pButtonManager->SetButtonSelectSE( BUTTON_ID_ITEM_INFO_ICON, ::app::tool::ButtonManager::SE_NULL );
          pButtonManager->SetButtonSelectSE( BUTTON_ID_TOKUSEI_INFO_ICON, ::app::tool::ButtonManager::SE_NULL );
        
          {
            u32 loopNum = GFL_NELEMS( m_pSkillButtonList );
            for( u32 i = 0; i < loopNum; ++i )
            {
              pButtonManager->SetButtonSelectSE( m_pSkillButtonList[ i ]->GetPanelButtonInfoEx().button_id, SEQ_SE_DECIDE1 );
            }
          }
        }
      }
    }

    //  カーソル移動種類取得(わざ選択のカーソルリスト移動)
    virtual CursorMoveType GetCursorMoveTyoe() const { return CURSOR_MOVE_TYPE_LIST; }

    //  カーソル移動最大idx
    virtual s32 GetCursorMoveIdxMax()
    {
      s32 listMax = -1;
      u32 loopNum = GFL_NELEMS( m_pSkillButtonList );
      for( u32 i = 0; i < loopNum; ++i )
      {
        if( m_pSkillButtonList[ i ] == NULL )
        {
          break;
        }

        if( m_pSkillButtonList[ i ]->IsVisible() )
        {
          ++listMax;
        }
      }

      return  listMax;
    }

    //------------------------------------------------------------------
    /**
      * @brief   カレントカーソル項目を決定
      */
    //------------------------------------------------------------------
    virtual bool PushBtnCurrentCursor()
    {
      if( m_bInputEnable == false )
      {
        return  false;
      }

      GFL_ASSERT( m_cursorIdx <= GetCursorMoveIdxMax() );

      parts::BattleUISkillButtonPartsLayout*  pSkillButton  = m_pSkillButtonList[ m_cursorIdx ];
      GFL_ASSERT( pSkillButton );

      if( PushButtonId( (ButtonId)pSkillButton->GetPanelButtonInfoEx().button_id ) )
      {
        return  true;
      }

      return false;
    }

    //------------------------------------------------------------------
    /**
      * @brief   カーソル移動
      */
    //------------------------------------------------------------------
    virtual bool MoveCursorUp( const bool bLoop )
    {
      if( m_bInputEnable == false )
      {
        return  false;
      }

     // s32 oldCursorIdx  = m_cursorIdx;
      s32 nowCursorIdx  = m_cursorIdx;
      s32 listIdxMax    = GetCursorMoveIdxMax();

      --nowCursorIdx;

      if( nowCursorIdx < 0 )
      {
        if( bLoop )
        {
          nowCursorIdx = listIdxMax;
        }
        else
        {
          nowCursorIdx = 0;
        }
      }

      if( SetCursorMoveIdx( nowCursorIdx ) == true )
      {
        return  true;
      }

      return  false;
    }

    virtual bool MoveCursorDown( const bool bLoop )
    {
      if( m_bInputEnable == false )
      {
        return  false;
      }

      s32 oldCursorIdx  = m_cursorIdx;
      s32 nowCursorIdx  = m_cursorIdx;
      s32 listIdxMax    = GetCursorMoveIdxMax();

      ++nowCursorIdx;

      if( listIdxMax < nowCursorIdx )
      {
        if( bLoop )
        {
          nowCursorIdx = 0;
        }
        else
        {
          nowCursorIdx = oldCursorIdx;
        }
      }

      if( SetCursorMoveIdx( nowCursorIdx ) == true )
      {
        return  true;
      }

      return  false;
    }

    //------------------------------------------------------------------
    /**
      * @brief   ボタンの有効設定
      */
    //------------------------------------------------------------------
    virtual void SetEnableButtonAll( const bool bFlg )
    {
      BattleUIPokeListLowerButtonCtrl::SetEnableButtonAll( bFlg );

      m_pMenuCursor->SetVisible( bFlg );
    }

    //  カーソル位置設定
    virtual bool SetCursorMoveIdx( const s32 cursorIdx )
    {
      GFL_ASSERT( cursorIdx <= GetCursorMoveIdxMax() );
      GFL_ASSERT( cursorIdx < GFL_NELEMS( m_pSkillButtonList ) );

      parts::BattleUISkillButtonPartsLayout*  pSkillButton  = m_pSkillButtonList[ cursorIdx ];
      GFL_ASSERT( pSkillButton );

      const ::app::ui::ButtonInfoEx&  rButtonInfoEx = pSkillButton->GetPanelButtonInfoEx();

      if( m_pActiveView->GetButtonManager()->IsButtonVisible( rButtonInfoEx.button_id ) == false )
      {
        return  false;
      }

      m_cursorOldIdx  = m_cursorIdx;
      m_cursorIdx     = cursorIdx;

      // @fix GFNMCat[1082] カーソル非表示設定が事前に呼ばれることもあるので、ここでは位置変更のみ
      m_pMenuCursor->PutNonVisible( m_pLayoutWork, pSkillButton->GetCursorNullPane() );

      return  true;
    }

    //  空関数にして、連続でキーアクションが呼ばれないように対応
    virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick ) { return ENABLE_ACCESS; }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief ペインタッチ時を検知
    * @note 主な用途  RELEASE判定だが、決定前時にカーソルを移動したい、とか
    */
    //--------------------------------------------------------------------------------------------
    virtual void OnLayoutPaneTouchTriggerEvent( const u32 buttonId )
    {
      s32 max = GFL_NELEMS( m_pSkillButtonList );
      for( s32 i = 0; i < max; ++i )
      {
        if( m_pSkillButtonList[ i ]->GetPanelButtonInfoEx().button_id == buttonId )
        {
          SetCursorMoveIdx( i );
          return;
        }
      }
    }

  private:
    //  ボタン入力結果を出力
    virtual void _OutputInputResultData( BattleUIPokeListLowerDrawDelegate::RESULT_DATA* pResultData, const ButtonId pushButtonId )
    {
      GFL_ASSERT( m_pDetailStatusView != NULL );

      BattleUIPokeListLowerButtonCtrl::_OutputInputResultData( pResultData, pushButtonId );

      //  わざボタンならわざidを設定
      {
        const s32 aSkillButtonId[]  =
        {
          BUTTON_ID_SKILL_ITEM_00,
          BUTTON_ID_SKILL_ITEM_01,
          BUTTON_ID_SKILL_ITEM_02,
          BUTTON_ID_SKILL_ITEM_03,
        };

        parts::BattleUISkillButtonPartsLayout*  pSkillButtonPartsObj  = NULL;
        u32 loopNum = GFL_NELEMS( aSkillButtonId );
        for( u32 i = 0; i < loopNum; ++ i )
        {
          if( aSkillButtonId[ i ] == pushButtonId )
          {
            GFL_ASSERT( i < GFL_NELEMS( m_pSkillButtonList ) );

            pSkillButtonPartsObj  = m_pSkillButtonList[ i ];
            GFL_ASSERT( pSkillButtonPartsObj );

            pResultData->skill_data.id         = pSkillButtonPartsObj->GetId();
            pResultData->skill_data.nowPPNum   = pSkillButtonPartsObj->GetNowPPNum();
            pResultData->skill_data.maxPPNum   = pSkillButtonPartsObj->GetMaxPPNum();
            return;
          }
        }
      }

      //  特性のインフォボタンを押しているか
      if( pushButtonId == BUTTON_ID_TOKUSEI_INFO_ICON )
      {
        pResultData->select_tokusei_id    = m_pDetailStatusView->GetHaveTokuseiId();
        pResultData->select_info_icon_pos = m_pActiveView->GetButtonManager()->GetButtonPosByGfVec3( BUTTON_ID_TOKUSEI_INFO_ICON );

        return;
      }
      //  アイテムのインフォボタンを押しているか
      else
      if( pushButtonId == BUTTON_ID_ITEM_INFO_ICON )
      {
        pResultData->select_item_id       = m_pDetailStatusView->GetHaveItemId();
        pResultData->select_info_icon_pos = m_pActiveView->GetButtonManager()->GetButtonPosByGfVec3( BUTTON_ID_ITEM_INFO_ICON );

        return;
      }
    }

    //  変数初期化
    void _Clear()
    {
      m_pMenuCursor       = NULL;
      m_pDetailStatusView = NULL;
      gfl2::std::MemFill( m_pSkillButtonList, NULL, sizeof( m_pSkillButtonList ) );
    }

    ::app::tool::MenuCursor*                      m_pMenuCursor;
    parts::BattleUISkillButtonPartsLayout*        m_pSkillButtonList[ 4 ];
    BattleUIPokeListLowerDetailStatusSubView*     m_pDetailStatusView;
  };

//
//  ここから先は BattleUIPokeListLowerSelectPosSubButtonCtrl クラスの定義
//
  //  配置位置選択の入力制御クラス
  class BattleUIPokeListLowerSelectPosSubButtonCtrl : public BattleUIPokeListLowerButtonCtrl
  {
  public:
    BattleUIPokeListLowerSelectPosSubButtonCtrl()
    {
      _Clear();
    }

    //  初期化
    virtual void Initialize( BattleUIPokeListLowerDraw* pRootView, ::app::ui::UIView* pActiveView, const s32 useLytWkId, gfl2::lyt::LytMultiResID* pMultiResId, ::app::util::Heap* pUtilHeap, ::app::tool::MenuCursor* pMenuCursor )
    {
      BattleUIPokeListLowerButtonCtrl::_SetBaseParam( pRootView, pActiveView, useLytWkId, pMultiResId, pUtilHeap );
      GFL_ASSERT( pMenuCursor );

      m_pMenuCursor = pMenuCursor;
      m_pMenuCursor->StartAnime();

      ::app::util::G2DUtil* pG2DUtil  = pRootView->GetG2DUtil();
      //  ２つ項目選択に切り替え
      pG2DUtil->StartAnimeFrame( 0, LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000_POS_SELECT2, 0 );
      {
        // 強制アニメをして画面反映
#if defined( GF_PLATFORM_CTR )
        m_pLayoutWork->GetLayoutCore()->Animate();
#elif defined( GF_PLATFORM_WIN32 )
        m_pLayoutWork->GetLayoutCore()->AnimateAndUpdateAnimFrame();
#endif
      }

      //  ボタン作成

      //  Leftボタンに常に必要なデータを設定
      gfl2::lyt::LytParts*  pLeftButtonPartsPane  = NULL;
      BUTTON_PANE_DATA* pButtonData = NULL;

      {
        pLeftButtonPartsPane = m_pLayoutWork->GetPartsPane( PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_PARTSMSGBTN_01 );
        GFL_ASSERT( pLeftButtonPartsPane );

        pButtonData = &m_aButtonPaneData[ BUTTON_OBJ_LEFT ];

        pButtonData->buttonId     = BUTTON_ID_CHANGE_POS_LEFT;
        pButtonData->seId         = SEQ_SE_DECIDE1;
        pButtonData->pCursorPane  = m_pLayoutWork->GetPane( pLeftButtonPartsPane, PANENAME_POKELIST_BTN_LOW_000_PANE_NULL_CUR, pMultiResId );
        GFL_ASSERT( pButtonData->pCursorPane );
      }

      //  Rightボタンに常に必要なデータを設定
      gfl2::lyt::LytParts*  pRightButtonPartsPane  = NULL;
      {
        pRightButtonPartsPane = m_pLayoutWork->GetPartsPane( PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_PARTSMSGBTN_02 );
        GFL_ASSERT( pRightButtonPartsPane );

        pButtonData = &m_aButtonPaneData[ BUTTON_OBJ_RIGHT ];

        pButtonData->buttonId     = BUTTON_ID_CHANGE_POS_RIGHT;
        pButtonData->seId         = SEQ_SE_DECIDE1;
        pButtonData->pCursorPane  = m_pLayoutWork->GetPane( pRightButtonPartsPane, PANENAME_POKELIST_BTN_LOW_000_PANE_NULL_CUR, pMultiResId );
        GFL_ASSERT( pButtonData->pCursorPane );
      }

      //  ボタン設定データ定義一覧
      const ::app::ui::ButtonInfoEx    aButtonPaneIdxList[ BUTTON_OBJ_MAX ] =
      {
        //  左ボタン
        {
          m_aButtonPaneData[ BUTTON_OBJ_LEFT ].buttonId,
          pLeftButtonPartsPane,
          m_pLayoutWork->GetBoundingPane( pLeftButtonPartsPane, PANENAME_POKELIST_BTN_LOW_000_PANE_BOUND_00, pMultiResId ),
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_01_TOUCH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_01_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_01_CANSEL,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_01_TOUCH_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_01_ACTIVE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_01_PASSIVE,
        },

        //  右ボタン
        {
          m_aButtonPaneData[ BUTTON_OBJ_RIGHT ].buttonId,
          pRightButtonPartsPane,
          m_pLayoutWork->GetBoundingPane( pRightButtonPartsPane, PANENAME_POKELIST_BTN_LOW_000_PANE_BOUND_00, pMultiResId ),
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_02_TOUCH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_02_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_02_CANSEL,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_02_TOUCH_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_02_ACTIVE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTSMSGBTN_02_PASSIVE,
        },
      };

      //  ボタン登録
      pActiveView->ReleaseButtonManager();
      pActiveView->CreateButtonManager( pUtilHeap, m_pLayoutWork, aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );

      SetEnableButtonAll( true );

      //  表示を有効に
      {
        u32 loopNum = GFL_NELEMS(  m_aButtonPaneData );
        for( u32 i = 0; i < loopNum; ++i )
        {
          SetVisibleButton( m_aButtonPaneData[ i ].buttonId, true );
        }
      }

      //  ボタンテキスト設定
      {
        gfl2::lyt::LytTextBox*  pTextBoxPane  = NULL;

        //  左ボタン
        {
          pTextBoxPane  = m_pLayoutWork->GetTextBoxPane( pLeftButtonPartsPane, PANENAME_POKELIST_BTN_LOW_000_PANE_TEXTBOX_00, pMultiResId );
          GFL_ASSERT( pTextBoxPane );

          pG2DUtil->SetTextBoxPaneString( pTextBoxPane, str_btl_pos_select_L );
        }

        //  右ボタン
        {
          pTextBoxPane  = m_pLayoutWork->GetTextBoxPane( pRightButtonPartsPane, PANENAME_POKELIST_BTN_LOW_000_PANE_TEXTBOX_00, pMultiResId );
          GFL_ASSERT( pTextBoxPane );

          pG2DUtil->SetTextBoxPaneString( pTextBoxPane, str_btl_pos_select_R );
          
        }
      }

      //  SE設定
      if( m_bUseInputSE )
      {
        ::app::tool::ButtonManager* pButtonManager  = pActiveView->GetButtonManager();

        u32 loopNum = GFL_NELEMS( m_aButtonPaneData );
        for( u32 i = 0; i < loopNum; ++i )
        {
          pButtonManager->SetButtonSelectSE( m_aButtonPaneData[ i ].buttonId, m_aButtonPaneData[ i ].seId );
        }
      }

      //  入力コールバック設定
      m_pActiveView->SetInputListener( this );

      m_pMenuCursor->SetVisible( true );

      //  初回カーソル位置を指定
      if( m_cursorIdx == -1 )
      {
        SetCursorPos( 0, false );
      }
      else
      {
        SetCursorPos( m_cursorIdx, false );
      }
    }

    //  更新処理
    virtual void Update()
    {
    }

    //------------------------------------------------------------------
    /**
      * @brief   カレントカーソル項目を決定
      */
    //------------------------------------------------------------------
    virtual bool PushBtnCurrentCursor()
    {
      if( PushButtonId( m_aButtonPaneData[ m_cursorIdx ].buttonId ) )
      {
        return  true;
      }

      return false;
    }

    virtual bool MoveCursorLeft( const bool bLoop )
    {
      --m_cursorIdx;
      if( m_cursorIdx < 0 )
      {
        if( bLoop )
        {
          m_cursorIdx = 1;
        }
        else
        {
          m_cursorIdx = 0;
        }
      }

      if( SetCursorPos( m_cursorIdx, true ) )
      {
        return true;
      }

      return false;
    }

    virtual bool MoveCursorRight( const bool bLoop )
    {
      ++m_cursorIdx;
      if( 1 < m_cursorIdx )
      {
        if( bLoop )
        {
          m_cursorIdx = 0;
        }
        else
        {
          m_cursorIdx = 1;
        }
      }

      if( SetCursorPos( m_cursorIdx, true ) )
      {
        return true;
      }

      return false;
    }

    //  カーソル位置設定
    bool SetCursorPos( const s32 cursorIdx, const bool bUseMoveSE )
    {
      if( m_pActiveView->GetButtonManager()->IsButtonVisible( m_aButtonPaneData[ cursorIdx ].buttonId ) == false )
      {
        return  false;
      }

      m_cursorOldIdx  = m_cursorIdx;
      m_cursorIdx     = cursorIdx;

      m_pMenuCursor->Put( m_pLayoutWork, m_aButtonPaneData[ cursorIdx ].pCursorPane );

      return  true;
    }

    //  空関数にして、連続でキーアクションが呼ばれないように対応
    virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick ) { return ENABLE_ACCESS; }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief ペインタッチ時を検知
    * @note 主な用途  RELEASE判定だが、決定前時にカーソルを移動したい、とか
    */
    //--------------------------------------------------------------------------------------------
    virtual void OnLayoutPaneTouchTriggerEvent( const u32 buttonId )
    {
      s32 max = GFL_NELEMS( m_aButtonPaneData );
      for( s32 i = 0; i < max; ++i )
      {
        if( m_aButtonPaneData[ i ].buttonId == (s32)buttonId )
        {
          SetCursorPos( i, false );
          return;
        }
      }
    }

  private:
    enum ButtonObj
    {
      BUTTON_OBJ_LEFT  = 0,
      BUTTON_OBJ_RIGHT,

      BUTTON_OBJ_MAX
    };

    //  ボタンペインデータ
    typedef struct _tag_button_pane_data_
    {
      //  ボタンid
      ButtonId buttonId;

      //  カーソルペイン
      gfl2::lyt::LytPane*     pCursorPane;

      //  SEId
      u32                     seId;

    } BUTTON_PANE_DATA;

    //  変数初期化
    void _Clear()
    {
      m_pMenuCursor = NULL;

      {
        u32 loopNum = GFL_NELEMS( m_aButtonPaneData );
        for( u32 i = 0; i < loopNum; ++i )
        {
          m_aButtonPaneData[ i ].buttonId = BUTTON_ID_MAX;
          m_aButtonPaneData[ i ].pCursorPane  = NULL;
          m_aButtonPaneData[ i ].seId = SMID_NULL;
        }
      }
    }

    BUTTON_PANE_DATA  m_aButtonPaneData[ BUTTON_OBJ_MAX ];
    ::app::tool::MenuCursor*  m_pMenuCursor;
  };

 //
 // ここから先は BattleUIPokeListLowerDraw クラスを定義
 //
  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUIPokeListLowerDraw::BattleUIPokeListLowerDraw( const INIT_PARAM& param ) :
    UIView( param.pUtilHeap, param.pUtilHeap->GetDeviceAllocator(), param.pUtilHeap->GetDeviceAllocator() )
  {
    GFL_ASSERT( param.pUtilHeap );
    GFL_ASSERT( param.pRenderingManager );
    GFL_ASSERT( param.pLytBuff );
    GFL_ASSERT( param.pBtlAppMsgData );
    GFL_ASSERT( param.pBtlWordSet );

    _Clear();

    m_initParam = param;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUIPokeListLowerDraw::~BattleUIPokeListLowerDraw()
  {
    UIView::RemoveAllSubView();

    GFL_SAFE_DELETE( m_pBtnMenuCursor );
    GFL_SAFE_DELETE( m_pCommonMessageWindow );
    GFL_SAFE_DELETE( m_messageData.pBlankStrBuf );
    GFL_SAFE_DELETE( m_pActiveSubView );
    GFL_SAFE_DELETE( m_pButtonCtrl );

    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    */
  //------------------------------------------------------------------
  bool BattleUIPokeListLowerDraw::Initialize()
  {
    if( m_initSeq == 0 )
    {
      _Initialize2D( m_initParam.pLytBuff );
  #ifdef DEBUG_ONLY_FOR_uchida_yuto
        // ヒープ残量チェック
        m_initParam.pUtilHeap->GetSystemHeap()->PrintHeapInfo( "BattleUIPokeListFrame(LocalSystem) Initialize0\n" );
        m_initParam.pUtilHeap->GetDeviceHeap()->PrintHeapInfo( "BattleUIPokeListFrame(LocalDevice) Initialize0\n" );
  #endif

      UIView::SetVisible( false );

      //  ポケモン用カーソル初期化
      m_pokeObjectCursor.Initialize( this );

      ++m_initSeq;
    }

    if( m_initSeq == 1 )
    {
      //  ポケオブジェクトリスト作成
      u32 loopNum = GFL_NELEMS( m_aPokeObj );
      for( u32 i = 0; i < loopNum; ++i )
      {
        PokeObject::INIT_PARAM initParam;

        //  初期化データを出力
        _OutputPokeObjInitParam( &initParam, i );

        m_aPokeObj[ i ].Initialize( initParam, i );
      }

  #ifdef DEBUG_ONLY_FOR_uchida_yuto
        // ヒープ残量チェック
        m_initParam.pUtilHeap->GetSystemHeap()->PrintHeapInfo( "BattleUIPokeListFrame(LocalSystem) Initialize1\n" );
        m_initParam.pUtilHeap->GetDeviceHeap()->PrintHeapInfo( "BattleUIPokeListFrame(LocalDevice) Initialize1\n" );
  #endif

      ++m_initSeq;
    }

    if( m_initSeq == 2 )
    {
      bool  bNextSeq  = true;
      u32 loopNum = GFL_NELEMS( m_aPokeObj );
      for( u32 i = 0; i < loopNum; ++i )
      {
        if( m_aPokeObj[ i ].IsReady() == false )
        {
          bNextSeq  = false;
        }
      }

      if( bNextSeq == true )
      {
  #ifdef DEBUG_ONLY_FOR_uchida_yuto
        // ヒープ残量チェック
        m_initParam.pUtilHeap->GetSystemHeap()->PrintHeapInfo( "BattleUIPokeListFrame(LocalSystem) Initialize2\n" );
        m_initParam.pUtilHeap->GetDeviceHeap()->PrintHeapInfo( "BattleUIPokeListFrame(LocalDevice) Initialize2\n" );
  #endif

        ++m_initSeq;
      }
    }

    if( m_initSeq == 3 )
    {
      ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

      gfl2::lyt::LytWk* pLayoutWork = UIView::GetLayoutWork( LYT_WK_TYPE_SCREEN );
      GFL_ASSERT( pLayoutWork );

      gfl2::lyt::LytMultiResID& rMultiResID = pG2DUtil->GetLayoutResourceID( 0 );

      //  入力制御作成
      {
        m_pButtonCtrl  = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) BattleUIPokeListLowerButtonCtrl();
        m_pButtonCtrl->Initialize( this, this, LYT_WK_TYPE_SCREEN, &rMultiResID, m_initParam.pUtilHeap, NULL );
      }

  #ifdef DEBUG_ONLY_FOR_uchida_yuto
        // ヒープ残量チェック
        m_initParam.pUtilHeap->GetSystemHeap()->PrintHeapInfo( "BattleUIPokeListFrame(LocalSystem) Initialize3\n" );
        m_initParam.pUtilHeap->GetDeviceHeap()->PrintHeapInfo( "BattleUIPokeListFrame(LocalDevice) Initialize3\n" );
  #endif

      //  Start関数が呼ばれるまで非表示
      UIView::SetVisible( false );

      //  性別アイコンペイン取得
      {
        m_pSexIconPartsPane  = pLayoutWork->GetPartsPane( PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_PARTS_GENDER );
        GFL_ASSERT( m_pSexIconPartsPane );
      }

      //  レベルテキストボックスペイン取得
      {
        m_pLvIconTextBoxPane  = pLayoutWork->GetTextBoxPane( PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TEXTBOX_02 );
        GFL_ASSERT( m_pLvIconTextBoxPane );

        //  レベル表記設定
        pG2DUtil->SetTextBoxPaneString( m_pLvIconTextBoxPane, str_btl_poke_lv );

        m_pLvNumTextBoxPane   = pLayoutWork->GetTextBoxPane( PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TEXTBOX_01 );
        GFL_ASSERT( m_pLvNumTextBoxPane );
      }

      //  状態異常ペイン取得
      {
        gfl2::lyt::LytParts* pLytParts = pLayoutWork->GetPartsPane( PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_PARTS_SICK );
        GFL_ASSERT( pLytParts );

        m_pSickIconPicturePane  = pLayoutWork->GetPicturePane( pLytParts, PANENAME_COMMON_ICN_002_PANE_ICON_SICK, &rMultiResID );
        GFL_ASSERT( m_pSickIconPicturePane );
      }

      //  ポケモンタイプアイコンペイン取得
      {
        const s32 aPaneIdx[ POKE_TYPE_MAX ]  =
        {
          PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_PARTSTYPE_00,
          PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_PARTSTYPE_01,
        };

        GFL_ASSERT( GFL_NELEMS( aPaneIdx ) == GFL_NELEMS( m_pPokeTypeIconPicPaneList ) );

        gfl2::lyt::LytParts*  pPartsPane  = NULL;
        for( s32 i = 0; i < GFL_NELEMS( aPaneIdx ); ++i )
        {
          pPartsPane  = pLayoutWork->GetPartsPane( aPaneIdx[ i ] );
          GFL_ASSERT( pPartsPane );

          m_pPokeTypeIconPicPaneList[ i ]  = pLayoutWork->GetPicturePane( pPartsPane, PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE, &rMultiResID );
          GFL_ASSERT( m_pPokeTypeIconPicPaneList[ i ] );
        }
      }

      //  メッセージウィンドウ作成
      {
        m_pCommonMessageWindow = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) App::Tool::CommonMessageWindowManipulator();
        App::Tool::CommonMessageWindowManipulator::SSetupInfo  setupInfo;
        // 設定内容を決める
        {
          setupInfo.layoutWorkID        = LYT_WK_TYPE_SCREEN;

          setupInfo.paneID_MsgCursor    = gfl2::lyt::LYTSYS_PANEINDEX_ERROR;
          setupInfo.paneID_NullText1    = PANENAME_COMMON_MSG_LOW_000_PANE_NULL_TEXT1;
          setupInfo.paneID_TextBox1a    = PANENAME_COMMON_MSG_LOW_000_MATERIAL_TEXTBOX_1A_OF_TEXTBOX_1A;
          setupInfo.paneID_TextBox2     = PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2;
          setupInfo.paneID_TextBox2a    = PANENAME_COMMON_MSG_LOW_000_MATERIAL_TEXTBOX_1B_OF_TEXTBOX_1B;
          setupInfo.paneID_WindowParts  = PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_MAG_L2;
          setupInfo.pHandler            = this;
          setupInfo.pHeap               = m_initParam.pUtilHeap;
          setupInfo.pTargetView         = this;
        }

        m_pCommonMessageWindow->Setup( setupInfo );
        m_pCommonMessageWindow->SetFinishMode_Timer( EVENT_MESSAGE_SPEED_FRAME );
      }
  #ifdef DEBUG_ONLY_FOR_uchida_yuto
        // ヒープ残量チェック
        m_initParam.pUtilHeap->GetSystemHeap()->PrintHeapInfo( "BattleUIPokeListFrame(LocalSystem) Initialize4\n" );
        m_initParam.pUtilHeap->GetDeviceHeap()->PrintHeapInfo( "BattleUIPokeListFrame(LocalDevice) Initialize4\n" );
  #endif

      //  メッセージの空文字作成
      {
        m_messageData.pBlankStrBuf = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) gfl2::str::StrBuf( 1 * 3, m_initParam.pUtilHeap->GetDeviceHeap() );
        m_messageData.pBlankStrBuf->SetStr( L"" );
      }
  #ifdef DEBUG_ONLY_FOR_uchida_yuto
        // ヒープ残量チェック
        m_initParam.pUtilHeap->GetSystemHeap()->PrintHeapInfo( "BattleUIPokeListFrame(LocalSystem) Initialize5\n" );
        m_initParam.pUtilHeap->GetDeviceHeap()->PrintHeapInfo( "BattleUIPokeListFrame(LocalDevice) Initialize5\n" );
  #endif

      //  ヘルプメッセージテキストペイン取得
      {
        gfl2::lyt::LytParts*  pMsgWndLytPartPane  = pLayoutWork->GetPartsPane( PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_MSG_L1 );
        GFL_ASSERT( pMsgWndLytPartPane );

        m_pHelpTextPane = pLayoutWork->GetTextBoxPane( pMsgWndLytPartPane, PANENAME_COMMON_MSG_LOW_001_PANE_TEXTBOX_00, &rMultiResID );
        GFL_ASSERT( m_pHelpTextPane );
      }

      //  ボタンのメニューカーソル作成
      {
        m_pBtnMenuCursor = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) ::app::tool::MenuCursor( m_initParam.pUtilHeap, &m_initParam.pMenuCursorLytBuff );
        m_pBtnMenuCursor->CreateLayoutWork( pG2DUtil->GetLayoutSystem(), pG2DUtil->GetLayoutWorkSetup( gfl2::lyt::DISPLAY_LOWER ), pG2DUtil->GetAppLytAccessor(), ::app::tool::MenuCursor::SIZE_SMALL );

        m_pBtnMenuCursor->SetVisible( false );
      }
  #ifdef DEBUG_ONLY_FOR_uchida_yuto
        // ヒープ残量チェック
        m_initParam.pUtilHeap->GetSystemHeap()->PrintHeapInfo( "BattleUIPokeListFrame(LocalSystem) Initialize6\n" );
        m_initParam.pUtilHeap->GetDeviceHeap()->PrintHeapInfo( "BattleUIPokeListFrame(LocalDevice) Initialize6\n" );
  #endif

      //  背景のループアニメ再生
      {
        pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__BG_00_BG_LOOP_00 );
      }

      ++m_initSeq;
    }

    if( m_initSeq == 4 )
    {
      return  true;
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    */
  //------------------------------------------------------------------
  void BattleUIPokeListLowerDraw::Terminate()
  {
    {
      u32 loopNum = GFL_NELEMS( m_aPokeObj );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_aPokeObj[ i ].Terminate();
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   削除可能か
    */
  //------------------------------------------------------------------
  bool BattleUIPokeListLowerDraw::IsDelete()
  {
    bool  bRet  = true;

    {
      u32 loopNum = GFL_NELEMS( m_aPokeObj );
      for( u32 i = 0; i < loopNum; ++i )
      {
        if( m_aPokeObj[ i ].IsDelete() == false )
        {
          bRet  = false;
        }
      }
    }

    {
      if( m_pBtnMenuCursor->DeleteLayoutWork() == false )
      {
        bRet  = false;
      }
    }

    if( UIView::IsDrawing() == true )
    {
      bRet  = false;
    }

    return  bRet;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 更新処理
    */
  //-------------------------------------------------------------------------
  void BattleUIPokeListLowerDraw::Update( void )
  {
    _UpdateState();

    {
      u32 loopNum = GFL_NELEMS( m_aPokeObj );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_aPokeObj[ i ].Update();
      }
    }

    m_pokeObjectCursor.Update();
    m_pCommonMessageWindow->UpdateWindow();

    UIView::Update();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 描画
    */
  //-------------------------------------------------------------------------
  void BattleUIPokeListLowerDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    for( s32 i = 0; i < LYT_WK_TYPE_MAX; ++i )
    {
      pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderingManager, displayNo, i, LOW_VIEW_ORDER_OTHER );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   メッセージイベント中か
    */
  //------------------------------------------------------------------
  bool BattleUIPokeListLowerDraw::IsProcEventMesssage()
  {
    if( m_updateSubState == UPDATE_SUB_STATE_MSG_OPEN )
    {
      return  true;
    }
    else
    if( m_updateSubState == UPDATE_SUB_STATE_MSG_UPDATE )
    {
      return  true;
    }
    else
    if( m_updateSubState == UPDATE_SUB_STATE_MSG_CLOSE )
    {
      return  true;
    }

    return  false;

  }

  //------------------------------------------------------------------
  /**
    * @brief   ViewModeを変更
    */
  //------------------------------------------------------------------
  void BattleUIPokeListLowerDraw::SetViewMode( const ViewMode viewMode )
  {
    PokeObject* pSelectPokeObj  = _GetSelectPokeObjPtr();

    BattleUIPokeListLowerButtonCtrl* pSubInput = m_pButtonCtrl->GetSubInput();

    //  前の画面がコマンド選択なら選択しているリストidxを保持
    if( m_viewMode == VIEW_MODE_CMD_SELECT )
    {
      if( pSubInput && pSelectPokeObj )
      {
        m_cmdViewCurrentCursorIdx = pSubInput->GetCursorMoveIdx();
      }
    }
    //  前の画面がステータスビューなら選択しているスキルリストidxを保存
    else
    if( m_viewMode == VIEW_MODE_STATUS )
    {
      if( pSubInput && pSelectPokeObj )
      {
        pSelectPokeObj->m_skillSelectIdx  = pSubInput->GetCursorMoveIdx();
      }
    }

    m_viewMode  = viewMode;

    //  前のサブViewは外す
    UIView::RemoveAllSubView();

    //  切り替え前のサブビューと入力があれば終了を呼ぶ(絶対に一緒に開放しないとハング地獄になる！！)
    {
      if( m_pActiveSubView != NULL )
      {
        m_pActiveSubView->End();
      }
      GFL_SAFE_DELETE( m_pActiveSubView );

      m_pButtonCtrl->DeleteSubInput();
    }

    BattleUIPokeListLowerButtonCtrl*  pBaseSubInput = NULL;

    //  コマンド選択ビューに
    if( viewMode == VIEW_MODE_CMD_SELECT )
    {
      //  ポケモンリスト有効
      _SetEnablePokeList( true );

      m_pActiveSubView  = GFL_NEW_LOW( m_initParam.pUtilHeap->GetDeviceHeap() ) BattleUIPokeListLowerSimpleStatusSubView( this, LYT_WK_TYPE_SCREEN, m_pBtnMenuCursor );
      m_pActiveSubView->Initialize();

      // 設定したカーソルのポインタは指定したm_pActiveSubViewと一連托生なのでm_pActiveSubViewを開放するとカーソルも消えるので注意
      BattleUIPokeListLowerSimpleStatusSubButtonCtrl*  pSubButton = m_pButtonCtrl->CreateSubButtonCtrlInst<BattleUIPokeListLowerSimpleStatusSubButtonCtrl>( m_pActiveSubView, m_pActiveSubView->GetMenuCursorPtr() );
      pBaseSubInput = pSubButton;

      pSubButton->SetCursorMoveIdx( m_cmdViewCurrentCursorIdx );
    }
    //  ステータスビューに
    else
    if( viewMode == VIEW_MODE_STATUS )
    {
      //  ポケモンリスト有効
      _SetEnablePokeList( true );

      BattleUIPokeListLowerDetailStatusSubView* pSubView = GFL_NEW_LOW( m_initParam.pUtilHeap->GetDeviceHeap() ) BattleUIPokeListLowerDetailStatusSubView( this, LYT_WK_TYPE_SCREEN, m_pBtnMenuCursor );
      m_pActiveSubView  = pSubView;
      m_pActiveSubView->Initialize();

      // 設定したカーソルのポインタは指定したm_pActiveSubViewと一連托生なのでm_pActiveSubViewを開放するとカーソルも消えるので注意
      BattleUIPokeListLowerDetailStatusSubButtonCtrl*  pSubButtonInputCtrl  = m_pButtonCtrl->CreateSubButtonCtrlInst<BattleUIPokeListLowerDetailStatusSubButtonCtrl>( m_pActiveSubView, m_pActiveSubView->GetMenuCursorPtr() );
      pBaseSubInput = pSubButtonInputCtrl;

      //  スタータス詳細View専用の接続
      pSubButtonInputCtrl->ConectDetailStatusView( pSubView );

      //  カーソル位置指定
      {
        pSubButtonInputCtrl->SetCursorMoveIdx( pSelectPokeObj->m_skillSelectIdx );
      }
    }
    //  ポケモン配置選択ビューに
    else
    if( viewMode == VIEW_MODE_POKE_POS_SELECT )
    {
      //  ポケモンリスト無効
      _SetEnablePokeList( false );

      m_pActiveSubView  = GFL_NEW_LOW( m_initParam.pUtilHeap->GetDeviceHeap() ) BattleUIPokeListLowerSimpleStatusSubView( this, LYT_WK_TYPE_SCREEN, m_pBtnMenuCursor );
      m_pActiveSubView->Initialize();

      BattleUIPokeListLowerSelectPosSubButtonCtrl* pSubButton = m_pButtonCtrl->CreateSubButtonCtrlInst<BattleUIPokeListLowerSelectPosSubButtonCtrl>( m_pActiveSubView, m_pActiveSubView->GetMenuCursorPtr() );
      pSubButton->SetCursorPos( 0, false );

      pBaseSubInput = pSubButton;

      //  ヘルプメッセージ設定
      ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
      pG2DUtil->SetTextBoxPaneString( m_pHelpTextPane, str_btl_pos_00 );
    }

    UIView::AddSubView( m_pActiveSubView );

    m_pActiveSubView->Start( pBaseSubInput );
  }

  //------------------------------------------------------------------
  /**
    * @brief   ボタンアニメ終了しているか
    */
  //------------------------------------------------------------------
  bool BattleUIPokeListLowerDraw::IsButtonEndAnime()
  {
    bool bFlg = ( UIResponder::IsButtonAnimation() == false );

    if( ( m_pActiveSubView != NULL ) && bFlg )
    {
      bFlg = ( m_pActiveSubView->IsButtonAnimation() == false );
    }

    return  bFlg;
  }

  //------------------------------------------------------------------
  /**
    * @brief   イベントメッセージを開始
    */
  //------------------------------------------------------------------
  //  
  void BattleUIPokeListLowerDraw::EventMessage( gfl2::str::StrBuf* pMessageStrBuf )
  {
    GFL_ASSERT( pMessageStrBuf );
    m_messageData.pStrBuf = pMessageStrBuf;

    _SetUpdateSubState( UPDATE_SUB_STATE_MSG_OPEN );
  }

  //------------------------------------------------------------------
  /**
    * @brief   参照しているポケモンパラメータを見て描画更新
    * @note
    *   アニメで画面が切り替わるので、切り替わった瞬間での更新では表示のON/OFFでの反映がうまくいかない
    */
  //------------------------------------------------------------------
  void BattleUIPokeListLowerDraw::UpdateCurrentPokeStatusView( const s32 currentListIdx )
  {
    GFL_ASSERT( currentListIdx < GFL_NELEMS( m_aPokeObj ) );
    GFL_ASSERT( 0 <= currentListIdx );
    GFL_ASSERT( m_pActiveSubView );

    if( m_pActiveSubView == NULL )
    {
      return;
    }

    PokeObject*  pCurrentPokeObj = &m_aPokeObj[ currentListIdx ];

    //  サブビューの情報更新
    {
      m_pActiveSubView->SetPokeInfoData( pCurrentPokeObj );
      //  詳細画面時にはカーソル位置を修正しないとわざの所持個数がポケモンごとに異なるとカーソル位置がおかしくなる
      if( m_viewMode == VIEW_MODE_STATUS )
      {
        //  カーソルはポケモンデータごとに異なるので再設定が必要
        BattleUIPokeListLowerButtonCtrl*  pSubButtonInputCtrl  = m_pButtonCtrl->GetSubInput();
        if( pSubButtonInputCtrl != NULL )
        {
          //  新しいポケモンオブジェクトが保持しているわざ選択カーソル位置を指定
          pSubButtonInputCtrl->SetCursorMoveIdx( pCurrentPokeObj->m_skillSelectIdx );
        }
      }
    }

    //  どの画面でも共通して表示するパラメータを設定
    {
      const PokeObject::INIT_PARAM& rPokeObjectParam  = pCurrentPokeObj->GetInitParam();

      const BTL_POKEPARAM*  pBtlPokeParam = rPokeObjectParam.pBtlParam;

      ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

      const pml::pokepara::PokemonParam*  pOrgPokeParam = pBtlPokeParam->GetSrcData();

      //  ポケモン名を表示
      {
        // @fix GFNMCat[1169] タマゴの時はニックネームに名前が入っているので、そのまま使う
        pG2DUtil->SetRegisterPokeNickName( 0, pOrgPokeParam );
        pG2DUtil->SetTextBoxPaneStringExpand( BattleUIPokeListLowerDraw::LYT_WK_TYPE_SCREEN, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_TEXTBOX_00, (u32)STR_POKE_NAME );
      }

      if( pOrgPokeParam->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )
      {
        //  卵なら名前以外は非表示
        pG2DUtil->SetPaneVisible( m_pSexIconPartsPane, false );
        pG2DUtil->SetPaneVisible( m_pSickIconPicturePane, false );

        u32 loopNum = GFL_NELEMS( m_pPokeTypeIconPicPaneList );
        for( u32 i = 0; i < loopNum; ++i )
        {
          pG2DUtil->SetPaneVisible( m_pPokeTypeIconPicPaneList[ i ], false );
        }

        pG2DUtil->SetPaneVisible( m_pLvIconTextBoxPane, false );
        pG2DUtil->SetPaneVisible( m_pLvNumTextBoxPane, false );

        return;
      }

      //  性別表示
      {
        pG2DUtil->SetPaneVisible( m_pSexIconPartsPane, true );

        pml::Sex  nowSex  = PokeTool::GetDrawSex( pOrgPokeParam );
        if( nowSex == pml::SEX_MALE )
        {
          //  オス
          pG2DUtil->StartAnime( BattleUIPokeListLowerDraw::LYT_WK_TYPE_SCREEN, LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTS_GENDER_ICON_MALE );
        }
        else
        if( nowSex == pml::SEX_FEMALE )
        {
          //  メス
          pG2DUtil->StartAnime( BattleUIPokeListLowerDraw::LYT_WK_TYPE_SCREEN, LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTS_GENDER_ICON_FEMALE );
        }
        else
        {
          //  それ以外
          pG2DUtil->SetPaneVisible( m_pSexIconPartsPane, false );
        }
      }

      //  レベル数表示
      {
        pG2DUtil->SetPaneVisible( m_pLvIconTextBoxPane, true );

        pG2DUtil->SetPaneVisible( m_pLvNumTextBoxPane, true );
        pG2DUtil->SetTextBoxPaneNumber( m_pLvNumTextBoxPane, STR_VALUE_3, pOrgPokeParam->GetLevel(), 3 );
      }

      //  状態異常表示
      {
        PokeSick  pokeSick  = pBtlPokeParam->GetPokeSick();
        bool  bSickVisible  = true;

        //  ひんし
        if( pBtlPokeParam->IsDead() )
        {
          //  UVテクスチャーの差し替え
          m_initParam.pAppCommonGrpIconData->ReplacePaneTextureByPokeHinshiIcon( m_pSickIconPicturePane );
        }
        else
        //  状態異常: 猛毒
        if( pBtlPokeParam->CheckMoudoku() == true )
        {
          //  UVテクスチャーの差し替え
          m_initParam.pAppCommonGrpIconData->ReplacePaneTextureByPokeDokudokuIcon( m_pSickIconPicturePane );
        }
        else
        //  状態異常
        if( pokeSick != pml::pokepara::SICK_NULL )
        {
          //  UVテクスチャーの差し替え
          m_initParam.pAppCommonGrpIconData->ReplacePaneTextureByPokeSickIcon( pokeSick, m_pSickIconPicturePane );
        }
        else
        //  ポケルス感染
        if( pBtlPokeParam->HavePokerus() == true )
        {
          //  UVテクスチャーの差し替え
          m_initParam.pAppCommonGrpIconData->ReplacePaneTextureByPokerusIcon( m_pSickIconPicturePane );
        }
        else
        {
          bSickVisible  = false;
        }

        if( bSickVisible == true )
        {
          pG2DUtil->StartAnime( BattleUIPokeListLowerDraw::LYT_WK_TYPE_SCREEN, LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTS_SICK_ACTIVE );
        }
        else
        {
          pG2DUtil->StartAnime( BattleUIPokeListLowerDraw::LYT_WK_TYPE_SCREEN, LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTS_SICK_PASSIVE );
        }

        pG2DUtil->SetPaneVisible( m_pSickIconPicturePane, bSickVisible );
      }

      //  ポケモンタイプアイコン表示
      {
        //  タイプ１
        pml::PokeType type1 = POKETYPE_NULL;
        pml::PokeType type2 = POKETYPE_NULL;

        //  タイプ出力
        ::app::util::OutputPokemonType( &type1, &type2, pOrgPokeParam );
        {
          if( type1 == POKETYPE_NULL )
          {
            //  タイプがない
            pG2DUtil->SetPaneVisible( m_pPokeTypeIconPicPaneList[ 0 ], false );
          }
          else
          {
            pG2DUtil->SetPaneVisible( m_pPokeTypeIconPicPaneList[ 0 ], true );
            m_initParam.pAppCommonGrpIconData->ReplacePaneTextureByWazaTypeIcon( (PokeType_tag)type1, m_pPokeTypeIconPicPaneList[ 0 ] );
          }
        }

        //  タイプ２
        {
          //  タイプ１で得たのと同じのであればタイプ２はないとしている(sango参照)
          if( type2 == POKETYPE_NULL )
          {
            //  タイプがない
            pG2DUtil->SetPaneVisible( m_pPokeTypeIconPicPaneList[ 1 ], false );
          }
          else
          {
            pG2DUtil->SetPaneVisible( m_pPokeTypeIconPicPaneList[ 1 ], true );
            m_initParam.pAppCommonGrpIconData->ReplacePaneTextureByWazaTypeIcon( (PokeType_tag)type2, m_pPokeTypeIconPicPaneList[ 1 ] );
          }
        }
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   レイアウトワークを生成する
    */
  //------------------------------------------------------------------
  void BattleUIPokeListLowerDraw::_Initialize2D( void* pAppLytBuff )
  {
    //  2D画面作成
    {
      // アーカイブIDテーブル
      const ::app::util::G2DUtil::LytResData resTbl[] =
      {
        {  pAppLytBuff, 32, ::app::util::G2DUtil::ATTACH_NONE, },
      };

      //  作成するレイアウトワーク
      const ::app::util::G2DUtil::LytwkData setupData[ LYT_WK_TYPE_MAX ] =
      {
          //  選択画面
          {
            0,
            LYTRES_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000_BFLYT,
            LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000___NUM,
            pAppLytBuff,
            true,
            ::app::util::G2DUtil::SETUP_LOWER,
            true
          },
      };

      UIView::Create2D( m_initParam.pUtilHeap, NULL, 32, resTbl, GFL_NELEMS(resTbl), setupData, GFL_NELEMS(setupData), m_initParam.pBtlAppMsgData, m_initParam.pBtlWordSet );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void BattleUIPokeListLowerDraw::_UpdateState()
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    if( m_state == STATE_TYPE_NONE )
    {
    }

    if( m_state == STATE_TYPE_IN )
    {
      _SetState( STATE_TYPE_UPDATE );
    }

    if( m_state == STATE_TYPE_UPDATE )
    {
      m_pButtonCtrl->Update();

      if( m_updateSubState == UPDATE_SUB_STATE_NORMAL )
      {

      }

      if( m_updateSubState == UPDATE_SUB_STATE_MSG_OPEN )
      {
        if( pG2DUtil->IsAnimeEnd( LYT_WK_TYPE_SCREEN, LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__MAG_L2_IN ) )
        {
          _SetUpdateSubState( UPDATE_SUB_STATE_MSG_UPDATE );
        }
      }

      if( m_updateSubState == UPDATE_SUB_STATE_MSG_UPDATE )
      {
        if( m_bFinishCommomMessageWindow == true )
        {
          _SetUpdateSubState( UPDATE_SUB_STATE_MSG_CLOSE );
        }
      }

      if( m_updateSubState == UPDATE_SUB_STATE_MSG_CLOSE )
      {
        if( pG2DUtil->IsAnimeEnd( LYT_WK_TYPE_SCREEN, LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__MAG_L2_OUT ) )
        {
          _SetUpdateSubState( UPDATE_SUB_STATE_NORMAL );
        }
      }
    }

    if( m_state == STATE_TYPE_OUT )
    {
      _SetState( STATE_TYPE_END );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void BattleUIPokeListLowerDraw::_SetState( const BattleUIPokeListLowerDraw::StateType newStateType )
  {
    GFL_ASSERT( m_state != newStateType );

    m_state = newStateType;

    if( m_state == STATE_TYPE_NONE )
    {
    }
    else
    if( m_state == STATE_TYPE_IN )
    {
      UIView::SetVisible( true );
    }
    else
    if( m_state == STATE_TYPE_UPDATE )
    {
      
    }
    else
    if( m_state == STATE_TYPE_OUT )
    {
      //  カーソルを非表示
      if( m_pActiveSubView )
      {
        m_pActiveSubView->GetMenuCursorPtr()->SetVisible( false );
      }
    }
    else
    if( m_state == STATE_TYPE_END )
    {
      UIView::SetInputEnabled( false );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   更新時のサブステート
    */
  //------------------------------------------------------------------
  void BattleUIPokeListLowerDraw::_SetUpdateSubState( const UpdateSubState newState )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    m_updateSubState  = newState;

    if( newState == UPDATE_SUB_STATE_NORMAL )
    {
      pG2DUtil->SetPaneVisible( LYT_WK_TYPE_SCREEN, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_MAG_L2, false );
    }
    else
    if( newState == UPDATE_SUB_STATE_MSG_OPEN )
    {
      pG2DUtil->SetPaneVisible( LYT_WK_TYPE_SCREEN, PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_MAG_L2, true );
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__MAG_L2_IN );

      m_bFinishCommomMessageWindow  = false;
      m_pCommonMessageWindow->ShowMessage( m_messageData.pStrBuf );
    }
    else
    if( newState == UPDATE_SUB_STATE_MSG_UPDATE )
    {
    }
    else
    if( newState == UPDATE_SUB_STATE_MSG_CLOSE )
    {
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__MAG_L2_OUT );
    }
  }

  //  ポケオブジェクトリストのアクティブ設定
  void BattleUIPokeListLowerDraw::SetActivePokeObjList( const s32 listIdx )
  {
    if( m_bEnablePokeObjList == false ) { return; }

    GFL_ASSERT( listIdx < GFL_NELEMS( m_aPokeObj ) );
    GFL_ASSERT( 0 <= listIdx );

    //  切り替え前のカレントポケモンオブジェクト取得
    PokeObject*  pOldPokeObj  = _GetSelectPokeObjPtr();

    //  ポケモンリストの中のカレントをアクティブ対象に
    {
      s32 loopNum = GFL_NELEMS( m_aPokeObj );
      bool  bSetActive  = false;

      for( s32 i = 0; i < loopNum; ++i )
      {
        if( i == listIdx )
        {
          if( bSetActive == false )
          {
            if( m_aPokeObj[ i ].SetAcitve( true, &m_pokeObjectCursor ) == true )
            {
              //  詳細画面時には切り替え前のカーソルを切り替え前のポケモンデータが保管
              if( m_viewMode == VIEW_MODE_STATUS )
              {
                //  カーソルはポケモンデータごとに異なるので再設定が必要
                BattleUIPokeListLowerButtonCtrl*  pSubButtonInputCtrl  = m_pButtonCtrl->GetSubInput();
                if( pSubButtonInputCtrl != NULL )
                {
                  //  変更前のポケモンデータにわざ選択のカーソル位置を保管する
                  if( pOldPokeObj != NULL )
                  {
                    pOldPokeObj->m_skillSelectIdx = pSubButtonInputCtrl->GetCursorMoveIdx();
                  }
                }
              }

              bSetActive  = true;
            }
          }
        }
        else
        {
          m_aPokeObj[ i ].SetAcitve( false, NULL );
        }
      }
    }

    //  メッセージウィンドウ処理中の時もあるので、切り替わった段階で終わらせる
    if( _IsMessageProc() )
    {
      _SetUpdateSubState( UPDATE_SUB_STATE_NORMAL );
    }

    //  ビューの表示を更新
    UpdateCurrentPokeStatusView( listIdx );
  }

  //  ポケオブジェクトリスト内で選択しているポケモン項目データを取得
  const BattleUIPokeListLowerDraw::POKE_SELECT_ITEM_PARAM* BattleUIPokeListLowerDraw::GetSelectPokeObjDataPtr()
  {
    PokeObject* pPokeObj  = _GetSelectPokeObjPtr();
    if( pPokeObj != NULL )
    {
      return &m_initParam.aPokeDataList[ pPokeObj->m_itemIdx ];
    }

    return  NULL;
  }

  //  メッセージ処理中か
  bool BattleUIPokeListLowerDraw::_IsMessageProc()
  {
    if( m_updateSubState == UPDATE_SUB_STATE_MSG_OPEN )
    {
      return  true;
    }
    else
    if( m_updateSubState == UPDATE_SUB_STATE_MSG_UPDATE )
    {
      return  true;
    }
    else
    if( m_updateSubState == UPDATE_SUB_STATE_MSG_CLOSE )
    {
      //  すでに閉じようとして終わろうとしているので、処理中とは見ない
    }

    return  false;
  }

  //  ポケモンアイコンのボタン設定データ作成
  const ::app::ui::ButtonInfoEx BattleUIPokeListLowerDraw::CreatePokeIconButtonInfoEx( const s32 idx, const BattleUIPokeListLowerButtonCtrl::ButtonId buttonId )
  {
    GFL_ASSERT( idx < GFL_NELEMS( m_aPokeObj ) );
    return  m_aPokeObj[ idx ].CreateButtonInfoEx( buttonId );
  }

  //------------------------------------------------------------------
  /**
    * @brief   汎用メッセージのコールバッグ
    */
  //------------------------------------------------------------------
  void  BattleUIPokeListLowerDraw::CommonMessageWindowManipulator_OnEvent( const App::Tool::CommonMessageWindowManipulator::EventID event )
  {
    if( event == App::Tool::CommonMessageWindowManipulator::EVENT_OnFinish )
    {
      m_bFinishCommomMessageWindow  = true;
    }
  }

  //  ポケオブジェクトリスト内で選択しているポケモンオブジェクト取得
  BattleUIPokeListLowerDraw::PokeObject* BattleUIPokeListLowerDraw::_GetSelectPokeObjPtr()
  {
    u32 max = GFL_NELEMS( m_aPokeObj );
    for( u32 i = 0; i < max; ++i )
    {
      if( m_aPokeObj[ i ].IsActive() )
      {
        return &m_aPokeObj[ i ];
      }
    }

    return  NULL;
  }

  //  ポケモンリストの有効設定
  void BattleUIPokeListLowerDraw::_SetEnablePokeList( const bool bFlg )
  {
    m_bEnablePokeObjList = bFlg;

    u32 loopNum = GFL_NELEMS( m_aPokeObj );
    for( u32 i = 0; i < loopNum; ++i )
    {
      m_aPokeObj[ i ].SetVisible( bFlg );
      m_aPokeObj[ i ].SetEnable( bFlg );
    }

    //  カーソルの表示設定
    if( bFlg == true )
    {
      if( m_pokeObjectCursor.IsActive() == false )
      {
        m_pokeObjectCursor.SetActive( bFlg );
      }
    }
    else
    {
      if( m_pokeObjectCursor.IsActive() == true )
      {
        m_pokeObjectCursor.SetActive( bFlg );
      }
    }
    
  }

  //  PokeObjのINIT_PARAM生成
  void BattleUIPokeListLowerDraw::_OutputPokeObjInitParam( PokeObject::INIT_PARAM* pOutputData, const s32 listIdx )
  {
    GFL_ASSERT( pOutputData );
    GFL_ASSERT( listIdx < GFL_NELEMS( m_aPokeObj ) );

    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    gfl2::lyt::LytWk* pLayoutWork   = pG2DUtil->GetLayoutWork( BattleUIPokeListLowerDraw::LYT_WK_TYPE_SCREEN );

    pOutputData->pUIView    = this;

    pOutputData->pUtilHeap  = m_initParam.pUtilHeap;
    GFL_ASSERT( pOutputData->pUtilHeap );

    pOutputData->pokeType   = m_initParam.aPokeDataList[ listIdx ].type;

    pOutputData->pBtlParam  = m_initParam.aPokeDataList[ listIdx ].pParam;

    const BTL_POKEPARAM*  pBtlParam = pOutputData->pBtlParam;
    if( pOutputData->pBtlParam != NULL )
    {
      // @fix GFNMCat[3155] 変身前のオリジナルの技を設定
      //  技関連のデータ取得
      {
        u8  wazaCount           = pBtlParam->WAZA_GetCount_Org();
        GFL_ASSERT( wazaCount <= GFL_NELEMS( pOutputData->aWazaParam ) );

        //  あらかじめ技名は作成しておく、ポケモン切り替わるたびに発生するロード時間をカットするため
        for( u8 i = 0; i < wazaCount; ++i )
        {
          GFL_ASSERT( pOutputData->aWazaParam[ i ].pWazaNameBuffer == NULL );

          // @fix NMCat[496] 技情報はバトル用のポケパラメータでないとPP反映できない

          pOutputData->aWazaParam[ i ].id     = pBtlParam->WAZA_GetID_Org( i );
          pOutputData->aWazaParam[ i ].nowPP  = pBtlParam->WAZA_GetPP_Org( i );
          pOutputData->aWazaParam[ i ].maxPP  = pBtlParam->WAZA_GetMaxPP_Org( i );
          pOutputData->aWazaParam[ i ].pWazaNameBuffer = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) gfl2::str::StrBuf( 8 * 3, m_initParam.pUtilHeap->GetDeviceHeap() );

          pG2DUtil->SetRegisterWazaName( 0, pOutputData->aWazaParam[ i ].id );
          pG2DUtil->GetStringExpand( pOutputData->aWazaParam[ i ].pWazaNameBuffer, STR_WAZA_NAME );
        }

        pOutputData->wazaCount  = wazaCount;
      }

      //  特性
      {
        // @fix NMCat[234] 変更前の元の特性を表示
        // @fix NMCat[1845] イリュージョンしたデータから特性を取得していたのを元ポケモンパラメータからに変更
        TokuseiNo tokuseiId = pBtlParam->GetSrcData()->GetTokuseiNo();
        if( tokuseiId == TOKUSEI_NULL )
        {
        }
        else
        {
          //  特性名作成
          pOutputData->pTokuseiName = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) gfl2::str::StrBuf( 8 * 3, m_initParam.pUtilHeap->GetDeviceHeap() );

          pG2DUtil->SetRegisterTokuseiName( 0, tokuseiId );
          pG2DUtil->GetStringExpand( pOutputData->pTokuseiName, STR_TOKUSEI_NAME );
        }
      }

      //  所持アイテム
      {
        u16 itemNo  = pBtlParam->GetItem();
        if( itemNo == ITEM_DUMMY_DATA )
        {
        }
        else
        {
          //  アイテム名作成
          pOutputData->pHaveItemName  = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) gfl2::str::StrBuf( 8 * 3, m_initParam.pUtilHeap->GetDeviceHeap() );

          pG2DUtil->SetRegisterItemName( 0, itemNo );
          pG2DUtil->GetStringExpand( pOutputData->pHaveItemName, STR_ITEM_NAME );
        }
      }
    }

    //  パーツペイン取得
    {
      const s32 aPartsPaneIdx[ POKE_OBJ_MAX ] =
      {
        PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_POKEICON_00,
        PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_POKEICON_01,
        PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_POKEICON_02,
        PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_POKEICON_03,
        PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_POKEICON_04,
        PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_POKEICON_05,
      };

      pOutputData->pIconPartsPane = pLayoutWork->GetPartsPane( aPartsPaneIdx[ listIdx ] );
      GFL_ASSERT( pOutputData->pIconPartsPane );
    }

    //  アニメ一覧
    {
      const s32 aAnimeIdx[ POKE_OBJ_MAX ][ PokeObject::ANIME_TYPE_MAX ]  =
      {
        //  0
        {
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_00_DIE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_00_REVIVE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_00_ITEM_CATCH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_00_ITEM_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_00_JAMP_HIGH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_00_JAMP_MIDDLE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_00_JAMP_LOW,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_00_SICK,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_00_TOUCH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_00_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_00_TOUCH_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_00_JAMP_SELECT,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_00_STOP,
        },

        //  1
        {
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_01_DIE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_01_REVIVE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_01_ITEM_CATCH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_01_ITEM_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_01_JAMP_HIGH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_01_JAMP_MIDDLE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_01_JAMP_LOW,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_01_SICK,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_01_TOUCH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_01_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_01_TOUCH_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_01_JAMP_SELECT,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_01_STOP,
        },

        //  2
        {
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_02_DIE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_02_REVIVE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_02_ITEM_CATCH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_02_ITEM_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_02_JAMP_HIGH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_02_JAMP_MIDDLE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_02_JAMP_LOW,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_02_SICK,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_02_TOUCH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_02_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_02_TOUCH_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_02_JAMP_SELECT,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_02_STOP,
        },

        //  3
        {
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_03_DIE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_03_REVIVE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_03_ITEM_CATCH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_03_ITEM_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_03_JAMP_HIGH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_03_JAMP_MIDDLE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_03_JAMP_LOW,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_03_SICK,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_03_TOUCH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_03_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_03_TOUCH_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_03_JAMP_SELECT,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_03_STOP,
        },

        //  4
        {
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_04_DIE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_04_REVIVE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_04_ITEM_CATCH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_04_ITEM_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_04_JAMP_HIGH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_04_JAMP_MIDDLE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_04_JAMP_LOW,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_04_SICK,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_04_TOUCH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_04_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_04_TOUCH_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_04_JAMP_SELECT,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_04_STOP,
        },

        //  5
        {
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_05_DIE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_05_REVIVE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_05_ITEM_CATCH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_05_ITEM_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_05_JAMP_HIGH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_05_JAMP_MIDDLE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_05_JAMP_LOW,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_05_SICK,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_05_TOUCH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_05_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_05_TOUCH_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_05_JAMP_SELECT,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__POKEICON_05_STOP,
        },
      };

      for( s32 i = 0; i < PokeObject::ANIME_TYPE_MAX; ++i )
      {
        pOutputData->aAnimeIdx[ i ]  = aAnimeIdx[ listIdx ][ i ];
      }
    }
  }

//
//  ここから先はPokeObjectCursorのクラス定義
//
  BattleUIPokeListLowerDraw::PokeObjectCursor::PokeObjectCursor()
  {
    _Clear();
  }

  void BattleUIPokeListLowerDraw::PokeObjectCursor::Initialize( UIView* pView )
  {
    GFL_ASSERT( pView );

    gfl2::lyt::LytWk* pLayoutWork = pView->GetLayoutWork( LYT_WK_TYPE_SCREEN );
    GFL_ASSERT( pLayoutWork );

    //  ポケモンリストカーソルアイコンペイン取得
    {
      m_pPokeListCursorPane  = pLayoutWork->GetPane( PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_BG_POKEICON );
      GFL_ASSERT( m_pPokeListCursorPane );
    }

    m_pView = pView;
  }

  void BattleUIPokeListLowerDraw::PokeObjectCursor::SetPos( const gfl2::math::VEC3& rPos )
  {
    m_pPokeListCursorPane->SetTranslate( rPos );
  }

  void BattleUIPokeListLowerDraw::PokeObjectCursor::SetActive( const bool bFlg )
  {
    bool  bSettingFlg = false;
    if( bFlg == true )
    {
      bSettingFlg = ( m_state != STATE_POKE_OBJECT_CURSOR_START );
    }
    else
    {
      bSettingFlg = ( m_state != STATE_POKE_OBJECT_CURSOR_END );
    }

    if( bSettingFlg == false )
    {
      return;
    }

    ::app::util::G2DUtil* pG2DUtil  = m_pView->GetG2DUtil();

    pG2DUtil->StopAnime( LYT_WK_TYPE_SCREEN, LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000_KEEP_00 );
    pG2DUtil->StopAnime( LYT_WK_TYPE_SCREEN, LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000_SELECT_00 );
    pG2DUtil->StopAnime( LYT_WK_TYPE_SCREEN, LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000_UNSELECT_00 );

    m_bActive = bFlg;

    if( bFlg == true )
    {            
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000_SELECT_00 );

      m_state = STATE_POKE_OBJECT_CURSOR_START;
    }
    else
    {
      pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000_UNSELECT_00 );
      m_state = STATE_POKE_OBJECT_CURSOR_END;
    }
  }

  void BattleUIPokeListLowerDraw::PokeObjectCursor::Update()
  {
    switch( m_state )
    {
      case STATE_POKE_OBJECT_CURSOR_START:
      {
        ::app::util::G2DUtil* pG2DUtil  = m_pView->GetG2DUtil();
        if( pG2DUtil->IsAnimeEnd( LYT_WK_TYPE_SCREEN, LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000_SELECT_00 ) == true )
        {
          //  開始が終了したのでkeepに
          pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000_KEEP_00 );

          m_state = STATE_POKE_OBJECT_CURSOR_KEEP;
        }
        else
        {
          break;
        }
      }

      case STATE_POKE_OBJECT_CURSOR_KEEP:
      {
        break;
      }

      case STATE_POKE_OBJECT_CURSOR_END:
      {
        ::app::util::G2DUtil* pG2DUtil  = m_pView->GetG2DUtil();
        if( pG2DUtil->IsAnimeEnd( LYT_WK_TYPE_SCREEN, LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000_UNSELECT_00 ) == true )
        {
          m_state = STATE_POKE_OBJECT_CURSOR_NONE;
        }

        break;
      }
    }
  }

//
//  ここから先はPokeObjのクラス定義
//
  //  初期化
  void BattleUIPokeListLowerDraw::PokeObject::Initialize( const INIT_PARAM& rInitParam, const u8 itemIdx )
  {
    GFL_ASSERT( rInitParam.pUIView );
    GFL_ASSERT( rInitParam.pUtilHeap );
    GFL_ASSERT( m_pIcon == NULL );

    m_initParam = rInitParam;

    m_itemIdx = itemIdx;

    //  パラメータがないのであれば、オブジェクトは非表示に
    if( rInitParam.pBtlParam == NULL )
    {
      SetVisible( false );
      SetEnable( false );

      return;
    }

    SetVisible( false );

    {
      struct ::app::tool::PokeIconToolEx::PokeIconToolParam initParam;

      initParam.pUIView   = rInitParam.pUIView;
      initParam.pHeap     = rInitParam.pUtilHeap;
      initParam.layout_id = LYT_WK_TYPE_SCREEN;
      initParam.res_id    = 0;
      initParam.iconMax   = 1;
      initParam.sync      = false;

      m_pIcon = GFL_NEW( rInitParam.pUtilHeap->GetDeviceHeap() ) ::app::tool::PokeIconToolEx( initParam );
      m_pIcon->SetPokeIcon( 0, rInitParam.pBtlParam->GetSrcData(), PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON, rInitParam.pIconPartsPane );
      m_bLoadIcon = true;

      //  死亡しているのであれば、アイコンは死亡アニメに
      if( rInitParam.pBtlParam->IsDead() )
      {
        m_pIcon->StartAnim( rInitParam.aAnimeIdx[ ANIME_TYPE_POKEICON_DIE ] );
      }
    }
  }
        
  bool BattleUIPokeListLowerDraw::PokeObject::IsReady()
  {
    if( m_pIcon == NULL )
    {
      return  true;
    }

    m_pIcon->Update();
    return  ( m_pIcon->IsAllReplaced() );
  }

  //  終了
  void BattleUIPokeListLowerDraw::PokeObject::Terminate()
  {
  }

  //  インスタンスの破棄可能か
  bool BattleUIPokeListLowerDraw::PokeObject::IsDelete()
  {
    bool  bRet  = true;
    if( m_pIcon )
    {
      if( m_pIcon->EndFunc() == false )
      {
        bRet  = false;
      }
    }

    return  bRet;
  }

  //  更新
  void BattleUIPokeListLowerDraw::PokeObject::Update()
  {
    if( m_bEnable == false ) { return; }

    if( m_pIcon != NULL )
    {
      if( m_bLoadIcon != NULL )
      {
        if( m_pIcon->IsReplaced( 0 ) == true )
        {
          SetVisible( true );
          m_bLoadIcon = false;
        }
      }

      m_pIcon->Update();
    }
  }

  //  オブジェクトのアクティブ設定
  bool BattleUIPokeListLowerDraw::PokeObject::SetAcitve( const bool bFlg, PokeObjectCursor* pObjectCursor )
  {
    if( m_pIcon == NULL )
    {
      return  false;
    }

    if( m_bEnable == false )
    {
      GFL_WARNING( "PokeObject Enable = %d\n", m_bEnable );
      return  false;
    }

    ::app::util::G2DUtil* pG2DUtil  = m_initParam.pUIView->GetG2DUtil();

    //  瀕死時は専用のアニメを再生しているのでカーソル選択のアニメ設定はしない
    if( m_initParam.pBtlParam->IsDead() == false )
    {
      if( bFlg )
      {
        //  選択中はジャンプアニメさせる
        pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, m_initParam.aAnimeIdx[ ANIME_TYPE_POKEICON_JAMP_SELECT ] );
      }
      else
      {
        //  ジャンプアニメをしているかもしれないので停止
        pG2DUtil->StopAnime( LYT_WK_TYPE_SCREEN, m_initParam.aAnimeIdx[ ANIME_TYPE_POKEICON_JAMP_SELECT ] );
        pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, m_initParam.aAnimeIdx[ ANIME_TYPE_POKEICON_STOP ] );
      }
    }

    //  状態が変化しているか取得
    bool  bChangeActive  = ( m_bActive != bFlg );

    //  選択中はカーソル移動
    if( ( bFlg == true ) && ( bChangeActive == true ) )
    {
      if( pObjectCursor != NULL )
      {
        pObjectCursor->SetPos( m_initParam.pIconPartsPane ->GetTranslate() );
        pObjectCursor->SetActive( bFlg );
      }
    }

    m_bActive = bFlg;

    return  bChangeActive;
  }

  //  表示設定
  void BattleUIPokeListLowerDraw::PokeObject::SetVisible( const bool bFlg )
  {
    ::app::util::G2DUtil* pG2DUtil  = m_initParam.pUIView->GetG2DUtil();

    if( m_pIcon == NULL )
    {
      pG2DUtil->SetPaneVisible( m_initParam.pIconPartsPane, false );
      return;
    }

    //  テクスチャーの差し替えができるないと表示設定はしない
    if( m_pIcon->IsReplaced( 0 )  == true )
    {
      pG2DUtil->SetPaneVisible( m_initParam.pIconPartsPane, bFlg );
    }
  }

  //  ボタンデータを取得
  const ::app::ui::ButtonInfoEx BattleUIPokeListLowerDraw::PokeObject::CreateButtonInfoEx( const s32 buttonId )
  {
    ::app::util::G2DUtil* pG2DUtil  = m_initParam.pUIView->GetG2DUtil();
    gfl2::lyt::LytWk* pLayoutWork   = pG2DUtil->GetLayoutWork( BattleUIPokeListLowerDraw::LYT_WK_TYPE_SCREEN );

    const gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

    ::app::ui::ButtonInfoEx buttonInfoEx;

    buttonInfoEx.button_id              = buttonId;
    buttonInfoEx.cancel_anime_index     = ::app::tool::ButtonManager::ANIMATION_NULL;
    buttonInfoEx.key_select_anime_index = ::app::tool::ButtonManager::ANIMATION_NULL;
    buttonInfoEx.release_anime_index    = ::app::tool::ButtonManager::ANIMATION_NULL;
    buttonInfoEx.touch_anime_index      = ::app::tool::ButtonManager::ANIMATION_NULL;
    buttonInfoEx.active_anime_index     = ::app::tool::ButtonManager::ANIMATION_NULL;
    buttonInfoEx.passive_anime_index    = ::app::tool::ButtonManager::ANIMATION_NULL;

    buttonInfoEx.picture_pane = m_initParam.pIconPartsPane;
    GFL_ASSERT( buttonInfoEx.picture_pane );

    {
      buttonInfoEx.bound_pane   = pLayoutWork->GetBoundingPane( m_initParam.pIconPartsPane, PANENAME_COMMON_BTN_LOW_001_PANE_BOUND_00, &rMultiResId );
      GFL_ASSERT( buttonInfoEx.bound_pane );
    }

    return  buttonInfoEx;
  }

//
//  ここから先は BattleUIPokeListLowerInputクラスの定義
//
  void BattleUIPokeListLowerButtonCtrl::Initialize( BattleUIPokeListLowerDraw* pRootView, ::app::ui::UIView* pActiveView, const s32 useLytWkId, gfl2::lyt::LytMultiResID* pMultiResId, ::app::util::Heap* pUtilHeap, ::app::tool::MenuCursor* pMenuCursor )
  {
    _Clear();

    _SetBaseParam( pRootView, pActiveView, useLytWkId, pMultiResId, pUtilHeap );

    //  共通で使用するボタンを作成
    {
      gfl2::lyt::LytParts*  pBackBtnParts = m_pLayoutWork->GetPartsPane( PANENAME_BATTLEPOKELIST_MIN_LOW_000_PANE_PARTS_BACK );
      GFL_ASSERT( pBackBtnParts );

      //  ボタン設定データ定義一覧
      const ::app::ui::ButtonInfoEx    aButtonPaneIdxList[] =
      {
        //  戻るボタン
        {
          BUTTON_ID_BACK,
          pBackBtnParts,
          m_pLayoutWork->GetBoundingPane( pBackBtnParts, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, pMultiResId ),
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTS_BACK_TOUCH,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTS_BACK_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTS_BACK_CANSEL,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTS_BACK_TOUCH_RELEASE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTS_BACK_ACTIVE,
          LA_POKE_LIST_LOW_BATTLEPOKELIST_MIN_LOW_000__PARTS_BACK_PASSIVE,
        },

        //  ポケモンアイコンのボタン
        pRootView->CreatePokeIconButtonInfoEx( 0, BattleUIPokeListLowerButtonCtrl::BUTTON_ID_POKE_ICON_0 ),
        pRootView->CreatePokeIconButtonInfoEx( 1, BattleUIPokeListLowerButtonCtrl::BUTTON_ID_POKE_ICON_1 ),
        pRootView->CreatePokeIconButtonInfoEx( 2, BattleUIPokeListLowerButtonCtrl::BUTTON_ID_POKE_ICON_2 ),
        pRootView->CreatePokeIconButtonInfoEx( 3, BattleUIPokeListLowerButtonCtrl::BUTTON_ID_POKE_ICON_3 ),
        pRootView->CreatePokeIconButtonInfoEx( 4, BattleUIPokeListLowerButtonCtrl::BUTTON_ID_POKE_ICON_4 ),
        pRootView->CreatePokeIconButtonInfoEx( 5, BattleUIPokeListLowerButtonCtrl::BUTTON_ID_POKE_ICON_5 ),
      };

      //  ボタン登録
      pActiveView->ReleaseButtonManager();
      pActiveView->CreateButtonManager( pUtilHeap, m_pLayoutWork, aButtonPaneIdxList, GFL_NELEMS( aButtonPaneIdxList ) );

      //  SE設定
      if( m_bUseInputSE )
      {
        ::app::tool::ButtonManager* pButtonManager  = pActiveView->GetButtonManager();
        pButtonManager->SetButtonSelectSE( BUTTON_ID_BACK, SEQ_SE_CANCEL1 );

        //  ポケモンアイコンの決定
        {
          const ButtonId aPokeIconButtonId[]  =
          {
            BattleUIPokeListLowerButtonCtrl::BUTTON_ID_POKE_ICON_0,
            BattleUIPokeListLowerButtonCtrl::BUTTON_ID_POKE_ICON_1,
            BattleUIPokeListLowerButtonCtrl::BUTTON_ID_POKE_ICON_2,
            BattleUIPokeListLowerButtonCtrl::BUTTON_ID_POKE_ICON_3,
            BattleUIPokeListLowerButtonCtrl::BUTTON_ID_POKE_ICON_4,
            BattleUIPokeListLowerButtonCtrl::BUTTON_ID_POKE_ICON_5,
          };

          u32 loopNum = GFL_NELEMS( aPokeIconButtonId );
          for( u32 i = 0; i < loopNum; ++i )
          {
            pButtonManager->SetButtonSelectSE( aPokeIconButtonId[ i ], SEQ_SE_DECIDE1 );
          }
        }
      }
    }

    //  入力コールバック設定
    m_pActiveView->SetInputListener( this );
  }

  //------------------------------------------------------------------
  /**
    * @brief   カレントカーソル項目を決定
    */
  //------------------------------------------------------------------
  bool BattleUIPokeListLowerButtonCtrl::PushBtnCurrentCursor()
  {
    if( m_pSubButtonCtrl )
    {
      return  m_pSubButtonCtrl->PushBtnCurrentCursor();
    }

    return false;
  }

  //------------------------------------------------------------------
  /**
    * @brief   指定したボタンidを押す
    */
  //------------------------------------------------------------------
  bool BattleUIPokeListLowerButtonCtrl::PushButtonId( const ButtonId id )
  {
    //  メイン、サブのどちらのボタンも無効なら押す処理はしない
    if( ( m_pActiveView->GetButtonManager()->IsButtonInputEnable( id ) == false ) )
    {
      if( m_pSubButtonCtrl )
      {
        return  m_pSubButtonCtrl->PushButtonId( id );
      }
    }
    else
    {
      if( m_bInputEnable == false )
      {
        return  false;
      }

      //  ボタンをプッシュアニメ
      //  コールバックの OnLayoutPaneEvent 関数が呼ばれる
      m_pActiveView->GetButtonManager()->StartSelectedAct( id );

      return true;
    }

    return false;
  }

  //------------------------------------------------------------------
  /**
    * @brief   カーソル移動
    */
  //------------------------------------------------------------------
  bool BattleUIPokeListLowerButtonCtrl::MoveCursorUp( const bool bLoop )
  {
    //  サブを優先
    if( m_pSubButtonCtrl )
    {
      return m_pSubButtonCtrl->MoveCursorUp( bLoop );
    }

    return false;
  }

  bool BattleUIPokeListLowerButtonCtrl::MoveCursorDown( const bool bLoop )
  {
    //  サブを優先
    if( m_pSubButtonCtrl )
    {
      return m_pSubButtonCtrl->MoveCursorDown( bLoop );
    }

    return false;
  }

  bool BattleUIPokeListLowerButtonCtrl::MoveCursorLeft( const bool bLoop )
  {
    //  サブを優先
    if( m_pSubButtonCtrl )
    {
      return m_pSubButtonCtrl->MoveCursorLeft( bLoop );
    }

    return false;
  }

  bool BattleUIPokeListLowerButtonCtrl::MoveCursorRight( const bool bLoop )
  {
    //  サブを優先
    if( m_pSubButtonCtrl )
    {
      return m_pSubButtonCtrl->MoveCursorRight( bLoop );
    }

    return false;
  }

  //  更新処理
  void BattleUIPokeListLowerButtonCtrl::Update()
  {
    if( m_pSubButtonCtrl )
    {
      return  m_pSubButtonCtrl->Update();
    }
  }

  //  カーソル移動種類取得
  BattleUIPokeListLowerButtonCtrl::CursorMoveType BattleUIPokeListLowerButtonCtrl::GetCursorMoveTyoe() const
  {
    //  サブを優先
    if( m_pSubButtonCtrl )
    {
      return  m_pSubButtonCtrl->GetCursorMoveTyoe();
    }

    return CURSOR_MOVE_TYPE_SPECIAL;
  }

  //  カレントカーソルを設定(押せるButtonId値で判別、一致しない場合は何も処理しない)
  void BattleUIPokeListLowerButtonCtrl::SetCurrentCursor( const BattleUIPokeListLowerButtonCtrl::ButtonId id, const bool bUseMoveSE )
  {
    //  サブを優先
    if( m_pSubButtonCtrl )
    {
      m_pSubButtonCtrl->SetCurrentCursor( id, bUseMoveSE );
    }
  }

  //  現在のカーソル移動idx
  s32 BattleUIPokeListLowerButtonCtrl::GetCursorMoveIdx() const
  {
    if( m_pSubButtonCtrl )
    {
      return m_pSubButtonCtrl->GetCursorMoveIdx();
    }

    return m_cursorIdx;
  }

  //  カーソル移動最大idx
  s32 BattleUIPokeListLowerButtonCtrl::GetCursorMoveIdxMax()
  {
    if( m_pSubButtonCtrl )
    {
      return m_pSubButtonCtrl->GetCursorMoveIdxMax();
    }

    return -1;
  }

  //  ボタンのテキスト設定
  void BattleUIPokeListLowerButtonCtrl::SetNameButton( const ButtonId id, const s32 textId )
  {
    if( m_pSubButtonCtrl )
    {
      m_pSubButtonCtrl->SetNameButton( id, textId );
    }
  }

  //  ボタンの表示設定
  void BattleUIPokeListLowerButtonCtrl::SetVisibleButton( const ButtonId id, const bool bFlg )
  {
    if( bFlg )
    {
      m_pActiveView->GetButtonManager()->SetButtonVisible( id );
    }
    else
    {
      m_pActiveView->GetButtonManager()->SetButtonInvisible( id );
    }

    if( m_pSubButtonCtrl )
    {
      m_pSubButtonCtrl->SetVisibleButton( id, bFlg );
    }
  }

  //  ボタンの有効設定
  void BattleUIPokeListLowerButtonCtrl::SetEnableButtonAll( const bool bFlg )
  {
    m_bInputEnable  = bFlg;
    m_pActiveView->GetButtonManager()->SetInputEnable( bFlg );

    if( m_pSubButtonCtrl )
    {
      m_pSubButtonCtrl->SetEnableButtonAll( bFlg );
    }
  }

  //  ボタンの有効設定
  void BattleUIPokeListLowerButtonCtrl::SetEnableButton( const ButtonId id, const bool bFlg )
  {
    if( bFlg )
    {
      m_pActiveView->GetButtonManager()->SetButtonActiveAnimeFrameLast( id );
    }
    else
    {
      m_pActiveView->GetButtonManager()->SetButtonPassiveAnimeFrameLast( id );
    }

    if( m_pSubButtonCtrl )
    {
      m_pSubButtonCtrl->SetEnableButton( id, bFlg );
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインタッチ時を検知
  * @note 主な用途  RELEASE判定だが、決定前時にカーソルを移動したい、とか
  */
  //--------------------------------------------------------------------------------------------
  // overwride
  void BattleUIPokeListLowerButtonCtrl::OnLayoutPaneTouchTriggerEvent( const u32 buttonId )
  {
    //  ポケモンリストの選択だと呼び出す
    BattleUIPokeListLowerDraw::INIT_PARAM&  rInitParam  = m_pRootView->GetInitParam();
    if( rInitParam.pDelegate == NULL )
    {
      return;
    }

    if( _IsTouchTriggerButtonId( (ButtonId)buttonId ) == true )
    {
      BattleUIPokeListLowerDrawDelegate::RESULT_DATA  resutlData;
      _OutputInputResultData( &resutlData, (ButtonId)buttonId );

      rInitParam.pDelegate->OnEnterInput( m_pRootView, resutlData );
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインイベントの検知
  *       【ボタンマネージャー関係】
  *
  * @param  buttonId   通知を受けるペインＩＤ
  *
  * @return 同フレーム内での他入力イベントを許可するならtrue、
  *         他のイベントを排除するならfalseを返却すること。
  */
  //--------------------------------------------------------------------------------------------
  // overwride
  ::app::ui::UIInputListener::ListenerResult BattleUIPokeListLowerButtonCtrl::OnLayoutPaneEvent( const u32 buttonId )
  {
    BattleUIPokeListLowerDraw::INIT_PARAM&  rInitParam  = m_pRootView->GetInitParam();
    if( rInitParam.pDelegate == NULL )
    {
      return  ENABLE_ACCESS;
    }

    if( _IsTouchTriggerButtonId( (ButtonId)buttonId ) == true )
    {
      return  ENABLE_ACCESS;
    }

    //  入力結果を出力
    BattleUIPokeListLowerDrawDelegate::RESULT_DATA  resutlData;
    _OutputInputResultData( &resutlData, (ButtonId)buttonId );

    if( rInitParam.pDelegate->OnEnterInput( m_pRootView, resutlData ) )
    {
      return  DISABLE_ACCESS;
    }

    return  ENABLE_ACCESS;
  }

  //------------------------------------------------------------------
  /**
    * @brief   キー入力の検知
    *
    * @param  pButton ボタン
    * @param  pKey    十字キー
    * @param  pStick  アナログスティック
    *
    * @return 同フレーム内での他入力イベントを許可するならtrue、
    *         他のイベントを排除するならfalseを返却すること。
    */
  //------------------------------------------------------------------
  // overwride
  ::app::ui::UIInputListener::ListenerResult BattleUIPokeListLowerButtonCtrl::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    BattleUIPokeListLowerDraw::INIT_PARAM&  rInitParam  = m_pRootView->GetInitParam();
    if( rInitParam.pDelegate == NULL )
    {
      return ENABLE_ACCESS;
    }

    if( rInitParam.pDelegate->OnKeyAction( m_pRootView, pButton, pKey, pStick ) )
    {
      return  DISABLE_ACCESS;
    }

    return ENABLE_ACCESS;
  }

  // ボタン入力結果を出力
  void BattleUIPokeListLowerButtonCtrl::_OutputInputResultData( BattleUIPokeListLowerDrawDelegate::RESULT_DATA* pResultData, const ButtonId pushButtonId )
  {
    GFL_ASSERT( pResultData );

    pResultData->Clear();
    pResultData->buttonId = pushButtonId;
  }

  //  基本パラメータ設定
  void BattleUIPokeListLowerButtonCtrl::_SetBaseParam( BattleUIPokeListLowerDraw* pRootView, ::app::ui::UIView* pActiveView, const s32 useLytWkId, gfl2::lyt::LytMultiResID* pMultiResId, ::app::util::Heap* pUtilHeap )
  {
    GFL_ASSERT( pRootView );
    GFL_ASSERT( pUtilHeap );
    GFL_ASSERT( pMultiResId );
    GFL_ASSERT( useLytWkId != -1 );

    m_pRootView   = pRootView;
    m_useLytWkId  = useLytWkId;
    m_pMultiResId = pMultiResId;
    m_pUtilHeap   = pUtilHeap;
    m_pActiveView = pActiveView;

    m_pLayoutWork = pRootView->GetLayoutWork( useLytWkId );
    GFL_ASSERT( m_pLayoutWork );

    m_pG2DUtil  = m_pRootView->GetG2DUtil();
    GFL_ASSERT( m_pG2DUtil );

    m_bUseInputSE = m_pRootView->GetInitParam().bUseInputSE;
  }

  //  タッチトリガー用のボタンidかチェック
  bool BattleUIPokeListLowerButtonCtrl::_IsTouchTriggerButtonId( const ButtonId id )
  {
    return  false;
    //  @tobo 押してすぐに反応する対象があれば復活!!（今はない）
    /*
    const ButtonId aChkButtonId[] =
    {
      BUTTON_ID_POKE_ICON_0,
      BUTTON_ID_POKE_ICON_1,
      BUTTON_ID_POKE_ICON_2,
      BUTTON_ID_POKE_ICON_3,
      BUTTON_ID_POKE_ICON_4,
      BUTTON_ID_POKE_ICON_5,
    };

    u32 loopNum = GFL_NELEMS( aChkButtonId );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( aChkButtonId[ i ] == id )
      {
        return  true;
      }
    }

    return  false;
    */
  }

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )
