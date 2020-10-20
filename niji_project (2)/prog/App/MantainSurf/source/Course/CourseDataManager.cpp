//======================================================================
/**
 * @file CourseDataManager.cpp
 * @date 2016/11/10 17:34:40
 * @author fang_yicheng
 * @brief マンタインサーフのコースデータマネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "CourseDataManager.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Course)

CourseDataManager::CourseDataManager() :
  m_pHeap(NULL),
  m_pResourceManager(NULL),
  m_curCourseIndex(0)
{
}

CourseDataManager::~CourseDataManager()
{
}

void CourseDataManager::Initialize(gfl2::heap::HeapBase* pHeap, Resource::ResourceManager* pResourceManager)
{
  m_pHeap = pHeap;
  m_pResourceManager = pResourceManager;
}

void CourseDataManager::ChangeCourse(u32 courseIndex)
{
  HOU_PRINT("[CourseData]ChangeCourse [%d]\n", courseIndex);
  GFL_ASSERT(courseIndex < COURSE_DATA_MAX);
  m_curCourseIndex = courseIndex;
  void* pHeader = m_pResourceManager->GetResourceData(HEADER_INDEX);
  void* pData = m_pResourceManager->GetResourceData(m_curCourseIndex + COURSE_DATA_START_INDEX);
  void* pBlockDataList[BLOCK_DATA_MAX];
  for(u32 i = 0; i < BLOCK_DATA_MAX; i++)
  {
    pBlockDataList[i] = m_pResourceManager->GetResourceData(BLOCK_DATA_START_INDEX + i);
  }
  m_CourseData.Setup(m_pHeap, pHeader, pData, pBlockDataList);
}

CourseData* CourseDataManager::GetCourseData()
{
  return &m_CourseData;
}

GFL_NAMESPACE_END(Course)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
