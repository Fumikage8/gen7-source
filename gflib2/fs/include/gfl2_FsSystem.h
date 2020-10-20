#if !defined(GFL2_FSSYSTEM_H_INCLUDED)
#define GFL2_FSSYSTEM_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsSystem.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   ファイルシステム
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


// fsのインクルード
#include <fs/include/gfl2_FsToolDefine.h>
#include <fs/include/gfl2_FsDefine.h>
#include <fs/include/gfl2_FsResult.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


// 前方宣言
class AsyncFileManager;
class DeviceBase;
class DirectSdmcFile;


#if   defined(GF_PLATFORM_WIN32)

class Win32Device;

#elif defined(GF_PLATFORM_CTR)

class CtrRomDevice;

#if GFL_FS_HIO_ENABLE
// CTR(3DS)のHIO
class CtrHioDevice;
#endif
#if GFL_FS_SDMC_ENABLE
// CTR(3DS)のSDカード(デバッグ用)
class CtrSdmcDevice;
class CtrSdmcPathDevice;
#endif

#endif  // GF_PLATFORM_




//==============================================================================
/**
 @class     FsSystem
 @brief     ファイルシステム
 */
//==============================================================================
class System
{
  GFL_FORBID_COPY_AND_ASSIGN(System);


public:
  static const u32 FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX_DEFAULT = 32;  //!< ArcFileのログFsDebugLogArcFileInfoの最大数のデフォルト値




  ////////////////////////////////////////////////////////////////////////////////
public:
  //! @brief  コンストラクタ
  System(void);
  //! @brief  デストラクタ
  virtual ~System();




  ////////////////////////////////////////////////////////////////////////////////
public:
  //! @brief      初期化処理
  //! @param[in]  devHeapForHio      HIOを初期化するのに使用するメモリ。必ずデバイスメモリで。
  //                                 この関数内で一定量のメモリを確保し、それをTerminateを呼ぶまで保持し続ける（devHeapForHio自体は覚えておかない）。
  //                                 NULLのときはHIOを使えません。
  //! @param[in]  heapForFsDebugLog  ファイルシステムのデバッグ用のログのヒープ。通常ヒープメモリでもデバイスメモリでもいい。
  //                                 このヒープに対して確保/解放を随時行うので、専用のヒープを渡して下さい。
  //                                 NULLのときはデバッグ用のログを録りません。
  static Result Initialize(gfl2::heap::HeapBase* devHeapForHio, gfl2::heap::HeapBase* heapForFsDebugLog);
  //! @brief  終了処理
  static Result Terminate(void);

  //! @brief  rom用の非同期ファイルマネージャを得る(現在のプラットフォームに合ったファイルマネージャを取得する)
  static AsyncFileManager* GetAsyncFileManagerForRomDevice(void);

  //! @brief  ArcSrcデバイスを得る
  static const DeviceBase* GetArcSrcDevice(void);

  //! @brief  開発中専用デバイスDirectSdmcを得る
  static const DeviceBase* GetDirectSdmcDevice(void);


  // CTR(3DS)のHIO
  // プラットフォームがWin32でも呼び出せますが、Win32では必ずfalseになります。
  // コンフィギュレーション(DebugとかReleaseとか)が「HIOが無効のもの」でも呼び出せますが、「HIOが無効のもの」では必ずfalseになります。
  //
  //! @brief       HIOが有効になっているか否か(有効になっていても使わないという選択は呼び出し元でしていい)
  //! @param[out]  有効になっていないときfalse。下記の順で判定します。
  //!                (1) HIOのプログラムコードが無効のときfalse
  //!                (2) HIOの初期化がされていないときfalse
  //! @return      異常がなければres.IsSuccess()がtrueになるが、そのときenableFlagはtrueもfalseもあり得る。
  //!              異常があればfalseになり、そのときenableFlagは必ずfalseになる。
  //! @note        Set関数はない。HIOを一度有効にしたら、どのファイルがHIOを使っているか分からないので、無効にはできない。
  static Result GetHioEnableFlag(bool* enableFlag);

