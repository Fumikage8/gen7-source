
//==============================================================================
/**
 * @file   GiftDeliveryUtil.cpp
 * @date   2015.06.16 Tue.
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <heap/include/gfl2_Heap.h>
#include  <ui/include/gfl2_UI.h>
#include  <System/include/HeapDefine.h>
#include  <System/include/ThreadPriority.h>

#include  <GameSys/include/GameProcManager.h>
#include  <GameSys/include/GameManager.h>
#include  <GameSys/include/GameData.h>
#include  <Prog/System/include/PokemonVersion.h>

#include  <NetStatic/NetLib/include/NetDefine.h>
#include  <NetStatic/NetLib/include/NijiNetworkSystem.h>
#include  <NetStatic/NetLib/include/P2P/P2pConnectionManager.h>
#include  <NetStatic/NetLib/include/Delivery/DeliveryManager.h>
#include  <NetStatic/NetLib/include/Delivery/DeliverySendListener.h>
#include  <NetStatic/NetLib/include/Delivery/DeliveryRecvListener.h>
#include  <NetStatic/NetAppLib/include/System/ApplicationWorkBase.h>
#include  <NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h>
#include  "./GiftDeliveryUtil.h"

#include "Savedata/include/MysteryGiftSave.h"



namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================
DeliveryUtil*  DeliveryUtil::m_pInstance  = NULL;


//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015.06.17 Wed.
  */
//==============================================================================
class DeliveryUtilImplBase
{
  GFL_FORBID_COPY_AND_ASSIGN(DeliveryUtilImplBase);



public:
  DeliveryUtilImplBase(NetAppLib::System::ApplicationWorkBase* pWorkBase) {}
  virtual ~DeliveryUtilImplBase() {}

  virtual bool  Initialize(const DeliveryUtil::Mode mode)   {return false;}
  virtual void  Finalize(void)                              {}
  virtual void  Update(void)                                {}
  virtual void  SetAttribute( const DeliveryUtil::EAttributePresetID attrID ) {}
  virtual void  ClearAttribute( s32 attributeIndex )            {}
  virtual void  SetFileListFilter( DeliveryUtil::IFileListFilter* pFilter )   {}
  virtual bool  StartSend( const u16 deliveryID , const void* pSendData , const size_t dataSize )  {return false;}
  virtual bool  StartRecv(void)                             {return false;}
  virtual void  PrepareFinalize( void ) {}
  virtual bool  IsCompretePrepareFinalize( void )               {return true;}
  virtual void  SetSendListener( DeliveryUtil::ISendListener* pSendListener ) {}
  virtual void  RemoveSendListener()    {}
  virtual void  SetRecvListener( DeliveryUtil::IRecvListener* pRecvListener ) {}
  virtual void  RemoveRecvListener()    {}
  virtual bool  IsInitialized( void ) const {return false;}
  virtual bool  GetWirelessSwitchState( void ) const {return false;}
  virtual void  SetSerialCode(const u32 serialCode)    {}
};


#if defined(GF_PLATFORM_CTR)
// =============================================================================
/*
    CTR
    ※このときだけ内部処理が有効
*/
//==============================================================================
/*    */
inline  gflnet2::EN_NET_TYPE  UtilModeToNetType(const DeliveryUtil::Mode utilMode)
{
  switch(utilMode)
  {
    case  DeliveryUtil::MODE_UDS:             return gflnet2::TYPE_P2P_WIRELESS;
    case  DeliveryUtil::MODE_Ir:              return gflnet2::TYPE_P2P_IR;
    case  DeliveryUtil::MODE_Boss:            return gflnet2::TYPE_P2P_WIFI;
    case  DeliveryUtil::MODE_BossViaPGLAuth:  return gflnet2::TYPE_P2P_WIFI;
  }

  GFL_ASSERT(false);
  return gflnet2::TYPE_P2P_IR;
}

