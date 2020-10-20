// ============================================================================
/*
 * @file NetAppTrainerIconUtility.h
 * @brief app::tool::AppToolTrainerIconRendering の生成、削除のシーケンス制御などを行います。
 * @date 2015.09.04
 * @author endo_akira
 */
// ============================================================================
#if !defined( NETAPP_TRAINER_ICON_UTILITY_FRAME_H_INCLUDE )
#define NETAPP_TRAINER_ICON_UTILITY_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gfx/include/gfl2_GFGL.h>
#include "AppLib/include/Util/app_util_2d.h"
#include "AppLib/include/Tool/AppToolTrainerIconRendering.h"
#include "Savedata/include/MyStatus.h"


GFL_NAMESPACE_BEGIN(app)

  GFL_NAMESPACE_BEGIN(util)
    class Heap;
    class AppRenderingManager;
  GFL_NAMESPACE_END(util)

  GFL_NAMESPACE_BEGIN(tool)
    class AppToolTrainerIconRendering;
  GFL_NAMESPACE_END(tool)

GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Util)


class NetAppTrainerIconUtility
{
  GFL_FORBID_COPY_AND_ASSIGN( NetAppTrainerIconUtility );

public:

  NetAppTrainerIconUtility();
  virtual~NetAppTrainerIconUtility();

  void Create( app::util::AppRenderingManager* pAppRenderingManager, u32 iconNum, app::util::G2DUtil::SETUP_DISPLAY_TYPE displayType, bool bNPCOnly = false );
  void Delete();

  bool IsCreate();
  bool IsDelete();

  void Update();
  void Draw( gfl2::gfx::CtrDisplayNo displayNo );
  void SetIconObjectData( const u32 idx, const app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA& rObjectData, bool bVisibleTimerIcon = true );

  //------------------------------------------------------------------
  /**
    *  @brief    SaveData::MyStatusからICON_OBJECT_DATAを構築する
    */
  //------------------------------------------------------------------
  void  SetIconObjectData( const u32 iconIndex, const Savedata::MyStatus& rMyStatus, gfl2::lyt::LytPicture* pIconPicture, gfl2::lyt::LytPane* pVisibilityControlPane, bool bVisibleTimerIcon = true )
  {
    SetIconObjectData(iconIndex, rMyStatus.GetIcon(), pIconPicture, pVisibilityControlPane, bVisibleTimerIcon );
  }

  //------------------------------------------------------------------
  /**
    *  @brief    SaveData::MyStatus::ICON_DATAからICON_OBJECT_DATAを構築する
    */
  //------------------------------------------------------------------
  void  SetIconObjectData( const u32 iconIndex, const Savedata::MyStatus::ICON_DATA& rIconData, gfl2::lyt::LytPicture* pIconPicture, gfl2::lyt::LytPane* pVisibilityControlPane, bool bVisibleTimerIcon = true );

  //------------------------------------------------------------------
  /**
    *  @brief    SaveData::MyStatus::ICON_DATAからICON_OBJECT_DATAを構築する
    */
  //------------------------------------------------------------------
  void  SetIconObjectDataByOutputTexture( const u32 iconIndex, const Savedata::MyStatus::ICON_DATA& rIconData, gfl2::lyt::LytPicture* pIconPicture, gfl2::lyt::LytPane* pVisibilityControlPane, bool bVisibleTimerIcon = true , app::tool::AppToolTrainerIconTexture* pOutputTexture = NULL );

  //------------------------------------------------------------------
  /**
    *  @brief    SetIconObjectData()関数のアイコン貼り付けが終了しているかチェック
    */
  //------------------------------------------------------------------
  bool  IsEndIconObjectData( const u32 iconIndex );

private:

  void updateSequence();

  enum Sequence
  {
    SEQUENCE_IDLE,
    SEQUENCE_CREATE_START,
    SEQUENCE_CREATE_WAIT,
    SEQUENCE_CREATE_COMPLETE,
    SEQUENCE_DELETE_START,
    SEQUENCE_DELETE_WAIT,
    SEQUENCE_DELETE_COMPLETE
  };

  class CreateParams
  {
    public:
      app::util::AppRenderingManager*           pAppRenderingManager;
      u32                                       iconNum;
      app::util::G2DUtil::SETUP_DISPLAY_TYPE    displayType;

    CreateParams() :
      pAppRenderingManager( NULL ),
      iconNum( 0 ),
      displayType( app::util::G2DUtil::SETUP_LOWER )
    {
    }
  };


  app::util::Heap*                          m_pAppHeap;
  app::tool::AppToolTrainerIconRendering*   m_pAppPlayerIconRendering;
  CreateParams                              m_CreateParams;
  Sequence                                  m_eSequence;
  bool                                      m_bCreateRequest;
  bool                                      m_bDeleteRequest;

  //  NPCアイコンのみしか使わない（モデルを使わないので使用メモリが少なくなる）
  bool                                      m_bNPCOnly;
};


GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetAppLib)

#endif // NETAPP_TRAINER_ICON_UTILITY_FRAME_H_INCLUDE
