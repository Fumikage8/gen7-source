//=============================================================================================
/**
 * @file    btl_Hemanager.cpp
 * @brief   ポケモンXY バトルシステム サーバコマンド生成ハンドラの反応処理プロセスマネージャ
 * @author  taya
 *
 * @date    2011.01.12  作成
 */
//=============================================================================================
#include <util/include/gfl2_std_string.h>

#include "btl_ServerFlow.h"
#include "btl_Hemanager.h"

//--------------------------------------------------------------------------------------------------------
// HandEx
//--------------------------------------------------------------------------------------------------------
enum {
  PRINT_CHANNEL = 0,  // 0:NoPrint 1:StdPrint 2~:ExPrint
};

GFL_NAMESPACE_BEGIN(btl)

//----------------------------------------------------------------------
/**
 *  コンストラクタ
 */
//----------------------------------------------------------------------
HEManager::HEManager( void )
{
  clearWork();
}
//----------------------------------------------------------------------
/**
 *  初期化
 */
//----------------------------------------------------------------------
void HEManager::Init( void )
{
  clearWork();
}
/**
 *  メンバ初期化
 */
void HEManager::clearWork( void )
{
  m_state = 0;
  gfl2::std::MemClear( m_workBuffer, sizeof(m_workBuffer) );
}


u32 HEManager::PushState( void )
{
  u32 state = m_state;

  m_useItem = ITEM_DUMMY_DATA;
  m_read_ptr = m_stack_ptr;
  m_fSucceed = 0;
  m_fPrevSucceed = 0;
  m_fUsed = 0;

  return state;
}

u32 HEManager::PushStateUseItem( u16 itemNo )
{
  u32 state = m_state;

  m_useItem = itemNo;
  m_read_ptr = m_stack_ptr;
  m_fSucceed = 0;
  m_fPrevSucceed = 0;
  m_fUsed = 0;

  return state;
}

void HEManager::PopState( u32 state )
{
  m_state = state;
}

BTL_HANDEX_PARAM_HEADER*  HEManager::ReadWork( void )
{
  if( m_read_ptr < m_stack_ptr )
//  if( (m_read_ptr==0) && (m_stack_ptr!=0) )
  {
    BTL_HANDEX_PARAM_HEADER* header = reinterpret_cast<BTL_HANDEX_PARAM_HEADER*>(&m_workBuffer[m_read_ptr]);
    m_read_ptr += header->size;
//    m_stack_ptr = 0;

    return header;
  }
  else
  {
    return NULL;
  }
}

bool HEManager::IsExistWork( void ) const
{
  return ( m_stack_ptr != 0 );
}

u16 HEManager::GetUseItemNo( void ) const
{
  return m_useItem;
}

bool HEManager::IsUsed( void ) const
{
  return m_fUsed;
}
void HEManager::SetResult( bool fSucceed )
{
  if( fSucceed ){
    m_fPrevSucceed = true;
    m_fSucceed = true;
  }else{
    m_fPrevSucceed = false;
  }
  m_fUsed = true;
}
bool HEManager::GetPrevResult( void ) const
{
  return m_fPrevSucceed;
}
bool HEManager::GetTotalResult( void ) const
{
  return m_fSucceed;
}

//=============================================================================================
/**
 * ワークメモリ１件確保
 *
 * @param   wk
 * @param   eq_type
 * @param   userPokeID
 *
 * @retval  BTL_HANDEX_PARAM_HEADER*
 */
