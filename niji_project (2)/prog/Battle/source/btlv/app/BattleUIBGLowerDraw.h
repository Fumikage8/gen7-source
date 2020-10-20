//=============================================================================
/**
 * @file BattleUIBGLowerDraw.h
 * @brief バトル下画面 >> 「背景」画面
 * @author yuto_uchida
 * @date 2015.03.23
 */
//=============================================================================
#if !defined( __BATTLE_UI_BG_LOWER_DRAW_H__ )
#define __BATTLE_UI_BG_LOWER_DRAW_H__
#pragma once

#include "AppLib/include/ui/UIView.h"

#include "Battle/include/battle_SetupParam.h"

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  /** バトル下BG画面の通知クラス */
  class BattleUIBGLowerDrawDelegate
  {
  public:
    BattleUIBGLowerDrawDelegate() {}
    virtual ~BattleUIBGLowerDrawDelegate() {}

    //  決定入力用リスナ
    virtual bool OnEnterInput( class BattleUIBGLowerDraw* pDrawInst, s32 buttonId )  = 0;

    //  キーとスティックの入力結果
    virtual void OnKeyAction( class BattleUIBGLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick ) = 0;
  };

  /** バトル下BG画面 */
  class BattleUIBGLowerDraw : public ::app::ui::UIView, public ::app::ui::UIInputListener
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleUIBGLowerDraw );  // コピーを禁止

    public:
      // ステート
      enum StateType
      {
        STATE_TYPE_NONE         = 0,
        STATE_TYPE_IN,
        STATE_TYPE_INPUT_IDLE,
        STATE_TYPE_OUT,
        STATE_TYPE_END
      };

      //  ボタンid
      enum ButtonId
      {
        BUTTON_ID_OTEIRE  = 0,
        BUTTON_ID_MAX
      };

      //  モード
      enum ModeType
      {
        MODE_TYPE_NORMAL = 0,
        MODE_TYPE_ROYALE,
        MODE_TYPE_MAX
      };

      //  クラス初期化時に必要なパラメータ
      typedef struct _tag_init_param
      {
        ::app::util::Heap*                pUtilHeap;
        ::app::util::AppRenderingManager* pRenderingManager;

        //  使用するメッセージのポインタ（GARC_message_btl_app_DATのデータでないとアウト）
        gfl2::str::MsgData*               pBtlAppMsgData;
        print::WordSet*                   pBtlWordSet;

        ModeType                          modeType;
        BtlWeather                        weatherType;
        BattleUIBGLowerDrawDelegate*      pDelegate;

        //  レイアウトバイナリ(あらかじめ使う側がレイアウトデータを作成して渡す。)
        void* pLytBuff;

        _tag_init_param()
        {
          pUtilHeap           = NULL;
          pRenderingManager   = NULL;
          pLytBuff            = NULL;
          pDelegate           = NULL;
          pBtlAppMsgData      = NULL;
          pBtlWordSet         = NULL;
          modeType            = MODE_TYPE_NORMAL;
          weatherType         = BTL_WEATHER_NONE;
        }

      } INIT_PARAM;

      //------------------------------------------------------------------
      /**
       * @brief   コンストラクタ
       */
      //------------------------------------------------------------------
      BattleUIBGLowerDraw( const INIT_PARAM& param );

      //------------------------------------------------------------------
      /**
       * @brief   デストラクタ
       */
      //------------------------------------------------------------------
      virtual ~BattleUIBGLowerDraw();

      //------------------------------------------------------------------
      /**
       * @brief   開始
       */
      //------------------------------------------------------------------
      void Start( void ) { _SetState( STATE_TYPE_IN ); }

      //------------------------------------------------------------------
      /**
       * @brief   終了
       */
      //------------------------------------------------------------------
      void End( void ) { _SetState( STATE_TYPE_OUT ); }

      //------------------------------------------------------------------
      /**
       * @brief   更新処理
       */
      //------------------------------------------------------------------
      virtual void Update( void );

      //------------------------------------------------------------------
      /**
       * @brief   描画
       */
      //------------------------------------------------------------------
      virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

      //------------------------------------------------------------------
      /**
       * @brief   現在のステート
       */
      //------------------------------------------------------------------
      StateType GetState() const { return m_state; }

      //------------------------------------------------------------------
      /**
       * @brief   天候設定
       */
      //------------------------------------------------------------------
      void SetWeather( const BtlWeather weather );

      //------------------------------------------------------------------
      /**
       * @brief   モード設定
       */
      //------------------------------------------------------------------
      void SetMode( const ModeType modeType );

      //------------------------------------------------------------------
      /**
       * @brief   BGカバー表示設定
       */
      //------------------------------------------------------------------
      void SetVisibleCurverBG( const bool bFlg );

      //------------------------------------------------------------------
      /**
       * @brief   おていれモード開始
       */
      //------------------------------------------------------------------
      void StartOteireMode();

      //------------------------------------------------------------------
      /**
       * @brief   おていれモード中か
       */
      //------------------------------------------------------------------
      bool IsOteireMode() const { return m_bStartOteire; }

      //-------------------------------------------------------------------------
      /**
        * @brief 指定したidのボタンを押す
        */
      //-------------------------------------------------------------------------
      void PushBtn( ButtonId buttonId );

      //--------------------------------------------------------------------------------------------
      /**
      * @brief ペイン選択アニメーションの開始を検知
      *       【ボタンマネージャー関係】
      *
      * @param  painId   通知を受けるペインＩＤ
      *
      * @return none
      */
      //--------------------------------------------------------------------------------------------
      virtual void OnLayoutPaneStartingAnimeEvent( const u32 buttonId );

      //------------------------------------------------------------------
      /**
       * @brief   ペインタッチ時を検知
       */
      //------------------------------------------------------------------
      virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 buttonId );

      //------------------------------------------------------------------
      /**
       * @brief   キー入力通知用リスナ
       */
      //------------------------------------------------------------------
      virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

    private:
      //  レイアウトワーク一覧
      enum LytWkType
      {
        LYT_WK_TYPE_SCREEN  = 0,
        LYT_WK_TYPE_MAX
      };

      //  各天候の対応したアニメid
      struct WeatherAnimeData
      {
        //  画面全体
        s32 bgKeepId;
      };

      /** 天候オブジェクト */
      class WeatherObject
      {
      public:
        typedef struct _tag_init_param
        {
          ::app::util::G2DUtil* pG2DUtil;
          gfl2::lyt::LytParts*  pPartsWeatherBG;

          s32 lytWorkId;

          _tag_init_param()
          {
            pG2DUtil          = NULL;
            pPartsWeatherBG   = NULL;
            lytWorkId         = -1;
          }
        } INIT_PARAM;

        WeatherObject() { _Clear(); }

        void Initialize( const INIT_PARAM& rInitParam );
        void Terminate()  {}

        void Start( const BtlWeather weather );
        void Update();

      private:
        void _Clear()
        {
          m_weather     = BTL_WEATHER_MAX;
        }

        INIT_PARAM  m_initParam;
        BtlWeather  m_weather;
      };

      void _Clear()
      {
        m_state             = STATE_TYPE_NONE;
        m_pCoverBGPane      = NULL;
        m_pWeatherObject    = NULL;
        m_bStartOteire      = false;
      }

      void _Initialize2D( void* pAppLytBuff );

      void _UpdateState();
      void _SetState( const StateType newStateType );

      //  変数宣言一覧
      INIT_PARAM  m_initParam;
      StateType   m_state;

      WeatherObject*  m_pWeatherObject;

      gfl2::lyt::LytPane* m_pCoverBGPane;

      bool  m_bStartOteire;

      static const WeatherAnimeData ms_aWeatherAnimeDataTable[ BTL_WEATHER_MAX ];
  };

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )

#endif // __BATTLE_UI_BG_LOWER_DRAW_H__ 
