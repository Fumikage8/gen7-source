// GFHIOHostDll.h

#pragma once


using namespace System;
using namespace System::Windows::Forms;

namespace GFHIOHostDll {

	public ref class GFHIO
	{
  public:
    enum class SIOAttr : int
    {
      SIOAttr_None    = DEVHIO_SIO_ATTR_NONE,   /**<  接続までブロック  */
      SIOAttr_NoWait  = DEVHIO_SIO_ATTR_NOWAIT  /**<  即時復帰          */
    };


    /*
      find
    */
    int Find(array<unsigned long>^% hDeviceIDArray, int elems)
    {
      int found = 0;

      {
        pin_ptr<unsigned long> pDeviceIDArray  = &hDeviceIDArray[0];
        found = m_pHIO->Find(pDeviceIDArray, elems);
        pDeviceIDArray  = nullptr;
      }

      return found;
    }

    /*
      connect
    */
    int Connect(unsigned long targetDeviceID)
    {
      return m_pHIO->Connect(targetDeviceID);
    }

    /*
      disconnect
    */
    int Disconnect()
    {
      return m_pHIO->Disconnect();
    }

    /************************************************************************/
    /**
      @retval 0:  接続していない
      @retval それ以外： 接続中のデバイスID
    */
    /************************************************************************/
    unsigned long GetConnectId()
    {
      return m_pHIO->GetConnectId();
    }

    /************************************************************************/
    /**
      @arg  [in]  channel チャンネル番号(0-DEVHIO_COMM_MAX_CHANNEL-1まで)  ※(DEVHIO_COMM_MAX_CHANNEL==15)
    */
    /************************************************************************/
    int OpenSio(int channel)
    {
      return m_pHIO->OpenSio(channel);
    }

    /************************************************************************/
    /**
    */
    /************************************************************************/
    int CloseSio(int channel)
    {
      return m_pHIO->CloseSio(channel);
    }

    /************************************************************************/
    /** 
    */
    /************************************************************************/
    int ConnectSio(int channel)
    {
      return m_pHIO->ConnectSio(channel);
    }

    /************************************************************************/
    /** 
    */
    /************************************************************************/
    int WaitSio(int channel, SIOAttr sioAttr)
    {
      return m_pHIO->WaitSio(channel, static_cast<int>(sioAttr));
    }

    int WaitSio(int channel)
    {
      return WaitSio(channel, SIOAttr::SIOAttr_None);
    }


    /************************************************************************/
    /** 
    */
    /************************************************************************/
    int DisconnectSio(int channel)
    {
      return m_pHIO->DisconnectSio(channel);
    }

    /************************************************************************/
    /** 
    */
    /************************************************************************/
    int FlushSio(int channel)
    {
      return m_pHIO->FlushSio(channel);
    }

    /************************************************************************/
    /** 
    */
    /************************************************************************/
    int ReadSio(int channel, array<byte>^% hBuffer, int dataBytes, SIOAttr sioAttr)
    {
      int result;

      {
        pin_ptr<byte> pBuffer  = &hBuffer[0];
        result = m_pHIO->ReadSio(channel, pBuffer, dataBytes, static_cast<int>(sioAttr));
        pBuffer = nullptr;
      }

      return result;
    }
    
    int ReadSio(int channel, array<byte>^% hBuffer, int dataBytes)
    {
      return ReadSio(channel, hBuffer, dataBytes, SIOAttr::SIOAttr_None);
    }


    /************************************************************************/
    /** 
    */
    /************************************************************************/
    int WriteSio(int channel, array<byte>^ hBuffer, int dataBytes, SIOAttr sioAttr)
    {
      int result;

      {
        pin_ptr<byte> pBuffer  = &hBuffer[0];
        result = m_pHIO->WriteSio(channel, pBuffer, dataBytes, static_cast<int>(sioAttr));
        pBuffer = nullptr;
      }

      return result;
    }
    
    int WriteSio(int channel, array<byte>^ hBuffer, int dataBytes)
    {
      return WriteSio(channel, hBuffer, dataBytes, SIOAttr::SIOAttr_None);
    }


    /************************************************************************/
    /** 
    */
    /************************************************************************/
    int GetReadableBytesSio(int channel, SIOAttr sioAttr)
    {
      return m_pHIO->GetReadableBytesSio(channel, static_cast<int>(sioAttr));
    }

    int GetReadableBytesSio(int channel)
    {
      return GetReadableBytesSio(channel, SIOAttr::SIOAttr_None);
    }

    /************************************************************************/
    /** 
    */
    /************************************************************************/
    int GetWritableBytesSio(int channel, SIOAttr sioAttr)
    {
      return m_pHIO->GetWritableBytesSio(channel, static_cast<int>(sioAttr));
    }

    int GetWritableBytesSio(int channel)
    {
      return GetWritableBytesSio(channel, SIOAttr::SIOAttr_None);
    }

    
    /************************************************************************/
    /** 
    */
    /************************************************************************/
    int OpenSmem(int channel)
    {
      return m_pHIO->OpenSmem(channel);
    }

    
    /************************************************************************/
    /** 
    */
    /************************************************************************/
    int CloseSmem(int channel)
    {
      return m_pHIO->CloseSmem(channel);
    }

    
    /************************************************************************/
    /** 
    */
    /************************************************************************/
    int GetSmemSize(int channel)
    {
      return m_pHIO->GetSmemSize(channel);
    }

    
    /************************************************************************/
    /** 
    */
    /************************************************************************/
    int ReadSmem(int channel, int readOffsetBytes, array<byte>^% hBuffer, int readBytes)
    {
      int result;

      {
        pin_ptr<byte> pBuffer = &hBuffer[0];
        result  = m_pHIO->ReadSmem(channel, readOffsetBytes, pBuffer, readBytes);
        pBuffer = nullptr;
      }

      return result;
    }

    
    /************************************************************************/
    /** 
    */
    /************************************************************************/
    int WriteSmem(int channel, int writeOffsetBytes, array<byte>^ hBuffer, int writeBytes)
    {
      int result;

      {
        pin_ptr<byte> pBuffer = &hBuffer[0];
        result  = m_pHIO->ReadSmem(channel, writeOffsetBytes, pBuffer, writeBytes);
        pBuffer = nullptr;
      }

      return result;
    }





    /*
      ctor
    */
    GFHIO()
      : m_pHIO(NULL)
    {
      AfxWinInit(GetModuleHandle(NULL), NULL, GetCommandLine(), 0);
      m_pHIO  = new CDEVHIO();
    }

    /*
      dtor
    */
    virtual ~GFHIO()
    {
      delete  m_pHIO;
    }

  protected:
    CDEVHIO*        m_pHIO;
	};
}
