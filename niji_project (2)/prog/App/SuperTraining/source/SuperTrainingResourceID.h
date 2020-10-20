// ============================================================================
/*
 * @file SuperTrainingResourceID.h
 * @brief すごい特訓アプリで使用するリソースのIDです。
 * @date 2015.12.04
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( SUPER_TRAINIG_RESOURCEID_H_INCLUDE )
#define SUPER_TRAINIG_RESOURCEID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(SuperTraining)


  enum SuperTrainingResourceID
  {
    SUPER_TRAINING_RESOURCE_ID_LAYOUT,
    SUPER_TRAINING_RESOURCE_ID_UPPLER_STATUS_LAYOUT,
    SUPER_TRAINING_RESOURCE_ID_MESSAGE,
    SUPER_TRAINING_RESOURCE_ID_UPPLER_STATUS_MESSAGE,
    SUPER_TRAINING_RESOURCE_ID_ITEM_TEXT,
    SUPER_TRAINIMG_RESOURCE_MAX,
  };

GFL_NAMESPACE_END(SuperTraining)
GFL_NAMESPACE_END(App)

#endif // SUPER_TRAINIGRESOURCEID_H_INCLUDE
