//=============================================================================
/**
 * @brief ポケモンライブラリ内部コード( 乱数 )
 * @file pml_Ranodm.cpp
 * @author obata_toshihiro
 * @date 2012.03.18
 */
//=============================================================================
#include <pml/include/local/pml_Random.h>

#include <pml/include/system/pml_Library.h>

namespace pml {
  namespace local {


    gfl2::math::Random* Random::m_RandObj = NULL;

    void Random::Initialize( gfl2::heap::HeapBase* heap )
    {
      if( m_RandObj == NULL )
      {
        m_RandObj = GFL_NEW( heap ) gfl2::math::Random();
        m_RandObj->Initialize();
      }
    }

    void Random::Finalize( void )
    {
      GFL_SAFE_DELETE( m_RandObj );
    }

    u32 Random::GetValue( void )
    {
      if ( Library::m_setting->randFunc )
      {
        return Library::m_setting->randFunc();
      }
      return m_RandObj->Next();
    }

    u32 Random::GetValue( u32 max )
    {
      if ( Library::m_setting->limitRandFunc )
      {
        return Library::m_setting->limitRandFunc( max );
      }
      return m_RandObj->Next( max );
    }


  } //namespace local
} //namespace pml
