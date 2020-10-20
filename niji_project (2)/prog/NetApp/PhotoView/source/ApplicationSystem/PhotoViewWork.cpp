//==============================================================================
/**
 * @file   PhotoViewWork.cpp
 * @date   2017/02/07 Tue. 13:10:26
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <print/include/PrintSystem.h>
#include  <str/include/gfl2_MsgData.h>
#include  <arc_index/message.gaix>

#include  "./PhotoViewWork.h"


namespace NetApp  {
namespace PhotoView {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_PhotoViewWork
namespace FILELOCAL {
  static const u32  APPHEAP_SIZE  = 0x00100000;
  static const u32  DEVHEAP_SIZE  = 0x01800000;
}

// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     PhotoView
  *  @brief    ctor
  */
//------------------------------------------------------------------
PhotoViewWork::PhotoViewWork(void)
  : NetAppLib::System::ApplicationWorkBase(FILELOCAL::APPHEAP_SIZE, FILELOCAL::DEVHEAP_SIZE)
  , m_pAppParam(NULL)
  , m_pTrainerIconUtil(NULL)
  , m_MessageUtility(GetAppHeap()->GetDeviceHeap(), 2)
  , m_pPhotoViewViewContainer(NULL)
{

}


//------------------------------------------------------------------
/**
  *  @func     PhotoView
  *  @brief    ctor
  */
//------------------------------------------------------------------
PhotoViewWork::~PhotoViewWork()
{
}


} /*  namespace PhotoView    */
} /*  namespace NetApp  */
