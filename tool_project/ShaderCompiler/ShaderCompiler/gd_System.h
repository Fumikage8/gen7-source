/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gd_System.h

  Copyright (C)2011 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
#ifndef NN_GD_CTR_GD_SYSTEM_H_
#define NN_GD_CTR_GD_SYSTEM_H_

#include <nn/gd/CTR/gd_Types.h>
#include <nn/gd/CTR/gd_Result.h>

#include <nn/gd/CTR/gd_VertexInput.h>
#include <nn/gd/CTR/gd_Memory.h>

#if NN_GD_DOCUMENT_TYPE == ENGLISH
/*! @file
  @brief System functions.
*/
#else
/*! @file
  @brief システム関数
*/
#endif

#ifndef NN_UNUSED_VAR
#if NN_GD_DOCUMENT_TYPE == ENGLISH
/*! @brief Unused variable. */
#else
/*! @brief 未使用の変数を設定します。*/
#endif
#define NN_UNUSED_VAR(var)  ((void)&var);
#endif

#if NN_GD_DOCUMENT_TYPE == ENGLISH
/*! @brief Define the type of the callback function pointer called everytime an error occurs
    @param result The error resulting from a call to a Gd function.
    @param functionName In debug and development mode, this is the name of the Gd function which generated the error. In release mode, this is always null.
*/
#else
/*! @brief エラー発生時に呼ばれるコールバック関数のタイプを定義します。
    @param result 呼ばれたGd関数からのエラー戻り値です。
    @param functionName デバック又は開発モードでは、エラーが発生したGd関数名が格納されるポインタを指定します。リリースモードではNULLを指定します。
*/
#endif
typedef void (*gdCallbackfunctionErrorPtr)(nnResult result, const char* functionName);

namespace nn {
namespace gd {
namespace CTR {

#if NN_GD_DOCUMENT_TYPE == ENGLISH
/*! @brief Recorded packet handle, storing information of a packet previously recorded
*/
#else
/*! @brief バーテックスバッファリソースです。バーテックスデータ又はインデックスデータを格納します。
*/
#endif
class RecordedPacketId;

#if NN_GD_DOCUMENT_TYPE == ENGLISH
/*! @defgroup systemFunction System functions
    @brief System functions.
    @{
*/
/*! @} */ // defgroup systemFunction
#else
/*! @defgroup systemFunction システム関数です。
    @brief システム関数です。
    @{
*/
/*! @} */ // defgroup systemFunction
#endif

#if NN_GD_DOCUMENT_TYPE == ENGLISH
/*! 
    :category     System
    @ingroup systemFunction
    @brief The System Module contains the functions related to all the 3D rendering pipeline stages.
    @anchor systemFunction
*/
#else
/*! 
    :category     System
    @ingroup systemFunction
    @brief システム関数は、全ての３Dレンダリングのパイプラインステージに関連する機能が含まれています。
    @anchor systemFunction
*/
#endif
class System
{
public:

    #if NN_GD_DOCUMENT_TYPE == ENGLISH
    /*! @brief Define the module from Gd Api. */
    #else
    /*! @brief 各Gd Apiモジュールのenum値です。*/
    #endif
    enum Module
    {
        #if NN_GD_DOCUMENT_TYPE == ENGLISH
        /*! @brief Define the rasterizer module, this module is available in @ref RasterizerStage class. */
        #else
        /*! @brief ラスタライザモジュールを指定します。このモジュールは @ref RasterizerStage クラスで利用できます。 */
        #endif
        MODULE_RASTERIZER = 1 << 0,

        #if NN_GD_DOCUMENT_TYPE == ENGLISH
        /*! @brief Define the lighting module, this module is available in @ref LightingStage class. */
        #else
        /*! @brief ライトモジュールを指定します。このモジュールは @ref LightingStage クラスで利用できます。 */
        #endif
        MODULE_LIGHTING = 1 << 1,

        #if NN_GD_DOCUMENT_TYPE == ENGLISH
        /*! @brief Define the output module, this module is available in @ref OutputStage class. */
        #else
        /*! @brief アウトプットモジュールを指定します。このモジュールは@ref OutputStage クラスで利用できます。*/
        #endif
        MODULE_OUTPUT = 1 << 2,

        #if NN_GD_DOCUMENT_TYPE == ENGLISH
        /*! @brief Define the texture combiner module, this module is available in @ref CombinerStage class. */
        #else
        /*! @brief コンバイナモジュールを指定します。このモジュールは @ref CombinerStage クラスで利用できます。*/
        #endif
        MODULE_TEXTURE_COMBINER = 1 << 3,

        #if NN_GD_DOCUMENT_TYPE == ENGLISH
        /*! @brief Define the gas and fog module, this module is available in @ref FogStage class. */
        #else
        /*! @brief ガスフォグモジュールを指定します。このモジュールは @ref FogStage クラスで利用できます。*/
        #endif
        MODULE_GAS_FOG = 1 << 4,

        #if NN_GD_DOCUMENT_TYPE == ENGLISH
        /*! @brief Define the texture module, this module is available in @ref TextureStage class. */
        #else
        /*! @brief テクスチャモジュールを指定します。このモジュールは @ref TextureStage クラスで利用できます。*/
        #endif
        MODULE_TEXTURE = 1 << 6,

        #if NN_GD_DOCUMENT_TYPE == ENGLISH
        /*! @brief Define the procedural texture module, this module is available in @ref ProceduralTextureStage class. */
        #else
        /*! @brief プロシージャルテクスチャモジュールを指定します。このモジュールは @ref ProceduralTextureStage クラスで利用できます。*/
        #endif
        MODULE_TEXTURE_PROCEDURAL = 1 << 7,

        #if NN_GD_DOCUMENT_TYPE == ENGLISH
        /*! @brief Define the vertex input module, this module is available in @ref VertexInputStage class. */
        #else
        /*! @brief バーテックス入力モジュールを指定します。このモジュールは @ref VertexInputStage クラスで利用できます。*/
        #endif
        MODULE_VERTEX_INPUT = 1 << 8,

        #if NN_GD_DOCUMENT_TYPE == ENGLISH
        /*! @brief Define the shader module, this module is available in @ref ShaderStage class. */
        #else
        /*! @brief シェーダモジュールを指定します。このモジュールは @ref ShaderStage クラスで利用できます。*/
        #endif
        MODULE_SHADER = 1 << 9,

        #if NN_GD_DOCUMENT_TYPE == ENGLISH
        /*! @brief Define all modules. */
        #else
        /*! @brief 全てのモジュールを指定します。*/
        #endif
        MODULE_ALL = 0xffffffffu
    };

    #if NN_GD_DOCUMENT_TYPE == ENGLISH
    /*! @brief Define how to use the recording packet. */
    #else
    /*! @brief パケット保存を行う時の方法を設定します。 */
    #endif
    enum RecordingPacketUsage
    {
        #if NN_GD_DOCUMENT_TYPE == ENGLISH
        /*! @brief Record and use packet with copies*/
        #else
        /*! @brief  パケットをコピーして記録します。*/
        #endif
        RECORD_COMMAND_LIST_COPY = 1 << 8,

        #if NN_GD_DOCUMENT_TYPE == ENGLISH
        /*! @brief Record and use packet without copy*/
        #else
        /*! @brief パケットをコピーしないで記録します。*/
        #endif
        RECORD_COMMAND_LIST_NO_COPY = 1 << 9,

        #if NN_GD_DOCUMENT_TYPE == ENGLISH
        /*! @brief Record the 3D command buffer and use the packet by doing a jump command*/
        #else
        /*! @brief 3Dコマンドバッファとジャンプコマンドを使用するパケットとして記録します。*/
        #endif
        RECORD_3D_COMMAND_BUFFER_FOR_JUMP = 1 << 10,
        
        RECORD_PACKET_RESERVED_MAX_U32 = 0xffffffffu
    };

    #if NN_GD_DOCUMENT_TYPE == ENGLISH
    /*! @brief Set the callback function.
        @param[in] callbackFunctionError Callback function called when an error occurs.
    */
    #else
    /*! @brief コールバック関数を設定します。
        @param[in] callbackFunctionError エラー発生に呼ばれるコールバック関数を指定します。
    */
    #endif
    static void SetCallbackFunctionError(gdCallbackfunctionErrorPtr callbackFunctionError);

