//=============================================================================
/**
 * @file	 BtlvUIPokeStatusFrame.h
 * @brief	 バトルUIポケモンステータス画面の制御フレーム
 * @author yuto_uchida
 * @date	 2015.03.19
 */
//=============================================================================
#ifndef __BTLV_UI_POKE_STATUS_FRAME_H__
#define __BTLV_UI_POKE_STATUS_FRAME_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"

#include "Battle/source/btlv/app/BattleUIDataManager.h"
#include "Battle/source/btlv/app/BattleUIPokeStatusLowerDraw.h"
#include "Battle/source/btl_pokeparam.h"

GFL_NAMESPACE_BEGIN( btl )

  /** バトルポケモンステータス画面の制御フレーム */
  class BattleUIPokeStatusFrame : public applib::frame::CellSuper, public app::BattleUIPokeStatusLowerDrawDelegate
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(BattleUIPokeStatusFrame);
  public:
    //  フレームの状態
    enum StateType
    {
      STATE_TYPE_NONE = 0,
      STATE_TYPE_LOAD,
      STATE_TYPE_OPEN,
      STATE_TYPE_UPDATE,
      STATE_TYPE_ENTER_EVENT,
      STATE_TYPE_INFO_WINDOW,
      STATE_TYPE_CLOSE,
      STATE_TYPE_END
    };

    enum StatusType
    {
      STATUS_TYPE_SELF  = 0,
      STATUS_TYPE_ENEMY
    };

    //  フレームアプリ用のパラメータ
    typedef struct _tag_app_param
    {
      /** 入力用パラメータ */
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

        StatusType  statusType;

        //  共通バトルパラメータのアクセサ
        app::BattleUIDataManager* pBtlParamAccessror;

        //  対象のポケモンパラメータ
        const BTL_POKEPARAM* pPokeParam;
         
         // 状態ステータスリスト
        const app::BattleUIDataManager::POKE_STATUS_DATA* pStatusDataList;

        _tag_in_param()
        {
          pParentAppHeap      = NULL;
          pParentDevHeap      = NULL;
          pTmpDevHeap         = NULL;
          pAsyncFileManager   = NULL;
          pRenderingManager   = NULL;
          pBtlParamAccessror  = NULL;
          pPokeParam          = NULL;
          pStatusDataList     = NULL;

          statusType  = STATUS_TYPE_SELF;
        }

      } in_param;

      /** 出力パラメータ（フレーム処理終了後に値が設定される ）*/
      struct _tag_out_param
      {
        //  フレーム終了後にtrueに
        bool  bEnd;

        void Clear()
        {
          bEnd = false;
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
    BattleUIPokeStatusFrame(void);

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~BattleUIPokeStatusFrame();

  public:

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
     * @brief 強制終了通知
     */
    //------------------------------------------------------------------
    void End();

    // 選択した選択項目
    virtual bool OnEnterInput( app::BattleUIPokeStatusLowerDrawBase* pDrawInst, const ENTER_DATA& rEnterData );

    //  キー入力通知用リスナ
    virtual bool OnKeyAction( app::BattleUIPokeStatusLowerDrawBase* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  private:
    enum
    {
      //  このフレームと描画を動かすのに必要なheapサイズを定義
      //  200Kbyte
      SIZE_MY_APP         = 1024 * 300,

      //  600Kbyte
      SIZE_MY_APPDEV      = 1024 * 700,

      //  2Kbyte
      SIZE_FRAME_MANAGER  = 4096
    };

    //  イベント一覧
    enum EventId
    {
      EVENT_ID_NONE = 0,
      EVENT_ID_CANCEL,
      EVENT_ID_TOKUSEI_INFO_DRAW,
      EVENT_ID_ITEM_INFO_DRAW,
      EVENT_ID_STATUS_INFO_DRAW,
      EVENT_ID_STATUS_LIST_PREV,
      EVENT_ID_STATUS_LIST_NEXT
    };

    //  イベントデータ
    typedef struct
    {
      EventId eventId;

      struct  IdStyle
      {
        u32                 id;
        gfl2::math::Vector3 infoPos;
      } idStyle;

      struct  InfoWindowMessageStyle
      {
        u32                 titleMsgId;
        u32                 infoMsgId;
        gfl2::math::Vector3 infoPos;

      } infoWindowMessageStyle;

      void Clear()
      {
        eventId     = EVENT_ID_NONE;
      }

    } EVENT_DATA;

    // 状態ステート設定
    void _SetState( const StateType newStateType );

    //  状態インフォ表示ステートに遷移
    void _SetStatusInfoState( const u32 titleMsgId, const u32 infoMsgId, const gfl2::math::Vector3 pos );

    //  特性インフォ表示ステートに遷移
    void _SetTokuseiInfoState( const u32 tokuseiId, const gfl2::math::Vector3 pos );

    //  アイテムインフォ表示ステートに遷移
    void _SetItemInfoState( const u32 itemId, const gfl2::math::Vector3 pos );

    //  変数クリア
    void _Clear()
    {
      m_state             = STATE_TYPE_NONE;

      m_pArcReadBuff      = NULL;
      m_pAppHeap          = NULL;
      m_pManagerHeapBase  = NULL;
      m_pAppParam         = NULL;
      m_pDraw             = NULL;
      m_bEnd              = false;
    }

    static const s32 s_arcId;

    APP_PARAM*                          m_pAppParam;
    ::app::util::Heap*                  m_pAppHeap;
    gfl2::heap::HeapBase*               m_pManagerHeapBase;

    StateType                           m_state;
    void*                               m_pArcReadBuff;
    bool                                m_bEnd;

    app::BattleUIPokeStatusLowerDrawBase* m_pDraw;

    //  イベント通知した後のイベントデータ
    EVENT_DATA  m_enterData;
  };

GFL_NAMESPACE_END( btlv )


#endif // __BTLV_UI_POKE_STATUS_FRAME_H__
