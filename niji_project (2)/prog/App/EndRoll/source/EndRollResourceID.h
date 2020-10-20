// ============================================================================
/*
 * @file EndRollResourceID.h
 * @brief エンドロールアプリで使用するリソースのIDです。
 * @date 2016.03.16
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( APPENDROLL_RESOURCEID_H_INCLUDE )
#define APPENDROLL_RESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(EndRoll)


enum EndRollResourceID
{
  APPENDROLL_RESOURCE_ID_LAYOUT,
  APPENDROLL_RESOURCE_ID_GRPFONT_LAYOUT,
  APPENDROLL_RESOURCE_MAX,
};

GFL_NAMESPACE_END(EndRoll)
GFL_NAMESPACE_END(App)

#endif // APPENDROLLRESOURCEID_H_INCLUDE
