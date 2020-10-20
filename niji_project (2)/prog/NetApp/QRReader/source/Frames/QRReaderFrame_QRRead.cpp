
#if defined(GF_PLATFORM_CTR)

//==============================================================================
/**
 * @file   QRReaderFrame_QRRead.cpp
 * @date   2016/01/18 Mon. 15:27:40
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <pml/include/pokepara/pml_PokemonCoreParam.h>
#include  <DeviceCamera/include/gfl2_DeviceCamera.h>

#include  "System/include/GflUse.h"


#include  "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include  "NetStatic/NetAppLib/include/QR/QRUtility.h"
#include  "NetStatic/NetAppLib/include/QR/QRRecordAndDiary.h"

#include  "../System/QRReaderWork.h"
#include  "../QRReaderResource.h"

#include  "./QRReaderFrame_QRRead.h"
// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,

    STATE_WaitForReadyToCamera,

    STATE_QRDecodeStart,
    STATE_QRDecode,

    STATE_QRDecodeRetryStart,
    STATE_QRDecodeRetryPending,

    STATE_WaitForPokeFrameAnimEnd,
    STATE_ShowDataType,
    STATE_ShowResult,

    STATE_ChargeStart,
    STATE_Charging,
    STATE_Charged,
    STATE_ScanPowerTotal,
    STATE_ChargeMax,
    STATE_CannotCharge,

    STATE_CameraEndRequest,
    STATE_WaitForCameraEnd,

    STATE_Exit,

    STATE_     /*  terminator  */
  };

  static const u32  DeviceCameraImageW            = 512;
  static const u32  DeviceCameraImageH            = 384;
  static const u32  DeviceCameraImageBPP          = 2;
  static const u32  DeviceCameraImageBytes        = DeviceCameraImageW * DeviceCameraImageH * DeviceCameraImageBPP;
  static const u32  DeviceCameraImageDirtyFrame   = 15;


  /** QRDecoderに入力する画像のPixelサイズ  */
  static const u16  QRSourceImageSize[2]  =
                    {
                      DeviceCameraImageW, DeviceCameraImageH
                    };

  /** デコード対象とする矩形のサイズ  */
  static const u32  QRTargetImageSize[2]  =
                    {
//                      DeviceCameraImageW, DeviceCameraImageH,   /*  ←画面内全体  */
                      308, 308,                                 /*  240 * (512 / 400)   ※幅512の画像を幅400の画面にフィットさせている  */
                    };

  /** デコード対象とする矩形  */
  static const s32  QRTargetImageRect[4]  = {
                      (QRSourceImageSize[1] - QRTargetImageSize[1]) / 2,                          /*  top     */
                      (QRSourceImageSize[0] - QRTargetImageSize[0]) / 2,                          /*  left    */
                      (QRSourceImageSize[0] - QRTargetImageSize[0]) / 2 + QRTargetImageSize[0],   /*  right   */
                      (QRSourceImageSize[1] - QRTargetImageSize[1]) / 2 + QRTargetImageSize[1],   /*  bottom  */
                    };


  static const u32  EncoderHeapBytes    = 0x00400000;

  /**
    Encoderに渡すデータの最大サイズ、Encoderワーキングメモリ算出に使用する
    ライブラリが対応する最大のLV40、ECCLV-Lで2,953byteを想定
  */
  static const u32  EncoderDataSizeMax  = 4096;


  static const u32  ScanPowerChargeUnit       = 10;
  static const u32  ScanPowerChargeWaitFrames = 20;

  static const u32  CameraEndFrames           = 15;



  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  bool  IsPromotionalData(const NetApp::QR::QR_TYPE_APP dataType)
        {
          return  (false
            || (dataType == NetApp::QR::QR_TYPE_APP_ALOLA_NEW_OFFICIAL)
            || (dataType == NetApp::QR::QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL)
            || (dataType == NetApp::QR::QR_TYPE_APP_OFFICIAL)
            || (dataType == NetApp::QR::QR_TYPE_APP_ALOLA_NEW_OFFICIAL_MAGIANA)                   /**<  マギアナOK: 図鑑未登録/マギアナイベント終了前  */
            || (dataType == NetApp::QR::QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_MAGIANA)                /**<  マギアナOK: 図鑑登録済/マギアナイベント終了前  */
            || (dataType == NetApp::QR::QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_MAGIANA_AFTER_EVENT)    /**<  マギアナOK: 図鑑登録済/マギアナイベント終了後  */
            || (dataType == NetApp::QR::QR_TYPE_APP_ALOLA_NEW_OFFICIAL_M20PIKATYU)                /**<  M20ピカチュウOK: 図鑑未登録/M20ピカチュウイベント終了前  */
            || (dataType == NetApp::QR::QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_M20PIKATYU)             /**<  M20ピカチュウOK: 図鑑登録済/M20ピカチュウイベント終了前  */
            || (dataType == NetApp::QR::QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_M20PIKATYU_AFTER_EVENT) /**<  M20ピカチュウOK: 図鑑登録済/M20ピカチュウイベント終了後  */
          );
        }



} /*  namespace  */



