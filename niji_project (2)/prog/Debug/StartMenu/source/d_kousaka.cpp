#if PM_DEBUG
//=============================================================================
/**
 * @file    d_kousaka.cpp
 * @brief   幸坂用ランチャーリスト処理
 * @author  kousaka_shin
 * @date    
 */
//=============================================================================
#include <Debug/StartMenu/include/StartLauncher.h>
#include <Debug/StartMenu/include/d_kousaka.h>



GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)


// 項目文字列データテーブル
const u32 LauncherKousaka::ItemList[LauncherKousaka::LIST_MAX] =
{
  KousakaDebugMenuItem_01,
  KousakaDebugMenuItem_02,
  KousakaDebugMenuItem_03,
  KousakaDebugMenuItem_04,
  KousakaDebugMenuItem_05
};

static bool s_isVsDemoRunning = false;

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
LauncherKousaka::LauncherKousaka(void)
{

#ifdef  GF_PLATFORM_CTR
  GFL_PRINT("imagedb::functest[%d]\n",imgdb::JpegLoader::GetSysMakerNoteDataSize());

  u8 message[100];
  //type03
  u8 publicKeyData[] =
  {
      //0x30, 0x7c, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x6b, 0x00, 0x30, 0x68, 0x02, 0x61, 0x00, 0xdb, 0x53, 0x6e, 0x5e, 0xb7, 0xef, 0xfe, 0xa4, 0xc8, 0x2a, 0x73, 0xf4, 0xee, 0xca, 0x08, 0x11, 0x2e, 0xfc, 0x32, 0xcb, 0x93, 0xdf, 0xd2, 0x9b, 0xea, 0xf2, 0x54, 0x64, 0xec, 0x36, 0x0e, 0x69, 0x37, 0xbe, 0x10, 0x6e, 0x46, 0xd9, 0x54, 0x21, 0x7d, 0xf7, 0x33, 0x35, 0xfc, 0x15, 0x98, 0xb7, 0x80, 0x11, 0xde, 0x82, 0xdf, 0x1e, 0xa4, 0x45, 0x4a, 0xdd, 0x3c, 0x57, 0x5d, 0x2f, 0xf9, 0x96, 0xdb, 0x56, 0x1f, 0x60, 0xeb, 0x5f, 0x60, 0xc0, 0xbe, 0x6c, 0xf1, 0xbf, 0x26, 0x72, 0x13, 0x70, 0x83, 0x9d, 0x08, 0xc6, 0x66, 0x16, 0xb1, 0xce, 0x2a, 0x44, 0x40, 0x87, 0xa2, 0xfd, 0x1c, 0xcf, 0x02, 0x03, 0x01, 0x00, 0x01, 
      0x30, 0x7C, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x6B, 0x00, 0x30, 0x68, 0x02, 0x61, 0x00, 0xB6, 0x1E, 0x19, 0x20, 0x91, 0xF9, 0x0A, 0x8F, 0x76, 0xA6, 0xEA, 0xAA, 0x9A, 0x3C, 0xE5, 0x8C, 0x86, 0x3F, 0x39, 0xAE, 0x25, 0x3F, 0x03, 0x78, 0x16, 0xF5, 0x97, 0x58, 0x54, 0xE0, 0x7A, 0x9A, 0x45, 0x66, 0x01, 0xE7, 0xC9, 0x4C, 0x29, 0x75, 0x9F, 0xE1, 0x55, 0xC0, 0x64, 0xED, 0xDF, 0xA1, 0x11, 0x44, 0x3F, 0x81, 0xEF, 0x1A, 0x42, 0x8C, 0xF6, 0xCD, 0x32, 0xF9, 0xDA, 0xC9, 0xD4, 0x8E, 0x94, 0xCF, 0xB3, 0xF6, 0x90, 0x12, 0x0E, 0x8E, 0x6B, 0x91, 0x11, 0xAD, 0xDA, 0xF1, 0x1E, 0x7C, 0x96, 0x20, 0x8C, 0x37, 0xC0, 0x14, 0x3F, 0xF2, 0xBF, 0x3D, 0x7E, 0x83, 0x11, 0x41, 0xA9, 0x73, 0x02, 0x03, 0x01, 0x00, 0x01,
  };


  u8 privateKeyData[] =
  {
      //0x30, 0x82, 0x01, 0xcb, 0x02, 0x01, 0x00, 0x02, 0x61, 0x00, 0xdb, 0x53, 0x6e, 0x5e, 0xb7, 0xef, 0xfe, 0xa4, 0xc8, 0x2a, 0x73, 0xf4, 0xee, 0xca, 0x08, 0x11, 0x2e, 0xfc, 0x32, 0xcb, 0x93, 0xdf, 0xd2, 0x9b, 0xea, 0xf2, 0x54, 0x64, 0xec, 0x36, 0x0e, 0x69, 0x37, 0xbe, 0x10, 0x6e, 0x46, 0xd9, 0x54, 0x21, 0x7d, 0xf7, 0x33, 0x35, 0xfc, 0x15, 0x98, 0xb7, 0x80, 0x11, 0xde, 0x82, 0xdf, 0x1e, 0xa4, 0x45, 0x4a, 0xdd, 0x3c, 0x57, 0x5d, 0x2f, 0xf9, 0x96, 0xdb, 0x56, 0x1f, 0x60, 0xeb, 0x5f, 0x60, 0xc0, 0xbe, 0x6c, 0xf1, 0xbf, 0x26, 0x72, 0x13, 0x70, 0x83, 0x9d, 0x08, 0xc6, 0x66, 0x16, 0xb1, 0xce, 0x2a, 0x44, 0x40, 0x87, 0xa2, 0xfd, 0x1c, 0xcf, 0x02, 0x03, 0x01, 0x00, 0x01, 0x02, 0x61, 0x00, 0xcf, 0xf6, 0xd6, 0x4a, 0x9d, 0xe2, 0xc4, 0xf3, 0xfe, 0x90, 0xb2, 0x8e, 0xe6, 0xf5, 0x4e, 0x9c, 0xec, 0x8e, 0x5a, 0xa3, 0xca, 0x3d, 0x8a, 0xf8, 0x28, 0xd9, 0x4a, 0xc8, 0xbf, 0x69, 0xaa, 0x54, 0x7e, 0x74, 0x8c, 0x23, 0xe4, 0xae, 0x22, 0x25, 0xf7, 0xc9, 0x34, 0x17, 0xd6, 0x45, 0xac, 0x1d, 0x61, 0xad, 0xa5, 0x98, 0x8b, 0xc0, 0x00, 0xac, 0x18, 0x57, 0x63, 0xcc, 0x4d, 0x3e, 0xe6, 0x40, 0x3e, 0x64, 0x95, 0x6b, 0xcd, 0xff, 0xe9, 0x19, 0x87, 0xdd, 0xd6, 0x6d, 0xeb, 0x61, 0xe7, 0x7c, 0x8f, 0xdd, 0xdc, 0x5b, 0x36, 0x88, 0xe7, 0x7c, 0xaf, 0x22, 0x3a, 0x9d, 0x7f, 0xc0, 0xf5, 0x51, 0x02, 0x31, 0x00, 0xf1, 0xf7, 0xb6, 0xea, 0xf3, 0x03, 0x93, 0xd0, 0xdb, 0xac, 0x54, 0xd2, 0xd7, 0x54, 0x11, 0xb7, 0xe1, 0x7e, 0x05, 0x16, 0x68, 0x78, 0x60, 0x34, 0x17, 0xdd, 0xec, 0x40, 0x9e, 0xbf, 0x74, 0xbb, 0x0d, 0xb4, 0x51, 0xbb, 0x7b, 0xe0, 0xb6, 0x76, 0xef, 0xb2, 0xdc, 0xce, 0xd9, 0xe8, 0x84, 0x9b, 0x02, 0x31, 0x00, 0xe8, 0x0b, 0x93, 0x03, 0xe8, 0x69, 0x8c, 0x91, 0xd3, 0xde, 0x2c, 0x8e, 0x1e, 0x03, 0xf7, 0x7e, 0xfc, 0x97, 0xb6, 0xd3, 0x6e, 0xc2, 0x39, 0xe1, 0x49, 0xb2, 0x83, 0xc2, 0xca, 0xe5, 0xb7, 0x42, 0x7c, 0x3c, 0x29, 0x20, 0x57, 0x63, 0x50, 0x52, 0x4c, 0x97, 0x6e, 0x0e, 0x0d, 0x26, 0x99, 0xdd, 0x02, 0x30, 0x37, 0x3f, 0x8c, 0x70, 0x91, 0x14, 0x5c, 0x07, 0xf0, 0x60, 0xa4, 0xed, 0x8f, 0x65, 0x0e, 0xb6, 0xa1, 0xb2, 0x18, 0x8f, 0x49, 0x43, 0x53, 0x14, 0x11, 0xef, 0x06, 0x26, 0xab, 0x18, 0xa4, 0x1a, 0xa1, 0xc2, 0x6e, 0x77, 0x14, 0x8d, 0xf6, 0x9b, 0xa9, 0x97, 0x79, 0x7b, 0x71, 0x9d, 0xf9, 0xc9, 0x02, 0x30, 0x65, 0x7e, 0x52, 0x82, 0x5e, 0x10, 0x74, 0x8c, 0x80, 0x27, 0xe8, 0x43, 0x0c, 0xfb, 0xdd, 0x2f, 0xec, 0xd9, 0x8a, 0xfc, 0xf5, 0xaf, 0xfc, 0x52, 0xfe, 0xc2, 0x17, 0x01, 0x87, 0x6f, 0x6c, 0x75, 0xa7, 0x13, 0x45, 0x45, 0xc1, 0x13, 0x4e, 0x59, 0x80, 0xe8, 0xa7, 0x19, 0x7c, 0x45, 0x0a, 0xfd, 0x02, 0x31, 0x00, 0xb0, 0x4a, 0x9c, 0xc0, 0x43, 0x4c, 0xdc, 0x1a, 0x46, 0xca, 0xf8, 0x84, 0x2e, 0xe6, 0x22, 0x11, 0xf0, 0x1e, 0x59, 0xd3, 0x5a, 0x76, 0x01, 0xec, 0xaa, 0x35, 0x75, 0x55, 0xa4, 0x99, 0xc0, 0xc7, 0xed, 0xe5, 0xa5, 0xef, 0x53, 0x6e, 0x1f, 0xb9, 0x4b, 0x5b, 0x85, 0xa6, 0xe5, 0x93, 0x15, 0x75, 
      0x30, 0x82, 0x01, 0xC9, 0x02, 0x01, 0x00, 0x02, 0x61, 0x00, 0xB6, 0x1E, 0x19, 0x20, 0x91, 0xF9, 0x0A, 0x8F, 0x76, 0xA6, 0xEA, 0xAA, 0x9A, 0x3C, 0xE5, 0x8C, 0x86, 0x3F, 0x39, 0xAE, 0x25, 0x3F, 0x03, 0x78, 0x16, 0xF5, 0x97, 0x58, 0x54, 0xE0, 0x7A, 0x9A, 0x45, 0x66, 0x01, 0xE7, 0xC9, 0x4C, 0x29, 0x75, 0x9F, 0xE1, 0x55, 0xC0, 0x64, 0xED, 0xDF, 0xA1, 0x11, 0x44, 0x3F, 0x81, 0xEF, 0x1A, 0x42, 0x8C, 0xF6, 0xCD, 0x32, 0xF9, 0xDA, 0xC9, 0xD4, 0x8E, 0x94, 0xCF, 0xB3, 0xF6, 0x90, 0x12, 0x0E, 0x8E, 0x6B, 0x91, 0x11, 0xAD, 0xDA, 0xF1, 0x1E, 0x7C, 0x96, 0x20, 0x8C, 0x37, 0xC0, 0x14, 0x3F, 0xF2, 0xBF, 0x3D, 0x7E, 0x83, 0x11, 0x41, 0xA9, 0x73, 0x02, 0x03, 0x01, 0x00, 0x01, 0x02, 0x60, 0x77, 0x54, 0x55, 0x66, 0x8F, 0xFF, 0x3C, 0xBA, 0x30, 0x26, 0xC2, 0xD0, 0xB2, 0x6B, 0x80, 0x85, 0x89, 0x59, 0x58, 0x34, 0x11, 0x57, 0xAE, 0xB0, 0x3B, 0x6B, 0x04, 0x95, 0xEE, 0x57, 0x80, 0x3E, 0x21, 0x86, 0xEB, 0x6C, 0xB2, 0xEB, 0x62, 0xA7, 0x1D, 0xF1, 0x8A, 0x3C, 0x9C, 0x65, 0x79, 0x07, 0x76, 0x70, 0x96, 0x1B, 0x3A, 0x61, 0x02, 0xDA, 0xBE, 0x5A, 0x19, 0x4A, 0xB5, 0x8C, 0x32, 0x50, 0xAE, 0xD5, 0x97, 0xFC, 0x78, 0x97, 0x8A, 0x32, 0x6D, 0xB1, 0xD7, 0xB2, 0x8D, 0xCC, 0xCB, 0x2A, 0x3E, 0x01, 0x4E, 0xDB, 0xD3, 0x97, 0xAD, 0x33, 0xB8, 0xF2, 0x8C, 0xD5, 0x25, 0x05, 0x42, 0x51, 0x02, 0x31, 0x00, 0xEE, 0x48, 0x58, 0xBC, 0x36, 0x7E, 0xFD, 0xE4, 0x2A, 0x6C, 0x0E, 0x15, 0x7E, 0xAB, 0x32, 0x7B, 0x48, 0x84, 0x5E, 0x55, 0xB7, 0xBA, 0x8E, 0x0A, 0x1D, 0x17, 0xCC, 0x53, 0xC4, 0x66, 0x84, 0x59, 0xD7, 0x90, 0xE9, 0xFB, 0xD9, 0x0D, 0x50, 0x18, 0x47, 0x88, 0xB2, 0xF3, 0xE6, 0x26, 0xC8, 0xC5, 0x02, 0x31, 0x00, 0xC3, 0xA8, 0xA9, 0xCF, 0x94, 0x1E, 0xDB, 0x0B, 0xDF, 0x2D, 0x57, 0x2D, 0xBD, 0xC3, 0x9D, 0x00, 0xBA, 0x1A, 0x9F, 0xB7, 0xC1, 0xFC, 0x6A, 0xC4, 0xF4, 0x39, 0xF9, 0x9A, 0x33, 0x8A, 0xCC, 0x8D, 0x5A, 0x78, 0x3C, 0x4C, 0x08, 0x1A, 0xC8, 0xB9, 0xB9, 0xDA, 0x8E, 0x89, 0xF2, 0x6B, 0x9C, 0xD7, 0x02, 0x30, 0x5F, 0x8D, 0xC1, 0x8F, 0xD0, 0x66, 0xC2, 0x97, 0xC1, 0xEF, 0x4D, 0x02, 0x8F, 0xA6, 0x45, 0x7B, 0x2C, 0xCC, 0x70, 0x2B, 0xFD, 0x9E, 0xA1, 0x21, 0x2B, 0x23, 0x84, 0xFB, 0xBE, 0x91, 0xC1, 0xD7, 0x69, 0x5B, 0xA2, 0x89, 0x2D, 0x34, 0x6A, 0x7F, 0x22, 0x08, 0x8E, 0x82, 0xC2, 0x33, 0xA1, 0x65, 0x02, 0x30, 0x3E, 0x7E, 0xB0, 0x06, 0xAA, 0x94, 0x14, 0x79, 0x3D, 0x46, 0xD9, 0x32, 0xB6, 0x0B, 0x36, 0x29, 0xD6, 0x47, 0xCC, 0xBE, 0x40, 0x0A, 0xE4, 0xFB, 0x95, 0xDA, 0xB2, 0xBC, 0x2B, 0x3C, 0x69, 0x4B, 0xB9, 0x1A, 0xF0, 0x7F, 0x84, 0x93, 0x31, 0x8C, 0x09, 0x2E, 0x23, 0x1B, 0x32, 0xC0, 0x66, 0xAF, 0x02, 0x30, 0x68, 0xC2, 0xD3, 0x45, 0x0C, 0x8E, 0x66, 0x1C, 0xAA, 0x82, 0xF1, 0x8C, 0xF1, 0xEF, 0x9B, 0xEC, 0xB7, 0xAF, 0xD1, 0x11, 0xDF, 0xCC, 0xEA, 0x6E, 0xA6, 0x09, 0xFC, 0x23, 0xFA, 0x7B, 0x09, 0x9C, 0x23, 0xA4, 0x05, 0x5D, 0xF6, 0x3E, 0x1F, 0x59, 0x28, 0xE9, 0x45, 0xC1, 0x17, 0xBD, 0xFD, 0xEE
  };



  const u8* data = "000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888";

  u32 len = sizeof(message);

  bool newEnable = gfl2::heap::debug::IsDefaultNewAssertEnabled();
  if( newEnable )
  {
    gfl2::heap::debug::DisableDefaultNewAssert();
  }
  
  /*
  bool result = mw::zorro::OpssrCmcSha1_Sign( message , len , data , 90 , 8 , publicKeyData, sizeof(publicKeyData),
                                          privateKeyData, sizeof(privateKeyData), 768);
  GFL_PRINT("sign [%d]\n",result);
  */
  if( newEnable )
  {
    gfl2::heap::debug::EnableDefaultNewAssert();
  }


#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
LauncherKousaka::~LauncherKousaka()
{

}

//-----------------------------------------------------------------------------
/**
 * @func    GetArcDataID
 * @brief   アークデータID取得
 * @date    2015.02.18
 *
 * @return  アークデータID
 */
//-----------------------------------------------------------------------------
gfl2::fs::ArcFile::ARCDATID LauncherKousaka::GetArcDataID(void)
{
  return GARC_debug_message_d_kousaka_DAT;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListTable
 * @brief   項目文字列データテーブル取得
 * @date    2015.02.18
 *
 * @return  文字列データテーブル
 */
//-----------------------------------------------------------------------------
const u32 * LauncherKousaka::GetListTable(void)
{
  return ItemList;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListTable
 * @brief   項目文字列データテーブルの要素数取得
 * @date    2015.02.18
 *
 * @return  文字列データテーブルの要素数
 */
//-----------------------------------------------------------------------------
u32 LauncherKousaka::GetListTableSize(void)
{
  return LIST_MAX;
}

//-----------------------------------------------------------------------------
/**
 * @func    SelectListener
 * @brief   決定時のリスナー
 * @date    2015.02.18
 *
 * @param   pos       カーソル位置 ( キャンセル時はCURSOR_POS_CANCEL )
 * @param   launcher  呼び出し元のランチャークラス
 *
 * @return  ランチャーへのリクエストID
 */
//-----------------------------------------------------------------------------
launcher::ListData::RequestID LauncherKousaka::SelectListener( u32 pos, launcher::LauncherProc * launcher )
{
  // キャンセル
  if( pos == launcher::LauncherProc::CURSOR_POS_CANCEL )
  {
    launcher->CreateListData<StartLauncher>();
    return launcher::ListData::REQID_RESET;
  }

  switch( ItemList[pos] )
  {
  case KousakaDebugMenuItem_03://観戦受信
    {
#ifdef  GF_PLATFORM_CTR
      /*
      gfl2::ui::Initialize();

      gfl2::heap::HeapBase*  heap  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE );
      gfl2::ui::DeviceManager* pDeviceManager = NULL;
      pDeviceManager = GFL_NEW(heap) gfl2::ui::CTR_DeviceManager( heap );
      pDeviceManager->Main();
      pDeviceManager->Main();
      gfl2::ui::Button* pButton      = pDeviceManager->GetButton(1);
      if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
      {
        GFL_PRINT("SetTimeDisable\n");
        LiveRecEvent::SetTimeDisable( true );
      }
      else
      {
        GFL_PRINT("SetTimeEnable\n");
        LiveRecEvent::SetTimeDisable( false );
      }

      GFL_SAFE_DELETE( pDeviceManager );
      */

      GameSys::GameEventManager* pGameEventManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameEventManager();
      GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, LiveRecEvent>( pGameEventManager );
      return launcher::ListData::REQID_FINISH;
#endif
    }
  case KousakaDebugMenuItem_01://クイックマッチテスト
    {
      #ifdef  GF_PLATFORM_CTR
      Test::LocalQuickMatchTest::LocalQuickMatchTest * proc = NIJI_PROC_CALL< GameSys::GameProcManager, Test::LocalQuickMatchTest::LocalQuickMatchTest >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );
      #endif
      return launcher::ListData::REQID_CALL;
    }
  case KousakaDebugMenuItem_02://タッチパネルテスト
    {
      //
      gfl2::ui::Initialize();

      gfl2::heap::HeapBase*  heap  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE );

      gfl2::ui::DeviceManager* pDeviceManager = NULL;
  
    #if defined(GF_PLATFORM_CTR)
      pDeviceManager = GFL_NEW(heap) gfl2::ui::CTR_DeviceManager( heap );
    #elif defined(GF_PLATFORM_WIN32)
      pDeviceManager = GFL_NEW(heap) gfl2::ui::WIN32_DeviceManager( heap, gfl2::gfx::GFGL::GetHwnd() );
    #endif
  
      gfl2::ui::Button* pButton      = pDeviceManager->GetButton(1);
      //gfl2::ui::VectorDevice* pCrossKey    = pDeviceManager->GetCrossKey(1);
      //gfl2::ui::VectorDevice* pStick       = pDeviceManager->GetStick(0);
      gfl2::ui::TouchPanel* pTouchPanel  = pDeviceManager->GetTouchPanel(0);

      //float stick_x = 0.0f;
      //float stick_y = 0.0f;

      u16 touch_x = 0;
      u16 touch_y = 0;

    for ever
    {
      pDeviceManager->Main();

      //TOUCH
      if( pTouchPanel->GetX() != touch_x )
      {
        touch_x = pTouchPanel->GetX();
        GFL_PRINT("TOUCH X[%d]\n",touch_x);
      }

      if( pTouchPanel->GetY() != touch_y )
      {
        touch_y = pTouchPanel->GetY();
        GFL_PRINT("TOUCH Y[%d]\n",touch_y);
      }

      GFL_PRINT( "move x[%d]y[%d]\n" , pTouchPanel->GetMoveX() , pTouchPanel->GetMoveY() );

      //終了
      if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
      {
        GFL_PRINT("exit\n");
        break;
      }

      gfl2::thread::ThreadStatic::CurrentSleep(1);
    }

    return launcher::ListData::REQID_NONE;
    }
  case KousakaDebugMenuItem_04://セーブロードテスト
    {
      #ifdef  GF_PLATFORM_CTR
      Test::NijiSaveLoadTest::NijiSaveLoadTest * proc = NIJI_PROC_CALL< GameSys::GameProcManager, Test::NijiSaveLoadTest::NijiSaveLoadTest >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );
      #endif
      return launcher::ListData::REQID_CALL;
    }
  case KousakaDebugMenuItem_05://クイックマッチアプリ
    {
      #ifdef  GF_PLATFORM_CTR
      //static NetEvent::QuickMatch::EVENT_PARAM param;
      //NetEvent::QuickMatch::QuickMatchEvent::StartEventForBattleTree( GFL_SINGLETON_INSTANCE( GameSys::GameManager ) , &param );
      gfl2::heap::HeapBase*  heap  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE );
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon()->Clear( heap );//全消去

      //日本語
      {
        
        const char*               pFilePath   = "convert/save_jp.dat";
        gfl2::fs::DirectSdmcFile* pSDMC       = gfl2::fs::System::CreateDirectSdmcFile(heap, pFilePath, gfl2::fs::Define::OpenMode::READ);

        u8* pBuffer = GFL_NEW_ARRAY( heap ) u8[64*1024];

        if(pSDMC)
        {
          size_t            readBytes;
          const size_t      bufferBytes   = 64*1024;
          const size_t      requestBytes  = bufferBytes;
          gfl2::fs::Result  fsResult;

          fsResult  = pSDMC->Read(&readBytes, pBuffer, bufferBytes, requestBytes);
          if(fsResult.IsSuccess())
          {
            _ConvertVCSavedata( pBuffer , SAVE_TYPE_JP );

          }
        }

        GFL_SAFE_DELETE_ARRAY( pBuffer );
      }

      //英語
      {
        const char*               pFilePath   = "convert/save_en.dat";
        gfl2::fs::DirectSdmcFile* pSDMC       = gfl2::fs::System::CreateDirectSdmcFile(heap, pFilePath, gfl2::fs::Define::OpenMode::READ);

        u8* pBuffer = GFL_NEW_ARRAY( heap ) u8[64*1024];

        if(pSDMC)
        {
          size_t            readBytes;
          const size_t      bufferBytes   = 64*1024;
          const size_t      requestBytes  = bufferBytes;
          gfl2::fs::Result  fsResult;

          fsResult  = pSDMC->Read(&readBytes, pBuffer, bufferBytes, requestBytes);
          if(fsResult.IsSuccess())
          {
            _ConvertVCSavedata( pBuffer , SAVE_TYPE_EN );

          }
        }

        GFL_SAFE_DELETE_ARRAY( pBuffer );
      }

      //sango
      {
        const char*               pFilePath   = "convert/save_sango.dat";
        gfl2::fs::DirectSdmcFile* pSDMC       = gfl2::fs::System::CreateDirectSdmcFile(heap, pFilePath, gfl2::fs::Define::OpenMode::READ);

        u8* pBuffer = GFL_NEW_ARRAY( heap ) u8[1024*1024];

        if(pSDMC)
        {
          size_t            readBytes;
          const size_t      bufferBytes   = 1024*1024;
          const size_t      requestBytes  = bufferBytes;
          gfl2::fs::Result  fsResult;

          fsResult  = pSDMC->Read(&readBytes, pBuffer, bufferBytes, requestBytes);
          if(fsResult.IsSuccess())
          {
            u8* pBox1Top = &pBuffer[0x33000];
            pml::pokepara::PokemonParam* pTemp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap );
            for( u32 index = 0 ; index < 30 ; index++ )
            {
              pTemp->Deserialize_Core( pBox1Top );
              pBox1Top += 232;

              GFL_PRINT("sango monsno[%d]\n",pTemp->GetMonsNo());

              GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon()->PutPokemonEmptyTrayAll( pTemp , 2 , heap );
            }
            GFL_SAFE_DELETE( pTemp );
          }
        }

        GFL_SAFE_DELETE_ARRAY( pBuffer );
      }


      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SaveStart( heap );
      while( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SaveMain() == gfl2::fs::FsCtrBackupSystem::RESULT_OK )
      {
        gfl2::thread::ThreadStatic::CurrentSleep(1);
      }

      launcher->CreateListData<StartLauncher>();
      return launcher::ListData::REQID_RESET;

      #endif
      //return launcher::ListData::REQID_CALL;
    }
  }

  return launcher::ListData::REQID_NONE;
}



