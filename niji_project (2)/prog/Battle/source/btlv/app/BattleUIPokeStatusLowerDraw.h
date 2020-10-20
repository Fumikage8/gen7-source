//=============================================================================
/**
 * @file BattleUIPokeStatusLowerDraw.h
 * @brief バトル下画面 >> 「ポケモンステータス」画面
 * @author yuto_uchida
 * @date 2015.03.19
 */
//=============================================================================
#if !defined( __BATTLE_UI_POKE_STATUS_LOWER_DRAW_H__ )
#define __BATTLE_UI_POKE_STATUS_LOWER_DRAW_H__
#pragma once

#include "AppLib/include/ui/UIView.h"
#include "AppLib/include/Tool/app_tool_PaneListView.h"
#include "AppLib/include/Tool/app_tool_PokeIcon.h"
#include "AppLib/include/Tool/AppToolGaugePartsLayout.h"

#include "Battle/source/btl_pokeparam.h"

#include "BattleUIDataManager.h"

//  前方宣言
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )

  class AppCommonGrpIconData;

GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )


GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  /** バトル下画面のポケモンステータス選択画面の通知クラス */
  class BattleUIPokeStatusLowerDrawDelegate
  {
  public:
    typedef struct _tag_enter_data
    {
      s32 buttonId;

      BattleUIDataManager::STATUS_DATA  pushStatusData;
      gfl2::math::Vector3               pushInfoIconPos;

      void Clear()
      {
        buttonId              = -1;
      }

    } ENTER_DATA;

    BattleUIPokeStatusLowerDrawDelegate() {}
    virtual ~BattleUIPokeStatusLowerDrawDelegate() {}

    // 画面内のイベント
    virtual bool OnEnterInput( class BattleUIPokeStatusLowerDrawBase* pDrawInst, const ENTER_DATA& rEnterData ) = 0;

    //  キー入力通知用リスナ
    virtual bool OnKeyAction( class BattleUIPokeStatusLowerDrawBase* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick ) = 0;
  };

  /**
   *  @brief バトル下画面のポケモンステータス選択画面基本クラス
   *  @note
   *    味方、敵とで別々の画面になるので、共通している箇所はこのクラスに記載する
   */
  class BattleUIPokeStatusLowerDrawBase :
   public ::app::ui::UIView,
   public ::app::ui::UIInputListener
  {
  public:
      //  ボタンID一覧
    enum ButtonId
    {
      BUTTON_ID_CANCEL,
      BUTTON_ID_INFO_TOKUSEI,
      BUTTON_ID_INFO_ITEM,
      BUTTON_ID_STATUS_LIST_LEFT_ARROW,
      BUTTON_ID_STATUS_LIST_RIGHT_ARROW,

      BUTTON_ID_STATIS_LIST_ITEM_INFO_0,
      BUTTON_ID_STATIS_LIST_ITEM_INFO_1,
      BUTTON_ID_STATIS_LIST_ITEM_INFO_2,
      BUTTON_ID_STATIS_LIST_ITEM_INFO_3,

      BUTTON_ID_MAX
    };

  private:

    GFL_FORBID_COPY_AND_ASSIGN( BattleUIPokeStatusLowerDrawBase );  // コピーを禁止

    //------------------------------------------------------------------
    /**
      * @brief  ステータスリスト項目オブジェクト
      * @note
      *      渡すUIViewはあらかじめ初期化をしておかないとハングする
      */
    //------------------------------------------------------------------
    //  StatusListViewクラス内で下記のクラスを使用しているので、宣言を手前にもってくる必要があり、ここで宣言
    class StatusItemObject
    {
    public:
      enum ButtonAnimId
      {
        BUTTON_ANIME_ID_TOUCH,        //!< タッチ時のアニメ
        BUTTON_ANIME_ID_RELEASE,      //!< タッチ決定時のアニメ
        BUTTON_ANIME_ID_CANCEL,       //!< タッチ状態からのキャンセルアニメ
        BUTTON_ANIME_ID_KEY_SELECT,   //!< キー決定時のアニメ ( touch => release の連続アニメ )
        BUTTON_ANIME_ID_ACTIVE,       //!< アクティブアニメ
        BUTTON_ANIME_ID_PASSIVE,      //!< パッシブアニメ

        BUTTON_ANIME_ID_MAX,
      };

            //  初期化構造体
      typedef struct _tag_init
      {
        //  項目のルートペインid
        u32 item_root_pane_id;

        //  項目名のtextboxペインid
        u32 item_name_pane_id;

        //  数字のtextboxペインid
        u32 item_num_pane_id;

        //  インフォボタンのパーツペインid
        u32 info_icon_parts_pane_id;
        //  インフォボタンのボックスペインid
        u32 info_icon_bound_pane_id;

        //  インフォアニメidリスト
        u32 a_info_anime_id[ BUTTON_ANIME_ID_MAX ];

      } INIT_PARAM;

      StatusItemObject() { _Clear(); }

      virtual ~StatusItemObject()
      {
        GFL_SAFE_DELETE( m_pStatusName );
      }

      //------------------------------------------------------------------
      /**
        * @brief 初期化処理
        * @note
        *  クラスを実行するまえに呼ばないと動作保障はしない
        */
      //------------------------------------------------------------------
      void Initialize( UIView* pParentUIView, const u32 layoutId, const u32 resId, gfl2::heap::HeapBase* pHeap, const INIT_PARAM& rInitParam );

      //  表示設定
      void SetVisible( const bool bFlg );

      //  テキスト設定
      void SetText( const BattleUIDataManager::STATUS_DATA& rStatusData, gfl2::str::MsgData* pStatusMsgData );

      //  インフォアイコンの座標
      const gfl2::math::Vector3&  GetInfoIconPos()
      {
        //  インフォボタンの座標保存
        const gfl2::math::VEC3 pos  = m_pG2DUtil->GetPanePos( m_buttonDataInfoIcon.picture_pane );
        m_infoIconPos.Set( pos.x, pos.y, pos.z );

        return m_infoIconPos;
      }

      //  インフォアイコンのボタンデータ取得
      ::app::ui::ButtonInfoEx& CreateButtonInfoIcon( const ButtonId id )
      {
        m_buttonDataInfoIcon.button_id = id;

        return m_buttonDataInfoIcon;
      }

    private:
      enum
      {
        LIST_ITEM_TITLE_NAME_LEN  = 8 * 3,
      };

      void _Clear()
      {
        m_pG2DUtil        = NULL;
        m_pRootPane       = NULL;
        m_pTitleTextPane  = NULL;
        m_pNumTextPane    = NULL;
        m_pStatusName     = NULL;

        gfl2::std::MemFill( &m_buttonDataInfoIcon, NULL, sizeof( m_buttonDataInfoIcon ) );
      }

      ::app::util::G2DUtil*   m_pG2DUtil;

      gfl2::lyt::LytPane*     m_pRootPane;
      gfl2::lyt::LytTextBox*  m_pTitleTextPane;
      gfl2::lyt::LytTextBox*  m_pNumTextPane;
      gfl2::str::StrBuf*      m_pStatusName;

      gfl2::math::Vector3     m_infoIconPos;

      ::app::ui::ButtonInfoEx   m_buttonDataInfoIcon;
    };

  public:
    // ステート
    enum StateType
    {
      STATE_TYPE_NONE         = 0,
      STATE_TYPE_IN,
      STATE_TYPE_INPUT_IDLE,
      STATE_TYPE_OUT,
      STATE_TYPE_END,
    };

    //  クラス初期化時に必要なパラメータ
    typedef struct _tag_init_param
    {
      ::app::util::Heap*                    pUtilHeap;
      ::app::util::AppRenderingManager*     pRenderingManager;
      BattleUIPokeStatusLowerDrawDelegate*  pDelegate;

      gfl2::str::MsgData*                   pBtlAppMsgData;
      gfl2::str::MsgData*                   pBtlStatusMsgData;

      print::WordSet*                       pBtlWordSet;

      const btl::BTL_POKEPARAM*             pPokeParam;

      //  アイコングラフィックデータ
      ::app::tool::AppCommonGrpIconData*    pAppCommonGrpIconData;

      //  すでに図鑑に登録しているか
      bool                                  bRegsterZukan;

      //  入力関連のSEを使うか
      bool                                  bUseInputSE;

      //  レイアウトバイナリ(あらかじめ使う側がレイアウトデータを作成して渡す。)
      void* p_lyt_buff;

      //  ポケモン状態パラメータリスト
      const BattleUIDataManager::POKE_STATUS_DATA* pStatusDataList;

      //  タイプアイコンを表示するかどうか
      bool                                  bShowTypeIcon;

      //  タイプはイリュージョンなどによって変わるので使う側で設定
      pml::PokeType aPokeType[ 2 ];

      _tag_init_param()
      {
        pBtlAppMsgData        = NULL;
        pBtlWordSet           = NULL;
        pUtilHeap             = NULL;
        pRenderingManager     = NULL;
        pDelegate             = NULL;
        p_lyt_buff            = NULL;
        pPokeParam            = NULL;
        pStatusDataList       = NULL;
        pBtlStatusMsgData     = NULL;
        pAppCommonGrpIconData = NULL;
        bRegsterZukan         = false;
        bUseInputSE           = false;
        bShowTypeIcon         = true;

        {
          u32 loopNum = GFL_NELEMS( aPokeType );
          for( u32 i = 0; i < loopNum; ++i )
          {
            aPokeType[ i ]  = POKETYPE_NULL;
          }
        }
      }

    } INIT_PARAM;

    //------------------------------------------------------------------
    /**
      * @brief  ステータスリスト
      */
    //------------------------------------------------------------------
    class StatusList
    {
    public:

      //  デストラクタ
      ~StatusList()
      {
        //  メモリ破棄はここで
        GFL_SAFE_DELETE_ARRAY( m_pStatusDataList );
      }

      //  初期化
      void Initialize( UIView* pRootView, const u32 layoutId, const u32 resId, const BattleUIPokeStatusLowerDrawBase::INIT_PARAM* pRootDrawInitParam );

      //  描画
      void Draw();

      //  ページについての値取得
      u32 GetNowPageNum() const { return m_pageNowNum; }
      u32 GetTopPageNum() const { return 0; }
      u32 GetBottomPageNum() const { return m_pageListBottomNum; }

      //  現在表示しているページから指定行のステータスデータ取得
      const BattleUIDataManager::STATUS_DATA& GetCurrentStatusItemData( const u32 viewListItemIdx );

      //  現在表示しているページから指定行のインフォアイコンの座標取得
      const gfl2::math::Vector3& GetCurrentStatusInfoIconPos( const u32 viewListItemIdx );

      //  ページ設定
      void SetPage( const u32 num );

      //  ページ移動
      void MoveNextPage();
      void MovePrevPage();

      //  リスト項目のインフォアイコンボタンデータ作成
      ::app::ui::ButtonInfoEx& CreateButtonListItem( const u32 listIdx, const ButtonId id )
      {
        GFL_ASSERT( listIdx < GFL_NELEMS( m_aStatusItemList ) );
        return  m_aStatusItemList[ listIdx ].CreateButtonInfoIcon( id );
      }

      //  上下矢印のボタンデータ作成
      ::app::ui::ButtonInfoEx& CreateButtonUpArrow( const ButtonId id )
      {
        m_buttonDataLeftArrow.button_id = id;

        return m_buttonDataLeftArrow;
      }

      ::app::ui::ButtonInfoEx& CreateButtonDownArrow( const ButtonId id )
      {
        m_buttonDataRightArrow.button_id = id;

        return m_buttonDataRightArrow;
      }

    private:
      enum
      {
        STATUS_LIST_NUM = 4,
      };

      //  ステータスデータ
      typedef struct
      {
        //  元ステータスデータ
        const BattleUIDataManager::STATUS_DATA* pOrgData;

      } STATUS_DATA;

      void _Clear()
      {
        m_pStatusDataList = NULL;
        m_pRootView       = NULL;
        m_pageListBottomNum = 0;
      }

      //  ここから変数宣言

      //  リスト項目のセットアップデータ
      static const StatusItemObject::INIT_PARAM   m_aListItemInitParamTable[ STATUS_LIST_NUM ];

        //  ステータス項目オブジェクト
      StatusItemObject          m_aStatusItemList[ STATUS_LIST_NUM ];

      //  上下の矢印のボタンデータ(クラス内部で矢印ボタンの表示設定をするのでボタンデータを保持しておく)
      ::app::ui::ButtonInfoEx   m_buttonDataLeftArrow;
      ::app::ui::ButtonInfoEx   m_buttonDataRightArrow;

      //  表示に使うステータスデータ
      //  リストデータを返す時などは、この描画用に作成したリストデータを使って返す
      //  決して元のリストデータは使わない！
      STATUS_DATA*              m_pStatusDataList;

      UIView*                   m_pRootView;

      u32                       m_pageListBottomNum;
      u32                       m_pageNowNum;

      const BattleUIPokeStatusLowerDrawBase::INIT_PARAM*  m_pDrawInitParam;
    };

    //------------------------------------------------------------------
    /**
      * @brief コンストラクタ
      * @note
      *  クラス動作に必要なパラメータは設定する。
      *  以後は設定したパラメータを参照して動作(途中でパラメータの変更はできない)
      */
    //------------------------------------------------------------------
    BattleUIPokeStatusLowerDrawBase( const INIT_PARAM& param );

    //------------------------------------------------------------------
    /**
      * @brief デストラクタ
      */
    //------------------------------------------------------------------
    virtual ~BattleUIPokeStatusLowerDrawBase();

    //------------------------------------------------------------------
    /**
      * @brief 初期化処理
      * @note
      *  クラスを実行するまえに呼ばないと動作保障はしない
      */
    //------------------------------------------------------------------
    virtual void Initialize();

    //------------------------------------------------------------------
    /**
      * @brief   終了処理
      */
    //------------------------------------------------------------------
    virtual void Terminate();

    //------------------------------------------------------------------
    /**
      * @brief   破棄可能か
      */
    //------------------------------------------------------------------
    virtual bool IsDelete();

    //------------------------------------------------------------------
    /**
      * @brief 開始
      */
    //------------------------------------------------------------------
    void Start( void ) { _SetState( STATE_TYPE_IN ); }

    //------------------------------------------------------------------
    /**
      * @brief 終了
      */
    //------------------------------------------------------------------
    void End( void ) { _SetState( STATE_TYPE_OUT ); }

    //------------------------------------------------------------------
    /**
      * @brief 更新
      */
    //------------------------------------------------------------------
    virtual void Update( void );

    //------------------------------------------------------------------
    /**
      * @brief 描画
      */
    //------------------------------------------------------------------
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

    //------------------------------------------------------------------
    /**
      * @brief ボタンを押す
      */
    //------------------------------------------------------------------
    void PushBtn( const ButtonId id );

    //------------------------------------------------------------------
    /**
      * @brief 現在のステート
      */
    //------------------------------------------------------------------
    StateType GetState() const { return m_state; }

    //------------------------------------------------------------------
    /**
      * @brief 状態リスト取得
      */
    //------------------------------------------------------------------
    StatusList& GetStatusList() { return m_statusList; }

    //------------------------------------------------------------------
    /**
      * @brief 初期化パラメータ取得
      */
    //------------------------------------------------------------------
    const INIT_PARAM& GetInitParam() const { return m_initParam; }

    //------------------------------------------------------------------
    /**
      * @brief ペインタッチ時を検知
      */
    //------------------------------------------------------------------
    virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 buttonId );

    //------------------------------------------------------------------
    /**
      * @brief キー入力通知用リスナ
      */
    //------------------------------------------------------------------
    virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick);

  protected:
    //  レイアウトワーク一覧
    enum LytWkType
    {
      LYT_WK_TYPE_SCREEN  = 0,
      LYT_WK_TYPE_MAX,
    };

    /**
      *  @brief バトルステータスプレートパーツオブジェクトクラス
      */
    class StatusPlatePartsObject
    {
      GFL_FORBID_COPY_AND_ASSIGN( StatusPlatePartsObject );  // コピーを禁止

      public:
        //  クラス初期化時に必要なパラメータ
        struct INIT_PARAM
        {
          ::app::ui::UIView*                    pUiView;
          ::app::util::Heap*                    pUtilHeap;

          gfl2::str::MsgData*                   pBtlAppMsgData;
          print::WordSet*                       pBtlWordSet;

          s32                                   layoutWorkId;
          bool                                  bRegsterZukan;
          //  タイプアイコンを表示するかどうか
          bool                                  bShowTypeIcon;

          //  ポケモンアイコンを反転させるか
          bool                                  bPokeIconFlip;

          //  味方パラメータか
          bool                                  bSelfParam;
          const btl::BTL_POKEPARAM*             pPokeParam;

          //  タイプはイリュージョンなどによって変わるので使う側で設定
          pml::PokeType aPokeType[ 2 ];

          INIT_PARAM()
          {
            layoutWorkId       = -1;
            pUiView            = NULL;
            pUtilHeap          = NULL;
            pPokeParam         = NULL;
            pBtlWordSet        = NULL;
            pBtlAppMsgData     = NULL;
            bRegsterZukan      = false;
            bShowTypeIcon      = true;
            bPokeIconFlip      = false;
            bSelfParam         = true;

            {
              u32 loopNum = GFL_NELEMS( aPokeType );
              for( u32 i = 0; i < loopNum; ++i )
              {
                aPokeType[ i ]  = POKETYPE_NULL;
              }
            }
          }
        };

        //  アニメidのデータ
        struct ANIME_ID_DATA
        {
          //  アニメ「pos_own」
          s32 pos_own_anime_idx;
          //  アニメ「pos_others」
          s32 pos_other_anime_idx;
          //  アニメ「keep_sick」
          s32 keep_sick_anime_idx;
        };

        //  ペイン設定パラメータ
        struct PANE_CONFIG_PARAM
        {
          //  Battle_PLT_UPP_098.flyt専用のデータ設定
          struct _tag_plt_low_data
          {
            //  アニメidデータ
            ANIME_ID_DATA anime_id_data;

            //  名前のペイン（TextBox_00）
            gfl2::lyt::LytTextBox*  p_name_text_box_pane;
            //  LVの数ペイン(TextBox_01)
            gfl2::lyt::LytTextBox*  p_lv_number_text_box_pane;
            //  Lvの項目ペイン(TextBox_02)
            gfl2::lyt::LytTextBox*  p_lv_item_text_box_pane;
            //  捕獲済みアイコン(Parts_Ball)
            gfl2::lyt::LytPane*     p_ball_icon_pane;
            //  性別アイコンペイン(Parts_Gender)
            gfl2::lyt::LytParts*    p_icon_parts;
            //  状態異常のアイコンペイン(Parts_Sickからのアイコン画像ペイン)
            gfl2::lyt::LytPicture*  p_sick_icon_picture;

          } plt_upp_data;

          //  Common_ICN_001.flyt専用のデータ(性別アイコン)
          struct _tag_icon_001_data
          {
            //  アニメ「icon_male」
            s32 icon_male_anime_idx;
            //  アニメ「icon_female」
            s32 icon_female_anime_idx;


          } icon_001_data;

          //  Common_ICN_002.flyt専用のデータ(状態異常アイコン)
          struct _tag_icon_002_data
          {
            //  アニメ「active」
            s32 icon_acitve_anime_idx;
            //  アニメ「passive」
            s32 icon_passive_anime_idx;

          } icon_002_data;
        
          //  Common_ICN_003.flyt専用のデータ(メガ進化アイコン)
          struct _tag_icon_003_data
          {
            //  アニメ「iconPatern_00」 -> メガ進化
            s32 icon_mega_anime_idx;
            //  アニメ「iconPatern_01」 -> アルファマーク
            s32 icon_alpha_anime_idx;
            //  アニメ「iconPatern_02」 -> オメガマーク
            s32 icon_omega_anime_idx;

            //  アニメ「active」
            s32 icon_active_anime_idx;
            //  アニメ「passive」
            s32 icon_passive_anime_idx;

          } icon_003_data;

          //  Common_BTN_LOW_001.flyt専用のデータ（ポケモンアイコン）
          struct _tag_poke_icon_btn_low_001_data
          {
            //  アニメ「死亡」
            s32 anime_die_idx;

            s32 root_parts_pane_id;

            s32 icon_pane_id;
          } poke_icon_btn_low_001_data;

          //  Common_GAC_000.flyt専用のデータ
          ::app::tool::AppToolHPGaugePartsLayout::INIT_PARAM  hp_gauge_data;

          //  Common_GAC_001.flyt専用のデータ
          ::app::tool::AppToolGaugePartsLayout::INIT_PARAM  exp_gauge_data;

          //  ポケモンタイプ1の画像ペイン
          gfl2::lyt::LytPicture*  p_poke_type1_icon_picture;
          //  ポケモンタイプ2の画像ペイン
          gfl2::lyt::LytPicture*  p_poke_type2_icon_picture;

        };

        //------------------------------------------------------------------
        /**
          * @brief コンストラクタ
          * @note
          *  クラス動作に必要なパラメータは設定する。
          *  以後は設定したパラメータを参照して動作(途中でパラメータの変更はできない)
          */
        //------------------------------------------------------------------
        StatusPlatePartsObject() { _Clear(); }

        //------------------------------------------------------------------
        /**
          * @brief デストラクタ
          */
        //------------------------------------------------------------------
        virtual ~StatusPlatePartsObject()
        {
          GFL_SAFE_DELETE( m_pPokeIcon );
          GFL_SAFE_DELETE( m_pExpGaugePartsLayout );
          GFL_SAFE_DELETE( m_pHpGaugePartsLayout );
        }

        //------------------------------------------------------------------
        /**
          * @brief 初期化処理
          * @note
          *  クラスを実行するまえに呼ばないと動作保障はしない
          */
        //------------------------------------------------------------------
        void Initialize( const INIT_PARAM& rInitParam, const PANE_CONFIG_PARAM& rConfigParam, ::app::tool::AppCommonGrpIconData* pAppCommonGrpIconData );

        //------------------------------------------------------------------
        /**
          * @brief   終了処理
          */
        //------------------------------------------------------------------
        void Terminate()
        {
        }

        //------------------------------------------------------------------
        /**
          * @brief   破棄可能か
          */
        //------------------------------------------------------------------
        bool IsDelete()
        {
          bool  bRet  = true;
          if( m_pPokeIcon->EndFunc() == false )
          {
            bRet  = false;
          }

          return  bRet;
        }

        //------------------------------------------------------------------
        /**
          * @brief 更新
          */
        //------------------------------------------------------------------
        void Update( void )
        {
          if( m_pHpGaugePartsLayout )
          {
            m_pHpGaugePartsLayout->Update();
          }

          if( m_pExpGaugePartsLayout )
          {
            m_pExpGaugePartsLayout->Update();
          }

          m_pPokeIcon->Update();
        }

        //  自身のパラメータ表示か
        void SetEnableSelfParamView( const bool bFlg );

      private:
        void _Clear()
        {
          m_pHpGaugePartsLayout   = NULL;
          m_pExpGaugePartsLayout  = NULL;
          m_pPokeIcon             = NULL;

          m_animeIdData.pos_own_anime_idx = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
          m_animeIdData.pos_other_anime_idx = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
          m_animeIdData.keep_sick_anime_idx = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
        }

        void _Initialize2D( void* pAppLytBuff );

        //  変数宣言一覧
        INIT_PARAM          m_initParam;
        ANIME_ID_DATA       m_animeIdData;

        ::app::tool::PokeIconToolEx*            m_pPokeIcon;

        ::app::tool::AppToolHPGaugePartsLayout* m_pHpGaugePartsLayout;
        ::app::tool::AppToolGaugePartsLayout*   m_pExpGaugePartsLayout;
    };

    //  変数宣言一覧
    INIT_PARAM                m_initParam;

    StatusPlatePartsObject*   m_pPlateObject;

  protected:
    bool  m_bPokeIconFlip;

    //  共通初期化
    void _InitializeBase( const bool bSelfParam );

  private:
    //------------------------------------------------------------------
    /**
      * @brief  パラメータランク項目オブジェクト
      * @note
      *      渡すUIViewはあらかじめ初期化をしておかないとハングする
      */
    //------------------------------------------------------------------
    class ParamItemRankObject
    {
    public:
      //  初期化構造体
      typedef struct _tag_init
      {
        //  項目のペインid
        s32 item_name_pane_id;
        //  項目のメッセージid
        s32 item_name_message_id;

        //  項目のステータスパーツid
        s32 status_parts_pane_id;
        //  項目のステータスランク
        s32 status_rank_num;

        //  ステータスアップ切り替えアニメ
        s32 status_up_type_change_anime_idx;

        //  ステータスダウン切り替えアニメ
        s32 status_down_type_change_anime_idx;

        //  ステータスアップ演出のループアニメ
        s32 status_up_type_effect_loop_anime;

        //  ステータスダウン演出のループアニメ
        s32 status_down_type_effect_loop_anime;

      } INIT_PARAM;

      ParamItemRankObject() { _Clear(); }

      //------------------------------------------------------------------
      /**
        * @brief 初期化処理
        * @note
        *  クラスを実行するまえに呼ばないと動作保障はしない
        */
      //------------------------------------------------------------------
      void Initialize( UIView* pParentUIView, const u32 layoutId, const s32 resId,  const INIT_PARAM& rInitParam );

      /** 項目名を指定 */
      void SetItemName( const s32 messageId );

      //------------------------------------------------------------------
      /**
        * @brief ステータスランク値を指定
        * @note
        *   rankNum = RANK_STATUS_DEFAULT であればステータスランクなし
        */
      //------------------------------------------------------------------
      void SetStatusRank( const s32 rankNum );

    private:
      void _Clear()
      {
        m_pParentUIView     = NULL;
        m_pStatusPartsPane  = NULL;
        m_pResId            = NULL;
        m_pLayoutWork       = NULL;
        m_layoutId          = 0;

        {
          m_initParam.item_name_pane_id = gfl2::lyt::LYTSYS_PANEINDEX_ERROR;
          m_initParam.item_name_message_id = gfl2::lyt::LYTSYS_PANEINDEX_ERROR;
          m_initParam.status_parts_pane_id = gfl2::lyt::LYTSYS_PANEINDEX_ERROR;
          m_initParam.status_rank_num = 0;
          m_initParam.status_up_type_change_anime_idx = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
          m_initParam.status_down_type_change_anime_idx = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
          m_initParam.status_up_type_effect_loop_anime = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
          m_initParam.status_down_type_effect_loop_anime = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;    
        }

      }

      UIView*                   m_pParentUIView;
      u32                       m_layoutId;
      gfl2::lyt::LytWk*         m_pLayoutWork;
      gfl2::lyt::LytParts*      m_pStatusPartsPane;
      gfl2::lyt::LytMultiResID* m_pResId;

      INIT_PARAM            m_initParam;
    };

    void _Clear()
    {
      m_state                 = STATE_TYPE_NONE;
      m_bPokeIconFlip         = false;
      m_pParamitemObjectArray = NULL;
      m_pPlateObject          = NULL;
    }

    void _Initialize2D( void* pAppLytBuff );

    void _UpdateState();
    void _SetState( const StateType newStateType );

    //  変数宣言一覧
    StateType   m_state;

    //  各パラメータの状態ランク項目リスト
    ParamItemRankObject*                m_pParamitemObjectArray;

    //  ステータスリスト
    StatusList                          m_statusList;
  };

  /**
   *  @brief バトル下画面の味方ポケモンステータス選択画面基本クラス
   */
  class BattleUISelfPokeStatusLowerDraw : public BattleUIPokeStatusLowerDrawBase
  {
  public:
    //------------------------------------------------------------------
    /**
      * @brief コンストラクタ
      * @note
      *  クラス動作に必要なパラメータは設定する。
      *  以後は設定したパラメータを参照して動作(途中でパラメータの変更はできない)
      */
    //------------------------------------------------------------------
    BattleUISelfPokeStatusLowerDraw( const INIT_PARAM& param );

    //------------------------------------------------------------------
    /**
      * @brief 初期化処理
      * @note
      *  クラスを実行するまえに呼ばないと動作保障はしない
      */
    //------------------------------------------------------------------
    virtual void Initialize();

    //------------------------------------------------------------------
    /**
      * @brief   終了処理
      */
    //------------------------------------------------------------------
    virtual void Terminate()
    {
      for( s32 i = 0; i < INFO_TYPE_MAX; ++i )
      {
        m_pInfoItemObjectArray[ i ]->Terminate();
        GFL_SAFE_DELETE( m_pInfoItemObjectArray[ i ] );
      }

      BattleUIPokeStatusLowerDrawBase::Terminate();
    }

    //------------------------------------------------------------------
    /**
      * @brief 更新
      */
    //------------------------------------------------------------------
    virtual void Update( void )
    {
      BattleUIPokeStatusLowerDrawBase::Update();

      for( s32 i = 0; i < INFO_TYPE_MAX; ++i )
      {
        if( m_pInfoItemObjectArray[ i ] )
        {
          m_pInfoItemObjectArray[ i ]->Update();
        }
      }
    }

  private:
    enum
    {
      INFO_TYPE_TOKUSEI =0,
      INFO_TYPE_ITEM,
      INFO_TYPE_MAX,
    };

    //------------------------------------------------------------------
    /**
      * @brief  info項目オブジェクト
      * @note
      *      渡すUIViewはあらかじめ初期化をしておかないとハングする
      */
    //------------------------------------------------------------------
    class InfoItemObject
    {
    public:
      //  初期化構造体
      typedef struct _tag_init
      {
        //  項目のペインid
        s32 item_name_pane_id;

        //  ボタンid
        s32 button_id;

      } INIT_PARAM;

      InfoItemObject() { _Clear(); }

        //------------------------------------------------------------------
      /**
        * @brief 初期化処理
        * @note
        *  クラスを実行するまえに呼ばないと動作保障はしない
        */
      //------------------------------------------------------------------
      void Initialize( UIView* pParentUIView, gfl2::heap::HeapBase* pHeap, const u32 layoutId, const INIT_PARAM& rInitParam )
      {
        GFL_ASSERT( pParentUIView );
        GFL_ASSERT( pHeap );

        m_pParentUIView   = pParentUIView;
        m_layoutId        = layoutId;
        m_initParam       = rInitParam;

        m_pG2DUtil  = m_pParentUIView->GetG2DUtil();
        GFL_ASSERT( m_pG2DUtil );
      }

      //  終了処理
      void Terminate()
      {
      }

      /** 更新 */
      void Update()
      {
        /*
            毎フレーム表示設定をする
            なぜか
              ・アニメ再生により画面切り替え時に項目の表示・非表示設定にじゃまされないのを防ぐため
                アニメ再生指定と同フレームで項目の表示設定をしても、その後にアニメ再生更新が走るので設定した表示設定が無効になるのを防ぐため(本当はやりたくない)
        */
        m_pG2DUtil->SetPaneVisible( m_layoutId, m_initParam.item_name_pane_id, m_bVisible );
      }

      //  レイアウトid取得
      s32 GetLayoutId() const { return m_layoutId; }

      //  名前ペインid取得
      s32 GetNamePaneId() const { return  m_initParam.item_name_pane_id; }

      /** 項目の表示設定 */
      void SetVisible( const bool bFlg )
      {          
        GFL_ASSERT( m_pG2DUtil );

        m_pG2DUtil->SetPaneVisible( m_layoutId, m_initParam.item_name_pane_id, bFlg );
        m_pParentUIView->SetButtonEnable( m_initParam.button_id, bFlg, false );

        m_bVisible  = bFlg;
      }

      void _Clear()
      {
        m_pParentUIView     = NULL;
        m_pLayoutWork       = NULL;
        m_pG2DUtil          = NULL;
        m_layoutId          = 0;
        m_bVisible          = false;

        m_initParam.item_name_pane_id = gfl2::lyt::LYTSYS_PANEINDEX_ERROR;
        m_initParam.button_id         = BUTTON_ID_MAX;
      }

      UIView*                   m_pParentUIView;
      u32                       m_layoutId;
      gfl2::lyt::LytWk*         m_pLayoutWork;
      ::app::util::G2DUtil*     m_pG2DUtil;
      bool                      m_bVisible;

      INIT_PARAM            m_initParam;
    };

    void _Clear()
    {
      gfl2::std::MemFill( m_pInfoItemObjectArray, 0, sizeof( m_pInfoItemObjectArray ) );
    }

    //  変数宣言
    InfoItemObject* m_pInfoItemObjectArray[ INFO_TYPE_MAX ];
  };

  /**
   *  @brief バトル下画面の敵ポケモンステータス選択画面基本クラス
   */
  class BattleUIEnemyPokeStatusLowerDraw : public BattleUIPokeStatusLowerDrawBase
  {
  public:
    BattleUIEnemyPokeStatusLowerDraw( const INIT_PARAM& param ) : BattleUIPokeStatusLowerDrawBase( param) {};

    //------------------------------------------------------------------
    /**
      * @brief 初期化処理
      * @note
      *  クラスを実行するまえに呼ばないと動作保障はしない
      */
    //------------------------------------------------------------------
    virtual void Initialize();
  };

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )

#endif // __BATTLE_UI_POKE_STATUS_LOWER_DRAW_H__ 
