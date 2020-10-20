//======================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file   pml_use.cpp
 *  @brief  PMLibを動作させるための初期化処理
 *  @date   2011.04.21
 *
 */
//====================================================================== 

#include <debug/include/gfl2_DebugPrint.h>

#include <pml/include/pmlib.h>
#include <pml/include/item/item.h>

#include "GameSys/include/GameManager.h"

#include <Print/include/PrintSystem.h>
#include "System/include/PokemonVersion.h"
#include "System/include/PmlUse.h"
#include "System/include/GflUse.h"
#include "./pokelib/NijiEvolveManager.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include "PmlUse.cdat"

GFL_NAMESPACE_BEGIN(System)
GFL_NAMESPACE_BEGIN(PmlUse)

    //----------------------------------------------------------------------------
    /**
     *  @brief  ポケモン生成乱数取得
     *  @param  max_range 取得する最大値（0 <= n < max_range）
     *  @return 乱数
     */
    //-----------------------------------------------------------------------------
    u32 GetPokemonCreateRandRange( u32 max_range )
    {
      // 乱数取得
      return GflUse::GetPokemonCreateRand(max_range);
    }

    //----------------------------------------------------------------------------
    /**
     *  @brief  ポケモン生成乱数取得（0?U32_MAX)
     *
     *  @return 乱数
     */
    //-----------------------------------------------------------------------------
    u32 GetPokemonCreateRand( void )
    {
      // 乱数取得
      return GflUse::GetPokemonCreateRand();
    }

    static pml::Library*  SystemCtrl = NULL;
    static NijiEvolveManager* EvolveManager = NULL;



    //---------------------------------------------------------------------------------
    /// ポケモンライブラリの外部設定に必要なものでメモリを使うもの
    //---------------------------------------------------------------------------------
    class ExternalSettingParam
    {
    public:
      char* garcPath_WazaData;
      char* garcPath_PersonalData;
      char* garcPath_WazaOboeData;
      char* garcPath_GrowTable;
      char* garcPath_EvolutionTable;
      char* garcPath_MegaEvolveTable;
      char* garcPath_KowazaTable;
      char* garcPath_SeedMonsNoTable;
      char* garcPath_ItemData;
      char* garcPath_FieldPocket;    
      char* garcPath_BattlePocket;    
      char* garcPath_Message;

    public:
      ExternalSettingParam(void)
        : garcPath_WazaData(NULL),
          garcPath_PersonalData(NULL),
          garcPath_WazaOboeData(NULL),
          garcPath_GrowTable(NULL),
          garcPath_EvolutionTable(NULL),
          garcPath_MegaEvolveTable(NULL),
          garcPath_KowazaTable(NULL),
          garcPath_SeedMonsNoTable(NULL),
          garcPath_ItemData(NULL),
          garcPath_FieldPocket(NULL),
          garcPath_BattlePocket(NULL),
          garcPath_Message(NULL)
      {}
    };
    static ExternalSettingParam s_externalSettingParam;


    static const u32 LOCAL_HEAP_SIZE = 0x50000;         // ローカルヒープサイズ
    static gfl2::heap::HeapBase* s_pLocalHeap = NULL;   // Initializeで使用するローカルヒープ

    /**
     * @brief ローカルヒープを生成
     * @param heap  親ヒープ
     */
    void CreateLocalHeap( gfl2::heap::HeapBase* heap )
    {
      s_pLocalHeap = GFL_CREATE_LOCAL_HEAP( heap, LOCAL_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
    }

    /**
     * @brief ローカルヒープを削除
     */
    void DeleteLocalHeap( void )
    {
      GFL_DELETE_HEAP( s_pLocalHeap );
    }

    /**
     * @brief PML 初期化処理
     */
    void Initialize( void )
    {
      GFL_ASSERT_STOP( s_pLocalHeap != NULL );

      //TAMADA_PRINT("--pml::Initialize() START\n");
      //TAMADA_PRINT("--Free/Total=%08x/%08x\n", s_pLocalHeap->GetTotalFreeSize(), s_pLocalHeap->GetTotalSize());
      if( EvolveManager == NULL )
      {
        EvolveManager = GFL_NEW( s_pLocalHeap ) NijiEvolveManager();
      }
      //TAMADA_PRINT("--Create XYEvolveManager\n");
      //TAMADA_PRINT("--Free/Total=%08x/%08x\n", s_pLocalHeap->GetTotalFreeSize(), s_pLocalHeap->GetTotalSize());


      if( SystemCtrl == NULL )
      {
        gfl2::fs::AsyncFileManager *fileMng = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

        // ポケモンライブラリの外部設定に必要なものでメモリを使うもの
        {
          s_externalSettingParam.garcPath_WazaData        = fileMng->GetUsableArcFileName(gfl2::fs::AsyncFileManager::ArcFileNameTableType::MAIN, s_pLocalHeap, ARCID_PMLIB_WAZA_TBL    );
          s_externalSettingParam.garcPath_PersonalData    = fileMng->GetUsableArcFileName(gfl2::fs::AsyncFileManager::ArcFileNameTableType::MAIN, s_pLocalHeap, ARCID_PMLIB_PERSONAL    );
          s_externalSettingParam.garcPath_WazaOboeData    = fileMng->GetUsableArcFileName(gfl2::fs::AsyncFileManager::ArcFileNameTableType::MAIN, s_pLocalHeap, ARCID_PMLIB_WOTBL       );
          s_externalSettingParam.garcPath_GrowTable       = fileMng->GetUsableArcFileName(gfl2::fs::AsyncFileManager::ArcFileNameTableType::MAIN, s_pLocalHeap, ARCID_PMLIB_GROWTBL     );
          s_externalSettingParam.garcPath_EvolutionTable  = fileMng->GetUsableArcFileName(gfl2::fs::AsyncFileManager::ArcFileNameTableType::MAIN, s_pLocalHeap, ARCID_PMLIB_EVO         );
          s_externalSettingParam.garcPath_MegaEvolveTable = fileMng->GetUsableArcFileName(gfl2::fs::AsyncFileManager::ArcFileNameTableType::MAIN, s_pLocalHeap, ARCID_PMLIB_MEGA        );
          s_externalSettingParam.garcPath_KowazaTable     = fileMng->GetUsableArcFileName(gfl2::fs::AsyncFileManager::ArcFileNameTableType::MAIN, s_pLocalHeap, ARCID_PMLIB_WAZA_EGG    );
          s_externalSettingParam.garcPath_SeedMonsNoTable = fileMng->GetUsableArcFileName(gfl2::fs::AsyncFileManager::ArcFileNameTableType::MAIN, s_pLocalHeap, ARCID_PMLIB_PMS         );
          s_externalSettingParam.garcPath_ItemData        = fileMng->GetUsableArcFileName(gfl2::fs::AsyncFileManager::ArcFileNameTableType::MAIN, s_pLocalHeap, ARCID_ITEM_DATA         );
          s_externalSettingParam.garcPath_FieldPocket     = fileMng->GetUsableArcFileName(gfl2::fs::AsyncFileManager::ArcFileNameTableType::MAIN, s_pLocalHeap, ARCID_ITEM_FIELD_POCKET );
          s_externalSettingParam.garcPath_BattlePocket    = fileMng->GetUsableArcFileName(gfl2::fs::AsyncFileManager::ArcFileNameTableType::MAIN, s_pLocalHeap, ARCID_ITEM_BATTLE_POCKET);
          s_externalSettingParam.garcPath_Message         = fileMng->GetUsableArcFileName(gfl2::fs::AsyncFileManager::ArcFileNameTableType::MAIN, s_pLocalHeap, print::GetMessageArcId());
        }

        pml::Library::ExternalSetting param; 
        param.garcPath_WazaData         = s_externalSettingParam.garcPath_WazaData       ;
        param.garcPath_PersonalData     = s_externalSettingParam.garcPath_PersonalData   ;
        param.garcPath_WazaOboeData     = s_externalSettingParam.garcPath_WazaOboeData   ;
        param.garcPath_GrowTable        = s_externalSettingParam.garcPath_GrowTable      ;
        param.garcPath_EvolutionTable   = s_externalSettingParam.garcPath_EvolutionTable ;
        param.garcPath_MegaEvolveTable  = s_externalSettingParam.garcPath_MegaEvolveTable;
        param.garcPath_KowazaTable      = s_externalSettingParam.garcPath_KowazaTable    ;
        param.garcPath_SeedMonsNoTable  = s_externalSettingParam.garcPath_SeedMonsNoTable;
        param.garcPath_ItemData         = s_externalSettingParam.garcPath_ItemData       ;
        param.garcPath_FieldPocket      = s_externalSettingParam.garcPath_FieldPocket    ;
        param.garcPath_BattlePocket     = s_externalSettingParam.garcPath_BattlePocket   ;
        param.garcDataID_BattlePocket   = 0;
        param.garcPath_Message          = s_externalSettingParam.garcPath_Message;
        param.msgDataID_MonsName        = GARC_message_monsname_DAT;
//        param.msgDataID_TokuseiName     = GARC_message_tokusei_DAT;
//        param.msgDataID_TokuseiInfo     = GARC_message_tokuseiinfo_DAT;
//        param.msgDataID_WazaName        = GARC_message_wazaname_DAT;
 //       param.msgDataID_WazaInfo        = GARC_message_wazainfo_DAT;
 //       param.msgDataID_ItemName        = GARC_message_itemname_DAT;
 //       param.msgDataID_ItemInfo        = GARC_message_iteminfo_DAT;
        param.cacheSize_PersonalData    = 6;
        param.cacheSize_GrowTable       = 6;
        param.cacheSize_WazaOboeData    = 4;
        param.cacheSize_EvolutionTable  = 4;
        param.cacheSize_MegaEvolveTable = 4;
        param.cacheSize_WazaData        = 4;
        param.bpWazaOshieTable          = sBpWazaOshieTable;
        param.evolve_manager            = EvolveManager; 
        param.countryCode               = 0;
        param.countryRegionCode         = 0;
        param.langId                    = System::GetLang();
        param.hwRegionCode              = 0;
        param.randFunc                  = GetPokemonCreateRand;
        param.limitRandFunc             = GetPokemonCreateRandRange;
        param.cassetteVersion           = System::GetVersion();
        param.personalRegionID          = pml::personal::REGION_ALOLA;

#if defined(GF_PLATFORM_CTR)
        {
          nn::cfg::SimpleAddressId simple_id;
          nn::cfg::GetSimpleAddressId( &simple_id );
          param.countryCode       = simple_id.GetCountryCode();
          param.countryRegionCode = simple_id.GetRegionCode();
          param.hwRegionCode      = nn::cfg::GetRegion();
        }
#endif

        SystemCtrl = GFL_NEW( s_pLocalHeap ) pml::Library( s_pLocalHeap, gfl2::fs::System::GetAsyncFileManagerForRomDevice() , param );
        //TAMADA_PRINT("GARC_Path=%d\n", print::GetMessageArcId() );
      }
      //TAMADA_PRINT("--Create pml::Library\n");
      //TAMADA_PRINT("--Free/Total=%08x/%08x\n", s_pLocalHeap->GetTotalFreeSize(), s_pLocalHeap->GetTotalSize());
      //TAMADA_PRINT("pml::Initialize() END\n");

      item::ITEM_DATA::CreateExpandData( s_pLocalHeap );

#if defined(DEBUG_ONLY_FOR_ariizumi_nobuhiko)
      {
        //ポケパラテスト
        pml::pokepara::PokemonParam *pp;
        pp = GFL_NEW(s_pLocalHeap)pml::pokepara::PokemonParam(s_pLocalHeap,MONSNO_PIKATYUU,50,pml::pokepara::INIT_SPEC_ID_RANDOM);
        GFL_PRINT( "MonsNo[%d] PokeparaTest",pp);
        GFL_PRINT( "LV [%d]\n",pp->GetLevel());
        GFL_PRINT( "HP [%d]\n",pp->GetPower(pml::pokepara::POWER_HP));
        GFL_PRINT( "ATK[%d]\n",pp->GetPower(pml::pokepara::POWER_ATK));
        GFL_PRINT( "DEF[%d]\n",pp->GetPower(pml::pokepara::POWER_DEF));
        GFL_PRINT( "SPD[%d]\n",pp->GetPower(pml::pokepara::POWER_AGI));
        GFL_PRINT( "SAT[%d]\n",pp->GetPower(pml::pokepara::POWER_SPATK));
        GFL_PRINT( "SDE[%d]\n",pp->GetPower(pml::pokepara::POWER_SPDEF));
        GFL_SAFE_DELETE(pp);
      }
#endif
    }


    /**
     * PML 破棄処理
     */
    void Finalize( void )
    {
      item::ITEM_DATA::DeleteExpandData();

      if( SystemCtrl )
      {
        GFL_DELETE( SystemCtrl );
        SystemCtrl = NULL;
      }

      // ポケモンライブラリの外部設定に必要なものでメモリを使うもの
      {
        GflHeapSafeFreeMemory(s_externalSettingParam.garcPath_WazaData       );
        GflHeapSafeFreeMemory(s_externalSettingParam.garcPath_PersonalData   );
        GflHeapSafeFreeMemory(s_externalSettingParam.garcPath_WazaOboeData   );
        GflHeapSafeFreeMemory(s_externalSettingParam.garcPath_GrowTable      );
        GflHeapSafeFreeMemory(s_externalSettingParam.garcPath_EvolutionTable );
        GflHeapSafeFreeMemory(s_externalSettingParam.garcPath_MegaEvolveTable);
        GflHeapSafeFreeMemory(s_externalSettingParam.garcPath_KowazaTable    );
        GflHeapSafeFreeMemory(s_externalSettingParam.garcPath_SeedMonsNoTable);
        GflHeapSafeFreeMemory(s_externalSettingParam.garcPath_ItemData       );
        GflHeapSafeFreeMemory(s_externalSettingParam.garcPath_FieldPocket    );
        GflHeapSafeFreeMemory(s_externalSettingParam.garcPath_BattlePocket   );
        GflHeapSafeFreeMemory(s_externalSettingParam.garcPath_Message        );
      }

      if( EvolveManager )
      {
        GFL_DELETE( EvolveManager );
        EvolveManager = NULL;
      }

    }


    //----------------------------------------------------------------------------
    /**
     * @brief 進化マネージャを取得する
     */
    //----------------------------------------------------------------------------
    const NijiEvolveManager* GetEvolveManager( void )
    {
      return EvolveManager;
    }



GFL_NAMESPACE_END(PmlUse)
GFL_NAMESPACE_END(System)
