
//==============================================================================
/**
 * @file   DeliveryUtilRetryHelper.cpp
 * @date   2015/07/06 Mon. 17:27:49
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <NetStatic/NetLib/include/NijiNetworkSystem.h>
#include  "./DeliveryUtilRetryHelper.h"


// =============================================================================
/**/
//==============================================================================
/*  local  */
namespace {
  enum
  {
    STATE_Initialize,
    STATE_Running,
    STATE_PrepareFinalize,
    STATE_Finalized,


    STATE_FIRST = STATE_Finalized,
    STATE_
  };
}


namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    ctor
  */
//------------------------------------------------------------------
DeliveryUtilRetryHelper::DeliveryUtilRetryHelper(DeliveryUtil::IRecvListener* pListener)
  : m_pListener(pListener)
  , m_State(STATE_FIRST)
  , m_ConnectionMode(DeliveryUtil::MODE_UDS)
  , m_bRetryEnable(true)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    dtor
  */
//------------------------------------------------------------------
DeliveryUtilRetryHelper::~DeliveryUtilRetryHelper()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  DeliveryUtilRetryHelper::Start(const DeliveryUtil::Mode connectionMode, const bool isRetryEnable)
{
  if(m_State  == STATE_Finalized)
  {
    m_ConnectionMode  = connectionMode;
    m_bRetryEnable    = isRetryEnable;
    m_State           = STATE_Initialize;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  DeliveryUtilRetryHelper::Update(void)
{
  DeliveryUtil*   pDeliv  = DeliveryUtil::GetInstance();
  
  switch(m_State)
  {
    case STATE_Initialize:
    {
      bool  isSuccess = false;

      if(CheckWirelessSwitchState())
      {
        if(pDeliv->Initialize(m_ConnectionMode))      /*  Deliveryを初期化  */
        {
//            GFL_PRINT("%s: StartRecv\n", __FUNCTION__);
          pDeliv->SetRecvListener(m_pListener);
          pDeliv->StartRecv();
          isSuccess = true;
          m_State   = STATE_Running;
        }
      }

      if(!isSuccess)
      {
        if(!m_bRetryEnable)
        {
          m_State = STATE_Finalized;        /*  自動復帰無効であればInitializeせずFinalizedへ  */
        }
      }
    }
      break;

    case STATE_Running:
      pDeliv->Update();
      /*  無線スイッチがOFFであるか、あるいはONであっても受信スレッドがKillされていたら終了  */
      if((!CheckWirelessSwitchState()) || (pDeliv->IsCompretePrepareFinalize()))
      {
        RequestFinalize(m_bRetryEnable);
      }
      break;

    case STATE_PrepareFinalize:
      if(pDeliv->IsCompretePrepareFinalize())
      {
        pDeliv->Finalize();
        m_State = m_bRetryEnable ? STATE_Initialize : STATE_Finalized;
      }
      else
      {
        pDeliv->Update();
      }
      break;

    case STATE_Finalized:
      break;

    default:
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    （エラー発生時など）Finalizeをリクエストする
  */
//------------------------------------------------------------------
void  DeliveryUtilRetryHelper::RequestFinalize(const bool isAutoInitializeEnable)
{
  if(m_State == STATE_Running)
  {
    DeliveryUtil::GetInstance()->PrepareFinalize();
    m_State = STATE_PrepareFinalize;
  }

  m_bRetryEnable  = isAutoInitializeEnable;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
DeliveryUtilRetryHelper::Status  DeliveryUtilRetryHelper::GetStatus(void) const
{
  DeliveryUtil*   pDeliv  = DeliveryUtil::GetInstance();

  if(m_State == STATE_Running)
  {
    return STATUS_Available;
  }
  else if((m_State == STATE_Initialize) && (!CheckWirelessSwitchState()))
  {
    return STATUS_NotAvailable_WirelessSwitchTurnedOff;
  }
  else if(m_State == STATE_Finalized)
  {
    return STATUS_Finalized;
  }


  return STATUS_NotAvailable;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  DeliveryUtilRetryHelper::SetSerialCode(const u32 serialCode)
{
  DeliveryUtil*   pDeliv  = DeliveryUtil::GetInstance();

  pDeliv->SetSerialCode(serialCode);
}


//------------------------------------------------------------------
/**
  *  @brief    
  *  @note  赤外線時は常にtrue
  */
//------------------------------------------------------------------
bool  DeliveryUtilRetryHelper::CheckWirelessSwitchState(void) const
{
  DeliveryUtil*   pDeliv  = DeliveryUtil::GetInstance();

  return ((m_ConnectionMode == DeliveryUtil::MODE_Ir) || pDeliv->GetWirelessSwitchState());
}






} /*  namespace Gift    */
} /*  namespace NetApp  */