  // CTR(3DS)のSDカード(デバッグ用)
  // プラットフォームがWin32でも呼び出せますが、Win32では必ずfalseになります。
  // コンフィギュレーション(DebugとかReleaseとか)が「SDカードが無効のもの」でも呼び出せますが、「SDカードが無効のもの」では必ずfalseになります。
  //
  //! @brief       SDカードが有効になっているか否か(有効になっていても使わないという選択は呼び出し元でしていい)
  //! @param[out]  有効になっていないときfalse。下記の順で判定します。
  //!                (1) SDカードのプログラムコードが無効のときfalse
  //!                (2) SDカードの初期化がされていないときfalse
  //! @return      異常がなければres.IsSuccess()がtrueになるが、そのときenableFlagはtrueもfalseもあり得る。
  //!              異常があればfalseになり、そのときenableFlagは必ずfalseになる。
  //! @note        Set関数はない。
  //!              代わりに、プラットフォームがCTRのときにだけ呼び出せるInitializeCtrSdmcDevice, TerminateCtrSdmcDevice関数がある。
  static Result GetSdmcEnableFlag(bool* enableFlag);


  //! @brief  DirectSdmc用のファイルインスタンスを生成し、ファイルをオープンする(デバッグ用)
  //! @param[in] heap      ヒープ。この関数内で確保したメモリはDestroyDirectSdmcFilCloseするまで保持される。
  //! @param[in] fileName  ファイル名。終端は'\0'になっていること。区切りは\でも/でもよい。
  //! @param[in] openMode  オープンモード。
  //! @return  生成＆オープンできなかったときはNULLを返す。
  static DirectSdmcFile* CreateDirectSdmcFile(gfl2::heap::HeapBase* heap, const char* fileName, Define::OpenMode::Tag openMode);
  //! @brief  DirectSdmc用のファイルをクローズし、ファイルインスタンスを破棄する(デバッグ用)
  //! @param[in]  file  DirectSdmc用のファイルインスタンス(CreateFileForDirectSdmcの戻り値)。NULLのときは何もしない。
  static void DestroyDirectSdmcFile(DirectSdmcFile* file);


  ////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////
#if   defined(GF_PLATFORM_WIN32)


  //! @name  Win32のromデバイス
  //@{

public:
  //! @brief  Win32のromデバイスをマウントする
  //! @param[in] heap  ヒープ。Win32のromデバイスインスタンスの作成とパス名の記憶に使用。UnmountWin32RomDeviceするまで左記のものが保持され続ける。
  //! @param[in] path  パス。末尾の'/'は書いても書かなくてもどちらでもよい。終端は'\0'になっていること。
  //                   以降ファイル名を指定するときはpathより後を指定する。
  //                   例：path="C:/home/niji/" (path="C:/home/niji", path="C:\\home\\niji\\", path="C:\\home\\niji" どれでも同じ)
  //                       fileName="git_gflib2/fs/Sample/data/read_data.txt"
  //                       とすると、"C:/home/niji/git_gflib2/fs/Sample/data/read_data.txt" が対象となる。
  static Result MountWin32RomDevice(gfl2::heap::HeapBase* heap, const char* path);
  //! @brief  Win32のromデバイスをアンマウントする
  static Result UnmountWin32RomDevice(void);
  //! @brief  Win32のromデバイスを得る
  static const DeviceBase* GetWin32RomDevice(void);


  //! @brief  Win32のromデバイス用の非同期ファイルマネージャを生成する
  //! @param[in] fsDebugLogArcFileInfoElemNumMax  ArcFileのログFsDebugLogArcFileInfoの最大数。
  static Result CreateAsyncFileManagerForWin32RomDevice(gfl2::heap::HeapBase* heap, int fileThreadPrio, int otherThreadPrio, u32 fsDebugLogArcFileInfoElemNumMax=FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX_DEFAULT);
  //! @brief  Win32のromデバイス用の非同期ファイルマネージャを破棄する
  static Result DestroyAsyncFileManagerForWin32RomDevice(void);
  //! @brief  Win32のromデバイス用の非同期ファイルマネージャを得る
  static AsyncFileManager* GetAsyncFileManagerForWin32RomDevice(void);

  //@}
 

  //! @name  Win32のArcSrc
  //@{

public:
  static Result MountWin32ArcSrcDevice(gfl2::heap::HeapBase* heap, const char* path);
  static Result UnmountWin32ArcSrcDevice(void);
private:
  static const DeviceBase* getWin32ArcSrcDevice(void);

