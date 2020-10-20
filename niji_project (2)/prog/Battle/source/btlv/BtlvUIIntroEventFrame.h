//======================================================================
/**
 * @file BtlvUIIntroEventFrame.h
 * @date 2015/08/24 17:52:20
 * @author uchida_yuto
 * @brief バトル開始時の導入イベントフレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if !defined __BTLV_UI_INTRO_EVENT_FRAME_H_INCLUDED__
#define __BTLV_UI_INTRO_EVENT_FRAME_H_INCLUDED__
#pragma once

#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"

#include "Battle/include/battle_SetupParam.h"

GFL_NAMESPACE_BEGIN( btl )

  //  前方宣言
  GFL_NAMESPACE_BEGIN( app )
    class BattleUIDataManager;
    class BattleUIIntroEventLowerDraw;
  GFL_NAMESPACE_END( app )

  /** バトル開始の導入フレーム */
  class BattleUIIntroEventFrame : public applib::frame::CellSuper
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(BattleUIIntroEventFrame);
  public:
    //  フレームの状態
    enum StateType
    {
      STATE_TYPE_NONE = 0,
      STATE_TYPE_LOAD,
      STATE_TYPE_IDLE,
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

        _tag_in_param()
        {
          pParentAppHeap      = NULL;
          pParentDevHeap      = NULL;
          pAsyncFileManager   = NULL;
          pRenderingManager   = NULL;
          pTmpDevHeap         = NULL;
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
    BattleUIIntroEventFrame(void);

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~BattleUIIntroEventFrame();

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

    //  処理の待機から実行開始
    void Start( const u32 delayFrame );

  private:
    enum
    {
      //  このフレームと描画を動かすのに必要なheapサイズを定義
      //  200Kbyte
      SIZE_MY_APP         = 1024 * 200,

      //  700kbyte
      SIZE_MY_APPDEV      = 1024 * 700,

      //  2Kbyte
      SIZE_FRAME_MANAGER  = 4096
    };

    // 状態ステート設定
    void _SetState( const StateType newStateType );

    //  変数クリア
    void _Clear()
    {
      m_state             = STATE_TYPE_NONE;
      m_pArcReadBuff      = NULL;
      m_pAppHeap          = NULL;
      m_pManagerHeapBase  = NULL;
      m_pAppParam         = NULL;
      m_pDraw             = NULL;
      m_startDelayFrame   = 0;
    }

    static const s32 s_arcId;

    APP_PARAM*                        m_pAppParam;
    ::app::util::Heap*                m_pAppHeap;
    gfl2::heap::HeapBase*             m_pManagerHeapBase;

    app::BattleUIIntroEventLowerDraw* m_pDraw;

    StateType                         m_state;
    void*                             m_pArcReadBuff;

    u32                               m_startDelayFrame;
  };

GFL_NAMESPACE_END( btlv )

#endif // __BTLV_UI_INTRO_EVENT_FRAME_H_INCLUDED__