#ifdef  GF_PLATFORM_CTR
//------------------------------------------------------------------
/**
* @brief 受信完了イベント
*
* @param pData 受信データ
* @return 受信を続けるかどうか( true : 続ける , false : 続けない)
*/
//------------------------------------------------------------------
bool LiveRecEvent::OnRecvComplete( const void* pData , size_t dataSize )
{
  if( m_pBattleSetupParam->recBuffer == NULL )
  {//初回データ
    //GFL_ASSERT( dataSize == sizeof( ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA ) );
    if( dataSize != sizeof(ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA) )
    {
      GFL_PRINT("skip 1\n");
      return true;
    }

    const ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA* pRecData = (const ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA*)(pData);
    m_pRecorderSaveData->CreateBattleSetupParamDebug( m_pBattleSetupParam , pRecData , gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ) );

    m_pProc->SetMode( 1 );
  }
  else if( m_pBattleSetupParam->recBuffer != NULL && m_isFirstRecvComplete == false )
  {//選出リスト
    s8 join_order[ BTL_CLIENT_NUM ][ pml::PokeParty::MAX_MEMBERS ];
    //GFL_ASSERT( dataSize == sizeof(join_order) );
    if( dataSize != sizeof( join_order ) )
    {
      GFL_PRINT("skip 2\n");
      return true;
    }

    std::memcpy( join_order , pData , sizeof(join_order) );

    for( u32 index = 0 ; index < 2 ; index++ )
    {//相手と自分で2ループ
      gfl2::heap::HeapBase*  heap  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE );
      pml::PokeParty* temp = GFL_NEW( heap->GetLowerHandle() ) pml::PokeParty( heap->GetLowerHandle() );
      temp->CopyFrom( *m_pBattleSetupParam->party[index] );

      m_pBattleSetupParam->party[index]->Clear();
      for( u32 pokeIndex = 0 ; pokeIndex < pml::PokeParty::MAX_MEMBERS ; ++pokeIndex )
      {
        s8 joinIndex = join_order[index][pokeIndex];
        if( joinIndex == -1 )
        {
          break;
        }

        const pml::pokepara::PokemonParam *pp = temp->GetMemberPointer( joinIndex );
        m_pBattleSetupParam->party[index]->AddMember( *pp );
        GFL_PRINT("Add [%d][%d][%d][%d]\n",index,pokeIndex,joinIndex,pp->GetMonsNo());
      }

      GFL_SAFE_DELETE( temp );
    }

    m_isFirstRecvComplete = true;
    m_pProc->End();
  }
  else if( dataSize < sizeof( ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA ) )
  {//録画データ
    m_pBattleSetupParam->recDataSize = dataSize - sizeof(u64);
    m_pBattleSetupParam->recRandSeed = *(reinterpret_cast<const u32*>( pData ));
    ::std::memcpy( m_pBattleSetupParam->recBuffer , &reinterpret_cast<const u32*>( pData )[2] , m_pBattleSetupParam->recDataSize );
  }

  m_pBattleSetupParam->isLiveRecPlayMode = true;

  return true;
}



