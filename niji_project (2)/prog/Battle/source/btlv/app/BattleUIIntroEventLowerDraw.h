//======================================================================
/**
 * @file BattleUIIntroEventLowerDraw.h
 * @date 2015/08/24 18:06:52
 * @author uchida_yuto
 * @brief バトル下画面の導入演出Draw
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if !defined __BATTLE_UI_INTRO_EVENT_LOWER_DRAW_H_INCLUDED__
#define __BATTLE_UI_INTRO_EVENT_LOWER_DRAW_H_INCLUDED__
#pragma once

#include "AppLib/include/ui/UIView.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  /** バトル下画面の登場演出画面 */
  class BattleUIIntroEventLowerDraw : public ::app::ui::UIView, public ::app::ui::UIInputListener
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleUIIntroEventLowerDraw );  // コピーを禁止

    public:
      // ステート
      enum StateType
      {
        STATE_TYPE_NONE         = 0,
        STATE_TYPE_RUN,
        STATE_TYPE_END
      };

      enum
      {
        SELECT_ITEM_MAX  = 2,
      };

      //  クラス初期化時に必要なパラメータ
      typedef struct _tag_init_param
      {
        ::app::util::Heap*                pUtilHeap;
        ::app::util::AppRenderingManager* pRenderingManager;

        //  レイアウトバイナリ(あらかじめ使う側がレイアウトデータを作成して渡す。)
        void* pLytBuff;

        _tag_init_param()
        {
          pUtilHeap             = NULL;
          pRenderingManager     = NULL;
          pLytBuff              = NULL;
        }

      } INIT_PARAM;

      /**
      * @brief コンストラクタ
      */
      BattleUIIntroEventLowerDraw( const INIT_PARAM& param );

      /**
      * @brief デストラクタ
      */
      virtual ~BattleUIIntroEventLowerDraw();

      /**
        * @brief 開始
        */
      void Start( void ) { _SetState( STATE_TYPE_RUN ); }

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

    private:
      //  レイアウトワーク一覧
      enum LytWkType
      {
        LYT_WK_TYPE_SCREEN  = 0,
        LYT_WK_TYPE_MAX,
      };

      enum
      {
        CIRCLE_PANE_NUM = 30,
        CIRCLE_ANIME_ID_NUM = 6
      };

      struct CIRCLE_DATA
      {
        u32 paneId;
        u32 aAnimeId[ CIRCLE_ANIME_ID_NUM ];
      };

      struct CIRCLE_EXE_DATA
      {
        u32   animeId;
      };

      void _Clear()
      {
        gfl2::std::MemClear( m_aCircleExeData, sizeof( m_aCircleExeData ) );

        m_subSeq  = 0;
        m_maskCircleNum = 0;
        m_state   = STATE_TYPE_NONE;
      };

      void _Initialize2D( void* pAppLytBuff );

      void _UpdateState();
      void _SetState( const StateType newStateType );

      //  変数宣言一覧
      INIT_PARAM  m_initParam;
      StateType   m_state;

      u32         m_subSeq;
      u32         m_maskCircleNum;

      //  配置している円ペインのそれぞれ再生しているデータ
      CIRCLE_EXE_DATA m_aCircleExeData[ CIRCLE_PANE_NUM ];

      //  配置している円ペインのそれぞれのアニメid配列
      static const struct CIRCLE_DATA ms_aCircleData[ CIRCLE_PANE_NUM ];
  };

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )

#endif // __BATTLE_UI_INTRO_EVENT_LOWER_DRAW_H_INCLUDED__