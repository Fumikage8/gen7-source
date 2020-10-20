//==============================================================================
/**
 @file    ZukanPrismParam.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2016.03.06
 @brief   図鑑のキラキラのパラメータ
 */
//==============================================================================


#ifdef GF_PLATFORM_WIN32  // windows.hのmin/max対策
#define NOMINMAX
#endif




// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <debug/include/gfl2_DebugPrint.h>

// 図鑑のインクルード
#include "ZukanDefine.h"
#include "ZukanPrismParam.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


#if NIJI_APP_ZUKAN_PRISM_DEVELOP

const f32 ZukanPrismDevelopParam::BAIRITSU = 100.0f;

ZukanPrismDevelopParam::TargetParam ZukanPrismDevelopParam::s_targetParam[TARGET_PLACE_NUM][TARGET_COLOR_NUM] =
{
  // GRID_CENTER
  {
    {  MODE_GYRO, 0, GYRO_ANGLE_AVE, FUNC_TYPE_COS, { -100, 20, 3000, 6000, 8000, 8000 } },
    {  MODE_GYRO, 0, GYRO_ANGLE_AVE, FUNC_TYPE_COS, { -100, 60, 3000, 7900, 8000, 8000 } },
    {  MODE_GYRO, 0, GYRO_ANGLE_AVE, FUNC_TYPE_COS, { -200, 80, 3000, 6000, 8000, 8000 } },
  },
  // GRID_CORNER
  {
    {  MODE_GYRO, 0, GYRO_ANGLE_AVE, FUNC_TYPE_COS, {    0,  0, 5000, 10000, 2500, 2000 } },
    {  MODE_GYRO, 0, GYRO_ANGLE_AVE, FUNC_TYPE_COS, { -160, 50, 5000, 24000, 1500, 1000 } },
    {  MODE_GYRO, 0, GYRO_ANGLE_AVE, FUNC_TYPE_COS, { -160, 70, 5000, 10000, 3000, 1000 } },
  },

  // BG
  {
    {  MODE_GYRO, 0, GYRO_ANGLE_X, FUNC_TYPE_COS, { -100, 10, 1000, 10000, 0, 0 } },
    {  MODE_GYRO, 0, GYRO_ANGLE_Y, FUNC_TYPE_COS, { -200, 30, 1200, 12000, 0, 0 } },
    { MODE_CONST, 0, GYRO_ANGLE_Z, FUNC_TYPE_COS, { -150, 30,  700,  2000, 0, 0 } },
  },                                                                              
  {                                                                               
    { MODE_CONST, 0, GYRO_ANGLE_AVE, FUNC_TYPE_COS, { -500, 40,  400,    0, 0, 0 } },
    { MODE_CONST, 0, GYRO_ANGLE_X  , FUNC_TYPE_COS, { -200, 70, 1000,    0, 0, 0 } },
    {  MODE_GYRO, 0, GYRO_ANGLE_Y  , FUNC_TYPE_COS, { -300, 70, 1000, 2000, 0, 0 } },
  },                                                                              
   {                                                                              
    {  MODE_GYRO, 0, GYRO_ANGLE_Z, FUNC_TYPE_COS, { -200, 30, 1000,     0, 0, 0 } },
    {  MODE_GYRO, 0, GYRO_ANGLE_Y, FUNC_TYPE_COS, { -500, 80,  800, 18000, 0, 0 } },
    { MODE_CONST, 0, GYRO_ANGLE_X, FUNC_TYPE_COS, { -300, 80, 1300,  7000, 0, 0 } },
  },                                                                              
  {                                                                               
    { MODE_CONST, 0, GYRO_ANGLE_Z, FUNC_TYPE_COS, { -100, 30, 1000, 1000, 0, 0 } },
    {  MODE_GYRO, 0, GYRO_ANGLE_Z, FUNC_TYPE_COS, { -400, 80,  500, 3000, 0, 0 } },
    { MODE_CONST, 0, GYRO_ANGLE_X, FUNC_TYPE_COS, { -300, 80,  700, 5000, 0, 0 } },
  },
};

ZukanPrismDevelopParam::ReduceParam ZukanPrismDevelopParam::s_reduceParam =
{
    1000,
     300,
    3000,
    7000,
    8000,
    5000,
};

#if PM_DEBUG
void ZukanPrismDevelopParam::Print(void)
{
  GFL_PRINT("ZukanPrismDevelopParam ================\n");
  for(s32 targetPlace=0; targetPlace<TARGET_PLACE_NUM; ++targetPlace)
  {
    for(s32 targetColor=0; targetColor<TARGET_COLOR_NUM; ++targetColor)
    {
      GFL_PRINT("targetPlace=%d, targetColor=%d ########\n", targetPlace, targetColor);
      
      TargetParam* p = &(s_targetParam[targetPlace][targetColor]);
      GFL_PRINT("mode=%d\n", p->mode);
      GFL_PRINT("constColor=%d\n", p->constColor);
      GFL_PRINT("gyroAngle=%d, funcType=%d\n", p->gyroAngle, p->funcType);
      GFL_PRINT("funcParam=(");
      for(s32 funcParam=0; funcParam<FUNC_PARAM_NUM; ++funcParam)
      {
        GFL_PRINT("%d, ", p->funcParam[funcParam]);
      }
      GFL_PRINT(")\n");
    }
  }
}

void ZukanPrismDevelopParam::Reset(bool allReset)
{
  if(allReset)
  {
    s_currTargetPlace = 0;
    for(s32 targetPlace=0; targetPlace<TARGET_PLACE_NUM; ++targetPlace)
    {
      s_currTargetColor[targetPlace] = 0;
    }
  
    for(s32 targetPlace=0; targetPlace<TARGET_PLACE_NUM; ++targetPlace)
    {
      for(s32 targetColor=0; targetColor<TARGET_COLOR_NUM; ++targetColor)
      {
        resetTargetParam(&(s_targetParam[targetPlace][targetColor]));
      }
    }
  }
  else
  {
    resetTargetParam(&(s_targetParam[s_currTargetPlace][s_currTargetColor[s_currTargetPlace]]));
  }
}

void ZukanPrismDevelopParam::resetTargetParam(TargetParam* targetParam)
{
  targetParam->mode        = MODE_CONST;
  targetParam->constColor  = 0;
  targetParam->gyroAngle   = GYRO_ANGLE_AVE;
  targetParam->funcType    = FUNC_TYPE_COS;
  targetParam->funcParam[FUNC_PARAM_SHINPUKU_MIN] = 0;
  targetParam->funcParam[FUNC_PARAM_SHINPUKU_MAX] = 0;
  targetParam->funcParam[FUNC_PARAM_SYUUKI      ] = 0;
  targetParam->funcParam[FUNC_PARAM_ISOU        ] = 0;
  targetParam->funcParam[FUNC_PARAM_ISOU_X      ] = 0;
  targetParam->funcParam[FUNC_PARAM_ISOU_Y      ] = 0;
}

s32 ZukanPrismDevelopParam::s_currTargetPlace = ZukanPrismDevelopParam::TARGET_PLACE_GRID_CENTER;
s32 ZukanPrismDevelopParam::s_currTargetColor[TARGET_PLACE_NUM] =
{
  TARGET_COLOR_R,
  TARGET_COLOR_R,
  TARGET_COLOR_R,
  TARGET_COLOR_R,
  TARGET_COLOR_R,
  TARGET_COLOR_R,
};

s32 ZukanPrismDevelopParam::s_gyroAnglePrintFlag = 0;
#endif  // PM_DEBUG

#endif  // NIJI_APP_ZUKAN_PRISM_DEVELOP


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

