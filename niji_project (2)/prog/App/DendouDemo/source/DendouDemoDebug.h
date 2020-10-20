//======================================================================
/**
 * @file DendouDemoDebug.h
 * @date 2016/03/08 17:31:22
 * @author nagihashi_toru
 * @brief 殿堂デモのデバッグ機能
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG
#if !defined __DENDOU_DEMO_H_INCLUDED__
#define __DENDOU_DEMO_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <base/include/gfl2_Singleton.h>
#include <PokeTool/include/PokeTool.h>
#include <debug/include/gfl2_DebugWinSystem.h>
#include <debug/include/gfl2_DebugWinItemTypes.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LightNode.h>
#include <model/include/gfl2_BaseCamera.h>

namespace app {
namespace dendou_demo {

/**
* @class Debug
* @brief DendouDemo のデバッグ用処理クラス
*/
class Debug : public gfl2::base::Singleton<Debug>
{
public:
  /**
    *  @brief  ctor
    */
  Debug();
  /**
    *  @brief  デストラクタ
    */
  virtual ~Debug();

  /**
    *  @brief  ライト調整用にポインタを貰う
    */
  void SetupPokemonDirectionalLight( gfl2::renderingengine::scenegraph::instance::LightNode* pokeDirLight );
  void SetupTrainerDirectionalLight( gfl2::renderingengine::scenegraph::instance::LightNode* trainerDirLight );
  void SetupTrainerAmbiaentLight( gfl2::renderingengine::scenegraph::instance::LightNode* trainerAmbLight );
  /**
    *  @brief  カメラにポインタを貰う
    */
  void SetupCamera( poke_3d::model::BaseCamera* camera );
  /**
    *  @brief  Aging用アップデート処理(終了時呼ぶこと)
    */
  void UpdatePokemonToAging();
  /**
    *  @brief  最初から開始するか
    *  @return true でreset false で通常状態
    */
  bool IsReset() const { return mIsReset; }
  void Reset() { mIsReset = false; }
  /**
    *  @brief  デバッグポケモンを使用するか
    *  @return true で使用 false で通常状態
    */
  bool UseDebugPokemon() const { return mUseDebugPokemon; }
  /**
    *  @brief  ポケモン変更しつつぐるぐる回すエイジングテスト設定どうか
    *  @return true でエイジング false で通常状態
    */
  bool IsAging() const { return mIsAging; }
  /**
    *  @brief  ループ設定かどうか
    *  @return true で強制ループ状態 false で通常状態
    */
  bool IsLoop() const { return mIsLoop; }

  /**
    *  @brief  デバッグ用のSimpleParamを取得
    *  @return SimpleParam
    */
  void GetSimpleParam( u32 index, PokeTool::SimpleParam & param ) const { param = mSimpleParam[index]; }

  /**
    *  @brief  シングルトン生成
    */
  static void InitializeSingleton();
  /**
    *  @brief  シングルトン破棄
    */
  static void FinalizeSingleton();

private:
  void Initialize();
  void Finalize();
  void AddDebugWin();
  void RemoveDebugWin();

  void Update_StopCamera();
  wchar_t* Draw_StopCamera();
  void Update_SetLight();

  static void DebugWinUpdateFunc_StopCamera( void* userWork, gfl2::debug::DebugWinItem *item );
  static wchar_t* DebugWinDrawFunc_StopCamera( void* userWork, gfl2::debug::DebugWinItem *item );
  static void DebugWinUpdateFunc_SetLight( void* userWork, gfl2::debug::DebugWinItem *item );

private:
  bool mIsReset;
  bool mIsLoop;
  bool mUseDebugPokemon;
  bool mIsAging;
  PokeTool::SimpleParam mSimpleParam[6];
  s32 mPokeIndex;
  u32 mAgingCounter;
  gfl2::math::Vector3 mLightPos;
  gfl2::math::Vector3 mLightDir;
  gfl2::math::Vector4 mLightPokeColor;
  gfl2::math::Vector4 mLightDirColor;
  gfl2::math::Vector4 mLightAmvColor;
  gfl2::renderingengine::scenegraph::instance::LightNode* mPokeDirLight;
  gfl2::renderingengine::scenegraph::instance::LightNode* mTrainerDirLight;
  gfl2::renderingengine::scenegraph::instance::LightNode* mTrainerAmbLight;
  poke_3d::model::BaseCamera* mCamera;
  gfl2::debug::DebugWinGroup* m_group;
  gfl2::debug::DebugWinGroup* m_groupPoke;
  gfl2::debug::DebugWinGroup* m_groupLight;
};

}		// dendou_demo
}		// app

SINGLETON_INSTANCE_DECLARATION( app::dendou_demo::Debug );

#endif // __DENDOU_DEMO_H_INCLUDED__
#endif //PM_DEBUG