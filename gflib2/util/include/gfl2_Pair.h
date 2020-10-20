// ============================================================================
/*
 * @file gfl2_Pair.h
 * @brief key と value を持つペア構造体です。
 * @date 2015.02.24
 */
// ============================================================================
#if !defined( GFL2_PAIR_H_INCLUDED )
#define GFL2_PAIR_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(util)

template<class Key, class Value>
struct Pair
{
  Key     first;
  Value   second;

  Pair() :
    first(),
    second()
  {
  }

  Pair( const Key& key, const Value& value ) :
    first( key ),
    second( value )
  {
  }
};


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_PAIR_H_INCLUDED