    #if NN_GD_DOCUMENT_TYPE == ENGLISH
    /*! @brief This function gives the error string of a given nnResult.
        @param[in] result Callback the result of which you want to have the error string.
        @return The string of the result in debug or developpement mode. The function always returns NULL in release mode.
        In debug or developpement mode, If the result.IsSuccess() is true, then NULL is returned, otherwise, a valid pointer to an error message is returned.

        @return The result of the execution of the function.
        @retval A valid pointer to a string giving a description of the result parameter.
        @retval NULL    We are in release mode (the function always returns NULL in release mode), or result.IsSuccess() is true.                
    */
    #else
    /*! @brief この関数は任意の @ref nnResultのエラー文字列を獲得します。
        @param[in] result エラー文字列を獲得する戻り値を指定します。

        @return 処理の結果を返します。
        @retval 文字列へのポインタ デバック又は開発モードでは引数のresultが@ref nn::Result::IsSuccess() となる値の場合はNULLが返され、それ以外の場合は当該のエラーメッセージへのポインタを返します
        @retval NULL               リリースモードでは常にNULLを返します。
    */
    #endif
    static char* GetErrorStringFromResult(nnResult result);

    #if NN_GD_DOCUMENT_TYPE == ENGLISH
    /*! @brief Initialize Gd Api, must be called before any other Gd Api call.
        @details A command list must be bound before calling this function.

        @return The result of the execution of the function.
        @retval Result::IsSuccess           The function successfully completed.
        @retval ResultInvalidFunctionCall   Return this error if we call System::Initialize several times in a row (unless we had called System::Finalize before).
        @retval ResultSystemNoCmdListBind   A command list must be bound when initializing Gd Api.
        @retval ResultOutOfMemory           Memory allocation failed.    
    */
    #else
    /*! @brief Gd Apiの初期化を行います。全てのGd Apiを呼ぶ前に必ず実行する必要があります。
        @details この関数を呼ぶ前に、コマンドリストをバインドする必要があります。

        @return 処理の結果を返します。
        @retval Result::IsSuccess           処理に成功しました。
        @retval ResultInvalidFunctionCall   既に初期化されています。
        @retval ResultSystemNoCmdListBind   コマンドリストがバインドされていません。
        @retval ResultOutOfMemory           メモリが獲得できません。
    */
    #endif
    static nnResult Initialize();

    #if NN_GD_DOCUMENT_TYPE == ENGLISH
    /*! @brief Release Gd Api.
        After calling this function, no more Gd commands must be performed.

        @details When calling System::Finalize function, all Gd's objects and states which have not already been released by the user will be released at that moment.
    */
    #else
    /*! @brief Gd Apiの終了処理を行います。
        この関数を呼び出した後は、任意のGd Apiを実行することはできません。

        @details System::Finalize を実行すると、ユーザアプリケーションで解放されなかったGdオブジェクトやステートオブジェクトは全て解放されます。
    */
    #endif
    static void Finalize();

    #if NN_GD_DOCUMENT_TYPE == ENGLISH
    /*! @brief Make some modules dirty. The modules to make dirty are defined by the flag parameter. The flag is a OR combinaison of @ref System::Module enum. 
        @details If a module is dirty, all its internal parameters and states will generate 3D commands during the next draw call command

        @param[in] flag Specifies which module need to be made dirty. Each bit of the flag corresponds to a specific module, as defined in System::Module enum table.
        Value System::MODULE_ALL will make all modules dirty.
    */
    #else
    /*! @brief 指定モジュールのコマンドを再設定状態にします。フラグは @ref System::Module enumを "OR"で指定できます。
        @details 指定のモジュールが再設定状態の時、次に描画コマンドが呼ばれるとそれら全ての内部パラメータやステータスの３Dコマンドが生成されます。

        @param[in] flag 再描画状態にするモジュールを指定します。フラグのそれぞれのビットは指定のモジュールに相当します。（System::Module enum table）
        System::MODULE_ALLを指定すると全てのモジュールが再設定状態になります。
    */
    #endif
    static void ForceDirty(u32 flag);

    #if NN_GD_DOCUMENT_TYPE == ENGLISH
    /*! @brief Reinit some modules with default values. The modules to set as default are defined by the flag parameter. The flag is a OR combinaison of @ref System::Module enum. 

        @param[in] flag Specifies which module need to be reset with its default values. Each bit of the flag corresponds to a specific module, as defined in System::Module enum table.
        Value System::MODULE_ALL will reset all modules with their default values.
    */
    #else
    /*! @brief 指定モジュールを初期値に再設定します。フラグは @ref System::Module enumを "OR"で指定できます。

        @param[in] flag 初期値に再設定するモジュールを指定します。フラグのそれぞれのビットは指定のモジュールに相当します。（System::Module enumテーブル)
        System::MODULE_ALLを指定すると全てのモジュールが初期値に再設定されます。
    */
    #endif
    static void SetDefault(u32 flag);

    #if NN_GD_DOCUMENT_TYPE == ENGLISH
    /*! @brief Flush some dirty modules. The modules to flush are defined by the flag parameter. The flag is a OR combinaison of @ref System::Module enum. 
        @details Normally, if some parameters (or states) from a module are dirty, Gd Api will generate 3D commands for these dirty parameters (or states) during the next draw call command.
        This function allows the user to immediatly flush some dirty modules, it will thus immediately generate 3D commands for these modules.

        @param[in] flag Specifies which module need to be flushed. Each bit of the flag corresponds to a specific module, as defined in System::Module enum table.
        Value System::MODULE_ALL will flush all modules.
    */
    #else
    /*! @brief 再描画状態のモジュールのコマンドを作成します。フラグは @ref System::Module enumを "OR"で指定できます。
        @details 通常はあるパラメータやステートが再描画状態の時、Gd Apiは次の描画コマンドが呼ばれるとそれらの３Dコマンドが生成されます。
        しかしこのApiを実行すると再描画状態のモジュールの３Dコマンドを直ぐに生成します。
        
        @param[in] flag 3Dコマンドを生成するモジュールを指定します。フラグのそれぞれのビットは指定のモジュールに相当します。（System::Module enumテーブル)
        System::MODULE_ALLを指定すると再設定状態の全てのモジュールのコマンドが直ちに生成されます。
    */
    #endif
    static void FlushDirtyModule(u32 flag);

    #if NN_GD_DOCUMENT_TYPE == ENGLISH
    /*! @brief Draw primitives from currently bound vertex.
        @param[in] vertexCount Number of vertices to draw.
        @param[in] startVertexLocation Starting index in the vertex buffers.
    */
    #else
    /*! @brief 現在設定されているバーテックスバッファからプリミティブを描画します。
        @param[in] vertexCount バーテックスの数を指定します。
        @param[in] startVertexLocation バーテックスバッファのスタートインデックスを指定します。
    */
    #endif
    static void Draw(u32 vertexCount, u32 startVertexLocation);

    #if NN_GD_DOCUMENT_TYPE == ENGLISH
    /*! @brief Draw indexed primitives from currently bound vertex buffers and index buffer.
        @param[in] indexCount How many indexes to draw.
        @param[in] startIndexLocation Starting index in the index buffer.
    */
    #else
    /*! @brief 現在設定されているバーテックスバッファ及びインデックスバッファからインデックス付きプリミティブを描画します。
        @param[in] indexCount インデックスバッファのスタートインデックスを指定します。
        @param[in] startIndexLocation インデックスバッファから読み出される各インデックスへのオフセット値を指定します。
    */
    #endif
    static void DrawIndexed(u32 indexCount, u32 startIndexLocation);

