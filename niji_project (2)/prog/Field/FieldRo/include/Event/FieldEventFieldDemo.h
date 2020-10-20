//======================================================================
/**
 * @file FieldEventFieldDemo.h
 * @brief デモシーケンス再生イベント
 * @author saita_kazuki
 * @data 2015.11.10
 */
//======================================================================

#if !defined( __FIELD_EVENT_FIELD_DEMO_H_INCLUDED__)
#define __FIELD_EVENT_FIELD_DEMO_H_INCLUDED__
#pragma once

// gfl2
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <util/include/gfl2_List.h>

#include "DemoLib/SequencePlayer/include/SequenceRenderPipeline.h"

// gamesys
#include "GameSys/include/GameEvent.h"

// 前方参照
namespace Field {
  class Fieldmap;
  class FieldRenderingPipeLineManager;

  namespace Effect {
    class EffectDummyPolygon;
    class EffectManager;
  }
}
namespace gfl2 {
  namespace renderingengine { namespace scenegraph {
    namespace resource {
      class TextureResouceNode;
    }
  }}
}
namespace DemoLib { namespace Sequence {
  class SequenceViewSystem;
}}


GFL_NAMESPACE_BEGIN( Field )

/**
 * @class EventFieldDemo
 * @brief デモシーケンス再生イベント
 */
class EventFieldDemo : public GameSys::GameEvent
{
  /**
   * @brief コピー禁止クラスにする
   */
  GFL_FORBID_COPY_AND_ASSIGN( EventFieldDemo );

public:

  /**
   * @brief 実行オプション
   */
  enum Option
  {
    OPTION_NONE                     = 0x00,           ///< なし
    OPTION_USE_FIELD_DRAW_CAPTURE   = 0x01 << 0,      ///< フィールド描画をキャプチャーにする
  };

  /**
   * @brief デモシステム未使用オプション
   * @note エフェクトシステムはnwレベルでシングルトンなため、フィールド生成時には使用できないため必ず未使用になります
   */
  enum UnUseOption
  {
    UN_USE_OPTION_NONE              = 0x00,         ///< なし
    UN_USE_OPTION_CHARA_MODEL       = 0x01 << 0,    ///< キャラモデルを使用しない
    UN_USE_OPTION_TEXT_WINDOW       = 0x01 << 1,    ///< テキストウインドウを使用しない
    UN_USE_OPTION_POKEMON           = 0x01 << 2,    ///< ポケモンシステムを使用しない
  };

  /**
   * @brief 初期化設定
   */
  struct Description
  {
    Description()
      : pFieldmap( NULL)
      , demoID( 0)
      , option( 0)
      , unUseOption( 0)
      , renderUseOption()
      , isForceOneStep( true)
      , textureIndex( 0)
      , pTextureNode( NULL)
    {}

    Fieldmap*                                               pFieldmap;          ///< フィールドマップ
    u32                                                     demoID;             ///< 再生するデモのdatID(demo_seq_script.gaix)
    u32                                                     option;             ///< 実行オプションビットフラグ
    u32                                                     unUseOption;        ///< デモシステム未使用オプションビットフラグ
    DemoLib::Sequence::SequenceRenderingPipeLine::UseOption renderUseOption;    ///< デモ描画使用オプション
    bool                                                    isForceOneStep;     ///< 強制1フレームモードにするか
    s32                                                     textureIndex;       ///< 差し替えテクスチャ番号
    gfl2::renderingengine::scenegraph::DagNode*             pTextureNode;       ///< 差し替えテクスチャ(NULLなら何もしない)
  };

public:

  /**
   * @brief コンストラクタ
   * @param pHeap GameEventに渡すヒープ
   */ 
  EventFieldDemo( gfl2::heap::HeapBase* pHeap);
  
  /**
   * @brief デストラクタ
   */ 
  virtual ~EventFieldDemo();

  /**
   * @brief 初期化処理(インスタンス生成直後に実行する)
   * @param desc 初期化設定
   */ 
  void Initialize( const Description& desc);

  /**
   * @brief イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @param gmgr ゲームマネージャークラス
   * @retval true 起動してよい
   * @retval false 起動しない
   */
  virtual bool BootChk(GameSys::GameManager* gmgr);

