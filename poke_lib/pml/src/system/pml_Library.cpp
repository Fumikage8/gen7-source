//=============================================================================
/**
 * @brief  ポケモンライブラリ システム
 * @file   pml_Library.cpp
 * @author obata_toshhiro
 * @date   2011.01.12
 */
//=============================================================================
#include <heap/include/gfl2_Heap.h>

#include "pml/include/pml_Type.h"
#include "pml/include/pml_Local.h"
#include "pml/include/pml_PokePara.h"
#include "pml/include/system/pml_Library.h"
#include "../pokepara/pml_PokemonParamAccessor.h"


namespace pml { 


  const Library::ExternalSetting* Library::m_setting = NULL;
  gfl2::fs::AsyncFileManager* Library::m_pmlFileManager = NULL;

  //---------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   * @param heap     システム確保に使用するヒープ
   * @param setting  ライブラリの設定
   */
  //---------------------------------------------------------------------------
  Library::Library( gfl2::heap::HeapBase* heap, gfl2::fs::AsyncFileManager *fileMng,const ExternalSetting& setting )
  {
    m_setting = static_cast<ExternalSetting*>(GflHeapAllocMemory( heap, sizeof(ExternalSetting) ));
   
    *( const_cast<ExternalSetting*>(m_setting) ) = setting;

    m_pmlFileManager = fileMng;

    pml::local::Random::Initialize( heap );
    pml::pokepara::Accessor::Initialize();
    pml::pokepara::CoreParam::CheckPublicDataSize();
    pml::pokepara::PokemonParam::CheckPublicDataSize();
    pml::pokepara::PokemonParam::SetEvolveManager( setting.evolve_manager );
    pml::pokepara::EggGenerator::Initialize( heap );
    pml::personal::Initialize( heap ); 
    pml::wazadata::Initialize( heap );
  } 

  //---------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //---------------------------------------------------------------------------
  Library::~Library()
  {
    pml::pokepara::EggGenerator::Finalize();
    pml::wazadata::Finalize();
    pml::personal::Finalize();
    pml::local::Random::Finalize();
    GflHeapFreeMemory( const_cast<ExternalSetting*>(m_setting) );
    
  }



} // namespace pml
