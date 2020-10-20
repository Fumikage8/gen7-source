//=============================================================================
/**
 * @file DebugPokeMake.cpp
 * @brief デバッグポケモン作成
 * @author obata_toshihiro
 * @date 2011.07.07
 */
//=============================================================================
#if PM_DEBUG

#include <System/include/HeapDefine.h>
#include <Debug/DebugPokeMake/include/DebugPokeMake.h>
#include "./DebugPokeMakeCore.h"
#include <Print/include/SystemFont.h>
#include <GameSys/include/GameManager.h>
#include "GameSys/include/GameData.h"
#include "Savedata/include/ZukanSave.h"


namespace debug {
namespace PokeMake {




  // 作業用文字列バッファの長さ
  const u32 DebugPokeMake::WORK_STRBUF_LENGTH = 128; 

  gfl2::heap::HeapBase* DebugPokeMake::m_device_heap = NULL;
  gfl2::str::StrBuf* DebugPokeMake::m_work_strbuf[DebugPokeMake::WORK_STRBUF_NUM] = {NULL, NULL, NULL};
  gfl2::heap::NwAllocator* DebugPokeMake::m_nw_heap_allocator = NULL;
  gfl2::heap::NwAllocator* DebugPokeMake::m_nw_device_allocator = NULL;
#if 0
  gfl::grp::GraphicsSystem* DebugPokeMake::m_graphics_system = NULL;
  gfl::grp::g3d::System* DebugPokeMake::m_g3d_system = NULL;
#endif
  gfl2::lyt::LytSys* DebugPokeMake::m_layout_system = NULL;
  const gfl2::ui::DeviceManager* DebugPokeMake::m_ui_device_manager = NULL;
  pml::pokepara::PokemonParam* DebugPokeMake::m_clone_param=NULL;
  debug::PokeMake::APP_PARAM*    DebugPokeMake::m_param = NULL;
  DefaultPowerUpDesc DebugPokeMake::m_clone_defaultPowerUpDesc;


  //---------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //---------------------------------------------------------------------------
  DebugPokeMake::DebugPokeMake( APP_PARAM *param ) : 
    GameSys::GameProc(),
    m_core( NULL ),
    m_isDrawEnable( true ),
    m_fast_mode_switch_flag( false )
  {
    m_param = param;
  } 

  //---------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //---------------------------------------------------------------------------
  DebugPokeMake::~DebugPokeMake()
  {
  } 

  //---------------------------------------------------------------------------
  /**
   * @brief プロセス初期化関数
   * @param proc_manager  プロセスマネージャ
   */
  //---------------------------------------------------------------------------
  gfl2::proc::Result DebugPokeMake::InitFunc( gfl2::proc::Manager* proc_manager )
  {
    GFL_ASSERT( m_param->poke ); // @check

    static const size_t DEVICE_HEAP_SIZE = 0x400000;

    // アロケーター作成 
    gfl2::heap::HeapBase* parent_dev_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
    m_device_heap = GFL_CREATE_LOCAL_HEAP_NAME( parent_dev_heap, DEVICE_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "HEAPID_DEBUG_POKE_MAKE_DEVICE" );
    m_nw_heap_allocator = GFL_NEW( m_device_heap ) gfl2::heap::NwAllocator( m_device_heap );
    m_nw_device_allocator = GFL_NEW( m_device_heap ) gfl2::heap::NwAllocator( m_device_heap );
    // グラフィックシステム初期化
    const GameSys::GameManager* game_manager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    m_ui_device_manager = game_manager->GetUiDeviceManager();
#if 0
    m_graphics_system = game_manager->GetGraphicsSystem();
    m_graphics_system->InitializeApplicationMemory( m_nw_device_allocator );
    gfl::grp::util::DrawUtil::PushAllocator( m_nw_heap_allocator, m_nw_device_allocator );

    m_g3d_system = GFL_NEW( m_device_heap ) gfl::grp::g3d::System( m_nw_heap_allocator, m_nw_device_allocator );
#endif

    {
      // レイアウトシステムのセットアップデータ
      const gfl2::lyt::LytSysSetUp LAYOUT_SYSTEM_S = 
      {
        GFL_SINGLETON_INSTANCE(print::SystemFont)->GetRectDrawerShaderRes(),
        128, // 管理リソース最大数
        10,  // 管理レイアウトワーク最大数
      }; 
      
      m_layout_system = GFL_NEW( m_device_heap ) gfl2::lyt::LytSys( &LAYOUT_SYSTEM_S, m_nw_heap_allocator, m_nw_device_allocator, GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetDrawingLytWkManager() );
    }
    //gfl2::lyt::LytSys::SetDefaultFont( SystemFont_GetFont(), print::LAYOUT_DEFAULT_FONT ); 不要になったためコメントアウト tomoya  
    // PokemonParam作成
    m_clone_param = GFL_NEW( m_device_heap ) pml::pokepara::PokemonParam( m_device_heap, MONSNO_HUSIGIDANE, 5, 0 );
    m_clone_param->CopyFrom( *m_param->poke ); 
#if 0 //なぜこれがあるのか？
    if( m_clone_param->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) ) {
      m_clone_param->Clear();
    }
#endif
    m_fast_mode_switch_flag = m_clone_param->StartFastMode();

