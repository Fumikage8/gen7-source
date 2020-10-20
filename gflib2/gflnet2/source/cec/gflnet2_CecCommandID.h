// ============================================================================
/*
 * @file gflnet2_CecCommandID.h
 * @brief すれ違い通信のコマンドID
 * @date 2015.02.16
 */
// ============================================================================
#if !defined( GFLNET2_CECCOMMANDID_H_INCLUDE )
#define GFLNET2_CECCOMMANDID_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)


enum CommandID
{
  NONE,
  IS_CREATE_MESSAGE_BOX,
  CREATE_MESSAGE_BOX,
  DELETE_MESSAGE_BOX,
  DELETE_ALL_MESSAGE,
  WRITE_MESSAGE,
  READ_MESSAGE,
  GET_MESSAGE_COUNT
};


GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif // GFLNET2_CECCOMMANDID_H_INCLUDE