//MonsNo変換テーブル
static const u16 VC_MONSNO_MAX = 151;
static const u16 MONSNO_CONVERT_TABLE[VC_MONSNO_MAX][2] =
{
  { 1 , 112 } , //サイドン
  { 2 , 115 } , //ガルーラ
  { 3 , 32 } , //ニドラン♂
  { 4 , 35 } , //ピッピ
  { 5 , 21 } , //オニスズメ
  { 6 , 100 } , //ビリリダマ
  { 7 , 34 } , //ニドキング
  { 8 , 80 } , //ヤドラン
  { 9 , 2 } , //フシギソウ
  { 10 , 103 } , //ナッシー
  { 11 , 108 } , //ベロリンガ
  { 12 , 102 } , //タマタマ
  { 13 , 88 } , //ベトベター
  { 14 , 94 } , //ゲンガー
  { 15 , 29 } , //ニドラン♀
  { 16 , 31 } , //ニドクイン
  { 17 , 104 } , //カラカラ
  { 18 , 111 } , //サイホーン
  { 19 , 131 } , //ラプラス
  { 20 , 59 } , //ウインディ
  { 21 , 151 } , //ミュウ
  { 22 , 130 } , //ギャラドス
  { 23 , 90 } , //シェルダー
  { 24 , 72 } , //メノクラゲ
  { 25 , 92 } , //ゴース
  { 26 , 123 } , //ストライク
  { 27 , 120 } , //ヒトデマン
  { 28 , 9 } , //カメックス
  { 29 , 127 } , //カイロス
  { 30 , 114 } , //モンジャラ
  { 33 , 58 } , //ガーディ
  { 34 , 95 } , //イワーク
  { 35 , 22 } , //オニドリル
  { 36 , 16 } , //ポッポ
  { 37 , 79 } , //ヤドン
  { 38 , 64 } , //ユンゲラー
  { 39 , 75 } , //ゴローン
  { 40 , 113 } , //ラッキー
  { 41 , 67 } , //ゴーリキー
  { 42 , 122 } , //バリヤード
  { 43 , 106 } , //サワムラー
  { 44 , 107 } , //エビワラー
  { 45 , 24 } , //アーボック
  { 46 , 47 } , //パラセクト
  { 47 , 54 } , //コダック
  { 48 , 96 } , //スリープ
  { 49 , 76 } , //ゴローニャ
  { 51 , 126 } , //ブーバー
  { 53 , 125 } , //エレブー
  { 54 , 82 } , //レアコイル
  { 55 , 109 } , //ドガース
  { 57 , 56 } , //マンキー
  { 58 , 86 } , //パウワウ
  { 59 , 50 } , //ディグダ
  { 60 , 128 } , //ケンタロス
  { 64 , 83 } , //カモネギ
  { 65 , 48 } , //コンパン
  { 66 , 149 } , //カイリュー
  { 70 , 84 } , //ドードー
  { 71 , 60 } , //ニョロモ
  { 72 , 124 } , //ルージュラ
  { 73 , 146 } , //ファイヤー
  { 74 , 144 } , //フリーザー
  { 75 , 145 } , //サンダー
  { 76 , 132 } , //メタモン
  { 77 , 52 } , //ニャース
  { 78 , 98 } , //クラブ
  { 82 , 37 } , //ロコン
  { 83 , 38 } , //キュウコン
  { 84 , 25 } , //ピカチュウ
  { 85 , 26 } , //ライチュウ
  { 88 , 147 } , //ミニリュウ
  { 89 , 148 } , //ハクリュー
  { 90 , 140 } , //カブト
  { 91 , 141 } , //カブトプス
  { 92 , 116 } , //タッツー
  { 93 , 117 } , //シードラ
  { 96 , 27 } , //サンド
  { 97 , 28 } , //サンドパン
  { 98 , 138 } , //オムナイト
  { 99 , 139 } , //オムスター
  { 100 , 39 } , //プリン
  { 101 , 40 } , //プクリン
  { 102 , 133 } , //イーブイ
  { 103 , 136 } , //ブースター
  { 104 , 135 } , //サンダース
  { 105 , 134 } , //シャワーズ
  { 106 , 66 } , //ワンリキー
  { 107 , 41 } , //ズバット
  { 108 , 23 } , //アーボ
  { 109 , 46 } , //パラス
  { 110 , 61 } , //ニョロゾ
  { 111 , 62 } , //ニョロボン
  { 112 , 13 } , //ビードル
  { 113 , 14 } , //コクーン
  { 114 , 15 } , //スピアー
  { 116 , 85 } , //ドードリオ
  { 117 , 57 } , //オコリザル
  { 118 , 51 } , //ダグトリオ
  { 119 , 49 } , //モルフォン
  { 120 , 87 } , //ジュゴン
  { 123 , 10 } , //キャタピー
  { 124 , 11 } , //トランセル
  { 125 , 12 } , //バタフリー
  { 126 , 68 } , //カイリキー
  { 128 , 55 } , //ゴルダック
  { 129 , 97 } , //スリーパー
  { 130 , 42 } , //ゴルバット
  { 131 , 150 } , //ミュウツー
  { 132 , 143 } , //カビゴン
  { 133 , 129 } , //コイキング
  { 136 , 89 } , //ベトベトン
  { 138 , 99 } , //キングラー
  { 139 , 91 } , //パルシェン
  { 141 , 101 } , //マルマイン
  { 142 , 36 } , //ピクシー
  { 143 , 110 } , //マタドガス
  { 144 , 53 } , //ペルシアン
  { 145 , 105 } , //ガラガラ
  { 147 , 93 } , //ゴースト
  { 148 , 63 } , //ケーシィ
  { 149 , 65 } , //フーディン
  { 150 , 17 } , //ピジョン
  { 151 , 18 } , //ピジョット
  { 152 , 121 } , //スターミー
  { 153 , 1 } , //フシギダネ
  { 154 , 3 } , //フシギバナ
  { 155 , 73 } , //ドククラゲ
  { 157 , 118 } , //トサキント
  { 158 , 119 } , //アズマオウ
  { 163 , 77 } , //ポニータ
  { 164 , 78 } , //ギャロップ
  { 165 , 19 } , //コラッタ
  { 166 , 20 } , //ラッタ
  { 167 , 33 } , //ニドリーノ
  { 168 , 30 } , //ニドリーナ
  { 169 , 74 } , //イシツブテ
  { 170 , 137 } , //ポリゴン
  { 171 , 142 } , //プテラ
  { 173 , 81 } , //コイル
  { 176 , 4 } , //ヒトカゲ
  { 177 , 7 } , //ゼニガメ
  { 178 , 5 } , //リザード
  { 179 , 8 } , //カメール
  { 180 , 6 } , //リザードン
  { 185 , 43 } , //ナゾノクサ
  { 186 , 44 } , //クサイハナ
  { 187 , 45 } , //ラフレシア
  { 188 , 69 } , //マダツボミ
  { 189 , 70 } , //ウツドン
  { 190 , 71 } , //ウツボット
};
  