/*    */
inline NetLib::Delivery::DeliveryManager::DELIVERY_TYPE UtilModeToDelivType(const DeliveryUtil::Mode utilMode)
{
  switch(utilMode)
  {
    case  DeliveryUtil::MODE_UDS:             return NetLib::Delivery::DeliveryManager::DELIVERY_TYPE_wireless;
    case  DeliveryUtil::MODE_Ir:              return NetLib::Delivery::DeliveryManager::DELIVERY_TYPE_ir;
    case  DeliveryUtil::MODE_Boss:            return NetLib::Delivery::DeliveryManager::DELIVERY_TYPE_wifi;
    case  DeliveryUtil::MODE_BossViaPGLAuth:  return NetLib::Delivery::DeliveryManager::DELIVERY_TYPE_wifi;
  }

  GFL_ASSERT(false);
  return NetLib::Delivery::DeliveryManager::DELIVERY_TYPE_ir;
}


//==============================================================================
/**
  *  @class  DeliveryUtilImpl
  *  @brief  DeliveryUtil処理実装部
  *  @date   2015.06.17 Wed.
  */
//==============================================================================
class DeliveryUtilImpl
  : public  DeliveryUtilImplBase
  , public  NetLib::Delivery::DeliverySendListener
  , public  NetLib::Delivery::DeliveryRecvListener
  , public  NetLib::Delivery::DeliveryFileListFilter
{
  GFL_FORBID_COPY_AND_ASSIGN(DeliveryUtilImpl);
public:

  enum
  {
    MAX_GIFT_NUM = 20
  };

  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  DeliveryUtilImpl(NetAppLib::System::ApplicationWorkBase* pWorkBase)
    : DeliveryUtilImplBase(pWorkBase)
    , m_pWorkBase(pWorkBase)
    , m_pDeliv(NULL)
    , m_pSendListener(NULL)
    , m_pRecvListener(NULL)
    , m_pFileListFilter(NULL)
    , m_Mode(DeliveryUtil::MODE_NONE)
    , m_IsFinalizing(false)
  {
    m_pDeliv  = GFL_NEW(pWorkBase->GetAppHeap()->GetDeviceHeap())  NetLib::Delivery::DeliveryManager();

    /*  おくりものは受信時署名確認  */
    m_pDeliv->SetUDSSignatureMode(false, true);
  }

  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual ~DeliveryUtilImpl()
  {
    /*  singletonInstanceの解体  */
    GFL_SAFE_DELETE(m_pDeliv);
  }

  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual bool  Initialize(const DeliveryUtil::Mode mode)
  {
    if(GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->IsError())
    {
      return false;
    }


    m_Mode  = mode;
    switch(mode)
    {
      case  DeliveryUtil::MODE_UDS:   /*  through  */
      case  DeliveryUtil::MODE_Ir:
        NetLib::NijiNetworkSystem::InitializeLocalP2PNetwork();
        break;

      case  DeliveryUtil::MODE_Boss:
        NetLib::NijiNetworkSystem::InitializeWifiNetwork(); 
        break;

      case  DeliveryUtil::MODE_BossViaPGLAuth:
        /*  nop:PGL認証を外部依存に→InitializeWifiNetworkを呼ばない  */
        break;

      default:
        GFL_ASSERT(false);
        break;
    }

    const gflnet2::InitParameter
            initParameter = {
              nn::cfg::GetTransferableId(GameSys::GameManager::APLICATION_UNIQUE_ID),   //  自分のTransferdID 
              gfl2::heap::Manager::GetHeapByHeapId( HEAPID_NETWORK_RESIDENT ),
              gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE ),
              UtilModeToNetType(mode),    /*  gflnet2::TYPE_P2P_WIRELESS,  // ネットワーク種別  */
              NetLib::P2P::NIJI_P2P_GIFT,   /*  ENUM_NIJI_P2P_ID,    // 通信別のID  */
              2,                          /*  最大接続人数                */
              false                       /*  //マイグレーション有効無効  */
            };

    const gflnet2::InitParameterFixed
            initParameterFixed  = {
              NIJI_PRODUCT_CODE,      ///< 任天堂からソフトごとに割り当ててくれるID 
              System::GetLang(),      ///< 言語番号 
              System::GetVersion(),   ///< ＲＯＭバージョン番号 
              NIJI_NET_VERSION        ///< 通信バージョン              
            };

    m_pDeliv->Initialize(&initParameter, &initParameterFixed, UtilModeToDelivType(mode));
    m_pDeliv->SetSendListener(this);
    m_pDeliv->SetRecvListener(this);
    //m_pDeliv->SetFileListFilter(this);

    m_IsFinalizing  = false;

    return true;
  }

  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual void  Finalize(void)
  {
    m_pDeliv->Finalize();
    RemoveSendListener();
    RemoveRecvListener();
    m_pFileListFilter = NULL;

    switch(m_Mode)
    {
      case  DeliveryUtil::MODE_UDS:   /*  through  */
      case  DeliveryUtil::MODE_Ir:    /*  through  */
      case  DeliveryUtil::MODE_Boss:  /*  through  */
        NetLib::NijiNetworkSystem::TerminateNetwork();
        break;

      case  DeliveryUtil::MODE_BossViaPGLAuth:
        /*  nop:PGL認証を外部依存に→TerminateNetworkを呼ばない  */
        break;
    }
  }


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual void  Update(void)
  {
    m_pDeliv->Update();
  }


  //------------------------------------------------------------------
  /**
    *  @brief    
    *  @note    nijiでは不要となった
    *  @return   
    */
  //------------------------------------------------------------------
  virtual void  SetAttribute( const DeliveryUtil::EAttributePresetID attrID )
  {
    /*
      struct BOSS_SEARCH_ATTRIBUTES
      {
        BOSS_SEARCH_ATTRIBUTES()
        : pAttribute1(NULL)
        , pAttribute2(NULL)
        , pAttribute3(NULL)
        {};//ユーザがパラメータを設定しなかった場合エラーになるように構造体を初期化
        const char* pAttribute1;
        const char* pAttribute2;
        const char* pAttribute3;
      };

       * NSAリストのダウンロード(NsaList::Download)時、NSAリストに含ませるNSAを指定するための検索要素。

        属性１～３において、NULLでないすべての属性の文字列が、
        サーバ側のNSAの属性の文字列と完全に一致するもののみ、
        ダウンロードされるNSAリストに含まれるようになります。
        例えばattribute1が"A"、attribute2とattribute3がNULLの場合、
        このSearchAttributesを指定してダウンロードしたNSAリストには、
        サーバのNSAで属性１が"A"となっているもののみ含まれるようになります。
        (サーバのNSAの属性２、３の文字列は影響しません。) またattribute1が"A"、attribute2が"B"、attribute3が"C"の場合、
        サーバのNSAで属性１が"A"、属性２が"B"、属性３が"C"となっているNSAのみ、NSAリストに含まれるようになります。
    */
    const struct 
    {
      const char* pAttr[3];
    }bossAttrPresets[DeliveryUtil::ATTR_MAX] = {
      {{"dummy", "dummy", "dummy"}},   /*  ATTR_Dummy  */
    };

    gflnet2::boss::BossManager::BOSS_SEARCH_ATTRIBUTES  attr;

    attr.pAttribute1  = bossAttrPresets[attrID].pAttr[0];
    attr.pAttribute2  = bossAttrPresets[attrID].pAttr[1];
    attr.pAttribute3  = bossAttrPresets[attrID].pAttr[2];

    //m_pDeliv->SetAttribute(&attr);
  }


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual void  ClearAttribute( s32 attributeIndex )
  {
    m_pDeliv->ClearAttribute(attributeIndex);
  }


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual void  SetFileListFilter( DeliveryUtil::IFileListFilter* pFilter )
  {
    m_pFileListFilter = pFilter;
  }


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual bool  StartSend( const u16 deliveryID , const void* pSendData , const size_t dataSize )
  {
    return m_pDeliv->StartSend(System::ThreadPriority::DELIVERY_THREAD_PRIORITY, deliveryID, pSendData, dataSize);
  }


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual bool  StartRecv(void)
  {
    /*  シリアルコード受信時は認証のためUtilの外でWifi接続するので、その旨DeliveryManagerへ通知  */
    return m_pDeliv->StartRecv(System::ThreadPriority::DELIVERY_THREAD_PRIORITY, (m_Mode == DeliveryUtil::MODE_BossViaPGLAuth));
  }


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual void  PrepareFinalize( void )
  {
    m_IsFinalizing  = true;
    m_pDeliv->PrepareFinalize();
  }


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual bool  IsCompretePrepareFinalize( void )
  {
    return m_pDeliv->IsCompretePrepareFinalize();
  }


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    *  @param    
    *  @return   
    */
  //------------------------------------------------------------------
  virtual void  SetSendListener( DeliveryUtil::ISendListener* pSendListener )
  {
    m_pSendListener = pSendListener;
  }


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual void  RemoveSendListener()
  {
    m_pSendListener   = NULL;
  }


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual void  SetRecvListener( DeliveryUtil::IRecvListener* pRecvListener )
  {
    m_pRecvListener = pRecvListener;
  }


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual void  RemoveRecvListener()
  {
    m_pRecvListener   = NULL;
  }


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual bool  IsInitialized( void ) const
  {
    return m_pDeliv->IsInitialized();
  }


  /*
    NetLib::Delivery::DeliverySendListener
  */
  virtual void  OnNetworkError( const nn::Result &result )
  {
    GFL_PRINT("%s\n", __FUNCTION__);

    if(m_pSendListener)
    {
      DeliveryUtil::ISendListener::ResultInfo resultInfo  =
                                              {
                                                DeliveryUtil::ISendListener::ERROR_Dummy
                                              };
      /**
        DeliveryManagerから投げられるResultをここでフィルタリングしてApplicationに通知する
        ※不要となった
      */
      m_pSendListener->DeliveryUtilSendListener_OnNetworkError(resultInfo);
    }
  }



  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual bool  GetWirelessSwitchState( void ) const
  {
    return NetLib::NijiNetworkSystem::IsWLANSwitchEnable();
  }


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual void  SetSerialCode(const u32 serialCode)
  {
    m_pDeliv->SetSerialCode(serialCode);
  }



  /*
    NetLib::Delivery::DeliveryRecvListener
  */
  virtual bool  OnRecvComplete( const void* pData , size_t dataSize )
  {
    bool  isContinueReceiving = false;

    u32 giftSize = sizeof(Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA);

    GFL_PRINT("%s\n", __FUNCTION__);


    /*  notfound  */
    if((!pData) || (dataSize < giftSize))
    {
      GFL_PRINT("%s: data not found\n", __FUNCTION__);
      m_pRecvListener->DeliveryUtilRecvListener_OnRecvComplete(DeliveryUtil::IRecvListener::COMPLETE_RESULT_NotFound);
      return false;
    }


    if( dataSize % giftSize != 0 )
    {
      GFL_ASSERT(0);//@fix
      return true;
    }

    u32 dataNum = dataSize / giftSize;
    if( dataNum > MAX_GIFT_NUM )
    {
      GFL_ASSERT(0);
      return true;
    }

    Savedata::MysteryGiftSave * pSaveData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetMysteryGiftSave();

    u32 enableItemCount = 0;
    const Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA* pRecvData = reinterpret_cast<const Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA*>( pData );

    /*
      CRCcheck
      破損判定
      ※crcフィールドを0クリア→CRC計算→crcフィールド書き戻しとしないといけないとのことなので、止むを得ずconst_cast
    */
    {
      Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA*  pConstCastedData  = const_cast<Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA*>(pRecvData);
      
      for(u32 index = 0; index < dataNum; ++index)
      {
        if(!(pConstCastedData[index].CRCCheck()))
        {
          /*  不合格  */
          if(m_pRecvListener)
          {
            isContinueReceiving = m_pRecvListener->DeliveryUtilRecvListener_OnRecvComplete(DeliveryUtil::IRecvListener::COMPLETE_RESULT_ReceiveFailed);
          }
          return isContinueReceiving;
        }
      }
    }



    Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA* pTempData = GFL_NEW_ARRAY( m_pWorkBase->GetAppHeap()->GetDeviceHeap()->GetLowerHandle() ) Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA[dataNum];
    Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA* pLastData = GFL_NEW_ARRAY( m_pWorkBase->GetAppHeap()->GetDeviceHeap()->GetLowerHandle() ) Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA[dataNum];


    /*
      おくりものツールでは、
        Sun   :   0000 0000 0000 0001 (0x01)
        Moon  :   0000 0000 0000 0010 (0x02)
        Sun2  :   0000 0000 0000 0100 (0x04)
        Moon2 :   0000 0000 0000 1000 (0x08)
      となっている。
      ここでは、(0x01 << (VERSION_LISTのindex+1))
      となっているため、SUN, MOON, SUN2, MOON2 とする必要がある
    */
    //自分のロムバージョン
    const u32 VERSION_LIST[] =
    {
      VERSION_SUN,
      VERSION_MOON,
      VERSION_SUN2,
      VERSION_MOON2,
    };

    int versionIndex = 1;
    for( u32 index = 0 ; index < GFL_NELEMS(VERSION_LIST) ; ++index )
    {
      if( VERSION_LIST[index] == System::GetVersion() )
      {
        versionIndex = index + 1;
        break;
      }
    }

    u8 romVersion = versionIndex;
    u32 bit = 1 << ( romVersion - 1 );
    //言語、バージョンチェック
    for( u32 index = 0 ; index < dataNum ; ++index )
    {
      u32 targetVertion = pRecvData[index].version;
      u8 targetLang = pRecvData[index].rang;
      u32 eventID = pRecvData[index].dataBody.eventID;

      u32 flag = bit & targetVertion;
      if( flag == 0 )
      {//ダウンロード不可能
        continue;
      }

      if( System::GetLang() != targetLang && targetLang != 0 )
      {//ダウンロード不可能
        continue;
      }

      if( pRecvData[index].randomNo > 0 )
      {//ランダム配信だったら受信履歴チェック
        bool isRandomExists = false;
        for( s32 flagIndex = 0 ; flagIndex < Savedata::MysteryGiftSave::RANDOM_DELIVERY_FLAG_NUM ; ++flagIndex )
        {
          if( pSaveData->GetRandomEventID( flagIndex ) == pRecvData[index].dataBody.eventID && pSaveData->GetRandomNo( flagIndex ) == pRecvData[index].randomNo )
          {//ダウンロード不可能
            GFL_PRINT("WonderGiftRecv::OnRecvComplete : aready got gift[%d][%d]\n",pRecvData[index].dataBody.eventID,pRecvData[index].randomNo);
            isRandomExists = true;
            break;
          }
        }

        if( isRandomExists == true )
        {//ダウンロード不可能
          continue;
        }
      }

      std::memcpy( &pTempData[enableItemCount] , &pRecvData[index] , giftSize );
      enableItemCount++;
    }

    //ランダム配信チェック
    u32 lastFileNum = 0;
    u32 lastIndexList[MAX_GIFT_NUM];
    std::memset( lastIndexList , 0 , sizeof(lastIndexList) );

    {
      bool isRandomExists = false;
      u32* randomList = GFL_NEW_ARRAY( m_pWorkBase->GetAppHeap()->GetDeviceHeap()->GetLowerHandle() ) u32[20*255];

      for( u32 enableIndex = 0 ; enableIndex < enableItemCount ; ++enableIndex )
      {
        //ランダムチェック
        u32 randomCount = 0;
        std::memset( randomList , 0 , 20*255*sizeof(u32) );
        for( u32 index = 0 ; index < enableItemCount ; ++index )
        {
          if( index == enableIndex )
            continue;

          if( pTempData[enableIndex].dataBody.eventID == pTempData[index].dataBody.eventID )
          {//イベントIDが一致する（ランダム配信）
            if( randomCount == 0 )
            {
              u32 loopNum = gfl2::math::Max<u32>( pTempData[enableIndex].randomParam , 1 );
              for( u32 loopCnt = 0 ; loopCnt < loopNum ; ++loopCnt )
              {
                randomList[randomCount] = enableIndex;
                randomCount++;
              }

              loopNum = gfl2::math::Max<u32>( pTempData[index].randomParam , 1 );
              for( u32 loopCnt = 0 ; loopCnt < loopNum ; ++loopCnt )
              {
                randomList[randomCount] = index;
                randomCount++;
              }
            }
            else
            {
              u32 loopNum = gfl2::math::Max<u32>( pTempData[index].randomParam , 1 );
              for( u32 loopCnt = 0 ; loopCnt < loopNum ; ++loopCnt )
              {
                randomList[randomCount] = index;
                randomCount++;
              }
            }
          }
        }

        GFL_ASSERT( randomCount < 20*255 );

        if( randomCount == 0 )
        {//ランダムじゃなかった
          lastIndexList[lastFileNum] = enableIndex;
          lastFileNum++;
        }
        else
        {//ランダムだった

          if( isRandomExists )
            continue;//ランダム受け取り済み

          s32 randIndex = System::GflUse::GetPokemonCreateRand( randomCount );
          lastIndexList[lastFileNum] = randomList[randIndex];
          lastFileNum++;

          isRandomExists = true;
        }

      }
      GFL_SAFE_DELETE_ARRAY( randomList );
    }

    //最終リスト作成
#if 0
    for( u32 index = 0 ; index < lastFileNum ; ++index )
    {
      std::memcpy( &pLastData[index] , &pTempData[lastIndexList[index]] , giftSize );
    }
#else
    /*
      2016/06/08
        Contact[717]   配信物がバッティングした際の「何度でも受信可能」なデータの挙動について
          TPC様要望
          受け取り可能なものを上位に寄せる
    */
    {
      u32 dstIndex  = 0;

      for(u32 loopCount = 0; loopCount < 2; ++loopCount)
      {
        for( u32 index = 0 ; index < lastFileNum ; ++index )
        {
          const u32                                                 lastIndex   = lastIndexList[index];
          const Savedata::MysteryGiftSave::MYSTERY_GIFT_RECV_DATA*  pSrcData    = &pTempData[lastIndex];
          const Savedata::MysteryGiftSave::ReceiveCheckResult       checkResult = Savedata::MysteryGiftSave::ReceiveCheck(pSrcData); 
          const bool                                                isOKData    = ((checkResult == Savedata::MysteryGiftSave::RECEIVE_CHECK_RESULT_OK) || (checkResult == Savedata::MysteryGiftSave::RECEIVE_CHECK_RESULT_NG_FULL));

          /*  1週目と2週目で条件を反転  */
          if(loopCount ? (!isOKData) : isOKData)
          {
            GFL_PRINT("storeIndex:[%02u]\n", lastIndex);
            std::memcpy( &pLastData[dstIndex++], pSrcData, giftSize);
          }
        }
      }
      
      GFL_ASSERT(dstIndex == lastFileNum);    /*  過不足ないかの確認  */
    }
#endif


    /*
      2016/04/27  Yu Muto
      GFNMCat[848]   対象ではないROMで「ふしぎなおくりもの」を受信した際に、エラーコードが表示されない
        データがない場合も(NULL, 0)でListenerをコールするよう修正
    */
//    if( lastFileNum > 0 )
    {
      if(m_pRecvListener)
      {
        if(lastFileNum)
        {
          /*  OK  */
          isContinueReceiving = m_pRecvListener->DeliveryUtilRecvListener_OnRecvComplete(DeliveryUtil::IRecvListener::COMPLETE_RESULT_OK, pLastData, giftSize * lastFileNum);
        }
        else
        {
          /*  見つけたけど受け取れない  */
          isContinueReceiving = m_pRecvListener->DeliveryUtilRecvListener_OnRecvComplete(DeliveryUtil::IRecvListener::COMPLETE_RESULT_FoundButCannotReceive);
        }
      }
    }

    GFL_SAFE_DELETE_ARRAY( pLastData );
    GFL_SAFE_DELETE_ARRAY( pTempData );

    return isContinueReceiving;
  }


  /*    */
  virtual void  OnUpdateProgress( const u32 progress )
  {
    GFL_PRINT("%s\n", __FUNCTION__);

    if(m_pRecvListener)
      m_pRecvListener->DeliveryUtilRecvListener_OnUpdateProgress( progress );
  }


  /*    */
  virtual void  OnNintendoZoneConnected()
  {
    GFL_PRINT("%s\n", __FUNCTION__);

    if(m_pRecvListener)
      m_pRecvListener->DeliveryUtilRecvListener_OnNintendoZoneConnected();
  }

  
  /*    */
  virtual void  OnNetworkError( const nn::Result &result , u32 errorCode )
  {
    GFL_PRINT("%s\n", __FUNCTION__);

    /*
      ユーザのキャンセル時にもOnNetworkErrorがコールされるため、prepareFinalize以降は通知しない
    */
    if(m_IsFinalizing)  return;


    /*
      NMCat[1118]   【ふしぎなおくりもの】にてIRモジュールが故障しているCTR本体に「せきがせんで　うけとる」を選択した際、フェイタルエラーが表示されます。
        下位モジュールで、Ir故障時はNetworkErrorとして通知されるようになった
        ※Irは故障時以外にはNetworkErrorは通知されない
    */
    if(m_Mode == DeliveryUtil::MODE_Ir)
    {
      /*  Irのエラー時はここではAppletを出さない  */
    }
    else
    {
      /*
        applet呼び出し
      */
      if( errorCode == 0 )
      {//エラーコードなし
        if( result != nn::Result() && result.IsSuccess() == false )
        {
          NetLib::Error::NijiNetworkError error;
          error.SetResultBossError( result );
          GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );
        }
      }
      else
      {//エラーコードあり
        NetLib::Error::NijiNetworkError error;
        error.SetResultBossTaskError( errorCode );
        GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );
      }
    }


    if(m_pRecvListener)
    {
      DeliveryUtil::IRecvListener::ResultInfo resultInfo  =
                                              {
                                                DeliveryUtil::IRecvListener::ERROR_Dummy
                                              };
      /**
        DeliveryManagerから投げられるResultをここでフィルタリングしてApplicationに通知する
        @note ※ここでのフィルタリングは行う必要がなくなった
      */
      m_pRecvListener->DeliveryUtilRecvListener_OnNetworkError(resultInfo);
    }
  }


  virtual void  OnWifiError( const u32 errorCode )
  {
    GFL_PRINT("%s\n", __FUNCTION__);

    /*
      ユーザのキャンセル時にもOnNetworkErrorがコールされるため、prepareFinalize以降は通知しない
    */
    if(m_IsFinalizing)  return;

    /*
      applet呼び出し
    */
    NetLib::Error::NijiNetworkError error;
    error.SetResultErrorCode( errorCode );
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );
    if(m_pRecvListener)
    {
      DeliveryUtil::IRecvListener::RecvWifiErrorCode  recvWifiErrorCode = DeliveryUtil::IRecvListener::WIFIERR_Dummy;
      
      /**
        DeliveryManagerから投げられるResultをここでフィルタリングしてApplicationに通知する
        @note ※ここでのフィルタリングは行う必要がなくなった
      */
      m_pRecvListener->DeliveryUtilRecvListener_OnWifiError(recvWifiErrorCode);
    }
  }

  /*
    NetLib::Delivery::DeliveryFileListFilter
  */
  virtual s32   CheckFileList( const nn::boss::NsaList::NsaInformation* pFileList, const u32 fileNum )
  {
    s32 resultIndex = -1;

    if(m_pFileListFilter)
    {
      DeliveryUtil::IFileListFilter::NSAInfo* pNSAInfos = GFL_NEW(m_pWorkBase->GetAppHeap()->GetDeviceHeap()) DeliveryUtil::IFileListFilter::NSAInfo[fileNum];
      DeliveryUtil::IFileListFilter::NSAInfo* pNSAInfo  = &pNSAInfos[0];

      for(u32 index = 0; index < fileNum; ++index, ++pNSAInfo, ++pFileList)
      {
        std::memcpy(pNSAInfo->fileName, pFileList->fileName, sizeof(pNSAInfo->fileName));         pNSAInfo->fileName[GFL_NELEMS(pNSAInfo->fileName)-1]  = '\0';
        pNSAInfo->fileSize          = pFileList->fileSize;
        pNSAInfo->updateEpochTime   = pFileList->updateEpochTime;
        std::memcpy(pNSAInfo->attribute1, pFileList->attribute1, sizeof(pNSAInfo->attribute1));
        std::memcpy(pNSAInfo->attribute2, pFileList->attribute2, sizeof(pNSAInfo->attribute2));
        std::memcpy(pNSAInfo->attribute3, pFileList->attribute3, sizeof(pNSAInfo->attribute3));
        std::memcpy(pNSAInfo->caption,    pFileList->caption,    sizeof(pNSAInfo->caption));      pNSAInfo->caption[GFL_NELEMS(pNSAInfo->caption)-1]    = '\0';
      }
      m_pFileListFilter->DeliveryUtilFileListFilter_CheckFileList(pNSAInfos, fileNum);

      GFL_SAFE_DELETE_ARRAY(pNSAInfos);
    }

    return  resultIndex;
  }
  virtual u32   GetSerialIdMax( void )  {return (m_pFileListFilter) ? m_pFileListFilter->DeliveryUtilFileListFilter_GetSerialIdMax() : 0;}
  virtual u32   GetSerialIdMin( void )  {return (m_pFileListFilter) ? m_pFileListFilter->DeliveryUtilFileListFilter_GetSerialIdMin() : 0;}

