#if PM_DEBUG

//=================================================================================================
/**
 * @file DebugPokeMake.h
 * @brief デバッグポケモン作成
 * @author obata_toshihiro
 * @date 2011.07.07
 */
//=================================================================================================
#ifndef __DEBUGPOKEMAKE_H__
#define __DEBUGPOKEMAKE_H__

#include <heap/include/gfl2_Heap.h>
#include <heap/include/gfl2_NwAllocator.h>
#include <Layout/include/gfl2_Layout.h>
#include <pml/include/pml_PokePara.h>
#include <GameSys/include/GameProc.h>
#include <Battle/include/battle_DefaultPowerUp.h>


namespace debug {
  namespace PokeMake {

    class PokeMakeCore;


    /**
     * @brief 呼び出しパラメータ
     */
    struct APP_PARAM {
      pml::pokepara::PokemonParam* poke;               ///< 編集するポケモンのポインタ
      DefaultPowerUpDesc*          defaultPowerUpDesc; ///< 編集するデフォルト能力アップ設定のポインタ
      bool                         result;             ///< 編集結果を返す（true:作成  false:キャンセル）
      int                          param;              ///< コールバック関数などで使える任意のパラメータ領域
    };


    class DebugPokeMake : public GameSys::GameProc
    {
      GFL_FORBID_COPY_AND_ASSIGN( DebugPokeMake );

      public:

      /**
       * @brief コンストラクタ
       */
      DebugPokeMake( APP_PARAM *param );

      /**
       * @brief デストラクタ
       */
      virtual ~DebugPokeMake();

      // プロセス関数
      virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* proc_manager );
      virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* proc_manager );
      virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* proc_manager );
      virtual void DrawFunc( gfl2::proc::Manager* proc_manager, gfl2::gfx::CtrDisplayNo displayNo );


      static gfl2::heap::HeapBase* DeviceHeap( void ) { return m_device_heap; }
      static gfl2::str::StrBuf* WorkStrBuf( int index = 0 ) { return m_work_strbuf[ index ]; }
      static gfl2::heap::NwAllocator* NwHeapAllocator( void ) { return m_nw_heap_allocator; }
      static gfl2::heap::NwAllocator* NwDeviceAllocator( void ) { return m_nw_device_allocator; }
#if 0
      static gfl::grp::GraphicsSystem* GraphicsSystem( void ) { return m_graphics_system; }
      static gfl::grp::g3d::System* G3dSystem( void ) { return m_g3d_system; }
#endif
      static gfl2::lyt::LytSys* LayoutSystem( void ) { return m_layout_system; }
      static const gfl2::ui::DeviceManager* UiDeviceManager( void ) { return m_ui_device_manager; }
      static pml::pokepara::PokemonParam* TargetPokemonParam( void ) { return m_param->poke; }
      static pml::pokepara::PokemonParam* ClonePokemonParam( void ) { return m_clone_param; }
      static DefaultPowerUpDesc* CloneDefaultPowerUpDesc( void ) { return &m_clone_defaultPowerUpDesc; }

//      static const gfl2::lyt::LytSysSetUp LAYOUT_SYSTEM_SETUP;



      private:
      static const int WORK_STRBUF_NUM  = 3;

      static const u32 WORK_STRBUF_LENGTH;
      static gfl2::heap::HeapBase* m_device_heap;
      static gfl2::str::StrBuf* m_work_strbuf[ WORK_STRBUF_NUM ];
      static gfl2::heap::NwAllocator* m_nw_heap_allocator;
      static gfl2::heap::NwAllocator* m_nw_device_allocator;
#if 0
      static gfl::grp::GraphicsSystem* m_graphics_system;
      static gfl::grp::g3d::System* m_g3d_system;
#endif
      static gfl2::lyt::LytSys* m_layout_system;
      static const gfl2::ui::DeviceManager* m_ui_device_manager;
      static pml::pokepara::PokemonParam* m_clone_param;
      static DefaultPowerUpDesc m_clone_defaultPowerUpDesc;
      static APP_PARAM *m_param;
  //    static pml::pokepara::PokemonParam* m_target_param;
  //    static bool m_make_result;
    
      PokeMakeCore* m_core;
      bool m_isDrawEnable;
      bool m_fast_mode_switch_flag;
    };


  } // namespace PokeMake
} // namespace debug


#endif // __DEBUGPOKEMAKE_H__

#endif // PM_DEBUG
