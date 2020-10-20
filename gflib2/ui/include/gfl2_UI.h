#ifndef __gfl2_UI_H__
#define __gfl2_UI_H__
/**
 * @file gfl2_UI.h
 * @brief UIシステム
 * @author obata_toshihiro
 * @date 2010.10.18
 */
#pragma once

#ifdef __cplusplus 
#include <ui/include/gfl2_UI_Types.h>
#include <ui/include/gfl2_UI_System.h>
#include <ui/include/gfl2_UI_DeviceManager.h>
#include <ui/include/gfl2_UI_Button.h>
#include <ui/include/gfl2_UI_VectorDevice.h>
#include <ui/include/gfl2_UI_TouchPanel.h>
#include <ui/include/gfl2_UI_Accelerometer.h>
#include <ui/include/gfl2_UI_Gyroscope.h>
#if defined(GF_PLATFORM_CTR)
#include "ui/include/ctr/gfl2_UI_CTR_DeviceManager.h"
#elif defined(GF_PLATFORM_WIN32)
#include "ui/include/win32/gfl2_UI_WIN32_DeviceManager.h"
#endif




#endif // __cplusplus
#endif // __gfl2_UI_H__
