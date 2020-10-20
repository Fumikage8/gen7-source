#ifndef GFLIB2_GRX_CTR_CTRCOMMANDLIST_H_INCLUDED
#define GFLIB2_GRX_CTR_CTRCOMMANDLIST_H_INCLUDED

#include <gfx/include/gfl2_Buffer.h>
#include <gfx/include/CTR/gfl2_CTRTypes.h>

#include <nn/gx.h>

namespace gfl2 { namespace gfx { namespace ctr {

class CTRGL;

// CTRのコマンドリストの薄いラッパークラス
// 主に gx Raw系のAPIを使用
class CTRCommandList : public GLMemory
{
public:

  typedef void (*CALLBACK_FUNC)(s32);
  typedef void (*TIMEOUT_FUNC)();

public:
	friend class CTRGL;

	CTRCommandList();
	virtual ~CTRCommandList();

  // GXオブジェクト取得
  nngxCommandList* GetGxObject()
  {
    return &m_CommandList;
  }

  // ■コマンドリスト実行制御
  // 指定したコマンドリストのデータ領域を確保します。
  void Storage(u32 bufSize, void* pCommandBuffer, u32 requestCount, void* pCommandRequestBuffer)
  {
    nngxCmdlistStorageRaw(&m_CommandList, bufSize, pCommandBuffer, requestCount, pCommandRequestBuffer);
  }
	// コマンドリストキューのデータ格納用バッファに必要なバッファサイズを計算し返します。
  static u32 GetRequestSize(u32 requestCount)
  {
    return nngxGetCommandRequestSizeRaw(requestCount);
  }
  //	指定したコマンドリストの3Dコマンドバッファを区切ります。
  void SplitDraw()
  {
    nngxSplitDrawCmdlistRaw(&m_CommandList);
  }
  // 指定したコマンドリストの終了コールバックを有効にします。
  void EnableCallback(s32 id)
  {
    nngxEnableCmdlistCallbackRaw(&m_CommandList, id);
  }
  // 指定したコマンドリストのコマンドリストの終了コールバックを無効にします。
  void DisableCallback(s32 id)
  {
    nngxDisableCmdlistCallbackRaw(&m_CommandList, id);
  }
  // 指定したコマンドリストの終了コールバック関数を設定します。
  void SetCallback(CALLBACK_FUNC func)
  {
    nngxSetCmdlistCallbackRaw(&m_CommandList, func);
  }
  // 指定したコマンドリストを実行します。
  void Run()
  {
    nngxRunCmdlistRaw(&m_CommandList);
  }
  // 指定したコマンドリストの停止予約をします。
  void ReserveStop(s32 id)
  {
    nngxReserveStopCmdlistRaw(&m_CommandList, id);
  }
  // 指定したコマンドリストに蓄積されたコマンドをクリアします。
  void Clear()
  {
    nngxClearCmdlistRaw(&m_CommandList);
  }
  // 指定したコマンドリストに蓄積されたコマンドを指定した値でクリアします。
  void ClearFill(u32 data)
  {
    nngxClearFillCmdlistRaw(&m_CommandList, data);
  }
  // 指定したコマンドリストの指定したコマンドリクエストのハンドラにおいて、ガス密度情報描画による加算ブレンド結果の更新を行います。
  void SetGasAutoAccumulationUpdate(s32 id)
  {
    nngxSetGasAutoAccumulationUpdateRaw(&m_CommandList, id);
  }
  // 指定したコマンドリストで、ガス密度情報描画による加算ブレンド結果の更新を行うかどうかを指定します。
  void SetGasUpdate(b32 enable)
  {
    nngxSetGasUpdateRaw(&m_CommandList, enable);
  }
  // 指定したコマンドリストが実行中かどうかを確認します。
  b32 GetIsRunning()
  {
    return nngxGetIsRunningRaw(&m_CommandList);
  }
  // 指定したコマンドリストで蓄積済みの 3Dコマンドバッファのサイズを返します。
  u32 GetUsedBufferSize()
  {
    return nngxGetUsedBufferSizeRaw(&m_CommandList);
  }
  // 指定したコマンドリストで蓄積済みのコマンドリクエストの数を返します。
  u32 GetUsedRequestCount()
  {
    return nngxGetUsedRequestCountRaw(&m_CommandList);
  }
  // 指定したコマンドリストで蓄積可能な 3Dコマンドバッファの最大サイズを返します。
  u32 GetMaxBufferSize()
  {
    return nngxGetMaxBufferSizeRaw(&m_CommandList);
  }
  // 指定したコマンドリストで蓄積可能なコマンドリクエストの最大個数を返します。
  u32 GetMaxRequestCount()
  {
    return nngxGetMaxRequestCountRaw(&m_CommandList);
  }
  // 指定したコマンドリストで設定されている 3Dコマンドバッファの先頭アドレスを返します。
  u32* GetTopBufferAddr()
  {
    return static_cast<u32*>(nngxGetTopBufferAddrRaw(&m_CommandList));
  }
  // 指定したコマンドリストで実行済みの 3Dコマンドバッファのサイズを返します。
  u32 GetRunBufferSize()
  {
    return nngxGetRunBufferSizeRaw(&m_CommandList);
  }
  // 指定したコマンドリストで実行済みのコマンドリクエストの個数を返します。
  u32 GetRunRequestCount()
  {
    return nngxGetRunRequestCountRaw(&m_CommandList);
  }
  // 指定したコマンドリストで設定されているコマンドリクエストキューデータ格納用バッファの先頭アドレスを返します。
  u32* GetTopRequestAddr()
  {
    return static_cast<u32*>(nngxGetTopRequestAddrRaw(&m_CommandList));
  }
  // 指定したコマンドリストに蓄積されているコマンドリクエストのうち、次に実行されるものの種類を返します。
  s32 GetNextRequestType()
  {
    return nngxGetNextRequestTypeRaw(&m_CommandList);
  }
  // 指定したコマンドリストに蓄積されている 3Dコマンドバッファで、次に実行されるバッファのアドレスを返します。
  u32* GetNextBufferAddr()
  {
    return static_cast<u32*>(nngxGetNextBufferAddrRaw(&m_CommandList));
  }
  // 指定したコマンドリストに蓄積されている 3Dコマンドバッファで、次に実行されるバッファのサイズを返します。
  u32 GetNextBufferSize()
  {
    return nngxGetNextBufferSizeRaw(&m_CommandList);
  }
  // 指定したコマンドリストで、次に 3Dコマンドが蓄積される 3Dコマンドバッファのアドレスを返します。
  u32* GetCurrentBufferAddr()
  {
    return static_cast<u32*>(nngxGetCurrentBufferAddrRaw(&m_CommandList));
  }
  // ハードウェアの状態を示す 32 ビットのデータを取得します。
  static bit32 GetHWState()
  {
    return nngxGetHWStateRaw();
  }

