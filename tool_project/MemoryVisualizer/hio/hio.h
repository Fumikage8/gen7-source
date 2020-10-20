// hio.h

#pragma once

#include <SDKDDKVer.h>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <conio.h>
#include <ISCTRDLL.h>

using namespace System;

namespace hio {

  public ref class ISCTRDLL
  {
  public:
    ISCTRDLL();

    ~ISCTRDLL();
    !ISCTRDLL();

    /// ISCTR.dllをロード済みかどうか
    bool IsDLLLoaded(void);
    
    bool Open(void);
    void Close(void);
    bool IsOpened(void);
    bool CheckConnect(void);
    bool DownloadGo(System::String^ pStrFileName);
    bool EnableHIO(void);
    bool DisableHIO(void);
    bool OpenSHMEM(unsigned short nChannel);
    void CloseSHMEM(unsigned short nChannel);
    bool ReadSHMEM(unsigned short nChannel, unsigned long nOffsetAddr, void* pData, unsigned long nLength);
    bool WriteSHMEM(unsigned short nChannel, unsigned long nOffsetAddr, const void* pData, unsigned long nLength);
    System::IO::Stream^ ReadInitialHeapInfo(unsigned short nChannel);

  private:
    CISCTRDLL *m_pDLL;
  };
}
