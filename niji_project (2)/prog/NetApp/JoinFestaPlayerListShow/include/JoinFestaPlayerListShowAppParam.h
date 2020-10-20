// ============================================================================
/*
 * @file JoinFestaPlayerListShowAppParam.h
 * @brief 閲覧アプリを起動する為のパラメータです。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSHOW_APPPARAM_H_INCLUDE )
#define JOINFESTAPLAYERLISTSHOW_APPPARAM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <util/include/gfl2_List.h>
#include <NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerData.h>
#include <NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListManager.h>
#include <NetApp/JoinFestaPlayerListShow/source/JoinFestaPlayerListShowDefine.h>




GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)


struct APP_PARAM
{
  static const u32  photoOwnerListCapacity  = NetAppLib::JoinFesta::JoinFestaPersonalDataManager::LIST_MAX * 2;   /**<  guest+ vip  */


  NetAppLib::JoinFestaUI::JoinFestPlayerDataList*   pPhotoOwnerList;
  bool                                              bPhotoFlagApply;    /**<  写真閲覧フラグを元データへ反映させる  */
  
  //------------------------------------------------------------------
  /**
    *  @brief    slideshow対象リストの生成
    *  @note     呼び出し元が管理する想定
    */
  //------------------------------------------------------------------
  void CreateSlideShowTargetList(gfl2::heap::HeapBase* pHeapBase)
  {
    if(!pPhotoOwnerList)
    {
      pPhotoOwnerList = GFL_NEW(pHeapBase) NetAppLib::JoinFestaUI::JoinFestPlayerDataList(pHeapBase, photoOwnerListCapacity);
    }
    else
    {
      GFL_ASSERT(false);
    }
  }

  //------------------------------------------------------------------
  /**
    *  @brief    slideshow対象リストの解放
    *  @note     呼び出し元が管理する想定
    */
  //------------------------------------------------------------------
  void ReleaseSlideShowTargetList(void)
  {
    GFL_SAFE_DELETE(pPhotoOwnerList);
  }


  /*  写真のslideshow対象の登録  */
  bool  RegisterSlideShowTarget(const NetAppLib::JoinFestaUI::JoinFestaPlayerData& rTarget)
  {
    const bool  isSuccess = pPhotoOwnerList ? (pPhotoOwnerList->Size() < pPhotoOwnerList->MaxSize()) : false;
    
    if(isSuccess)
    {
      pPhotoOwnerList->PushBack(rTarget);
    }
    
    return isSuccess;
  }

  /*  写真のslideshow対象をクリア  */
  void  ClearSlideShowTarget(void)
  {
    if(pPhotoOwnerList)
    {
      pPhotoOwnerList->Erase(); /*  all  */
    }
  }

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  bool  IsEmptySlideShowTarget(void) const
  {
    return (pPhotoOwnerList ? (!pPhotoOwnerList->Size()) : true);
  }


  
  /*  ctor  */
  APP_PARAM(void)
    : pPhotoOwnerList(NULL)
    , bPhotoFlagApply(false)
  {}
};



GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSHOW_APPPARAM_H_INCLUDE
