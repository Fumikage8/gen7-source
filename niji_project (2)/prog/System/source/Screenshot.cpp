//======================================================================
/**
 * @file  Screenshot.cpp
 * @date 15/05/28
 * @author  araki_syo
 * @brief  デバッグ用スクリーンショットを保存する関数
 */
//======================================================================
#if (PM_DEBUG)

#include <System/include/HeapDefine.h>
#include <gfx/include/gfl2_PCDebugUtil.h>
#include <System/include/Screenshot.h>

GFL_NAMESPACE_BEGIN(System)

/**
 * @brief スクリーンショットを保存する
 * @param fileName ファイル名
 * @param target 撮影する画面を指定する 指定しない場合は上画面左側
 * @detail targetで指定された画面のスクリーンショットを保存する
 */
void Screenshot::PrintScreen(const char* fileName, gfl2::gfx::DisplayMode displayMode = gfl2::gfx::DisplayMode::CTR_LEFT)
{
  char path[MAX_PATH];
  GetTargetPath(path, fileName);

  u32 displayNo = displayMode - gfl2::gfx::DisplayMode::CTR_LEFT;

  gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG_DEBUGGER);
  gfl2::heap::HeapBase* pHeap = GFL_CREATE_LOCAL_HEAP_NAME(pDevHeap, 0x200000, gfl2::heap::HEAP_TYPE_EXP, false, "screenshot buffer");
  gfl2::gfx::PCDebugUtil::PrintScreen(path, System::GflUse::m_HandleList[displayNo], pHeap);

  GFL_DELETE_HEAP(pHeap);
}

/**
 * @brief 深度画像を保存する
 * @param fileName ファイル名
 * @param target 撮影する画面を指定する 指定しない場合は上画面左側
 * @detail targetで指定された画面の深度画像を保存する
 */
void Screenshot::PrintDepthMap(const char* fileName, gfl2::gfx::DisplayMode displayMode = gfl2::gfx::DisplayMode::CTR_LEFT)
{
  char path[MAX_PATH];
  GetTargetPath(path, fileName);

  u32 displayNo = displayMode - gfl2::gfx::DisplayMode::CTR_LEFT;

  gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG_DEBUGGER);
  gfl2::heap::HeapBase* pHeap = GFL_CREATE_LOCAL_HEAP_NAME(pDevHeap, 0x200000, gfl2::heap::HEAP_TYPE_EXP, false, "screenshot buffer");
  gfl2::gfx::PCDebugUtil::PrintDepthMap(path, System::GflUse::m_HandleList[displayNo], pHeap);

  GFL_DELETE_HEAP(pHeap);
}

/**
 * @brief 絶対深度画像を保存する
 * @param fileName ファイル名
 * @param target 撮影する画面を指定する 指定しない場合は上画面左側
 * @detail targetで指定された画面の深度画像を保存する
 */
void Screenshot::PrintAbsDepthMap(const char* fileName, gfl2::gfx::DisplayMode displayMode = gfl2::gfx::DisplayMode::CTR_LEFT)
{
  char path[MAX_PATH];
  GetTargetPath(path, fileName);

  u32 displayNo = displayMode - gfl2::gfx::DisplayMode::CTR_LEFT;

  gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG_DEBUGGER);
  gfl2::heap::HeapBase* pHeap = GFL_CREATE_LOCAL_HEAP_NAME(pDevHeap, 0x200000, gfl2::heap::HEAP_TYPE_EXP, false, "screenshot buffer");
  gfl2::gfx::PCDebugUtil::PrintAbsDepthMap(path, System::GflUse::m_HandleList[displayNo], pHeap);

  GFL_DELETE_HEAP(pHeap);
}

/**
 * @brief ステンシル画像を保存する
 * @param fileName ファイル名
 * @param target 撮影する画面を指定する 指定しない場合は上画面左側
 * @detail targetで指定された画面のステンシル画像を保存する
 */
void Screenshot::PrintStencil(const char* fileName, gfl2::gfx::DisplayMode displayMode = gfl2::gfx::DisplayMode::CTR_LEFT)
{
  char path[MAX_PATH];
  GetTargetPath(path, fileName);

  u32 displayNo = displayMode - gfl2::gfx::DisplayMode::CTR_LEFT;

  gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG_DEBUGGER);
  gfl2::heap::HeapBase* pHeap = GFL_CREATE_LOCAL_HEAP_NAME(pDevHeap, 0x200000, gfl2::heap::HEAP_TYPE_EXP, false, "screenshot buffer");
  gfl2::gfx::PCDebugUtil::PrintStencil(path, System::GflUse::m_HandleList[displayNo], pHeap);

  GFL_DELETE_HEAP(pHeap);
}

/**
 * @brief キャプチャ画像保存先パス(%NIJI_ROOT_DIR%/Capture/filename)を取得する
 * @param[out] path 保存先パス
 * @param[in] filename ファイル名
 */
void Screenshot::GetTargetPath(char* path, const char* filename)
{
  u32 envVarLength;
  char envVar[MAX_PATH];

  bool flag = false;
  gfl2::fs::Result res = gfl2::fs::PcUtil::IsPcAccessEnableFlag(&flag);

  if(flag = res.IsSuccess() && flag)
  {
    gfl2::fs::PcUtil::GetProcessEnvironmentVariable(&envVarLength, envVar, MAX_PATH, "NIJI_ROOT_DIR");
  }

#if defined(GF_PLATFORM_WIN32)
  sprintf_s(path, MAX_PATH, "%sCapture/Win32/%s", flag? envVar : "", filename);
#else
  sprintf(path, "%sCapture/CTR/%s", flag? envVar : "", filename);
#endif
}

GFL_NAMESPACE_END(System)

#endif // PM_DEBUG
