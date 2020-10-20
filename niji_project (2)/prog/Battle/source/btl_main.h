#ifndef __BTL_MAIN_H__
#define __BTL_MAIN_H__

#include <proc/include/gfl2_Proc.h>

#include "Battle/include/battle_SetupParam.h"
#include "btl_MainModule.h"

GFL_NAMESPACE_BEGIN(btl)
    class BtlMainProc : public gfl2::proc::BaseProcess
    {
      public:
        BtlMainProc( void );
        virtual ~BtlMainProc();

      private:
        BtlMainModule*             m_mainModule;
        const BATTLE_SETUP_PARAM*  m_setupParam;

    } // class BtlMain


GFL_NAMESPACE_END(btl)
#endif
