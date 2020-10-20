//==============================================================================
/**
 * @file   JoinFestaEffectSystem.cpp
 * @date   2015/10/22 Thu. 15:11:29
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <System/include/gflUse.h>

/*  fieldmap  */
#include  "Field/FieldRo/include/Fieldmap.h"
#include  "Field/FieldRo/include/Effect/FieldEffectManager.h"
#include  "Field/FieldRo/include/Effect/content/FieldEffectFesMessage.h"
#include  "Field/FieldRo/include/Effect/content/FieldEffectFesLevelUp.h"
#include  "Field/FieldRo/include/Effect/content/FieldEffectFesShopOpen.h"
#include  "Field/FieldRo/include/Effect/content/FieldEffectFesStartSplash.h"
#include  "Field/FieldRo/include/Effect/content/FieldEffectFesWarp.h"


#include  "NetApp/JoinFesta/include/JoinFestaResourceManager/JoinFestaResourceManager.h"
#include  "NetApp/JoinFesta/include/JoinFestaEffectSystem/JoinFestaEffectSystem.h"

// パーソナルイベントデータクラス
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalEvent/JoinFestaPersonalEvent.h"
// アトラクションマネージャ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"

namespace NetApp    {
namespace JoinFesta {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
JoinFestaEffectSystem::JoinFestaEffectSystem(
  gfl2::heap::HeapBase*         pHeapBase
  , JoinFestaResourceManager*   pResourceManager
#ifdef JF_DEBUG_MENU
  , gfl2::debug::DebugWinGroup* pParentDebugWindowGroup
#endif
)
  : m_pHeapBase(pHeapBase)
  , m_pResourceManager(pResourceManager)
  , m_pHandler(NULL)
  , m_pFesMessage(NULL)
  , m_pFesLevelUp(NULL)
  , m_pFesShopOpen(NULL)
  , m_pFesStartSplash(NULL)
  , m_pFesWarp(NULL)
  , m_LastTalkerFriendKey()
  , m_IsShowingMessage(false)
#ifdef JF_DEBUG_MENU
  , m_Debug(pHeapBase, this, pParentDebugWindowGroup)
#endif
{
  m_LastTalkerFriendKey.initialize();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
JoinFestaEffectSystem::~JoinFestaEffectSystem()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void    JoinFestaEffectSystem::Initialize(void)
{
  Field::Fieldmap*                pFieldMap       = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap();
  Field::Effect::EffectManager*   pEffectManager  = pFieldMap->GetEffectManager();
  
  if( !pEffectManager->IsLoadedEffectResource(Field::Effect::EFFECT_TYPE_FES_MESSAGE) )
  {
    pEffectManager->LoadDynamicEffectResource( Field::Effect::EFFECT_TYPE_FES_MESSAGE, m_pHeapBase, true );
    pEffectManager->SetupDynamicEffectResource( Field::Effect::EFFECT_TYPE_FES_MESSAGE, m_pHeapBase );

    m_pFesMessage = static_cast<Field::Effect::EffectFesMessage*>(pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_FES_MESSAGE, NULL));
  }

  {
    const Field::Effect::Type typeTable[] =
    {
      Field::Effect::EFFECT_TYPE_FES_LEVELUP,
      Field::Effect::EFFECT_TYPE_FES_SHOP_OPEN,
      Field::Effect::EFFECT_TYPE_FES_START_SPLASH,
      Field::Effect::EFFECT_TYPE_FES_WARP,
    };

    for(u32 index = 0; index < GFL_NELEMS(typeTable); ++index)
    {
      const Field::Effect::Type type  = typeTable[index];
      
      if( !pEffectManager->IsLoadedEffectResource(type) )
      {
        pEffectManager->LoadDynamicEffectResource(type, m_pHeapBase, true);
        pEffectManager->SetupDynamicEffectResource(type, m_pHeapBase );
      }
    }
    m_pFesLevelUp       = static_cast<Field::Effect::EffectFesLevelUp*    >(pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_FES_LEVELUP,       gfl2::math::Vector3(), false /* bool in_bPlaySe */ ));
    m_pFesShopOpen      = static_cast<Field::Effect::EffectFesShopOpen*   >(pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_FES_SHOP_OPEN,     gfl2::math::Vector3(), false /* bool in_bPlaySe */ ));
    m_pFesStartSplash   = static_cast<Field::Effect::EffectFesStartSplash*>(pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_FES_START_SPLASH,  gfl2::math::Vector3(), false /* bool in_bPlaySe */ ));
    m_pFesWarp          = static_cast<Field::Effect::EffectFesWarp*       >(pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_FES_WARP,          gfl2::math::Vector3(), false /* bool in_bPlaySe */ ));

    if( m_pFesWarp )
    {
      m_pFesWarp->SetEventHandler(this);
    }
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void    JoinFestaEffectSystem::Update(void)
{
  
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
JoinFestaEffectSystem::RESULT  JoinFestaEffectSystem::Terminate(void)
{
  /*
    ここでリソースだけを解放してしまうと、EffectManagerと共にエフェクトが解体される際に
    解放済みのリソース情報を参照して解体処理が走ってしまい、クラッシュする
    そのためリソースの解放もシステムに任せるよう修正
  */
//  Field::Fieldmap*                pFieldMap       = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap();
//  Field::Effect::EffectManager*   pEffectManager  = pFieldMap->GetEffectManager();
//
//  if( pEffectManager->IsLoadedEffectResource(Field::Effect::EFFECT_TYPE_FES_MESSAGE) )
//  {
//    pEffectManager->UnLoadDynamicEffectResource( Field::Effect::EFFECT_TYPE_FES_MESSAGE, m_pHeapBase );
//  }

  return  RESULT_FINISH;
}




//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  JoinFestaEffectSystem::ShowMessageEffect(const NetAppLib::JoinFesta::JoinFestaPersonalData* pTargetPersonalData, const bool isImmediate)
{
  if(!pTargetPersonalData)  return;
  if(!m_pFesMessage)      return;


  Field::Fieldmap*                pFieldMap       = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap();
  Field::Effect::EffectManager*   pEffectManager  = pFieldMap->GetEffectManager();


  /*
    Keyが異なる場合のみ新規表示
    ※テストのため!pTargetPersonalDataで動作するようにする
  */
  if(pTargetPersonalData)
  {
    if(pTargetPersonalData->IsSameFriendKey(m_LastTalkerFriendKey))
    {
      return;
    }
    m_LastTalkerFriendKey = pTargetPersonalData->GetJoinFestaFriendKey();
  }
  m_IsShowingMessage  = true;


  {
    // 一言ウィンドウの優先度
    // １．アトラクション中の場合は、アトラクション専用メッセージ
    // ２．パーソナルイベント発動中は、イベント専用メッセージ
    // ３．上記以外は、パーソナルの挨拶メッセージ
    u32 msgId;
    bool bEvent;
    bool isExpand = false;
    if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->IsPlaying() )
    {
      msgId = pTargetPersonalData->GetAttractionSingleWordMessage();
      bEvent = pTargetPersonalData->IsAttractionPersonalEventEnable();
    }
    else if( pTargetPersonalData->IsPersonalEventEnable() )
    {
      msgId = NetAppLib::JoinFesta::JoinFestaPersonalEvent::GetWordWindowMessageIdFromPersonalEvent( pTargetPersonalData->GetPersonalEventId(), pTargetPersonalData->GetSex() );
      bEvent = true;
    }
    else
    {
      msgId = pTargetPersonalData->GetPattern( JoinFestaScript::PATTERN_HELLO );
      bEvent = false;
    }

    const Field::Effect::EffectFesMessage::ColorMode  colMode = (bEvent ? Field::Effect::EffectFesMessage::COLMODE_Event : Field::Effect::EffectFesMessage::COLMODE_Normal );

    // メッセージにタグあるやつチェック
    if( msgId == msg_jf_phrase_m_008 || msgId == msg_jf_phrase_f_008 )
    {
      m_pFesMessage->SetRegisterMonsNameNo( 0, static_cast<MonsNo>(pTargetPersonalData->GetAttractionMonsNo()) );
      isExpand = true;
    }

    m_pFesMessage->Show(
      pTargetPersonalData->GetName(),
      m_pResourceManager->GetString(JoinFestaResourceManager::RESOURCE_Message_JFPhrase, msgId).GetPtr(),
      isExpand,
      isImmediate,
      colMode
    );
  }



}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  JoinFestaEffectSystem::HideMessageEffect(const bool isImmediate)
{
  if(!m_pFesMessage)      return;
  if(!m_IsShowingMessage) return;


  Field::Fieldmap*                pFieldMap       = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap();
  Field::Effect::EffectManager*   pEffectManager  = pFieldMap->GetEffectManager();

  m_pFesMessage->Hide(isImmediate);
  m_LastTalkerFriendKey.initialize();
  m_IsShowingMessage  = false;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  JoinFestaEffectSystem::Start_Rankup(void)
{
  if(m_pFesLevelUp)   m_pFesLevelUp->Start();
  if(m_pFesShopOpen)  m_pFesShopOpen->Start();
}


void  JoinFestaEffectSystem::Start_NewShop(void)
{
  if(m_pFesShopOpen)  m_pFesShopOpen->Start();
}


void  JoinFestaEffectSystem::Start_Warp(const gfl2::math::Vector3& rPos)
{
  if(m_pFesWarp)  m_pFesWarp->Start(rPos);
}

void  JoinFestaEffectSystem::Start_Attraction(void)
{
  if(m_pFesStartSplash)  m_pFesStartSplash->Start();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  JoinFestaEffectSystem::Stop_Rankup(void)
{
  if(m_pFesLevelUp)   m_pFesLevelUp->Stop();
  if(m_pFesShopOpen)  m_pFesShopOpen->Stop();
}


void  JoinFestaEffectSystem::Stop_NewShop(void)
{
  if(m_pFesShopOpen)  m_pFesShopOpen->Stop();
}


//void  JoinFestaEffectSystem::Stop_Warp(void)
//{
//  if(m_pFesWarp)  m_pFesWarp->Stop();
//}

void  JoinFestaEffectSystem::Stop_Attraction(void)
{
  if(m_pFesStartSplash) m_pFesStartSplash->Start();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  JoinFestaEffectSystem::EffectFesWarp_OnEvent(Field::Effect::EffectFesWarp* pCaller, const Field::Effect::EffectFesWarp::IEventHandler::EventID eventID)
{
  GFL_PRINT("%s: onEvent(%d)\n", __FUNCTION__, eventID);

  switch (eventID)
  {
    case IEventHandler::EVENT_Warp_AppearCharacter:
      Event(IEventHandler::EVENT_Warp_AppearCharacter);
      break;

    case  IEventHandler::EVENT_Warp_End:
      Event(IEventHandler::EVENT_Warp_End);
      break;
  }
}



} /*  namespace JoinFesta */
} /*  namespace NetApp    */
