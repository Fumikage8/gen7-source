// ============================================================================
/*
 * @file CecTestStateAccessor.cpp
 * @brief すれ違い通信の状態の設定と取得をするクラスです。
 * @date 2015.03.03
 */
// ============================================================================
#if PM_DEBUG

#include "Test/CecTest/include/CecTestStateAccessor.h"

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(CecTest)


CecTestStateAccessor::CecTestStateAccessor() :
  m_eState( CecTestStateNone )
{
}


CecTestStateAccessor::~CecTestStateAccessor()
{
}


CecTestState CecTestStateAccessor::GetState() const
{
  return m_eState;
}


void CecTestStateAccessor::SetState( CecTestState eState )
{
  m_eState = eState;
}


GFL_NAMESPACE_END(CecTest)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
