//=============================================================================
/**
 * @file	 BtlvUISkillStatusFrame.h
 * @brief	 バトルUI技ステータス画面の制御フレーム
 * @author yuto_uchida
 * @date	 2015.03.19
 */
//=============================================================================
#ifndef __BTLV_UI_SKILL_STATUS_FRAME_H__
#define __BTLV_UI_SKILL_STATUS_FRAME_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"

#include "Battle/source/btlv/app/BattleUISkillStatusLowerDraw.h"
#include "Battle/source/btlv/app/BattleUIDataManager.h"

#include "Battle/source/btl_pokeparam.h"

//  前方宣言
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )

  class AppCommonGrpIconData;

GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( btl )

  /** バトルわざステータス画面の制御フレーム */
  class BattleUISkillStatusFrame : public applib::frame::CellSuper, public app::BattleUISkillStatusLowerDrawDelegate
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(BattleUISkillStatusFrame);
  public:
    //  ウィンドウタイプ
    enum WindowType
    {
      WINDOW_TYPE_NORMAL = 0,
      WINDOW_TYPE_INFO
    };

    //  フレームの状態
    enum StateType
    {
      STATE_TYPE_NONE = 0,
      STATE_TYPE_LOAD,
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
        WindowType          windowType;

        //  スキルパラメータを取得するインターフェイス
        const BattleViewSystem*   pBtlViewSystem;

        //  使用するメッセージのポインタ（GARC_message_btl_app_DATのデータでないとアウト）
        gfl2::str::MsgData* pBtlAppMsgData;
        print::WordSet*     pBtlWordSet;

        //  わざ説明のメッセージデータ
        gfl2::str::MsgData* pWazaInfoMsgData;

        //  フレーム内で使用する親ヒープ
        gfl2::heap::HeapBase *pParentAppHeap;
        gfl2::heap::HeapBase *pParentDevHeap;

        //  一時使用用のヒープ(ファイル圧縮解凍で使う)
        gfl2::heap::HeapBase* pTmpDevHeap;

        //  ファイルマネージャー
        gfl2::fs::AsyncFileManager* pAsyncFileManager;

        //  描画パイプライン
        ::app::util::AppRenderingManager* pRenderingManager;

        //  アイコングラフィックデータ
        ::app::tool::AppCommonGrpIconData*  pAppCommonGrpIconData;

        //  ウィンドウの基点位置
        gfl2::math::Vector3 infoWindowPivotPos;

        //  技id
        WazaID  wazaId;
        WazaID  zenryokuWazaId;

        //  全力技名
        gfl2::str::StrBuf*  pZenryokuWazaName;

        //  現在と最大pp値
        u8      ppNowNum, ppMaxNum;

        //  入力SEを使うか
        bool    bUseInputSE;

        //  全力わざ用か
        bool    bZenryoku;

        //  ユーザー入力可能か
        bool    bUserInput;

        _tag_in_param()
        {
          pBtlViewSystem        = NULL;
          pBtlAppMsgData        = NULL;
          pBtlWordSet           = NULL;
          pWazaInfoMsgData      = NULL;
          pParentAppHeap        = NULL;
          pParentDevHeap        = NULL;
          pTmpDevHeap           = NULL;
          pAsyncFileManager     = NULL;
          pRenderingManager     = NULL;
          pZenryokuWazaName     = NULL;
          pAppCommonGrpIconData = NULL;
          wazaId                = WAZANO_NULL;
          zenryokuWazaId        = WAZANO_NULL;
          ppNowNum              = 0;
          ppMaxNum              = 0;
          bUseInputSE           = false;
          bZenryoku             = false;
          bUserInput            = true;
          windowType            = WINDOW_TYPE_NORMAL;

          infoWindowPivotPos.Set( 0, 0, 0 );
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

      } out_param;

    } APP_PARAM;

    //------------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     */
    //------------------------------------------------------------------
    BattleUISkillStatusFrame(void);

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~BattleUISkillStatusFrame();

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

    //  決定入力用リスナ
    virtual bool OnEnterInput( app::BattleUISkillStatusLowerDraw* pDrawInst, s32 buttonId );

  private:
    enum
    {
      //  このフレームと描画を動かすのに必要なheapサイズを定義
      //  100Kbyte
      SIZE_MY_APP         = 1024 * 100,

      //  750Kbyte
      SIZE_MY_APPDEV      = 1024 * 750,

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
      m_pWazaSetumeiStr   = NULL;
      m_bEnd              = false;
    }

    static const s32 s_arcId;

    APP_PARAM*                          m_pAppParam;
    ::app::util::Heap*                  m_pAppHeap;
    gfl2::heap::HeapBase*               m_pManagerHeapBase;

    StateType                           m_state;
    void*                               m_pArcReadBuff;
    bool                                m_bEnd;

    gfl2::str::StrBuf*                  m_pWazaSetumeiStr;

    app::BattleUISkillStatusLowerDraw*  m_pDraw;
  };

GFL_NAMESPACE_END( btlv )


#endif // __BTLV_UI_SKILL_STATUS_FRAME_H__
