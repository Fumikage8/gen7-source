//==============================================================================
/**
 * @file   DecoItem.cpp
 * @date   2017/02/14 Tue. 20:14:49
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <arc_def_index/arc_def.h>
#include  <Layout/include/gfl2_Layout.h>

#include  <App/FinderStudioStatic/include/FinderStudioDecoObject.h>
#include  <App/FinderStudioStatic/include/FinderStudioDecoDataAccessor.h>

#include  "./DecoItem.h"

namespace App {
namespace FinderStudio {
namespace Viewer {
namespace Deco {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_DecoItem
namespace FILELOCAL {
  static const u32  arcid = ARCID_DECOVIEW;
}


// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
DecoItem::DecoItem(void)
 :  m_State(STATE_Initial)
 ,  m_pUtilHeap(NULL)
 ,  m_Loader()
 ,  m_LoadRequestInfo()
 ,  m_StampData()
 ,  m_pParentPane(NULL)
 ,  m_pPicture(NULL)
 ,  m_TexInfo()
 ,  m_bReleaseRequest(false)
 ,  m_IsFrameData(false)
{
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
DecoItem::~DecoItem()
{
  GFL_ASSERT((m_State == STATE_Initial) || (m_State == STATE_Empty));
}

//------------------------------------------------------------------
/**
  *  @param    
  */
//------------------------------------------------------------------
void  DecoItem::Setup(app::util::Heap* pUtilHeap, gfl2::lyt::LytPane* pParentPane)
{
  const bool  bValidState = (GetState() == STATE_Initial);
  const bool  bValidParam = (!!pParentPane);
  
  GFL_ASSERT(bValidState);
  GFL_ASSERT(bValidParam);
  if((!bValidState) || (!bValidParam))  return;

  m_pUtilHeap   = pUtilHeap;
  m_pParentPane  = pParentPane;

  /*  next state  */
  m_State       = STATE_Empty;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DecoItem::Request(const App::FinderStudio::Static::StampData& rStampData, const bool isFrameData)
{
  if( ( !isFrameData ) && ( !App::FinderStudio::Static::IsAvailableDecoID( rStampData ) ) ) return;
  if( ( isFrameData ) && ( !App::FinderStudio::Static::IsAvailableFrameID( rStampData ) ) ) return;

  m_LoadRequestInfo.stampData   = rStampData;
  m_LoadRequestInfo.isFrameData = isFrameData;
  m_LoadRequestInfo.isAvailable = true;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
DecoItem::State DecoItem::Update(void)
{
  switch(m_State)
  {
    case  STATE_Initial:
      /*  nop  */
      break;

    case  STATE_Empty:
      m_bReleaseRequest = false;
      if(m_LoadRequestInfo.isAvailable)
      {
        m_StampData     = m_LoadRequestInfo.stampData;
        m_IsFrameData   = m_LoadRequestInfo.isFrameData;
        {
          if( m_IsFrameData ) m_Loader.RequestLoad( m_pUtilHeap, FILELOCAL::arcid, Static::DecoDataAccessor::GetDecoViewFrameTexID(m_StampData.frame_id), true, /*bool isSkipOpenClose =*/ false );
          else                m_Loader.RequestLoad( m_pUtilHeap, FILELOCAL::arcid, Static::DecoDataAccessor::GetDecoViewTexID(m_StampData.id), true, /*bool isSkipOpenClose =*/ false );
        }
        m_LoadRequestInfo.isAvailable = false;
        m_State = STATE_Loading;
      }
      break;

    case  STATE_Loading:
    {
      if(m_Loader.ObserveLoad())
      {
        gfl2::heap::HeapBase* pHeapBase = m_pUtilHeap->GetDeviceHeap();

        nw::lyt::LoadTexture(&m_TexInfo, m_Loader.GetData(), m_Loader.GetDataBytes());
        if((!m_bReleaseRequest) && (!m_LoadRequestInfo.isAvailable))
        {
          /*  追加  */
          if(m_IsFrameData) m_pPicture  = App::FinderStudio::Static::DecoObject::CreateDecoObject_UpperFrame(pHeapBase, &m_TexInfo);    /*  frame  */
          else              m_pPicture  = App::FinderStudio::Static::DecoObject::CreateDecoObject_Upper(pHeapBase, &m_TexInfo, gfl2::math::VEC3(m_StampData.x, m_StampData.y, 0.0f)); /*  stamp  */  

          m_pParentPane->AppendChild(m_pPicture);
          m_State = STATE_Idle;
        }
        else
        {
          /*  破棄  */
          m_bReleaseRequest = false;
          m_State           = STATE_Release;
        }
      }
    }
      break;

    case  STATE_Idle:
      /*  次のリクエストがあれば現在のものを破棄  */
      if(m_LoadRequestInfo.isAvailable)
      {
        m_bReleaseRequest = true;
      }

      if(m_bReleaseRequest)
      {
        m_pParentPane->RemoveChild(m_pPicture);
        
        m_bReleaseRequest = false;
        m_State           = STATE_Release;      /*    */
      }
      break;

    case  STATE_Release:
      /*  破棄  */
      m_Loader.ReleaseData();
#if defined(GF_PLATFORM_CTR)
      nw::lyt::DeleteTexture(m_TexInfo); /*  CTRにしか存在しない関数  */
#endif
      if(m_pPicture)
      {
        App::FinderStudio::Static::DecoObject::DeleteDecoObject(m_pPicture);
        m_pPicture  = NULL;
      }
      m_State = STATE_Empty;
      break;


    default:
      GFL_ASSERT(false);  /*  invalid state  */
      break;
  }

  return m_State;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  DecoItem::ReleaseRequest(const bool isFinalize)
{
  m_bReleaseRequest = true;
  if(isFinalize)  m_LoadRequestInfo.isAvailable = false;
}


}  /*  namespace Deco  */
}  /*  namespace Viewer  */
}  /*  namespace FinderStudio  */
}  /*  namespace App  */
