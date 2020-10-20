// ============================================================================
/*
 * @file JoinFestaDummySendEvent.cpp
 * @brief アトラクションでデバッグ用に他人になりすましてビーコン送信するイベント
 * @date 2016.05.09
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetStatic/NetEvent/include/Debug/JoinFestaDummySendEvent.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"
// ジョインフェスタパーソナルデータ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"

#if PM_DEBUG

GFL_NAMESPACE_BEGIN(NetEvent)

u32 JoinFestaDummySendEvent::m_patternMax = 0;  // なりすましパターン

//--------------------------------------------------------------
/**
 * @brief アトラクションでデバッグ用に他人になりすましてビーコン送信するイベント呼び出し
 * 
 * @param   pGameManager  ゲームマネージャ
 */
//--------------------------------------------------------------
void JoinFestaDummySendEvent::StartEvent( GameSys::GameManager* pGameManager )
{
  JoinFestaDummySendEvent* p_event;
  p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, JoinFestaDummySendEvent>( pGameManager->GetGameEventManager() );
}

JoinFestaDummySendEvent::JoinFestaDummySendEvent( gfl2::heap::HeapBase* pHeap ) :
 GameSys::GameEvent( pHeap )
 ,m_pattern(0)
 ,m_rotateTime(0)
 ,m_host(false)
{
#if defined(GF_PLATFORM_CTR)
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
  NetAppLib::JoinFesta::JoinFestaPersonalData* pMyData = manager->GetMyData();
  // 主催者チェック
  if( pMyData->IsOpenAttraction() )
  {
    NetAppLib::JoinFesta::PacketAttraction* attractionPacket = pMyData->GetPacketAttractionPtr();
    if( attractionPacket->ownerTransferdID == nn::cfg::CTR::GetTransferableId( GameSys::GameManager::APLICATION_UNIQUE_ID ) )
    {
      m_host = true;
    }
  }
#endif // defined(GF_PLATFORM_CTR)
}


JoinFestaDummySendEvent::~JoinFestaDummySendEvent()
{
}


bool JoinFestaDummySendEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void JoinFestaDummySendEvent::InitFunc( GameSys::GameManager* pGameManager )
{
}


void JoinFestaDummySendEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT JoinFestaDummySendEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  bool isEnd = false;


  // Bボタンでいつでもキャンセル
  {
    const gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
    if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
    {
      isEnd = true;
    }
  }

#if defined(GF_PLATFORM_CTR)
  // 60フレームに１回ビーコン内容更新
  {
    // 一定時間で拡張情報変更
    u64 currentTime = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();
    if( currentTime - m_rotateTime > BEACON_ROTATE_TIME )
    {
      m_rotateTime = currentTime;

      NetAppLib::JoinFesta::JoinFestaPersonalDataManager* manager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
      NetAppLib::JoinFesta::JoinFestaPersonalData* pMyData = manager->GetMyData();

      NetAppLib::JoinFesta::JoinFestaPacketData packet;
      pMyData->OutputPacketData( &packet );

      // 別人になる
      packet.personalInfo.myPrincipalId++;
      packet.personalInfo.transferdId++;


      // 主催者の場合は別アトラクションにする
      if( m_host == true )
      {
        packet.unionInfo.attraction.ticketID++;
        packet.unionInfo.attraction.ownerTransferdID = packet.personalInfo.transferdId; 

        GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->DEBUG_SetPacketAttraction( packet.unionInfo.attraction );
      }

      // 名前も変える
      swprintf( packet.personalInfo.name, System::STRLEN_PLAYER_NAME+System::EOM_LEN, L"LA-%d", m_pattern );

      // 日本言語は全角にする
      if( pMyData->GetLanguageId() == POKEMON_LANG_JAPAN )
      {
        for( u32 index = 0 ; index < System::STRLEN_PLAYER_NAME+System::EOM_LEN ; ++index )
        {
          if( packet.personalInfo.name[index] >= L'0' && packet.personalInfo.name[index] <= L'9' )
          {
            packet.personalInfo.name[index] = packet.personalInfo.name[index] - L'0' + L'０';
          }
          else if( packet.personalInfo.name[index] >= L'A' && packet.personalInfo.name[index] <= L'Z' )
          {
            packet.personalInfo.name[index] = packet.personalInfo.name[index] - L'A' + L'Ａ';
          }
          else if( packet.personalInfo.name[index] == L'-' )
          {
            packet.personalInfo.name[index] = L'－';
          }
        }
      }

      // パケット更新して送信
      pMyData->SetPacketData( packet );
      GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData();


      // 指定された回数なりすましたら終了
      if( ++m_pattern == m_patternMax )
      {
        isEnd = true;
      }
    }
  }
#endif // defined(GF_PLATFORM_CTR)


  if( isEnd )
  {
    return GameSys::GMEVENT_RES_FINISH;
  }
  return GameSys::GMEVENT_RES_CONTINUE;
}

GFL_NAMESPACE_END(NetEvent)

#endif  /*  PM_DEBUG  */
