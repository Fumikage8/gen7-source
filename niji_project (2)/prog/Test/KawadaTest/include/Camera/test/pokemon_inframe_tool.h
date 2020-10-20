
#if PM_DEBUG

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   pokemon_inframe_tool.h
 *  @brief  ポケモンインフレームカメラ　ツール
 *  @author
 *  @date   2012.11.12
 *  @author kawada_koji@gamefreak.co.jp
 *  @date   2015.11.30
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#ifndef __POKEMON_INFRAME_TOOL_H__
#define __POKEMON_INFRAME_TOOL_H__
#pragma once


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <Layout/include/gfl2_Layout.h>

// pmlのインクルード
#include <pml/include/pmlib.h>

// nijiのインクルード
#include <GameSys/include/GameProc.h>
#include <GameSys/include/GameManager.h>

#include <PokeTool/include/PokeTool.h>

// 前方宣言
namespace System{
  namespace Camera{
    class CModelViewerInFrame;
    class InFrameCamera;
  }
}
namespace System{
  class nijiAllocator;
}
namespace app{
  namespace util{
    class Heap;
    class AppRenderingManager;
  }
}
namespace PokeTool{
  class PokeModelSystem;
  class PokeModel;
}
namespace System{
namespace Camera{
  namespace test{
    class PokemonInframeToolView;
  }
}
}
namespace gfl2{
namespace util{
  class UtilTextRenderingPipeLine;
  class DrawUtilText;
}
}


namespace System{
namespace Camera{
  namespace test{

    //-----------------------------------------------------------------------------
    /**
     * @brief ポケモンインフレームのパラメータ調整を行うツールクラス
     * @brief テストメニューの[PokeInframeTool]を選択すると生成・実行される。
     * @brief ポケモン毎の(枠に対して相対的な)スケール値や、Y軸回転角度などを調整・確認できる。
     */
    //-----------------------------------------------------------------------------
    class PokemonInframeTool : public GameSys::GameProc
    {
    private:
      enum{
        SEQ_INITIALIZE,
        SEQ_UPDATE,
        SEQ_FINALIZE,
        SEQ_POKEINPUT,
        SEQ_POKEINPUT_RECOVER,
      };

      enum{
        SEQ_POKEDATA_UPDATE,
        SEQ_POKEDATA_LOAD_DELETE,
        SEQ_POKEDATA_LOAD_LOAD,
        
        SEQ_POKEDATA_DELETE_START,
        SEQ_POKEDATA_DELETE,
        SEQ_POKEDATA_DELETE_END,
      };
      
      enum{
        RET_NONE,
        RET_POKEINPUT,
        RET_END,
      };

      enum{
        SCENE_GROUP_MAIN,
        SCENE_GROUP_EDGE_MAP,
        SCENE_GROUP_POSTEFFECT,
        SCENE_GROUP_NUM,
      };

      ///< デバッグオブジェクトレベル列挙型
      enum EDebugObjectLevel
      {
        DEBUG_OBJECT_LEVEL_0 = 0 ,
        DEBUG_OBJECT_LEVEL_1     ,
        DEBUG_OBJECT_LEVEL_2     ,
        DEBUG_OBJECT_LEVEL_3     ,

        DEBUG_OBJECT_LEVEL_MAX = DEBUG_OBJECT_LEVEL_3 ,
      };

      static const u32 ROOT_DEVICE_SIZE = 0x1400000;
      static const u32 HEAP_SIZE = 0x10000;
      static const u32 DEVICE_SIZE = 0x1300000;

    private:
      u16 m_Seq;
      u16 m_SubSeq;

      // ヒープ
      gfl2::heap::HeapBase* m_pRootDeviceHeap;
      app::util::Heap* m_pAppHeap;
      System::nijiAllocator* m_pEdgeMapTextureAllocator;


      // 描画システム
      app::util::AppRenderingManager* m_pAppRenderingManager;


      // レイアウトシステム
      PokemonInframeToolView* m_pView;


      // ポケモンモデル
      PokeTool::PokeModelSystem*    m_pPokeModelSys;
      s32                           m_PokeModelSysSeq;
      PokeTool::PokeModel*          m_pPokeModel;
      bool                          m_PokeModelIsLoaded;
      bool                          m_PokeModelIsCreated;
      bool                          m_PokeModelIsAdded;
      PokeTool::SimpleParam         m_PokeParamSimple;
      pml::pokepara::PokemonParam*  m_pPokeParam;
      s32                           m_PokeDataSeq;
      bool m_bIsHIO;


      // びゅーわーで選択されているポケモン
      MonsNo      m_eSelectedPokemonMonsNo;
      pml::FormNo m_nSelectedPokemonFormNo;
      pml::Sex    m_eSelectedPokemonSex;


      // キーボード
      gfl2::str::StrBuf* m_keyboard_buf;
      s32 m_keyboard_choice;
      
      // てきすとぺいん
      gfl2::str::StrBuf* m_layout_string;

      // UI情報
      bool m_UiPokeParamChange;

