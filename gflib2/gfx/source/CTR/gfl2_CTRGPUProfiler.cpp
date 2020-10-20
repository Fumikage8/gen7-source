#include <string.h>

#include <gfx/include/CTR/gfl2_CTRGPUProfiler.h>
#include <gfx/include/CTR/gfl2_CTRCommandList.h>
#include <gfx/include/CTR/gfl2_CTRTimerQuery.h>

#include <orcsFormat.h>
#include <orcsRecorder.h>

namespace gfl2 { namespace gfx { namespace ctr {

CTRGPUProfiler* CTRGPUProfiler::s_pInstance = NULL;
static u32 GPU_PROFILE_BUSY_MICRO_SECOND = 1;      // このマイクロ秒単位にビジーチェックする
static u32 GPU_PROFILE_BUSY_SAMPLIG_COUNT = 0;

// コンストラクタ
CTRGPUProfiler::CTRGPUProfiler() :
  m_TotalCPUTimeMicroSeconds(0),
  m_TotalCPUTimeMicroSecondsBegin(0),
  m_TotalGPUTimeMicroSeconds(0),
  m_TotalGPUTimeMicroSecondsBegin(0),
  m_TotalGPUTimeRequestNo(0),
  m_TotalGPUTimeWaiting(false),
  m_pTotalGPUTimeCommandList(NULL),
  m_ProfilingWating(false)
{
  s_pInstance = this;

  // コールバックを初期化
  for (u32 i = 0; i < MAX_TIMER_QUERY_COUNT; ++i)
  {
    TimerQueryParam* pParam = &m_TimerQueryParamList[i];

    pParam->pTimerQuery = NULL;
    pParam->beginRequestNo = 0;
    pParam->endRequestNo = 0;
  }

  // プロファイリングの結果を初期化
  std::memset(&m_RawProfilingResult, 0, sizeof(m_RawProfilingResult));
  for (u32 i = 0; i < TYPE::TOTAL_COUNT; ++i)
  {
    m_ProfilingResult.values[i] = 0;
    m_ProfilingResultPrev.values[i] = 0;
  }
}

// デストラクタ
CTRGPUProfiler::~CTRGPUProfiler()
{
  s_pInstance = NULL;
}

// GPUプロファイラー用の.orcsのファイル名を設定
void CTRGPUProfiler::SetOrcsName(const char* pFileName)
{
  u32 length = strlen(pFileName);
  if (length > MAX_ORCS_NAME_LENGTH)
  {
    length = MAX_ORCS_NAME_LENGTH;
  }
  memset(m_OrcsName, 0, MAX_ORCS_NAME_LENGTH);
  memcpy(m_OrcsName, pFileName, length);
}

// GPUプロファイラー用の.orcsファイルを出力
void CTRGPUProfiler::ExportOrcs(CTRCommandList* pTargetList)
{
// HIO有効なら
#if GFL_HIO_ENABLE
  // 出力用のメモリ確保
  size_t scratchBufferSize = ORCS_DEFAULT_SCRATCH_BUFFER_SIZE_IN_BYTES * 10;
  int recordLimit = ORCS_DEFAULT_RECORD_LIMIT * 10;
  size_t orcsSize = OrcsCalcRequiredStorageSize(scratchBufferSize, recordLimit);

  u32 heapid = 0;

  // ダンプ用メモリが確保できるヒープを探す
  // この関数内だけなのですぐに返します。
  gfl2::heap::HeapBase* pHeap = NULL;
  while (true)
  {
    pHeap = gfl2::heap::Manager::GetHeapByHeapId(heapid);
    ++heapid;
    if (pHeap == NULL)
    {
      break;
    }

    int allocatableSize = pHeap->GetTotalAllocatableSize();
    if (allocatableSize >= orcsSize)
    {
      break;
    }
  }


  GFL_ASSERT(pHeap != NULL);
  u8* orcsMem = (u8*)GflHeapAllocMemoryAlign(pHeap, orcsSize, 128);

  nngxCommandList* commandLists[] = {pTargetList->GetGxObject()};

  // 仕様パラメータで構造体をフィル
  OrcsRecorderInfo ri;
  ri.scratchBufferSize = scratchBufferSize;
  ri.recordLimit = recordLimit;
  ri.totalBufferSize = orcsSize;
  ri.passInStorageBuffer = orcsMem;

  ri.optional.cmdlistCount = 0;
  ri.optional.cmdlistIds = NULL;
  ri.optional.cmdlistCountRaw = 1;
  ri.optional.cmdlistPtrsRaw = commandLists;

  ri.optional.display0.displayX = 0;
  ri.optional.display0.displayY = 0;
  ri.optional.display0Ext.displayX = 0;
  ri.optional.display0Ext.displayY = 0;
  ri.optional.display1.displayX = 0;
  ri.optional.display1.displayY = 0;

  const size_t BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];

  nn::hio::GetCurrentDirectory(buffer, sizeof(buffer));
  NN_LOG("Exporting ORCS file to directory:%s\n", buffer);

