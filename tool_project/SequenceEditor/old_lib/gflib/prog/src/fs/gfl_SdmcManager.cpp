//=============================================================================
/**
 * @brief   SDカード管理
 * @file    gfl_SdmcManager.h
 * @auhtor  tamada
 * @date    2012.10.06
 */
//=============================================================================

#include "fs/gfl_SDMCManager.h"

namespace gfl {
namespace fs {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
SDMCManager::SDMCManager() :
  m_IsInserted(false),
  m_IsWritable(false),
  m_EjectEvent(),
  m_InsertEvent(),
  m_Listener(NULL)

{
  m_IsInserted = nn::fs::IsSdmcInserted();
  m_IsWritable = nn::fs::IsSdmcWritable();
  m_EjectEvent.Initialize( true );
  m_InsertEvent.Initialize( true );

  nn::fs::RegisterSdmcEjectedEvent( &m_EjectEvent );
  nn::fs::RegisterSdmcInsertedEvent( &m_InsertEvent );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
SDMCManager::~SDMCManager()
{
  m_EjectEvent.Finalize();
  m_InsertEvent.Finalize();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void SDMCManager::Update( void )
{
  if ( m_EjectEvent.TryWait() )
  {
    m_IsInserted = false;
    m_EjectEvent.ClearSignal();
    if ( m_Listener )
    {
      m_Listener->OnEjectEvent();
    }
  }

  if ( m_InsertEvent.TryWait() )
  {
    m_IsInserted = true;
    //@attention この関数呼び出しは高負荷だと思われる。頻繁にInsertチェックするべきでない
    m_IsWritable = nn::fs::IsSdmcWritable();
    m_InsertEvent.ClearSignal();
    if ( m_Listener )
    {
      m_Listener->OnInsertEvent();
    }
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void SDMCManager::AddEventListener( ISDMCEventListener * pListener )
{
  GFL_ASSERT_STOP( m_Listener == NULL );
  m_Listener = pListener;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void SDMCManager::RemoveEventListener( ISDMCEventListener * pListener )
{
  if ( m_Listener == pListener )
  {
    m_Listener = NULL;
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool SDMCManager::IsWritable( void ) const
{
  return m_IsWritable;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool SDMCManager::IsInserted( void ) const
{
  return m_IsInserted;
}


};  // namespace fs
};  // namespace gfl



