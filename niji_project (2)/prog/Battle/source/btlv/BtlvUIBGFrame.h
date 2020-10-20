//=============================================================================
/**
 * @file	 BtlvUIBGFrame.h
 * @brief	 バトルUIBG画面の制御フレーム
 * @author yuto_uchida
 * @date	 2015.03.23
 */
//=============================================================================
#ifndef __BTLV_UI_BG_FRAME_H__
#define __BTLV_UI_BG_FRAME_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"

#include "Battle/source/btlv/app/BattleUIBGLowerDraw.h"
#include "Battle/include/battle_SetupParam.h"

GFL_NAMESPACE_BEGIN( btl )

  //  前方宣言
  GFL_NAMESPACE_BEGIN( app )

    class BattleUIDataManager;
    class BattleUIBGFrontLowerDraw;

  GFL_NAMESPACE_END( app )

  /** バトルBG画面の制御フレーム */
  class BattleUIBGFrame : public applib::frame::CellSuper, app::BattleUIBGLowerDrawDelegate
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(BattleUIBGFrame);
  public:
    //  フレームの状態
    enum StateType
    {
      STATE_TYPE_NONE = 0,
      STATE_TYPE_LOAD,
      STATE_TYPE_WAITING,
      STATE_TYPE_OPEN,
      STATE_TYPE_UPDATE,
      STATE_TYPE_CLOSE,
      STATE_TYPE_END
    };

    //  フレームアプリ用のパラメータ
    typedef struct _tag_app_param
    {
      /** 入力用パラメータ */
      struct _tag_in_param
      {
        //  フレーム内で使用する親ヒープ
        gfl2::heap::HeapBase* pParentAppHeap;
        gfl2::heap::HeapBase* pParentDevHeap;

        //  一時使用用のヒープ(ファイル圧縮解凍で使う)
        gfl2::heap::HeapBase* pTmpDevHeap;

        //ファイルマネージャー
        gfl2::fs::AsyncFileManager* pAsyncFileManager;

        //  描画パイプライン
        ::app::util::AppRenderingManager* pRenderingManager;

        //  バトルパラメータのアクセサー
        app::BattleUIDataManager* pBtlParamAccessror;

        _tag_in_param()
        {
          pParentAppHeap      = NULL;
          pParentDevHeap      = NULL;
          pAsyncFileManager   = NULL;
          pRenderingManager   = NULL;
          pTmpDevHeap         = NULL;
          pBtlParamAccessror  = NULL;
        }

      } in_param;

      /** 出力パラメータ( フレーム処理終了時に結果反映 ) */
      struct _tag_out_param
      {
        //  フレームが破棄されたらtrueになる
        bool  bEnd;

        void Clear()
        {
          bEnd                  = false;
        }

        _tag_out_param()
        {
          Clear();
        }

      } out_param;

    } APP_PARAM;

    //------------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     */
    //------------------------------------------------------------------
    BattleUIBGFrame(void);

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~BattleUIBGFrame();

  public:

    //------------------------------------------------------------------
    /**
     * @brief	  動作に必要な引数を登録
     * @note
     *    必ず最初に呼び出さないと後の動作保障はなし
     */
    //------------------------------------------------------------------
    void Initialize( APP_PARAM* pAppParam);

    //------------------------------------------------------------------
    /**
     * @brief	  PROCの初期化を行う関数
     * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
     */
    //------------------------------------------------------------------
    virtual applib::frame::Result InitFunc(void);

    //------------------------------------------------------------------
    /**
     * @brief	  PROCのメイン更新を行う関数
     * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
     */
    //------------------------------------------------------------------
    virtual applib::frame::Result UpdateFunc(void);

    //------------------------------------------------------------------
    /**
     * @brief	  PROCのメイン描画を行う関数
     *          純粋仮想ではありません。
     */
    //------------------------------------------------------------------
    virtual void DrawFunc(gfl2::gfx::CtrDisplayNo no);

    //------------------------------------------------------------------
    /**
     * @brief	  PROCの終了を行う関数
     * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
     */
    //------------------------------------------------------------------
    virtual applib::frame::Result EndFunc(void);

    //  使用可能か！
    bool IsReady()
    {
      if( m_state == STATE_TYPE_LOAD )
      {
        return  false;
      }

      return  true;
    }

    //------------------------------------------------------------------
    /**
     * @brief 開始通知
     * @return true -> 成功 / false -> 失敗
     * @note
     *  待機中のステート( m_state == STATE_TYPE_WAITING )でないと処理をして成功しない
     */
    //------------------------------------------------------------------
    bool Start();

    //------------------------------------------------------------------
    /**
     * @brief 強制終了通知
     * @note
     *  画面が閉じた後にインスタンスが破棄されるので注意
     */
    //------------------------------------------------------------------
    void End();

    //------------------------------------------------------------------
    /**
     * @brief 強制破棄通知
     * @note
     *  画面が閉じる演出なしにいきなりインスタンスが破棄
     */
    //------------------------------------------------------------------
    void Die();

    //------------------------------------------------------------------
    /**
     * @brief 天候設定
     */
    //------------------------------------------------------------------
    void SetWeather( const BtlWeather weather );

    //------------------------------------------------------------------
    /**
      * @brief   ルール設定
      */
    //------------------------------------------------------------------
    void SetRule( const BtlRule rule );

    //------------------------------------------------------------------
    /**
      * @brief   BGカバー表示設定
      */
    //------------------------------------------------------------------
    void SetVisibleCurverBG( const bool bFlg );

    //------------------------------------------------------------------
    /**
      * @brief   おていれを表示イベント
      */
    //------------------------------------------------------------------
    void EventOteireDisp();

    //------------------------------------------------------------------
    /**
      * @brief   おていれ表示中か
      */
    //------------------------------------------------------------------
    bool IsOteireDisp();

    //------------------------------------------------------------------
    /**
      * @brief   おていれイベントのユーザー入力設定
      */
    //------------------------------------------------------------------
    void SetEnableInputEventOteire( const bool bFlg );

    //------------------------------------------------------------------
    /**
      * @brief   おていれを押したか
      */
    //------------------------------------------------------------------
    bool IsPushOteire();

    //------------------------------------------------------------------
    /**
     * @brief 下画面の暗転フィルターの有効設定
     * @param bEnable : 有効設定
     */
    //------------------------------------------------------------------
    void SetEnableDimFilterLayout( const bool bEnable );

    //------------------------------------------------------------------
    /**
      * @brief   通信待機メッセージウィンドウ表示
      */
    //------------------------------------------------------------------
    void ShowWaitComMesssageWindow();

    //------------------------------------------------------------------
    /**
      * @brief   通信待機メッセージウィンドウ表示
      */
    //------------------------------------------------------------------
    void HideWaitComMesssageWindow();

    //  決定入力用リスナ
    virtual bool OnEnterInput( app::BattleUIBGLowerDraw* pDrawInst, s32 buttonId );

    //  キーとスティックの入力結果
    virtual void OnKeyAction( app::BattleUIBGLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

    //デバッグ用背景色変え
#if PM_DEBUG
    void UpdateDebugBgColorCahnge(void);
#endif //PM_DEBUG
  private:
    enum
    {
      //  このフレームと描画を動かすのに必要なheapサイズを定義
      //  200Kbyte
      SIZE_MY_APP         = 1024 * 200,

      //  700kbyte
      SIZE_MY_APPDEV      = 1024 * 550,

      //  2Kbyte
      SIZE_FRAME_MANAGER  = 4096
    };

    // 状態ステート設定
    void _SetState( const StateType newStateType );

    //  変数クリア
    void _Clear()
    {
      m_state             = STATE_TYPE_NONE;
      m_pDraw             = NULL;
      m_pFrontDraw        = NULL;
      m_pArcReadBuff      = NULL;
      m_pAppHeap          = NULL;
      m_pManagerHeapBase  = NULL;
      m_pAppParam         = NULL;

      m_bOpen             = false;
      m_bPushOteire       = false;
    }

    static const s32 s_arcId;

    APP_PARAM*                        m_pAppParam;
    ::app::util::Heap*                m_pAppHeap;
    gfl2::heap::HeapBase*             m_pManagerHeapBase;

    app::BattleUIBGLowerDraw*         m_pDraw;
    app::BattleUIBGFrontLowerDraw*    m_pFrontDraw;

    StateType                         m_state;
    void*                             m_pArcReadBuff;

    bool                              m_bOpen;
    bool                              m_bPushOteire;

    app::BattleUIBGLowerDraw::INIT_PARAM  m_drawInitParam;

#if PM_DEBUG
    int m_debugColorCnt;
#endif
  };

GFL_NAMESPACE_END( btlv )


#endif // __BTLV_UI_BG_FRAME_H__