private:
  NetAppLib::System::ApplicationWorkBase*   m_pWorkBase;
  NetLib::Delivery::DeliveryManager*        m_pDeliv;

  DeliveryUtil::ISendListener*              m_pSendListener;
  DeliveryUtil::IRecvListener*              m_pRecvListener;
  DeliveryUtil::IFileListFilter*            m_pFileListFilter;

  DeliveryUtil::Mode                        m_Mode;
  bool                                      m_IsFinalizing;

};  /* class DeliveryUtilImpl   */



#else   /*  #if defined(GF_PLATFORM_CTR)  */

// =============================================================================
/*
    非CTR
*/
//==============================================================================

class DeliveryUtilImpl
  : public  DeliveryUtilImplBase
{
  GFL_FORBID_COPY_AND_ASSIGN(DeliveryUtilImpl);
public:
  DeliveryUtilImpl(NetAppLib::System::ApplicationWorkBase* pWorkBase) : DeliveryUtilImplBase(pWorkBase) {}
  virtual ~DeliveryUtilImpl() {}
};

#endif  /*  #if defined(GF_PLATFORM_CTR)  */



// =============================================================================
/*
    以下Platform共通
*/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  DeliveryUtil::Create(NetAppLib::System::ApplicationWorkBase* pWorkBase)
{
  if(!m_pInstance)
  {
    gfl2::heap::HeapBase* pHeapBase = pWorkBase->GetAppHeap()->GetDeviceHeap();

    m_pInstance = GFL_NEW(pHeapBase)  DeliveryUtil(pWorkBase);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  DeliveryUtil::Destroy(void)
{
  if(m_pInstance)
  {
    m_pInstance->Finalize();

    GFL_SAFE_DELETE(m_pInstance);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
DeliveryUtil*  DeliveryUtil::GetInstance(void)  {
  return m_pInstance;
}




//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
DeliveryUtil::DeliveryUtil(NetAppLib::System::ApplicationWorkBase* pWorkBase)
  : m_pImpl(NULL)
{
  m_pImpl = GFL_NEW(pWorkBase->GetAppHeap()->GetDeviceHeap()) DeliveryUtilImpl(pWorkBase);
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
DeliveryUtil::~DeliveryUtil(void)
{
  GFL_SAFE_DELETE(m_pImpl);
}











/*
  以下pImpl呼び出し
*/
bool  DeliveryUtil::Initialize(const DeliveryUtil::Mode mode)       {return m_pImpl->Initialize(mode);}
void  DeliveryUtil::Finalize(void)                                  {m_pImpl->Finalize();}
void  DeliveryUtil::Update(void)                                    {m_pImpl->Update();}
void  DeliveryUtil::SetAttribute(const EAttributePresetID attrID)   {m_pImpl->SetAttribute(attrID);}
void  DeliveryUtil::ClearAttribute( s32 attributeIndex )            {m_pImpl->ClearAttribute(attributeIndex);}
void  DeliveryUtil::SetFileListFilter( IFileListFilter* pFilter )   {m_pImpl->SetFileListFilter(pFilter);}
bool  DeliveryUtil::StartSend(
        const u16     deliveryID,
        const void*   pSendData,
        const size_t  dataSize
      )
      {return m_pImpl->StartSend(deliveryID , pSendData , dataSize);}

bool  DeliveryUtil::StartRecv(void)                                 {return m_pImpl->StartRecv();}
void  DeliveryUtil::PrepareFinalize( void )                         {m_pImpl->PrepareFinalize();}
bool  DeliveryUtil::IsCompretePrepareFinalize( void )               {return m_pImpl->IsCompretePrepareFinalize();}
void  DeliveryUtil::SetSendListener( ISendListener* pSendListener ) {m_pImpl->SetSendListener(pSendListener);}
void  DeliveryUtil::RemoveSendListener()                            {m_pImpl->RemoveSendListener();}
void  DeliveryUtil::SetRecvListener( IRecvListener* pRecvListener ) {m_pImpl->SetRecvListener(pRecvListener);}
void  DeliveryUtil::RemoveRecvListener()                            {m_pImpl->RemoveRecvListener();}
bool  DeliveryUtil::IsInitialized( void ) const                     {return m_pImpl->IsInitialized();}

bool  DeliveryUtil::GetWirelessSwitchState(void) const              {return m_pImpl->GetWirelessSwitchState();}
void  DeliveryUtil::SetSerialCode(const u32 serialCode)             {return m_pImpl->SetSerialCode(serialCode);}



} /*  namespace Gift    */
} /*  namespace NetApp  */
