//=============================================================================
/**
 * @file	 TitleFrame.h
 * @brief	 タイトル制御フレーム
 * @author yuto_uchida
 * @date	 2015.05.08
 */
//=============================================================================
#ifndef NIJI_PROJECT_TITLE_FRAME_H_INCLUDED
#define NIJI_PROJECT_TITLE_FRAME_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameManager.h>
#include <AppLib/include/Util/AppRenderingManager.h>

#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "AppLib/include/Ui/UIInputListener.h"

#include "niji_conv_header/poke_lib/monsno_def.h"

//  型の前方宣言
GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( fs )

  class  AsyncFileManager;

GFL_NAMESPACE_END( fs )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )

  class  AppUtilFileControlState;
  class  Heap;

GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN(PokeTool)
  class PokeModelSystem;
  class PokeModel;
GFL_NAMESPACE_END(PokeTool)

GFL_NAMESPACE_BEGIN(poke_3d)
  GFL_NAMESPACE_BEGIN(model)
    class BaseCamera;
  GFL_NAMESPACE_END(model)
GFL_NAMESPACE_END(poke_3d)


GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( title )

  class TitleUpperUIDraw;
  class TitleLowerUIDraw;
  class PokeModelObj;

  /** タイトル制御フレーム */
  //  フレーム側に入力コールバックを用意（画面が複雑でないので、わざわざ専用のコールバックは必要ないと判断）
  class TitleFrame : public applib::frame::CellSuper, public ::app::ui::UIInputListener
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(TitleFrame);
  public:
    //  フレームの状態
    enum StateType
    {
      STATE_TYPE_NONE = 0,
      STATE_TYPE_LOAD,
      STATE_TYPE_LOAD_MODEL,
      STATE_TYPE_OPEN,
      STATE_TYPE_UPDATE,
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

        //  描画パイプライン
        app::util::AppRenderingManager* p_rendering_manager;

        _tag_in_param()
        {
          p_parent_app_heap = NULL;
          p_parent_dev_heap = NULL;
          p_rendering_manager = NULL;
        }

      } in_param;

      /** 出力用パラメータ */
      struct _tag_out_param
      {
        bool b_ret_debug_seq;
        bool b_ret_del_save_seq;

        _tag_out_param()
        {
          b_ret_debug_seq = false;
          b_ret_del_save_seq  = false;
        }

      } out_param;

    } APP_PARAM;

    //------------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     */
    //------------------------------------------------------------------
    TitleFrame();

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~TitleFrame();

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
    virtual applib::frame::Result InitFunc();

    //------------------------------------------------------------------
    /**
     * @brief	  PROCのメイン更新を行う関数
     * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
     */
    //------------------------------------------------------------------
    virtual applib::frame::Result UpdateFunc();

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
    virtual applib::frame::Result EndFunc();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  タッチパネルイベントの検知
    */
    //--------------------------------------------------------------------------------------------
    virtual ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );

    /// キー入力通知用リスナ
    virtual ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  private:
    enum
    {
      //  このフレームと描画を動かすのに必要なheapサイズを定義
      //  450kbyte
      SIZE_MY_APP         = 1024 * 450,

      //  16Mbyte
      SIZE_MY_APPDEV      = 1024 * 1024 * 16,

      //  2Kbyte
      SIZE_FRAME_MANAGER  = 4096,
    };

    //  ファイル制御一覧
    enum
    {
      FILE_DATA_UI_LYT  = 0,

      FILE_DATA_MAX
    };

    //  バージョンタイプ
    enum VersionType
    {
      VERSION_TYPE_SUN  = 0,
      VERSION_TYPE_MOON,

      VERSION_TYPE_MAX
    };

    // 状態ステート設定
    void _SetState( const StateType newStateType );

    //  変数クリア
    void _Clear()
    {
      m_state             = STATE_TYPE_NONE;

      m_pAppHeap                  = NULL;
      m_pManagerHeapBase          = NULL;
      m_pAppParam                 = NULL;
      m_pUpperUIDraw              = NULL;
      m_pLowerUIDraw              = NULL;
      m_pPokeModelSys             = NULL;
      m_pDrawEnvNode              = NULL;
      m_pCamera                   = NULL;
      m_pPokeModelObj             = NULL;
      m_arcId                     = 0xffffff;
      m_versionType               = VERSION_TYPE_MAX;

      ::std::memset( m_pFileCtrlStateList, NULL, sizeof( m_pFileCtrlStateList ) );
    }

    APP_PARAM*                                                m_pAppParam;
    app::util::Heap*                                          m_pAppHeap;
    gfl2::heap::HeapBase*                                     m_pManagerHeapBase;

    StateType                                                 m_state;
    
    TitleUpperUIDraw*                                         m_pUpperUIDraw;
    TitleLowerUIDraw*                                         m_pLowerUIDraw;

    PokeModelObj*                                             m_pPokeModelObj;

    u32                                                       m_arcId;

    PokeTool::PokeModelSystem*                                m_pPokeModelSys;
    gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pDrawEnvNode;  // カメラを含む環境
    poke_3d::model::BaseCamera*                               m_pCamera;       // カメラ

    VersionType                                               m_versionType;

    //  ファイル制御リスト
    app::util::AppUtilFileControlState* m_pFileCtrlStateList[ FILE_DATA_MAX ];
  };

GFL_NAMESPACE_END( title )
GFL_NAMESPACE_END( app )


#endif // NIJI_PROJECT_TITLE_FRAME_H_INCLUDED
