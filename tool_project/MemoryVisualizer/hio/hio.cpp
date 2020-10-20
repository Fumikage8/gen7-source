// これは メイン DLL ファイルです。

#include "stdafx.h"
#include <atlstr.h>

#include "hio.h"

namespace hio {

  ISCTRDLL::ISCTRDLL()
  {
    m_pDLL = new CISCTRDLL();
  }

  ISCTRDLL::~ISCTRDLL()
  {
    this->!ISCTRDLL();
  }

  ISCTRDLL::!ISCTRDLL()
  {
    if(m_pDLL)
    {
      delete m_pDLL;
    }
  }

  bool ISCTRDLL::IsDLLLoaded(void)
  {
    return m_pDLL->IsDLLLoaded();
  }

  bool ISCTRDLL::Open(void)
  {
    return m_pDLL->Open();
  }

  void ISCTRDLL::Close(void)
  {
    m_pDLL->Close();
  }

  bool ISCTRDLL::IsOpened(void)
  {
    return m_pDLL->IsOpened();
  }

  bool ISCTRDLL::CheckConnect(void)
  {
    return m_pDLL->CheckConnect();
  }

  bool ISCTRDLL::DownloadGo(System::String^ pStrFileName)
  {
    pin_ptr<const wchar_t> wch = PtrToStringChars(pStrFileName);

    size_t convertedChars = 0;
    size_t sizeInBytes = ((pStrFileName->Length + 1) * 2);
    errno_t err = 0;
    char* ch = (char*)malloc(sizeInBytes);

    err = wcstombs_s(&convertedChars, ch, sizeInBytes, wch, sizeInBytes);

    return m_pDLL->DownloadGo(ch);
  }

  bool ISCTRDLL::EnableHIO(void)
  {
    return m_pDLL->EnableHIO();
  }

  bool ISCTRDLL::DisableHIO(void)
  {
    return m_pDLL->DisableHIO();
  }

  bool ISCTRDLL::OpenSHMEM(unsigned short nChannel)
  {
    return m_pDLL->OpenSHMEM(nChannel);
  }

  void ISCTRDLL::CloseSHMEM(unsigned short nChannel)
  {
    return m_pDLL->CloseSHMEM(nChannel);
  }

  bool ISCTRDLL::ReadSHMEM(unsigned short nChannel, unsigned long nOffsetAddr, void* pData, unsigned long nLength)
  {
    return m_pDLL->ReadSHMEM(nChannel, nOffsetAddr, pData, nLength);
  }

  bool ISCTRDLL::WriteSHMEM(unsigned short nChannel, unsigned long nOffsetAddr, const void* pData, unsigned long nLength)
  {
    return m_pDLL->WriteSHMEM(nChannel, nOffsetAddr, pData, nLength);
  }

  System::IO::Stream^ ISCTRDLL::ReadInitialHeapInfo(unsigned short nChannel)
  {
    System::Text::StringBuilder^ builder = gcnew System::Text::StringBuilder();

    char buf[100];

    while(true)
    {
      if(!ReadSHMEM(nChannel, 0, buf, 100))
      {
        return gcnew System::IO::MemoryStream(Text::Encoding::UTF8->GetBytes(builder->ToString()));
      }

      if(strcmp(buf, "OK") == 0)
      {
        continue;
      }

      builder->Append(gcnew String(buf));
      WriteSHMEM(nChannel, 0, "OK", 100);

      if (System::String(buf).Contains("CTRExpHeap Dump END"))
      {
        break;
      }
    }

    return gcnew System::IO::MemoryStream(Text::Encoding::UTF8->GetBytes(builder->ToString()));
  }
}
