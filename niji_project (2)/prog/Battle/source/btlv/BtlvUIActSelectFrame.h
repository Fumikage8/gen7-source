//=============================================================================
/**
 * @file	 BtlvUIActSelectFrame.h
 * @brief	 バトル下画面の行動選択画面の制御フレーム
 * @author yuto_uchida
 * @date	 2015.03.10
 * @note
 *  使用側でフレーム内で自動開放させるか、させないかを指定することができる
 */
//=============================================================================
#ifndef __BTLV_UI_ACT_SELECT_FRAME_H__
#define __BTLV_UI_ACT_SELECT_FRAME_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "AppLib/include/Frame/AppFrameManager.h"

#include "Battle/source/btlv/app/BattleUIActSelectLowerDraw.h"

#include "Battle/include/battle_def.h"
#include "Battle/source/btlv/app/BattleUIDataManager.h"
#include "Battle/source/btlv/BtlvUIPokeStatusFrame.h"
#include "Battle/source/btlv/BtlvUIPokeEntryConfirmFrame.h"
#include "Battle/source/btlv/BtlvUIBGFrame.h"

#include "Battle/source/btl_pokeparam.h"

GFL_NAMESPACE_BEGIN( btl )

  /**
  * @brief バトル下画面の行動選択制御フレームクラス
  */
  class BattleUIActSelectFrame : public applib::frame::CellSuper
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(BattleUIActSelectFrame);

  public:
    //  フレームに通知するコマンド
    enum CmdType
    {
      CMD_TYPE_PUSH_BUTTON_FIGHT  = 0,
      CMD_TYPE_PUSH_BUTTON_BAG
    };

    //  フレームの状態
    enum StateType
    {
      STATE_TYPE_NONE = 0,
      STATE_TYPE_LOAD,
      STATE_TYPE_CREATE,
      STATE_TYPE_IDLE,
      STATE_TYPE_UPDATE,
      STATE_TYPE_SUSPEND,
      STATE_TYPE_END
    };

    //  決定入力結果一覧
    enum ResultType
    {
      RESULT_TYPE_NONE = 0,
      RESULT_TYPE_FIGHT,
      RESULT_TYPE_POKE,
      RESULT_TYPE_BAG,
      RESULT_TYPE_RUN,
      RESULT_TYPE_MONS_BALL,
      RESULT_TYPE_MONS_ICON,
      RESULT_TYPE_USER_INFO
    };

    /** 処理結果データ */
    typedef struct _tag_result_data
    {
      ResultType type;

      union
      {
        s32 work[ 8 ];

        //  ポケモンデータ用( enterType = RESULT_ENTER_TYPE_MONS_ICON でデータが入る )
        struct _tag_mons_data
        {
          //  モンスターについて
          const app::BattleUIActSelectLowerDraw::POKE_DATA*       pPokeData;
        } mons;
      } data;

      _tag_result_data()
      {
        Clear();
      }

      void Clear()
      {
        type = RESULT_TYPE_NONE;
        gfl2::std::MemClear( &data, sizeof( data ) );
      }

    } RESULT_DATA;

    //  フレームアプリ用のパラメータ
    typedef struct _tag_app_param
    {
      //  開始のたびに設定するパラメータ
      struct _tag_config_param
      {
        //  初回カーソル(フレーム内部でカーソル位置を変更している)
        app::BattleUIActSelectLowerDraw::CursorType selectCursorType;

        _tag_config_param()
        {
          Clear();
        }

        void Clear()
        {
          selectCursorType    = app::BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_FIGHT;
        }

      } config_param;

      /** 入力用パラメータ(一度しか設定しない用) */
      struct _tag_in_param
      {
        //  ボタンを押した後に破棄処理に遷移するかどうか
        bool  bAutoDelete;

        //  戻る行動状態か
        bool  bActReturn;

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

        //  背景フレームのアドレス
        BattleUIBGFrame*              pBGFrame;

        _tag_in_param()
        {
          pParentAppHeap     = NULL;
          pParentDevHeap     = NULL;
          pTmpDevHeap        = NULL;
          pAsyncFileManager  = NULL;
          pRenderingManager  = NULL;
          pBtlParamAccessror = NULL;
          pBGFrame           = NULL;
          bActReturn         = false;
          bAutoDelete        = false;
        }

      } in_param;

      /** 出力用パラメータ( フレーム処理が終わると反映 ) */
      struct _tag_out_param
      {
        RESULT_DATA resultData;

        //  フレーム処理が終わるとtrueになる
        bool        bEnd;

        //  インスタンスが死亡しているかどうか
        bool        bDie;

        void Clear()
        {
          bEnd  = false;
          bDie  = false;
          resultData.Clear();
        };
      } out_param;

    } APP_PARAM;

    /** バトルルール(シングル、複数、バトルロワイヤル)ごとの制御の基本クラス */
    class CoreImp : public app::BattleUIActSelectLowerDrawDelegate
    {
    public:
      //  親クラスの BattleUIActSelectFrame にすでに STATE_TYPE_XXX 定義名があるのでかぶらないようにした。
      enum StateCoreType
      {
        STATE_CORE_TYPE_NONE = 0,
        STATE_CORE_TYPE_OPEN,
        STATE_CORE_TYPE_UPDATE,
        STATE_CORE_TYPE_ENTER_EVENT,
        STATE_CORE_TYPE_POKE_STATUS,
        STATE_CORE_TYPE_USER_INFO,
        STATE_CORE_TYPE_PAUSE,
        STATE_CORE_TYPE_CLOSE,
        STATE_CORE_TYPE_END
      };

      CoreImp()
      {
        m_state               = STATE_CORE_TYPE_NONE;
        m_pUtilHeap           = NULL;
        m_pAppParam           = NULL;
        m_pFrameManager       = NULL;
        m_pDraw               = NULL;
        m_pInput              = NULL;
        m_pUIPokeStatusFrame  = NULL;
        m_pUIPokeEntryConfirmFrame  = NULL;
        m_subSeq              = 0;
        m_useShortCutItemBall = true;
        m_bNextPause          = false;
        m_targetPokeIndex     = 0;
      }

      virtual ~CoreImp()
      {
        GFL_SAFE_DELETE( m_pDraw );
        GFL_SAFE_DELETE( m_pInput );
      };

      //  初期化処理
      virtual void Initialize( APP_PARAM* pAppParam, applib::frame::Manager* pFrameManager, ::app::util::Heap* pUtilHeap, void* pLytDataBuff )
      {
        GFL_ASSERT( pAppParam );
        GFL_ASSERT( pUtilHeap );
        GFL_ASSERT( pFrameManager );

        m_pUtilHeap         = pUtilHeap;
        m_pAppParam         = pAppParam;
        m_pFrameManager     = pFrameManager;
      }

      //  Drawクラスの初期化パラメータ作成
      void CreateDrawInitParam( app::BattleUIActSelectLowerDraw::INIT_PARAM* pOutParam, void* pLytDataBuff );

      //  終了処理
      void Terminate();

      //  インスタンスの破棄が可能かどうか
      bool IsDelete() { return m_pDraw->IsDelete(); }

      //  現在のステートを返す
      StateCoreType GetNowState() const { return m_state; }

      //  リザルト取得
      const RESULT_DATA&  GetResultData() const { return  m_resultData; }

      //  更新処理
      void Update();

      //  描画
      void Draw( gfl2::gfx::CtrDisplayNo diplayNo )
      {
        m_pDraw->DrawTree( diplayNo );
      }

      //  開始
      void Start();

      //  再開
      void ReStart();

      //  終了
      void End();
      //  内部で作成した子フレームのみ削除
      void EndChildFrame( const bool bNextPause );

      /** 処理の終了通知(終了なので結果反映、終了フラグを立てる)*/
      void SendProcEnd()
      {
        m_pAppParam->out_param.bEnd = true;
        m_pAppParam->out_param.resultData  = m_resultData;
      }

      /** ポケモンショートカット使用設定 */
      void SetUseShortCutItemBall( const bool bFlg );

      /** マスク設定 */
      void SetMaskDraw( const bool bFlg );

      //  入力設定
      void SetEnableInput( const bool bFlg );

      //  入力インターフェイスを取得
      app::BattleUIActSelectLowerInputInterface* GetInput() { return m_pInput; }

      //  ステート設定
      virtual void SetState( const StateCoreType newState );

      //  決定入力用リスナ
      virtual bool OnEnterInput( app::BattleUIActSelectLowerDraw* pDrawInst, s32 buttonId );

      /// キー入力通知用リスナ(入力制御はクラス側にまかせているので、処理を変更する時は参照するクラスを差し替えるだけでOK)
      virtual bool OnKeyAction( app::BattleUIActSelectLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );


      void  SetTargetPokeIndex(const u32 targetPokeIndex) {m_targetPokeIndex  = targetPokeIndex;}

    protected:
      APP_PARAM*                                  m_pAppParam;

      applib::frame::Manager*                     m_pFrameManager;
      ::app::util::Heap*                          m_pUtilHeap;

      StateCoreType                               m_state;
      RESULT_DATA                                 m_resultData;

      BattleUIPokeStatusFrame*                    m_pUIPokeStatusFrame;
      BattleUIPokeStatusFrame::APP_PARAM          m_uiPokeStatusFrameAppParam;

      BattleUIPokeEntryConfirmFrame*              m_pUIPokeEntryConfirmFrame;
      BattleUIPokeEntryConfirmFrame::APP_PARAM    m_uiPokeEntryConfirmFrameAppParam;

      app::BattleUIActSelectLowerDraw*            m_pDraw;
      app::BattleUIActSelectLowerInputInterface*  m_pInput;

      u32                                         m_subSeq;

      bool                                        m_useShortCutItemBall;

      u32                                         m_targetPokeIndex;    /**<  場に出ているポケモンのうち、行動選択中のスロット番号  */

    private:

      bool                                        m_bNextPause;

      //  モンスターボールアイコンの状態を更新
      void _UpdateMonsterBallIconState();
    };

    //------------------------------------------------------------------
    /**
     * @brief	  コンストラクタ
     */
    //------------------------------------------------------------------
    BattleUIActSelectFrame(void);

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~BattleUIActSelectFrame();

    //------------------------------------------------------------------
    /**
     * @brief	  動作に必要な引数を登録
     * @note
     *    必ず最初に呼び出さないと後の動作保障はなし
     */
    //------------------------------------------------------------------
    void Initialize( APP_PARAM* pAppParam );

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
     * @brief フレームが使えるようになっているかどうか
     */
    //------------------------------------------------------------------
    bool IsReady();

    //------------------------------------------------------------------
    /**
     * @brief イベント中か
     */
    //------------------------------------------------------------------
    bool IsEvent();

    //------------------------------------------------------------------
    /**
     * @brief 開始通知
     */
    //------------------------------------------------------------------
    void Start(const u32 targetPokeIndex);

    //------------------------------------------------------------------
    /**
     * @brief 再度開始通知
     */
    //------------------------------------------------------------------
    void Restart(const u32 targetPokeIndex);

    //------------------------------------------------------------------
    /**
     * @brief 強制終了通知
     */
    //------------------------------------------------------------------
    void End();

    //------------------------------------------------------------------
    /**
     * @brief 強制終了通知(子フレームのみ)
     */
    //------------------------------------------------------------------
    void EndChildFrame();

    //------------------------------------------------------------------
    /**
     * @brief コマンド通知
     */
    //------------------------------------------------------------------
    bool PushCmd( const CmdType type );

    //------------------------------------------------------------------
    /**
     * @brief ステート取得
     */
    //------------------------------------------------------------------
    StateType GetState() const { return m_state; }

    //------------------------------------------------------------------
    /**
     * @brief 入力設定
     */
    //------------------------------------------------------------------
    void SetEnableInput( const bool bFlg );

  private:
    enum
    {
      //  このフレームと描画を動かすのに必要なheapサイズを定義
      //  100kbyte
      SIZE_MY_APP         = 1024 * 100,

      //  800kbyte
      SIZE_MY_APPDEV      = 1024 * 800,

      //  2kbyte
      SIZE_FRAME_MANAGER  = 4096
    };

    //------------------------------------------------------------------
    /**
     * @brief 再開通知
     * @note
     *  config_paramのパラメータを参照して設定およびリザルトをクリアして再開
     */
    //------------------------------------------------------------------
    void _ReStart();

    // 状態ステート設定
    void _SetState( const StateType newStateType );

    //------------------------------------------------------------------
    /**
      * @brief 処理終了チェック
      * @note
      *   処理中にCore処理が終了しているかチェックして、終了していればステートを終了に切り替えて終わらせる
    */
    //------------------------------------------------------------------
    void _ChkEndUpdate();

    // 変数クリア
    void _Clear()
    {
      m_state             = STATE_TYPE_NONE;
      m_pCore             = NULL;
      m_pArcReadBuff      = NULL;
      m_pAppHeap          = NULL;
      m_pManagerHeapBase  = NULL;
      m_pAppParam         = NULL;
      m_bStartup          = false;
    }

    static const s32 s_arcId;

    APP_PARAM*                        m_pAppParam;
    ::app::util::Heap*                m_pAppHeap;
    gfl2::heap::HeapBase*             m_pManagerHeapBase;

    CoreImp*                          m_pCore;

    StateType                         m_state;
    void*                             m_pArcReadBuff;

    RESULT_DATA                       m_resultData;

    bool                              m_bStartup;
  };

GFL_NAMESPACE_END( btlv )


#endif // __BTLV_UI_ACT_SELECT_FRAME_H__
