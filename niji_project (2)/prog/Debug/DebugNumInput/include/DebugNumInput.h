//=============================================================================
/**
 * @file NumInput.h
 * @brief デバッグ数値入力
 * @author obata_toshihiro
 * @date 2011.07.11
 */
//=============================================================================
#if PM_DEBUG
#ifndef __DEBUGNUMINPUT_H__
#define __DEBUGNUMINPUT_H__ 
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include <System/include/nijiAllocator.h>
#include <System/include/AppRenderingPipeLine.h>
#include <AppLib/include/Tool/app_tool_ButtonManager.h>
#include <AppLib/include/Util/app_util_Controller.h>
#include "../source/temp/DebugNumInputTemp_app_util.h"


namespace debug {


  class NumInput : 
    public app::util::Controller,
    public NumInputTemp::LytUtil,
#if 0
    public app::util::LayoutContainer,
#endif
    public app::tool::ButtonManager::IButtonCallback
  {
    public:

      /**
       * @brief 最大桁数
       */
      static const u32 PLACE_NUM = 7;

      /**
       * @brief 更新結果
       */
      enum Result {
        RES_CONT,    // 入力中
        RES_DECIDE,  // 決定した
        RES_CANCEL,  // キャンセルした
      };

      //-----------------------------------------------------------------------
      // コンストラクタ・デストラクタ
      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       * @param heap             使用するヒープ
       * @param device_heap      使用するデバイスヒープ
       * @param graphics_system  描画に使用するグラフィックスシステム
       * @param layout_system    描画に使用するレイアウトシステム
       * @param play_se          入力時SEを再生するかどうか
       */
      NumInput( 
          gfl2::heap::HeapBase* heap,
          gfl2::heap::HeapBase* device_heap,
#if 0
          gfl::grp::GraphicsSystem* graphisc_system,
#endif
          gfl2::lyt::LytSys* layout_system,
          bool play_se = true );

      /**
       * @brief デストラクタ
       */
      virtual ~NumInput();

      //-----------------------------------------------------------------------
      // 基本動作
      //-----------------------------------------------------------------------
      /**
       * @brief 入力を開始する
       * @param title  タイトル
       * @param init   初期値
       * @param min    最小値
       * @param max    最大値
       */
      void Start( const wchar_t* title, s32 init, s32 min, s32 max );

      /**
       * @brief 更新処理
       * @return 結果
       */
      Result Update( void ); 

      /**
       * @brief 入力された値を取得する
       * @note 入力中なら, 表示されている値が返ります。
       */
      s32 GetValue( void ) const;

      /**
       * @brief 描画処理
       */
      void Draw( void ); 

      /**
       * @brief   レイアウトワークを取得
       * @return  レウアウトワーク
       */
      gfl2::lyt::LytWk * GetLayoutWork(void);




    private:

      enum Sequence {
        SEQ_WAIT_INPUT,  // 入力待ち
        SEQ_WAIT_ANIME,  // アニメ終了待ち
        SEQ_FINISH,      // 終了
        SEQ_NUM,
      };

      enum ButtonIndex {
        BUTTON_SIGN,
        BUTTON_PLACE_00_UP,
        BUTTON_PLACE_01_UP,
        BUTTON_PLACE_02_UP,
        BUTTON_PLACE_03_UP,
        BUTTON_PLACE_04_UP,
        BUTTON_PLACE_05_UP,
        BUTTON_PLACE_06_UP,
        BUTTON_PLACE_00_DOWN,
        BUTTON_PLACE_01_DOWN,
        BUTTON_PLACE_02_DOWN,
        BUTTON_PLACE_03_DOWN,
        BUTTON_PLACE_04_DOWN,
        BUTTON_PLACE_05_DOWN,
        BUTTON_PLACE_06_DOWN,
        BUTTON_OK,
        BUTTON_CANCEL,
        BUTTON_NUM,
        BUTTON_NULL,
      };

      enum AnimeDataIndex {
        ANIME_DATA_DUMMY_BUTTON_HOLD,
        ANIME_DATA_DUMMY_BUTTON_RELEASE,
        ANIME_DATA_DUMMY_BUTTON_SELECT,
        ANIME_DATA_FOCUS_PLACE,
        ANIME_DATA_TITLE_SCROLL,
        ANIME_DATA_NUM,
      };

      enum ShareAnimeIndex {
        SHARE_ANIME_BUTTON_HOLD,
        SHARE_ANIME_BUTTON_RELEASE,
        SHARE_ANIME_BUTTON_SELECT,
        SHARE_ANIME_NUM,
      };

      enum AnimeIndex {
        ANIME_DUMMY_BUTTON_HOLD,
        ANIME_DUMMY_BUTTON_RELEASE,
        ANIME_DUMMY_BUTTON_SELECT,
        ANIME_FOCUS_PLACE,
        ANIME_TITLE_SCROLL,
        // 以下, 共有アニメ
        ANIME_BUTTON_HOLD,
        ANIME_BUTTON_RELEASE = ANIME_BUTTON_HOLD + BUTTON_NUM,
        ANIME_BUTTON_SELECT = ANIME_BUTTON_RELEASE + BUTTON_NUM,
        ANIME_NUM = ANIME_BUTTON_SELECT + BUTTON_NUM,
        ANIME_NULL,
      };

      struct PlaceDesc {
        gfl2::lyt::LytPaneIndex num_pane;
        gfl2::lyt::LytPaneIndex up_pane;
        gfl2::lyt::LytPaneIndex down_pane;
        ButtonIndex inc_button;
        ButtonIndex dec_button;
      };

      struct ButtonDesc {
        gfl2::lyt::LytPaneIndex pane_index;
        AnimeIndex hold_anime;
        AnimeIndex release_anime;
        AnimeIndex select_anime;
        u32 select_SE;
      }; 


      static const gfl2::lyt::LytWkSetUp LAYOUT_WORK_SETUP_DATA;
#if 0
      static const gfl::grp::ProjectionParam PROJECTION_PARAM[2];
      static const gfl::grp::ViewParam VIEW_PARAM[2];
      static const gfl::grp::FragmentOperationParam OPERATION_PARAM;
#endif
      static const gfl2::lyt::LytArcIndex ANIME_DATA_TABLE[ ANIME_DATA_NUM ];
#if 0
      static const app::util::LayoutContainer::ShareAnimeDesc SHARE_ANIME_TABLE[ SHARE_ANIME_NUM ];
#endif
      static const PlaceDesc PLACE_DESC[ PLACE_NUM ];
      static const ButtonDesc BUTTON_DESC[ BUTTON_NUM ];

#if 0
      gfl::grp::GraphicsSystem* m_graphics_system;
#endif
      app::tool::ButtonManager* m_button_manager;

      AppRenderingPipeLine*        m_renderingPipeLine;
      gfl2::heap::NwAllocator*     m_sysAllocator;
      gfl2::heap::NwAllocator*     m_devAllocator;

      gfl2::lyt::LytSys*           m_lytSys;
      gfl2::lyt::LytWk*            m_lytWk;
      gfl2::lyt::LytMultiResID     m_lytResID;
      void*                        m_lytDataBuffer;

      s32 m_min_value;            // 入力できる最小値
      s32 m_max_value;            // 入力できる最大値
      s32 m_value;                // 入力値
      u8 m_max_place_index;       // 最大桁のインデックス
      u8 m_current_place_index;   // 現在操作中の桁インデックス
      Sequence m_seq;
      Result m_result;
      AnimeIndex m_wait_anime;
      bool m_mute;


      void CreateRenderingPipeLine( gfl2::heap::HeapBase* systemHeap, gfl2::heap::HeapBase* deviceHeap );
      void DeleteRenderingPipeLine( void );

      void CreateButtonManager( gfl2::heap::HeapBase* heap );
      void DeleteButtonManager( void );
      void UpdateSeq_WAIT_INPUT( void );
      void UpdateSeq_WAIT_ANIME( void );
      u32 CalcPlaceNumber( s32 value, u8 place_index );
      u32 CalcPlaceCount( s32 value );
      void SwitchNumSign( void );
      void IncPlaceNumber( u8 place_index );
      void DecPlaceNumber( u8 place_index );
      void UpdateDisplayValue( void );
      void ChangePlaceIndex( s8 place_index );
      void UpdatePlaceNumber( u32 place_index, u32 number );

      /**
       * @brief ボタンが選択された際に呼ばれるコールバック関数
       * @param buttonID  選択されたボタンのID
       */
      virtual void CallbackFuncOnButtonSelected( u32 buttonID );

  };


} // namespace debug 


#endif // __DEBUGNUMINPUT_H__
#endif // PM_DEBUG
