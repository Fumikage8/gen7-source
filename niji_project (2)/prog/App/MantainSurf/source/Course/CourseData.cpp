//======================================================================
/**
 * @file CourseData.cpp
 * @date 2016/11/10 17:34:31
 * @author fang_yicheng
 * @brief マンタインサーフのコースデータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "CourseData.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Course)

const u32 COURSE_END_TIME = 5 * 30;   // コース終了時間
const u32 MAX_GIMMICK_NUM_RATIO[CourseData::BLOCK_DATA_TYPE_MAX] = { 1, 1, 4 }; // 最大障害物数の倍率
const u32 MAX_FIXED_GIMMICK_NUM_RATIO = 5;    // 最大固定障害物数の倍率
const u32 MAX_U32_WORK_ARRAY_SIZE     = 64;   // u32ワーク配列の最大サイズ

// 抽選パターンのサイズ
const u32 CourseData::BLOCK_PATTERN_MAX_SIZE[BLOCK_DATA_TYPE_MAX] = { 
  MAX_BLOCK_PATTERN_NUM_UPWARD, 
  MAX_BLOCK_PATTERN_NUM_DOWNWARD, 
  MAX_BLOCK_PATTERN_NUM_RANDOM,
};

CourseData::CourseData() :
  m_pHeap(NULL),
  m_pHeader(NULL),
  m_pBody(NULL),
  m_pDataTable(NULL),
  m_keyFrameCount(0),
  m_totalFrame(0),
  m_totalTime(0.0f),
  m_randomSelectCount(0)
{
  for(u32 i = 0; i < BLOCK_DATA_TYPE_MAX; i++)
  {
    m_pBlockDataList[i] = NULL;
  }
}

CourseData::~CourseData()
{
  GFL_SAFE_DELETE(m_pDataTable);
}

void CourseData::Setup(gfl2::heap::HeapBase* pHeap, void* pHeader, void* pData, void** pBlockDataList)
{
  m_pHeap = pHeap;
  SetupHeader(pHeader);
  SetupCourseData(pData);
  SetupBlockData(pBlockDataList);
}

void CourseData::SetupHeader(void* pHeader)
{
  m_pHeader = static_cast<DataHeader*>(pHeader);
  m_pHeader->m_blockLengthX *= 100;  // cmに換算
  m_pHeader->m_blockLengthZ *= 100;  // cmに換算
  m_pHeader->m_courseEndTime = COURSE_END_TIME;
}

void CourseData::SetupCourseData(void* pData)
{
  m_pBody = static_cast<DataBody*>(pData);
  // データテーブルを生成する
  GFL_SAFE_DELETE(m_pDataTable);
  m_keyFrameCount = m_pBody->m_waveCount * 4 + 2; // キーフレーム数
  m_totalFrame = 0;
  m_pDataTable = GFL_NEW_ARRAY(m_pHeap) u32[m_keyFrameCount * 2];
  m_pDataTable[0] = 0;
  m_pDataTable[m_keyFrameCount] = 0;
  for(u32 i = 0; i < m_keyFrameCount - 2; i += 4)
  {
    // 高さ0%→0%を維持する区間
    m_totalFrame += MakeFrame(m_pBody->m_waveInterval, m_pBody->m_waveIntervalRandom);
    m_pDataTable[i + 1] = m_totalFrame;
    m_pDataTable[i + 1 + m_keyFrameCount] = 0;
    // 高さ0%→100%を変化する区間
    m_totalFrame += MakeFrame(m_pBody->m_waveChangeTime, m_pBody->m_waveChangeTimeRandom);
    m_pDataTable[i + 2] = m_totalFrame;
    m_pDataTable[i + 2 + m_keyFrameCount] = 1;
    // 高さ100%を維持する区間
    m_totalFrame += MakeFrame(m_pBody->m_waveKeepTime, m_pBody->m_waveKeepTimeRandom);
    m_pDataTable[i + 3] = m_totalFrame;
    m_pDataTable[i + 3 + m_keyFrameCount] = 1;
    // 高さ100%→0%を変化する区間
    m_totalFrame += MakeFrame(m_pBody->m_waveChangeTime, m_pBody->m_waveChangeTimeRandom);
    m_pDataTable[i + 4] = m_totalFrame;
    m_pDataTable[i + 4 + m_keyFrameCount] = 0;
  }
  // 最後にコース終了区間を追加
  m_totalFrame += m_pHeader->m_courseEndTime;
  m_pDataTable[m_keyFrameCount - 1] = m_totalFrame;
  m_pDataTable[m_keyFrameCount * 2 - 1] = 0;
  m_totalTime = static_cast<f32>(m_totalFrame) / static_cast<f32>(m_pHeader->m_frameRate);

#if PM_DEBUG
  HOU_PRINT("[CourseData][");
  for(u32 i = 0; i < m_keyFrameCount * 2; i++)
  {
    HOU_PRINT("%d, ", m_pDataTable[i]);
  }
  HOU_PRINT("]\n");
#endif
}

u32 CourseData::GetWaveEndKeyFrameIndex()
{
  return m_keyFrameCount - 2;
}

u32 CourseData::GetLastWaveKeyFrameIndex()
{
  return (m_pBody->m_waveCount > 0 ? m_keyFrameCount - 3 : 0);
}

void CourseData::SetupBlockData(void** pBlockDataList)
{
  for(u32 i = 0; i < BLOCK_DATA_TYPE_MAX; i++)
  {
    m_pBlockDataList[i] = static_cast<BlockDataIn*>(pBlockDataList[i]);
    MakePatternIdTable(static_cast<BlockDataType>(i));
    CalcMaxGimmickNum(static_cast<BlockDataType>(i));
  }
  CalcMaxFixedGimmickNum();
  MakeRandomHitTable();  

#if PM_DEBUG
  for(u32 i = 0; i < BLOCK_DATA_TYPE_MAX; i++)
  {
    PrintBlockData(i);
  }
  // 障害物数
  HOU_PRINT("[GimmickNum%d]", BLOCK_DATA_TYPE_MAX);
  for(u32 i = 0; i < GMKID_MAX; i++)
  {
    HOU_PRINT("%d, ", m_MaxFixedGimmickNum[i]);
  }
  HOU_PRINT("\r\n");
#endif
}

void CourseData::MakePatternIdTable(BlockDataType type)
{
  // パターンIDテーブル
  const u32 *PATTERN_TABLE[BLOCK_DATA_TYPE_MAX] = {
    m_pBody->m_blockPatternUpward,
    m_pBody->m_blockPatternDownward,
  };

  switch(type)
  {
  case BLOCK_DATA_TYPE_UPWARD:
  case BLOCK_DATA_TYPE_DOWNWARD:
    {
      u32 workArray[MAX_U32_WORK_ARRAY_SIZE];
      u32 valueCount = 0;
      m_PatternIdTable[type].m_tableSize = m_pBody->m_waveCount;
      if(m_PatternIdTable[type].m_tableSize > 0)
      {
        m_PatternIdTable[type].m_tablePatternId = GFL_NEW_ARRAY(m_pHeap) u32[m_PatternIdTable[type].m_tableSize];
      }
      for(u32 waveIndex = 0; waveIndex < m_pBody->m_waveCount; waveIndex++)
      {
        // ワーク配列を更新
        u32 workArraySize = 0;
        u32 size = valueCount;
        valueCount = 0;
        for(u32 i = 0; i < size; i++)
        {
          if(workArray[i] > 0)
          {
            workArray[workArraySize] = workArray[i];
            workArraySize++;
            valueCount++;
          }
        }
        if(valueCount == 0)
        {
          // 有効な値がないため、ワーク配列を初期化
          for(u32 i = 0; i < BLOCK_PATTERN_MAX_SIZE[type]; i++)
          {
            if(PATTERN_TABLE[type][i] > 0)
            {
              workArray[workArraySize] = PATTERN_TABLE[type][i];
              workArraySize++;
              valueCount++;
            }
          }
        }
        s32 indexSelected = -1;
        m_PatternIdTable[type].m_tablePatternId[waveIndex] = SelectPatternId(workArray, workArraySize, false, &indexSelected);
        if(indexSelected >= 0)
        {
          // 次回選択されないようパターンIDを0に
          workArray[indexSelected] = 0;
        }
      }
    }
    break;
  }
}

void CourseData::MakeRandomHitTable()
{
  u32 INDEX_TABLE[MAX_BLOCK_PATTERN_NUM_RANDOM];
  for(u32 i = 0; i < MAX_BLOCK_PATTERN_NUM_RANDOM; i++)
  {
    INDEX_TABLE[i] = i + 1;
    m_randomHitTable[i] = 0;
  }
  // 必中テーブルを作成
  for(u32 i = 0; i < MAX_BLOCK_PATTERN_NUM_RANDOM; i++)
  {
    if(m_pBody->m_blockPatternRandom[i] > 100)
    {
      m_pBody->m_blockPatternRandom[i] -= 100;
      u32 index = SelectPatternId(INDEX_TABLE, MAX_BLOCK_PATTERN_NUM_RANDOM, true);
      INDEX_TABLE[index - 1] = 0;
      m_randomHitTable[index - 1] = i + 1;  // パターンID
    }
  }
}

void CourseData::CalcMaxGimmickNum(BlockDataType type)
{
  // パターンIDテーブル
  const u32 *PATTERN_TABLE[BLOCK_DATA_TYPE_MAX] = {
    m_pBody->m_blockPatternUpward,
    m_pBody->m_blockPatternDownward,
    m_pBody->m_blockPatternRandom,
  };

  // 初期化
  for(u32 i = 0; i < GMKID_MAX; i++)
  {
    m_MaxGimmickNum[type][i] = 0;
  }

  // カウンタ配列を初期化
  u32 gimmickCounter[MAX_U32_WORK_ARRAY_SIZE][GMKID_MAX];
  for(u32 i = 0; i < BLOCK_PATTERN_MAX_SIZE[type]; i++)
  {
    for(u32 j = 0; j < GMKID_MAX; j++)
    {
      gimmickCounter[i][j] = 0;
    }
  }

  // 障害物数をカウントする
  for(u32 i = 0; i < BLOCK_PATTERN_MAX_SIZE[type]; i++)
  {
    for(u32 j = 0; j < m_pBlockDataList[type]->m_blockDataUnitArraySize; j++)
    {
      BlockDataUnit unit = m_pBlockDataList[type]->m_blockDataUnitArray[j];
      if(type != BLOCK_DATA_TYPE_RANDOM && PATTERN_TABLE[type][i] == unit.m_patternId)
      {
        // パターンID一致のためカウントする
        gimmickCounter[i][unit.m_gmkId]++;
      }
      else if(type == BLOCK_DATA_TYPE_RANDOM && i + 1 == unit.m_patternId && PATTERN_TABLE[type][i] > 0)
      {
        gimmickCounter[i][unit.m_gmkId]++;
      }
    }
  }

  // 最大数を求める
  for(u32 i = 0; i < BLOCK_PATTERN_MAX_SIZE[type]; i++)
  {
    for(u32 j = 0; j < GMKID_MAX; j++)
    {
      if(m_MaxGimmickNum[type][j] < gimmickCounter[i][j])
      {
        m_MaxGimmickNum[type][j] = gimmickCounter[i][j] * MAX_GIMMICK_NUM_RATIO[type];
      }
    }
  }
}

void CourseData::CalcMaxFixedGimmickNum()
{
  for(u32 i = 0; i < GMKID_MAX; i++)
  {
    m_MaxFixedGimmickNum[i] = 0;
  }
  for(u32 i = 0; i < GMKID_MAX; i++)
  {
    m_MaxFixedGimmickNum[i] = gfl2::math::FCeil(static_cast<f32>(m_pBody->m_gimmickProb[i]) / 100.0f) * MAX_FIXED_GIMMICK_NUM_RATIO;
  }
}

u32 CourseData::CalcFixedBlockDataMaxSize()
{
  // 固定障害物データの最大サイズを計算
  f32 size = 0;
  return static_cast<u32>(size);
}

u32 CourseData::GetMaxGimmickNum(BlockDataType type, GimmickId gmkId)
{
  return m_MaxGimmickNum[type][gmkId];
}

u32 CourseData::GetMaxFixedGimmickNum(GimmickId gmkId)
{
  return m_MaxFixedGimmickNum[gmkId];
}

void CourseData::GetNextBlockData(BlockDataType type, BlockDataOut* pBlockDataOut, u32 maxSize)
{
  GFL_ASSERT(type < BLOCK_DATA_TYPE_MAX);
  GFL_ASSERT(pBlockDataOut);
  pBlockDataOut->m_blockDataUnitArraySize = 0;
  u32 patternId = 0;
  if(type == BLOCK_DATA_TYPE_RANDOM)
  {
    patternId = CalcNextRandomBlockPatternId();
  }
  else
  {
    patternId = m_PatternIdTable[type].GetNextPatternId();
  }
  if(patternId > 0)
  {
    BlockDataUnit* pDataUnit = pBlockDataOut->m_blockDataUnitArray;
    for(u32 i = 0; i < m_pBlockDataList[type]->m_blockDataUnitArraySize; i++)
    {
      BlockDataUnit dataUnit = m_pBlockDataList[type]->m_blockDataUnitArray[i];
      if(dataUnit.m_patternId == patternId)
      {
        if(pBlockDataOut->m_blockDataUnitArraySize >= maxSize)
        {
          // 最大数を超えた
          HOU_PRINT("[GetNextBlockData]Over Maximun Block Num.\r\n");
          break;
        }
        *pDataUnit = dataUnit;
        pDataUnit++;
        pBlockDataOut->m_blockDataUnitArraySize++;
      }
    }
  }
}

u32 CourseData::CalcNextRandomBlockPatternId()
{
  // 必中テーブルをチェック
  u32 patternId = 0;
  if(m_randomSelectCount >= MAX_RANDOM_TABLE_SIZE && m_randomSelectCount < MAX_RANDOM_TABLE_SIZE + MAX_BLOCK_PATTERN_NUM_RANDOM)
  {
    patternId = m_randomHitTable[m_randomSelectCount - MAX_RANDOM_TABLE_SIZE];
  }
  m_randomSelectCount++;
  HOU_PRINT("[CourseData]RandomCount=%d PatternId=%d\r\n", m_randomSelectCount, patternId);
  if(patternId > 0)
  {
    return patternId;
  }
  // 抽選する
  GFL_ASSERT(MAX_BLOCK_PATTERN_NUM_RANDOM <= MAX_U32_WORK_ARRAY_SIZE);
  u32 workArray[MAX_U32_WORK_ARRAY_SIZE];
  u32 candicateSize = 0;
  u32 random = System::GflUse::GetPublicRand(100);
  for(u32 i = 0; i < MAX_BLOCK_PATTERN_NUM_RANDOM; i++)
  {
    if(m_pBody->m_blockPatternRandom[i] > random)
    {
      workArray[candicateSize] = i + 1;   // パターンID
      candicateSize++;
    }
  }
  patternId = SelectPatternId(workArray, candicateSize, false);
  return patternId;
}

void CourseData::GetNextFixedBlockData(BlockDataOut* pBlockDataOut, u32 maxSize)
{
  GFL_ASSERT(pBlockDataOut);
  pBlockDataOut->m_blockDataUnitArraySize = 0;
  BlockDataUnit* pDataUnit = pBlockDataOut->m_blockDataUnitArray;
  // 発生区域を作成
  const u32 MAX_ZONE_NUM = MAX_BLOCK_POS_X * MAX_BLOCK_POS_Z / 4;
  u32 pos_x_offset = System::GflUse::GetPublicRand(2);
  u32 ZONE_ID_LIST[MAX_ZONE_NUM];
  for(u32 i = 0; i < MAX_ZONE_NUM; i++)
  {
    ZONE_ID_LIST[i] = i + 1;
  }
  // 発生障害物を計算
  for(u32 i = 0; i < GMKID_MAX; i++)
  {
    // 障害物発生率から発生数を計算
    u32 prob = m_pBody->m_gimmickProb[i];
    u32 count = 0;
    if(prob > System::GflUse::GetPublicRand(100))
    {
      count++;
    }
    if(count == 0)
    {
      continue;
    }
    if(pBlockDataOut->m_blockDataUnitArraySize >= maxSize)
    {
      // 最大数を超えた
      HOU_PRINT("[GetNextFixedBlockData]Over Maximun Block Num.\r\n");
      break;
    }
    // 発生位置を計算
    u32 zoneId = SelectPatternId(ZONE_ID_LIST, MAX_ZONE_NUM, true);
    ZONE_ID_LIST[zoneId - 1] = 0;
    s32 posX = (zoneId - 1) % (MAX_BLOCK_POS_X / 2) * 2 + pos_x_offset;
    s32 posZ = (zoneId - 1) / (MAX_BLOCK_POS_X / 2) * 2;
    pBlockDataOut->m_blockDataUnitArraySize += count;
    pDataUnit->m_patternId = 0;
    pDataUnit->m_gmkId = i;
    if(i == GMKID_HOERUOH)
    {
      pDataUnit->m_posX = 0;
    }
    else
    {
      pDataUnit->m_posX = posX;
    }
    pDataUnit->m_posZ = posZ;
    pDataUnit++;
  }
}

void CourseData::PrintBlockData(u32 type)
{
  // ブロックデータ
  for(u32 i = 0; i < m_pBlockDataList[type]->m_blockDataUnitArraySize; i++)
  {
    BlockDataUnit dataUnit = m_pBlockDataList[type]->m_blockDataUnitArray[i];
    HOU_PRINT("[BlockData%d]PatternId=%d GmkId=%d X=%d Y=%d\r\n", type, dataUnit.m_patternId, dataUnit.m_gmkId, dataUnit.m_posX, dataUnit.m_posZ);
  }
  // パターンID
  HOU_PRINT("[PatternId%d]", type);
  for(u32 i = 0; i < m_PatternIdTable[type].m_tableSize; i++)
  {
    HOU_PRINT("%d, ", m_PatternIdTable[type].m_tablePatternId[i]);
  }
  HOU_PRINT("\r\n");
  // 障害物数
  HOU_PRINT("[GimmickNum%d]", type);
  for(u32 i = 0; i < GMKID_MAX; i++)
  {
    HOU_PRINT("%d, ", m_MaxGimmickNum[type][i]);
  }
  HOU_PRINT("\r\n");
}

u32 CourseData::SelectPatternId(const u32* patternIdList, u32 size, bool isCompact, s32 *pIndexOut)
{
  GFL_ASSERT(patternIdList);
  if(pIndexOut) *pIndexOut = -1;  // isCompact=trueの場合は常に-1を返す
  if(size == 0)
  {
    return 0;
  }

  // 候補のサイズを計算
  u32 id = 0;
  if(isCompact)
  {
    // 0を除去して選択
    GFL_ASSERT(size <= MAX_U32_WORK_ARRAY_SIZE);
    u32 workArray[MAX_U32_WORK_ARRAY_SIZE];
    u32 candicateSize = 0;
    for(u32 i = 0; i < size; i++)
    {
      workArray[i] = 0;
      if(patternIdList[i] > 0)
      {
        workArray[candicateSize] = patternIdList[i];
        candicateSize++;
      }
    }
    // 候補リストを作成
    if(candicateSize > 0)
    {
      u32 rand = gfl2::math::Clamp(System::GflUse::GetPublicRand(candicateSize), 0u, candicateSize - 1);
      id = workArray[rand];
    }
  }
  else
  {
    if(size > 0)
    {
      u32 rand = gfl2::math::Clamp(System::GflUse::GetPublicRand(size), 0u, size - 1);
      id = patternIdList[rand];
      if(pIndexOut) *pIndexOut = rand;
    }
  }
  return id;
}

u32 CourseData::MakeFrame(u32 frame, u32 rand)
{
  s32 result = static_cast<s32>(frame);
  s32 randFrame = static_cast<s32>(System::GflUse::GetPublicRand(rand + 1));
  result += randFrame;
  if(result <= 0)
  {
    result = 1;
  }
  return static_cast<u32>(result);
}

CourseData::DataHeader* CourseData::GetHeader() const
{
  return m_pHeader;
}

CourseData::DataBody* CourseData::GetBody() const
{
  return m_pBody;
}

u32 CourseData::GetKeyFrame(u32 index) const
{
  return m_pDataTable[index];
}

CourseData::WaveStateInfo CourseData::GetWaveStateInfo(u32 index, u32 curFrame)
{
  WaveStateInfo state;
  if(index >= m_keyFrameCount - 1)
  {
    state.m_waveState = WAVE_STATE_END;
    return state;
  }
  u32 keyFrame1 = m_pDataTable[index];
  u32 keyFrame2 = m_pDataTable[index + 1];
  GFL_ASSERT(curFrame >= keyFrame1 && curFrame < keyFrame2);
  state.m_stateProgress = static_cast<f32>(curFrame - keyFrame1) / static_cast<f32>(keyFrame2 - keyFrame1);
  f32 data1 = static_cast<f32>(m_pDataTable[index + m_keyFrameCount]);
  f32 data2 = static_cast<f32>(m_pDataTable[index + 1 + m_keyFrameCount]);
  // 状態をチェック
  if(data1 == 0.0f && data2 == 0.0f)
  {
    state.m_waveState = WAVE_STATE_0_0;
  }
  else if(data1 == 0.0f && data2 == 1.0f)
  {
    state.m_waveState = WAVE_STATE_0_1;
  }
  else if(data1 == 1.0f && data2 == 1.0f)
  {
    state.m_waveState = WAVE_STATE_1_1;
  }
  else if(data1 == 1.0f && data2 == 0.0f)
  {
    state.m_waveState = WAVE_STATE_1_0;
  }
  // 高さ値を計算
  if(data1 < data2)
  {
    // 波が低い→高い変化
    state.m_waveHeight = gfl2::math::SinDeg(90.0f * state.m_stateProgress);
  }
  else if(data1 > data2)
  {
    // 波が高い→低い変化
    state.m_waveHeight = 1.0f - gfl2::math::SinDeg(90.0f * state.m_stateProgress);
  }
  else
  {
    // 波高さ変化なし
    state.m_waveHeight = data1;
  }
  return state;
}

u32 CourseData::GetTotalFrame() const
{
  return m_totalFrame;
}

f32 CourseData::GetTotalTime() const
{
  return m_totalTime;
}

GFL_NAMESPACE_END(Course)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
