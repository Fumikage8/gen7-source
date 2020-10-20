#pragma once

//=================================================================================================
//
//  システムコントロール用Utility
//
//=================================================================================================
#include <mmsystem.h>
#include <strsafe.h>

//=================================================================================================
//
//  高分解能タイマーコントロール
//
//=================================================================================================
class MMTimerControl
{
public:
  // コンストラクタ
	MMTimerControl(){ valid = FALSE; }

  // デストラクタ
	~MMTimerControl(){ cleanup(); }

	bool activate(void)
  {
	  // コンピュータが現在サポートしている精度を取得
	  TIMECAPS timercaps;
	  ZeroMemory(&timercaps, sizeof(timercaps));

	  if(timeGetDevCaps(&timercaps, sizeof(TIMECAPS)) != TIMERR_NOERROR){ return FALSE; }
	  UPeriod = timercaps.wPeriodMin;	//タイマー分解能取得

    // アプリケーションまたはデバイスドライバの最小タイマ分解能を設定する
	  if(timeBeginPeriod(UPeriod) != TIMERR_NOERROR){ return false; }

    prevTime = timeGetTime();
    valid = true;

		return true;
  }

	void cleanup(void)
	{
		if(valid == true){ 
			timeEndPeriod(UPeriod);
			valid = false;
		}
	}

  // 現在のタイマー取得
  DWORD getTime(){ return timeGetTime(); }

  // フレームカウントリセット
  void resetTimeFrame( void )
  {
    prevTime = timeGetTime();
  }

  // フレームカウント取得
  DWORD getTimeFrame( DWORD interval )
  {
    DWORD time = timeGetTime();
		DWORD diff = time - prevTime;

    prevTime = time - (diff % interval);
    return diff / interval;
  }

  // 内部変数
public:
  bool  valid;

private:
  UINT  UPeriod;
  DWORD prevTime;
  DWORD interval;
};


//=================================================================================================
//
// ファイルからデータ読み込み
//
//=================================================================================================
inline void* ut_LoadFile(const wchar_t* filename, DWORD* size = NULL)
{
  char* srcBuffer;

  // ファイルオープン
  HANDLE file = CreateFile( filename, 
												    GENERIC_READ,
												    FILE_SHARE_READ,
												    NULL,
												    OPEN_EXISTING,
												    FILE_ATTRIBUTE_READONLY,
												    NULL);
  if(file == INVALID_HANDLE_VALUE){ return NULL; }

  // 読み込み用バッファ作成
  DWORD fileSize = GetFileSize(file, NULL);
  srcBuffer = new char[fileSize];

  // 読み込み
  DWORD readSize;
  ReadFile(file, srcBuffer, fileSize, &readSize, NULL);
  CloseHandle(file);
  if(readSize != fileSize){
    delete[] srcBuffer;
    return NULL;
  }
  if(size){ *size = fileSize; }

  return (void*)srcBuffer;
}

