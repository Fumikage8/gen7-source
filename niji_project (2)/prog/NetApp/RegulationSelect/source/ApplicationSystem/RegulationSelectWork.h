// ============================================================================
/*
 * @file RegulationSelectWork.h
 * @brief レギュレーション選択アプリのワーククラスです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATIONSELECT_WORK_H_INCLUDE )
#define REGULATIONSELECT_WORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <util/include/gfl2_List.h>
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"
#include "NetApp/RegulationSelect/source/RegulationSelectDefine.h"
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetStatic/NetAppLib/include/UI/NetAppRegulationDrawInfo.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)
class NetAppEmptyView;
GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


class RegulationSelectWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( RegulationSelectWork );

private:

  static const u32  APP_HEAP_SIZE             = NetApp::RegulationSelect::REGULATION_SELECT_APP_HEAP_SIZE;
  static const u32  APP_DEVICE_HEAP_SIZE      = NetApp::RegulationSelect::REGULATION_SELECT_APP_DEVICE_HEAP_SIZE;

public:
  RegulationSelectWork();
  virtual~RegulationSelectWork();

  NetAppLib::UI::NetAppEmptyView* GetRootUpperView() const { return m_pRootUpperView; }
  void SetRootUpperView( NetAppLib::UI::NetAppEmptyView* pRootUpperView ) { m_pRootUpperView = pRootUpperView; }

  NetAppLib::UI::NetAppEmptyView* GetRootLowerView() const { return m_pRootLowerView; }
  void SetRootLowerView( NetAppLib::UI::NetAppEmptyView* pRootLowerView ) { m_pRootLowerView = pRootLowerView; }

  NetApp::RegulationSelect::BootMode GetBootMode() const { return m_eBootMode; }
  void SetBootMode( NetApp::RegulationSelect::BootMode eBootMode ) { m_eBootMode = eBootMode; }

  Regulation::BATTLE_RULE GetBattleRule() const { return m_eBattleRule; }
  void SetBattleRule( Regulation::BATTLE_RULE eBattleRule ) { m_eBattleRule = eBattleRule; }

  NetApp::RegulationSelect::DetailBootFrame GetDetailBootFrame() const { return m_eDetailBootFrame; }
  void SetDetailBootInfo( NetApp::RegulationSelect::DetailBootFrame eDetailBootFrame, NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo )
  {
    m_eDetailBootFrame = eDetailBootFrame;
    m_pSelectRegulationDrawInfo = pRegulationDrawInfo;
  }

  gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* GetPresetList() { return &m_PresetList; }
  gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* GetDownloadList() { return &m_DownloadList; }
  gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* GetDownloadedList() { return &m_DownloadedList; }

  void ClearList( gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pRegulationList );
  void AddRegulation( gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pRegulationList, const Regulation::PRESET presetRegulation, NetAppLib::UI::RegulationDrawInfo::Rank rank );
#if PM_DEBUG
  void AddRegulation( gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pRegulationList, const Regulation::PRESET presetRegulation, NetAppLib::UI::RegulationDrawInfo::Rank rank, bool preset );
#endif // PM_DEBUG
  void AddRegulation( gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pRegulationList, const void* pBuffer, const u32 size );
  void AddRegulation( gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pRegulationList, const void* pBuffer, const u32 size, int fileIndex );

  NetAppLib::UI::RegulationDrawInfo* GetSelectRegulationDrawInfo() const { return m_pSelectRegulationDrawInfo; }
  void SetSelectRegulationDrawInfo( NetAppLib::UI::RegulationDrawInfo* pSelectRegulationDrawInfo ) { m_pSelectRegulationDrawInfo = pSelectRegulationDrawInfo; }

  NetAppLib::UI::RegulationDrawInfo* GetActiveSelectRegulationDrawInfo() const { return m_pActiveSelectRegulationDrawInfo; }
  void SetActiveSelectRegulationDrawInfo( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo ) { m_pActiveSelectRegulationDrawInfo = pRegulationDrawInfo; }

  NetAppLib::UI::RegulationDrawInfo* GetActiveDownloadRegulationDrawInfo() const { return m_pActiveDownloadRegulationDrawInfo; }
  void SetActiveDownloadRegulationDrawInfo( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo ) { m_pActiveDownloadRegulationDrawInfo = pRegulationDrawInfo; }

  NetAppLib::UI::RegulationDrawInfo* GetActiveDeleteRegulationDrawInfo() const { return m_pActiveDeleteRegulationDrawInfo; }
  void SetActiveDeleteRegulationDrawInfo( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo ) { m_pActiveDeleteRegulationDrawInfo = pRegulationDrawInfo; }

  bool IsFatalError() const { return m_bFatalErrorFlag; }
  void SetFatalError( bool bFlag ) { m_bFatalErrorFlag = bFlag; }

  bool GetSingleNormalClearFlag() const { return m_bSingleNormalClearFlag; }
  void SetSingleNormalClearFlag( bool bFlag ) { m_bSingleNormalClearFlag = bFlag; }

  bool GetDoubleNormalClearFlag() const { return m_bDoubleNormalClearFlag; }
  void SetDoubleNormalClearFlag( bool bFlag ) { m_bDoubleNormalClearFlag = bFlag; }

  bool GetMultiNormalClearFlag() const { return m_bMultiNormalClearFlag; }
  void SetMultiNormalClearFlag( bool bFlag ) { m_bMultiNormalClearFlag = bFlag; }



  bool GetBattleRoyalNormalClearFlag() const { return m_bBattleRoyalNormalClearFlag; }
  void SetBattleRoyalNormalClearFlag( bool bFlag ) { m_bBattleRoyalNormalClearFlag = bFlag; }

  bool GetBattleRoyalSuperClearFlag() const { return m_bBattleRoyalSuperClearFlag; }
  void SetBattleRoyalSuperClearFlag( bool bFlag ) { m_bBattleRoyalSuperClearFlag = bFlag; }

  bool GetBattleRoyalHyperClearFlag() const { return m_bBattleRoyalHyperClearFlag; }
  void SetBattleRoyalHyperClearFlag( bool bFlag ) { m_bBattleRoyalHyperClearFlag = bFlag; }

  bool GetQRPartyFlag() const { return m_bQRPartyFlag; }
  void SetQRPartyFlag( bool bFlag ) { m_bQRPartyFlag = bFlag; }

private:

  NetAppLib::UI::NetAppEmptyView*                            m_pRootUpperView;                     //! 上画面用のルートビュー
  NetAppLib::UI::NetAppEmptyView*                            m_pRootLowerView;                     //! 下画面用のルートビュー

  NetApp::RegulationSelect::BootMode                         m_eBootMode;                          //! 起動モード
  Regulation::BATTLE_RULE                                    m_eBattleRule;                        //! 選択モードで起動する時に表示するバトルルール
  NetApp::RegulationSelect::DetailBootFrame                  m_eDetailBootFrame;                   //! 詳細画面を起動した Frame もしくは Proc
  gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>       m_PresetList;                         //! ROMに保存してあるプリセットのレギュレーションリスト
  gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>       m_DownloadList;                       //! サーバーから取得したレギュレーションリスト
  gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>       m_DownloadedList;                     //! SDカードに保存してあるダウンロード済みレギュレーションリスト
  NetAppLib::UI::RegulationDrawInfo*                         m_pSelectRegulationDrawInfo;          //! 選択したレギュレーション

  NetAppLib::UI::RegulationDrawInfo*                         m_pActiveSelectRegulationDrawInfo;
  NetAppLib::UI::RegulationDrawInfo*                         m_pActiveDownloadRegulationDrawInfo;
  NetAppLib::UI::RegulationDrawInfo*                         m_pActiveDeleteRegulationDrawInfo;

  bool                                                       m_bFatalErrorFlag;

  bool                                                       m_bSingleNormalClearFlag;
  bool                                                       m_bDoubleNormalClearFlag;
  bool                                                       m_bMultiNormalClearFlag;
  bool                                                       m_bBattleRoyalNormalClearFlag;
  bool                                                       m_bBattleRoyalSuperClearFlag;
  bool                                                       m_bBattleRoyalHyperClearFlag;

  bool                                                       m_bQRPartyFlag;

};



GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONSELECTWORK_H_INCLUDE
