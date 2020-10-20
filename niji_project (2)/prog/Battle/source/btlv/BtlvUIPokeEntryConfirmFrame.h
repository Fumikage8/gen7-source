//=============================================================================
/**
 * @file	 BtlvUIPokeEntryConfirmFrame.h
 * @brief	 バトルUIポケモン参加確認画面の制御フレーム
 * @author yuto_uchida
 * @date	 2015.04.23
 */
//=============================================================================
#ifndef __BTLV_UI_POKE_ENTRY_CONFIRM_FRAME_H__
#define __BTLV_UI_POKE_ENTRY_CONFIRM_FRAME_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"

#include "Battle/source/btlv/app/BattleUIPokeEntryConfirmLowerDraw.h"
#include "Battle/source/btlv/app/BattleUIDataManager.h"

#include "Battle/include/battle_SetupParam.h"

GFL_NAMESPACE_BEGIN( btl )

  /** バトルポケモン参加確認の制御フレーム */
  class BattleUIPokeEntryConfirmFrame : public applib::frame::CellSuper, public app::BattleUIlPokeEntryConfirmLowerDrawDelegate
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(BattleUIPokeEntryConfirmFrame);
  public:
    //  フレームの状態
    enum StateType
    {
      STATE_TYPE_NONE = 0,
      STATE_TYPE_LOAD,
      STATE_TYPE_OPEN,
      STATE_TYPE_UPDATE,
      STATE_TYPE_END_ENTER,
      STATE_TYPE_CLOSE,
      STATE_TYPE_END,
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
          pTmpDevHeap         = NULL;
          pAsyncFileManager   = NULL;
          pRenderingManager   = NULL;
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
    BattleUIPokeEntryConfirmFrame(void);

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~BattleUIPokeEntryConfirmFrame();

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

    //------------------------------------------------------------------
    /**
     * @brief 強制終了通知
     */
    //------------------------------------------------------------------
    void End();

    //  決定入力用リスナ
    virtual bool OnEnterInput( app::BattleUIPokeEntryConfirmLowerDraw* pDrawInst, app::BattleUIPokeEntryConfirmtLowerButtonCtrl* pButtonCtrl, s32 buttonId );

    /// キー入力通知用リスナ
    virtual bool OnKeyAction( app::BattleUIPokeEntryConfirmLowerDraw* pDrawInst, app::BattleUIPokeEntryConfirmtLowerButtonCtrl* pButtonCtrl, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  private:
    enum
    {
      //  このフレームと描画を動かすのに必要なheapサイズを定義
      //  200Kbyte
      SIZE_MY_APP         = 1024 * 200,

      //  400kbyte
      SIZE_MY_APPDEV      = 1024 * 400,

      //  2Kbyte
      SIZE_FRAME_MANAGER  = 4096,
    };

    // 状態ステート設定
    void _SetState( const StateType newStateType );

    //  変数クリア
    void _Clear()
    {
      m_state             = STATE_TYPE_NONE;
      m_pDraw             = NULL;
      m_pDrawButtonCtrl   = NULL;
      m_pArcReadBuff      = NULL;
      m_pAppHeap          = NULL;
      m_pManagerHeapBase  = NULL;
      m_pAppParam         = NULL;

      m_pDrawParamPartyList = NULL;
      m_drawParamPartyListNumIdx  = 0;
      m_drawParamPartyListIdxMax  = 0;
      m_bEnd              = false;
    }

    static const s32 s_arcId;

    APP_PARAM*                                      m_pAppParam;
    ::app::util::Heap*                              m_pAppHeap;
    gfl2::heap::HeapBase*                           m_pManagerHeapBase;

    app::BattleUIPokeEntryConfirmLowerDraw*         m_pDraw;
    app::BattleUIPokeEntryConfirmtLowerButtonCtrl*  m_pDrawButtonCtrl;

    StateType                                       m_state;
    void*                                           m_pArcReadBuff;

    app::BattleUIPokeEntryConfirmLowerDraw::PARTY_DATA* m_pDrawParamPartyList;
    u32                                                 m_drawParamPartyListNumIdx;
    u32                                                 m_drawParamPartyListIdxMax;

    bool                                                m_bEnd;
  };

GFL_NAMESPACE_END( btlv )


#endif // __BTLV_UI_POKE_ENTRY_CONFIRM_FRAME_H__