  //@}


  //! @name  Win32の開発中専用デバイスDirectSdmc
  //@{

public:
  //! @brief  開発中専用デバイスDirectSdmcをマウントする
  static Result MountWin32DirectSdmcDevice(gfl2::heap::HeapBase* heap, const char* path);
  //! @brief  開発中専用デバイスDirectSdmcをアンマウントする
  static Result UnmountWin32DirectSdmcDevice(void);

  //@}


  ////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////
#elif defined(GF_PLATFORM_CTR)


  //! @name  CTR(3DS)のrom
  //@{

public:
  //! @brief  CTR(3DS)のromをマウントする
  //! @param[in] heap  ヒープ。CTR(3DS)のromインスタンスの作成とパス名の記憶に使用。romアーカイブの動作に使用するメモリ領域の確保も行う。UnmountCtrRomDeviceするまで左記のものが保持され続ける。
  //! @param[in] archiveName  アーカイブ名。終端は'\0'になっていること。
  //                          例：archiveName="rom:" ("/"はどこにも入りません)
  //! @param[in] path  パス。先頭の'/'は書いて下さい。末尾の'/'は書いても書かなくてもどちらでもよい。終端は'\0'になっていること。
  //                   以降ファイル名を指定するときはpathより後を指定する。
  //                   例：path="/data/"("C:/home/niji/data/"があり、romfiles="C:/home/niji"となっているときの例)
  //                       fileName="sample/sample_data.txt"
  //                       とすると、"rom:/data/sample/sample_data.txt"即ち"C:/home/niji/data/sample/sample_data.txt"が対象となる。
  //                       (実際は、romfilesはアーカイブされており、そこから呼び出します。PCから呼び出すわけではありません。)
  static Result MountCtrRomDevice(gfl2::heap::HeapBase* heap, const char* archiveName, const char* path, size_t maxFile, size_t maxDirectory, bool useCache);
  //! @brief  CTR(3DS)のromをアンマウントする
  static Result UnmountCtrRomDevice(void);
  //! @brief  CTR(3DS)のromを得る
  static const DeviceBase* GetCtrRomDevice(void);


  //! @brief  CTR(3DS)のrom用の非同期ファイルマネージャを生成する
  //! @param[in] fsDebugLogArcFileInfoElemNumMax  ArcFileのログFsDebugLogArcFileInfoの最大数。
  static Result CreateAsyncFileManagerForCtrRomDevice(gfl2::heap::HeapBase* heap, int fileThreadPrio, int otherThreadPrio, u32 fsDebugLogArcFileInfoElemNumMax=FS_DEBUG_LOG_ARC_FILE_INFO_ELEM_NUM_MAX_DEFAULT);
  //! @brief  CTR(3DS)のrom用の非同期ファイルマネージャを破棄する
  static Result DestroyAsyncFileManagerForCtrRomDevice(void);
  //! @brief  CTR(3DS)のrom用の非同期ファイルマネージャを得る
  static AsyncFileManager* GetAsyncFileManagerForCtrRomDevice(void);

  //@}


  //! @name  CTR(3DS)のHIOのrom
  //@{

public:
  static Result MountCtrHioRomDevice(gfl2::heap::HeapBase* heap, const char* path);
  static Result UnmountCtrHioRomDevice(void);
  static const DeviceBase* GetCtrHioRomDevice(void);

  //@}
  

  //! @name  CTR(3DS)のArcSrc
  //@{

public:
  static Result MountCtrArcSrcDevice(gfl2::heap::HeapBase* heap, const char* path);
  static Result UnmountCtrArcSrcDevice(void);
private:
  static const DeviceBase* getCtrArcSrcDevice(void);

  //@}


  //! @name  CTR(3DS)のSDカード(デバッグ用)のデバイス
  //@{
  
public:
  static Result InitializeCtrSdmcDevice(gfl2::heap::HeapBase* heap, const char* archiveName);
  static Result TerminateCtrSdmcDevice(void);
  