    #if NN_GD_DOCUMENT_TYPE == ENGLISH
    /*! @brief Draw primitives without using the vertex buffers resources bound in the VertexInputStage.
        @details The vertex attributes data are directly passed in the function parameters and will be copied into the command buffer.
        This function has the advantage of being more convenient to use from the user side (no vertex buffer resources need to be created and bound)
        Also, in opposition to draw functions using vertex buffers resources, after calling DrawImmediate function we can immediately release or reuse the buffer data arrays.
        The drawback of this function is that it is much slower to use in comparison with draw functions using vertex buffers, and it can also fill up the command buffer very quickly.
        
        Some geometry shader are not compatible with immediate draw (such like subdivision, ...)
        An error will be returned by DrawImmediate function if an incompatible geometry shader is used when calling this function.

        @param[in] vertexCount Number of vertices to draw.
        @param[in] startVertexLocation Starting index in the vertex buffers.
        @param[in] vertexBuffers Points to an array of vertex data to use.
         The number and the type of arrays to use must correspond to the arrays defined by the InputLayout bound in the VertexInputStage.
        @param[in] offsets Points to an array of offsets. Each value define the offset (in byte) by which the data is read within each vertex buffer data.
         If this parameter is set to NULL, a default value 0 will be used as offset for each vertex buffer data.
    */
    #else
    /*! @brief バーテックスバッファリソースを使用しない描画モードをVertexInputStageに設定します。
        @details バーテックスバッファattribute dataは関数のパラメータから渡してコマンドバッファにコピーされます。
        この関数はバーテックスバッファリソースを作成してVertexInputStageに設定する必用がありませんので、使用するのにとても便利です。
        描画実行後バーテックスバッファリソースとは違い、直ぐにバッファデータを解放したりまた再び使用するなどが出来ます。
        しかし、描画実行は遅くなります。

        予約ジオメトリシェーダのシルエット、サブディビジョン、ループサブディビジョンは頂点バッファを使用できない為、
        Immediate Vertex Buffer を設定するとDrawImmediateを実行時にデバッグモードならば ASSERT が発生します。

        @param[in] vertexCount 描画する頂点数を指定します。
        @param[in] startVertexLocation 頂点バッファの開始インデックスを指定します。
        @param[in] vertexBuffers 使用する頂点データの位置配列を指定します。
         VertexInputStageで結びつけられるInputLayoutによって定義される配列と、使用する配列の数とタイプは一致しなければなりません。
        @param[in] offsets 位置へのオフセットの配列を指定します。それぞれのオフセット（バイト値）は、それぞれの頂点バッファデータ中の読み出し位置を指定します。
         もしこの変数にNULLが指定された場合は、それぞれの頂点バッファデータのオフセットとして0が使用されます。
    */
    #endif
    static void DrawImmediate(u32 vertexCount, u32 startVertexLocation, void** vertexBuffers, u32* offsets);

    #if NN_GD_DOCUMENT_TYPE == ENGLISH
    /*! @brief Draw primitives without using the vertex buffers resources bound in the VertexInputStage
        @details Same function as DrawImmediate function, but using an Index buffer array in addition (see @ref DrawImmediate).
        The vertex attributes data are directly passed in the function parameters and will be copied into the command buffer.
        This function has the advantage of being more convenient to use from the user side (no vertex buffer resources need to be created and bound)
        Also, in opposition to draw functions using vertex buffers resources, after calling DrawImmediate function we can immediately release or reuse the buffer data arrays.
        The drawback of this function is that it is much slower to use in comparison with draw functions using vertex buffers, and it can also fill up the command buffer very quickly.
        
        Some geometry shader are not compatible with immediate draw (such like subdivision, ...).
        An error will be returned by DrawImmediateIndexed function if an incompatible geometry shader is used when calling this function.

        @param[in] indexCount Number of indexes to draw.
        @param[in] startIndexLocation Starting index in the index buffer.
         The address of the first index used for drawing is calculated in bytes as:
         'indexBuffer' + 'indexOffset' + 'startIndexLocation' * ('format'==INDEX_FORMAT_UBYTE? 1 : 2)
        @param[in] vertexBuffers Points to an array of vertex attribute data to use.
         The number and the type of arrays to use must correspond to the arrays defined by the InputLayout bound in the VertexInputStage.
        @param[in] offsets Points to an array of offsets. Each value define the offset (in byte) by which the data is read within each vertex buffer data.
         If this parameter is set to NULL, a default value 0 will be used as offset for each vertex buffer data.
        @param[in] indexBuffer index buffer data.
        @param[in] format Format of the index data (see @ref IndexFormat).
        @param[in] indexOffset This is the number of bytes that separate 'index 0' address from the address pointed by 'indexBuffer'.
         In the general case of 'indexBuffer' pointing directly to 'index 0' you simply need to use 0 for 'indexOffset'.
         The address of the first index used for drawing is calculated in bytes as:
         'indexBuffer' + 'indexOffset' + 'startIndexLocation' * ('format'==INDEX_FORMAT_UBYTE? 1 : 2)
    */
    #else
    /*! @brief バーテックスバッファリソースを使用しない描画モードをVertexInputStageに設定します。
        @details インデックスバッファを使用すること以外は DrawImmediate 関数と機能は同じです。( @ref DrawImmediate)。
        バーテックスバッファattribute dataは関数のパラメータから渡してコマンドバッファにコピーされます。
        この関数はバーテックスバッファリソースを作成してVertexInputStageに設定する必用がありませんので、使用するのにとても便利です。
        描画実行後バーテックスバッファリソースとは違い、直ぐにバッファデータを解放したりまた再び使用するなどが出来ます。
        しかし、描画実行は遅くなります。

        予約ジオメトリシェーダのシルエット、サブディビジョン、ループサブディビジョンは頂点バッファを使用できない為、
        Immediate Vertex Buffer を設定するとDrawImmediateを実行時にデバッグモードならば ASSERT が発生します。

        @param[in] indexCount 描画インデックス数を指定します。
        @param[in] startIndexLocation インデックスバッファの開始インデックスを指定します。
         描画に使用される最初のインデックスのアドレスは、次のようにバイト単位で計算されます。
         'indexBuffer' + 'indexOffset' + 'startIndexLocation' * ('format'==INDEX_FORMAT_UBYTE? 1 : 2)
        @param[in] vertexBuffers 使用する頂点アトリビュートの位置配列を指定します。
         VertexInputStageで結びつけられるInputLayoutによって定義される配列と、使用する配列の数とタイプは一致しなければなりません。
        @param[in] offsets 位置へのオフセットの配列を指定します。それぞれのオフセット（バイト値）は、それぞれの頂点バッファデータ中の読み出し位置を指定します。
         もしこの変数にNULLが指定された場合は、それぞれの頂点バッファデータのオフセットとして0が使用されます。
        @param[in] indexBuffer インデックスバッファを指定します。
        @param[in] format インデックスデータのフォーマットを指定します。(@ref IndexFormat).
        @param[in] indexOffset インデックスバッファ配列の使用する開始インデックスのオフセット(バイト値)を指定します。
         'indexBuffer'が直接'index 0'を指すような場合は、'indexOffset'に0を指定します。
         描画に使用される最初のインデックスのアドレスは、次のようにバイト単位で計算されます。
         'indexBuffer' + 'indexOffset' + 'startIndexLocation' * ('format'==INDEX_FORMAT_UBYTE? 1 : 2)
    */
    #endif
    static void DrawImmediateIndexed(u32 indexCount, u32 startIndexLocation, void** vertexBuffers, u32* offsets, void* indexBuffer, nn::gd::CTR::VertexInputStage::IndexFormat format, u32 indexOffset);

    #if NN_GD_DOCUMENT_TYPE == ENGLISH
    /*! @brief Flush the 3D command buffer.
    */
    #else
    /*! @brief 3Dコマンドバッファを実行します。
    */
    #endif
    static void Execute3DCommandList();

