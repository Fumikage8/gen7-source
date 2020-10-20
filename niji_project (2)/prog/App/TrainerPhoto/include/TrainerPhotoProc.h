//======================================================================
/**
 * @file TrainerPhotoProc.h
 * @date 2015/08/31 18:12:31
 * @author uchida_yuto
 * @brief 証明写真アプリProc
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __TRAINER_PHOTO_PROC_H_INCLUDED__
#define __TRAINER_PHOTO_PROC_H_INCLUDED__
#pragma once

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include "GameSys/include/GameProc.h"
#include "GameSys/include/GameManager.h"
#include "AppLib/include/Util/AppRenderingManager.h"

//  証明写真のフレーム群
#include "App/TrainerPhoto/source/Frame/TrainerPhotoModelViewerFrame.h"
#include "App/TrainerPhoto/source/Frame/TrainerPhotoModelShootFrame.h"
#include "App/TrainerPhoto/source/Frame/TrainerPhotoShootConfirmFrame.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(print)

  class WordSet;

GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)

  class MsgData;

GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(applib)
GFL_NAMESPACE_BEGIN(frame)

  class Manager;

GFL_NAMESPACE_END(frame)
GFL_NAMESPACE_END(applib)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class AppToolTrainerIconRendering;
  class CharaSimpleModel;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class AppUtilFileControlState;
  class Heap;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(System)

  class nijiAllocator;

GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(photo)

  class TrainerPhotoUIViewPool;

  //  処理終了時の結果
  enum ProcRet
  {
    //  撮影キャンセル
    PROC_RET_SHOOT_CANCEL  = 0,
    //  撮影成功
    PROC_RET_SHOOT_SUCCESS
  };

  //  procアプリ用のパラメータ
  struct APP_PARAM
  {
    /** 出力用パラメータ */
    struct _tag_out_param
    {
      ProcRet ret;

      _tag_out_param()
      {
        ret = PROC_RET_SHOOT_CANCEL;
      }

    } out_param;
  };

  //==============================================================================
  /**
   @class     TrainerPhotoProc
   */
  //==============================================================================
  class TrainerPhotoProc : public GameSys::GameProc
  {
    GFL_FORBID_COPY_AND_ASSIGN(TrainerPhotoProc);

  public:
    TrainerPhotoProc();
    virtual ~TrainerPhotoProc();

    //------------------------------------------------------------------
    /**
     * @brief	  動作に必要な引数を登録
     * @note
     *    必ずインスタンス生成の段階で呼び出さないと後の動作保障はなし
     */
    //------------------------------------------------------------------
    void Initialize( APP_PARAM* pAppParam );

    virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
    virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
    virtual void PreDrawFunc( gfl2::proc::Manager* pManager );
    virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );
    virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pManager );

  private:
    enum Frame
    {
      FRAME_MODEL_VIEWER  = 0,
      FRAME_MODEL_SHOOT,
      FRAME_SHOOT_CONFIRM,
      FRAME_END
    };

    //  フレーム作成
    applib::frame::CellSuper* _CreateFrame( const Frame frame );

    void _Clear()
    {
      m_nowFrame              = FRAME_MODEL_VIEWER;
      m_trainerId             = 0;
      m_pNowFrameCell         = NULL;

      m_pHeapMem                = NULL;
      m_pDevMem                 = NULL;
      m_pUtilHeap               = NULL;
      m_pRenderingManager       = NULL;
      m_pAppParam               = NULL;
      m_pFrameManager           = NULL;
      m_pTrainerIconRendering   = NULL;
      m_pCharaModel             = NULL;
      m_pUIViewPool             = NULL;
      m_pLytFileCtrlState       = NULL;
      m_pAppMsgData             = NULL;
      m_pAppWordSet             = NULL;
      m_pCharaModelDrawEnv      = NULL;
      m_pCharaModelLightDrawEnv = NULL;
      m_pCharaModelCamera       = NULL;

      m_bDefaultViewerParam       = true;
      m_bDefaultShootViewerParam  = true;

      m_pNijiAllocatorForEdgeMapTexture = NULL;
    }

    //  外部で設定するパラメータ参照
    struct APP_PARAM*                           m_pAppParam;

    u32                                         m_trainerId;

    gfl2::heap::HeapBase*                       m_pHeapMem;  // heapMemでなければならないところに使うメモリ
    gfl2::heap::HeapBase*                       m_pDevMem;   // devMemでなければならないところに使うメモリ

    app::util::Heap*                            m_pUtilHeap;

    applib::frame::Manager*                     m_pFrameManager;

    app::util::AppRenderingManager*             m_pRenderingManager;

    //  表示するキャラモデル
    app::tool::CharaSimpleModel*                m_pCharaModel;

    //  3Dモデル描画用
    System::nijiAllocator*                      m_pNijiAllocatorForEdgeMapTexture;  // VRAM限定。

    //  トレーナーアイコン
    app::tool::AppToolTrainerIconRendering*     m_pTrainerIconRendering;

    //  フレーム制御
    Frame                                       m_nowFrame;
    applib::frame::CellSuper*                   m_pNowFrameCell;

    //  アプリで使うUIViewプールクラス
    TrainerPhotoUIViewPool*                     m_pUIViewPool;

    //  レイアウトファイル
    app::util::AppUtilFileControlState*         m_pLytFileCtrlState;

    //  モデル表示の3D環境
    gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pCharaModelDrawEnv;
    gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pCharaModelLightDrawEnv;
    poke_3d::model::BaseCamera*                 m_pCharaModelCamera;

    //  アプリ内で使うメッセージ
    gfl2::str::MsgData*                         m_pAppMsgData;
    print::WordSet*                             m_pAppWordSet;

    //  「すがたをみる/しゃしんをとる」フレームの3D環境設定のデフォルトフラグ
    bool                                        m_bDefaultViewerParam;
    bool                                        m_bDefaultShootViewerParam;

    //  フレームの外部パラメータ
    TrainerPhotoModelViewerFrame::FRAME_PARAM   m_modelViewrFrameParam;
    TrainerPhotoModelShootFrame::FRAME_PARAM    m_modelShootFrameParam;
    TrainerPhotoShootConfirmFrame::FRAME_PARAM  m_shootConfirmFrameParam;
  };

GFL_NAMESPACE_END(photo)
GFL_NAMESPACE_END(app)

#endif // __TRAINER_PHOTO_PROC_H_INCLUDED__