static const u32 PLAYER_NAME_ADDRESS[] =
{
  0x2598,
  0x2598
};

static const u32 BOX1_ADDRESS[] =
{
  0x4000,
  0x4000
};

static const u32 POKE_DATA_OFFSET[] =
{
  0x20,
  0x16
};

static const u32 OTNAME_OFFSET[] =
{
  0x3FE,
  0x2AA
};

static const u32 NICKNAME_OFFSET[] =
{
  0x4B2,
  0x386
};

static const u32 CONVERT_POKE_LANG[] =
{
  POKEMON_LANG_JAPAN,
  POKEMON_LANG_ENGLISH
};

static const u32 BOX_POKE_NUM[]=
{
  30,
  20
};



static const u32 TEXT_CODE_VALUE = 16;
static const u32 TEXT_SIZE_JP = 7;
static const u32 TEXT_SIZE_EN = 12;
static const gfl2::str::STRCODE s_vcTextCodeJP[TEXT_CODE_VALUE][TEXT_CODE_VALUE] =
{
  {L'　' , L'　' , L'　' , L'　' , L'　' , L'ガ' , L'ギ' , L'グ' , L'ゲ' , L'ゴ' , L'ザ' , L'ジ' , L'ズ' , L'ゼ' , L'ゾ' , L'ダ'},
  {L'ヂ' , L'ヅ' , L'デ' , L'ド' , L'　' , L'　' , L'　' , L'　' , L'　' , L'バ' , L'ビ' , L'ブ' , L'ボ' , L'　' , L'　' , L'　'},
  {L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'が' , L'ぎ' , L'ぐ' , L'げ' , L'ご' , L'ざ' , L'じ' , L'ず' , L'ぜ' , L'ぞ'},
  {L'だ' , L'ぢ' , L'づ' , L'で' , L'ど' , L'　' , L'　' , L'　' , L'　' , L'　' , L'ば' , L'び' , L'ぶ' , L'べ' , L'ぼ' , L'　'},
  {L'パ' , L'ピ' , L'プ' , L'ポ' , L'ぱ' , L'ぴ' , L'ぷ' , L'ぺ' , L'ぽ' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　'},
  {   0  , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　'},
  {L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　'},
  {L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　'},
  {L'ア' , L'イ' , L'ウ' , L'エ' , L'オ' , L'カ' , L'キ' , L'ク' , L'ケ' , L'コ' , L'サ' , L'シ' , L'ス' , L'セ' , L'ソ' , L'タ'},
  {L'チ' , L'ツ' , L'テ' , L'ト' , L'ナ' , L'ニ' , L'ヌ' , L'ネ' , L'ノ' , L'ハ' , L'ヒ' , L'フ' , L'ホ' , L'マ' , L'ミ' , L'ム'},
  {L'メ' , L'モ' , L'ヤ' , L'ユ' , L'ヨ' , L'ラ' , L'ル' , L'レ' , L'ロ' , L'ワ' , L'ヲ' , L'ン' , L'ッ' , L'ャ' , L'ュ' , L'ョ'},
  {L'ィ' , L'あ' , L'い' , L'う' , L'え' , L'お' , L'か' , L'き' , L'く' , L'け' , L'こ' , L'さ' , L'し' , L'す' , L'せ' , L'そ'},
  {L'た' , L'ち' , L'つ' , L'て' , L'と' , L'な' , L'に' , L'ぬ' , L'ね' , L'の' , L'は' , L'ひ' , L'ふ' , L'へ' , L'ほ' , L'ま'},
  {L'み' , L'む' , L'め' , L'も' , L'や' , L'ゆ' , L'よ' , L'ら' , L'り' , L'る' , L'れ' , L'ろ' , L'わ' , L'を' , L'ん' , L'っ'},
  {L'ゃ' , L'ゅ' , L'ょ' , L'ー' , L'　' , L'　' , L'　' , L'　' , L'　' , L'ァ' , L'ゥ' , L'ェ' , L'　' , L'　' , L'　' , L'　'},
  {L'　' , L'　' , L'　' , L'　' , L'ォ' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　' , L'　'}
};

static const gfl2::str::STRCODE s_vcTextCodeEN[TEXT_CODE_VALUE][TEXT_CODE_VALUE] =
{
  {L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' '},
  {L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' '},
  {L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' '},
  {L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' '},
  {L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' '},
  {   0 , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' '},
  {L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' '},
  {L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' '},
  {L'A' , L'B' , L'C' , L'D' , L'E' , L'F' , L'G' , L'H' , L'I' , L'J' , L'K' , L'L' , L'M' , L'N' , L'O' , L'P'},
  {L'Q' , L'R' , L'S' , L'T' , L'U' , L'V' , L'W' , L'X' , L'Y' , L'Z' , L'(' , L')' , L':' , L';' , L'[' , L']'},
  {L'a' , L'b' , L'c' , L'd' , L'e' , L'f' , L'g' , L'h' , L'i' , L'j' , L'k' , L'l' , L'm' , L'n' , L'o' , L'p'},
  {L'q' , L'r' , L's' , L't' , L'u' , L'v' , L'w' , L'x' , L'y' , L'z' , L' ' , L' ' , L' ' , L' ' , L' ' , L' '},
  {L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' '},
  {L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' '},
  {L' ' ,0xE0A7,0xE0A8, L' ' , L' ' , L' ' , L'?' , L'!' , L'-' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' ,0xE08E},
  {L' ' ,0xE088, L'.' , L'/' , L',' ,0xE08F, L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' ' , L' '}
};

static const u32 TEXT_SIZE_TABLE[] =
{
  TEXT_SIZE_JP,
  TEXT_SIZE_EN
};



bool LauncherKousaka::_ConvertVCSavedata( u8* pData , u32 type )
{
  gfl2::heap::HeapBase*  heap  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEVICE );

  void* pPlayerName = &pData[PLAYER_NAME_ADDRESS[type]];
  u8* pBox1 = &pData[BOX1_ADDRESS[type]];

  u8 pokeNum = pBox1[0];
  if( pokeNum > BOX_POKE_NUM[type] )
  {
    GFL_ASSERT(0);//@fix
    return false;
  }

    
  CONVERT_POKE* pPokeArray = reinterpret_cast<CONVERT_POKE*>( &pBox1[POKE_DATA_OFFSET[type]] );//ポケモンデータ配列
  u8* pOTName = reinterpret_cast<u8*>( &pBox1[OTNAME_OFFSET[type]] );//親名配列
  u8* pName = reinterpret_cast<u8*>( &pBox1[NICKNAME_OFFSET[type]] );//ニックネーム配列

  gfl2::str::StrBuf tmpBuf( 32, heap );
  _ConvertString( &tmpBuf , pPlayerName , type );

  for( u32 index = 0 ; index < BOX_POKE_NUM[type] ; index++ )
  {
    CONVERT_POKE* pPoke = &pPokeArray[index];

    //monsNo変換
    u16 monsNo = 0;
    GFL_PRINT("monsNo = [%d]\n",pPoke->monsNo);
    for( u32 monsNoIndex = 0 ; monsNoIndex < VC_MONSNO_MAX ; ++monsNoIndex )
    {
      if( pPoke->monsNo == MONSNO_CONVERT_TABLE[monsNoIndex][0] )
      {
        monsNo = MONSNO_CONVERT_TABLE[monsNoIndex][1];
        GFL_PRINT("conv monsNo = [%d]\n",monsNo);
        break;
      }
    }

    if( monsNo == 0 )
    {
      GFL_PRINT("monsNo NULL[%d]\n",index);
      continue;
    }

    pml::pokepara::InitialSpec spec;

    //レア乱数(色違い乱数)
    bool isRare = false;
    if( pPoke->powerRand2 == 10 && pPoke->powerRand3 == 10 && pPoke->powerRand4 == 10 )
    {
      if( ( pPoke->powerRand1 & 2 ) == 2 )
      {
        isRare = true;
      }
    }

    if( isRare )
    {
      spec.rare_rnd = pml::pokepara::INIT_SPEC_RARE_TRUE;
    }
    else
    {
      spec.rare_rnd = pml::pokepara::INIT_SPEC_RARE_FALSE;
    }

    //IDNo
    {//そのまま設定
      u16 oyaID = 0;
      std::memcpy( &oyaID , &pPoke->idNumber , sizeof(pPoke->idNumber) );
      oyaID = SwapU16( oyaID );

      spec.id = oyaID;
    }

    //モンスターNo
    spec.monsno =       static_cast<MonsNo>(monsNo);



    //現在のレベル
    {
      spec.level  =       pPoke->level;//レベル
    }

    //性格
    {
      u32 exp = 0;
      u8* pTempU8 = (u8*)&exp;
      std::memcpy( &pTempU8[1] , pPoke->exp , 3 );
      exp = SwapU32( exp );

      spec.seikaku = exp % 25;
    }

    //個体値
    spec.talentVNum = 3;//3V
    if( spec.monsno ==  MONSNO_MYUU )
    {
      spec.talentVNum = 5;//5V
    }

    //PokemonParamで設定
    pml::pokepara::PokemonParam * pp;
    pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap , spec );

    pp->SetTokuseiIndex( 2 );//特性３固定

    //捕獲or孵化の思い出
    nn::fnd::DateTime now;
    pp->SetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_YEAR , now.GetYear() );  // タマゴが孵化した or 捕獲した年
    pp->SetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_MONTH , now.GetMonth() );   // タマゴが孵化した or 捕獲した月
    pp->SetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_DAY , now.GetDay() );  // タマゴが孵化した or 捕獲した日
    pp->SetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE , 30013 );  // タマゴが孵化した or 捕獲した場所

    //捕まえたレベル
    pp->SetMemories( pml::pokepara::MEMORIES_CAPTURED_LEVEL , pPoke->level );  ///<捕まえたレベル

    //国コード、言語コード
    {
      nn::cfg::CTR::SimpleAddressId sId;
      nn::cfg::CTR::GetSimpleAddressId( &sId );

      pp->SetCountryCode( sId.GetCountryCode() );
      pp->SetCountryRegionCode( sId.GetRegionCode() );
      pp->SetHWRegionCode( nn::cfg::CTR::GetRegion() );
      pp->SetLangId( CONVERT_POKE_LANG[type] );
    }

    //技
    pp->SetWaza( 0 , (WazaNo)pPoke->waza1 );
    pp->SetWaza( 1 , (WazaNo)pPoke->waza2 );
    pp->SetWaza( 2 , (WazaNo)pPoke->waza3 );
    pp->SetWaza( 3 , (WazaNo)pPoke->waza4 );

    //ゼロにする
    //pp->SetWazaPPUpCount( 0 , pPoke->waza );
    //pp->SetWazaPPUpCount( 1 , pp2->pp_count[1] );
    //pp->SetWazaPPUpCount( 2 , pp2->pp_count[2] );
    //pp->SetWazaPPUpCount( 3 , pp2->pp_count[3] );

    //ROMバージョン
    {
      pp->SetCassetteVersion( VERSION_VC_RED );//ROMから入れる
    }

    //なつき度(パーソナルデータの初期値を入れる)
    {
      pml::personal::PersonalData *personal_data  = GFL_NEW( heap ) pml::personal::PersonalData( heap );
      personal_data->LoadData( pp->GetMonsNo() , pp->GetFormNo() );
      pp->SetFamiliarity( personal_data->GetParam( pml::personal::PARAM_ID_initial_familiarity ) );
      GFL_SAFE_DELETE( personal_data );
    }

    //ニックネーム
    {
      gfl2::str::StrBuf nickNameBuf( 32, heap );
      _ConvertString( &nickNameBuf , pName , type );
        
      gfl2::str::StrBuf monsNameBuf( 32, heap );
      pml::personal::GetMonsName( &monsNameBuf, pp->GetMonsNo() );

      if( nickNameBuf.Compare( monsNameBuf ) == false )
      {//つけてる
        GFL_PRINT("have nick name\n");
        pp->SetNickName( &nickNameBuf );
      }
      else
      {//つけてない
        GFL_PRINT("default name\n");
        pp->SetDefaultNickName();
      }
    }

    //親の名前
    {
      gfl2::str::StrBuf oyanameBuf( 32, heap );
      _ConvertString( &oyanameBuf , pOTName , type );
      pp->SetParentName( &oyanameBuf );
    }

    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon()->PutPokemonEmptyTrayAll( pp , type , heap );

    pName += (TEXT_SIZE_TABLE[type] - 1);
    pOTName += (TEXT_SIZE_TABLE[type] - 1);
  }

  return true;
}