  /**
   * @brief bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @param gmgr ゲームマネージャークラス
   */
  virtual void InitFunc(GameSys::GameManager* gmgr);

  /**
   * @brief 呼び出される関数 純粋仮想関数
   * @param gmgr ゲームマネージャークラス
   * @return GameSys::GMEVENT_RESULT イベント制御関数の戻り値
   */
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* gmgr);

  /**
   * @brief 呼び出される関数 純粋仮想関数
   * @param gmgr ゲームマネージャークラス
   */
  virtual void EndFunc(GameSys::GameManager* gmgr);

private:

  /**
   * @brief オプション関数
   */
  typedef bool (EventFieldDemo::*OptionFunc)( s32* pSeq);

  /**
   * @class オプション関数実行
   */
  class OptionFunction
  {
  public:
    OptionFunction();
    OptionFunction( EventFieldDemo* pOwner, EventFieldDemo::OptionFunc pFunc);
    virtual ~OptionFunction();

    bool Execute();
    bool IsFinish() const ;

  private:
    EventFieldDemo*             m_pOwner;
    EventFieldDemo::OptionFunc  m_pFunc;
    s32                         m_seq;
    bool                        m_isFinish;
  };

  // DLL定義
  static const u32                                            RO_MODULE_NUM = 2;                  ///< Roモジュール数
  static const char* const                                    RO_MODULES[ RO_MODULE_NUM ];        ///< Roモジュール名

private:

  bool ConvertUseOption( u32 check, u32 bit);

  void LoadDllModules( gfl2::heap::HeapBase* pHeap);
  void DisposeDllModules();

  void InitializeSequenceViewSystem();
  bool TerminateSequenceViewSystem();

  void InitializeOptionFunction();
  bool UpdateOptionFunction( gfl2::util::List<OptionFunction>* pList);

  void RequestCapture();

  // オプション用関数

  bool InitializeCapturePipeLine( s32* pSeq);
  bool TerminateCapturePipeLine( s32* pSeq);

  bool InitializeCaptureTexture( s32* pSeq);
  bool TerminateCaptureTexture( s32* pSeq);

  bool InitializeCapturePolygon( s32* pSeq);
  bool TerminateCapturePolygon( s32* pSeq);

  bool ChangeExecuteTypeCapture( s32* pSeq);
  bool ChangeExecuteTypeNormal( s32* pSeq);

private:

  Description                                                       m_desc;                               ///< 初期化情報
  bool                                                              m_isBoot;                             ///< 起動して良いか

  gfl2::heap::HeapBase*                                             m_pHeap;                              ///< ヒープ
  System::nijiAllocator*                                            m_pAllocator;                         ///< アロケーター
  gfl2::ro::Module*                                                 m_pRoModules[ RO_MODULE_NUM ];        ///< Roモジュール

  // アクセスしやすいよう保持する                                  
  FieldRenderingPipeLineManager*                                    m_pFieldRenderingPipeLineManager;     ///< フィールド描画管理クラス
  Effect::EffectManager*                                            m_pEffectManager;                     ///< エフェクト管理クラス
                                                                   
  // 初期化、終了処理リスト                                        
  gfl2::util::List<OptionFunction>                                  m_initializeFuncList;                 ///< オプション用初期化関数リスト
  gfl2::util::List<OptionFunction>                                  m_terminateFuncList;                  ///< オプション用破棄関数リスト

  // シーケンス                                                    
  DemoLib::Sequence::SequenceViewSystem*                            m_pSequenceViewSystem;                ///< デモシーケンスシステム

  // フィールドキャプチャー描画用
  Effect::EffectDummyPolygon*                                       m_pCapturePolygon;                    ///< キャプチャ差し替え用板ポリゴンモデル
  gfl2::renderingengine::scenegraph::resource::TextureResourceNode* m_pTextureResourceNode;               ///< テクスチャリソースノード

}; // class EventFieldDemo

GFL_NAMESPACE_END( Field )

#endif // __FIELD_EVENT_FIELD_DEMO_H_INCLUDED__

