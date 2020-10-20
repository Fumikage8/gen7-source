#if !defined(NETAPPCLEARSETTINGUTILITY_H_INCLUDED)
#define NETAPPCLEARSETTINGUTILITY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   NetAppClearSettingUtility.h
 * @date   2016/01/27 Wed. 21:12:49
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>


namespace NetAppLib {
namespace Util      {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/01/27 Wed. 21:13:41
  */
//==============================================================================
class NetAppClearSettingUtility
{
  GFL_FORBID_COPY_AND_ASSIGN(NetAppClearSettingUtility);
public:
  NetAppClearSettingUtility(void);
  virtual ~NetAppClearSettingUtility();

  void  PushClearColor(const f32 r=0.0f, const f32 g=0.0f, const f32 b=0.0f, const f32 a=0.0f);
  void  PopClearColor(void);

protected:
  void  SetClearSetting(const bool isRestoreBackup);
  void  GetClearSetting(void);

  enum  {
    CLEARSETTING_BACKUP,
    CLEARSETTING_MINE,
    
    CLEARSETTING_MAX
    
  };
  GameSys::GameManager::BUFFER_CLEAR_SETTING  m_BufferClearSetting[CLEARSETTING_MAX][gfl2::gfx::CtrDisplayNo::NumberOf];
};



} /*  namespace Util      */
} /*  namespace NetAppLib */
#endif  /*  #if !defined(NETAPPCLEARSETTINGUTILITY_H_INCLUDED)  */