    #if NN_GD_DOCUMENT_TYPE == ENGLISH
    /*! @brief Start recording the packets inserted in both the command request list and the 3D command buffer.
        @details We cannot call StartRecordingPackets function if some packets are already being recorded.
        When calling non-immediate functions, it is possible that some instructions called between StartRecordingPackets and StopRecordingPackets do not generate any command.
        (for example if we set a state which is already currently activated, nothing may happen)
        So if we want to make sure that a specific function generates a command in any case, forceDirtyModuleFlag parameter can be used.

        When using recording packets features, immediate functions can be more convenient to use compared to non-immediate functions,
        because with immediate functions we can easily control if the commands generated by these function is to be recorded or not inside the recorded packets.
        As a reminder, Shaders’ uniform variables can be updated using either immediate functions or non-immediate functions
        (we can declare some shader uniform variables as unmanaged then call ShaderStage::SetFloatConstantBuffer immediate function to directly send the new uniform values into the 3D command buffer).

        When we record packet with usage sets to RECORD_3D_COMMAND_BUFFER_FOR_JUMP, Gd Api is only recording the 3D command buffer.
        In this case no request must be inserted in the command request list, otherwise an error will be generated when calling System::StopRecordingPackets function.

        @param[in] forceDirtyModuleFlag Pointer to a flag which specify which modules to be made dirty before we can start recording the packets. 
        If forceDirtyModuleFlag is NULL, no module is made dirty.
        When we call a non-immediate function, the commands will be sent in the command buffer only during the next draw call.
        Depending on the current states of Gd Api, the commands generated by this non-immediate function will not necessary been sent.
        So the flag forceDirtyModuleFlag can be used in this case if the user wants to make sure to send some commands for a spefic modules (defined by the flag)
        StartRecordingPackets function will use this flag to internly call ForceDirty function

        @param[in] RecordingPacketUsage If set to RECORD_COMMAND_LIST_COPY, the packets will be copied into a different buffer.
        After the packets are stored in their own buffer, they can be modified and replayed anytime we want. When replaying the packets, we will copy them back into the command buffer.
        As these copies can be cpu time-consuming, we can set RecordingPacketUsage parameter to RECORD_COMMAND_LIST_NO_COPY in order to directly use references on the packets already stored in the command buffer.
        In such case no copy is necessary, and System::StopRecordingPackets and System::ReplayPackets are much faster.
        Also in such case the packets cannot be modified before being replayed, and after the command buffer is cleared we must not use the packets anymore.
        If set to RECORD_3D_COMMAND_BUFFER_FOR_JUMP, the 3D command buffer will be copied into a different buffer
        So Gd Api can later execute a jump command (nngxAddSubroutineCommand function) into this 3D command buffer when we call System::ReplayPackets function

        @see System::GetCommandBufferOffset
        @see System::GetCommandBufferBaseAddress
        @see System::StopRecordingPackets
        @see System::ReplayPackets
        @see System::ReleasePackets

        @return The result of the execution of the function.
        @retval Result::IsSuccess                           The function successfully completed.
        @retval ResultSystemAPacketIsAlreadyBeingRecorded   A packet is already being recorded.
        @retval ResultOutOfMemory                           Memory allocation failed.
        @retval ResultSystemReceiveErrorFromGlGetError      The execution of nngxStartCmdlistSave function generated an error.
        @retval ResultInvalidParameter                      Some parameters are invalid.
    */
    #else
    /*! @brief 現在のコマンドリストオブジェクトの保存を開始します。forceDirtyModuleFlagフラグは @ref System::Module enumを "OR"で指定できます。
        
        @details 既にほかのパケット保存が開始されている場合は、新たなパケット保存を開始しません。
        実行しようとする関数がイミディエイト関数でない場合、StartRecordingPackets と StopRecordingPacketsの間で実行してもコマンドが
        生成されない可能性があります。
        (例えば、既に設定されているステートを設定しても何も変化しません。）
        実行した関数で必ずコマンドを生成したい場合は、forceDirtyModuleFlag を使用してください。
        
        パケット保存を行う場合、イミディエイト関数が通常の関数よりも便利な場合があります。イミディエイト関数は保存されたパケットの中で
        コマンドを保存するかどうかの制御を行うことができます。これはShaderのuniform変数の設定も同じような機能を備えています。
        ShaderStage::SetFloatConstantBuffer)

        RecordingPacketUsageにRECORD_3D_COMMAND_BUFFER_FOR_JUMPが設定されている場合、Gd Apiは3Dコマンドバッファのみ保存します。
        この場合コマンドリクエストリストには何のリクエストも挿入されません。そうでなければ System::StopRecordingPackets 関数が実行された時にエラーが発生します。
       
        @param[in] forceDirtyModuleFlag 特定のモジュールのコマンドを生成可能状態にするフラグを指定します。NULLを指定した場合はモジュールの状態は変わりません。
        イミディエイト関数でない場合、コマンドは描画コマンドが実行された時にコマンドバッファに生成されます。
        Gd Apiの現在のステータスに応じて、イミディエイトでない関数で生成されたコマンドは、その呼び出しに続いて送られる必要がない場合があります。
        ユーザがフラグによって指定されたモジュールのコマンドを送信したい時など、このforceDirtyModuleFlagを使用することが出来ます。
        StartRecordingPackets関数は、内部でForceDirty関数を呼び出すためにこのフラグを使用します。

        @param[in] usage RECORD_COMMAND_LIST_COPYが指定された場合、コマンドパケットは他のバッファにコピーされます。コマンドパケットがユーザバッファに格納された場合は、
        コマンドパケットを編集し実行することができます。コマンドパケットを実行する場合は、Gd関数が内部で管理しているコマンドバッファ
        にコピーします。この為、コマンドバッファにコピーするのはCPUの時間がかかります。
        
        RECORD_COMMAND_LIST_NO_COPYが指定された場合、このコピーを行わずに、既にGd関数内部のコマンドバッファに格納されているコマンドパケットを使用します。
        この場合、System::StopRecordingPackets と System::ReplayPackets are much faster はコマンドパケットのコピーが無い分早くなります。
        しかし、コマンドパケットを編集して実行するとこが出来ませんし、コマンドバッファがクリアされた後では再使用することも出来ません。
        
        RECORD_3D_COMMAND_BUFFER_FOR_JUMPが指定された場合、Gd内部で管理している3Dコマンドバッファは他のバッファにコピーされます。
        Gd ApiはSystem::ReplayPackets()を実行した時、 nngxAddSubroutineCommand()で挿入されたジャンプコマンドにより、コピーされたコマンドにジャンプして実行します。
 
        @see System::GetCommandBufferOffset
        @see System::GetCommandBufferBaseAddress
        @see System::StopRecordingPackets
        @see System::ReplayPackets
        @see System::ReleasePackets

        @return 処理の結果を返します。
        @retval Result::IsSuccess                         処理に成功しました。
        @retval ResultSystemAPacketIsAlreadyBeingRecorded 既に保存を開始しています。
        @retval ResultOutOfMemory                         メモリを獲得できませんでした。
        @retval ResultSystemReceiveErrorFromGlGetError    nngxStartCmdlistSave()でエラーが発生しました。
        @retval ResultInvalidParameter                    パラメータが不正です。
      */
    #endif
    static nnResult StartRecordingPackets(u32* forceDirtyModuleFlag, RecordingPacketUsage usage = RECORD_COMMAND_LIST_COPY);

    #if NN_GD_DOCUMENT_TYPE == ENGLISH
    /*! @brief Get the current command buffer offset for a packet being recorded.
        @details The offset corresponds to the number of 3D packets (in 32-bits) between the base address and the current command buffer position.
        A packet must be being recorded, in order to get a command buffer offset for this specific packet.
        This offset can then be used later, after the packets has been recorded, in order to make modifications to its command buffer.

        @ref System::GetCommandBufferBaseAddress

        @param[out] offset Will store the value of the current command buffer offset for the packets being recorded.

        @return The result of the execution of the function.
        @retval Result::IsSuccess               The function successfully completed.
        @retval ResultNullParameter             Some parameters have been set with the value NULL.
        @retval ResultSystemNoPacketRecorded    No packet is being recorded.
        @retval ResultSystemInvalidPacketId     The function must be called between System::StartRecordingPackets and System::StopRecordingPackets.
    */
    #else
    /*! @brief コマンドリストオブジェクトの保存が開始されているコマンドバッファの、現在のオフセットを獲得します。
        @details offsetはベースアドレスと現在のコマンドバッファ位置の3Dパケットコマンド(32bit)と一致します。
        指定のパケットから適切にコマンドバッファオフセットを獲得できるようにパケットは保存されていなければなりません。
        
        @ref System::GetCommandBufferBaseAddress

        @param[out] offset 保存されているコマンドリストオブジェクトの現在のコマンドバッファオフセットの値を格納します。
        @see System::StartRecordingPackets

        @return 処理の結果を返します。
        @retval Result::IsSuccess              処理に成功しました。
        @retval ResultNullParameter            引数にNULLが指定されました。
        @retval ResultSystemInvalidPacketId    コマンドリストオブジェクトが正しくないか、コマンドリストオブジェクトの保存が開始されていません。@ref StartRecordingPackets を実行してください。
        @retval ResultSystemNoPacketRecorded   コマンドリストオブジェクトの保存が開始されていません。System::StartRecordingPackets を実行してください。
      */
    #endif
    static nnResult GetCommandBufferOffset(u32* offset);

