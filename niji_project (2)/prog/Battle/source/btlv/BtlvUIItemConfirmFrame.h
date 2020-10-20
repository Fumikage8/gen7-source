//=============================================================================
/**
 * @file	 BtlvUIItemConfirmFrame.h
 * @brief	 バトルUIアイテム使用確認の制御フレーム
 * @author yuto_uchida
 * @date	 2015.04.07
 */
//=============================================================================
#ifndef __BTLV_UI_ITEM_CONFIRM_FRAME_H__
#define __BTLV_UI_ITEM_CONFIRM_FRAME_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "AppLib/include/Tool/ItemIcon.h"

#include "Battle/source/btlv/app/BattleUIItemConfirmLowerDraw.h"
#include "Battle/source/btlv/app/BattleUIDataManager.h"

GFL_NAMESPACE_BEGIN( itemman )
  class ITEM_MANAGER;
GFL_NAMESPACE_END( itemman )

GFL_NAMESPACE_BEGIN( btl )

  /** アイテム使用確認画面の制御フレーム */
  class BattleUIItemConfirmFrame : public applib::frame::CellSuper, public app::BattleUIItemConfirmLowerDrawDelegate
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(BattleUIItemConfirmFrame);
  public:
    //  フレームに通知するコマンド
    enum CmdType
    {
      CMD_TYPE_PUSH_BUTTON_ITEM_USE  = 0
    };

    //  フレームの状態
    enum StateType
    {
      STATE_TYPE_NONE = 0,
      STATE_TYPE_LOAD,
      STATE_TYPE_OPEN,
      STATE_TYPE_UPDATE,
      STATE_TYPE_ENTER_EVENT,
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
        gfl2::heap::HeapBase*             pParentAppHeap;
        gfl2::heap::HeapBase*             pParentDevHeap;

        //  一時使用用のヒープ(ファイル圧縮解凍で使う)
        gfl2::heap::HeapBase* pTmpDevHeap;

        //ファイルマネージャー
        gfl2::fs::AsyncFileManager*       pAsyncFileManager;

        //  描画パイプライン
        ::app::util::AppRenderingManager* pRenderingManager;

        //  アイテムカテゴリ
        item::BtlPocket                   itemPocketType;

        //  ボール使用不可能メッセージid(使用不可能であれば-1以外のメッセージid値が入っている)
        s32                               useBallErrorMessageId;

        //  アイテム選択対象のポケモンバトルパラメータ(対象選択用でないのであればNULL)
        const btl::BTL_POKEPARAM*         pTargetPokeParam;

        //  対象ポケモンパラメータ（シングルor乱入バトルのみ）
        const btl::BTL_POKEPARAM*         pSingleEntryPokeParam;

        //  バトルパラメータ
        app::BattleUIDataManager*         pBattleParamAcceesor;

        //  アイテムポケットリスト
        ::app::bag::POCKET_DATA*          pPocketDataList;

        //  カレントアイテムid(初回表示するアイテムid -1ならリストの先頭になる )
        u32                               currentItemId;

        //  描画の黒マスクを表示するか(バッグ画面からは出して、トップ画面からは出さないケースがある)
        bool                              bBGMaskDraw;

        // @fix MMCat[225]: ダブルバトルさしおさえ対処
        const btl::BTL_POKEPARAM*         pDoubleEntryPokeParam[2];   // 場に出てるポケモン
        u32 targetPokeIndex;  // 行動選択中のスロット番号

        _tag_in_param()
        {
          pParentAppHeap          = NULL;
          pParentDevHeap          = NULL;
          pTmpDevHeap             = NULL;
          pAsyncFileManager       = NULL;
          pRenderingManager       = NULL;
          pTargetPokeParam        = NULL;
          pSingleEntryPokeParam   = NULL;
          pBattleParamAcceesor    = NULL;
          pPocketDataList         = NULL;
          itemPocketType          = item::BTLPOCKET_BALL;
          useBallErrorMessageId   = -1;
          currentItemId           = ITEM_DUMMY_DATA;
          bBGMaskDraw             = true;
          pDoubleEntryPokeParam[0] = NULL;
          pDoubleEntryPokeParam[1] = NULL;
          targetPokeIndex          = 0;
        }

      } in_param;

      /** 出力パラメータ( フレーム処理終了時に結果反映 ) */
      struct _tag_out_param
      {
        //  フレームが破棄されたらtrueになる
        bool  bEnd;

        //  アイテムを使用するか（falseなら使用しないでキャンセルと解釈してよい）
        bool  bUse;

        u16   useItemId;

        void Clear()
        {
          bEnd        = false;
          bUse        = false;
          useItemId   = ITEM_DUMMY_DATA;
        }

      } out_param;

    } APP_PARAM;

    //------------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     */
    //------------------------------------------------------------------
    BattleUIItemConfirmFrame(void);

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~BattleUIItemConfirmFrame();

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
     * @note
     *  画面が閉じた後にインスタンスが破棄されるので注意
     */
    //------------------------------------------------------------------
    void End();

    //------------------------------------------------------------------
    /**
     * @brief 決定入力用リスナ
     */
    //------------------------------------------------------------------
    virtual bool OnEnterInput( app::BattleUIItemConfirmLowerDraw* pDrawInst, s32 buttonId );

    //------------------------------------------------------------------
    /**
     * @brief キーとスティックの入力結果
     */
    //------------------------------------------------------------------
    virtual void OnKeyAction( app::BattleUIItemConfirmLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

    //  ステート取得
    StateType GetState() const { return m_state; }

    //------------------------------------------------------------------
    /**
     * @brief コマンド通知
     */
    //------------------------------------------------------------------
    bool PushCmd( const CmdType type );

  private:
    enum
    {
      //  このフレームと描画を動かすのに必要なheapサイズを定義
      //  200Kbyte
      SIZE_MY_APP         = 1024 * 200,

      //  800Kbyte
      SIZE_MY_APPDEV      = 1024 * 800,

      //  2Kbyte
      SIZE_FRAME_MANAGER  = 4096
    };

    //  更新ステート内のサブステート
    enum UpdateSubState
    {
      UPDATE_SUB_STATE_INFO = 0,
      UPDATE_SUB_STATE_ERROR_MESSAGE
    };

    // 状態ステート設定
    void _SetState( const StateType newStateType );

    //  更新内のサブステート設定
    void _SetUpdateSubState( const UpdateSubState newState );

    //  アイテム表示内容を変更
    void _ChangeItemInfoData( const s32 pocketListIdx );

    //  にげるアイテムがつかえるか
    bool _IsUseEscItem( const u16 itemId );

    //  変数クリア
    void _Clear()
    {
      m_state                   = STATE_TYPE_NONE;
      m_updateSubState          = UPDATE_SUB_STATE_INFO;
      m_pArcReadBuff            = NULL;
      m_pDraw                   = NULL;
      m_pAppHeap                = NULL;
      m_pManagerHeapBase        = NULL;
      m_pAppParam               = NULL;
      m_pItemIconData           = NULL;
      m_pUseErrorMessageStrBuf  = NULL;
      m_currentPocketListIdx    = 0;
      m_bEnd                    = false;
    }

    static const s32 ms_arcId;
    static const u32 ms_tmpStrBufferLength;

    APP_PARAM*                              m_pAppParam;
    ::app::util::Heap*                      m_pAppHeap;
    gfl2::heap::HeapBase*                   m_pManagerHeapBase;

    app::BattleUIItemConfirmLowerDraw*      m_pDraw;

    StateType                               m_state;
    UpdateSubState                          m_updateSubState;

    void*                                   m_pArcReadBuff;
    ::app::tool::ItemIcon*                  m_pItemIconData;

    gfl2::str::StrBuf*                      m_pUseErrorMessageStrBuf;

    u16                                     m_currentPocketListIdx;
    bool                                    m_bEnd;
  };

GFL_NAMESPACE_END( btlv )


#endif // __BTLV_UI_ITEM_CONFIRM_FRAME_H__