      bool m_bIsChangeModel; ///< モデルを切り替えるフラグ
      System::Camera::CModelViewerInFrame* m_pInModelInFrame; ///< 枠の中にモデル表示制御クラス
      System::Camera::InFrameCamera* m_pInFrameCamera;

      // 調整パラメータ
      float m_fFrameScale;
      float m_fFramePosiOfstRatioY;
      float m_fModelYaw;
      float m_fModelYawOfstR;
      float m_fFrameLength;

      bool m_bIsEnableDrawDebug; ///< デバッグ描画ができるようになったフラグ
      EDebugObjectLevel m_eDebugObjectLevel; ///< デバッグオブジェクトレベル
      bool m_bIsDispHelp; ///< ヘルプ表示フラグ
      bool m_bIsTurnRightMode; ///< 右向きモードフラグ


      f32 m_fAnimeFrame;
      bool m_isContestMode;


      gfl2::util::UtilTextRenderingPipeLine*  m_drawUtilTextRenderingPipeLine;
      gfl2::util::DrawUtilText*               m_drawUtilText;


    public:
      PokemonInframeTool(); ///< コンストラクタ
      ~PokemonInframeTool() { return ; } ///< デストラクタ

      virtual gfl2::proc::Result InitFunc  (gfl2::proc::Manager* pProcManager); ///< 初期化
      virtual gfl2::proc::Result EndFunc   (gfl2::proc::Manager* pProcManager) { return gfl2::proc::RES_FINISH; } ///< 解放処理
      virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pProcManager); ///< フレーム処理
      virtual void PreDrawFunc(gfl2::proc::Manager* pProcManager);
      virtual void DrawFunc(gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo); ///< 描画処理

    private:
      
      bool initialize( void );
      u32 update( void );
      bool finalize( void );

      void initializeHeap( void );

      void initializeDrawUtilText(void);
      void finalizeDrawUtilText(void);

      void initializeInFrame( void );
      void initializeAppRenderingManager( void );

      void initialize3D( void );
      void update3D( void );
      void draw3D( gfl2::gfx::CtrDisplayNo displayNo );

      void initialize2D( void );

      bool initializePokeModelSys( void );
      bool finalizePokeModelSys( void );

      void initializePokeData( void );
      bool finalizePokeData( void );
      void updatePokeData( void );
      bool isPokeDataLoading() const { return (m_PokeDataSeq != SEQ_POKEDATA_UPDATE); } ///< ポケモン情報読み込み中チェック

      void startCreatePokeModel( void );
      bool destroyPokeModel( void );
      bool isPokeModelEnable( void ) const;
      
      void initializeUi( void );
      u32 updateUi(void);

      u32 updateUi_MonsNo( s32 plus_minus );        // モンスターナンバー変更
      u32 updateUi_FormNo( s32 plus_minus );     // フォルムナンバー変更
      u32 updateUi_Sex( s32 plus_minus );            // 性別変更
      u32 updateUi_Egg( s32 plus_minus );             // 卵
      u32 updateUi_Scale( s32 plus_minus );             // 大きさ
      u32 updateUi_OffsetY( s32 plus_minus );          // Yオフセット
      u32 updateUi_RotateY( s32 plus_minus );          // Y軸回転

      void setupUiDisp();
      void setTextboxPaneString( u32 pane_index, gfl2::str::StrBuf* p_buf );
      void setTextboxPaneString( u32 pane_index, const gfl2::str::STRCODE* p_buf );
      void setTextboxPaneString( u32 pane_index, f32 number, const bool bIsRadian = false);

      void changeMonsNo( MonsNo new_monsno );

      bool isUiPokeParamChange() const  { return m_UiPokeParamChange;  } ///< UIによりポケモンパラメータ変更されたかチェック
      void uiPokeParamChangeFlagReset() { m_UiPokeParamChange = false; } ///< ポケモンパラメータ変更フラグをリセット

      // 関数ポインタ型
      typedef u32 (PokemonInframeTool::*pUiFunc)( s32 plus_minus );
      struct UI_Reaction{
        gfl2::lyt::LytPaneIndex index;  // 反応インデックス
        PokemonInframeTool::pUiFunc func;
        s32     plus_minus;
      };

    private:
      void GetFramePosition(int* pnLeft, int* pnTop, int* pnRight, int* pnBottom); ///< 枠の位置を取得する
      void UpdateByInput(); ///< 入力による更新を行う
      void DispHelp(gfl2::gfx::CtrDisplayNo displayNo);      ///< ヘルプを表示する

      // びゅーわーで選択されているポケモンを取得・設定する
      void GetSelectedPokemonInViewer(MonsNo* peMonsNo, pml::FormNo* pnFormNo, pml::Sex* peSex);
      void SetSelectedPokemonInViewer(const MonsNo eMonsNo, const pml::FormNo nFormNo, const pml::Sex eSex);

    };

  } // namespace test
} // namespace Camera
} // namespace System

#endif // __POKEMON_INFRAME_TOOL_H__
#endif // PM_DEBUG

/*  EOF  */