    #if NN_GD_DOCUMENT_TYPE == ENGLISH
    /*! @brief Get the recorded packet's command buffer base address.
        @details This function is only valid if the recorded packets data have been actually copied into their own buffer
        (if we had called @ref System::StartRecordingPackets with RecordingPacketUsage parameter no equals to RECORD_COMMAND_LIST_NO_COPY).
        The command buffer base address is used in order to make modifications to the recorded packets, before replaying it.
        Function @ref System::GetCommandBufferOffset should be called while recording the packets in order to get a command buffer offset for a specific instruction for which we want to make modifications.
        
        @param[in] packetId The packet for which we want to get the base command buffer address.
        @param[out] commandBufferAddr The address of the command buffer will be stored at this address.
        @see System::StartRecordingPackets
        @see System::GetCommandBufferOffset

        @return The result of the execution of the function.
        @retval Result::IsSuccess               The function successfully completed.
        @retval ResultNullParameter             Some parameters have been set with the value NULL.
        @retval ResultSystemNoPacketRecorded    The packet must have been successfully recorded.
        @retval ResultSystemInvalidPacketId     The usage of the packet must be System::RECORD_COMMAND_LIST_COPY or System::RECORD_3D_COMMAND_BUFFER_FOR_JUMP.
    */
    #else
    /*! @brief 保存されたコマンドバッファのベースアドレスを獲得します。
        @details この関数は保存パケットデータがユーザバッファにコピーされている場合のみ有効です。
        （もし@ref System::StartRecordingPackets が RecordingPacketUsageで実行された場合はRECORD_COMMAND_LIST_NO_COPYではありません。)
        コマンドバッファのベースアドレスは、保存されたコマンドバッファを変更する時に使用されます。@ref System::GetCommandBufferOffset 
        でコマンドリストオブジェクトが保存されている必要があります。
        
        @param[in] packetId  保存されたパケットのオブジェクトを指定します。
        @param[out] commandBufferAddr 保存されているコマンドバッファのベースアドレスの値を格納するアドレスを指定します。  
        @see System::StartRecordingPackets
        @see System::GetCommandBufferOffset

        @return 処理の結果を返します。
        @retval Result::IsSuccess              処理に成功しました。
        @retval ResultNullParameter            引数にNULLが指定されました。
        @retval ResultSystemNoPacketRecorded   コマンドリストオブジェクトが保存されていません。@ref StartRecordingPacketsと@ref StopRecordingPackets を呼び出してください。
        @retval ResultSystemInvalidPacketId    packetIdが正しくありません。 System::StartRecordingPackets で引数のflagに RECORD_COMMAND_LIST_COPY 又は、 
                                               RECORD_3D_COMMAND_BUFFER_FOR_JUMP を指定してください。
      */
    #endif
    static nnResult GetCommandBufferBaseAddress(RecordedPacketId* packetId, u32** commandBufferAddr);

    #if NN_GD_DOCUMENT_TYPE == ENGLISH
    /*! @brief Stop recording the packets.

        @details All packets recorded between @ref System::StartRecordingPackets and System::StopRecordingPackets will be stored in order to be replayed later.
        If we had called @ref System::StartRecordingPackets function with RecordingPacketUsage parameter set at RECORD_COMMAND_LIST_COPY or RECORD_3D_COMMAND_BUFFER_FOR_JUMP,
        the packets will be copied into their own buffer, so then the packets can be modified and replayed anytime we want.
        On the other side, if RecordingPacketUsage parameter was set at RECORD_COMMAND_LIST_NO_COPY, the packet will not be copied, we directly use references on the packets as currently store in the command buffer,
        In such case the packets cannot be modified before being replayed and they will become invalid after we clear the command buffer.

        @param[in] packetId Reference on the packets we finished recording.

        @return The result of the execution of the function.
        @retval Result::IsSuccess               The function successfully completed.
        @retval ResultNullParameter             Some parameters have been set with the value NULL.
        @retval ResultSystemNoPacketRecorded    No packet is being recorded.
        @retval ResultSystemInvalidCmdListBind  The same command list must be bound when we call functions System::StartRecordingPackets and System::StopRecordingPackets.
        @retval ResultSystemReceiveErrorFromGlGetError  The execution of some nngx functions (nngxStopCmdlistSave or nngxExportCmdlist) generated an error.
        @retval ResultSystemNoPacketToRecord    There is no data to record within the packet.
        @retval ResultOutOfMemory               Memory allocation failed.
        @retval ResultSystemRequestListInsertionIncompatibleWithJump  When recording a packet for a jump instruction (using System::RECORD_3D_COMMAND_BUFFER_FOR_JUMP),
                                                no command request (such like P3D, DMA, ...) must have been inserted in the command list between functions System::StartRecordingPackets and System::StopRecordingPackets.
        @retval ResultInvalidParameter          Some parameters are invalid.
    */
    #else
    /*! @brief 現在のコマンドリストオブジェクトの保存を終了します。
        
        @details @ref System::StartRecordingPackets と System::StopRecordingPackets の間で実行された関数のコマンドパケットを後で再利用するために格納されます。
        @ref System::StartRecordingPackets の RecordingPacketUsageパラメータにRECORD_COMMAND_LIST_COPY又はRECORD_3D_COMMAND_BUFFER_FOR_JUMP
        が指定された場合は、コマンドパケットはユーザバッファに格納されます。
        このコマンドパケットは編集することが出来て、必要な時に実行することが出来ます。
        RecordingPacketUsageパラメータにRECORD_COMMAND_LIST_NO_COPYが指定された場合は、コマンドパケットをコピーせずに直接Gd関数内部のコマンドバッファに格納されているパケットを再利用することができます。
        この場合、コマンドパケットを編集して実行するとこが出来ませんし、コマンドバッファがクリアされた後は再利用することが出来なくなります。
        
        @param[in] packetId 取得されたコマンドリストオブジェクトの保存開始IDポインタを格納するアドレスを指定します。

        @return 処理の結果を返します。
        @retval Result::IsSuccess              処理に成功しました。
        @retval ResultNullParameter            packetIdにNULLが指定されました。
        @retval ResultSystemNoPacketRecorded   コマンドリストオブジェクトの保存が開始されていません。System::StartRecordingPackets を呼び出して開始してください。
        @retval ResultSystemInvalidCmdListBind 現在のコマンドリストと保存を開始したコマンドリストが違います。
        @retval ResultSystemReceiveErrorFromGlGetError   nngxStopCmdlistSave()又は、nngxExportCmdlist()が正しく実行できませんでした。
        @retval ResultSystemNoPacketToRecord   保存されたデータはありませんでした。
        @retval ResultOutOfMemory              メモリが獲得できませんでした。
        @retval ResultSystemRequestListInsertionIncompatibleWithJump  （RECORD_3D_COMMAND_BUFFER_FOR_JUMP 指定時のみ）3D実行コマンド以外のコマンドリクエストが含まれているため、ジャンプに利用できません。
        @retval ResultInvalidParameter         パラメータが不正です。
    */
    #endif
    static nnResult StopRecordingPackets(RecordedPacketId** packetId);

