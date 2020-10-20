#if PM_DEBUG
//======================================================================
/**
 * @file DebugMovie.h
 * @date 2016/02/26 11:43:25
 * @author nagihashi_toru
 * @brief ムービーのデバッグ機能
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __DEBUG_MOVIE_HEADER_H_INCLUDED__
#define __DEBUG_MOVIE_HEADER_H_INCLUDED__
#pragma once

#include <base/include/gfl2_Singleton.h>

GFL_NAMESPACE_BEGIN(movie)
class DebugMovieImpl;

/**
* @class DebugMovie
* @brief Movie のデバッグ用処理クラス
*/
class DebugMovie : public gfl2::base::Singleton<DebugMovie>
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugMovie);

public:
  /**
    *  @brief  ctor
    */
  DebugMovie();
  /**
    *  @brief  デストラクタ
    */
  virtual ~DebugMovie();
  /**
    *  @brief  初期化
    */
  void Initialize();
  /**
    *  @brief  破棄
    */
  void Finalize();
  /**
    *  @brief  スキップ設定かどうか
    *  @return true で強制スキップ false で通常状態
    */
  bool IsSkip();
  /**
    *  @brief  ループ設定かどうか
    *  @return true で強制ループ状態 false で通常状態
    */
  bool IsLoop();
  /**
    *  @brief  シングルトン生成
    */
  static void InitializeSingleton();
  /**
    *  @brief  シングルトン破棄
    */
  static void FinalizeSingleton();

private:
  DebugMovieImpl *m_impl;
};

GFL_NAMESPACE_END(movie)

SINGLETON_INSTANCE_DECLARATION( movie::DebugMovie );

#endif // __DEBUG_MOVIE_HEADER_H_INCLUDED__
#endif //PM_DEBUG
