// ============================================================================
/*
 * @file EndRollWork.cpp
 * @brief エンドロールアプリのワーククラスです。
 * @date 2016.03.16
 * @author uchida_yuto
 */
// ============================================================================
#include "App/EndRoll/source/System/EndRollWork.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(EndRoll)
GFL_NAMESPACE_BEGIN(System)


EndRollWork::EndRollWork() :
  ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE )
{
  _Clear();
}


EndRollWork::~EndRollWork()
{
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(EndRoll)
GFL_NAMESPACE_END(App)
