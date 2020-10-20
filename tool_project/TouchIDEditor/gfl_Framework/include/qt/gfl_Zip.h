/* 
 * File:   gfl_Zip.h
 * Author: s-kimura
 *
 * Created on 2010/06/14, 18:50
 * 圧縮
 */

#ifndef GFL_ZIP_H
#define	GFL_ZIP_H

#include <qt/gfl_QtGui.h>

namespace gfl {
namespace qt {

/**
 *   zlib ラッパークラス
 */
class Zip {
public:
	/**
	 *   zlib エンコードファイルをデコードしてメモリーに展開
	 * @param  dst 展開されたデータ
	 * @param  fname ファイル名
	 * @return 真　成功　偽　失敗
	 */
	static bool DecodeFile( QByteArray* dst, const QString& fname );

	/**
	 *   ファイルを圧縮してファイルとして保存
	 * @param  rfname 読み込みファイル名
	 * @return 無し
	 */
	static void EncodeFile( const QString& rfname );
};

}
}

#endif	/* GFL_ZIP_H */

