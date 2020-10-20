//=============================================================================
/**
 * @file	 BtlvUITargetSelectFrame.h
 * @brief	 バトルUI対象選択画面の制御フレーム
 * @author yuto_uchida
 * @date	 2015.03.23
 */
//=============================================================================
#ifndef __BTLV_UI_TARGET_SELECT_FRAME_H__
#define __BTLV_UI_TARGET_SELECT_FRAME_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"

#include "Battle/source/btlv/app/BattleUITargetSelectLowerDraw.h"
#include "Battle/source/btlv/app/BattleUIDataManager.h"
#include "Battle/source/btl_common.h"

GFL_NAMESPACE_BEGIN( btl )

  /** バトル２選択画面の制御フレーム */
  class BattleUITargetSelectFrame : public applib::frame::CellSuper, public app::BattleUITargetSelectLowerDrawDelegate
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(BattleUITargetSelectFrame);
  public:
    //  フレームの状態
    enum StateType
    {
      STATE_TYPE_NONE = 0,
      STATE_TYPE_LOAD,
      STATE_TYPE_CREATE,
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

        //  技を使用している味方パーティのポケモンidx
        u8 pokeIndex;

        //  攻撃するポケモンのパラメータ
        const BTL_POKEPARAM*  pPokeParam;

        //  使用する技No
        WazaNo wazaNo;

        //  初回カーソル
        s32 firstCursorPos;

        void _tag_app_param()
        {
          pParentAppHeap     = NULL;
          pParentDevHeap     = NULL;
          pTmpDevHeap        = NULL;
          pAsyncFileManager  = NULL;
          pBtlParamAccessror = NULL;
          pRenderingManager  = NULL;
          pPokeParam         = NULL;
          pokeIndex          = 0xff;
          wazaNo             = WAZANO_NULL;
          firstCursorPos     = app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_NONE;
        }

      } in_param;

      /** 出力パラメータ( フレーム処理終了時に結果反映 ) */
      struct _tag_out_param
      {
        //  フレームが破棄されたらtrueになる
        bool  bEnd;

        BtlvPos btlvPos;
        app::BattleUITargetSelectLowerDraw::SelectItemPos     selectItemPos;
        

        void Clear()
        {
          bEnd    = false;
          btlvPos = BTL_VPOS_ERROR;
          selectItemPos = app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_NONE;
        }

      } out_param;

    } APP_PARAM;

    //------------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     */
    //------------------------------------------------------------------
    BattleUITargetSelectFrame(void);

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~BattleUITargetSelectFrame();

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

    //------------------------------------------------------------------
    /**
     * @brief 決定入力用リスナ
     */
    //------------------------------------------------------------------
    virtual bool OnEnterInput( app::BattleUITargetSelectLowerDraw* pDrawInst, s32 buttonId );

    //------------------------------------------------------------------
    /**
     * @brief キー入力通知用リスナ
     */
    //------------------------------------------------------------------
    virtual bool OnKeyAction( app::BattleUITargetSelectLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  private:
    enum
    {
      //  このフレームと描画を動かすのに必要なheapサイズを定義
      //  150Kbyte
      SIZE_MY_APP         = 1024 * 150,

      //  300Kbyte
      SIZE_MY_APPDEV      = 1024 * 500,

      //  2Kbyte
      SIZE_FRAME_MANAGER  = 4096
    };

    /**
      * @brief 技の効果範囲
      */
    enum WazaRange
    { 
      WAZA_RANGE_OTHER_ONE,         // 自分以外の1ポケ
      WAZA_RANGE_OTHER_ONE_LONG,    // 自分以外の1ポケ( 遠距離技 )
      WAZA_RANGE_OTHER_ALL,         // 自分以外の全ポケ
      WAZA_RANGE_ENEMY_ONE,         // 相手側の1ポケ
      WAZA_RANGE_ENEMY_ALL,         // 相手側の全ポケ
      WAZA_RANGE_FRIEND_ONE,        // 味方側の1ポケ
      WAZA_RANGE_FRIEND_OTHER_ONE,  // 自分以外の1味方ポケ
      WAZA_RANGE_FRIEND_ALL,        // 自分を含む全味方ポケ
      WAZA_RANGE_ALL,               // 自分を含む全ポケ
      WAZA_RANGE_USER,              // 自分のみ
      WAZA_RANGE_ENEMY_ONE_RANDOM,  // 相手側の1ポケランダム
      WAZA_RANGE_FIELD_ALL,         // 場全体
      WAZA_RANGE_FIELD_ENEMY,       // 相手場
      WAZA_RANGE_FIELD_FRIEND,      // 味方場
      WAZA_RANGE_UNKNOWN,           // 特殊型
      WAZA_RANGE_NUM
    }; 

    /**
      * @brief 技を使用するポケモンの立ち位置
      */
    enum UserPos
    {
      USER_POS_LEFT,          // 左側
      USER_POS_RIGHT,         // 右側
      USER_POS_CENTER,        // 中央
      USER_POS_NUM_2VS2 = 2,  // 2vs2 での立ち位置数
      USER_POS_NUM_3VS3 = 3,  // 3vs3 での立ち位置数
      USER_POS_NUM_1VS5 = 1   // 1vs5 での立ち位置数
    }; 

    // 状態ステート設定
    void _SetState( const StateType newStateType );

    // 技範囲を取得
    WazaRange _GetTargetWazaRange( const WazaNo wazano, const u8 poke_index ) const;

    //  対象選択画面の動作設定( わざを使用するポケモンの立ち位置 )
    u32 _GetTargetSelectSetting_UserPos( u8 poke_index ) const;

    //  各ルールごとの選択データ出力
    void _OutputDoubleTargetSelectData( app::BattleUITargetSelectLowerDraw::INIT_PARAM* pOutputData, const WazaRange range, const s32 firstCursorPos );
    void _OutputRoyaleTargetSelectData( app::BattleUITargetSelectLowerDraw::INIT_PARAM* pOutputData, const WazaRange range,  const s32 firstCursorPos );
    void _OutputIntrusionTargetSelectData( app::BattleUITargetSelectLowerDraw::INIT_PARAM* pOutputData, const WazaRange range, const s32 firstCursorPos );

    //  自分以外のどれか１つを選択時のデータ出力
    void _OutputUseSelfOtherTargetSelectData( app::BattleUITargetSelectLowerDraw::INIT_PARAM* pOutputData, const s32 firstCursorPos );
    //  自分以外のすべてをまとめて選択時のデータ出力
    void _OutputOtherAllTargetSelectData( app::BattleUITargetSelectLowerDraw::INIT_PARAM* pOutputData, const s32 firstCursorPos );
    //  すべてをまとめて選択時のデータ出力
    void _OutputAllTaragetSelectData( app::BattleUITargetSelectLowerDraw::INIT_PARAM* pOutputData, const s32 firstCursorPos );
    //  自分自身のみ選択時のデータ出力
    void _OutputUseSelfOnlyTargetSelectData( app::BattleUITargetSelectLowerDraw::INIT_PARAM* pOutputData, const s32 firstCursorPos );

    //------------------------------------------------------------------
    /**
     * @brief カーソル位置を取得
     * @param beginCursorPos : 初回カーソル位置を指定する、もし初回カーソル位置を設定できない場合はチェックする
     * @note
     *    この関数は _OutputXXXTargetSelectData関数を呼んだ後に呼ぶのを前提に作成している
     */
    //------------------------------------------------------------------
    app::BattleUITargetSelectLowerDraw::SelectItemPos _GetSelectItemCursorPos( app::BattleUITargetSelectLowerDraw::SelectItemPos beginCursorPos, app::BattleUITargetSelectLowerDraw::INIT_PARAM& rDrawInitParam );
    
    //  PosIdから描画項目idへ変換
    app::BattleUITargetSelectLowerDraw::SelectItemPos _ConvBtlvPosIdToDrawSelectItemPos( const BtlvPos btlvPos ) const;

    //  描画項目idからPosIdへ変換
    BtlvPos _ConvDrawSelectItemPosToBtlvPosId( const app::BattleUITargetSelectLowerDraw::SelectItemPos drawPos ) const;

    //  変数クリア
    void _Clear()
    {
      m_state             = STATE_TYPE_NONE;
      m_pDraw             = NULL;
      m_pArcReadBuff      = NULL;
      m_pAppHeap          = NULL;
      m_pManagerHeapBase  = NULL;
      m_pAppParam         = NULL;
      m_bEnd              = false;
    }

    static const s32 s_arcId;

    APP_PARAM*                          m_pAppParam;
    ::app::util::Heap*                  m_pAppHeap;
    gfl2::heap::HeapBase*               m_pManagerHeapBase;

    app::BattleUITargetSelectLowerDraw* m_pDraw;
    StateType                           m_state;
    void*                               m_pArcReadBuff;
    bool                                m_bEnd;

    static u32  m_saConvPosIdTable[ app::BattleUITargetSelectLowerDraw::SELECT_ITEM_POS_MAX ][3];
  };

GFL_NAMESPACE_END( btlv )

#endif // __BTLV_UI_TARGET_SELECT_FRAME_H__
