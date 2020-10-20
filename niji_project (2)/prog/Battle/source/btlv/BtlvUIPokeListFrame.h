//=============================================================================
/**
 * @file	 BtlvUITargetSelectFrame.h
 * @brief	 バトルUIポケモンリスト画面の制御フレーム
 * @author yuto_uchida
 * @date	 2015.04.09
 */
//=============================================================================
#ifndef __BTLV_UI_POKE_LIST_FRAME_H__
#define __BTLV_UI_POKE_LIST_FRAME_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"

#include "Battle/source/btlv/app/BattleUIPokeListLowerDraw.h"
#include "Battle/source/btlv/app/BattleUIDataManager.h"
#include "Battle/source/btl_common.h"
#include "Battle/source/btlv/BtlvUISkillStatusFrame.h"

#include "Print/include/WordSetLoader.h"

GFL_NAMESPACE_BEGIN( btl )

  /** バトルポケモンリスト選択画面の制御フレーム */
  class BattleUIPokeListFrame : public applib::frame::CellSuper, private app::BattleUIPokeListLowerDrawDelegate
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(BattleUIPokeListFrame);
  public:
    //  フレームタイプ
    enum FrameType
    {
      //  行動選択からの通常用
      FRAME_TYPE_NORMAL = 0,
      //  気絶時の再配置用
      FRAME_TYPE_REPLACE
    };

    //  ポケモン配置idx
    enum PokeExChangeIdx
    {
      LISTPOS_UL  = 0,
      LISTPOS_UR,
      LISTPOS_MAX,
    };

    //  フレームの状態
    enum StateType
    {
      STATE_TYPE_NONE = 0,
      STATE_TYPE_LOAD,
      STATE_TYPE_OPEN,
      STATE_TYPE_UPDATE,
      STATE_TYPE_ENTER_EVENT,
      STATE_TYPE_WAZA_STATUS,
      STATE_TYPE_INFO_WINDOW,
      STATE_TYPE_MESSAGE,
      STATE_TYPE_CLOSE,
      STATE_TYPE_END
    };

    //  決定入力結果一覧
    enum ResultType
    {
      RESULT_TYPE_CANCEL = 0,
      //  回復するを押した時に設定
      RESULT_TYPE_USE_ITEM,
      //  入れ替えを押した時に設定
      RESULT_TYPE_CHANGE_POKE,
      //  複数入れ替えの時に設定
      RESULT_TYPE_EXCHANGE_POKE,
    };

    //  初回ボタンカーソル位置
    enum FirstButtonCursorPos
    {
      FIRST_BUTTON_CURSOR_POS_NONE  = 0,
      FIRST_BUTTON_CURSOR_POS_CURE
    };

    //  複数入れ替え時のパラメータ
    struct EX_CHANGE_PARAM
    {
      u32     outPokeIdx;
      u32     selectPokeIdx;
      BtlvPos btlPos;

      void Clear()
      {
        btlPos        = BTL_VPOS_ERROR;
        outPokeIdx    = 0;
        selectPokeIdx = 0;
      }

      EX_CHANGE_PARAM()
      {
        Clear();
      }
    };

    /** 処理結果データ */
    typedef struct _tag_result_data
    {
      //  結果タイプ
      ResultType type;

      //  カレントポケモンパラメータ(バッグから戻った時のカーソル位置参照に必要)
      const btl::BTL_POKEPARAM* pCurrentPokeParam;

      //  結果タイプによるデータ
      union
      {
        s32 work[ 32 ];

        //  type == RESULT_TYPE_CHANGE_POKE or RESULT_TYPE_USE_ITEMの時に値が入っている
        struct _tag_poke_data
        {
          const btl::BTL_POKEPARAM* pParam;

          u8  pokeIdx;
        } changePoke;

      } data;

      //  このデータはフレームが死んでも中身は保持しつづけないといけない。
      //  ダブル戦などで複数体同時に死んだ場合の入れ替えで選択している最中のが入っているから
      //  このデータはフレーム内で初期化すると入れ替えがうまくいかないでバグるので注意
      EX_CHANGE_PARAM aExchangeOccurred[ LISTPOS_MAX ];

      _tag_result_data()
      {
        Clear();
      }

      void Clear()
      {
        type              = RESULT_TYPE_CANCEL;
        pCurrentPokeParam = NULL;

        gfl2::std::MemClear( &data, sizeof( data ) );
      }

    } RESULT_DATA;

    //  フレームアプリ用のパラメータ
    typedef struct _tag_app_param
    {
      /** 入力用パラメータ */
      struct _tag_in_param
      {
        FrameType frameType;

        //  現在のポケモン選択結果
        PokeSelResult* pPokeSelectResult;

        //  フレーム内で使用する親ヒープ
        gfl2::heap::HeapBase *pParentAppHeap;
        gfl2::heap::HeapBase *pParentDevHeap;

        //  一時使用用のヒープ(ファイル圧縮解凍で使う)
        gfl2::heap::HeapBase* pTmpDevHeap;

        //  ファイルマネージャー
        gfl2::fs::AsyncFileManager* pAsyncFileManager;

        //  バトルパラメータのアクセサー
        app::BattleUIDataManager* pBtlParamAccessror;

        //  描画パイプライン
        ::app::util::AppRenderingManager* pRenderingManager;

        //  入れ替えでにげることができるか
        bool bCanEsc;

        //  初回参照するポケモンデータ
        const pml::pokepara::PokemonParam* pFirstCurrentPokeParam;

        //  初回ボタンカーソル位置
        FirstButtonCursorPos  firstButtonCursorPos;

        //  入れ替え元の先頭のポケモンパラメータ
        const BTL_POKEPARAM*  m_pFirstExChangeSourcePokeParam;

        void _tag_app_param()
        {
          frameType               = FRAME_TYPE_NORMAL;
          pParentAppHeap          = NULL;
          pParentDevHeap          = NULL;
          pTmpDevHeap             = NULL;
          pAsyncFileManager       = NULL;
          pBtlParamAccessror      = NULL;
          pRenderingManager       = NULL;
          pPokeSelectResult       = NULL;
          bCanEsc                 = true;
          pFirstCurrentPokeParam  = NULL;
          m_pFirstExChangeSourcePokeParam = NULL;
          firstButtonCursorPos    = FIRST_BUTTON_CURSOR_POS_NONE;
        }

      } in_param;

      /** 出力パラメータ( フレーム処理終了時に結果反映 ) */
      struct _tag_out_param
      {
        //  フレームが破棄されたらtrueになる
        bool  bEnd;

        //  処理結果
        RESULT_DATA resutlData;

        void Clear()
        {
          bEnd            = false;

          resutlData.Clear();
        }

        _tag_out_param()
        {
          Clear();
        }

      } out_param;

      u32             m_ExchangeOccurredCnt;

    } APP_PARAM;

    //------------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     */
    //------------------------------------------------------------------
    BattleUIPokeListFrame();

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~BattleUIPokeListFrame();

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
     * @brief 強制破棄通知
     * @note
     *  画面が閉じる演出なしにいきなりインスタンスが破棄
     */
    //------------------------------------------------------------------
    void Die();

    //------------------------------------------------------------------
    /**
     * @brief 決定入力用リスナ
     */
    //------------------------------------------------------------------
    virtual bool OnEnterInput( app::BattleUIPokeListLowerDraw* pDrawInst, const BattleUIPokeListLowerDrawDelegate::RESULT_DATA& rResultData );

    //------------------------------------------------------------------
    /**
     * @brief キー入力通知用リスナ
     */
    //------------------------------------------------------------------
    virtual bool OnKeyAction( app::BattleUIPokeListLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

    //------------------------------------------------------------------
    /**
     * @brief     わざnoとバトル用ポケモンパラメータからから現在配置している敵を参照して効果タイプを取得
     * @param[out]   pPutFlg       : 表示可能
     * @param[in]    wazaNo        : 対象のわざNo
     * @param[in]    pBtlPokeParam : wazaNoも持っているバトルポケモンのパラメータ
     */
    //------------------------------------------------------------------
    virtual BtlTypeAffAbout OnGetKoukaType( bool* pPutFlg, const WazaNo wazaNo, const BTL_POKEPARAM* pBtlPokeParam );

    //------------------------------------------------------------------
    /**
     * @brief	   描画できる状態になった事を返すための関数
     * @return	 描画できる状態になった場合trueを返す
     */
    //------------------------------------------------------------------
    virtual bool IsDrawEnable(void);

  private:
    enum
    {
      //  このフレームと描画を動かすのに必要なheapサイズを定義
      //  300Kbyte
      SIZE_MY_APP         = 1024 * 300,

      //  900KByte
      SIZE_MY_APPDEV      = 1024 * 900,

      //  2Kbyte
      SIZE_FRAME_MANAGER  = 4096
    };

    enum ChangeState
    {
      CHANGE_STATE_NONE = -1,
      CHANGE_STATE_CURE,
      CHANGE_STATE_CANCEL,
      CHANGE_STATE_STATUS,
      CHANGE_STATE_CMD,
      CHANGE_STATE_POKE_POS_SELECT,
      CHANGE_STATE_POKE_SET,
      CHANGE_STATE_WAZA_STATUS,
      CHANGE_STATE_TOKUSE_INFO,
      CHANGE_STATE_ITEM_INFO
    };

    // 状態ステート設定
    void _SetState( const StateType newStateType );

    //  メッセージステート設定
    void _SetStateByMessage( const bool bEventWait, gfl2::str::StrBuf* pMessageStrBuf );

    //  ポケリストidxをアクティブに
    void _SetActivePokeObjList( const s32 pokeListIdx );

    //------------------------------------------------------------------
    /**
     * @brief   バトルポケモンリスト選択のデータ出力
     * @return  ポケモンリスト選択数
     * @note
     *  バトルのルールや通信対戦などで他人のポケモンもリストに含まれる
     */
    //------------------------------------------------------------------
    s32 _OutputPokeSelectItemListData( app::BattleUIPokeListLowerDraw::POKE_SELECT_ITEM_PARAM outputData[ BTL_PARTY_MEMBER_MAX ] );

    //  ポケモンがすでに配置されているか
    bool _IsInBattlePoke( const app::BattleUIPokeListLowerDraw::POKE_SELECT_ITEM_PARAM* pPokeSelectItemParam );

    //------------------------------------------------------------------
    /**
     * @brief   ポケモン入れ替えが可能かチェック
     * @return  入れ替え不可能 -> false / 可能 -> true
     * @note
     *  falseの場合には pOutErrorMessage にエラーメッセージが出力される
     */
    //------------------------------------------------------------------
    bool _ChkPokeChange( gfl2::str::StrBuf* pOutErrorMessage, const app::BattleUIPokeListLowerDraw::POKE_SELECT_ITEM_PARAM* pSelectPokeData );

    //  ポケモン入れ替えが可能かチェック(ポケモン位置指定)
    bool _ChkPokeReplacePos( gfl2::str::StrBuf* pOutErrorMessage, const PokeExChangeIdx exChangeIdx );

    //  ポケモン入れ替え位置指定フローに遷移するかどうか
    bool _IsPokePostionSettingFlow();

    //  指定配置位置にポケモンがいるか
    //  配置位置を指定
    const BTL_POKEPARAM* _GetSettingPokeParam( const PokeExChangeIdx exChangeIdx );

    //  選択したポケモンデータから結果データを作成
    void _CreateResultDataPokeChange( const ResultType resultType, const app::BattleUIPokeListLowerDraw::POKE_SELECT_ITEM_PARAM* pSelectPokeData );

    //  入力データを取得状態か
    bool _IsInputCatch();

    //  特性インフォ表示ステートに遷移
    void _SetTokuseiInfoState( const u32 tokuseiId, const gfl2::math::Vector3& pos );

    //  アイテムインフォ表示ステートに遷移
    void _SetItemInfoState( const u32 itemId, const gfl2::math::Vector3& pos );

    //  複数入れ替えidxからバトル配置idxを取得
    BtlvPos ConvExChangeIdxFromBtlvPos( const PokeExChangeIdx  );

    //  変数クリア
    void _Clear()
    {
      m_state               = STATE_TYPE_NONE;
      m_subState            = 0;
      m_changeState         = CHANGE_STATE_NONE;
      m_pDraw               = NULL;
      m_pArcReadBuff        = NULL;
      m_pAppHeap            = NULL;
      m_pManagerHeapBase    = NULL;
      m_pAppParam           = NULL;
      m_pEventMessageStrBuf = NULL;
      m_pokeListMax         = 0;
      m_pSkillStatusFrame   = NULL;
      m_bEventWait          = false;
      m_bNextMessageStep    = false;
      m_bEnd                = false;
      m_bOldInputKeyByButtonAnimation = false;

      m_inputResult.Clear();
    }

    //  ファイルARCID
    static const s32 s_arcId;

    APP_PARAM*                          m_pAppParam;
    ::app::util::Heap*                  m_pAppHeap;
    gfl2::heap::HeapBase*               m_pManagerHeapBase;

    app::BattleUIPokeListLowerDraw*                     m_pDraw;
    app::BattleUIPokeListLowerDrawDelegate::RESULT_DATA m_inputResult;
    StateType                                           m_state;
    u32                                                 m_subState;
    void*                                               m_pArcReadBuff;

    ChangeState                                         m_changeState;
    s32                                                 m_pokeListMax;
    bool                                                m_bEnd;

    gfl2::str::StrBuf*                                  m_pEventMessageStrBuf;

    btl::BattleUISkillStatusFrame*                      m_pSkillStatusFrame;
    btl::BattleUISkillStatusFrame::APP_PARAM            m_skillStatusFrameAppParam;

    bool                                                m_bEventWait;
    bool                                                m_bNextMessageStep;
    bool                                                m_bOldInputKeyByButtonAnimation;
  };

GFL_NAMESPACE_END( btlv )

#endif // __BTLV_UI_POKE_LIST_FRAME_H__
