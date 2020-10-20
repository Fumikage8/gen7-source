//======================================================================
/**
 * @file btl_DamageProcParamStack.cpp
 * @date 2016/06/22 10:45:59
 * @author taya
 * @brief ServerFlow の scproc_Fight_damage_side_core 計算で使う各種パラメータをスタックしておくための機構
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include "./btl_DamageProcParamStack.h"

GFL_NAMESPACE_BEGIN(btl)


// ctor / dtor
DamageProcParamStack::DamageProcParamStack()
: m_stackPointer(0)
{
  gfl2::std::MemClear( m_params, sizeof(m_params) );
}
DamageProcParamStack::~DamageProcParamStack()
{

}


/**
 * @brief スタック１件Pushしてパラメータ構造体へのポインタを取得
 * @return パラメータ構造体へのポインタ
 */
DamageProcParamStack::Params* DamageProcParamStack::Push( void )
{
  if( m_stackPointer >= GFL_NELEMS(m_params) ){
    GFL_ASSERT(0);
    return &(m_params[ GFL_NELEMS(m_params)-1 ]);
  }

  Params*  p = &m_params[ m_stackPointer ];
  if( m_stackPointer < GFL_NELEMS(m_params) ){
    ++m_stackPointer;
  }

  return p;
}


/**
 * @brief スタック１件Popする
 */
void DamageProcParamStack::Pop( void )
{
  if( m_stackPointer <= 0 ){
    GFL_ASSERT(0);
    return;
  }
  --m_stackPointer;
}




GFL_NAMESPACE_END(btl)
