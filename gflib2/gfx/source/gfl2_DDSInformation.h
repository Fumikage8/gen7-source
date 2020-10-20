
//! @file
//! @brief DDS フォーマット情報取得クラス nv_dds 代替用

#ifndef DDS_INFORMATION_H_INCLUDED
#define DDS_INFORMATION_H_INCLUDED

#if defined(GF_PLATFORM_WIN32)

#include <stdio.h>
#include <memory>
#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_types.h>

#include <gfx/include/gfl2_GLMemory.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>

#define GFL2_NUMBEROF(VARIABLE) (static_cast<s32>(sizeof(VARIABLE)/sizeof((VARIABLE)[0])))

#if defined(GF_PLATFORM_WIN32)
	#define GL_BGRA  0x80E1
	#define GL_BGR   0x80E0
	#define GL_RED   0x1903
	#define GL_GREEN 0x1904
	#define GL_BLUE  0x1905
	#define GL_ALPHA 0x1906
	#define GL_RGB   0x1907
	#define GL_RGBA  0x1908
	
	#define GL_LUMINANCE         0x1909
	#define GL_LUMINANCE_ALPHA   0x190A
	#define GL_RGBA16            0x805B
	#define GL_RGBA16F_ARB       0x881A
	#define GL_RGB16F_ARB        0x881B
	#define GL_LUMINANCE8_ALPHA8 0x8045
	
	#define GL_COMPRESSED_ALPHA_ARB               0x84E9
	#define GL_COMPRESSED_LUMINANCE_ARB           0x84EA
	#define GL_COMPRESSED_LUMINANCE_ALPHA_ARB     0x84EB
	#define GL_COMPRESSED_s32ENSITY_ARB           0x84EC
	#define GL_COMPRESSED_RGB_ARB                 0x84ED
	#define GL_COMPRESSED_RGBA_ARB                0x84EE
	#define GL_TEXTURE_COMPRESSION_Hs32_ARB       0x84EF
	#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB  0x86A0
	#define GL_TEXTURE_COMPRESSED_ARB             0x86A1
	#define GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB 0x86A2
	#define GL_COMPRESSED_TEXTURE_FORMATS_ARB     0x86A3
	
	#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT  0x83F0
	#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
	#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
	#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
	
	typedef u32 GLenum;
	
	#define GL_TEXTURE_1D 0x0DE0
	#define GL_TEXTURE_2D 0x0DE1
#endif

#define GL_RGBA4444 0x8820

namespace gfl2 { namespace gfx { namespace DDSInformation {
	
	const u32 DDS_FOURCC            = 0x00000004;
	const u32 DDS_RGB               = 0x00000040;
	const u32 DDS_RGBA              = 0x00000041;
	const u32 DDS_DEPTH             = 0x00800000;
	const u32 DDS_ALPHAPIXELS       = 0x00000001; // DDPF_ALPHAPIXELS
	const u32 DDS_ALPHA             = 0x00000002; // DDPF_ALPHA
	const u32 DDS_PALETTEINDEXED4   = 0x00000008; // DDPF_PALETTEINDEXED4
	const u32 DDS_PALETTEINDEXEDTO8 = 0x00000010; // DDPF_PALETTEINDEXEDTO8
	const u32 DDS_PALETTEINDEXED8   = 0x00000020; // DDPF_PALETTEINDEXED8
	const u32 DDS_COMPRESSED        = 0x00000080; // DDPF_COMPRESSED
	const u32 DDS_RGBTOYUV          = 0x00000100; // DDPF_RGBTOYUV
	const u32 DDS_YUV               = 0x00000200; // DDPF_YUV
	const u32 DDS_ZBUFFER           = 0x00000400; // DDPF_ZBUFFER
	const u32 DDS_PALETTEINDEXED1   = 0x00000800; // DDPF_PALETTEINDEXED1
	const u32 DDS_PALETTEINDEXED2   = 0x00001000; // DDPF_PALETTEINDEXED2
	const u32 DDS_ZPIXELS           = 0x00002000; // DDPF_ZPIXELS
	const u32 DDS_STENCILBUFFER     = 0x00004000; // DDPF_STENCILBUFFER
	const u32 DDS_ALPHAPREMULT      = 0x00008000; // DDPF_ALPHAPREMULT
	const u32 DDS_LUMINANCE         = 0x00020000; // DDPF_LUMINANCE
	const u32 DDS_LUMINANCE_ALPHA   = 0x00020001; // DDPF_LUMINANCE | DDPF_ALPHAPIXELS
	const u32 DDS_BUMPLUMINANCE     = 0x00040000; // DDPF_BUMPLUMINANCE
	const u32 DDS_BUMPDUDV          = 0x00080000; // DDPF_BUMPDUDV
	
