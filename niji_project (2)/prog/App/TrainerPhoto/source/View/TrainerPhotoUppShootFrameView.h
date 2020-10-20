﻿//======================================================================
/**
 * @file TrainerPhotoUppShootFrameView.h
 * @date 2015/09/02 16:23:10
 * @author uchida_yuto
 * @brief 証明写真アプリの上画面撮影フレームView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __TRAINER_PHOTO_UPP_SHOOT_FRAME_VIEW_H_INCLUDED__
#define __TRAINER_PHOTO_UPP_SHOOT_FRAME_VIEW_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "AppLib/include/ui/UIView.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(print)

  class WordSet;

GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(photo)

  /**
   * @class TrainerPhotoUppShootFrameView
   * @brief 証明写真アプリの上画面撮影View
   */
  class TrainerPhotoUppShootFrameView : public app::ui::UIView
  {
      GFL_FORBID_COPY_AND_ASSIGN(TrainerPhotoUppShootFrameView);

  public:
    //  状態一覧
    enum State
    {
      STATE_IDLE  = 0,
    };

    //  初期化パラメータ
    struct INIT_PARAM
    {
      INIT_PARAM()
      {
        pHeap           = NULL;
        pRenderManager  = NULL;
        pLytBinData     = NULL;
        pAppMsgData     = NULL;
        pWordSet        = NULL;
      }

      app::util::Heap* pHeap;
      app::util::AppRenderingManager* pRenderManager;
      void* pLytBinData;
      gfl2::str::MsgData* pAppMsgData;
      print::WordSet* pWordSet;
    };

    //------------------------------------------------------------------
    /**
      * @brief   コンストラクタ
      */
    //------------------------------------------------------------------
    TrainerPhotoUppShootFrameView( INIT_PARAM& rInitParam );

    virtual ~TrainerPhotoUppShootFrameView();

    //------------------------------------------------------------------
    /**
      * @brief   初期化
      */
    //------------------------------------------------------------------
    bool InitFunc();

    //------------------------------------------------------------------
    /**
      * @brief   終了処理
      */
    //------------------------------------------------------------------
    bool EndFunc();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  更新処理
    *         子ウインドウについても自動的にコールする(子→親の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Update();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  描画処理
    *         子ウインドウについても自動的にコールする(親→子の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Draw(  gfl2::gfx::CtrDisplayNo displayNo );

    //------------------------------------------------------------------
    /**
      * @brief   シャッターイベント開始
      */
    //------------------------------------------------------------------
    void DoEventShutter();

    //------------------------------------------------------------------
    /**
      * @brief   シャッターイベント中か
      */
    //------------------------------------------------------------------
    bool IsEventShutter();

    //------------------------------------------------------------------
    /**
      * @brief   状態設定
      * @param[in] newStaet 設定する状態
      */
    //------------------------------------------------------------------
    void SetState( const State newStaet );

  private:
    //  作成するレイアウトワークの種類
    enum
    {
      LYT_WORK_NORMAL  = 0,
      LYT_WORK_MAX
    };

    //------------------------------------------------------------------
    /**
      * @brief   2D画面の作成
      * @param   pAppLytBuff レイアウトデータのバイナリポインタ
      */
    //------------------------------------------------------------------
    void _Initialize2D( void* pAppLytBuff, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet );

    //------------------------------------------------------------------
    /**
      * @brief   変数の初期化はここに
      */
    //------------------------------------------------------------------
    void _Clear()
    {
      m_state           = STATE_IDLE;
      m_initSeq         = 0;
      m_endSeq          = 0;
    }

    State                           m_state;

    u32                             m_initSeq;
    u32                             m_endSeq;

    INIT_PARAM                      m_initParam;
  };

GFL_NAMESPACE_END(photo)
GFL_NAMESPACE_END(app)

#endif // __TRAINER_PHOTO_UPP_SHOOT_FRAME_VIEW_H_INCLUDED__