    #if NN_GD_DOCUMENT_TYPE == ENGLISH
    /*! @brief Replay the packets identified by packetId parameter.
        @details It is possible that the commands replayed can cause conflicts with Gd internal states.
        If so such states can be made invalidate thanks to forceDirtyModuleFlag (this flag is used to call System::ForceDirty function).
        The safest way to invalidate all state is to use MODULE_ALL flag.
        If you already know which states are put in conflict you can use the flag corresponding to the specific modules.

        If we had called @ref System::StartRecordingPackets function with RecordingPacketUsage parameter set at RECORD_COMMAND_LIST_COPY, the packets was copied into their own buffer,
        and so System::ReplayPackets function will copy them back into the command buffer
        If we wish to spare the cpu with these copies, we could have called @ref System::StartRecordingPackets function with RecordingPacketUsage parameter set at RECORD_COMMAND_LIST_NO_COPY
        No copy will be made in this case, we directly use references on the packets as currently store in the command buffer,
        In such case you must be careful that the packets cannot be modified before being replayed, and also that they will become invalid after we clear the command buffer
        If we had called @ref System::StartRecordingPackets function with RecordingPacketUsage parameter set at RECORD_3D_COMMAND_BUFFER_FOR_JUMP, a jump command (using nngxAddSubroutineCommand function)
        will be added in order to directly jump to the 3D command buffer from the recorded packets.

        @param[in] packetId Reference on the packets recorded we want to replay.
        @param[in] forceDirtyModuleFlag Pointer to a flag which specifies which modules to be made dirty after the packets have been replayed.
        If forceDirtyModuleFlag is NULL, no module is made dirty. Modules are defined using enums from nn::Gd::CTR::System::Module.

        @return The result of the execution of the function.
        @retval Result::IsSuccess               The function successfully completed.
        @retval ResultNullParameter             Some parameters have been set with the value NULL.
        @retval ResultSystemNoPacketRecorded    The packet has not been successfully recorded.
        @retval ResultSystemReceiveErrorFromGlGetError  The execution of some nngx functions (nngxImportCmdlist or nngxUseSavedCmdlist or nngxAddSubroutineCommand) generated an error.
    */
    #else
    /*! @brief packetIdで指定されたコマンドリストオブジェクトを再実行します。forceDirtyModuleFlagフラグは @ref System::Module enumを "OR"で指定できます。
        
        @details コマンドの再実行はGdの内部状態との競合を引き起こす可能性があります。そのような場合は、forceDirtyModuleFlagフラグによってGdの
        内部状態を再設定可能にすることができます。(このフラグは @ref System::ForceDirty 関数を呼びます。) 安全な方法としては、forceDirtyModuleFlag
        にMODULE_ALLを設定して、全てのモジュールを再設定可能状態にします。もし、既にどのモジュールが競合するかわかっている場合は、特定のモジュール
        に対応するフラグを設定することができます。
        
        @ref System::StartRecordingPackets のRecordingPacketUsageパラメータでRECORD_COMMAND_LIST_COPYが指定された場合、
        コマンドパケットはユーザバッファに格納され、System::ReplayPackets 関数でコマンドパケットを実行した時に、Gd内部
        で管理しているコマンドバッファにコピーされます。
        もしこのコピーをしたくない時は、@ref System::StartRecordingPackets のRecordingPacketUsageパラメータで
        RECORD_COMMAND_LIST_NO_COPYを指定します。この場合、Gd内部で管理しているコマンドバッファを再利用することが出来ますが、
        コマンドパケットを直接編集することが出来ないのとコマンドバッファがクリアされた後、再利用することが出来ません。
        @ref System::StartRecordingPackets 関数が、引数RecordingPacketUsageにRECORD_3D_COMMAND_BUFFER_FOR_JUMPが設定されて実行された場合、
        保存されたパケットから直接3Dコマンドバッファへジャンプする為に、nngxAddSubroutineCommand()を使用してジャンプコマンドが追加されます。
       
        @param[in] packetId 実行する保存開始IDポインタを指定します。
        @param[in] forceDirtyModuleFlag パケットが再生された後に指定のモジュールを再設定可能状態にするフラグのポインタです。forceDirtyModuleFlag
        がNULLの場合は、Gdの内部状態は変わりません。モジュールは@ref nn::gd::CTR::System::Module のenum値で定義されています。

        @return 処理の結果を返します。
        @retval Result::IsSuccess                      処理に成功しました。
        @retval ResultNullParameter                    packetIdにNULLが指定されました。
        @retval ResultSystemNoPacketRecorded           指定されたpacketIdのデータが0でした。
        @retval ResultSystemReceiveErrorFromGlGetError nngxImportCmdlist()でエラーが発生しました。
    */
    #endif
    static nnResult ReplayPackets(RecordedPacketId* packetId, u32* forceDirtyModuleFlag);

    #if NN_GD_DOCUMENT_TYPE == ENGLISH
    /*! @brief Copy a recorded packet.
        @details This function is only working if the parameter RecordingPacketUsage was set to RECORD_COMMAND_LIST_COPY or RECORD_3D_COMMAND_BUFFER_FOR_JUMP when creating the packet.
        The memory location cannot be Memory::VRAM when we copy a recorded packet which were created with RECORD_COMMAND_LIST_COPY
        For a recorded packet created with RECORD_3D_COMMAND_BUFFER_FOR_JUMP, memory location can either be Memory::FCRAM or Memory::VRAM
        In case of the memory location is Memory::VRAM, the function will send a dma transfert request in the request list in order to copy the packet data into the VRAM,
        so it will be the user responsability to make sure that the dma transfert has been completed before to release the source recorded packet, and before to use the new recorded packet.

        @param[in] source The recorded packet that we want to copy.
        @param[in] packetId The new recorded packet.
        @param[in] memory The memory location where to store the packetId data

        @return The result of the execution of the function.
        @retval Result::IsSuccess               The function successfully completed.
        @retval ResultNullParameter             Some parameters have been set with the value NULL.
        @retval ResultSystemInvalidPacketId     The usage of the packet must be System::RECORD_COMMAND_LIST_COPY or System::RECORD_3D_COMMAND_BUFFER_FOR_JUMP.
        @retval ResultInvalidMemoryRegion       memory parameter must be either Memory::FCRAM, Memory::VRAMA or Memory::VRAMB.
                                                If the packet usage was System::RECORD_COMMAND_LIST_COPY, the memory region must be Memory::FCRAM.
        @retval ResultOutOfMemory               Memory allocation failed.
    */
    #else
    /*! @brief 保存パケットをコピーします。
        @details この関数は、パケットが作成された時、RecordingPacketUsageにRECORD_COMMAND_LIST_COPYかRECORD_3D_COMMAND_BUFFER_FOR_JUMP
        が指定された場合に動作します。RECORD_COMMAND_LIST_COPYで指定れた保存パケット先は、Memory::VRAMには出来ません。
        RECORD_3D_COMMAND_BUFFER_FOR_JUMP用の保存パケット先は、Memory::FCRAMかMemory::VRAMが指定できます。
        もしMemory::VRAMにパケットが作成された場合、パケットをVRAMにコピーする為のDMA転送リクエストコマンドを生成します。
        この場合、ユーザが新しいパケットを使用する前に、DMA転送が終了してから元の保存パケットを解放するなどの確認が必要になります。
        
        @param[in] source コピーする保存パケットを指定します。
        @param[in] packetId 新しい保存パケットIDを指定します。
        @param[in] memory packetIdデータを保存するメモリロケーションを指定します。

        @return 処理の結果を返します。
        @retval Result::IsSuccess              処理に成功しました。
        @retval ResultNullParameter            引数にNULLが指定されました。
        @retval ResultSystemInvalidPacketId    コマンドリストオブジェクトが正しくないか、コマンドリストオブジェクトの保存が開始されていません。@ref StartRecordingPackets を実行してください。
        @retval ResultInvalidMemoryRegion      不正なメモリロケーションを指定しました。
        @retval ResultOutOfMemory              メモリが獲得できませんでした。
    */
    #endif
    static nnResult CopyPacket(RecordedPacketId* source, RecordedPacketId** packetId, Memory::MemoryLocation memory);


    #if NN_GD_DOCUMENT_TYPE == ENGLISH
    /*! @brief Release the packets recorded.
        @param[in] packetId Reference on the recorded packets we want to release.

        @return The result of the execution of the function.
        @retval Result::IsSuccess           The function successfully completed.
        @retval ResultNullParameter         Some parameters have been set with the value NULL.
    */
    #else
    /*! @brief 保存されたコマンドリストオブジェクトを解放します。
        @param[in] packetId 解放する保存開始IDポインタを指定します。

        @return 処理の結果を返します。
        @retval Result::IsSuccess          処理に成功しました。
        @retval ResultNullParameter        packetIdにNULLが指定されました。
    */
    #endif
    static nnResult ReleasePackets(RecordedPacketId* packetId);

