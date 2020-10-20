/**
 * @file	gfl_File.h
 * @brief	
 * @author	kimura_shin
 * @data	2011/04/19, 11:34
 */

#ifndef GFL_FILE_H
#define	GFL_FILE_H

namespace gfl {
namespace core {

class File {
public:
#ifdef GFL_PLATFORM_PC
	static QString s_RootDir;
#endif

	enum {
		MODE_BINARY,
		MODE_TEXT,

		MAX_PATH_SIZE = 0x100
	};
	// スマートじゃないロード　明示的に GFL_DELETE が必要
	static u8* Load( const c8 * const fname_org, gfl::heap::HeapBase* heap = 0, size_t* read_size = 0, const s32 mode = MODE_BINARY );
	static ByteArrayPointer LoadSmart( const c8 * const fname_org, gfl::heap::HeapBase* heap = 0, const s32 mode = MODE_BINARY );
};

}
}

#endif	/* GFL_FILE_H */
