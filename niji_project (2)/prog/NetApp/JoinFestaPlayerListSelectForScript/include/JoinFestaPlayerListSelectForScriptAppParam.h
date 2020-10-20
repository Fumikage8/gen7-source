// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptAppParam.h
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)を起動する為のパラメータです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTFORSCRIPT_APPPARAM_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTFORSCRIPT_APPPARAM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetApp/JoinFestaPlayerListSelectForScript/source/JoinFestaPlayerListSelectForScriptDefine.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"
#include <NetStatic/NetAppLib/include/JoinFestaUI\JoinFestaPlayerData.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)


struct APP_PARAM
{
  u32       bootMode;     //! [in]起動モード( JoinFestaPlayerListSelectForScriptDefine.h の BootMode を参照 )
  
  // 仕様書のモード５(リストのサイズに指定があるバージョン)の引数
  u32       listSize;
};








//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/12/06 Tue. 17:44:01
  */
//==============================================================================
struct BFCModeInfo
{
  static const u32  SELECT_MAX    = 2;  /**<  最大選択数  */
  static const u32  INVALID_INDEX = UINT_MAX;
  NetAppLib::JoinFestaUI::JoinFestaPlayerData selectedPlayerData[SELECT_MAX];
  u32                                         dataNum;
  NetAppLib::JoinFestaUI::JoinFestaPlayerData statusTargetPlayerData;   /**<  status表示のためのPlayerData  */
  
  BFCModeInfo(void)
    : dataNum(0)
    , statusTargetPlayerData()
  {}
  
  
  /*  */
  NetAppLib::JoinFestaUI::JoinFestaPlayerData*  GetPlayerData(const u32 index)
  {
    return ((index < dataNum) ? &selectedPlayerData[index] : NULL);
  }
  
  /*    */
  static bool  IsValidIndex(const u32 index)
  {
    return (index < SELECT_MAX);
  }
  
  
  /*    */
  void  Clear(void)
  {
    memset(selectedPlayerData, 0, sizeof(selectedPlayerData));
    dataNum = 0;
  }
  
  /*    */
  u32  Insert(const NetAppLib::JoinFestaUI::JoinFestaPlayerData& rData)
  {
    u32 index = INVALID_INDEX;
    
    if(dataNum < SELECT_MAX)
    {
      index = dataNum;
      selectedPlayerData[dataNum++] = rData;
    }

    GFL_PRINT("%s: %u\n", __FUNCTION__, dataNum);
    return index;
  }
  
  /*    */
  void  Remove(const NetAppLib::JoinFestaUI::JoinFestaPlayerData& rData)
  {
    const u32 targetIndex = Find(rData);
    
    if(targetIndex < SELECT_MAX)
    {
      /*  前詰め  */
      for(u32 moveIndex = targetIndex+1; moveIndex < dataNum; ++moveIndex)
      {
        selectedPlayerData[moveIndex-1] = selectedPlayerData[moveIndex];
      }
      memset(&selectedPlayerData[GFL_NELEMS(selectedPlayerData)-1], 0, sizeof(selectedPlayerData[0]));
      --dataNum;
    }
    GFL_PRINT("%s: %u\n", __FUNCTION__, dataNum);
  }
  
  /*    */
  u32   Find(const NetAppLib::JoinFestaUI::JoinFestaPlayerData& rData)
  {
    for(u32 index = 0; index < dataNum; ++index)
    {
      if(rData.IsSameFriendKey(selectedPlayerData[index].GetJoinFestaFriendKey()))
      {
        return index;
      }
    }
    
    GFL_PRINT("%s: %u\n", __FUNCTION__, dataNum);
    return INVALID_INDEX;
  }
  
  /*    */
  bool  IsContain(const NetAppLib::JoinFestaUI::JoinFestaPlayerData& rData)
  {
    GFL_PRINT("%s: %u\n", __FUNCTION__, dataNum);
    return IsValidIndex(Find(rData));
  }
  
  /*  なければInsert/あればRemove  */
  u32  ToggleInsert(const NetAppLib::JoinFestaUI::JoinFestaPlayerData& rData)
  {
    u32 index = INVALID_INDEX;

    if(IsContain(rData))  Remove(rData);
    else                  index = Insert(rData);

    return index;
  }
  
  /*    */
  bool  IsFull(void) const  {return (SELECT_MAX <= dataNum);}
  
  /*
    status関連
  */
  void  SetStatusTarget(const NetAppLib::JoinFestaUI::JoinFestaPlayerData& rPlayerData) {statusTargetPlayerData.Copy(rPlayerData);}
  bool  IsStatusTargetAvailable(void) const                                             {return statusTargetPlayerData.IsEnable();}
  void  ResetStatusTarget(void)                                                         {memset(&statusTargetPlayerData, 0, sizeof(statusTargetPlayerData));}
};


//! アプリの結果
struct APP_RESULT
{
  NetAppLib::JoinFesta::JoinFestaPersonalData*    pSelectJoinFestaPersonalData;       //! 選択したパーソナルデータ
  bool                                            bIsSelectedJoinFestaPersonalData;   //! 選択フラグ[ true : 選択した, false : 選択しないで戻った ]

  /*  BFC向け拡張  */
  NetAppLib::JoinFesta::JoinFestaPersonalData*    pSelectJoinFestaPersonalData_2;     //! 選択したパーソナルデータ(二人目)

  /*  ctor  */
  APP_RESULT(void)
    : pSelectJoinFestaPersonalData(NULL)
    , bIsSelectedJoinFestaPersonalData(false)
    , pSelectJoinFestaPersonalData_2(NULL)
  {}
};


GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSELECTFORSCRIPT_APPPARAM_H_INCLUDE