    #if NN_GD_DOCUMENT_TYPE == ENGLISH
    /*! @brief Insert some dummy commands into the command buffer.
        @details The dummy commands will not write any instructions into the hardware registries.
        The use of this function can be either to consume some GPU cycles, or to make sure that the command buffer has a minimum number of data

        @param[in] countDummyCommands Number of dummy commands to insert.
    */
    #else
    /*! @brief コマンドバッファにダミーコマンドを挿入します。
        @details ダミーコマンドはGPUに命令とは認識されません。
        これはGPUに次のコマンドまでWaitさせたり、最少データサイズにコマンドバッファデータをそろえたりします。

        @param[in] countDummyCommands 挿入するダミーコマンドの数を指定します。
    */
    #endif
    static void AddDummyCommands(u32 countDummyCommands);


    /*! @name Debug functions:
      @{
    */
    #if NN_GD_DOCUMENT_TYPE == ENGLISH
    /*!
      @brief Functions useful for debugging purposes
    */
    #else
    /*!
      @brief デバック時に便利な関数です。
    */
    #endif
    class Debug
    {
    public:
        #if NN_GD_DOCUMENT_TYPE == ENGLISH
        /*! @brief Define the filters to be used in System::Debug::SetFilter function. */
        #else
        /*! @brief System::Debug::SetFilter 関数で使用されるフィルタを指定します。 */
        #endif
        enum Filter
        {
            #if NN_GD_DOCUMENT_TYPE == ENGLISH
            /*! @brief This filter does not do anything. It can be used to let the user disable all filters */
            #else
            /*! @brief このフィルタは何もしません。これは、ユーザが全てのフィルタを無効に出来るようにするために使用します。*/
            #endif
            FILTER_NONE = 0,

            #if NN_GD_DOCUMENT_TYPE == ENGLISH
            /*! @brief When using this filter, Gd will set a unique 8X8 texture for all texture units, 
                The 8x8 texture used by this filter is allocated in the FCRAM.
            */
            #else
            /*! @brief このフィルタを使用すると、Gdは全てのテクスチャユニットをユニークな8x8 テクスチャに設定します。
                このフィルタに使用される8x8 テクスチャは、FCRAMに割り当てられます。
            */
            #endif
            FILTER_TEXTURE_8X8 = 1 << 0,

            #if NN_GD_DOCUMENT_TYPE == ENGLISH
            /*! @brief When using this filter, Gd will set the custom texture for all texture units, 
                The custom texture can be defined by the user using function @ref System::debug::SetCustomTexture.
                If the user uses this filter without defining its own custom texture, the 8X8 texture (used by filter @ref System::debug::FILTER_TEXTURE_8X8) will be used instead.
            */
            #else
            /*! @brief このフィルタは、全てのテクスチャユニットにカスタムテクスチャを設定します。
                このカスタムテクスチャは @ref System::debug::SetCustomTexture 使用してユーザが定義することが出来ます。
                もし、カスタムテクスチャを定義しないでこのフィルタを設定した場合は、8x8テクスチャ (@ref System::debug::FILTER_TEXTURE_8X8フィルタで使用される)が代わりに使用されます。
            */
            #endif
            FILTER_TEXTURE_CUSTOM = 1 << 1,

            #if NN_GD_DOCUMENT_TYPE == ENGLISH
            /*! @brief This filter sets Gd to use a sampler state with nearest filter for both the sampling of the mipmap levels and the sampling of the texels.
            */
            #else
            /*! @brief このフィルタは、テクスチャのミップマップレベル間と、テクスチャのフィルタリングを共にポイントサンプリングに設定します。
            */
            #endif
            FILTER_SAMPLER_STATE_FILTER_NEAREST_MIPMAP_NEAREST = 1 << 2,

            #if NN_GD_DOCUMENT_TYPE == ENGLISH
            /*! @brief This filter sets Gd to use a sampler state with nearest filter for the sampling of the mipmap levels and linear filter for the sampling of the texels.
            */
            #else
            /*! @brief このフィルタは、ミップマップレベルのサンプリングの為の最も近いフィルタとテクセルのサンプリングの為の線形フィルタでサンプラステート使用するように、Gdを設定します。
            */
            #endif
            FILTER_SAMPLER_STATE_FILTER_LINEAR_MIPMAP_NEAREST = 1 << 3,

            #if NN_GD_DOCUMENT_TYPE == ENGLISH
            /*! @brief This filter sets Gd to use a 1X1 viewport */
            #else
            /*! @brief このフィルタは、1x1ビューポートに設定します。*/
            #endif
            FILTER_VIEWPORT_1X1 = 1 << 4,

            #if NN_GD_DOCUMENT_TYPE == ENGLISH
            /*! @brief This filter sets Gd to use a simple combiner which output all pixels with a green (0, 255, 0) const color */
            #else
            /*! @brief このフィルタは、緑の固定カラー(0, 255, 0)で全てのピクセルを出力するシンプルなコンバイナ構成にGdを設定します。
            */
            #endif
            FILTER_SIMPLE_COMBINER = 1 << 5,

            #if NN_GD_DOCUMENT_TYPE == ENGLISH
            /*! @brief This filter sets Gd to disable the blending */
            #else
            /*! @brief このフィルタは、ブレンディングを無効に設定します。*/
            #endif
            FILTER_DISABLE_BLENDING = 1 << 6,

            #if NN_GD_DOCUMENT_TYPE == ENGLISH
            /*! @brief This filter sets Gd to disable the depths and stencil test */
            #else
            /*! @brief このフィルタは、デプス、ステンシルテストを無効にします。*/
            #endif
            FILTER_DISABLE_DEPTH_STENCIL_TEST = 1 << 7,

            #if NN_GD_DOCUMENT_TYPE == ENGLISH
            /*! @brief This filter sets Gd to disable the alpha test */
            #else
            /*! @brief このフィルタは、アルファテストを無効にします。*/
            #endif
            FILTER_DISABLE_ALPHA_TEST = 1 << 8,

            #if NN_GD_DOCUMENT_TYPE == ENGLISH
            /*! @brief This filter sets Gd to use 1 light maximum. If several ligths are activated by the user, only the first activated light will be use */
            #else
            /*! @brief このフィルタは、最大ライト数を１に設定します。もし複数のライトが有効な場合は最初に有効になったライトが使用されます。*/
            #endif
            FILTER_LIGHTING_STAGE_1_LIGHT_MAX = 1 << 9,

            #if NN_GD_DOCUMENT_TYPE == ENGLISH
            /*! @brief This filter sets Gd to use a simple layer configuration for the lighting stage.
                The simple layer configuration will make the Lighting computation to use the smaller number of cycles.
            */
            #else
            /*! @brief このフィルタは、サイクル数が最小になるように、ライトステージをシンプルな構成に設定します。*/
            #endif
            FILTER_LIGHTING_STAGE_SIMPLE_LAYER_CONFIGURATION = 1 << 10,

            #if NN_GD_DOCUMENT_TYPE == ENGLISH
            /*! @brief This filter sets Gd to disable the draw call */
            #else
            /*! @brief このフィルタは、描画を行わないように設定します。*/
            #endif
            FILTER_DISABLE_DRAW = 1 << 11,

            #if NN_GD_DOCUMENT_TYPE == ENGLISH
            /*! @brief This filter can be used to display overdraw.
                When using this filter, Gd sets both a Combiner displaying a constant color RGBA = (15,15,15,255), and an additive blend state. The more overdraw there is on a pixel, the more white it becomes.
                This filter can be used in combination with filter FILTER_DISABLE_DEPTH_STENCIL_TEST, depending whether we want to display the overdrawn pixels discarded by the depth depth or not.
            */
            #else
            /*! @brief このフィルタはオーバードローを表示する為に用いることができます。
                このフィルタを使用するとき、Gdは固定カラー RGBA=(15,15,15,255)と追加ブレンドステートの両方を表示するように、コンバイナを設定します。オーバードローが多くなるほど、ピクセルはより白くなります。
                このフィルタは、FILTER_DISABLE_DEPTH_STENCIL_TEST と組み合わせることで、デプステストによって破棄される(されない)オーバードローピクセルを確認することができます。      
            */
            #endif
            FILTER_VISUALIZE_OVERDRAW = 1 << 12,

