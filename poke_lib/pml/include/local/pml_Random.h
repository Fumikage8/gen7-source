//=============================================================================
/**
 * @brief ポケモンライブラリ内部コード( 乱数 )
 * @file pml_Random.h
 * @author obata_toshihiro
 * @date 2012.03.18
 */
//=============================================================================
#ifndef __PML_RANDOM_H__
#define __PML_RANDOM_H__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_math_random.h>

namespace pml {
  namespace local {


    class Random
    {
      public:
      static void Initialize( gfl2::heap::HeapBase* heap ); 
      static void Finalize( void ); 
      static u32 GetValue( void );
      static u32 GetValue( u32 max );

      private:
      static gfl2::math::Random* m_RandObj;
    };


  }
}

#endif
