//======================================================================
/**
 * @file BattleUIBGFrontLowerDraw.h
 * @date 2015/07/28 12:47:37
 * @author uchida_yuto
 * @brief バトル下画面のBG手前描画
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if !defined __BATTLE_UI_BG_FRONT_LOWER_DRAW_H_INCLUDED__
#define __BATTLE_UI_BG_FRONT_LOWER_DRAW_H_INCLUDED__
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>

//  nijiのインクルード
#include "AppLib/include/ui/UIView.h"
#include "Battle/include/battle_SetupParam.h"

//  前方宣言
//  前方宣言
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Tool )

  class CommonMessageWindowManipulator;

GFL_NAMESPACE_END( Tool )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class AppRenderingManager;
  class Heap;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  /**
   * @class BattleUIBGFrontLowerDraw
   */
  class BattleUIBGFrontLowerDraw : public ::app::ui::UIView
  {
    GFL_FORBID_COPY_AND_ASSIGN(BattleUIBGFrontLowerDraw);
  public:
    //  コンストラクタに引き渡す初期化パラメータ(引数の数が増えたので構造体にまとめる)
    //  ポインタ先のメモリ開放するとハングするので注意
    struct INIT_PARAM
    {
      //  レイアウトバイナリデータ
      void*                             pLytBinData;

      //  UIView描画で使うレンダリング
      //  上画面にモデルを描画するので引数 pRenderinManager には上画面のポケモンモデル描画設定したのを渡さないといけない
      ::app::util::AppRenderingManager*   pRenderinManager;

      //  レーダーチャートのロードで必要
      gfl2::fs::AsyncFileManager*       pAsyncFileManager;

      //  クラス内で使うヒープ
      ::app::util::Heap*                pHeap;

      //  使用するメッセージのポインタ（GARC_message_btl_app_DATのデータでないとアウト）
      gfl2::str::MsgData*               pBtlAppMsgData;
      print::WordSet*                   pBtlWordSet;

      INIT_PARAM()
      {
        pLytBinData             = NULL;
        pRenderinManager        = NULL;
        pHeap                   = NULL;
        pAsyncFileManager       = NULL;
        pBtlAppMsgData          = NULL;
        pBtlWordSet             = NULL;
      }
    };

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      初期設定
    * @param[in]  rInitParam       : 初期化パラメータ
    */
    //--------------------------------------------------------------------------------------------
    BattleUIBGFrontLowerDraw( const INIT_PARAM& rInitParam )
    : ::app::ui::UIView( rInitParam.pHeap )
    {
      GFL_ASSERT( rInitParam.pLytBinData );
      GFL_ASSERT( rInitParam.pRenderinManager );
      GFL_ASSERT( rInitParam.pHeap );
      GFL_ASSERT( rInitParam.pBtlAppMsgData );
      GFL_ASSERT( rInitParam.pBtlWordSet );

      _Clear();

      m_initParam = rInitParam;
    }

    virtual ~BattleUIBGFrontLowerDraw();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      初期化処理
    * @param[in]  pLytBinData       : レイアウトデータ
    * @param[in]  pRenderingManager : 描画管理
    * @param[in]  pUtilHeap         : ヒープ
    * @return     true 処理終了 / false 処理が終了していないので再度呼び出す
    * @note
    *         返り値が true になるまで呼び出す
    */
    //--------------------------------------------------------------------------------------------
    bool InitFunc();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  終了処理
    * @return true 処理終了 / false 処理が終了していないので再度呼び出す
    * @note
    *         返り値が true になるまで
    *         この関数を呼び出すとInitFunc()で初期化しないと使えないので注意
    *         使うケースとしては画面をもう使わないときの後処理に使う
    */
    //--------------------------------------------------------------------------------------------
    bool EndFunc();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  画面を開く
    */
    //--------------------------------------------------------------------------------------------
    void Open();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  画面を閉じる
    * @note
    *         画面を非表示にする時にはこれを使う
    *         SetVisible関数で非表示にしても3Dモデル表示は非表示にできないから
    */
    //--------------------------------------------------------------------------------------------
    void Close();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  初期化パラメータを取得
    * @return 初期化パラメータ参照を返す
    */
    //--------------------------------------------------------------------------------------------
    const INIT_PARAM& GetInitParam() const { return m_initParam; }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  更新処理
    *         子ウインドウについても自動的にコールする(子→親の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Update();

    //------------------------------------------------------------------
    /**
     * @brief 天候設定
     * @param weathre : 設定する天候
     * @note  天候アイコン挙動が変わる
     */
    //------------------------------------------------------------------
    void SetWeather( const BtlWeather weather );

    //------------------------------------------------------------------
    /**
     * @brief 暗転フィルターの有効設定
     * @param bEnable : 有効設定
     */
    //------------------------------------------------------------------
    void SetEnableDimFilterLayout( const bool bEnable );

    //------------------------------------------------------------------
    /**
      * @brief   メッセージウィンドウの表示開始
      */
    //------------------------------------------------------------------
    void ShowMessageWindow( const u32 strId, const bool bShowTimerIcon );

    //------------------------------------------------------------------
    /**
      * @brief   メッセージウィンドウの非表示開始
      */
    //------------------------------------------------------------------
    void HideMessageWindow();

  protected:
    //--------------------------------------------------------------------------------------------
    /**
    * @brief  描画処理
    *         子ウインドウについても自動的にコールする(親→子の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  private:
    enum
    {
      LYT_WORK_NORMAL,
      LYT_WORK_MAX,
    };

    enum WeatherIconState
    {
      WEATHER_ICON_STAET_IDLE = 0,
      WEATHER_ICON_STAET_IN,
      WEATHER_ICON_STAET_OUT
    };

    //  各天候の対応したアニメid
    struct WeatherAnimeData
    {
      //  アイコン登場
      s32 iconInId;
      //  アイコン退場
      s32 iconOutId;
    };

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  2D初期化処理
    * @note
    *     レイアウトデータから描画に必要な生成をしている
    */
    //--------------------------------------------------------------------------------------------
    void _Init2D();

    void _Clear()
    {
      m_bInit                 = false;

      m_initSeq               = 0;
      m_endSeq                = 0;
      m_iconWeatherInAnimeId  = 0;
      m_iconWeatherOutAnimeId = 0;

      m_pCommonMessageWindow  = NULL;
      m_pMaskScreenPane       = NULL;

      m_weatherIconState      = WEATHER_ICON_STAET_IDLE;
      m_newIconWeather        = BTL_WEATHER_NONE;
      m_nowIconWeather        = BTL_WEATHER_NONE;
    }

    //  天候アイコン登場
    bool _InWeatherIcon( const BtlWeather weather );
    //  天候アイコン退場
    void _OutWeatherIcon( const BtlWeather weather );

    INIT_PARAM                                                m_initParam;

    u32   m_initSeq;
    u32   m_endSeq;

    BtlWeather  m_newIconWeather;
    BtlWeather  m_nowIconWeather;

    gfl2::lyt::LytPane* m_pMaskScreenPane;

    u32 m_iconWeatherInAnimeId;
    u32 m_iconWeatherOutAnimeId;

    bool  m_bInit;

    WeatherIconState  m_weatherIconState;

    App::Tool::CommonMessageWindowManipulator*  m_pCommonMessageWindow;

    static const WeatherAnimeData ms_aWeatherAnimeDataTable[ BTL_WEATHER_MAX ];
  };

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )

#endif // __BATTLE_UI_BG_FRONT_LOWER_DRAW_H_INCLUDED__