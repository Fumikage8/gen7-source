/**
 * @file btl_AiScriptCommandHandler.h
 * @brief 戦闘AIスクリプトコマンド処理
 * @author obata_toshihiro
 * @date 2015.04.03
 */

#ifndef BTL_AI_SCRIPT_COMMAND_HANDLER_H_INCLUDED
#define BTL_AI_SCRIPT_COMMAND_HANDLER_H_INCLUDED

#include "../btl_common.h"


GFL_NAMESPACE_BEGIN( btl )

class MainModule;
class ServerFlow;
class POKECON;
class BTL_POKEPARAM;


/**
 * @class AiScriptCommandHandler
 * @brief 戦闘AIスクリプトのコマンドを処理する。
 *        スクリプトからコマンドが呼ばれると
 *        コマンドに対応する関数が呼ばれ、AiScriptCommandHandler が渡される。
 *        コマンド関数は、AiScriptCommandHandler を通じて各種バトルパラメータにアクセスする。
 */
class AiScriptCommandHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( AiScriptCommandHandler );

public:

  //---------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   * @param heapHandle  使用するヒープ
   * @param mainModule  参照する MainModule
   * @param serverFlow  参照する ServerFlow
   * @param pokecon     全ポケモンを保持するコンテナ
   * @param randContext 使用する乱数コンテキスト
   */
  //---------------------------------------------------------------------------
  AiScriptCommandHandler( 
    HeapHandle heapHandle,
    const MainModule* mainModule,
    ServerFlow* serverFlow,
    const POKECON* pokecon,
    const u32 randSeed );

  //---------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //---------------------------------------------------------------------------
  virtual ~AiScriptCommandHandler();




  /**
   * @brief スクリプトコマンドの挙動を決定するパラメータ
   */
  struct CommandParam
  {
    const BTL_POKEPARAM* attackPoke;  // 攻撃側ポケモンのパラメータ
    const BTL_POKEPARAM* defensePoke; // 防御側ポケモンのパラメータ

    u8                   currentWazaIndex; // 採点対象の技のインデックス
    WazaNo               currentWazaNo;    // 採点対象の技の技No.
    u16                  currentItemNo;    // 採点対象の道具No.
    const BTL_POKEPARAM* currentBenchPoke; // 採点対象の控えポケモンのパラメータ


    CommandParam( void ) : 
      attackPoke( NULL ),
      defensePoke( NULL ),
      currentWazaIndex( 0 ),
      currentWazaNo( WAZANO_NULL ),
      currentItemNo( ITEM_DUMMY_DATA ),
      currentBenchPoke( NULL ) {}
  };

  //---------------------------------------------------------------------------
  /**
   * @brief コマンドの挙動を決定するパラメータを設定する
   */
  //---------------------------------------------------------------------------
  void SetCommandParam( const CommandParam& commandParam );

  //---------------------------------------------------------------------------
  /**
   * @brief 直前に使ったワザをワークに保存しておく
   * @param bpp  技の使用履歴を保存するポケモン
   */
  //---------------------------------------------------------------------------
  void StoreUsedWaza( const BTL_POKEPARAM* bpp );

  //---------------------------------------------------------------------------
  /**
   * @brief 使用履歴保存ワークに指定された技があるか判定
   * @param bpp      チェック対象のポケモン
   * @param waza_no  チェック対象の技
   * @retval true   履歴にある
   * @retval false  履歴にない
   */
  //---------------------------------------------------------------------------
  bool CheckWazaStored( const BTL_POKEPARAM* bpp, WazaNo waza_no ) const;




  //---------------------------------------------------------------------------
  /**
   * @brief 乱数ジェネレータを取得する
   */
  //---------------------------------------------------------------------------
  AIRandSys* GetRandGenerator( void ) const;

  //---------------------------------------------------------------------------
  /**
   * @brief MainModule を取得する
   */
  //---------------------------------------------------------------------------
  const MainModule* GetMainModule( void ) const;

  //---------------------------------------------------------------------------
  /**
   * @brief ServerFlow を取得する
   */
  //---------------------------------------------------------------------------
  ServerFlow* GetServerFlow( void ) const;

  //---------------------------------------------------------------------------
  /**
   * @brief POKECON を取得する
   */
  //---------------------------------------------------------------------------
  const POKECON* GetPokeCon( void ) const;

  //---------------------------------------------------------------------------
  /**
   * @brief 攻撃側ポケモンのパラメータを取得する
   * @retval NULL  攻撃側ポケモンが登録されていない場合
   */
  //---------------------------------------------------------------------------
  const BTL_POKEPARAM* GetAttackPokeParam( void ) const;

  //---------------------------------------------------------------------------
  /**
   * @brief 防御側ポケモンのパラメータを取得する
   * @retval NULL  防御側ポケモンが登録されていない場合
   */
  //---------------------------------------------------------------------------
  const BTL_POKEPARAM* GetDefensePokeParam( void ) const;

  //---------------------------------------------------------------------------
  /**
   * @brief 採点対象の控えポケモンのパラメータを取得する
   */
  //---------------------------------------------------------------------------
  const BTL_POKEPARAM* GetBenchPokeParam( void ) const;

  //---------------------------------------------------------------------------
  /**
   * @brief 攻撃側ポケモンの立ち位置を取得する
   * @retval BTL_POS_NULL  攻撃側ポケモンが登録されていない場合
   */
  //---------------------------------------------------------------------------
  BtlPokePos GetAttackPokePos( void ) const;

  //---------------------------------------------------------------------------
  /**
   * @brief 防御側ポケモンの立ち位置を取得する
   * @retval BTL_POS_NULL  攻撃側ポケモンが登録されていない場合
   */
  //---------------------------------------------------------------------------
  BtlPokePos GetDefensePokePos( void ) const;

  //---------------------------------------------------------------------------
  /**
   * @brief 現在採点中の技のインデックスを取得する
   */
  //---------------------------------------------------------------------------
  u8 GetCurrentWazaIndex( void ) const;

  //---------------------------------------------------------------------------
  /**
   * @brief 現在採点中の技No.を取得する
   */
  //---------------------------------------------------------------------------
  WazaNo GetCurrentWazaNo( void ) const;

  //---------------------------------------------------------------------------
  /**
   * @brief 現在採点中の道具No.を取得する
   */
  //---------------------------------------------------------------------------
  u16 GetCurrentItemNo( void ) const;




  //---------------------------------------------------------------------------
  /**
   * @brief ポケモン位置からポケモンパラメータを取得する
   * @param pos  ポケモンの位置
   * @return 指定した場所にいるポケモンのパラメータ
   */
  //---------------------------------------------------------------------------
  const BTL_POKEPARAM*  GetBpp( BtlPokePos pos ) const;

  //---------------------------------------------------------------------------
  /**
   * @brief AI指定サイドからポケモンパラメータする取得する
   * @param ai_side  AI指定サイド( CHECK_ATTACK, CHECK_DEFENCE など )
   */
  //---------------------------------------------------------------------------
  const BTL_POKEPARAM*  GetBppByAISide( u32 ai_side ) const;

  //---------------------------------------------------------------------------
  /**
   *  AI指定サイドからクライアントID取得
   * @param ai_side  AI指定サイド( CHECK_ATTACK, CHECK_DEFENCE など )
   */
  //---------------------------------------------------------------------------
  BtlPokePos AISideToPokePos( u32 ai_side ) const;

  //---------------------------------------------------------------------------
  /**
   *  AI指定サイドからクライアントID取得
   * @param ai_side  AI指定サイド( CHECK_ATTACK, CHECK_DEFENCE など )
   */
  //---------------------------------------------------------------------------
  u8 AISideToClientID( u32 ai_side ) const;
  
  //---------------------------------------------------------------------------
  /**
   * @brief ポケモンの持っている特性をチェック
   *        ( 相手側なら、すでに見えているとくせいか否かで処理を別ける )
   *
   * @param ai_side  AI指定サイド( CHECK_ATTACK, CHECK_DEFENCE など )
   */
  //---------------------------------------------------------------------------
  TokuseiNo CheckTokuseiByAISide( int ai_side );

  //---------------------------------------------------------------------------
  /**
   * @brief 攻撃側の持っているワザを全てチェックし、防御側に与えられる最大ダメージを計算する
   * @param atkPoke    攻撃側ポケモン
   * @param defPoke    防御側ポケモン
   * @param loss_flag  ダメージ計算の乱数が有効か
   * @return  攻撃側ポケモンが与えることの出来る最大のダメージ量
   */
  //---------------------------------------------------------------------------
  u32 CalcMaxDamage( const BTL_POKEPARAM* atkPoke, const BTL_POKEPARAM* defPoke, bool loss_flag ) const;




  //---------------------------------------------------------------------------
  /**
   * @brief 「にげる」の選択をリセットする
   */
  //---------------------------------------------------------------------------
  void ResetEscape( void );

  //---------------------------------------------------------------------------
  /**
   * @brief 「にげる」を選択したことを記録する
   */
  //---------------------------------------------------------------------------
  void NotifyEscapeByAI( void );

  //---------------------------------------------------------------------------
  /**
   * @brief 「にげる」を選択したか？
   * @retval true  「にげる」を選択した
   * @retval false 「にげる」を選択していない
   */
  //---------------------------------------------------------------------------
  bool IsEscapeSelected( void ) const;



private:
  BtlPokePos GetPokePos( const BTL_POKEPARAM* pokeParam ) const;


private:

  const MainModule* m_mainModule;
  ServerFlow*       m_serverFlow;
  const POKECON*    m_pokecon;

  CommandParam m_commandParam;
  WazaNo       m_usedWaza[ BTL_POKEID_MAX ][ PTL_WAZA_MAX ]; 

  bool m_isEscapeSelected;

  AIRandSys* m_randGenerator;

};


GFL_NAMESPACE_END( btl )


#endif // BTL_AI_SCRIPT_COMMAND_HANDLER_H_INCLUDED