    DEFAULT_POWERUP_DESC_Copy( &m_clone_defaultPowerUpDesc, *m_param->defaultPowerUpDesc );

    for(int i=0; i<WORK_STRBUF_NUM;++i){
      m_work_strbuf[i] = GFL_NEW( m_device_heap ) gfl2::str::StrBuf( WORK_STRBUF_LENGTH, m_device_heap );
    }
    // デバッグポケモン作成メニュー起動
    m_core = GFL_NEW( m_device_heap ) PokeMakeCore();
    return gfl2::proc::RES_FINISH;
  }

  //---------------------------------------------------------------------------
  /**
   * @brief プロセス終了関数
   * @param proc_manager  プロセスマネージャ
   */
  //---------------------------------------------------------------------------
  gfl2::proc::Result DebugPokeMake::EndFunc( gfl2::proc::Manager* proc_manager )
  {
    m_isDrawEnable = false;

    // 描画中は解放しない
    if( m_core->IsDrawing() )
    {
      return gfl2::proc::RES_CONTINUE;
    }

    // キャンセルしていなければ適用
    if(m_core->GetResult())
    {
      m_param->poke->CopyFrom( *m_clone_param );  // 編集結果をコピーして代入
      DEFAULT_POWERUP_DESC_Copy( m_param->defaultPowerUpDesc, m_clone_defaultPowerUpDesc );
      m_param->poke->SetDebugEditFlag(true);  //デバッグでいじられたことを覚えておく
      m_param->result = true;
      GameSys::GameManager* game_manager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
      Savedata::ZukanData * zukan = game_manager->GetGameData()->GetZukanData();
      zukan->SetPokeGet( *m_clone_param );         // 図鑑登録
    }

    GFL_DELETE m_core;
    for(int i=0; i<WORK_STRBUF_NUM;++i){ 
      GFL_DELETE m_work_strbuf[i]; 
    }

    m_clone_param->EndFastMode( m_fast_mode_switch_flag );
    GFL_DELETE m_clone_param;
    GFL_DELETE m_layout_system;
#if 0
    GFL_DELETE m_g3d_system;
#endif
    GFL_DELETE m_nw_heap_allocator;
    GFL_DELETE m_nw_device_allocator;
    gfl2::heap::Manager::DeleteHeap( m_device_heap );
    return gfl2::proc::RES_FINISH;
  } 

  //---------------------------------------------------------------------------
  /**
   * @brief プロセス描画関数
   * @param proc_manager  プロセスマネージャ
   */
  //---------------------------------------------------------------------------
  void DebugPokeMake::DrawFunc( gfl2::proc::Manager* proc_manager, gfl2::gfx::CtrDisplayNo displayNo )
  {
    if( !m_isDrawEnable ) {
      return;
    }
    m_core->Draw( displayNo );
  } 

  //---------------------------------------------------------------------------
  /**
   * @brief プロセス更新関数
   * @param proc_manager  プロセスマネージャ
   */
  //---------------------------------------------------------------------------
  gfl2::proc::Result DebugPokeMake::UpdateFunc( gfl2::proc::Manager* proc_manager )
  {
    bool finished = m_core->Update();

    return ( finished ) ? 
      gfl2::proc::RES_FINISH : 
      gfl2::proc::RES_CONTINUE;
  } 


} // namespace PokeMake
} // namespace debug

#endif // PM_DEBUG
