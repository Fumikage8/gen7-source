//======================================================================
/**
* @file DendouDemoProc.h
* @date 2016/02/26 19:17:34
* @author nagihashi_toru(移植), Hiroyuki Nakamura
* @brief 殿堂デモProc
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if !defined __DENDOU_DEMO_PROC_HEADER_H_INCLUDED__
#define __DENDOU_DEMO_PROC_HEADER_H_INCLUDED__
#pragma once

// niji/gflib2
#include <heap/include/gfl2_Heap.h>
#include <GameSys/include/GameProc.h>
#include <AppLib/include/Util/app_util_heap.h>
namespace pml { class PokeParty; }
namespace app { namespace util { class Heap; class AppRenderingManager; } }
namespace applib { namespace frame { class Manager; } }
namespace System { class nijiAllocator; }
namespace Savedata { class MyStatus; }

//mine
namespace app { namespace dendou_demo { class Frame; } }

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
* @namespace	app::dendou_demo
* @brief			殿堂入りデモ
*/
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace app {
  namespace dendou_demo {

    //--------------------------------------------------------------------------------------------
    /**
    * @brief	外部設定パラメータ（初期化データ・終了時のデータなど）
    */
    //--------------------------------------------------------------------------------------------
    struct APP_PARAM {
      // [ IN ]
      const pml::PokeParty * pPokeParty;	//!< 手持ちポケモン
	    const Savedata::MyStatus * pMyStatus;					//!< プレイヤーデータ
      u32 defensedCount;  //!< 防衛回数
      bool isFirstClear;  //!< 初回クリアかどうか
    };

    //--------------------------------------------------------------------------------------------
    /**
    * @brief	Proc
    */
    //--------------------------------------------------------------------------------------------
    class Proc : public GameSys::GameProc {		// 基低クラス[Gamesys::GameProc]を継承
    public:
      //--------------------------------------------------------------------------------------------
      /**
      * @brief		コンストラクタ
      *
      * @param		prm		外部設定パラメータ
      */
      //--------------------------------------------------------------------------------------------
      Proc( APP_PARAM * prm );

      //--------------------------------------------------------------------------------------------
      /**
      * @brief		デストラクタ
      */
      //--------------------------------------------------------------------------------------------
      virtual ~Proc();

      //--------------------------------------------------------------------------------------------
      /**
      * @brief		Proc初期化処理
      *
      * @param		proc_manager		gfl2::proc::Manager
      *
      * @return	gfl2::proc::Result
      */
      //--------------------------------------------------------------------------------------------
      virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* proc_manager );

      //--------------------------------------------------------------------------------------------
      /**
      * @brief		Proc終了処理
      *
      * @param		proc_manager		gfl2::proc::Manager
      *
      * @return	gfl2::proc::Result
      */
      //--------------------------------------------------------------------------------------------
      virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager * proc_manager );

      //--------------------------------------------------------------------------------------------
      /**
      * @brief		Procアップデート処理
      *
      * @param		proc_manager		gfl2::proc::Manager
      *
      * @return	gfl2::proc::Result
      */
      //--------------------------------------------------------------------------------------------
      virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager * proc_manager );

      //--------------------------------------------------------------------------------------------
      /**
      * @brief		ProcPre描画処理
      *
      * @param		proc_manager		gfl2::proc::Manager
      *
      * @return	none
      */
      //--------------------------------------------------------------------------------------------
      virtual void PreDrawFunc( gfl2::proc::Manager* proc_manaer );

      //--------------------------------------------------------------------------------------------
      /**
      * @brief		Proc描画処理
      *
      * @param		proc_manager		gfl2::proc::Manager
      * @param		display_no  		gfl2:gfx:CtrDisplayNo
      *
      * @return	none
      */
      //--------------------------------------------------------------------------------------------
      virtual void DrawFunc( gfl2::proc::Manager* proc_manager, gfl2::gfx::CtrDisplayNo display_no );

      //--------------------------------------------------------------------------------------------
      /**
      * @brief		フレームモード取得
      *
      * @param		none
      *
      * @return	gamesystem::FRAMEMODE_30
      */
      //--------------------------------------------------------------------------------------------
      virtual GameSys::FrameMode GetFrameMode( void ) const { return GameSys::FRAMEMODE_30; }


    private:
      //--------------------------------------------------------------------------------------------
      /**
      * @brief		システムヒープ作成
      *
      * @param		none
      *
      * @return	none
      */
      //--------------------------------------------------------------------------------------------
      void CreateHeap(void);

      //--------------------------------------------------------------------------------------------
      /**
      * @brief		システムヒープ削除
      *
      * @param		none
      *
      * @return	none
      */
      //--------------------------------------------------------------------------------------------
      void DeleteHeap(void);

      //--------------------------------------------------------------------------------------------
      /**
      * @brief	  レンダリングパイプライン作成
      */
      //--------------------------------------------------------------------------------------------
      void CreateRenderingManager(void);
      //--------------------------------------------------------------------------------------------
      /**
      * @brief	  レンダリングパイプライン作成待ち
      */
      //--------------------------------------------------------------------------------------------
      bool WaitRenderingManager(void);
      //--------------------------------------------------------------------------------------------
      /**
      * @brief	  レンダリングパイプライン破棄
      * @return	true 破棄完了 false 破棄できなかった
      */
      //--------------------------------------------------------------------------------------------
      bool DeleteRenderingManager(void);
      //--------------------------------------------------------------------------------------------
      /**
      * @brief	  Frameの作成
      */
      //--------------------------------------------------------------------------------------------
      void CreateFrame(void);
      //--------------------------------------------------------------------------------------------
      /**
      * @brief	  Frameの破棄
      * @return	true Frameの破棄 false 破棄できなかった
      */
      //--------------------------------------------------------------------------------------------
      bool DeleteFrame(void);
      //--------------------------------------------------------------------------------------------
      /**
      * @brief	  MainFrame実行
      */
      //--------------------------------------------------------------------------------------------
      void CallMainFrame(void);

    private:
      APP_PARAM * pParam;
      app::util::Heap appHeap;   //!< アプリ用ヒープ(確保用ヒープ作成前なので実体として宣言する)
      gfl2::heap::HeapBase * pFrameManagerHeap; //!< フレームマネージャ用（フレームマネージャはヒープが荒れるらしい）
      System::nijiAllocator * pNijiAllocator; //!< appRenderingManagerに渡すアロケータ

      app::util::AppRenderingManager * pAppRenderingManager; //!< レンダリングマネージャ
      applib::frame::Manager * pFrameManager; //!< フレームマネージャ
      Frame * pFrame;						//!< メインシステム
    };

  } //dendou_demo
} //app

#endif // __DENDOU_DEMO_PROC_HEADER_H_INCLUDED__