	const u32 DDS_COMPLEX = 0x00000008;
	const u32 DDS_CUBEMAP = 0x00000200;
	const u32 DDS_VOLUME  = 0x00200000;
	
	const u32 FOURCC_DXT1 = 0x31545844; //(MAKEFOURCC('D','X','T','1'))
	const u32 FOURCC_DXT3 = 0x33545844; //(MAKEFOURCC('D','X','T','3'))
	const u32 FOURCC_DXT5 = 0x35545844; //(MAKEFOURCC('D','X','T','5'))
	
	const u32 FOURCC_ABGR16F = 0x00000071;
	
// ----------------------------------------------------------------------------
	
	struct DDSPixelFormat{
		u32 dwSize;
		u32 dwFlags;
		u32 dwFourCC;
		u32 dwRGBBitCount;
		u32 dwRBitMask;
		u32 dwGBitMask;
		u32 dwBBitMask;
		u32 dwABitMask;
	};
	
	struct DXTColorBlock{
		u16 color0;
		u16 color1;
		
		unsigned char row[4];
	};
	
	struct DXT3AlphaBlock{
		u16 row[4];
	};
	
	struct DXT5AlphaBlock{
		unsigned char alpha0;
		unsigned char alpha1;
		
		unsigned char row[6];
	};
	
	struct DDSHeader{
		u32 dwSize;
		u32 dwFlags;
		u32 dwHeight;
		u32 dwWidth;
		u32 dwPitchOrLinearSize;
		u32 dwDepth;
		u32 dwMipMapCount;
		u32 dwReserved1[11];
		DDSPixelFormat ddspf;
		u32 dwCaps1;
		u32 dwCaps2;
		u32 dwReserved2[3];
	};
	
// ----------------------------------------------------------------------------
	
	class MemorySource{
	public:
		MemorySource(const void* source):
			source(source),
			index(0){
			GFL_ASSERT(source != NULL);
		}
		
		void Forward(size_t size){
			this->index += size;
		}
		
		void* GetCurrent(){
			return (void*) &((unsigned char*) source)[this->index];
		}
		
	private:
		const void* source;
		size_t index;
	};
	
// ----------------------------------------------------------------------------
	
	class Surface : GLMemory{
		friend class Texture;
		friend class Information;
		
	public:
		Surface();
		Surface(size_t width, size_t height, size_t depth, size_t size);
		Surface(const Surface& source);
		virtual ~Surface();
		
		Surface& operator =(const Surface &source);
		
		void Instance(void* source);
		void Substance(void* source);
		
		inline void*  GetPixel() { return ((this->instance != NULL) ? this->instance : this->substance); }
		inline size_t GetWidth() { return this->width;  }
		inline size_t GetHeight(){ return this->height; }
		inline size_t GetDepth() { return this->depth;  }
		inline size_t GetSize()  { return this->size;   }
		
	private:
		void Clear();
		void Copy(const Surface& source);
		
	protected:
		size_t width;
		size_t height;
		size_t depth;
		size_t size;
		
		void* instance;  // 内部保持
		void* substance; // 外部参照
	};
	
// ----------------------------------------------------------------------------
	
	class Texture{
		friend class Information;
		public:
			Texture();
			Texture(const Texture& source);
			~Texture();
			
			Texture& operator =(const Texture& source);
			
			inline void* GetPixel(){
				GFL_ASSERT(this->surfaceContainerIndex > 0);
				return this->surfaceContainer[0].GetPixel();
			}
			
			inline size_t GetWidth(){
				GFL_ASSERT(this->surfaceContainerIndex > 0);
				return this->surfaceContainer[0].GetWidth();
			}
			
			inline size_t GetHeight(){
				GFL_ASSERT(this->surfaceContainerIndex > 0);
				return this->surfaceContainer[0].GetHeight();
			}
			
			inline size_t GetDepth(){
				GFL_ASSERT(this->surfaceContainerIndex > 0);
				return this->surfaceContainer[0].GetDepth();
			}
			
			inline size_t GetSize(){
				GFL_ASSERT(this->surfaceContainerIndex > 0);
				return this->surfaceContainer[0].GetSize();
			}
			
			inline Surface& GetSurface(){
				GFL_ASSERT(this->surfaceContainerIndex > 0);
				return this->surfaceContainer[0];
			}
			
			inline size_t GetNumberOfMipmap() const{
				return this->surfaceContainerIndex;
			}
			
			inline Surface& GetMipmap(size_t index){
				GFL_ASSERT(index < this->surfaceContainerIndex);
				return this->surfaceContainer[index];
			}
			
		private:
			inline void AddMipmap(Surface* surface){
				GFL_ASSERT(this->surfaceContainerIndex < GFL2_NUMBEROF(this->surfaceContainer));
				this->surfaceContainer[this->surfaceContainerIndex++] = *surface;
			}
			