namespace NetApp   {
namespace QRReader {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_QRRead::Frame_QRRead(QRReaderWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->m_pViewContainer)
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_IsCameraError(false)
  , m_IsCameraErrorInfoEnd(false)
  , m_ScanPower(0)
  , m_ScanPowerGain(0)
  , m_ScanPowerChargeWaitFrame(ScanPowerChargeWaitFrames)
  /*  QR  */
  , m_pEncoderHeap(NULL)
  , m_QREncoder()
  , m_QRDecoder()
  , m_pQRDecodeSource(NULL)
  , m_QRDecodeDirtyFrame(DeviceCameraImageDirtyFrame)
  , m_CameraEndFrameCount(CameraEndFrames)
  , m_IsDeviceCameraUpdate(true)
  , m_IsQRDecoderUpdate(true)
  , m_IsQRDecodeRequestEnable(false)
  , m_bFieldScanPowerFullChargedTrigger(false)
//  , m_bScanPowerChargeEOM(false)
  , m_IsPromotionalData(false)
  /*  view  */
  , m_pUpperView(NULL)
  , m_pUpperView_Pokemon(NULL)
  , m_pLowerView(NULL)
  , m_pMessageMenu(NULL)
  , m_ScanResultMode(MessageMenu::MODE_None)
{
}


//------------------------------------------------------------------
/**
  *  @func     Frame_QRRead
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_QRRead::~Frame_QRRead()
{
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  Frame_QRRead::startup()
{
  /*  view  */
  m_pUpperView          = m_pViewContainer->m_pUpperView_Main;
  m_pUpperView_Pokemon  = m_pViewContainer->m_pUpperView_Pokemon;
  m_pLowerView          = m_pViewContainer->m_pLowerView_Main;
  m_pMessageMenu        = m_pLowerView->GetMessageMenu();

  m_pViewContainer->SetScene(QRReaderViewContainer::SCENE_QRRead);

  m_pUpperView->SetBatteryGaugeChangeImmediately();
  m_pLowerView->RequestInputClean();    /*  ボタン類のリリースを要求  */

  m_pUpperView->SetViewMode(UpperView_Main::VIEWMODE_None);
  m_pUpperView->SetBatteryGaugeInvilibeImmediate();
  m_pUpperView_Pokemon->SetViewMode(UpperView_Pokemon::VIEWMODE_None);
  m_pLowerView->SetViewMode(LowerView_Main::VIEWMODE_None);
  m_pMessageMenu->Hide();

  /*  DeviceCamera  */
  {
    using namespace gfl2::devicecamera;

    DeviceCamera::Config  config;


//    config.pAppHeap           = m_Heap.GetDeviceHeap();
    config.pAppHeap           = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_NETWORK_RESIDENT );
    config.pDevHeap           = m_pWork->GetDeviceHeap();

    config.camType            = CAM_OUT_R;

    config.resolution.Set(DeviceCameraImageW, DeviceCameraImageH);
    config.resolution.SetCropCenter(640, 480);

    config.photoMode.type     = MODE_LETTER;

    config.trimming.isEnable  = false;


    config.exposure.isEnable    = true;
    config.exposure.value       = 2;
    config.exposure.rectUnit[0] = 0;
    config.exposure.rectUnit[1] = 0;
    config.exposure.rectUnit[2] = 16;
    config.exposure.rectUnit[3] = 16;


    config.whiteBalance.type        = WB_AUTO;
    config.whiteBalance.rectUnit[0] = 0;
    config.whiteBalance.rectUnit[1] = 0;
    config.whiteBalance.rectUnit[2] = 16;
    config.whiteBalance.rectUnit[3] = 16;

    config.sharpness.value      = 5;
    config.flip.type            = FLIP_NONE;
    config.frameRate.type       = FPS_30;
    config.effect.type          = EFFECT_NONE;
    config.contrast.type        = CONTRAST_HIGH;
    config.lensCorrection.type  = LC_NORMAL;
    config.noiseFilter.isEnable = false ;
    config.outImageSize[0]      = 512;
    config.outImageSize[1]      = 512;
    config.outputFormat         = Y2RThread::OUTPUT_RGB_24;
    config.alpha                = 0xff;


    GFL_NEW(m_pWork->GetDeviceHeap()) DeviceCamera();

    DeviceCamera* pDeviceCamera = GFL_SINGLETON_INSTANCE(DeviceCamera);

    pDeviceCamera->Setup(config);
    pDeviceCamera->Start();
  }

  /*  QR  */
  {
    gfl2::heap::HeapBase*     pHeapBase = m_pWork->GetDeviceHeap();

    /*  encoder  */
    {
      QRUtil::QREncoder::Config conf;
      
      /*  encoder heap  */
      m_pEncoderHeap = GFL_CREATE_LOCAL_HEAP(pHeapBase, EncoderHeapBytes, gfl2::heap::HEAP_TYPE_EXP, false);
      
      conf.pAppHeap     = m_pEncoderHeap;
      conf.pDevHeap     = m_pEncoderHeap;
      conf.srcDataSize  = EncoderDataSizeMax;
      conf.pixelPerCell = 2;
      conf.divideNum    = 1;
      
      m_QREncoder.Setup(conf);
    }

    /*  decoder  */
    {
      QRUtil::QRDecoder::Config config;
      
      config.pHeapBase              = pHeapBase;
      config.decodeImageSizeMax[0]  = DeviceCameraImageW;    /*  入力画像サイズ：カメラの出力画像のうち、有効部分  */
      config.decodeImageSizeMax[1]  = DeviceCameraImageH;
      
      
      m_QRDecoder.Setup(config);
      m_pQRDecodeSource = GflHeapAllocMemoryAlign(pHeapBase, DeviceCameraImageBytes, 32);
    }
  }

