//=============================================================================
/**
 * @file ConfigUpperUIDraw.h
 * @brief コンフィグの上UI画面
 * @author yuto_uchida
 * @date 2015.05.08
 */
//=============================================================================
#if !defined( NIJI_PROJECT_CONFIG_UPPER_UI_DRAW_H_INCLUDED )
#define NIJI_PROJECT_CONFIG_UPPER_UI_DRAW_H_INCLUDED
#pragma once

#include "AppLib/include/ui/UIView.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"

#include "Savedata/include/ConfigSave.h"

//  前方宣言
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Tool )

  class CommonMessageWindowManipulator;

GFL_NAMESPACE_END( Tool )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( config )

  /** コンフィグ下UI画面 */
  class ConfigUpperUIDraw : public ::app::ui::UIView
  {
    GFL_FORBID_COPY_AND_ASSIGN( ConfigUpperUIDraw );  // コピーを禁止

    public:
      // ステート
      enum StateType
      {
        STATE_TYPE_NONE         = 0,
        STATE_TYPE_IN,
        STATE_TYPE_UPDATE,
        STATE_TYPE_OUT,
        STATE_TYPE_END,
      };

      //  クラス初期化時に必要なパラメータ
      typedef struct _tag_init_param
      {
        ::app::util::AppRenderingManager* pRenderingManager;

        //  レイアウトバイナリ(あらかじめ使う側がレイアウトデータを作成して渡す。)
        void* pLytBuff;

        gfl2::str::MsgData* pMsgData;
        print::WordSet*     pWordSet;

        _tag_init_param()
        {
          pRenderingManager         = NULL;
          pLytBuff                  = NULL;
          pMsgData                  = NULL;
          pWordSet                  = NULL;
        }

      } INIT_PARAM;

      //---------------------------------------------------------------------------------
      // コンストラクタ・デストラクタ
      //---------------------------------------------------------------------------------
      /**
      * @brief コンストラクタ
      */
      ConfigUpperUIDraw( ::app::util::Heap* pUtilHeap );

      /**
      * @brief デストラクタ
      */
      virtual ~ConfigUpperUIDraw();

      //------------------------------------------------------------------
      /**
       * @brief   初期化
       */
      //------------------------------------------------------------------
      void Initialize( const INIT_PARAM& param );

      //------------------------------------------------------------------
      /**
       * @brief   終了処理
       */
      //------------------------------------------------------------------
      void Terminate();

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
      virtual void Draw(  gfl2::gfx::CtrDisplayNo displayNo );

      //------------------------------------------------------------------
      /**
       * @brief   現在のステート
       */
      //------------------------------------------------------------------
      const StateType GetState() const { return m_state; }

      //------------------------------------------------------------------
      /**
       * @brief   ヘルプメッセージ設定
       * @param   textId    : 表示するテキストid
       * @param   msgSpeed  : メッセージスピードタイプ
       */
      //------------------------------------------------------------------
      void SetHelpMessage( const u32 textId, Savedata::ConfigSave::MSG_SPEED msgSpeed );

      //------------------------------------------------------------------
      /**
       * @brief   テキスト更新
       */
      //------------------------------------------------------------------
      void UpdateText( gfl2::str::MsgData* pMsgData );

    private:
      //  レイアウトワーク一覧
      enum LytWkType
      {
        LYT_WK_TYPE_SCREEN  = 0,
        LYT_WK_TYPE_MAX,
      };

      void _Clear()
      {
        m_pUtilHeap           = NULL;
        m_pMsgWndTextRootPane = NULL;
        m_state               = STATE_TYPE_NONE;
        m_nowHelpTextId       = 0;

        m_pCommonMessageWindow  = NULL;
      }

      void _Initialize2D( void* pAppLytBuff );

      void _UpdateState();
      void _SetState( const StateType newStateType );

      //  変数宣言一覧
      INIT_PARAM                          m_initParam;
      StateType                           m_state;

      gfl2::lyt::LytPane*                 m_pMsgWndTextRootPane;
      app::util::Heap*                    m_pUtilHeap;

      u32                                 m_nowHelpTextId;

      App::Tool::CommonMessageWindowManipulator*  m_pCommonMessageWindow;
  };

GFL_NAMESPACE_END( config )
GFL_NAMESPACE_END( app )

#endif // NIJI_PROJECT_CONFIG_UPPER_UI_DRAW_H_INCLUDED 
