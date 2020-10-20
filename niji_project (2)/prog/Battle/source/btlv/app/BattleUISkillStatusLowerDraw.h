//=============================================================================
/**
 * @file BattleUISkillStatusLowerDraw.h
 * @brief バトル下画面 >> 「わざスタータス」画面
 * @author yuto_uchida
 * @date 2015.03.19
 */
//=============================================================================
#if !defined( __BATTLE_UI_SKILL_STATUS_LOWER_DRAW_H__ )
#define __BATTLE_UI_SKILL_STATUS_LOWER_DRAW_H__
#pragma once

#include "AppLib/include/ui/UIView.h"
#include "Battle/source/btl_common.h"

//  前方宣言
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
  
    class AppCommonGrpIconData;

GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  /** バトル下画面のわざステータス画面の通知クラス */
  class BattleUISkillStatusLowerDrawDelegate
  {
  public:
    BattleUISkillStatusLowerDrawDelegate() {}
    virtual ~BattleUISkillStatusLowerDrawDelegate() {}

    //  決定入力用リスナ
    virtual bool OnEnterInput( class BattleUISkillStatusLowerDraw* pDrawInst, s32 buttonId )  = 0;
  };

  /**
   *  @brief バトル下画面のわざステータス選択画面クラス
   */
  class BattleUISkillStatusLowerDraw : public ::app::ui::UIView, public ::app::ui::UIInputListener
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleUISkillStatusLowerDraw );  // コピーを禁止

    public:
      enum WindowType
      {
        WINDOW_TYPE_NORMAL  = 0,
        WINDOW_TYPE_INFO
      };

      // ステート
      enum StateType
      {
        STATE_TYPE_NONE         = 0,
        STATE_TYPE_IN,
        STATE_TYPE_INPUT_IDLE,
        STATE_TYPE_OUT,
        STATE_TYPE_END,
      };

      enum
      {
        BUTTON_ID_CLOSE,
      };

      //  クラス初期化時に必要なパラメータ
      typedef struct _tag_init_param
      {
        //  使用するメッセージのポインタ（GARC_message_btl_app_DATのデータでないとアウト）
        gfl2::str::MsgData* pBtlAppMsgData;
        print::WordSet*     pBtlWordSet;

        BattleUISkillStatusLowerDrawDelegate* pDelegate;

        ::app::util::Heap* pUtilHeap;
        ::app::util::AppRenderingManager* pRenderingManager;

        //  アイコングラフィックデータ
        ::app::tool::AppCommonGrpIconData*  pAppCommonGrpIconData;

        //  レイアウトバイナリ(あらかじめ使う側がレイアウトデータを作成して渡す。)
        void* pLytBuff;

        //  わざid
        WazaID        wazaId;
        //  全力わざid
        WazaID        zenryokuWazaId;
        //  全力技の名前
        gfl2::str::StrBuf*  pZenryokuWazaName;

        //  タイプ
        PokeType_tag  wazaType;

        //  分類タイプ
        WazaDamageType  wazaBunruiType;

        //  PP(now/max)
        u8 nowPPNum, maxPPNum;

        //  威力
        s32 powerNum;

        //  命中
        s32 hitNum;

        //  技説明
        gfl2::str::StrBuf*  pWazaSetumeiString;

        //  入力SEを使うか
        bool  bUseInputSE;

        //  全力わざか
        bool  bZenryoku;

        _tag_init_param()
        {
          pBtlAppMsgData        = NULL;
          pBtlWordSet           = NULL;
          pAppCommonGrpIconData = NULL;
          pDelegate             = NULL;
          pUtilHeap             = NULL;
          pRenderingManager     = NULL;
          pLytBuff              = NULL;
          pZenryokuWazaName     = NULL;
          pAppCommonGrpIconData = NULL;
          pWazaSetumeiString    = NULL;

          wazaType           = POKETYPE_NORMAL;
          wazaId             = WAZANO_NULL;
          zenryokuWazaId     = WAZANO_NULL;
          wazaBunruiType     = pml::wazadata::DMG_NONE; 
          nowPPNum           = 0;
          maxPPNum           = 0;
          powerNum           = 0;
          hitNum             = 0;

          bUseInputSE        = false;
          bZenryoku          = false;

        }

      } INIT_PARAM;

      //---------------------------------------------------------------------------------
      // コンストラクタ・デストラクタ
      //---------------------------------------------------------------------------------
      /**
      * @brief コンストラクタ
      */
      BattleUISkillStatusLowerDraw( const INIT_PARAM& param );

      /**
      * @brief デストラクタ
      */
      virtual ~BattleUISkillStatusLowerDraw();

      /**
        * @brief 開始
        */
      void Start( const WindowType windowType, const gfl2::math::Vector3& rStartPos )
      {
        m_windowType        = windowType;
        m_openEventStartPos = rStartPos;
        _SetState( STATE_TYPE_IN );
      }

      /**
        * @brief 終了
        */
      void End( void ) { _SetState( STATE_TYPE_OUT ); }

      /**
        * @brief 更新処理
        */
      virtual void Update( void );

      /**
      * @brief 描画
      */
      virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

      /**
      * @brief  現在のステート
      */
      StateType GetState() const { return m_state; }

      //  タッチパネルイベントの検知(画面全体をタッチしているかどうかの判定で必要)
    virtual ::app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );

      /// キー入力通知用リスナ
    virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick);

    private:
      //  レイアウトワーク一覧
      enum LytWkType
      {
        LYT_WK_TYPE_SCREEN  = 0,
        LYT_WK_TYPE_MAX,
      };

      void _Clear()
      {
        m_pOpenEventStartPosPane  = NULL;
        m_pOpenEventEndPosPane    = NULL;
        m_windowType              = WINDOW_TYPE_NORMAL;
        m_startAnimeId            = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
      }

      void _Initialize2D( void* pAppLytBuff );

      void _UpdateState();
      void _SetState( const StateType newStateType );

      //  全力技エフェクトからmsgIdを取得
      //  -1が返ってきたらエラー
      s32 _GetZenryokuWazaEffectMsgId( const ZenryokuWazaEffect effectId );

      //  変数宣言一覧
      INIT_PARAM  m_initParam;
      StateType   m_state;

      gfl2::math::Vector3 m_openEventStartPos;
      WindowType          m_windowType;

      u32                 m_startAnimeId;

      gfl2::lyt::LytPane* m_pOpenEventStartPosPane;
      gfl2::lyt::LytPane* m_pOpenEventEndPosPane;
  };

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )

#endif // __BATTLE_UI_SKILL_STATUS_LOWER_DRAW_H__ 