  /*  scanpower  */
  {
    m_ScanPower = GetQRReaderSaveData()->GetScanPower();
  }
  /*  record  */
  {
    m_pUpperView->SetScanCount(NetApp::QR::GetQRScanCount());
  }

  m_pMessageMenu->Show(MessageMenu::MODE_ActivateCameraInformation);
  return true;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_QRRead::setListener()
{
  m_pUpperView->SetEventHandler(this);
  m_pUpperView->SetQRScanPowerProvider(this);
  m_pLowerView->SetEventHandler(this);
  m_pLowerView->SetFieldScanPowerProvider(this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_QRRead::removeListener()
{
  m_pUpperView->SetEventHandler(NULL);
  m_pUpperView->SetQRScanPowerProvider(NULL);
  m_pUpperView->SetTargetPokemonCoreParam(NULL);
  m_pLowerView->SetEventHandler(NULL);
  m_pLowerView->SetFieldScanPowerProvider(NULL);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_QRRead::updateSequence()
{
  using namespace gfl2::devicecamera;
  DeviceCamera*   pDeviceCamera       = GFL_SINGLETON_INSTANCE(DeviceCamera);
  bool            isDeviceCameraReady = false;

  /*  battery  */
  {
    GetQRReaderSaveData()->BatteryQuery();
  }
  /*  scanpower  */
  {
    m_ScanPower = GetQRReaderSaveData()->GetScanPower();
  }

  /*  devicecamera  */
  if(m_IsDeviceCameraUpdate)
  {
    pDeviceCamera->Update();
    isDeviceCameraReady = pDeviceCamera->SetPicturePaneTexture(
                            m_pUpperView->GetCameraImagePane(),
                            Y2RThread::CH_1
                          );
  }

  /*  QR  */
  if(m_IsQRDecoderUpdate)
  {
    m_QRDecoder.Update();
  }

  /*  pokemodelUtil  */
  {
    m_pWork->m_pPokeModelUtil->SetFramePosition(m_pUpperView->GetPokeFramePane());
  }

  /*  record  */
  {
    m_pUpperView->SetScanCount(NetApp::QR::GetQRScanCount());
  }

  switch(m_State)
  {
    case  STATE_Entry:
      QRDecodeUpdateEnable(true, true);

//      m_pUpperView->SetBatteryGaugeVisibility(true);
      m_State = STATE_WaitForReadyToCamera;
      /*  fall through  */


    case  STATE_WaitForReadyToCamera:
    {
      if(isDeviceCameraReady || m_IsCameraErrorInfoEnd)
      {
        m_pViewContainer->SetAllInputEnable(true);
        m_pLowerView->RequestInputClean();    /*  ボタン類のリリースを要求  */

        m_pLowerView->SetViewMode(LowerView_Main::VIEWMODE_QRRead);
        m_State = STATE_QRDecodeStart;
      }
      else if(!m_IsCameraError && pDeviceCamera->IsError())
      {
        m_IsCameraError = true;
        m_pViewContainer->SetAllInputEnable(true);
        m_pLowerView->RequestInputClean();    /*  ボタン類のリリースを要求  */
        m_pMessageMenu->Show(MessageMenu::MODE_CameraBroken);
      }
    }
      break;

    /*
      decode
    */
    /*  デコード処理の開始  */
    case  STATE_QRDecodeStart:
    {
      const u8* pYUVImage  = pDeviceCamera->GetYUVImage(gfl2::devicecamera::CAM_OUT_R);

      if(m_IsQRDecodeRequestEnable)
      {
        m_pUpperView->SetViewMode(UpperView_Main::VIEWMODE_OnScan);
        m_pLowerView->SetViewMode(LowerView_Main::VIEWMODE_QRRead, LowerView_Main::VIEWMODE_SUB_QRReadOnScan);

        if(pYUVImage)
        {
          /*  cameraのReadBufferのSwapはUpdateからなのでcopyは排他不要  */
          memcpy(m_pQRDecodeSource, pYUVImage, DeviceCameraImageBytes);
          m_QRDecoder.StartDecode(
            m_pQRDecodeSource,
            QRSourceImageSize,
            QRTargetImageRect,
            true
          );
          m_State = STATE_QRDecode;
        }
      }
      else
      {
        m_pUpperView->SetViewMode(UpperView_Main::VIEWMODE_Camera);
        m_pLowerView->SetViewMode(LowerView_Main::VIEWMODE_QRRead, GetQRReaderSaveData()->IsBatteryEnoughForQRRead() ? LowerView_Main::VIEWMODE_SUB_None : LowerView_Main::VIEWMODE_SUB_QRReadLessPower);
      }
    }
      break;

    /*  デコード待ち  */
    case  STATE_QRDecode:
    {
      if(m_QRDecoder.IsEnd())
      {
        /*  処理が完了  */
        if(m_QRDecoder.IsSuccess())
        {
          /*  QRが認識され、データを取得できた  */
          /*  結果を分析  */
          AnalyzeDecodedBinary();

          /*  入力OFF  */
          m_pViewContainer->SetAllInputEnable(false);
        }
        else
        {
          /*  QR画像が認識できなければ即時再開  */
          m_State = STATE_QRDecodeStart;
        }
      }
    }
      break;

    /*
      retry
    */
    /*  一定時間後にデコード開始  */
    case  STATE_QRDecodeRetryStart:
      QRDecodeUpdateEnable(true, true);

//      m_pUpperView->SetViewMode(UpperView_Main::VIEWMODE_Camera);
      m_pLowerView->SetViewMode(LowerView_Main::VIEWMODE_QRRead);
//      m_pUpperView->SetBatteryGaugeVisibility(true);
      m_pUpperView_Pokemon->SetViewMode(UpperView_Pokemon::VIEWMODE_None);
      m_pMessageMenu->Show(MessageMenu::MODE_QRReadGuide);

      m_pWork->m_pPokeModelUtil->ReleaseRequest(0);
      m_pWork->m_pPokeModelUtil->ReleaseRequest(1);

      Sound::PlaySE(SEQ_SE_SYS_NJ_034);

      m_QRDecodeDirtyFrame  = DeviceCameraImageDirtyFrame;
      m_State               = STATE_QRDecodeRetryPending;
      /*  through  */

    case  STATE_QRDecodeRetryPending:
      if(m_QRDecodeDirtyFrame)
      {
        --m_QRDecodeDirtyFrame;
      }
      if(!m_QRDecodeDirtyFrame)
      {
        /*  入力ON  */
        m_pViewContainer->SetAllInputEnable(true);
        m_pLowerView->RequestInputClean();    /*  ボタン類のリリースを要求  */
        m_State = STATE_QRDecodeStart;
      }
      break;


    /*
      result
    */
    case  STATE_WaitForPokeFrameAnimEnd:
      break;


    case  STATE_ShowDataType:
      break;

    case  STATE_ShowResult:
      break;


    /*
      charge
    */
    case  STATE_ChargeStart:
    {
      const APP_PARAM::QRReadResult&  rReadResult       = m_pWork->m_pAppParam->m_QRReadResultInfo;
      Savedata::QRReaderSaveData*     pQRRederSaveData  = GetQRReaderSaveData();
     
      
      m_bFieldScanPowerFullChargedTrigger  = false;
      m_pLowerView->SetViewMode(LowerView_Main::VIEWMODE_FieldScan, LowerView_Main::VIEWMODE_SUB_FieldScan_Charge);
      
      /*  ゲージ出てくる  */
      Sound::PlaySE(SEQ_SE_SYS_NJ_033);
      
      if(m_IsPromotionalData)
      {
        /*  連動QR  */
        m_ScanPowerGain = Savedata::QRReaderSaveData::QRREAD_SCANPOWER_GAIN_VALUE_PROMOTIONAL;
      }
      else
      {
        /*  通常  */
        m_ScanPowerGain = Savedata::QRReaderSaveData::QRREAD_SCANPOWER_GAIN_VALUE_NORMAL;
      }
      
      /*  遷移  */
      if(GetQRReaderSaveData()->IsScanPowerFull())
      {
        /*  既に100%  */
        m_pMessageMenu->Show(MessageMenu::MODE_ScanPowerAlreadyFull);
        m_State = STATE_ChargeMax;
      }
      else
      {
        /*  チャージ演出へ  */
        m_ScanPowerChargeWaitFrame  = ScanPowerChargeWaitFrames;
        m_pMessageMenu->Hide();
        m_State                     = STATE_Charging;
      }
    }
      /*  through */

    case  STATE_Charging:
      if(m_ScanPowerGain)
      {
        /*  増分が残っている  */
        
        if(m_ScanPowerChargeWaitFrame)
        {
          --m_ScanPowerChargeWaitFrame;
        }

        if(!m_ScanPowerChargeWaitFrame)
        {
          u32 scanPowerGainCurrent = m_ScanPowerGain;
          
          if(ScanPowerChargeUnit < scanPowerGainCurrent)
          {
            scanPowerGainCurrent = ScanPowerChargeUnit;
          }
          m_ScanPowerGain -= scanPowerGainCurrent;

          {
            const u8  oldScanPower  = m_ScanPower;
  
            GetQRReaderSaveData()->ChargeScanPower(scanPowerGainCurrent);   /*  ScanPower加算  */

            m_ScanPower = GetQRReaderSaveData()->GetScanPower();
            m_bFieldScanPowerFullChargedTrigger  = ((oldScanPower != m_ScanPower) && (GetQRReaderSaveData()->IsScanPowerFull()));
          }

          if(GetQRReaderSaveData()->IsScanPowerFull())
          {
            /*  full  */
            m_ScanPowerGain = 0;
            Sound::PlaySE(SEQ_SE_SYS_NJ_032);
          }
          else
          {
            /*  charge  */
            m_ScanPowerChargeWaitFrame  = ScanPowerChargeWaitFrames;
            Sound::PlaySE(SEQ_SE_SYS_NJ_027);
          }
        }
      }
      else
      {
        /*  全て反映した  */
        
        if(m_ScanPowerChargeWaitFrame)
        {
          --m_ScanPowerChargeWaitFrame;
        }
        
        
        if(!m_ScanPowerChargeWaitFrame)
        {
          /*  n%チャージした  */
          const u32 powerGain = m_IsPromotionalData ? Savedata::QRReaderSaveData::QRREAD_SCANPOWER_GAIN_VALUE_PROMOTIONAL : Savedata::QRReaderSaveData::QRREAD_SCANPOWER_GAIN_VALUE_NORMAL;
          
          m_pMessageMenu->SetScanPowerGainValue(powerGain);
          m_pMessageMenu->Show(MessageMenu::MODE_Charged);
          
          m_State = STATE_Charged;
        }
      }
      break;

    case  STATE_Charged:
      break;

    case  STATE_ScanPowerTotal:
      break;

    case  STATE_ChargeMax:
      /*  nop  */
      break;

    case  STATE_CannotCharge:
      /*  nop  */
      break;


    case  STATE_CameraEndRequest:
      QRDecodeUpdateEnable(false, false);
      m_pUpperView->SetViewMode(UpperView_Main::VIEWMODE_None);
      m_pLowerView->SetViewMode(LowerView_Main::VIEWMODE_None);
      m_pMessageMenu->Show(MessageMenu::MODE_InactivateCameraInformation);
      m_CameraEndFrameCount = CameraEndFrames;
      m_State               = STATE_WaitForCameraEnd;
      break;

    case  STATE_WaitForCameraEnd:
    {
      if(m_CameraEndFrameCount) --m_CameraEndFrameCount;
      
      if(!m_CameraEndFrameCount)
      {
        pDeviceCamera->Cleanup();
        m_State = STATE_Exit;
      }
    }
      break;

    case  STATE_Exit:
      m_pWork->SetFrameResult( m_Result );
      break;
  }

  m_IsQRDecodeRequestEnable = false;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  Frame_QRRead::cleanup()
{
  bool  bAllDone = false;

  /*  QR  */
  {
    /*  encoder  */
    m_QREncoder.Cleanup();
    gfl2::heap::Manager::DeleteHeap(m_pEncoderHeap);

    /*  decoder  */
    m_QRDecoder.Cleanup();
    GflHeapFreeMemory(m_pQRDecodeSource);
    m_pQRDecodeSource = NULL;
  }

  /*  DeviceCamera  */
  {
    using namespace gfl2::devicecamera;

    DeviceCamera*   pDeviceCamera  = GFL_SINGLETON_INSTANCE(DeviceCamera);

    pDeviceCamera->Cleanup();
    GFL_SAFE_DELETE(pDeviceCamera);
  }

  bAllDone  = true;


  return  bAllDone;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
u8    Frame_QRRead::QRScanPowerProvider_OnQueryPower(void)
{
  return GetQRReaderSaveData()->GetBattery();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_QRRead::UpperView_Main_OnEvent(const UpperView_Main::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  UpperView_Main::IEventHandler::EVENT_PokeFrameAnimEnd:
      if(m_State == STATE_WaitForPokeFrameAnimEnd)
      {
        ShowDataTypeMessage();
      }
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_QRRead::ShowDataTypeMessage(void)
{
  switch(m_pWork->m_pAppParam->m_QRReadResultInfo.dataType)
  {
    case  NetApp::QR::QR_TYPE_APP_NORMAL:                   //アローラ外（一般QR）
      /** ミラクルQRだ  */
      m_pMessageMenu->Show(MessageMenu::MODE_Found_NormalData);
      m_State = STATE_ShowDataType;
      break;


    case  NetApp::QR::QR_TYPE_APP_ALOLA_NEW:                //アローラ新規
    case  NetApp::QR::QR_TYPE_APP_ALOLA_UPDATE:             //アローラ更新
      /*  図鑑QR  */
      m_pMessageMenu->Show(MessageMenu::MODE_Found_PokedexData);
      m_State = STATE_ShowDataType;
      break;

    case  NetApp::QR::QR_TYPE_APP_ALOLA_NEW_OFFICIAL:                           //アローラ新規（20％チャージ）
    case  NetApp::QR::QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL:                        //アローラ更新（20％チャージ）
    case  NetApp::QR::QR_TYPE_APP_ALOLA_NEW_OFFICIAL_MAGIANA:                   /**<  マギアナOK: 図鑑未登録/マギアナイベント終了前  */
    case  NetApp::QR::QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_MAGIANA:                /**<  マギアナOK: 図鑑登録済/マギアナイベント終了前  */
    case  NetApp::QR::QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_MAGIANA_AFTER_EVENT:    /**<  マギアナOK: 図鑑登録済/マギアナイベント終了後  */
    case  NetApp::QR::QR_TYPE_APP_ALOLA_NEW_OFFICIAL_M20PIKATYU:                /**<  M20ピカチュウOK: 図鑑未登録/M20ピカチュウイベント終了前  */
    case  NetApp::QR::QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_M20PIKATYU:             /**<  M20ピカチュウOK: 図鑑登録済/M20ピカチュウイベント終了前  */
    case  NetApp::QR::QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_M20PIKATYU_AFTER_EVENT: /**<  M20ピカチュウOK: 図鑑登録済/M20ピカチュウイベント終了後  */
    case  NetApp::QR::QR_TYPE_APP_OFFICIAL:                                     //アローラ外（公式QR）
      /*  連動QR  */
      m_pMessageMenu->Show(MessageMenu::MODE_Found_PromotionalData);
      m_State = STATE_ShowDataType;
      break;

    case  NetApp::QR::QR_TYPE_APP_ROM_NG:                   //ROMコードがNG
    case  NetApp::QR::QR_TYPE_APP_REGION_NG:                //リージョンがNG
    case  NetApp::QR::QR_TYPE_APP_MAGIANA_NG:               //マギアナQRまだ読めない
    case  NetApp::QR::QR_TYPE_APP_M20PIKATYU_NG:            //M20PikatyuQRまだ読めない
    default:
      m_pMessageMenu->Show(m_ScanResultMode);
      m_State = STATE_ShowResult;
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_QRRead::LowerView_Main_OnEvent(const LowerView_Main::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Main::IEventHandler::EVENT_EOM:
      if(m_State == STATE_ShowResult)
      {
        const APP_PARAM::QRReadResult&  rReadResult = m_pWork->m_pAppParam->m_QRReadResultInfo;

        if(false
          || (rReadResult.dataType == NetApp::QR::QR_TYPE_APP_ALOLA_NEW)
          || (rReadResult.dataType == NetApp::QR::QR_TYPE_APP_ALOLA_NEW_OFFICIAL)
          || (rReadResult.dataType == NetApp::QR::QR_TYPE_APP_ALOLA_UPDATE)
          || (rReadResult.dataType == NetApp::QR::QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL)
          || (rReadResult.dataType == NetApp::QR::QR_TYPE_APP_OFFICIAL)
          || (rReadResult.dataType == NetApp::QR::QR_TYPE_APP_NORMAL)
          || (rReadResult.dataType == NetApp::QR::QR_TYPE_APP_ALOLA_NEW_OFFICIAL_MAGIANA)
          || (rReadResult.dataType == NetApp::QR::QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_MAGIANA)
          || (rReadResult.dataType == NetApp::QR::QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_MAGIANA_AFTER_EVENT)
          || (rReadResult.dataType == NetApp::QR::QR_TYPE_APP_ALOLA_NEW_OFFICIAL_M20PIKATYU)
          || (rReadResult.dataType == NetApp::QR::QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_M20PIKATYU)
          || (rReadResult.dataType == NetApp::QR::QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_M20PIKATYU_AFTER_EVENT)
        )
        {
          /*
            2016/03/28  Yu Muto
            FieldScan発動中も先行してパワーをためられるようにとの要望
          */
#if 0
          if(GetQRReaderSaveData()->IsFieldScanActive())
          {
            m_pMessageMenu->Show(MessageMenu::MODE_CannotCharge);
            m_State = STATE_CannotCharge;
          }
          else
#endif
          {
            /*  スキャンパワーチャージ  */
            m_State = STATE_ChargeStart;
          }
        }
        else
        {
          m_State = STATE_QRDecodeRetryStart;
        }
      }
      else if(m_State == STATE_ShowDataType)
      {
        m_pMessageMenu->Show(m_ScanResultMode);
        m_State = STATE_ShowResult;
      }

      else if(m_State == STATE_Charging)
      {
      }

      else if(m_State == STATE_Charged)
      {
        if(m_bFieldScanPowerFullChargedTrigger)
        {
          /*  100%になった  */
          m_pMessageMenu->Show(MessageMenu::MODE_ScanPowerFullCharged);
          m_bFieldScanPowerFullChargedTrigger  = false;
          m_State                              = STATE_ChargeMax;
        }
        else if(GetQRReaderSaveData()->IsScanPowerFull())
        {
          /*  既に100%  */
          m_pMessageMenu->Show(MessageMenu::MODE_ScanPowerAlreadyFull);
          m_State                              = STATE_ChargeMax;
        }
        else
        {
          m_pMessageMenu->SetScanPowerGainValue(GetQRReaderSaveData()->GetScanPower());
          m_pMessageMenu->Show(MessageMenu::MODE_ScanPowerTotal);
          m_State = STATE_ScanPowerTotal;
        }
      }

      else if(m_State == STATE_ScanPowerTotal)
      {
        m_State = STATE_QRDecodeRetryStart;
      }

      else if(m_State == STATE_ChargeMax)
      {
        m_State = STATE_QRDecodeRetryStart;
      }

      else if(m_State == STATE_CannotCharge)
      {
        m_State = STATE_QRDecodeRetryStart;
      }
      
      else if(m_State == STATE_WaitForReadyToCamera)
      {
        /*  故障メッセージ終了  */
        m_pViewContainer->SetAllInputEnable(false);
        m_IsCameraErrorInfoEnd  = true;
      }
      
      break;

    case  LowerView_Main::IEventHandler::EVENT_MessageFinish:
      break;

    case  LowerView_Main::IEventHandler::EVENT_Back:
      ExitFrame();
      break;

    case  LowerView_Main::IEventHandler::EVENT_QRRead:
      break;

    case  LowerView_Main::IEventHandler::EVENT_FieldScan:
      break;

    case  LowerView_Main::IEventHandler::EVENT_ModeChange:
      Sound::PlaySE(SEQ_SE_SYS_NJ_042);
      ExitFrame(RESULT_QRRead_FieldScan);
      break;

    case  LowerView_Main::IEventHandler::EVENT_Information:
      m_pLowerView->ShowInformation();
      break;

    case LowerView_Main::IEventHandler::EVENT_QRScanButtonHolding:
      m_IsQRDecodeRequestEnable = true;
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_QRRead::ExitFrame(const FrameResult frameResult)
{
  m_Result  = frameResult;
//  m_State   = STATE_Exit;
  m_State   = STATE_CameraEndRequest;
  m_pViewContainer->SetAllInputEnable(false);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_QRRead::AnalyzeDecodedBinary(void)
{
  gfl2::heap::HeapBase*     pHeapBase             = m_pWork->GetDeviceHeap()->GetLowerHandle();
  const void*               pData                 = NULL;
  u32                       dataBytes             = 0;
  NetApp::QR::QR_TYPE_APP   dataType;
  pml::pokepara::CoreParam* pPokeCore             = GFL_NEW(pHeapBase) pml::pokepara::CoreParam(pHeapBase);
  bool                      isRegisteredData      = false;
  bool                      isSameFormBothGender  = false;
  
  m_pLowerView->SetViewMode(LowerView_Main::VIEWMODE_None);

  /* 履歴照合  */
  pData             = m_QRDecoder.GetDecodeData(&dataBytes);
  isRegisteredData  = GetQRReaderSaveData()->IsRegisteredData(m_pWork->GetDeviceHeap()->GetLowerHandle(), pData, dataBytes);

  if(isRegisteredData)
  {
    dataType  = NetApp::QR::QR_TYPE_APP_INVALID;
  }
  else
  {
    /*  解析  */
    
    dataType  = NetApp::QR::QRUtility::AnalyzeQRBinaryForApp(m_pWork->GetDeviceHeap(), pData, dataBytes, pPokeCore, isSameFormBothGender);
  }


  switch(dataType)
  {
    case  NetApp::QR::QR_TYPE_APP_ALOLA_NEW:                //アローラ新規
    case  NetApp::QR::QR_TYPE_APP_ALOLA_NEW_OFFICIAL:       //アローラ新規（20％チャージ）
      GetQRReaderSaveData()->RegisterScannedDataHistory(m_pWork->GetDeviceHeap()->GetLowerHandle(), pData, dataBytes);
      m_ScanResultMode  = MessageMenu::MODE_Found_Pokemon_Alola;
      GetQRReaderSaveData()->UseBatteryOnQRScan();
      break;

    case  NetApp::QR::QR_TYPE_APP_ALOLA_UPDATE:             //アローラ更新
    case  NetApp::QR::QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL:    //アローラ更新（20％チャージ）
      GetQRReaderSaveData()->RegisterScannedDataHistory(m_pWork->GetDeviceHeap()->GetLowerHandle(), pData, dataBytes);
      m_ScanResultMode  = MessageMenu::MODE_Found_Pokemon_AlreadyIdentified;
      GetQRReaderSaveData()->UseBatteryOnQRScan();
      break;

    case  NetApp::QR::QR_TYPE_APP_OFFICIAL:                 //アローラ外（公式QR）
    case  NetApp::QR::QR_TYPE_APP_NORMAL:                   //アローラ外（一般QR）
      GetQRReaderSaveData()->RegisterScannedDataHistory(m_pWork->GetDeviceHeap()->GetLowerHandle(), pData, dataBytes);
      m_ScanResultMode  = MessageMenu::MODE_Found_Pokemon_NotAlola;
      GetQRReaderSaveData()->UseBatteryOnQRScan();
      break;

    /*  マギアナ関連成功  */
    case  NetApp::QR::QR_TYPE_APP_ALOLA_NEW_OFFICIAL_MAGIANA:                   /**<  マギアナOK: 図鑑未登録/マギアナイベント終了前  */
      GetQRReaderSaveData()->RegisterScannedDataHistory(m_pWork->GetDeviceHeap()->GetLowerHandle(), pData, dataBytes);
      m_ScanResultMode  = MessageMenu::MODE_Found_Pokemon_MagianaNew;
      GetQRReaderSaveData()->UseBatteryOnQRScan();
      break;

    case  NetApp::QR::QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_MAGIANA:                /**<  マギアナOK: 図鑑登録済/マギアナイベント終了前  */
      GetQRReaderSaveData()->RegisterScannedDataHistory(m_pWork->GetDeviceHeap()->GetLowerHandle(), pData, dataBytes);
      m_ScanResultMode  = MessageMenu::MODE_Found_Pokemon_MagianaUpdate;
      GetQRReaderSaveData()->UseBatteryOnQRScan();
      break;

    case  NetApp::QR::QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_MAGIANA_AFTER_EVENT:    /**<  マギアナOK: 図鑑登録済/マギアナイベント終了後  */
      GetQRReaderSaveData()->RegisterScannedDataHistory(m_pWork->GetDeviceHeap()->GetLowerHandle(), pData, dataBytes);
      m_ScanResultMode  = MessageMenu::MODE_Found_Pokemon_MagianaUpdateAfterEvent;
      GetQRReaderSaveData()->UseBatteryOnQRScan();
      break;

    /*  M20ピカチュウ関連成功  */
    case  NetApp::QR::QR_TYPE_APP_ALOLA_NEW_OFFICIAL_M20PIKATYU:                /**<  M20ピカチュウOK: 図鑑未登録/M20ピカチュウイベント終了前  */
      GetQRReaderSaveData()->RegisterScannedDataHistory(m_pWork->GetDeviceHeap()->GetLowerHandle(), pData, dataBytes);
      m_ScanResultMode  = MessageMenu::MODE_Found_Pokemon_M20PikatyuNew;
      GetQRReaderSaveData()->UseBatteryOnQRScan();
      break;

    case  NetApp::QR::QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_M20PIKATYU:             /**<  M20ピカチュウOK: 図鑑登録済/M20ピカチュウイベント終了前  */
      GetQRReaderSaveData()->RegisterScannedDataHistory(m_pWork->GetDeviceHeap()->GetLowerHandle(), pData, dataBytes);
      m_ScanResultMode  = MessageMenu::MODE_Found_Pokemon_M20PikatyuUpdate;
      GetQRReaderSaveData()->UseBatteryOnQRScan();
      break;

    case  NetApp::QR::QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_M20PIKATYU_AFTER_EVENT: /**<  M20ピカチュウOK: 図鑑登録済/M20ピカチュウイベント終了後  */
      GetQRReaderSaveData()->RegisterScannedDataHistory(m_pWork->GetDeviceHeap()->GetLowerHandle(), pData, dataBytes);
      m_ScanResultMode  = MessageMenu::MODE_Found_Pokemon_M20PikatyuUpdateAfterEvent;
      GetQRReaderSaveData()->UseBatteryOnQRScan();
      break;

    /*
      NG系
    */
    case  NetApp::QR::QR_TYPE_APP_ROM_NG:         //ROMコードがNG
      GFL_PRINT("%s: NG, ROM code\n", __FUNCTION__);

      pData     = NULL;
      dataBytes = 0;
      m_ScanResultMode  = MessageMenu::MODE_DataNotAvailable_App;
      break;

    case NetApp::QR::QR_TYPE_APP_REGION_NG:       //リージョンがNG
      GFL_PRINT("%s: NG, region code\n", __FUNCTION__);

      pData     = NULL;
      dataBytes = 0;
      m_ScanResultMode  = MessageMenu::MODE_DataNotAvailable_Console;
      break;

    case  NetApp::QR::QR_TYPE_APP_MAGIANA_NG:     //マギアナQRまだ読めない
      GFL_PRINT("%s: NG, magiana\n", __FUNCTION__);

      pData     = NULL;
      dataBytes = 0;
      m_ScanResultMode  = MessageMenu::MODE_Found_Pokemon_CannotAnalyze;
      break;

    case  NetApp::QR::QR_TYPE_APP_M20PIKATYU_NG:  //M20PikatyuQRまだ読めない
      GFL_PRINT("%s: NG, m20pikatyu\n", __FUNCTION__);

      pData     = NULL;
      dataBytes = 0;
      m_ScanResultMode  = MessageMenu::MODE_Found_M20Pikatyu_CannotAnalyze;
      break;
      

    default:
      GFL_PRINT("%s: invalid data\n", __FUNCTION__);

      pData     = NULL;
      dataBytes = 0;
      m_ScanResultMode  = isRegisteredData ? MessageMenu::MODE_DataIsAlreadyRead : MessageMenu::MODE_DataNotAvailable_App;
      break;
  }


  /*  結果保存  */
  QRDecodeUpdateEnable(false, false);
  m_pWork->m_pAppParam->m_QRReadResultInfo.SetData(dataType, pData, dataBytes);
  m_IsPromotionalData = IsPromotionalData(dataType);
  m_State             = STATE_WaitForPokeFrameAnimEnd;

  {
    if(pPokeCore->GetMonsNo() == MONSNO_NULL)
    {
      /*  NG  */
      Sound::PlaySE(SEQ_SE_SYS_NJ_026);
      UpperView_Main_OnEvent(UpperView_Main::IEventHandler::EVENT_PokeFrameAnimEnd);
    }
    else
    {
      /*  OK  */
      PokeTool::SimpleParam simpleParam;

      PokeTool::GetSimpleParam(&simpleParam, pPokeCore);
      m_pWork->m_pPokeModelUtil->RequestEx(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, 0, simpleParam);
      m_pUpperView->SetViewMode(UpperView_Main::VIEWMODE_PokeFrame);
      m_pUpperView->SetTargetPokemonCoreParam(pPokeCore);
      m_pUpperView_Pokemon->ShowQRReadResult(pPokeCore, isSameFormBothGender);
      m_pMessageMenu->Show(MessageMenu::MODE_QRAnalyizing);


      /*  演出用QR  */
      m_QREncoder.Encode(
        dataBytes,
        pData,
        QRUtil::QREncoder::QRVER_AUTO,
        1,      /*  qrDivTotal    */
        1,      /*  pixelPerCell  */
        QRUtil::QREncoder::ECCLV_L
      );
      
      m_QREncoder.BuildQRTexture();
      m_QREncoder.AssignTexture(m_pUpperView->GetQRImagePane());

      Sound::PlaySE(SEQ_SE_SYS_NJ_025);
    }
  }

  /*
    record/diary処理
    OK/NGは関数内でdataTypeにより判定している
  */
  {
    NetApp::QR::QRRecordFunc(dataType);
    NetApp::QR::QRDiaryFunc(dataType);
  }

  GFL_SAFE_DELETE(pPokeCore);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
u32   Frame_QRRead::IFieldScanPowerProvider_OnQueryPower(bool& rIsFull, bool& rIsActive)
{
  rIsFull   = GetQRReaderSaveData()->IsScanPowerFull();
  rIsActive = GetQRReaderSaveData()->IsFieldScanActive();
  
  return m_ScanPower;
}





} /*  namespace QRReader */
} /*  namespace NetApp   */


#endif  /* #if defined(GF_PLATFORM_CTR)  */