  nn::hio::HostFile hf;
  nn::Result hfResult = hf.Open(m_OrcsName, nn::hio::HostFile::ACCESS_MODE_READ_WRITE, nn::hio::HostFile::OPEN_DISP_CREATE_ALWAYS);
  if (hfResult.IsSuccess())
  {
    // ORCSを出力
    OrcsCreateAndExportDefault(OrcsDefaultErrorHandler, &ri, &hf);

    hf.Close();
    NN_LOG("Exported ORCS file:%s\n", m_OrcsName);
  }
  else
  {
    NN_LOG("Problem with host file\n");
  }

  GflHeapSafeFreeMemory(orcsMem);

	GLuint err = glGetError();
	if ( err != GL_NO_ERROR )
	{
    // ここでエラーになるがデバッグ用途なので大丈夫
	}

#endif // GFL_HIO_ENABLE
}

// GPU計測開始
void CTRGPUProfiler::StartGPUTotalTime(CTRCommandList* pTargetList)
{
  // 前回の計測が終わっているか？
  if (m_TotalGPUTimeWaiting)
  {
    // 完了を待つ
    m_pTotalGPUTimeCommandList->WaitDone();
  }

  // コールバック設定
  u32 requestCount = pTargetList->GetUsedRequestCount();
  if (requestCount == 0)
  {
    // リクエスト数が０なら計測できない
    return;
  }
  pTargetList->SetCallback(CTRGPUProfiler::CommandListDoneCallback);
  pTargetList->EnableCallback(requestCount);

  // 計測開始
  m_pTotalGPUTimeCommandList = pTargetList;
  m_TotalGPUTimeWaiting = true;
  m_TotalGPUTimeRequestNo = requestCount;
  m_TotalGPUTimeMicroSecondsBegin = GetCurrentMicroSeconds();
}

// CPU計測開始
void CTRGPUProfiler::StartCPUTotalTime()
{
  m_TotalCPUTimeMicroSecondsBegin = GetCurrentMicroSeconds();
}

// CPU計測終了
void CTRGPUProfiler::EndCPUTotalTime()
{
  m_TotalCPUTimeMicroSeconds = GetCurrentMicroSeconds() - m_TotalCPUTimeMicroSecondsBegin;
}

// タイマークエリの開始コールバック登録
void CTRGPUProfiler::SetTimerQueryBeginCallback(CTRTimerQuery* pTimerQuery, CTRCommandList* pCommandList)
{
  if (pTimerQuery == NULL)
  {
    return;
  }
  if (pCommandList == NULL)
  {
    return;
  }

  u32 requestCount = pCommandList->GetUsedRequestCount();

  // 空いているリストを検索
  for (u32 i = 0; i < MAX_TIMER_QUERY_COUNT; ++i)
  {
    TimerQueryParam* pParam = &m_TimerQueryParamList[i];

    if (pParam->pTimerQuery != NULL)
    {
      continue;
    }

    pParam->pTimerQuery = pTimerQuery;
    pParam->beginRequestNo = requestCount;

    pCommandList->EnableCallback(requestCount);
    break;
  }
}

// タイマークエリの終了コールバック登録
void CTRGPUProfiler::SetTimerQueryEndCallback(CTRTimerQuery* pTimerQuery, CTRCommandList* pCommandList)
{
  if (pTimerQuery == NULL)
  {
    return;
  }
  if (pCommandList == NULL)
  {
    return;
  }

  u32 requestCount = pCommandList->GetUsedRequestCount();

  // 同じクエリを検索
  for (u32 i = 0; i < MAX_TIMER_QUERY_COUNT; ++i)
  {
    TimerQueryParam* pParam = &m_TimerQueryParamList[i];

    if (pParam->pTimerQuery != pTimerQuery)
    {
      continue;
    }

    pParam->endRequestNo = requestCount;

    pCommandList->EnableCallback(requestCount);
    break;
  }
}

// コマンドリスト終了時のコールバック
void CTRGPUProfiler::CommandListDoneCallback(s32 requestNo)
{
  // 現在の時間
  s64 currentTime = GetCurrentMicroSeconds();

  // GPUの合計時間
  if (s_pInstance->m_TotalGPUTimeRequestNo == requestNo)
  {
    s_pInstance->m_TotalGPUTimeWaiting = false;
    s_pInstance->m_TotalGPUTimeMicroSeconds = currentTime - s_pInstance->m_TotalGPUTimeMicroSecondsBegin;
    return;
  }

  // タイマークエリのはず
  for (u32 i = 0; i < MAX_TIMER_QUERY_COUNT; ++i)
  {
    TimerQueryParam* pParam = &s_pInstance->m_TimerQueryParamList[i];

    if (pParam->pTimerQuery == NULL)
    {
      continue;
    }

    if (pParam->beginRequestNo == requestNo)
    {
      // 開始コールバック
      pParam->pTimerQuery->GPUBeginCallback(currentTime);
    }
    else if (pParam->endRequestNo == requestNo)
    {
      // 終了コールバック
      pParam->pTimerQuery->GPUEndCallback(currentTime);
      pParam->pTimerQuery = NULL;
      pParam->beginRequestNo = 0;
      pParam->endRequestNo = 0;
    }
    break;
  }
}

// GPUプロファイリングの開始
void CTRGPUProfiler::StartGPUProfile()
{
  // 計測中なら無視
  if (m_ProfilingWating)
  {
    return;
  }

  // ビジーカウンタ用の設定
  nngxSetProfilingParameter(NN_GX_PROFILING_BUSY_SAMPLING_TIME_MICRO_SECOND, GPU_PROFILE_BUSY_MICRO_SECOND);
  nngxSetProfilingParameter(NN_GX_PROFILING_BUSY_SAMPLING_COUNT, GPU_PROFILE_BUSY_SAMPLIG_COUNT);

  // ビジーカウンタのプロファイリング開始
  m_ProfilingWating = true;
  nngxStartProfiling(NN_GX_PROFILING_BUSY);
}

// GPUプロファイリングの終了
void CTRGPUProfiler::StopGPUProfile()
{
  // 計測開始していないなら無視
  if (!m_ProfilingWating)
  {
    return;
  }

  // ビジーカウンタのプロファイリング終了
  nngxStopProfiling(NN_GX_PROFILING_BUSY);
  m_ProfilingWating = false;

  // プロファイリング結果取得
  nngxGetProfilingResult(NN_GX_PROFILING_BUSY, m_RawProfilingResult.busy);
  nngxGetProfilingResult(NN_GX_PROFILING_VERTEX_SHADER0, m_RawProfilingResult.vertexShader[0]);
  nngxGetProfilingResult(NN_GX_PROFILING_VERTEX_SHADER1, m_RawProfilingResult.vertexShader[1]);
  nngxGetProfilingResult(NN_GX_PROFILING_VERTEX_SHADER2, m_RawProfilingResult.vertexShader[2]);
  nngxGetProfilingResult(NN_GX_PROFILING_VERTEX_SHADER3, m_RawProfilingResult.vertexShader[3]);
  nngxGetProfilingResult(NN_GX_PROFILING_VERTEX_CACHE, &m_RawProfilingResult.vertexCache);
  nngxGetProfilingResult(NN_GX_PROFILING_POLYGON, m_RawProfilingResult.polygon);
  nngxGetProfilingResult(NN_GX_PROFILING_FRAGMENT, &m_RawProfilingResult.fragment);
  nngxGetProfilingResult(NN_GX_PROFILING_MEMORY_ACCESS, m_RawProfilingResult.memoryAccess);

  // プロファイリング結果を読める形に直す

  // そのまま格納
  // Busy
  for (u32 i = 0; i < 4; ++i)
  {
    u32 index = TYPE::BUSY_SHADER0 + (i * 2);
    m_ProfilingResult.values[index + 0] = m_RawProfilingResult.busy[i] >> 16;        // 上位ビット
    m_ProfilingResult.values[index + 1] = m_RawProfilingResult.busy[i] & 0x0000FFFF; // 下位ビット
  }

  // 前回との差分を格納
  // VertexShader
  for (u32 i = 0; i < 4; ++i)
  {
    u32 tmp = TYPE::VERTEX0_PROGCOUNTER + (i * 5);
    for (u32 j = 0; j < 5; ++j)
    {
      u32 index = tmp + j;
      m_ProfilingResult.values[index] = m_RawProfilingResult.vertexShader[i][j] - m_ProfilingResultPrev.values[index];
      m_ProfilingResultPrev.values[index] = m_RawProfilingResult.vertexShader[i][j];
    }
  }

  // Polygon
  for (u32 i = 0; i < 3; ++i)
  {
    u32 index = TYPE::POLYGON_INPUT_VERTEX + i;
    m_ProfilingResult.values[index] = m_RawProfilingResult.polygon[i] - m_ProfilingResultPrev.values[index];
    m_ProfilingResultPrev.values[index] = m_RawProfilingResult.polygon[i];
  }

  // Fragment
  m_ProfilingResult.values[TYPE::FRAGMENT_INPUT_FRAGMENT] = m_RawProfilingResult.fragment - m_ProfilingResultPrev.values[TYPE::FRAGMENT_INPUT_FRAGMENT];
  m_ProfilingResultPrev.values[TYPE::FRAGMENT_INPUT_FRAGMENT] = m_RawProfilingResult.fragment;

  // MemoryAccess
  for (u32 i = 0; i < 18; ++i)
  {
    u32 index = TYPE::ACCESS_VRAMA_READ + i;
    m_ProfilingResult.values[index] = (m_RawProfilingResult.memoryAccess[i] * 16) - (m_ProfilingResultPrev.values[index] * 16);
    m_ProfilingResultPrev.values[index] = m_RawProfilingResult.memoryAccess[i];
  }
}

}}}