  //@}


//  //! @name  CTR(3DS)のSDカード(デバッグ用)のrom
//  //@{
//  
//public:
//  //! @param[in] path  InitializeCtrSdmcDeviceのarchiveName以降のパスを指定。
//  //! @note  InitializeCtrSdmcDeviceを先に呼んでおいて下さい。
//  static Result MountCtrSdmcRomDevice(gfl2::heap::HeapBase* heap, const char* path);
//  static Result UnmountCtrSdmcRomDevice(void);
//  static const DeviceBase* GetCtrSdmcRomDevice(void);
// 
//  //@}

//↓SDカード(デバッグ用)をromとして使うのではなく、デバッグ用途で読み書きに使うので下記のような開発中専用デバイスにした。
 
  //! @name  CTR(3DS)のSDカード(デバッグ用)の開発中専用デバイスDirectSdmc
  //@{

public:
  //! @brief  開発中専用デバイスDirectSdmcをマウントする
  //! @param[in] path  InitializeCtrSdmcDeviceのarchiveName以降のパスを指定。
  //! @note  InitializeCtrSdmcDeviceを先に呼んでおいて下さい。
  static Result MountCtrDirectSdmcDevice(gfl2::heap::HeapBase* heap, const char* path);
  //! @brief  開発中専用デバイスDirectSdmcをアンマウントする
  static Result UnmountCtrDirectSdmcDevice(void);

  //@}


#endif  // GF_PLATFORM_



  ////////////////////////////////////////////////////////////////////////////////
private:
  static bool m_onceInitializeFlag;  // 一度初期化したら、もう二度と初期化しないもの
  static bool m_initializeFlag;      // Initializeで初期化処理をし、Terminateで終了処理をするもの


#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_HIO_ENABLE
  // CTR(3DS)のHIO
  static void* m_hioBuf;  // HIOのワークメモリ。デバイスメモリ。
#endif
#endif  // GF_PLATFORM_CTR

#if GFL_FS_DEBUG_LOG
  static gfl2::heap::HeapBase* m_fsDebugLogHeap;
#endif


  ////////////////////////////////////////////////////////////////////////////////
#if   defined(GF_PLATFORM_WIN32)


  static Win32Device*      m_win32RomDevice;
  static AsyncFileManager* m_asyncFileManagerForWin32RomDevice;
 

  ////////////////////////////////////////////////////////////////////////////////
#elif defined(GF_PLATFORM_CTR)

  
  static CtrRomDevice*     m_ctrRomDevice;
  static AsyncFileManager* m_asyncFileManagerForCtrRomDevice;
  
  //static CtrSaveDataDevice*     m_ctrSaveDataDevice;
  //static CtrExtSaveDataDevice*  m_ctrExtSaveDataDevice;
  //static CtrSdmcDevice*         m_ctrSdmcDevice;
  //static CtrExtraRomDevice*     m_ctrExtraRomDevice;


#endif  // GF_PLATFORM_


  ////////////////////////////////////////////////////////////////////////////////
#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_HIO_ENABLE
  // CTR(3DS)のHIO
  static CtrHioDevice*  m_ctrHioRomDevice;
#endif
#endif  // GF_PLATFORM_CTR


#if   defined(GF_PLATFORM_CTR)
#if GFL_FS_SDMC_ENABLE
  // CTR(3DS)のSDカード(デバッグ用)
  static CtrSdmcDevice*  m_ctrSdmcDevice;  // SDカード(デバッグ用)のデバイス。romデバイスか開発中専用デバイスDirectSdmcかはつくった人次第。
#endif
#endif  // GF_PLATFORM_CTR


  ////////////////////////////////////////////////////////////////////////////////
#if GFL_FS_GASC_ENABLE
#if   defined(GF_PLATFORM_WIN32)
  // Win32のArcSrc
  static Win32Device*   m_win32ArcSrcDevice;
#elif defined(GF_PLATFORM_CTR)
  // CTR(3DS)のArcSrc(CTR(3DS)のHIO)
  static CtrHioDevice*  m_ctrArcSrcDevice;
#endif  // GF_PLATFORM_
#endif


  ////////////////////////////////////////////////////////////////////////////////
#if GFL_FS_DIRECT_SDMC_DEVICE_USE
  static DeviceBase* m_directSdmcDevice;  // 開発中専用デバイスDirectSdmc  // Win32ではPCが、CTRではSDカード(デバッグ用)がデバイスになります。
#endif

};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL2_FSSYSTEM_H_INCLUDED