            #if NN_GD_DOCUMENT_TYPE == ENGLISH
            /*! @brief This filter sets Gd to use no culling. */
            #else
            /*! @brief このフィルタは、カリングを無効にします。*/
            #endif
            FILTER_DISABLE_CULLING = 1 << 13,

            #if NN_GD_DOCUMENT_TYPE == ENGLISH
            /*! @brief This filter sets Gd to use the clockwise culling. */
            #else
            /*! @brief このフィルタは、カリングモードを右回り(clockwise)に設定します。*/
            #endif
            FILTER_ENABLE_CULLING_CLOCKWISE = 1 << 14,

            #if NN_GD_DOCUMENT_TYPE == ENGLISH
            /*! @brief This filter sets Gd to use the counter-clockwise culling. */
            #else
            /*! @brief このフィルタは、カリングモードを左回り(counter-clockwise)に設定します。*/
            #endif
            FILTER_ENABLE_CULLING_COUNTERCLOCKWISE = 1 << 15,

            FILTER_RESERVED_MAX_U32 = 0xffffffffu
        };

        #if NN_GD_DOCUMENT_TYPE == ENGLISH
        /*! @brief Set some filters in order to tell Gd Api to change some states and/or features for the next draw calls. The filterFlag parameter is a OR combinaison of @ref System::Filter enum.
            @details This function is only available in debug or development mode, it has no effect in release mode.
            The purpose of this function is to let user experiment the effect of some states or features on the performance or visual side.
            The states and/or features described by filterFlag parameter will be changed starting from a specified next drawCall and last for some drawCalls, as defined by drawStartIndex and drawCount parameters.
            After the effects are no longer to be changed, Gd Api will automatically restore the states as defined by the user.

            @param[in] filterFlag Flag describing the states or effects to change. This is a OR combinaison of @ref System::Filter enum.
            @param[in] drawStartIndex Define how many draw calls to wait before to activate the filters. If drawStartIndex equals zero, the next draw call will use the filters.
            @param[in] drawCount Define for how many draw calls the filters are to be used.

            @return The result of the execution of the function.
            @retval Result::IsSuccess           The function successfully completed.
        */
        #else
        /*! @brief 次の描画時の幾つかの状態変更を行うフィルタを設定します。filterFlagパラメータは @ref System::Filter enumを "OR"で指定できます。
            @details この関数は、コンパイル時のモードがデバックかデベロップメントの時に使用出来ます。リリースの時は何も行いません。
            この関数は、ユーザがパフォーマンスやビジュアル面のステートや機能の効果をテスト出来るようにしたものです。
            filterFlagパラメータで指定されたステート及び機能は、次の描画実行からdrawStartIndexとdrawCountパラメータで指定された幾つかの描画で変更されます。
            効果が変更されなくなった後は、Gd Apiが自動的にユーザが定義した状態に復元します。
            
            @param[in] filterFlag 変更するステータス又は、効果を指定します。
            @param[in] drawStartIndex 効果を有効にする前に待機する描画実行の数を指定します。drawStartIndexにゼロが指定された場合は次の描画実行から効果が有効になります。
            @param[in] drawCount 効果を有効にする描画実行の数を指定します。

            @return 処理の結果を返します。
            @retval Result::IsSuccess           処理に成功しました。
        */
        #endif
        static nnResult SetFilter(u32 filterFlag, u32 drawStartIndex, s32 drawCount);

        #if NN_GD_DOCUMENT_TYPE == ENGLISH
        /*! @brief Function which colorize some mipmap levels of the resource with a single color
            @details The purpose of this function is to help the user to visualize the mipmap levels of a texture resource.
            This function internally calls @see Memory::ClearTexture2DResource function.
            @param[in] texture2DResource The texture 2D resource used for the creation of the texture 2D.
            @param[in] minMipLevelIndex Index of the minimal mipLevel to fill, must be in [0, texture2DResource->mipLevels[, if the value is -1, the last mipLevel is used.
            @param[in] maxMipLevelIndex Index of the maximal mipLevel to fill, must be in [0, texture2DResource->mipLevels[, if the value is -1, the last mipLevel is used.
            @param[in] colors Array of colors to use to fill the miplevels. The array must contains as many color as the number of miplevel to fill (defined by minMipLevelIndex and maxMipLevelIndex),
            if colors is NULL, a default array of color values will be used.

            @return The result of the execution of the function.
            @retval Result::IsSuccess                   The function successfully completed.
            @retval ResultNullParameter                 Some parameters have been set with value NULL.
            @retval ResultInvalidTextureFormat          format parameter is not valid.
            @retval ResultInvalidTextureMipLevelIndex   minMipLevelIndex is greater than maxMipLevelIndex, or the texture resource is not using the mipmap level specified by maxMipLevelIndex.
            @retval ResultInvalidMemoryRegion           Invalid memory region.
        */
        #else
        /*! @brief リソースの幾つかのミップマップレベルに単色で色付けを行います。
            @details この関数は、ユーザのデバックの為に幾つかのテクスチャのミップマップレベルを視覚化します。
            この関数は内部で @see Memory::ClearTexture2DResource関数を呼びます。
            
            @param[in] texture2DResource 対象の Texture2DResource リソースへのポインタを指定します。
            @param[in] minMipLevelIndex 色付けをする最小ミップマップレベルのインデックスを指定します[ 0, texture2DResource->mipLevels]。 -1 の時は最後のミップマップレベルを使用します。
            @param[in] maxMipLevelIndex 色付けをする最大ミップマップレベルのインデックスを指定します[ 0, texture2DResource->mipLevels]。 -1 の時は最後のミップマップレベルを使用します。
            @param[in] colors ミップマップレベルに色付けする為に使用する色の配列を指定します。
            配列は色付けするミップマップレベルの色のカラー値を指定します。(minMipLevelIndexとmaxMipLevelIndexを指定する)
            もしカラー値がNULLの場合、カラー値の初期値配列を使用します。

            @return 処理の結果を返します。
            @retval Result::IsSuccess                 処理に成功しました。
            @retval ResultNullParameter               指定のリソースにNULLが指定されました。
            @retval ResultInvalidTextureFormat        指定のリソースのフォーマットが @ref Resource::NativeFormat 中のETCとdepthフォーマット除くフォーマット以外のが指定されました。
            @retval ResultInvalidTextureMipLevelIndex mipLevelIndexStartがmipLevelIndexEndより大い値が指定されました。又は、mipLevelIndexEndが指定のtex2DResourceのミップマップレベルより大きい値が指定されました。
            @retval ResultInvalidMemoryRegion         リソースのメモリタイプが正しくありません。指定のリソースはFCRAM領域のメモリです。
        */
        #endif
        static nnResult ColorizeMipmaps(Texture2DResource* texture2DResource, s32 minMipLevelIndex, s32 maxMipLevelIndex, u8 colors[][4]);

        #if NN_GD_DOCUMENT_TYPE == ENGLISH
        /*! @brief Set the custom texture used by filter @ref System::debug::FILTER_TEXTURE_CUSTOM,
            If no custom texture is set (or if customTexture parameter is null), Gd will use the 8X8 texture used by the filter @ref System::debug::FILTER_TEXTURE_8X8.

            @param[in] customTexture The custom texture to use. If null, the 8X8 texture used by the filter @ref System::debug::FILTER_TEXTURE_8X8 will be used.
        */
        #else
        /*! @brief @ref System::debug::FILTER_TEXTURE_CUSTOM フィルタで使用されるカスタムテクスチャを設定します。
            カスタムテクスチャを設定しない場合 (又はカスタムテクスチャがNULLの場合)は、Gdは @ref System::debug::FILTER_TEXTURE_8X8 フィルタで使用されている8x8テクスチャを使用します。

            @param[in] customTexture カスタムテクスチャを指定します。nullの場合は、@ref System::debug::FILTER_TEXTURE_8X8 フィルタで使用されている8x8テクスチャが使用されます。
        */
        #endif
        static void SetCustomTexture(Texture2D* customTexture);
    };
    /*! @} */ // name

};  // System

/*! @} */ // addtogroup systemFunction

}  // namespace CTR
}  // namespace gd
}  // namespace nn


#endif // NN_GD_CTR_GD_SYSTEM_H_