//=============================================================================================
BTL_HANDEX_PARAM_HEADER*  HEManager::PushWork( EventHandlerExhibition eq_type, u8 userPokeID )
{
  static const struct {
    u8 eq_type;
    u8 size;
  }work_size_table[] = {
    { BTL_HANDEX_USE_ITEM,             sizeof(BTL_HANDEX_PARAM_USE_ITEM)            },
    { BTL_HANDEX_RECOVER_HP,           sizeof(BTL_HANDEX_PARAM_RECOVER_HP)          },
    { BTL_HANDEX_DRAIN,                sizeof(BTL_HANDEX_PARAM_DRAIN)               },
    { BTL_HANDEX_SHIFT_HP,             sizeof(BTL_HANDEX_PARAM_SHIFT_HP)            },
    { BTL_HANDEX_RECOVER_PP,           sizeof(BTL_HANDEX_PARAM_PP)                  },
    { BTL_HANDEX_DECREMENT_PP,         sizeof(BTL_HANDEX_PARAM_PP)                  },
    { BTL_HANDEX_CURE_SICK,            sizeof(BTL_HANDEX_PARAM_CURE_SICK)           },
    { BTL_HANDEX_ADD_SICK,             sizeof(BTL_HANDEX_PARAM_ADD_SICK)            },
    { BTL_HANDEX_RANK_EFFECT,          sizeof(BTL_HANDEX_PARAM_RANK_EFFECT)         },
    { BTL_HANDEX_SET_RANK,             sizeof(BTL_HANDEX_PARAM_SET_RANK)            },
    { BTL_HANDEX_RECOVER_RANK,         sizeof(BTL_HANDEX_PARAM_RECOVER_RANK)        },
    { BTL_HANDEX_RESET_RANK,           sizeof(BTL_HANDEX_PARAM_RESET_RANK)          },
    { BTL_HANDEX_RESET_RANKUP,         sizeof(BTL_HANDEX_PARAM_RESET_RANKUP)        },
    { BTL_HANDEX_SET_STATUS,           sizeof(BTL_HANDEX_PARAM_SET_STATUS)          },
    { BTL_HANDEX_DAMAGE,               sizeof(BTL_HANDEX_PARAM_DAMAGE)              },
    { BTL_HANDEX_KILL,                 sizeof(BTL_HANDEX_PARAM_KILL)                },
    { BTL_HANDEX_DEAD_CHECK,           sizeof(BTL_HANDEX_PARAM_DEAD_CHECK)          },
    { BTL_HANDEX_CHANGE_TYPE,          sizeof(BTL_HANDEX_PARAM_CHANGE_TYPE)         },
    { BTL_HANDEX_EX_TYPE,              sizeof(BTL_HANDEX_PARAM_EX_TYPE)             },
    { BTL_HANDEX_MESSAGE,              sizeof(BTL_HANDEX_PARAM_MESSAGE)             },
    { BTL_HANDEX_TOKWIN_IN,            sizeof(BTL_HANDEX_PARAM_HEADER)              },
    { BTL_HANDEX_TOKWIN_OUT,           sizeof(BTL_HANDEX_PARAM_HEADER)              },
    { BTL_HANDEX_SET_TURNFLAG,         sizeof(BTL_HANDEX_PARAM_TURNFLAG)            },
    { BTL_HANDEX_RESET_TURNFLAG,       sizeof(BTL_HANDEX_PARAM_TURNFLAG)            },
    { BTL_HANDEX_SET_CONTFLAG,         sizeof(BTL_HANDEX_PARAM_SET_CONTFLAG)        },
    { BTL_HANDEX_RESET_CONTFLAG,       sizeof(BTL_HANDEX_PARAM_SET_CONTFLAG)        },
    { BTL_HANDEX_SET_PERMFLAG,         sizeof(BTL_HANDEX_PARAM_SET_PERMFLAG)        },
    { BTL_HANDEX_SIDEEFF_ADD,          sizeof(BTL_HANDEX_PARAM_SIDEEFF_ADD)         },
    { BTL_HANDEX_SIDEEFF_REMOVE,       sizeof(BTL_HANDEX_PARAM_SIDEEFF_REMOVE)      },
    { BTL_HANDEX_SIDEEFF_SLEEP,        sizeof(BTL_HANDEX_PARAM_SIDEEFF_SLEEP)       },
    { BTL_HANDEX_ADD_FLDEFF,           sizeof(BTL_HANDEX_PARAM_ADD_FLDEFF)          },
    { BTL_HANDEX_CHANGE_WEATHER,       sizeof(BTL_HANDEX_PARAM_CHANGE_WEATHER)      },
    { BTL_HANDEX_REMOVE_FLDEFF,        sizeof(BTL_HANDEX_PARAM_REMOVE_FLDEFF)       },
    { BTL_HANDEX_POSEFF_ADD,           sizeof(BTL_HANDEX_PARAM_POSEFF_ADD)          },
    { BTL_HANDEX_CHANGE_TOKUSEI,       sizeof(BTL_HANDEX_PARAM_CHANGE_TOKUSEI)      },
    { BTL_HANDEX_SET_ITEM,             sizeof(BTL_HANDEX_PARAM_SET_ITEM)            },
    { BTL_HANDEX_CHECK_ITEM_EQUIP,     sizeof(BTL_HANDEX_PARAM_CHECK_ITEM_EQUIP)    },
    { BTL_HANDEX_ITEM_SP,              sizeof(BTL_HANDEX_PARAM_ITEM_SP)             },
    { BTL_HANDEX_CONSUME_ITEM,         sizeof(BTL_HANDEX_PARAM_CONSUME_ITEM)        },
    { BTL_HANDEX_SWAP_ITEM,            sizeof(BTL_HANDEX_PARAM_SWAP_ITEM)           },
    { BTL_HANDEX_UPDATE_WAZA,          sizeof(BTL_HANDEX_PARAM_UPDATE_WAZA)         },
    { BTL_HANDEX_COUNTER,              sizeof(BTL_HANDEX_PARAM_COUNTER)             },
    { BTL_HANDEX_DELAY_WAZADMG,        sizeof(BTL_HANDEX_PARAM_DELAY_WAZADMG)       },
    { BTL_HANDEX_QUIT_BATTLE,          sizeof(BTL_HANDEX_PARAM_QUIT_BATTLE)         },
    { BTL_HANDEX_CHANGE_MEMBER,        sizeof(BTL_HANDEX_PARAM_CHANGE_MEMBER)       },
    { BTL_HANDEX_BATONTOUCH,           sizeof(BTL_HANDEX_PARAM_BATONTOUCH)          },
    { BTL_HANDEX_ADD_SHRINK,           sizeof(BTL_HANDEX_PARAM_ADD_SHRINK)          },
    { BTL_HANDEX_RELIVE,               sizeof(BTL_HANDEX_PARAM_RELIVE)              },
    { BTL_HANDEX_SET_WEIGHT,           sizeof(BTL_HANDEX_PARAM_SET_WEIGHT)          },
    { BTL_HANDEX_PUSHOUT,              sizeof(BTL_HANDEX_PARAM_PUSHOUT)             },
    { BTL_HANDEX_INTR_POKE,            sizeof(BTL_HANDEX_PARAM_INTR_POKE)           },
    { BTL_HANDEX_INTR_WAZA,            sizeof(BTL_HANDEX_PARAM_INTR_WAZA)           },
    { BTL_HANDEX_SEND_LAST,            sizeof(BTL_HANDEX_PARAM_SEND_LAST)           },
    { BTL_HANDEX_SWAP_POKE,            sizeof(BTL_HANDEX_PARAM_SWAP_POKE)           },
    { BTL_HANDEX_HENSIN,               sizeof(BTL_HANDEX_PARAM_HENSIN)              },
    { BTL_HANDEX_FAKE_BREAK,           sizeof(BTL_HANDEX_PARAM_FAKE_BREAK)          },
    { BTL_HANDEX_JURYOKU_CHECK,        sizeof(BTL_HANDEX_PARAM_HEADER)              },
    { BTL_HANDEX_TAMEHIDE_CANCEL,      sizeof(BTL_HANDEX_PARAM_TAMEHIDE_CANCEL)     },
    { BTL_HANDEX_ADD_EFFECT,           sizeof(BTL_HANDEX_PARAM_ADD_EFFECT)          },
    { BTL_HANDEX_VANISH_MSGWIN,        sizeof(BTL_HANDEX_PARAM_HEADER)              },
    { BTL_HANDEX_CHANGE_FORM,          sizeof(BTL_HANDEX_PARAM_CHANGE_FORM)         },
    { BTL_HANDEX_SET_EFFECT_IDX,       sizeof(BTL_HANDEX_PARAM_SET_EFFECT_IDX)      },
    { BTL_HANDEX_WAZAEFFECT_ENABLE,    sizeof(BTL_HANDEX_PARAM_WAZAEFFECT_ENABLE)   },
    { BTL_HANDEX_FRIENDSHIP_EFFECT,    sizeof(BTL_HANDEX_PARAM_FRIENDSHIP_EFFECT)   },
    { BTL_HANDEX_INSERT_WAZA_ACTION,   sizeof(BTL_HANDEX_PARAM_INSERT_WAZA_ACTION)  },
    { BTL_HANDEX_SET_DIRT,             sizeof(BTL_HANDEX_PARAM_SET_DIRT)  },

  };
  u32 size, i;

  for(i=0, size=0; i<GFL_NELEMS(work_size_table); ++i)
  {
    if( work_size_table[i].eq_type == eq_type ){
      size = work_size_table[i].size;
      break;
    }
  }

  if( size )
  {
    while( size % 4 ){ ++size; }
    if( (m_stack_ptr + size) <= sizeof(m_workBuffer) )
    {
      BTL_HANDEX_PARAM_HEADER* header = (BTL_HANDEX_PARAM_HEADER*) &(m_workBuffer[m_stack_ptr]);

      for(i=0; i<size; ++i){
        m_workBuffer[ m_stack_ptr + i ] = 0;
      }
      header->equip = eq_type;
      header->size = size;
      header->userPokeID = userPokeID;
      header->tokwin_flag = 0;
      header->usingFlag = 1;

      m_stack_ptr += size;
//      m_read_ptr = m_stack_ptr;

      BTL_N_PrintfEx( PRINT_CHANNEL_HEMSYS, DBGSTR_HEM_PushWork, eq_type, userPokeID, size, m_stack_ptr );
      return header;
    }
    else
    {
      GFL_ASSERT_MSG(0, "stack over flow ... eq_type=%d, pokeID=%d", eq_type, userPokeID);
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "illegal eq_type = %d", eq_type);
  }
  return NULL;
}
//=============================================================================================
/**
 * ワークメモリ１件返却
 *
 * @param   wk
 * @param   exWork
 */
