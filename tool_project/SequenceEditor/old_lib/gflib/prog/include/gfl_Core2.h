/* 
 * File:   gfl_Core.h
 * Author: s-kimura
 *
 * Created on 2010/05/28, 11:21
 * コアクラス
 */

#ifndef GFL_CORE_H
#define	GFL_CORE_H
#pragma once

// プラットフォーム別の定義が収めてあります
//#include <core/gfl_Config.h>
// プラットフォーム別のヘッダが収めてあります
#include <core/gfl_OsHeader.h>
// プラットフォーム差を吸収する「型定義」が収めてあります
#include <core/gfl_Typedef.h>
// プラットフォーム差を吸収する「マクロ」が収めてあります
#include <core/gfl_Macro.h>
// デバッグ出力クラス　アサートは未定義…
#include <core/gfl_Debug.h>
// ３DS用命令を通すためのダミークラス
#include <core/gfl_Dummy.h>
// 算術ライブラリ　基礎定義など
#include <core/gfl_MathBase.h>
// 便利クラス　現在はアドレス計算のみ
#include <core/gfl_Utility.h>
// タプル構造体、クラス
//#include <core/gfl_Tuple.h>
// ベクタークラス
#include <core/gfl_Vector.h>
// レクトクラス
#include <core/gfl_Rect.h>
// マトリックスクラス
#include <core/gfl_Matrix.h>
// クウォータニオンクラス
#include <core/gfl_Quaternion.h>
// 楕円クラス
#include <core/gfl_Ellipse.h>
// シングルトンクラス
#include <core/gfl_Singleton.h>
// 時間測定クラス
#include <core/gfl_Time.h>
// ハッシュリストクラス
#include <core/gfl_HashList.h>
// カメラクラス
#include <core/gfl_Camera.h>
// スマートポインタ
#include <core/gfl_SmartPointer.h>
// ファイルクラス
#include <core/gfl_CoreFile.h>

#endif	/* GFL_CORE_H */

