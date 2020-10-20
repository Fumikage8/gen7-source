
#if !defined(PHOTOVIEWAPPPARAM_H_INCLUDED)
#define PHOTOVIEWAPPPARAM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   PhotoViewAppParam.h
 * @date   2017/02/07 Tue. 11:59:07
 * @author muto_yu
 * @brief  写真閲覧
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  <util/include/gfl2_List.h>

#include  <NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h>
#include  <App/FinderStudioStatic/include/FinderStudioViewerDefine.h>
#include  <App/FinderStudioStatic/include/FinderStudioDefine.h>

namespace NetApp {
namespace PhotoView {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
struct  APP_PARAM
{
  struct PhotoInfo
  {
    gfl2::str::STRCODE                        name[pml::PERSON_NAME_BUFFER_SIZE];
    App::FinderStudio::Static::PhotoData      photoData;
    JoinFestaFriendKey                        key;
    bool                                      bShowed;
    
    /*  ctor  */
    PhotoInfo(void)
      : photoData()
      , key()
      , bShowed(false)
    {
      memset(name, 0, sizeof(name));
    }
    
    bool  Setup(const NetAppLib::JoinFesta::JoinFestaPersonalData& rJFPersonal)
    {
      
      const bool isSuccess  = rJFPersonal.GetPacketPhotoData( &photoData );
      
      GFL_ASSERT(isSuccess);
      if(isSuccess)
      {
        rJFPersonal.GetName(name);
        key = rJFPersonal.GetJoinFestaFriendKey();
      }
      
      return isSuccess;
    }

    void  Showed(void)          {bShowed  = true;}
    bool  IsShowed(void) const  {return bShowed;}
  };

  gfl2::util::List<PhotoInfo>*    m_pPlayList;    /**<  [in/out]  写真情報  */



  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  bool  RegisterJoinFestaPersonalData(const NetAppLib::JoinFesta::JoinFestaPersonalData& rJFPersonal)
  {
    bool  isSuccess = false;
    
    if(m_pPlayList && (m_pPlayList->Size() < m_pPlayList->MaxSize()))
    {
      PhotoInfo info;
      
      isSuccess = info.Setup(rJFPersonal);
      if(isSuccess)
      {
        m_pPlayList->PushBack(info);
      }
    }
    
    return isSuccess;
  }


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void  ClearPhotoInfos(void)
  {
    if(m_pPlayList)
    {
      m_pPlayList->Erase();
    }
  }

  /*  ctor  */
  APP_PARAM(void)
    : m_pPlayList(NULL)
  {}

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  static gfl2::util::List<PhotoInfo>* CreatePhotoInfoList(gfl2::heap::HeapBase* pHeapBase, const u32 capacity)
  {
    return GFL_NEW(pHeapBase) gfl2::util::List<PhotoInfo>(pHeapBase, capacity);
  }
};


}  /*  namespace PhotoView  */
}  /*  namespace NetApp  */
#endif  /*  #if !defined(PHOTOVIEWAPPPARAM_H_INCLUDED)  */

