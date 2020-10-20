/* 
 * File:   gfl_QtFile.h
 * Author: kimura_shin
 *
 * Created on 2010/12/03, 16:33
 */

#ifndef GFL_QTFILE_H
#define	GFL_QTFILE_H

#include <qt/gfl_QtGui.h>

namespace gfl {
namespace qt {

#define LINK_KEYWORD "GFL_FILE_LINKED"

class File {
public:

	enum {
		MODE_LITTLE_ENDIAN = 0,
		//		MODE_BIG_ENDIAN, // 今回はいらない？
		MODE_ENDIAN_MAX,
	};

	typedef u8* (*MemCatByEndian)(u8* dst, const void* src, const s32 size);
	static u8* MemCatEndianSwap( u8* dst, const void* src, const s32 size );
	static u8* MemCatNoEndianSwap( u8* dst, const void* src, const s32 size );
	/**
	 * ファイルをロード
	 * @param fname ファイル名
	 * @param flag フラグ　テキストかバイナリか等
	 * @return　ファイルデータ
	 */
	static QByteArray Load( const QString& fname, const QIODevice::OpenModeFlag flag = QIODevice::Text );

	static const size_t LINE_BUFFER_SIZE = 0x200;
	/**
	 * テキストファイル行単位読み込み
	 * @param fname　ファイル名
	 * @param remove_crlf 改行コードを削除するか？ デフォルトが真
	 * @return 行単位文字列
	 * @remark 一行の長さに制限(LINE_BUFFER_SIZE)があります。
	 */
	static QStringList LoadLines( const QString& fname, const bool remove_crlf = true );

	static QByteArray LoadBinary( const QString& fname, const QIODevice::OpenModeFlag flag = QIODevice::ReadOnly ) {
		return Load( fname, flag );
	}

	/**
	 * ファイルを保存
	 * @param fname　ファイル名
	 * @param dat　書きこむデータ
	 * @param flag　フラグ　テキストかバイナリか等
	 * @return 真　保存成功　偽　失敗
	 */
	static bool Save( const QString& fname, const QByteArray& dat, const QIODevice::OpenModeFlag flag = QIODevice::Text );

	static bool SaveBinary( const QString& fname, const QByteArray& dat, const QIODevice::OpenModeFlag flag = QIODevice::ReadOnly ) {
		return Save( fname, dat, flag );
	}

	/**
	 * バイナリーファイルをリンクする
	 * @param save_fname　リンクされたファイル名
	 * @param fnames　リンクするファイル名リスト
	 * @param endian　エンディアン
	 * @param align_size　アラインメントサイズ
	 * @param header　ヘッダファイルを書きだすか？
	 * @return 真　成功　偽　失敗
	 */
	static bool LinkBinaryFiles( const QString& save_fname, const QStringList& fnames, const s32 endian = MODE_LITTLE_ENDIAN
								 , const u32 align_size = sizeof (u32), bool header = true );

	/**
	 * 修正タイムスタンプの比較
	 * @param orgfname　元ファイル名
	 * @param tagfname　先ファイル名
	 * @return 真　元ファイルの方があたらしいか、先ファイルがない
	 */
	static bool CheckModified( const QString& orgfname, const QString& tagfname );

	/**
	 * 修正タイムスタンプの比較
	 * @param orgtime　ファイルの元の修正時間
	 * @param fname　ファイル名
	 * @return 真　ファイルが更新された
	 */
	static bool CheckModified( const QDateTime& orgtime, const QString& fname );

	/**
	 * 修正タイムスタンプの取得
	 * @param fname ファイル名
	 * @return 修正時間
	 */
	static QDateTime GetModifiedTime( const QString& fname );
	
	/**
	 * 絶対パスへ変換
     * @param fname 相対パスファイル名
     * @return 絶対パスファイル名
     */
	static QString ToAbsoluteFilePath( const QString& fname );
};

}}

#endif	/* GFL_QTFILE_H */