  // ■コマンドキャッシュ操作
  // コマンドリストオブジェクトを別のコマンドリストオブジェクトへコピーします。
  //void Copy(CTRCommandList* pSource)
  //{
    // SDK11以降
    //nngxCopyCmdlistRaw(&pSource->m_CommandList, &m_CommandList);
  //}
  // 指定したコマンドリストに対し、指定した領域のデータ（3Dコマンド）を 3Dコマンドバッファに追加します。
  void Add3DCommandNoCacheFlush(const void* pBuffer, u32 bufferSize);

  // 指定したコマンドリストオブジェクトの内容を、別のコマンドリストオブジェクトへ追加でコピーします。
  //void Add(CTRCommandList* pSource)
  //{
    // SDK11以降
    //nngxAddCmdlistRaw(&pSource->m_CommandList, &m_CommandList);
  //}

  // ■低レベルコマンドリクエスト発行
  // 指定したコマンドリストに3D実行コマンドを発行し、蓄積済み3Dコマンドバッファをフラッシュします。
  void Flush3DCommandNoCacheFlush()
  {
    nngxFlush3DCommandNoCacheFlushRaw(&m_CommandList);
  }
  // 指定したコマンドリストに3D実行コマンドを発行し、蓄積済み3Dコマンドバッファをフラッシュします。 ただし、3D コマンドは指定したサイズ分のみ実行します。
  void Flush3DCommandPartially(u32 bufferSize)
  {
    nngxFlush3DCommandPartiallyRaw(&m_CommandList, bufferSize);
  }
  // 指定したコマンドリストの 3Dコマンドバッファの現在の出力先ポインタを移動させます。
  void MoveCommandbufferPointer(s32 offset)
  {
    nngxMoveCommandbufferPointerRaw(&m_CommandList, offset);
  }
  // 指定したコマンドリストに、ブロックイメージからリニアイメージへの変換転送コマンドを追加します。
  void AddB2LTransferCommand(const void* pSourceAddr, u32 sourceWidth, u32 sourceHeight, u32 sourceFormat, void* pDestAddr, u32 destWidth, u32 destHeight, u32 destFormat, u32 aaMode, b32 yFlip, u32 blockSize)
  {
    nngxAddB2LTransferCommandRaw(&m_CommandList, pSourceAddr, sourceWidth, sourceHeight, sourceFormat, pDestAddr, destWidth, destHeight, destFormat, aaMode, yFlip, blockSize);
  }
  // 指定したコマンドリストに、ブロックイメージの転送コマンドを追加します。
  void AddBlockImageCopyCommand(const void* pSourceAddr, u32 sourceUnit, u32 sourceInterval, void* pDestAddr, u32 destUnit, u32 destInterval, u32 totalSize)
  {
    nngxAddBlockImageCopyCommandRaw(&m_CommandList, pSourceAddr, sourceUnit, sourceInterval, pDestAddr, destUnit, destInterval, totalSize);
  }
  // 指定したコマンドリストに、リニアイメージからブロックイメージへの変換転送コマンドを追加します。
  void AddL2BTransferCommand(const void* pSourceAddr, void* pDestAddr, u32 width, u32 height, u32 format, u32 blockSize)
  {
    nngxAddL2BTransferCommandRaw(&m_CommandList, pSourceAddr, pDestAddr, width, height, format, blockSize);
  }
  // 指定したコマンドリストに、メモリフィルコマンドを追加します。
  void AddMemoryFillCommand(void* pStartAddr0, u32 size0, u32 data0, u32 width0, void* pStartAddr1, u32 size1, u32 data1, u32 width1)
  {
    nngxAddMemoryFillCommandRaw(&m_CommandList, pStartAddr0, size0, data0, width0, pStartAddr1, size1, data1, width1);
  }
  // 指定したコマンドリストに、DMA 転送コマンドを追加します。
  void AddVramDmaCommandNoCacheFlush(const void* pSourceAddr, void* pDestAddr, u32 size)
  {
    nngxAddVramDmaCommandNoCacheFlushRaw(&m_CommandList, pSourceAddr, pDestAddr, size);
  }
  // 指定したコマンドリストに、ブロックイメージのアンチエイリアスフィルタ転送コマンドを追加します。
  void FilterBlockImage(const void* pSourceAddr, void* pDestAddr, u32 width, u32 height, u32 format)
  {
    nngxFilterBlockImageRaw(&m_CommandList, pSourceAddr, pDestAddr, width, height, format);
  }
  // 指定したコマンドバッファへ実行アドレスをジャンプさせるコマンドを、指定したコマンドリストオブジェクトに追加します。
  //void AddJumpCommand(const void* pBufferAddr, u32 bufferSize)
  //{
    // SDK11以降
    //nngxAddJumpCommandRaw(&m_CommandList, pBufferAddr, bufferSize);
  //}
  // 指定したコマンドバッファへ実行アドレスをジャンプするコマンドと ジャンプ元のコマンドバッファに戻るためのアドレス情報設定用コマンドを、指定したコマンドリストオブジェクトに追加します。
  //void AddSubroutineCommand(const void* pBufferAddr, u32 bufferSize)
  //{
    // SDK11以降
    //nngxAddSubroutineCommandRaw(&m_CommandList, pBufferAddr, bufferSize);
  //}

  // ■共通
  // 実行中のコマンドリストオブジェクトの完了を待ちます。
  static void WaitDone()
  {
    nngxWaitCmdlistDone();
  }
  // nngxWaitCmdlistDone を指定した時間でタイムアウトするように設定します。
  static void SetTimeout(u64 time, TIMEOUT_FUNC func)
  {
    nngxSetTimeout(time, func);
  }
  // コマンドリストの実行を停止します。
  static void Stop()
  {
    nngxStopCmdlist();
  }

  // ■デバッグ
  // コンソールにデバッグ出力
  void DebugDumpConsole();

private:

  nngxCommandList m_CommandList;
};

}}}

#endif
