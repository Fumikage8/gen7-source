//======================================================================
/**
* @file FieldCameraTypes.h
* @date 2015/07/03 18:52:30
* @author ikuta_junya
* @brief フィールドカメラ関連の定義
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if !defined __FIELDCAMERATYPES_H_INCLUDED__
#define __FIELDCAMERATYPES_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );

struct SupportAction
{
  enum Value
  {
    STOP = 0,   // 一時停止
    RESUME,     // 再開
    
    FOCUS_STOP,   // カメラフォーカスによる一時停止
    FOCUS_RESUME, // カメラフォーカスによる一時停止、再開
    
    MAX
  };
};

// GameDataが持つ型、本当のSaveデータはMiscが持っている
struct SaveData
{
  u32 areaId;
  u32 areaDataNo;
  u32 oldAreaDataNo;

  // gamedataのみが保持する
  u32 focusStopBit;
};

GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );


#endif // __FIELDCAMERATYPES_H_INCLUDED__