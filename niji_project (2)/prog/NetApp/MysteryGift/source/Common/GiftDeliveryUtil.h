#if !defined(GIFTDELIVERYUTIL_H_INCLUDED)
#define GIFTDELIVERYUTIL_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftDeliveryUtil.h
*  @date   2015.06.16 Tue.
 * @author muto_yu
*  @brief  NetLib::Delivery::DeliveryManagerのwrapper
*          DeliveryManagerがnotCTRでシンボルごと失われるので、platform差分をここで吸収
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>


namespace NetAppLib {
namespace System  {
  class ApplicationWorkBase;
}
}


namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================
class DeliveryUtilImpl;


//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015.06.17 Wed.
  */
//==============================================================================
class DeliveryUtil
{
  GFL_FORBID_COPY_AND_ASSIGN(DeliveryUtil);
  


public:
  static void           Create(NetAppLib::System::ApplicationWorkBase* pWorkBase);
  static void           Destroy(void);
  static DeliveryUtil*  GetInstance(void);


  /*
  */
  class ISendListener
  {
  public:
    enum  SendNetworkErrorCode
    {
      ERROR_Dummy,

      ERROR_
    };

    

    typedef struct
    {
      SendNetworkErrorCode errorCode;
    }ResultInfo;



    virtual void  DeliveryUtilSendListener_OnNetworkError(const ResultInfo& rResultInfo) = 0;
  };


  /*
  */
  class IRecvListener
  {
  public:
    enum  CompleteResult
    {
      COMPLETE_RESULT_OK,                     /**<  受け取り可能            */

      COMPLETE_RESULT_NotFound,               /**<  見つからなかった        */
      COMPLETE_RESULT_FoundButCannotReceive,  /**<  見つけたが受け取れない  */
      COMPLETE_RESULT_ReceiveFailed,          /**<  受け取りに失敗          */

      COMPLETE_RESULT_
    };

    //------------------------------------------------------------------
    /**
    * @brief 受信完了イベント
    *
    * @param pData 受信データ
    * @return 受信を続けるかどうか( true : 続ける , false : 続けない)
    */
    //------------------------------------------------------------------
    virtual bool DeliveryUtilRecvListener_OnRecvComplete(const CompleteResult completeResult, const void* pData =NULL, size_t dataSize =0) = 0;

    //------------------------------------------------------------------
    /**
    * @brief 受信経過更新イベント
    *
    * @param progress 受信経過（一番進んでいるものを%で表したもの）
    */
    //------------------------------------------------------------------
    virtual void DeliveryUtilRecvListener_OnUpdateProgress( const u32 progress ) = 0;

    //------------------------------------------------------------------
    /**
    * @brief ニンテンドーゾーン接続通知
    */
    //------------------------------------------------------------------
    virtual void DeliveryUtilRecvListener_OnNintendoZoneConnected() = 0;

    //------------------------------------------------------------------
    /**
    * @brief ネットワークエラーイベント
    *
    * @param result 結果
    */
    //------------------------------------------------------------------
    enum  RecvNetworkErrorCode
    {
      ERROR_Dummy,
      
      ERROR_
    };
    

    typedef struct
    {
      RecvNetworkErrorCode  errorCode;
    }ResultInfo;



    virtual void DeliveryUtilRecvListener_OnNetworkError( const ResultInfo& rResultInfo ) = 0;

    //------------------------------------------------------------------
    /**
    * @brief ネットワークエラーコード
    *
    * @param result 結果
    */
    //------------------------------------------------------------------
    enum  RecvWifiErrorCode
    {
      WIFIERR_Dummy,

      WIFIERR_
    };

    virtual void DeliveryUtilRecvListener_OnWifiError( const RecvWifiErrorCode errorCode ) = 0;
  };



  //------------------------------------------------------------------
  /**
   * @brief 配信ファイル用フィルター
   */
  //------------------------------------------------------------------
  class IFileListFilter
  {
  public:
    /*
      nn::boss::NsaList::NsaInformation と同等
    */
    typedef struct
    {
      char  fileName[32];     /**<  最大31文字  */
      u32   fileSize;         /**<  bytes       */
      u32   updateEpochTime;  /**<  ファイルの更新時間(unix epochであるため、1970-01-01 00:00:00からの経過秒数)。NSAの公開開始日を値を用いるため、ダウンロード時のHTTPレスポンスのLast-Modifiedと一致する値となる。*/
      char  attribute1[10];   /**<  属性1(最大9文字の文字列)。(0-9)|(a-z,A-Z)|(_)。サーバ側UIからNSAごとに任意の値を設定できる。NSAリスト取得時にそれに含ませるNSAを指定するための検索要素となる。  */
      char  attribute2[10];
      char  attribute3[10];
      u8    caption[150];     /**<  utf8, 最大50文字  */
    }NSAInfo;