		protected:
			Surface surfaceContainer[13]; // Max : 4096x4096 pixel
			size_t surfaceContainerIndex;
	};
	
// ----------------------------------------------------------------------------
	
	class Information : public GLMemory
	{
	public:
		Information();
		Information(const void* source, bool swapRestras32 = false);
		virtual ~Information();
		
		void SetSource( const void* source_, bool swapRestras32_ = false );
	public:
		
		inline void* GetPixel(){
			GFL_ASSERT(this->textureContainerIndex > 0);
			return this->textureContainer[0].GetPixel();
		}
		
		inline size_t GetWidth(){
			GFL_ASSERT(this->textureContainerIndex > 0);
			return this->textureContainer[0].GetWidth();
		}
		
		inline size_t GetHeight(){
			GFL_ASSERT(this->textureContainerIndex > 0);
			return this->textureContainer[0].GetHeight();
		}
		
		inline size_t GetDepth(){
			GFL_ASSERT(this->textureContainerIndex > 0);
			return this->textureContainer[0].GetDepth();
		}
		
		inline size_t GetSize(){
			GFL_ASSERT(this->textureContainerIndex > 0);
			return this->textureContainer[0].GetSize();
		}
		
		inline size_t GetNumberOfMipmap(){
			GFL_ASSERT(this->textureContainerIndex > 0);
			return this->textureContainer[0].GetNumberOfMipmap();
		}
		
		inline Surface& GetMipmap(size_t index){
			GFL_ASSERT(this->textureContainerIndex > 0);
			return this->textureContainer[0].GetMipmap(index);
		}
		
		inline size_t GetComponents(){ return components; }
		inline size_t GetFormat()    { return format;     }
		
		inline bool IsCompressed(){ return compressed; }
		inline bool IsCubemap()   { return cubemap;    }
		inline bool IsVolume()    { return volume;     }
		inline bool IsValid()     { return valid;      }
		
		inline Texture& GetTexture(size_t index = 0){
			GFL_ASSERT(index < this->textureContainerIndex);
			return this->textureContainer[index];
		}
		
	private:
		bool Initialize();
		
		bool IsDDS(void* source);
		
		inline void AddTexture(Texture* texture){
			GFL_ASSERT(this->textureContainerIndex < GFL2_NUMBEROF(this->textureContainer));
			this->textureContainer[this->textureContainerIndex++] = *texture;
		}
		
		size_t ClampSize(size_t size);
		size_t GetLineWidth(size_t width, size_t bpp);
		size_t SizeDXTC(size_t width, size_t height);
		size_t SizeRGB(size_t width, size_t height);
		
		bool IsNecessaryAlignMemory(size_t width, size_t height, size_t size);
		void AlignMemory(Surface& target);
		
		bool IsNecessarySwapEndianImage(size_t rgbBitCount);
		void SwapEndianImage(void* target, size_t width, size_t height, size_t depth, size_t size);
		
		void Flip(void* target, size_t width, size_t height, size_t depth, size_t size);
		
		inline void* GetSource(){ return &((unsigned char*) this->source)[this->sourceIndex]; }
		inline void ForwardSource(size_t size){ this->sourceIndex += size; }
		inline void LoadSource(void* destination, size_t size){
			::memcpy(destination, this->GetSource(), size);
			this->ForwardSource(size);
		}
		
	private:
		static void SwapEndian32(gfl2::gfx::IGLAllocator* pAllocator, void* source);
		static void SwapEndian16(gfl2::gfx::IGLAllocator* pAllocator, void* source);
		static void Swap(gfl2::gfx::IGLAllocator* pAllocator, void* left, void* right, size_t size);
		static void FlipBlocksDXTC1(gfl2::gfx::IGLAllocator* pAllocator, DXTColorBlock* line, size_t numberBlocks);
		static void FlipBlocksDXTC3(gfl2::gfx::IGLAllocator* pAllocator, DXTColorBlock* line, size_t numberBlocks);
		static void FlipBlocksDXTC5(gfl2::gfx::IGLAllocator* pAllocator, DXTColorBlock* line, size_t numberBlocks);
		static void FlipDXTC5Alpha(gfl2::gfx::IGLAllocator* pAllocator, DXT5AlphaBlock* block);
		
	private:
		bool swapRestras32;
		bool valid;
		
		size_t format;
		size_t components;
		bool compressed;
		bool cubemap;
		bool volume;
		
		const void* source;
		size_t sourceIndex;
		
		Texture textureContainer[6]; // Max : CubeMap
		size_t textureContainerIndex;
	};
	
}}}

#endif // #if defined(GF_PLATFORM_WIN32)
#endif // #ifndef DDS_INFORMATION_H_INCLUDED
