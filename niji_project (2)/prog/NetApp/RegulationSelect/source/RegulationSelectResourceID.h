// ============================================================================
/*
 * @file RegulationSelectResourceID.h
 * @brief レギュレーション選択アプリで使用するリソースのIDです。
  * @date 2015.06.12
 * @author endo_akira
*/
// ============================================================================
#if !defined( REGULATIONSELECT_RESOURCEID_H_INCLUDE )
#define REGULATIONSELECT_RESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)


enum RegulationSelectResourceID
{
  REGULATIONSELECT_RESOURCE_ID_LAYOUT,
  REGULATIONSELECT_RESOURCE_ID_MESSAGE_REGULATION,
  REGULATIONSELECT_RESOURCE_MAX,
};

GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONSELECTRESOURCEID_H_INCLUDE
