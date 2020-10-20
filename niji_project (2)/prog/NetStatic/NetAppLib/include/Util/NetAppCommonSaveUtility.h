// ============================================================================
/*
 * @file NetAppCommonSaveUtility.h
 * @brief セーブユーティリティ
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#if !defined( NETAPP_COMMON_SAVE_UTILITY_H_INCLUDE )
#define NETAPP_COMMON_SAVE_UTILITY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/Util/NetAppTimeout.h"


GFL_NAMESPACE_BEGIN(Savedata)
  class ZukanData;
GFL_NAMESPACE_END(Savedata)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QR)
  struct QR_ZUKAN;
GFL_NAMESPACE_END(QR)
GFL_NAMESPACE_END(NetApp)


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Util)


class NetAppCommonSaveUtility
{
  GFL_FORBID_COPY_AND_ASSIGN(NetAppCommonSaveUtility);

private:
  
  enum State
  {
    STATE_SAVE_START,
    STATE_SAVE_MAIN,
    STATE_SAVE_WAIT
  };

public:

  NetAppCommonSaveUtility();
  virtual~NetAppCommonSaveUtility();


  bool Save( bool bUpdatePlayTime = true );
  bool SaveWait( bool bUpdatePlayTime = true );

  //! @brief セーブ処理が実行中か判定する
  bool IsExecute() const { return m_bIsExecute; }

public:

  //! @brief  NetApp::QR::QR_ZUKANに書かれている全情報に対してSavedata::ZukanDataのSetPokeSeeを呼ぶ
  static void SetZukanSavePokeSee(Savedata::ZukanData* pZukanSave, const NetApp::QR::QR_ZUKAN& rZukanQR);

  static void SetDisconnectDetectedFlag( bool bFlag );

private:

  void setState( State eState );
  void saveStart( bool bUpdatePlayTime );
  bool saveMain( bool bUpdatePlayTime );


  State                             m_eState;
  NetAppLib::Util::NetAppTimeout    m_Timer;
  bool                              m_bIsExecute;

};


GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetAppLib)

#endif // NETAPP_COMMON_SAVE_UTILITY_H_INCLUDE
