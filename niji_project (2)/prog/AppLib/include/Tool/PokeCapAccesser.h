//======================================================================
/**
 * @file    PokeCapAccesser.h
 * @brief   ポケモンキャプチャー画像取得アクセサ
 * @author  k.ohno
 * @data    15/10/25
 */
//======================================================================

#ifndef __POKEMON_CAP_ACCESSER_H__
#define __POKEMON_CAP_ACCESSER_H__
#pragma once




GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class Heap;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( fs )
class AsyncFileManager;
GFL_NAMESPACE_END( fs )
GFL_NAMESPACE_END( gfl2 )


GFL_NAMESPACE_BEGIN( pml )
GFL_NAMESPACE_BEGIN( pokepara )
class CoreParam;
GFL_NAMESPACE_END( pokepara )
GFL_NAMESPACE_END( pml )


GFL_NAMESPACE_BEGIN(AppLib)
GFL_NAMESPACE_BEGIN(Tool)


class PokeCapAccesser
{
  GFL_FORBID_COPY_AND_ASSIGN(PokeCapAccesser); //コピーコンストラクタ＋代入禁止

  const static int MANAGEMENT_MAX = 18;
  
public:
  PokeCapAccesser(app::util::Heap* pAppHeap,gfl2::fs::AsyncFileManager* pAsyncFileManager); //コンストラクタ
  virtual ~PokeCapAccesser(void);  //デストラクタ

  //--------------------------------------------------------------------------
  /**
   * @brief   ポケモンに合ったアークファイルのインデックスを返す
   * @param   ポケモン構造体
   * @return  ArcFileIndex
   */
  //--------------------------------------------------------------------------
  int GetArcFileIndex( pml::pokepara::CoreParam* pPoke );

  //--------------------------------------------------------------------------
  /**
   * @brief   ポケモンデータ読み込み
   * @param   index   管理番号
   * @param   読込先のポインタのアドレス
   */
  //--------------------------------------------------------------------------
  void PokemonCaptureRead(int arcFileIndex,void** pReadBuff);

  //--------------------------------------------------------------------------
  /**
   * @brief   ポケモンデータファイルオープン
   * @param   index   管理番号
   * @param   arcFileIndex
   */
  //--------------------------------------------------------------------------
  void OpenPokemonCapture(void);

  //--------------------------------------------------------------------------
  /**
   * @brief   ポケモンデータファイルクローズ
   * @param   index   管理番号
   * @param   arcFileIndex
   */
  //--------------------------------------------------------------------------
  void ClosePokemonCapture(void);

private:
  void PokemonCaptureListRead(void);
  void OpenPokemonCaptureList(void);
  void ClosePokemonCaptureList(void);

  
  app::util::Heap* mpAppHeap;
  gfl2::fs::AsyncFileManager* mpAsyncFileManager;   //ファイルマネージャー
  void* mpReadBuff;  //キャプチャーした絵のリスト

};



GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(AppLib)



#endif  //__POKEMON_CAP_ACCESSER_H__
