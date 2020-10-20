//======================================================================
/**
 * @file BattleVideoPlayerUppGameResultView.h
 * @date 2015/09/24 16:23:10
 * @author uchida_yuto
 * @brief アプリの上画面ゲーム結果View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if !defined __BATTLE_VIDEO_PLAYER_UPP_GAME_RESULT_VIEW_H_INCLUDED__
#define __BATTLE_VIDEO_PLAYER_UPP_GAME_RESULT_VIEW_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

//  nijiのインクルード
#include "AppLib/include/ui/UIView.h"

#include "NetApp/BattleVideoPlayer/include/BattleVideoPlayerProcAppParam.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(print)

  class WordSet;

GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class PokeIconToolEx;
  class AppToolTrainerIconRendering;
  class AppToolBaseGrpFont;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

  class PokeIconTexPool;
  class PokeIconTexData;

GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

  /**
   * @class UppGameResultView
   * @brief アプリの上画面ゲーム結果View
   */
  class UppGameResultView : public app::ui::UIView
  {
    GFL_FORBID_COPY_AND_ASSIGN(UppGameResultView);

  public:
    //  状態一覧
    enum State
    {
      STATE_IDLE  = 0,
    };

    enum
    {
      PLAYER_INFO_POS_L  = 0,  //  左
      PLAYER_INFO_POS_R,       //  右

      PLAYER_INFO_POS_MAX
    };

    //  初期化パラメータ
    struct INIT_PARAM
    {
      INIT_PARAM()
      {
        pHeap           = NULL;
        pRenderManager  = NULL;
        pLytBinData     = NULL;
        pAppMsgData     = NULL;
        pWordSet        = NULL;
        pGrpFont        = NULL;

        pTrainerIconRendering = NULL;
        pPokeIconTexPool      = NULL;
      }

      app::util::Heap* pHeap;
      app::util::AppRenderingManager* pRenderManager;
      void* pLytBinData;
      gfl2::str::MsgData* pAppMsgData;
      print::WordSet* pWordSet;

      app::tool::AppToolBaseGrpFont*          pGrpFont;
      App::Tool::PokeIconTexPool*             pPokeIconTexPool;
      app::tool::AppToolTrainerIconRendering* pTrainerIconRendering;

      u32 aReservTrainerIconId[ PLAYER_INFO_POS_MAX ];
    };

    //------------------------------------------------------------------
    /**
      * @brief   コンストラクタ
      * @param[in] pHeap 使用するメモリ
      */
    //------------------------------------------------------------------
    UppGameResultView( INIT_PARAM& rInitParam );

    virtual ~UppGameResultView();

    //------------------------------------------------------------------
    /**
      * @brief   初期化
      */
    //------------------------------------------------------------------
    bool InitFunc();

    //------------------------------------------------------------------
    /**
      * @brief   終了処理
      */
    //------------------------------------------------------------------
    bool EndFunc();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  更新処理
    *         子ウインドウについても自動的にコールする(子→親の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Update();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  描画処理
    *         子ウインドウについても自動的にコールする(親→子の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Draw(  gfl2::gfx::CtrDisplayNo displayNo );

    //------------------------------------------------------------------
    /**
      * @brief   状態設定
      * @param[in] newStaet 設定する状態
      */
    //------------------------------------------------------------------
    void SetState( const State newStaet );

    //------------------------------------------------------------------
    /**
      * @brief   ビデオ情報を設定
      * @param[in] pVideoData 画面更新するビデオデータ
      */
    //------------------------------------------------------------------
    void SetVideoData( NetAppLib::BattleVideoPlayer::VIDEO_DATA* pVideoData );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief   可視状態の設定
    *          不可視状態時は描画、操作通知ともに無効となる。
    *
    * @param   isVisible  可視状態に設定するならtrueを指定
    */
    //--------------------------------------------------------------------------------------------
    void SetDrawVisible( bool isVisible );

  private:
    //  作成するレイアウトワークの種類
    enum
    {
      LYT_WORK_NORMAL  = 0,
      LYT_WORK_MAX
    };

    //  プレイヤー情報のペイン
    struct PLAYER_INFO_PANE
    {
      gfl2::lyt::LytParts*    pFaceIconRootPane;
      gfl2::lyt::LytPicture*  pFaceIconPane;
      gfl2::lyt::LytPane*     pFaceIconFramePane;
      gfl2::lyt::LytTextBox*  pNameTextBoxPane;

      PLAYER_INFO_PANE()
      {
        pFaceIconRootPane   = NULL;
        pFaceIconPane       = NULL;
        pFaceIconFramePane  = NULL;
        pNameTextBoxPane    = NULL;
      }
    };

    //  ポケモンアイコン
    struct POKE_ICON
    {
      gfl2::lyt::LytParts*    pRootPane;
      gfl2::lyt::LytPicture*  pPicPane;

      u32 iconId;
      u32 dieAnimeId;

      POKE_ICON()
      {
        pRootPane = NULL;
        pPicPane  = NULL;
        iconId    = 0;
        dieAnimeId  = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
      }
    };

    //------------------------------------------------------------------
    /**
      * @brief   2D画面の作成
      * @param   pAppLytBuff レイアウトデータのバイナリポインタ
      */
    //------------------------------------------------------------------
    void _Initialize2D( void* pAppLytBuff, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet );

    //------------------------------------------------------------------
    /**
      * @brief   画面変更アニメ停止
      */
    //------------------------------------------------------------------
    void _StopAnimeScreen();

    //------------------------------------------------------------------
    /**
      * @brief   変数の初期化はここに
      */
    //------------------------------------------------------------------
    void _Clear()
    {
      m_state           = STATE_IDLE;
      m_initSeq         = 0;
      m_endSeq          = 0;
      m_screenAnimeId   = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;

      m_pResultGrpFontPicPane = NULL;
      m_pTurnNumTextBoxPane   = NULL;
      m_pMessageTextBoxPane   = NULL;
      m_pRootPane             = NULL;
      m_bDrawVisible          = true;

      gfl2::std::MemClear( m_pPokeIconTexDataArray, sizeof( m_pPokeIconTexDataArray ) );
    }

    State                           m_state;

    u32                             m_initSeq;
    u32                             m_endSeq;

    u32                             m_screenAnimeId;

    App::Tool::PokeIconTexData*     m_pPokeIconTexDataArray[ pml::PokeParty::MAX_MEMBERS ];

    gfl2::lyt::LytPane*             m_pRootPane;

    gfl2::lyt::LytTextBox*          m_pTurnNumTextBoxPane;
    gfl2::lyt::LytTextBox*          m_pMessageTextBoxPane;
    gfl2::lyt::LytPicture*          m_pResultGrpFontPicPane;

    POKE_ICON                       m_aPokeIcon[ pml::PokeParty::MAX_MEMBERS ];
    PLAYER_INFO_PANE                m_aPlayerInfoPane[ PLAYER_INFO_POS_MAX ];

    bool                            m_bDrawVisible;
    INIT_PARAM                      m_initParam;

    /*  battlefes  */
    gfl2::lyt::LytTextBox*          m_pTextBoxPane_BattleFesPartnerName_0;
    gfl2::lyt::LytTextBox*          m_pTextBoxPane_BattleFesPartnerName_1;
  };

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetApp)

#endif // __BATTLE_VIDEO_PLAYER_UPP_GAME_RESULT_VIEW_H_INCLUDED__