//=============================================================================================
void HEManager::PopWork( void* exWork )
{
  BTL_HANDEX_PARAM_HEADER* header = (BTL_HANDEX_PARAM_HEADER*) exWork;

  if( header->size <= m_stack_ptr )
  {
    u32 basePos = ((u32)(exWork) - (u32)(m_workBuffer));
    u32 btm = basePos + header->size;

    if( btm == m_stack_ptr )
    {
      m_stack_ptr -= header->size;

      BTL_N_PrintfEx( PRINT_CHANNEL_HEMSYS, DBGSTR_HEM_PophWork,
         header->equip, header->userPokeID, header->size, m_stack_ptr, basePos );
    }
    else
    {
      GFL_ASSERT_MSG(0, "exWork pop error! workSize=%d, endPtr=%d\n", header->size, btm );
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "exWork pop over! workSize=%d, sp=%d\n", header->size, m_stack_ptr );
    TAYA_Printf("for BreakPoint\n");
  }
}





void HANDEX_STR_Clear( BTL_HANDEX_STR_PARAMS* param )
{
  gfl2::std::MemClear( param, sizeof(*param) );
  param->type = BTL_STRTYPE_NULL;
}

bool HANDEX_STR_IsEnable( const BTL_HANDEX_STR_PARAMS* param )
{
  return param->type != BTL_STRTYPE_NULL;
}

void HANDEX_STR_Setup( BTL_HANDEX_STR_PARAMS* param, BtlStrType type, u16 strID )
{
  param->type = type;
  param->ID = strID;
  param->argCnt = 0;
}
void HANDEX_STR_AddArg( BTL_HANDEX_STR_PARAMS* param, int arg )
{
  if( param->argCnt < BTL_STR_ARG_MAX )
  {
    param->args[ param->argCnt++ ] = arg;
  }
}
void HANDEX_STR_AddSE( BTL_HANDEX_STR_PARAMS* param, u32 SENo )
{
  if( param->argCnt < BTL_STR_ARG_MAX )
  {
    param->args[ BTL_STR_ARG_MAX - 1 ] = SENo;
    param->fSEAdd = true;
  }
}
void HANDEX_STR_SetFailMsgFlag( BTL_HANDEX_STR_PARAMS* param )
{
  param->fFailMsg = true;
}

bool HANDEX_STR_IsFailMsg( const BTL_HANDEX_STR_PARAMS* param )
{
  return param->fFailMsg;
}
GFL_NAMESPACE_END(btl)



