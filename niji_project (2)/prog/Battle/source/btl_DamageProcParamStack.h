//======================================================================
/**
 * @file btl_DamageProcParamStack.h
 * @date 2016/06/22 10:07:43
 * @author taya
 * @brief ServerFlow の scproc_Fight_damage_side_core 計算で使う各種パラメータをスタックしておくための機構
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BTL_DAMAGEPROC_PARAM_STACK_H__
#define __BTL_DAMAGEPROC_PARAM_STACK_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "./btl_common.h"
#include "./btl_pokeparam.h"

GFL_NAMESPACE_BEGIN(btl)



  /**
   * @class DamageProcParamStack
   * @brief scproc_Fight_damage_side_core で保持するパラメータをスタック保存するための機構
   */
  class DamageProcParamStack
  {
      GFL_FORBID_COPY_AND_ASSIGN(DamageProcParamStack);

  public:

    /**
     * @struct Params
     * @brief  スタック１段分のパラメータ構造
     */
    struct Params
    {
      BTL_POKEPARAM *bpp[ BTL_POSIDX_MAX ];
      u16            dmg[ BTL_POSIDX_MAX ];
      BtlTypeAff     affAry[ BTL_POSIDX_MAX ];
      CriticalType   criticalTypes[ BTL_POSIDX_MAX ];
      BTL_POKEPARAM::KoraeruCause  koraeru_cause[ BTL_POSIDX_MAX ];
      u8             pluralDeadIdx[ BTL_POSIDX_MAX ];
    };

    // ctor / dtor
    DamageProcParamStack();
    ~DamageProcParamStack();


    /**
     * @brief スタック１件Pushしてパラメータ構造体へのポインタを取得
     * @return パラメータ構造体へのポインタ
     */
    Params* Push( void );


    /**
     * @brief スタック１件Popする
     */
    void Pop( void );


  private:
    enum {
      STACK_DEPTH = 12,  /// スタックサイズ   ポケ数最大(4)x行動数最大(2)=8あれば充分なはずだが余裕をもたせている
    };

    //! スタックポインタ
    int m_stackPointer;

    //! パラメータ構造体のスタック
    Params  m_params[ STACK_DEPTH ];
  };

GFL_NAMESPACE_END(btl)

#endif // __BTL_DAMAGEPROC_PARAM_STACK_H__