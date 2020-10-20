//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		TrainetrPairData.cpp
 *	@brief  視線トレーナーペアデータ管理モジュール
 *	@author Miyuki Iwasawa	
 *	@date		2016.01.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __TRAINERPAIRDATA_H__
#define __TRAINERPAIRDATA_H__


// 前方宣言
namespace GameSys{
  class GameManager;
  class GameData;
}

namespace trainer{

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//----------------------------------------------------------------------------

  class TrainerPairData
  {
    GFL_FORBID_COPY_AND_ASSIGN(TrainerPairData); //コピーコンストラクタ＋代入禁止
 
  public:
    enum TrainerPairIndex{
      TRAINER_PAIR_NONE,
      TRAINER_PAIR_1ST,
      TRAINER_PAIR_2ND,
    };
  private:
    enum{
      TRAINER_PAIR_NUM_MAX = 20, //20ペアまでメモリサイズ保障
    };
    struct PairData{
      u32  scrid_trainer01; //一人目のトレーナーのスクリプトID
      u32  scrid_trainer02; //二人目のトレーナーのスクリプトID
    };

  public:

    // コンストラクタ　デストラクタ
    TrainerPairData( const GameSys::GameData* p_gdata );
    ~TrainerPairData();

    //----------------------------------------------------------------------------
    /**
     *	@brief  TrainerPairDataデータロード
     */
    //-----------------------------------------------------------------------------
    void Initialize( GameSys::GameManager* p_gman, gfl2::heap::HeapBase* pHeap );

    //----------------------------------------------------------------------------
    /**
     *	@brief  TrainerPairDataデータ待ち
     */
    //-----------------------------------------------------------------------------
    bool InitializeWait( GameSys::GameManager* p_gman );

    //----------------------------------------------------------------------------
    /**
     *	@brief  TrainerPairDataデータ破棄
     */
    //-----------------------------------------------------------------------------
    void Terminate( void );

    //----------------------------------------------------------------------------
    /**
     *	@brief  ペアデータのサーチ
     */
    //-----------------------------------------------------------------------------
    TrainerPairIndex SearchPairData( u32 scrid, u32* scrid_pair  );

  private:
    const GameSys::GameData* m_pGameData;

    int   m_seq;
    gfl2::heap::HeapBase* m_pHeap;
    void* m_pTrainerPairBuffer;
    u32   m_TrainerPairBufferSize;
    u32   m_TrainerPairSize;
  };

} // trainer 

#endif	// __TRAINERPAIRDATA_H__

