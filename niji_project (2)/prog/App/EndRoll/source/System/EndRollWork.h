// ============================================================================
/*
 * @file EndRollWork.h
 * @brief エンドロールアプリのワーククラスです。
 * @date 2016.03.16
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( APPENDROLL_WORK_H_INCLUDE )
#define APPENDROLL_WORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(EndRoll)
GFL_NAMESPACE_BEGIN(System)

//  前方宣言
class EndRollViewSystemAccessor;

class EndRollWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( EndRollWork );

private:

  static const int  APP_HEAP_SIZE             = 1024 * 1024 * 4;    //  4MB
  static const int  APP_DEVICE_HEAP_SIZE      = 1024 * 1024 * 10;   // 10MB

public:
  EndRollWork();
  virtual~EndRollWork();

  void SetViewSystemAccessor( EndRollViewSystemAccessor* pAccessor )
  {
    GFL_ASSERT( pAccessor );
    m_pViewSystemAcceeor  = pAccessor;
  }

  EndRollViewSystemAccessor*  GetViewSystemAccessor() const { return m_pViewSystemAcceeor; }

  void _Clear()
  {
    m_pViewSystemAcceeor  = NULL;
  }

  EndRollViewSystemAccessor*  m_pViewSystemAcceeor;
};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(EndRoll)
GFL_NAMESPACE_END(App)

#endif // APPENDROLLWORK_H_INCLUDE
