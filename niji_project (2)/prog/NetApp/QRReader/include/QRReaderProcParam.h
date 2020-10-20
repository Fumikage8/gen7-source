#if !defined(QRREADERPROCPARAM_H_INCLUDED)
#define QRREADERPROCPARAM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   QRReaderProcParam.h
 * @date   2016/01/14 Thu. 11:24:01
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>

#include  "NetStatic/NetAppLib/include/QR/QRUtility.h"


namespace NetApp    {
namespace QRReader  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/01/14 Thu. 11:24:39
  */
//==============================================================================
typedef struct APP_PARAM
{
  struct  QRReadResult
  {
    static const u32  DataBytesMax  = 1024;
    
    u8                        data[DataBytesMax];
    u32                       dataBytes;
    NetApp::QR::QR_TYPE_APP   dataType;
    
    /** ctor  */
    QRReadResult(void)
      : dataBytes(0)
      , dataType(NetApp::QR::QR_TYPE_APP_INVALID)
    {
    }
    
    /** setdata  */
    void  SetData(const NetApp::QR::QR_TYPE_APP _dataType = NetApp::QR::QR_TYPE_APP_INVALID, const void* pData = NULL, const u32 _dataBytes = 0)
    {
      dataType  = _dataType;

      if(pData && ((_dataBytes) && (_dataBytes <= DataBytesMax)))
      {
        dataBytes = _dataBytes;
        memcpy(data, pData, dataBytes);
      }
      else
      {
        dataBytes = 0;
      }
    }
  }m_QRReadResultInfo;    /**<  [out] QR読み取り結果情報群  */



  enum  BootMode
  {
    BOOTMODE_QRScan,
    BOOTMODE_FriendMatch,
    BOOTMODE_LiveMatch,
    BOOTMODE_BattleTeam,

    BOOTMODE_
  };

  BootMode  m_BootMode;

  /*  qrsimple  */
  Regulation*       pDst_Regulation;    /**<  [out] FriendMatch/LiveMatch時出力  */
  pml::PokeParty*   pDst_PokeParty;     /**<  [out] TeamSelect時出力             */
  bool              m_QRSimpleReadDecoded;




  /*
    ctor
  */
  APP_PARAM(void)
    : m_QRReadResultInfo()
    , m_BootMode(BOOTMODE_QRScan)
    , pDst_Regulation(NULL)
    , pDst_PokeParty(NULL)
    , m_QRSimpleReadDecoded(false)
  {}
}APP_PARAM;



} /*  namespace QRReader  */
} /*  namespace NetApp    */
#endif  /*  #if !defined(QRREADERPROCPARAM_H_INCLUDED)  */
