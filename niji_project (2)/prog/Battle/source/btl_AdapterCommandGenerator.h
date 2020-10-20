//=============================================================================
/**
 * @file   btl_AdapterCommandGenerator.h
 * @date   2015/09/05 16:56:22
 * @author obata_toshihiro
 * @brief  アダプターコマンド生成
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_ADAPTERCOMMANDGENERATOR_H_INCLUDED
#define BTL_ADAPTERCOMMANDGENERATOR_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "./btl_server_const.h"
#include "./btl_server_TimeLimit.h"
#include "./btl_ServerFlow.h"
#include "./btl_rec.h"
#include "./btl_SendData_ClientLimitTime.h"


GFL_NAMESPACE_BEGIN( btl )

  class SendDataContainer;
  class ServerCommandCalculator;


  /**
   * @class AdapterCommandGenerator
   * @brief アダプターコマンドを生成する
   *        サーバーは、生成するコマンドが既に送信済みなら、生成処理を行わない。
   *        このクラスは、サーバから、生成するコマンドが送信済みかどうかを隠蔽する。
   */
  class AdapterCommandGenerator
  {
    GFL_FORBID_COPY_AND_ASSIGN( AdapterCommandGenerator );

  public:

    /**
     * @brief セットアップパラメータ
     */
    struct SetupParam
    {
      SendDataContainer*        sendDataContainer;       // 送信データの生成に参照する、送信済みデータコンテナ
      ServerCommandCalculator*  serverCommandCalclator;  // サーバーコマンドの生成に使用するオブジェクト
      ServerFlow*               serverFlow;              // サーバーコマンドの生成に使用するオブジェクト
      const SCQUE*              serverCommandQueue;      // serverFlow が生成したサーバーコマンドが格納されるコマンドキュー
      rec::SendTool*            recTool;                 // 送信データの生成に使用するツール
      const SVCL_WORK*          clientWorkArray;         // 送信データの生成に参照する、全クライアントの管理ワーク
      const SVCL_ACTION*        clientAction;            // 送信データの生成に参照する、全クライアントの行動パラメータ
      const u8*                 changePokePosArray;      // 送信データの生成に参照する、入れ替え対象位置の配列
      const u8*                 changePokePosNum;        // 送信データの生成に参照する、入れ替え対象位置の数
      const u8*                 enemyPutPokeId;          // 送信データの生成に参照する、相手が次に繰り出そうとしているポケモンのID
      const BTL_RESULT_CONTEXT* battleResultContext;     // 送信データの生成に参照する、勝敗コンテキスト
      const EscapeInfo*         escapeInfo;              // 送信データの生成に参照する、逃走情報
      const TimeLimit*          timeLimit;               // 送信データの生成に参照する、制限時間情報
    };

     /**
      * @brief コンストラクタ
      * @param setupParam  セットアップパラメータ
      */
    AdapterCommandGenerator( const SetupParam& setupParam );

     /**
      * @brief デストラクタ
      */
    virtual ~AdapterCommandGenerator();
    

    /**
     * @brief サーバーからのリクエストから、送信するアダプターコマンドを生成する
     * @param[out] adapterCommand   送信するアダプターコマンド
     * @param[out] data             送信するコマンドデータ本体へのポインタの格納先
     * @param[out] dataSize         送信するコマンドデータ本体のサイズの格納先
     * @param      serverReq        サーバーからのリクエストID
     * @param      commandSerialNo  生成するコマンドの通し番号
     */
    void GenerateCommandData( AdapterCommand* adapterCommand, const void** data, u32* dataSize, ServerRequest serverReq, u32 commandSerialNo );

    /**
     * @brief フロー計算の結果を取得する
     */
    ServerFlow::Result GetFlowResult( void ) const;

    /**
     * @brief 逃げる、次を出す　選択において、「逃げる」コマンドに成功したか？
     * @retval true   成功した
     * @retval false  失敗した
     */
    bool IsEscapeSucceededOnChangeOrEscape( void ) const;
    



  private:

    AdapterCommand GetAdapterCommand( ServerRequest serverReq ) const;
    void CalcCommandData( const void** data, u32* dataSize, ServerRequest serverReq );


    SendDataContainer*        m_sendDataContainer;     
    ServerCommandCalculator*  m_serverCommandCalculator;
    ServerFlow*               m_serverFlow;            
    const SCQUE*              m_serverCommandQueue;
    rec::SendTool*            m_recTool;
    const SVCL_WORK*          m_clientWorkArray;
    const SVCL_ACTION*        m_clientAction;
    const u8*                 m_changePokePosArray;
    const u8*                 m_changePokePosNum;
    const u8*                 m_enemyPutPokeId;
    const BTL_RESULT_CONTEXT* m_battleResultContext;
    const EscapeInfo*         m_escapeInfo;
    const TimeLimit*          m_timeLimit;

    SEND_DATA_CLIENT_LIMIT_TIME_SYNC m_sendData_ClientLimitTime;

    bool               m_isBattleInEventOccured;  // 初期ポケモン投入コマンド生成時に、何らかのイベントが発生したか？
    bool               m_isEscapeSucceeded;       // 逃げる、次を出す　選択において、「逃げる」を選択した場合、逃げることに成功したか？
    ServerFlow::Result m_flowResult;              // フロー計算の結果
  };


GFL_NAMESPACE_END( btl )

#endif // BTL_ADAPTERCOMMANDGENERATOR_H_INCLUDED