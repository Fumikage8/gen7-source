#ifndef GFLIB2_GRX_CTR_CTRGPUPROFILER_H_INCLUDED
#define GFLIB2_GRX_CTR_CTRGPUPROFILER_H_INCLUDED

#include <gfx/include/CTR/gfl2_CTRTypes.h>
#include <nn/os.h>

namespace gfl2 { namespace gfx { namespace ctr {

class CTRCommandList;
class CTRTimerQuery;

class CTRGPUProfiler
{
public:

  struct TYPE
  {
    enum ENUM
    {
      // BUSY
      BUSY_SHADER0 = 0,           // シェーダープロセッサ0
      BUSY_COMMAND_VERTEX_LOAD,   // コマンドバッファおよび頂点アレイロードモジュール
      BUSY_RASTERIZE,             // ラスタライズ
      BUSY_TRIANGLE_SETUP,        // トライアングルセットアップ
      BUSY_FRAGMENT,              // フラグメントライティング
      BUSY_TEXTURE_UNIT,          // テクスチャユニット
      BUSY_PERFRAGMENT,           // パーフラグメントオペレーション
      BUSY_TEXTURE_COMB,          // テクスチャコンバイナ

      // VERTEX_SHADER_0
      VERTEX0_PROGCOUNTER,        // 頂点シェーダー０　プログラムカウンタ
      VERTEX0_CMD_STOLE,          // 頂点シェーダー０　命令の依存関係によるストールしたクロック数
      VERTEX0_MOVA_CMD_STOLE,     // 頂点シェーダー０　アドレスレジスタの更新によるストールしたクロック数
      VERTEX0_CMP_CMD_STOLE,      // 頂点シェーダー０　ステータスレジスタの更新によるストールしたクロック数
      VERTEX0_FETCH_MISS_STOLE,   // 頂点シェーダー０　プリフェッチのミスによりストールしたクロック数

      // VERTEX_SHADER_1
      VERTEX1_PROGCOUNTER,        // 頂点シェーダー０　プログラムカウンタ
      VERTEX1_CMD_STOLE,          // 頂点シェーダー０　命令の依存関係によるストールしたクロック数
      VERTEX1_MOVA_CMD_STOLE,     // 頂点シェーダー０　アドレスレジスタの更新によるストールしたクロック数
      VERTEX1_CMP_CMD_STOLE,      // 頂点シェーダー０　ステータスレジスタの更新によるストールしたクロック数
      VERTEX1_FETCH_MISS_STOLE,   // 頂点シェーダー０　プリフェッチのミスによりストールしたクロック数

      // VERTEX_SHADER_2
      VERTEX2_PROGCOUNTER,        // 頂点シェーダー０　プログラムカウンタ
      VERTEX2_CMD_STOLE,          // 頂点シェーダー０　命令の依存関係によるストールしたクロック数
      VERTEX2_MOVA_CMD_STOLE,     // 頂点シェーダー０　アドレスレジスタの更新によるストールしたクロック数
      VERTEX2_CMP_CMD_STOLE,      // 頂点シェーダー０　ステータスレジスタの更新によるストールしたクロック数
      VERTEX2_FETCH_MISS_STOLE,   // 頂点シェーダー０　プリフェッチのミスによりストールしたクロック数

      // VERTEX_SHADER_3
      VERTEX3_PROGCOUNTER,        // 頂点シェーダー０　プログラムカウンタ
      VERTEX3_CMD_STOLE,          // 頂点シェーダー０　命令の依存関係によるストールしたクロック数
      VERTEX3_MOVA_CMD_STOLE,     // 頂点シェーダー０　アドレスレジスタの更新によるストールしたクロック数
      VERTEX3_CMP_CMD_STOLE,      // 頂点シェーダー０　ステータスレジスタの更新によるストールしたクロック数
      VERTEX3_FETCH_MISS_STOLE,   // 頂点シェーダー０　プリフェッチのミスによりストールしたクロック数

      // POLYGON
      POLYGON_INPUT_VERTEX,       // トライアングルセットアップへの入力頂点数
      POLYGON_INPUT_POLYGON,      // トライアングルセットアップへの入力ポリゴン数
      POLYGON_OUTPUT_POLYGON,     // トライアングルセットアップへの出力ポリゴン数

      // FRAGMENT
      FRAGMENT_INPUT_FRAGMENT,    // 入力フラグメント数カウンタの値

