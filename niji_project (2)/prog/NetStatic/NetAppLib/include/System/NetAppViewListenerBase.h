// ============================================================================
/*
 * @file NetAppViewListenerBase.h
 * @brief ビューリスナーの基底クラスです。
 * @date 2015.06.24
 */
// ============================================================================
#if !defined( NETAPPVIEWLISTENERBASE_H_INCLUDE )
#define NETAPPVIEWLISTENERBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_Assert.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(System)


class NetAppViewListenerBase
{
  GFL_FORBID_COPY_AND_ASSIGN( NetAppViewListenerBase );

public:

  static const u32 INPUT_EVENT_ID_NONE = 0xffffffff;

  NetAppViewListenerBase() :
    m_InputEventID( INPUT_EVENT_ID_NONE )
  {
  }

  virtual~NetAppViewListenerBase()
  {
  }

  void ResetInputEvent()
  {
    m_InputEventID = INPUT_EVENT_ID_NONE;
  }

  u32 GetInputEventID() const
  {
    return m_InputEventID;
  }

protected:  
  
  void SetInputEventID( u32 inputEventID )
  {
    GFL_ASSERT_MSG( ( m_InputEventID == INPUT_EVENT_ID_NONE ), "Current event ID = %d, New event ID = %d\n", m_InputEventID, inputEventID );
    m_InputEventID = inputEventID;
  }

private:

  u32   m_InputEventID;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)


#endif // NETAPPVIEWLISTENERBASE_H_INCLUDE
