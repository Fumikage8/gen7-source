//=============================================================================
/**
 * @file	 BtlvUISkillSelectFrame.h
 * @brief	 バトルUI技選択画面の制御フレーム
 * @author yuto_uchida
 * @date	 2015.03.16
 * @note
 *    フレーム内で画面を開いているが、閉じるなどの終了通知は使う側で行う必要がある。
 *    わざ選択決定しても、状況によって再度選択やり直しがあるから。
 *
 *    使用側は毎回フレームのメモリ確保してフレーム管理に登録する必要がある
 */
//=============================================================================
#ifndef __BTLV_UI_SKILL_SELECT_FRAME_H__
#define __BTLV_UI_SKILL_SELECT_FRAME_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"

#include "Battle/source/btlv/app/BattleUIDataManager.h"
#include "Battle/source/btlv/app/BattleUISkillSelectLowerDraw.h"
#include "Battle/source/btlv/BtlvUISkillStatusFrame.h"

//  前方宣言
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )

  class KeyRepeatController;

GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( btl )

  /** バトル技選択画面の制御フレーム */
  class BattleUISkillSelectFrame : public applib::frame::CellSuper, public app::BattleUISkillSelectLowerDrawDelegate
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(BattleUISkillSelectFrame);
  public:
    //  外部コマンド通知
    enum CmdType
    {
      //  わざ項目ボタンの１番目を押す
      CMD_TYPE_PUSH_SKILL_BUTTON_0  = 0,
      // 全力技項目ボタンを押す
      CMD_TYPE_PUSH_BUTTON_ZENRYOKU,
      // 全力技 技項目の2番目を押す
      CMD_TYPE_PUSH_BUTTON_ZENRYOKU_SKILL_BUTTON_1 ,
    };

    //  フレームの状態
    enum StateType
    {
      STATE_TYPE_NONE = 0,
      STATE_TYPE_LOAD,
      STATE_TYPE_OPEN,
      STATE_TYPE_UPDATE,
      STATE_TYPE_END_ENTER,
      STATE_TYPE_SKILL_STATUS,
      STATE_TYPE_CLOSE,
      STATE_TYPE_END
    };

    //  処理結果状態
    enum ResultType
    {
      RESULT_TYPE_NONE = 0,
      RESULT_TYPE_SKILL_SELECT,
      RESULT_TYPE_SKILL_SELECT_INFO,

      RESULT_TYPE_BACK
    };

    /** 処理結果データ */
    typedef struct _tag_result_data
    {
      ResultType type;

      bool  bMega;
      bool  bZenryoku;
      bool  bUltraBurst;

      //  画面を抜ける時のカーソル位置タイプ
      app::BattleUISkillSelectLowerDraw::CursorType endCurrentCursorType;

      //  処理後のスキルカーソルidx
      s32 endSkillCursorIdx;

      //  選択したスキルデータ
      struct _tag_skill_select_
      {
        s32                 list_idx;
        bool                b_zenryoku;
        gfl2::math::Vector3 btnPos;

        app::BattleUISkillSelectLowerDraw::SKILL_SELECT_PARAM skillSelectParam;
      } push_skill_select;

      _tag_result_data()
      {
        Clear();
      }

      void Clear()
      {
        endSkillCursorIdx         = 0;
        endCurrentCursorType      = app::BattleUISkillSelectLowerDraw::CURSOR_POS_TYPE_SKILL_00;

        type       = RESULT_TYPE_NONE;
        bMega      = false;
        bZenryoku  = false;
        bUltraBurst= false;

        gfl2::std::MemClear( &push_skill_select, sizeof( push_skill_select ) );
      }

    } RESULT_DATA;

    //  フレームアプリ用のパラメータ
    typedef struct _tag_app_param
    {
      /** 入力用パラメータ(一度しか設定しないよう) */
      struct _tag_in_param
      {
        //  フレーム内で使用する親ヒープ
        gfl2::heap::HeapBase *pParentAppHeap;
        gfl2::heap::HeapBase *pParentDevHeap;

        //  一時使用用のヒープ(ファイル圧縮解凍で使う)
        gfl2::heap::HeapBase* pTmpDevHeap;

        //ファイルマネージャー
        gfl2::fs::AsyncFileManager* pAsyncFileManager;

        //  描画パイプライン
        ::app::util::AppRenderingManager* pRenderingManager;

        //  バトルパラメータのアクセサー
        app::BattleUIDataManager* pBtlParamAccessror;

        //  自身のパーティーidx
        s32 selfPartyPokeIdx;

        //  初回技選択カーソル
        app::BattleUISkillSelectLowerDraw::CursorType firstSkillListCursorType;

        _tag_in_param()
        {
          pParentAppHeap     = NULL;
          pParentDevHeap     = NULL;
          pTmpDevHeap        = NULL;
          pAsyncFileManager  = NULL;
          pRenderingManager  = NULL;
          pBtlParamAccessror = NULL;

          selfPartyPokeIdx          = -1;
          firstSkillListCursorType  = app::BattleUISkillSelectLowerDraw::CURSOR_POS_TYPE_SKILL_00;
        }

      } in_param;

      /** 出力パラメータ( フレーム処理終了時に結果反映 ) */
      struct _tag_out_param
      {
        //  フレームが破棄されたらtrueになる
        bool          bEnd;
        bool          bEnterSuspend;

        //  処理結果
        RESULT_DATA   resultData;

        void Clear()
        {
          bEnd                  = false;
          bEnterSuspend         = false;
          resultData.Clear();
        }

      } out_param;

    } APP_PARAM;

    //------------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     */
    //------------------------------------------------------------------
    BattleUISkillSelectFrame(void);

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~BattleUISkillSelectFrame();

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
     * @brief 決定サスペンド状態から復帰
     */
    //------------------------------------------------------------------
    void Resume();

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
     * @brief   イベント中か
     * @return  イベント中 -> true / イベントしていない -> false
     */
    //------------------------------------------------------------------
    bool IsEvent();

    //------------------------------------------------------------------
    /**
     * @brief コマンド通知
     */
    //------------------------------------------------------------------
    bool PushCmd( const CmdType type );

    //  決定入力用リスナ
    virtual bool OnEnterInput( app::BattleUISkillSelectLowerDraw* pDrawInst, s32 buttonId );

    /// キー入力通知用リスナ
    virtual bool OnKeyAction( app::BattleUISkillSelectLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  private:
    enum
    {
      //  このフレームと描画を動かすのに必要なheapサイズを定義
      //  100Kbyte
      SIZE_MY_APP         = 1024 * 100,

      //  1.5byte
      SIZE_MY_APPDEV      = 1024 * 1536,

      //  2Kbyte
      SIZE_FRAME_MANAGER  = 4096
    };

    enum
    {
      NAME_LENGHT_ZENRYOKE_WAZA = 16,
    };

    // 状態ステート設定
    void _SetState( const StateType newStateType );

    //  現在参照しているポケモンのパラメーターを取得
    const BTL_POKEPARAM* _GetSelfBtlPokeParam();

    //  カーソル移動
    s32 _MoveCursorPos( const s32 nowCurosrPos, const s32 cursorMinPos, const s32 cursorMaxPos, const s32 moveNum, app::BattleUISkillSelectLowerDraw* pDrawInst, const bool bPosLoop );

    //  変数クリア
    void _Clear()
    {
      m_state             = STATE_TYPE_NONE;

      m_pArcReadBuff      = NULL;
      m_pAppHeap          = NULL;
      m_pManagerHeapBase  = NULL;
      m_pAppParam         = NULL;
      m_pDraw             = NULL;
      m_pUISkillStatusFrame = NULL;
      m_pSelectListYKeyRepertCtrl = NULL;

      m_bEnd  = false;

      m_resultData.Clear();
    }

    static const s32 s_arcId;

    APP_PARAM*                            m_pAppParam;
    ::app::util::Heap*                    m_pAppHeap;
    gfl2::heap::HeapBase*                 m_pManagerHeapBase;

    StateType                             m_state;
    void*                                 m_pArcReadBuff;

    app::BattleUISkillSelectLowerDraw*    m_pDraw;

    RESULT_DATA                           m_resultData;
    bool                                  m_bEnd;

    //  わざ説明用フレーム
    BattleUISkillStatusFrame*             m_pUISkillStatusFrame;
    BattleUISkillStatusFrame::APP_PARAM   m_uiSkillStatusFrameAppParam;

    ::app::util::KeyRepeatController*     m_pSelectListYKeyRepertCtrl;

    //  メンバー変数にしたのは変数の一部にメモリ確保用の文字列があり、こちらで開放するために保存しないといけないから
    app::BattleUISkillSelectLowerDraw::INIT_PARAM m_skillSelectDrawInitParam;
  };

GFL_NAMESPACE_END( btlv )


#endif // __BTLV_UI_SKILL_SELECT_FRAME_H__