      // メモリアクセスカウント（バイト数）
      ACCESS_VRAMA_READ,                 // PICA による VRAM A チャネルのリード
      ACCESS_VRAMA_WRITE,                // PICA による VRAM A チャネルのライト
      ACCESS_VRAMB_READ,                 // PICA による VRAM B チャネルのリード
      ACCESS_VRAMB_WRITE,                // PICA による VRAM B チャネルのライト
      ACCESS_CMD_VERTEX,                 // コマンドバッファ、頂点アレイ、インデックスアレイのリード
      ACCESS_TEX_UNIT,                   // テクスチャユニットによる　テクスチャメモリリード
      ACCESS_DEPTH_READ,                 // フラグメントオペレーション　デプスバッファ　リード
      ACCESS_DEPTH_WRITE,                // フラグメントオペレーション　デプスバッファ　ライト
      ACCESS_COLOR_READ,                 // フラグメントオペレーション　カラーバッファ　リード
      ACCESS_COLOR_WRITE,                // フラグメントオペレーション　カラーバッファ　ライト
      ACCESS_LCD_UP_DISP_READ,           // 上画面LCDによる　ディスプレイバッファ　リード
      ACCESS_LCD_DOWN_DISP_READ,         // 下画面LCDによる　ディスプレイバッファ　ライト
      ACCESS_POST_READ,                  // ポスト転送モジュールによるリード
      ACCESS_POST_WRITE,                 // ポスト転送モジュールによるライト
      ACCESS_MEMFILL0_WRITE,             // メモリフィルモージュールのチャネル０によるバッファのライト
      ACCESS_MEMFILL1_WRITE,             // メモリフィルモージュールのチャネル１によるバッファのライト
      ACCESS_READPIXCEL,                // glReadPixelsなどによるVRAMリード
      ACCESS_DMA_VRAM_WRITE,            // DMA転送による、VRAMライト

      TOTAL_COUNT                       // 総数
    };
  };

  // プロファイリング結果
  struct ProfilingResult
  {
    u32 values[TYPE::TOTAL_COUNT];
  };

public:

  // コンストラクタ
	CTRGPUProfiler();
  // デストラクタ
	~CTRGPUProfiler();

  // GPUプロファイラー用の.orcsのファイル名を設定
  void SetOrcsName(const char* pFileName);

  // GPUプロファイラー用の.orcsファイルを出力
  void ExportOrcs(CTRCommandList* pTargetList);

  // GPUの処理合計時間の計測開始
  void StartGPUTotalTime(CTRCommandList* pTargetList);

  // GPUの処理合計時間を取得
  u64 GetTotalGPUTimeMicroSeconds() const
  {
    return m_TotalGPUTimeMicroSeconds;
  }

  // CPUの処理合計時間の計測開始
  void StartCPUTotalTime();

  // CPUの処理合計時間の計測終了
  void EndCPUTotalTime();

  // CPUの処理合計時間を取得
  u64 GetTotalCPUTimeMicroSeconds() const
  {
    return m_TotalCPUTimeMicroSeconds;
  }

  // タイマークエリの開始コールバック登録
  void SetTimerQueryBeginCallback(CTRTimerQuery* pTimerQuery, CTRCommandList* pCommandList);

  // タイマークエリの終了コールバック登録
  void SetTimerQueryEndCallback(CTRTimerQuery* pTimerQuery, CTRCommandList* pCommandList);

  // GPUプロファイリングの開始
  void StartGPUProfile();

  // GPUプロファイリングの終了
  void StopGPUProfile();

  // GPUプロファイリング結果の取得
  const ProfilingResult& GetGPUProfileResult() const
  {
    return m_ProfilingResult;
  }

  // 現在の時間をマイクロ秒で取得
  static s64 GetCurrentMicroSeconds()
  {
    return nn::os::Tick::GetSystemCurrent().ToMicroSeconds();
  }


private:

  // 取得された生のプロファイリング結果
  struct RawProfilingResult
  {
    GLuint busy[4];
    GLuint vertexShader[4][5];
    GLuint vertexCache;
    GLuint polygon[3];
    GLuint fragment;
    GLuint memoryAccess[18];
  };

  // タイマークエリ用パラメータ
  struct TimerQueryParam
  {
    CTRTimerQuery* pTimerQuery;
    s32 beginRequestNo;
    s32 endRequestNo;
  };

  // タイマークエリ数
  static const u32 MAX_TIMER_QUERY_COUNT = 10;

  // orcs名
  static const u32 MAX_ORCS_NAME_LENGTH = 256;

private:

  // コマンドリスト終了時のコールバック
  static void CommandListDoneCallback(s32 requestNo);

private:

  // 出力する名前
  char m_OrcsName[MAX_ORCS_NAME_LENGTH];

  // GPU総処理時間
  u64 m_TotalCPUTimeMicroSeconds;
  s64 m_TotalCPUTimeMicroSecondsBegin;

  // GPU総処理時間
  u64 m_TotalGPUTimeMicroSeconds;
  s64 m_TotalGPUTimeMicroSecondsBegin;
  s32 m_TotalGPUTimeRequestNo;
  b32 m_TotalGPUTimeWaiting;
  CTRCommandList* m_pTotalGPUTimeCommandList;

  // 処理時間コールバック
  TimerQueryParam m_TimerQueryParamList[MAX_TIMER_QUERY_COUNT];

  // プロファイリング
  b32 m_ProfilingWating;
  RawProfilingResult m_RawProfilingResult;
  ProfilingResult m_ProfilingResult;
  ProfilingResult m_ProfilingResultPrev;

  // コールバック用にインスタンスが必要
  static CTRGPUProfiler* s_pInstance;
};

}}}

#endif
