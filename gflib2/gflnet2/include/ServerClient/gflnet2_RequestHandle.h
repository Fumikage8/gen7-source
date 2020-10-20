
//=============================================================================
/**
 * @file   gflnet2_RequestHandle.h
 * @brief  リクエストハンドル
 * @author shin kosaka
 */
//=============================================================================
#ifndef __GFLNET2_REQUEST_HANDLE_H__
#define __GFLNET2_REQUEST_HANDLE_H__
#pragma once



#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(ServerClient)


typedef u64 RequestHandle;
enum
{
  INVALID_HANDLE = 0,//無効なハンドル
};

GFL_NAMESPACE_END(ServerClient)
GFL_NAMESPACE_END(gflnet2)

#endif // __GFLNET2_REQUEST_HANDLE_H__