void LauncherKousaka::_ConvertString( gfl2::str::StrBuf* pOutput , void* pAddr , u32 type )
{
  gfl2::str::STRCODE text[TEXT_SIZE_EN];
  std::memset( text , 0 , sizeof(text) );

  for( u32 index = 0 ; index < TEXT_SIZE_TABLE[type] - 1 ; ++index )
  {
    //GFL_PRINT("_ConvertString : %d [%d]\n" , index , pName->name[index] );

    switch( type )
    {
    case SAVE_TYPE_JP:
      {
        CONVERT_NAME_JP* pName = reinterpret_cast<CONVERT_NAME_JP*>( pAddr );
        u8 index1 = pName->name[index] / TEXT_CODE_VALUE;
        u8 index2 = pName->name[index] % TEXT_CODE_VALUE;
        text[index] = s_vcTextCodeJP[index1][index2];
      }
    break;
    case SAVE_TYPE_EN:
      {
        CONVERT_NAME_EN* pName = reinterpret_cast<CONVERT_NAME_EN*>( pAddr );
        u8 index1 = pName->name[index] / TEXT_CODE_VALUE;
        u8 index2 = pName->name[index] % TEXT_CODE_VALUE;
        text[index] = s_vcTextCodeEN[index1][index2];
      }
    default:
    break;
    }
    
  }

  pOutput->SetStr( text );
}

u16 LauncherKousaka::SwapU16(u16 val)
{
  return ((val&0xFF00)>>8) | ((val&0x00FF)<<8);
}

u32 LauncherKousaka::SwapU32(u32 val)
{
  return ((val&0xFF000000)>>24) | ((val&0x00FF0000)>>8) | ((val&0x0000FF00)<<8) | ((val&0x000000FF)<<24);
}


#endif

GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)




#endif  // PM_DEBUG