    //------------------------------------------------------------------
    /**
      * @brief    受け取るファイルを決定する
      * @return   受け取るファイルのIndex、受け取らない場合は負
      */
    //------------------------------------------------------------------
//    virtual s32 DeliveryUtilFileListFilter_CheckFileList( const nn::boss::NsaList::NsaInformation* pFileList , const u32 fileNum ) = 0;
    virtual s32 DeliveryUtilFileListFilter_CheckFileList( const NSAInfo* pNsaInfo, const u32 fileNum ) = 0;

    //------------------------------------------------------------------
    /**
    * @brief シリアルIDの範囲取得（最大）
    */
    //------------------------------------------------------------------
    virtual u32 DeliveryUtilFileListFilter_GetSerialIdMax( void ) = 0;

    //------------------------------------------------------------------
    /**
    * @brief シリアルIDの範囲取得（最少）
    */
    //------------------------------------------------------------------
    virtual u32 DeliveryUtilFileListFilter_GetSerialIdMin( void ) = 0;
  };


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  enum  Mode
  {
    MODE_UDS,             /**<  ローカル                                              */
    MODE_Ir,              /**<  赤外線                                                */
    MODE_Boss,            /**<  Internet(BOSSサーバ)                                  */
    MODE_BossViaPGLAuth,  /**<  Internet(BOSSサーバ):シリアルコードによるPGL認証経由（内部でネットワークのInit/Finalizeを行わない）  */
    
    MODE_NONE
  };


  bool  Initialize(const Mode mode);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  void  Finalize(void);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  void  Update(void);


  //------------------------------------------------------------------
  /**
   * @brief   検索属性の設定（Wifi用）
   */
  //------------------------------------------------------------------
  enum  EAttributePresetID
  {
    ATTR_Dummy,
    
    ATTR_MAX
  };


//  void SetAttribute( const gflnet2::boss::BossManager::BOSS_SEARCH_ATTRIBUTES* pAttributes );
  void SetAttribute( const EAttributePresetID attrID );


  //------------------------------------------------------------------
  /**
   * @brief   検索属性の削除（Wifi用）
   */
  //------------------------------------------------------------------
  void ClearAttribute( s32 attributeIndex );

  //------------------------------------------------------------------
  /**
   * @brief   検索用フィルターの設定（Wifi用）
   */
  //------------------------------------------------------------------
  void SetFileListFilter( IFileListFilter* pFilter );


  //------------------------------------------------------------------
  /**
   * @brief   送信開始
   */
  //------------------------------------------------------------------
  bool StartSend( const u16 deliveryID , const void* pSendData , const size_t dataSize );


  //------------------------------------------------------------------
  /**
   * @brief   受信開始
   */
  //------------------------------------------------------------------
  bool StartRecv(void);


  //------------------------------------------------------------------
  /**
   * @brief  終了準備
   */
  //------------------------------------------------------------------
  void PrepareFinalize( void );

  //------------------------------------------------------------------
  /**
   * @brief  停止確認
   */
  //------------------------------------------------------------------
  bool IsCompretePrepareFinalize( void );


  //------------------------------------------------------------------
  /**
   * @brief   送信リスナー設定
   */
  //------------------------------------------------------------------
  void SetSendListener( ISendListener* pSendListener );
  //------------------------------------------------------------------
  /**
   * @brief   送信リスナー解除
   */
  //------------------------------------------------------------------
  void RemoveSendListener();
  //------------------------------------------------------------------
  /**
   * @brief   受信リスナー設定
   */
  //------------------------------------------------------------------
  void SetRecvListener( IRecvListener* pRecvListener );
  //------------------------------------------------------------------
  /**
   * @brief   受信リスナー解除
   */
  //------------------------------------------------------------------
  void RemoveRecvListener();

  //------------------------------------------------------------------
  /**
  * @brief  初期化状態を調査
  *
  * @retval true： 初期済み
  * @retval false: 未初期化・または初期化中
  */
  //------------------------------------------------------------------
  bool IsInitialized( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief  初期化パラメータの取得
   */
  //------------------------------------------------------------------
//  gflnet2::InitParam* GetInitParam( void );



  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void  SetSerialCode(const u32 serialCode);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  bool  GetWirelessSwitchState(void) const;


private:
  DeliveryUtil(NetAppLib::System::ApplicationWorkBase* pWorkBase);
  virtual ~DeliveryUtil(void);

  static DeliveryUtil*    m_pInstance;
  DeliveryUtilImpl*       m_pImpl;
};


} /*  namespace Gift    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(GIFTDELIVERYUTIL_H_INCLUDED)  */
