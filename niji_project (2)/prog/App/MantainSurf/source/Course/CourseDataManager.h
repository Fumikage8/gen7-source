//======================================================================
/**
 * @file CourseDataManager.h
 * @date 2016/11/10 17:34:00
 * @author fang_yicheng
 * @brief マンタインサーフのコースデータマネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __COURSE_DATA_MANAGER_H_INCLUDED__
#define __COURSE_DATA_MANAGER_H_INCLUDED__
#pragma once

#include "CourseData.h"
#include "../Resource/ResourceManager.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Course)

class CourseDataManager
{
  GFL_FORBID_COPY_AND_ASSIGN(CourseDataManager);

public:

  // 定数の定義
  enum
  {
    HEADER_INDEX = GARC_mantain_course_data_mantain_course_data_00_header_BIN,
    COURSE_DATA_START_INDEX = GARC_mantain_course_data_mantain_course_data_01_course_000_BIN,
    COURSE_DATA_MAX = GARC_mantain_course_data_mantain_course_data_02_block_00_BIN - COURSE_DATA_START_INDEX,
    BLOCK_DATA_START_INDEX = GARC_mantain_course_data_mantain_course_data_02_block_00_BIN,
    BLOCK_DATA_MAX = GARC_mantain_course_data_mantain_course_data_02_block_02_BIN - BLOCK_DATA_START_INDEX + 1,
    TUTORIAL_COURSE_ID = COURSE_DATA_MAX - 1,
  };

public:

  CourseDataManager();
  virtual ~CourseDataManager();

  void Initialize(gfl2::heap::HeapBase* pHeap, Resource::ResourceManager* pResourceManager);
  void ChangeCourse(u32 courseIndex);

  CourseData* GetCourseData();

private:

  gfl2::heap::HeapBase*   m_pHeap;

  Resource::ResourceManager* m_pResourceManager;
  CourseData m_CourseData;

  u32 m_curCourseIndex;

};

GFL_NAMESPACE_END(Course)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __COURSE_DATA_MANAGER_H_INCLUDED__
