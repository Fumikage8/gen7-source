//=============================================================================
/**
 * @file	 BtlvUIYesNoFrame.h
 * @brief	 バトルUI2選択画面の制御フレーム
 * @author yuto_uchida
 * @date	 2015.03.05
 */
//=============================================================================
#ifndef __BTLV_UI_YESNO_FRAME_H__
#define __BTLV_UI_YESNO_FRAME_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include "AppLib/include/Frame/AppFrameCellSuper.h"

#include "Battle/source/btlv/app/BattleUIYesNoLowerDraw.h"

//  前方宣言
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )

  class KeyRepeatController;

GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( btl )

  /** バトル２選択画面の制御フレーム */
  class BattleUIYesNoFrame : public applib::frame::CellSuper, public app::BattleUIYesNoLowerDrawDelegate
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(BattleUIYesNoFrame);
  public:
    //  選択できる種類
    enum SelectType
    {
      SELECT_TYPE_YES = 0,
      SELECT_TYPE_NO,

      SELECT_TYPE_MAX
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
      STATE_TYPE_END,
    };

    //  フレームアプリ用のパラメータ
    typedef struct _tag_app_param
    {
      /** 入力用パラメータ */
      struct _tag_in_param
      {
        //  フレーム内で使用する親ヒープ
        gfl2::heap::HeapBase *p_parent_app_heap;
        gfl2::heap::HeapBase *p_parent_dev_heap;

        //  一時使用用のヒープ(ファイル圧縮解凍で使う)
        gfl2::heap::HeapBase* p_tmp_dev_heap;

        //ファイルマネージャー
        gfl2::fs::AsyncFileManager* p_async_file_manager;

        //  描画パイプライン
        ::app::util::AppRenderingManager* p_rendering_manager;

        //  使用するメッセージのポインタ（GARC_message_btl_app_DATのデータでないとアウト）
        gfl2::str::MsgData* p_btl_app_msg_data;
        print::WordSet*     p_btl_word_set;

        //  カーソルのレイアウトバッファ(使う側が事前にレイアウトデータを作成)
        void* p_menu_cursor_lyt_buffer;

        //  入力SEを使うか
        bool                b_use_input_se;

        //  Bボタンを使えるか
        bool                b_use_b_button;

        //  ユーザー入力可能か
        bool                b_user_input;

        //  初回選択
        SelectType          first_select_type;

        //  表示する選択項目のメッセージID
        s32 a_select_item_text_id_list[ app::BattleUIYesNoLowerDraw::SELECT_ITEM_MAX ];

      } in_param;

      /** 出力パラメータ( フレーム処理終了時に結果反映 ) */
      struct _tag_out_param
      {
        SelectType resultSelectItemIdx;

        //  フレームが破棄されたらtrueになる
        bool  bEnd;

        void Clear()
        {
          bEnd                 = false;
          resultSelectItemIdx  = SELECT_TYPE_YES;
        }

      } out_param;

    } APP_PARAM;

    //------------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     */
    //------------------------------------------------------------------
    BattleUIYesNoFrame(void);

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~BattleUIYesNoFrame();

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
     * @brief 選択した選択項目
     */
    //------------------------------------------------------------------
    virtual bool OnPushSelectItem( const u32 in_itemIdx );

    //------------------------------------------------------------------
    /**
     * @brief キーとスティックの入力結果
     */
    //------------------------------------------------------------------
    virtual void OnKeyAction( app::BattleUIYesNoLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  private:
    enum
    {
      //  このフレームと描画を動かすのに必要なheapサイズを定義
      //  100Kbyte
      SIZE_MY_APP         = 1024 * 100,

      //  500Kbyte
      SIZE_MY_APPDEV      = 1024 * 100,

      //  2Kbyte
      SIZE_FRAME_MANAGER  = 4096,
    };

    // 状態ステート設定
    void _SetState( const StateType newStateType );

    //  変数クリア
    void _Clear()
    {
      m_state             = STATE_TYPE_NONE;

      m_pArcReadBuff      = NULL;
      m_pDraw             = NULL;
      m_pAppHeap          = NULL;
      m_pManagerHeapBase  = NULL;
      m_pAppParam         = NULL;
      m_pSelectListYKeyRepertCtrl = NULL;
      m_selectItemIdx     = 0;
      m_bEnd              = false;
    }

    static const s32 s_arcId;

    APP_PARAM*                        m_pAppParam;
    ::app::util::Heap*                m_pAppHeap;
    gfl2::heap::HeapBase*             m_pManagerHeapBase;

    app::BattleUIYesNoLowerDraw*      m_pDraw;
    StateType                         m_state;
    void*                             m_pArcReadBuff;
    bool                              m_bEnd;

    ::app::util::KeyRepeatController*     m_pSelectListYKeyRepertCtrl;

    s32 m_selectItemIdx;
  };

GFL_NAMESPACE_END( btlv )


#endif // __BTLV_UI_YESNO_FRAME_H__
