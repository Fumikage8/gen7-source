//==============================================================================
/**
 * @file   NetAppClearSettingUtility.cpp
 * @date   2016/01/27 Wed. 21:10:50
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "GameSys/include/GameManager.h"

#include  "NetStatic/NetAppLib/include/Util/NetAppClearSettingUtility.h"


namespace NetAppLib {
namespace Util      {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
NetAppClearSettingUtility::NetAppClearSettingUtility(void)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppClearSettingUtility::~NetAppClearSettingUtility()
{
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppClearSettingUtility::PushClearColor(const f32 r, const f32 g, const f32 b, const f32 a)
{
  const gfl2::gfx::Color  col(r ,g, b, a);
  
  GetClearSetting();
  
  m_BufferClearSetting[CLEARSETTING_MINE][gfl2::gfx::CtrDisplayNo::LEFT].color  = col;
  m_BufferClearSetting[CLEARSETTING_MINE][gfl2::gfx::CtrDisplayNo::RIGHT].color = col;
  m_BufferClearSetting[CLEARSETTING_MINE][gfl2::gfx::CtrDisplayNo::DOWN].color  = col;

  SetClearSetting(false);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppClearSettingUtility::PopClearColor(void)
{
  SetClearSetting(true);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppClearSettingUtility::SetClearSetting(const bool isRestoreBackup)
{
  const u32 settingIndex  = isRestoreBackup ? CLEARSETTING_BACKUP : CLEARSETTING_MINE;
  
  GameSys::GameManager* pGame = GFL_SINGLETON_INSTANCE(GameSys::GameManager);

  pGame->SetBufferClearSetting( gfl2::gfx::CtrDisplayNo::LEFT,  m_BufferClearSetting[settingIndex][gfl2::gfx::CtrDisplayNo::LEFT]);
  pGame->SetBufferClearSetting( gfl2::gfx::CtrDisplayNo::RIGHT, m_BufferClearSetting[settingIndex][gfl2::gfx::CtrDisplayNo::RIGHT]);
  pGame->SetBufferClearSetting( gfl2::gfx::CtrDisplayNo::DOWN,  m_BufferClearSetting[settingIndex][gfl2::gfx::CtrDisplayNo::DOWN]);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppClearSettingUtility::GetClearSetting(void)
{
  GameSys::GameManager* pGame = GFL_SINGLETON_INSTANCE(GameSys::GameManager);

  pGame->GetBufferClearSetting( gfl2::gfx::CtrDisplayNo::LEFT,  &(m_BufferClearSetting[CLEARSETTING_BACKUP][gfl2::gfx::CtrDisplayNo::LEFT]));
  pGame->GetBufferClearSetting( gfl2::gfx::CtrDisplayNo::RIGHT, &(m_BufferClearSetting[CLEARSETTING_BACKUP][gfl2::gfx::CtrDisplayNo::RIGHT]));
  pGame->GetBufferClearSetting( gfl2::gfx::CtrDisplayNo::DOWN,  &(m_BufferClearSetting[CLEARSETTING_BACKUP][gfl2::gfx::CtrDisplayNo::DOWN]));
  pGame->GetBufferClearSetting( gfl2::gfx::CtrDisplayNo::LEFT,  &(m_BufferClearSetting[CLEARSETTING_MINE][gfl2::gfx::CtrDisplayNo::LEFT]));
  pGame->GetBufferClearSetting( gfl2::gfx::CtrDisplayNo::RIGHT, &(m_BufferClearSetting[CLEARSETTING_MINE][gfl2::gfx::CtrDisplayNo::RIGHT]));
  pGame->GetBufferClearSetting( gfl2::gfx::CtrDisplayNo::DOWN,  &(m_BufferClearSetting[CLEARSETTING_MINE][gfl2::gfx::CtrDisplayNo::DOWN]));
}


} /*  namespace Util      */
} /*  namespace NetAppLib */
