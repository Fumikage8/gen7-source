//==============================================================================
/**
 * @file	  BoxUppStatusListener.h
 * @brief	  上画面との伝達
 * @author	k.ohno
 * @date	  2016.03.06
 */
// =============================================================================

#if !defined( __BOX_UPP_STATUS_LISTENER_H__ )
#define	__BOX_UPP_STATUS_LISTENER_H__
#pragma once

#include "pml/include/pml_PokePara.h"

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

class BoxUppStatusListener {


public:

  enum UPPDISP_TYPE{
    UPPDISP_NONE,
    UPPDISP_STATE,
    UPPDISP_UPDATE_STATE,
    UPPDISP_NET_STATE,
    UPPDISP_JUGDE,
    UPPDISP_CHAT,
    UPPDISP_CHAT_FORCE,
    UPPDISP_CHAT_UPDATE,
    UPPDISP_FOLDER,
    UPPDISP_FOLDER_END,
    UPPDISP_FOLDER_ICONUPDATE,
    UPPDISP_FOLDER_RELOAD,
  };

  enum UPPDISP_SELECT{
    UPPDISP_STATUS_ME = 1,
    UPPDISP_STATUS_TARGET,
  };

  struct UppDispChange{
    UPPDISP_TYPE requestNo;
    u32 param;
    u32 param2;
  };

  static const int UPPDISP_SELECT_PUSHMAX = 2;
  static const int UPPDISP_SELECT_POKE_PUSHMAX = 3;
  
public:
  virtual void Init(void)=0;
  virtual void Request(UPPDISP_TYPE no, u32 param)=0;
  virtual void PokeChangeRequest(UPPDISP_TYPE no, u32 param, u32 param2)=0;
  virtual void Request(UPPDISP_TYPE no, u32 param, u32 param2)=0;
  virtual void Update(void)=0;
  virtual void UpdatePokemon(void)=0;

  UppDispChange mUppDispChange[UPPDISP_SELECT_PUSHMAX];  //切り替え用変数バックアップ２層
  UppDispChange mUppDispChangePoke[UPPDISP_SELECT_POKE_PUSHMAX];  //切り替え用変数
  
};

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )

#endif	/* __BOX_UPP_STATUS_LISTENER_H__ */
