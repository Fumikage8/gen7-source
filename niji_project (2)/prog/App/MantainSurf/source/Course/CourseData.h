//======================================================================
/**
 * @file CourseData.h
 * @date 2016/11/10 17:33:33
 * @author fang_yicheng
 * @brief マンタインサーフのコースデータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __COURSE_DATA_H_INCLUDED__
#define __COURSE_DATA_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <ctype.h>

#include <System/include/HeapDefine.h>
#include <AppLib/include/Util/app_util_heap.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameProcManager.h>

#include <arc_index/mantain_course_data.gaix>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Course)

// 前方宣言
class CourseDataManager;

class CourseData
{
  GFL_FORBID_COPY_AND_ASSIGN(CourseData);

public:

  // 定数
  enum
  {
    MAX_BLOCK_PATTERN_NUM_UPWARD = 3,     // 波上りブロック抽選パターンの最大サイズ
    MAX_BLOCK_PATTERN_NUM_DOWNWARD = 3,   // 波下りブロック抽選パターンの最大サイズ
    MAX_BLOCK_PATTERN_NUM_RANDOM = 16,    // ランダムブロック抽選パターンの最大サイズ
    MAX_BLOCK_POS_X = 6,                  // ブロック内X座標の最大値
    MAX_BLOCK_POS_Z = 12,                 // ブロック内Y座標の最大値
    MAX_RANDOM_TABLE_SIZE = 32,           // 必中テーブルのサイズ
  };

  // ブロックデータ定数
  enum BlockDataType
  {
    BLOCK_DATA_TYPE_NONE = -1,
    BLOCK_DATA_TYPE_UPWARD,     // 波上りブロック
    BLOCK_DATA_TYPE_DOWNWARD,   // 波下りブロック
    BLOCK_DATA_TYPE_RANDOM,     // ランダムブロック
    BLOCK_DATA_TYPE_MAX,
  };

  enum BlockDataConst
  {
    BLOCK_DATA_UNIT_SIZE = 4,     // ブロックデータの単位サイズ
  };

  // ブロック内の障害物ID
  enum GimmickId
  {
    GMKID_MENOKURAGE,   // メノクラゲ
    GMKID_HOERUKO,      // ホエルコ
    GMKID_SAMEHADER,    // サメハダー
    GMKID_HOERUOH,      // ホエルオー
    GMKID_REEF_S,       // 岩礁S
    GMKID_REEF_M,       // 岩礁M
    GMKID_REEF_L,       // 岩礁L
    GMKID_COMMON,       // 通用型
    GMKID_MAX,
  };

  // 波の状態
  enum WaveState
  {
    WAVE_STATE_NONE = -1,
    WAVE_STATE_0_0,       // 波が低いまま変化しない状態
    WAVE_STATE_0_1,       // 波が低いから高いまで変化中
    WAVE_STATE_1_1,       // 波が高いまま変化しない状態
    WAVE_STATE_1_0,       // 波が高いから低いまで変化中
    WAVE_STATE_END,       // 終了状態
  };

  static const u32 BLOCK_PATTERN_MAX_SIZE[BLOCK_DATA_TYPE_MAX];

public:

  friend class CourseDataManager;

  struct DataHeader
  {
    // 以下はGARCより読み取る
    u32 m_frameRate;          // フレームレート
    u32 m_courseEndTime;      // コース終了時間(フレーム)
    u32 m_blockLengthX;       // ブロックの横長(cm)
    u32 m_blockLengthZ;       // ブロックの縦長(cm)
    u32 m_courseNum;          // コース数
  };

  struct DataBody
  {
    u32 m_courseId;               // コースID
    u32 m_waveCount;              // ビッグウェーブ発生回数
    u32 m_waveInterval;           // ビッグウェーブの発生間隔(フレーム)
    u32 m_waveIntervalRandom;     // ビッグウェーブの発生間隔の乱数(フレーム)
    u32 m_waveChangeTime;         // ビッグウェーブの発生時間(フレーム)
    u32 m_waveChangeTimeRandom;   // ビッグウェーブの発生時間の乱数(フレーム)
    u32 m_waveKeepTime;           // ビッグウェーブの継続時間(フレーム)
    u32 m_waveKeepTimeRandom;     // ビッグウェーブの継続時間の乱数(フレーム)
    u32 m_blockPatternUpward[MAX_BLOCK_PATTERN_NUM_UPWARD];       // 波上りブロックパターン
    u32 m_blockPatternDownward[MAX_BLOCK_PATTERN_NUM_DOWNWARD];   // 波下りブロックパターン
    u32 m_blockPatternRandom[MAX_BLOCK_PATTERN_NUM_RANDOM];       // ランダムブロックパターン
    u32 m_gimmickProb[GMKID_MAX]; // 障害物の発生率
    u32 m_gimmickStaticProbA;     // 設置型障害物(通用型)の発生率
    u32 m_gimmickStaticProbB;     // 設置型障害物(岩礁S)の発生率
    u32 m_gimmickStaticProbC;     // 設置型障害物(岩礁M)の発生率
    u32 m_gimmickStaticProbD;     // 設置型障害物(岩礁L)の発生率
    u32 m_gimmickStaticProbE;     // 設置型障害物(メノクラゲ)の発生率
    u32 m_gimmickStaticProbF;     // 設置型障害物(ホエルコ)の発生率
    u32 m_gimmickDynamicProbA;    // 発生型障害物(サメハダー)の発生率
    u32 m_gimmickDynamicProbB;    // 発生型障害物(ホエルオー)の発生率
  };

  struct BlockDataUnit
  {
    u32 m_patternId;    // パターン番号(1以上の整数)
    u32 m_gmkId;        // 障害物ID
    s32 m_posX;         // X座標
    s32 m_posZ;         // Z座標
  };

  // 入力(リソース保持)用
  struct BlockDataIn
  {
    u32            m_blockDataUnitArraySize;    // ブロックデータ単位の配列サイズ
    BlockDataUnit  m_blockDataUnitArray[1];     // ブロックデータ単位の配列
  };

  // 出力用
  struct BlockDataOut
  {
    u32            m_blockDataUnitArraySize;    // ブロックデータ単位の配列サイズ
    BlockDataUnit* m_blockDataUnitArray;        // ブロックデータ単位の配列

    BlockDataOut()
    {
      m_blockDataUnitArraySize = 0;
      m_blockDataUnitArray = NULL;
    }

    ~BlockDataOut()
    {
      GFL_SAFE_DELETE_ARRAY(m_blockDataUnitArray);
    }
  };

  struct BlockPatternIdTable
  {
    u32  m_curIndex;
    u32  m_tableSize;
    u32* m_tablePatternId;

    BlockPatternIdTable()
    {
      m_curIndex = 0;
      m_tableSize = 0;
      m_tablePatternId = NULL;
    }

    ~BlockPatternIdTable()
    {
      GFL_SAFE_DELETE_ARRAY(m_tablePatternId);
    }

    u32 GetNextPatternId()
    {
      u32 id = 0;
      if(m_tablePatternId && m_tableSize > 0)
      {
        id = m_tablePatternId[m_curIndex];
        m_curIndex = (m_curIndex + 1) % m_tableSize;
      }
      return id;
    }
  };

  struct WaveStateInfo
  {
    WaveState m_waveState;
    f32       m_stateProgress;
    f32       m_waveHeight;

    WaveStateInfo()
    {
      m_waveState = WAVE_STATE_NONE;
      m_stateProgress = 0.0f;
      m_waveHeight = 0.0f;
    }
  };

public:

  CourseData();
  virtual ~CourseData();

  void Setup(gfl2::heap::HeapBase* pHeap, void* pHeader, void* pData, void** pBlockDataList);

  DataHeader* GetHeader() const;
  DataBody*   GetBody()   const;

  u32  GetKeyFrameCount() { return m_keyFrameCount; }

  u32  GetKeyFrame(u32 index) const;

  WaveStateInfo GetWaveStateInfo(u32 index, u32 curFrame);

  u32  GetTotalFrame() const;
  f32  GetTotalTime()  const;

  void GetNextBlockData(BlockDataType type, BlockDataOut* pBlockDataOut, u32 maxSize);
  void GetNextFixedBlockData(BlockDataOut* pBlockDataOut, u32 maxSize);

  u32  CalcFixedBlockDataMaxSize();
  u32  GetMaxGimmickNum(BlockDataType type, GimmickId gmkId);
  u32  GetMaxFixedGimmickNum(GimmickId gmkId);

  u32  GetWaveEndKeyFrameIndex();
  u32  GetLastWaveKeyFrameIndex();

private:

  void SetupHeader(void* pHeader);
  void SetupCourseData(void* pData);
  void SetupBlockData(void** pBlockDataList);

  void MakePatternIdTable(BlockDataType type);
  void CalcMaxGimmickNum(BlockDataType type);
  void CalcMaxFixedGimmickNum();
  u32  MakeFrame(u32 frame, u32 rand);
  void MakeRandomHitTable();

  void PrintBlockData(u32 type);

  u32  CalcNextRandomBlockPatternId();
  u32  SelectPatternId(const u32* patternIdList, u32 size, bool isCompact, s32 *pIndexOut = NULL);

private:

  gfl2::heap::HeapBase* m_pHeap;
  DataHeader*           m_pHeader;
  DataBody*             m_pBody;
  u32*                  m_pDataTable;
  BlockDataIn*          m_pBlockDataList[BLOCK_DATA_TYPE_MAX];            // ブロックパターンデータ
  BlockPatternIdTable   m_PatternIdTable[BLOCK_DATA_TYPE_MAX];            // パターンIDテーブル
  u32                   m_MaxGimmickNum[BLOCK_DATA_TYPE_MAX][GMKID_MAX];  // 各タイプの障害物最大数
  u32                   m_MaxFixedGimmickNum[GMKID_MAX];

  u32 m_keyFrameCount;  // キーフレームの最大数
  u32 m_totalFrame;
  f32 m_totalTime;

  u32 m_randomSelectCount;                                // ランダムブロックの抽選回数
  u32 m_randomHitTable[MAX_BLOCK_PATTERN_NUM_RANDOM];     // ランダムブロックの必中テーブル
};

GFL_NAMESPACE_END(Course)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __COURSE_DATA_H_INCLUDED__

