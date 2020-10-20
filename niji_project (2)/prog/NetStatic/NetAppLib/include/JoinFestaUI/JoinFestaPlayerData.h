// ============================================================================
/*
 * @file JoinFestaPlayerData.h
 * @brief プレイヤーデータ
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERDATA_H_INCLUDE )
#define JOINFESTAPLAYERDATA_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <util/include/gfl2_List.h>
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaBlackPlayerData.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)

class JoinFestaPlayerData : public NetAppLib::JoinFesta::JoinFestaPersonalData
{
public:

  enum Type
  {
    TYPE_GUEST,
    TYPE_VIP,
    TYPE_BLACK
  };

  enum RecordCategory
  {
    RECORD_CATEGORY_BATTLE,
    RECORD_CATEGORY_TRADE,
    RECORD_CATEGORY_GREED,
    RECORD_CATEGORY_POKEMON,
    RECORD_CATEGORY_DAILY,
    RECORD_CATEGORY_CIRCLE,
    RECORD_CATEGORY_VARIETY,
    RECORD_CATEGORY_TOTAL,
    RECORD_CATEGORY_MAX
  };

  typedef struct
  {
    u32   aRank[ RECORD_CATEGORY_MAX ];
  } RecordRank;

  JoinFestaPlayerData();
  JoinFestaPlayerData( const JoinFestaPlayerData& data );
  virtual~JoinFestaPlayerData();

  JoinFestaPlayerData& operator = ( const JoinFestaPlayerData& data );

  void SetSelect( bool bSelect ) { m_bSelect = bSelect; }
  bool IsSelect() const { return m_bSelect; }

  void SetPassive( bool bPassive ) { m_bPassive = bPassive; }
  bool IsPassive() const { return m_bPassive; }

  void SetType( Type eType ){ m_eType = eType; }
  bool IsGuest() const { return ( m_eType == TYPE_GUEST ); }
  bool IsVip() const { return ( m_eType == TYPE_VIP ); }
  bool IsBlack() const { return ( m_eType == TYPE_BLACK ); }

  const NetAppLib::JoinFestaUI::JoinFestaBlackPlayerData::CoreData& GetBlackPlayerCoreData() const { return m_BlackPlayerCoreData; }
  void SetBlackPlayerCoreData( const NetAppLib::JoinFestaUI::JoinFestaBlackPlayerData::CoreData& data ) { m_BlackPlayerCoreData = data; }

  void SetRank( RecordCategory eRecordCategory, u32 rank ) { m_RecordRank.aRank[ static_cast<int>( eRecordCategory ) ] = rank; }
  u32 GetRank( RecordCategory eRecordCategory ) { return m_RecordRank.aRank[ static_cast<int>( eRecordCategory ) ]; }

  /*  battlefes  */
  bool  BFC_GetPokeparaAvailability(void) const                             {return (!!m_pCoreParam);}
  void  BFC_SetRecentPartner(const bool bRecentPartner)                     {m_bRecentPartner = bRecentPartner;}
  bool  BFC_GetRecentPartner(void) const                                    {return m_bRecentPartner;}
  void  BFC_SetPokepara(const pml::pokepara::CoreParam* pCoreParam)         {m_pCoreParam = pCoreParam;}
  const pml::pokepara::CoreParam* BFC_GetPokepara(void) const               {return m_pCoreParam;}

  /*  photo  */
  enum  PhotoDataStatus
  {
    PhotoData_NotAvailable,   /**<  無効                        */
    PhotoData_Available,      /**<  有効                        */
    PhotoData_AvailableNew,   /**<  有効であり、閲覧していない  */
    
    PhotoData_
  };
  PhotoDataStatus   FS_GetPhotoDataStatus(void) const                                {return m_PhotoDataStatus;}
  void              FS_SetPhotoDataStatus(const PhotoDataStatus& rPhotoDataStatus)   {m_PhotoDataStatus  = rPhotoDataStatus;}
  void              FS_SetPhotoDataStatus_NotAvailable(void)                         {FS_SetPhotoDataStatus(PhotoData_NotAvailable);}
  void              FS_SetPhotoDataStatus_Available(void)                            {FS_SetPhotoDataStatus(PhotoData_Available);}
  void              FS_SetPhotoDataStatus_AvailableNew(void)                         {FS_SetPhotoDataStatus(PhotoData_AvailableNew);}
  bool              FS_IsPhotoDataAvailable(void) const                              {return (FS_GetPhotoDataStatus() != PhotoData_NotAvailable);}

private:

  NetAppLib::JoinFestaUI::JoinFestaBlackPlayerData::CoreData    m_BlackPlayerCoreData;

  RecordRank    m_RecordRank;

  Type            m_eType;
  bool            m_bSelect;
  bool            m_bPassive;
  /*  BFC  */
  const pml::pokepara::CoreParam*   m_pCoreParam;         /**<  所有権は持たない  */
  bool                              m_bRecentPartner;
  /*  photo  */
  PhotoDataStatus m_PhotoDataStatus;

};


typedef gfl2::util::List<NetAppLib::JoinFestaUI::JoinFestaPlayerData>             JoinFestPlayerDataList;
typedef gfl2::util::List<NetAppLib::JoinFestaUI::JoinFestaPlayerData>::Iterator   ItJoinFestPlayerDataList;


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTAPLAYERDATA_H_INCLUDE
