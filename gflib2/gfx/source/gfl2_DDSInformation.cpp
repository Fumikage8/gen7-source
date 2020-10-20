#if defined(GF_PLATFORM_WIN32)

#include <stdio.h>

#include "gfl2_DDSInformation.h"
#include <gfx/include/gfl2_GLMemory.h>

#define DDS_Y_FLIP(raw) (((raw&0x03)<<6)+((raw&0x0c)<<2)+((raw&0x30)>>2)+((raw&0xc0)>>6))

namespace gfl2 { namespace gfx { namespace DDSInformation{
	
	Information::Information():
		GLMemory(),
		valid                (false),
		format               (0),
		components           (0),
		compressed           (false),
		cubemap              (false),
		volume               (false),
		sourceIndex          (0),
		textureContainerIndex(0)
	{
		
	}

	Information::Information(const void* source, bool swapRestras32):
		GLMemory(),
		valid                (false),
		format               (0),
		components           (0),
		compressed           (false),
		cubemap              (false),
		volume               (false),
		sourceIndex          (0),
		textureContainerIndex(0)
	{
		this->SetSource( source, swapRestras32 );
	}

	void Information::SetSource( const void* source_, bool swapRestras32_ )
	{
		swapRestras32 = swapRestras32_;
		source = source_;
		this->Initialize();
	}
	
	Information::~Information(){
	}
	
	bool Information::Initialize(){
		char descriptor[4];
		this->LoadSource(descriptor, sizeof(descriptor));
		
		if(!this->IsDDS(descriptor))
			return false;
		
		DDSHeader ddsHeader;
		this->LoadSource(&ddsHeader, sizeof(ddsHeader));
		
		if(!this->swapRestras32){
			this->SwapEndian32(this->GetInstanceAllocator(), &ddsHeader.dwSize);
			this->SwapEndian32(this->GetInstanceAllocator(), &ddsHeader.dwFlags);
			this->SwapEndian32(this->GetInstanceAllocator(), &ddsHeader.dwHeight);
			this->SwapEndian32(this->GetInstanceAllocator(), &ddsHeader.dwWidth);
			this->SwapEndian32(this->GetInstanceAllocator(), &ddsHeader.dwPitchOrLinearSize);
			this->SwapEndian32(this->GetInstanceAllocator(), &ddsHeader.dwDepth);
			this->SwapEndian32(this->GetInstanceAllocator(), &ddsHeader.dwMipMapCount);
			this->SwapEndian32(this->GetInstanceAllocator(), &ddsHeader.ddspf.dwSize);
			this->SwapEndian32(this->GetInstanceAllocator(), &ddsHeader.ddspf.dwFlags);
			this->SwapEndian32(this->GetInstanceAllocator(), &ddsHeader.ddspf.dwFourCC);
			this->SwapEndian32(this->GetInstanceAllocator(), &ddsHeader.ddspf.dwRGBBitCount);
			this->SwapEndian32(this->GetInstanceAllocator(), &ddsHeader.dwCaps1);
			this->SwapEndian32(this->GetInstanceAllocator(), &ddsHeader.dwCaps2);
		}
		
		if(ddsHeader.ddspf.dwFlags & DDS_FOURCC){
			switch(ddsHeader.ddspf.dwFourCC){
				case FOURCC_DXT1:{
					this->format     = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
					this->components = 3;
					this->compressed = true;
					break;
				}
				case FOURCC_DXT3:{
					this->format     = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
					this->components = 4;
					this->compressed = true;
					break;
				}
				case FOURCC_DXT5:{
					this->format     = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
					this->components = 4;
					this->compressed = true;
					break;
				}
				case FOURCC_ABGR16F:{
					this->format     = GL_RGBA;
					this->components = GL_RGBA16F_ARB;
					this->compressed = false;
					break;
				}
				default:{
					return false;
					break;
				}
			}
		}
		else
		if((ddsHeader.ddspf.dwFlags       == DDS_RGBA)
		&& (ddsHeader.ddspf.dwRGBBitCount == 32)){
			this->format     = GL_BGRA;
			this->compressed = false;
			this->components = 4;
		}
		else
		if((ddsHeader.ddspf.dwFlags       == DDS_RGB)
		&& (ddsHeader.ddspf.dwRGBBitCount == 32)){
			this->format     = GL_BGRA;
			this->compressed = false;
			this->components = 4;
		}
		else
		if((ddsHeader.ddspf.dwFlags       == DDS_RGB)
		&& (ddsHeader.ddspf.dwRGBBitCount == 24)){
			this->format     = GL_BGR;
			this->compressed = false;
			this->components = 3;
		}
		else
		if((ddsHeader.ddspf.dwFlags       == DDS_RGBA)
		&& (ddsHeader.ddspf.dwRGBBitCount == 16)){
			this->format     = GL_RGBA16;
			this->compressed = false;
			this->components = GL_RGBA4444;
		}
		else
		if((ddsHeader.ddspf.dwRGBBitCount == 16) &&
#if defined(GFL2_LITTLE_ENDIAN)
			(ddsHeader.ddspf.dwRBitMask == 0x000000ff) && (ddsHeader.ddspf.dwGBitMask == 0x0000ff00)
#else
			(ddsHeader.ddspf.dwRBitMask == 0xff000000) && (ddsHeader.ddspf.dwGBitMask == 0x00ff0000)
#endif
		){
			this->format     = GL_LUMINANCE8_ALPHA8;
			this->compressed = false;
			this->components = 2;
		}
		else
		if((ddsHeader.ddspf.dwFlags       == DDS_LUMINANCE_ALPHA)
		&& (ddsHeader.ddspf.dwRGBBitCount == 16)){
			this->format     = GL_LUMINANCE8_ALPHA8;
			this->compressed = false;
			this->components = 2;
		}
		else
		if(ddsHeader.ddspf.dwRGBBitCount == 8){
			this->format     = GL_LUMINANCE;
			this->compressed = false;
			this->components = 1;
		}
		else{
			return false;
		}
		
		if(ddsHeader.dwCaps2 & DDS_CUBEMAP){
			this->cubemap = true;
		}
		
		if((ddsHeader.dwCaps2 & DDS_VOLUME)
		&& (ddsHeader.dwDepth > 0)){
			this->volume = true;
		}
		
		const size_t width  = ddsHeader.dwWidth;
		const size_t height = ddsHeader.dwHeight;
		const size_t depth  = this->ClampSize(ddsHeader.dwDepth);
		
		const size_t numberMipmap = (ddsHeader.dwMipMapCount == 0) ? 1 : ddsHeader.dwMipMapCount;
		
		size_t (Information::*sizeFunction)(size_t, size_t) = (this->compressed ? &Information::SizeDXTC : &Information::SizeRGB);
		
		const size_t size = (this->*sizeFunction)(width, height) * depth;
		
		const bool necessaryAlignMemory     = this->IsNecessaryAlignMemory(width, height, size);
		const bool necessarySwapEndianImage = this->IsNecessarySwapEndianImage(ddsHeader.ddspf.dwRGBBitCount);
		
		for(size_t n = 0; n < (size_t) ((this->cubemap) ? 6 : 1); n++){
			Texture texture;
			
			size_t w = width;
			size_t h = height;
			size_t d = depth;
			
			for(size_t i = 0; (i < numberMipmap) && ((w != 0) || (h != 0)); i++){
				const size_t s = (this->*sizeFunction)(w, h) * d;
				
				Surface surface(w, h, d, s);
				
				if(((i == 0) && necessaryAlignMemory) || (necessarySwapEndianImage)){
					surface.Instance(this->GetSource());
					this->ForwardSource(surface.GetSize());
				}
				else{
					surface.Substance(this->GetSource());
					this->ForwardSource(surface.GetSize());
				}
				
				if((i == 0) && necessaryAlignMemory)
					this->AlignMemory(surface);
				
				if(necessarySwapEndianImage)
					this->SwapEndianImage(surface.GetPixel(), surface.GetWidth(), surface.GetHeight(), surface.GetDepth(), surface.GetSize());
				
//				this->Flip(surface.GetPixel(), surface.GetWidth(), surface.GetHeight(), surface.GetDepth(), surface.GetSize());
				
				texture.AddMipmap(&surface);
				
				w = this->ClampSize(w >> 1);
				h = this->ClampSize(h >> 1);
				d = this->ClampSize(d >> 1);
			}
			
			this->AddTexture(&texture);
		}
		
//		if(this->cubemap){
//			Texture texture;
//			texture                   = this->textureContainer[3];
//			this->textureContainer[3] = this->textureContainer[2];
//			this->textureContainer[2] = texture;
//		}
		
		this->valid = true;
		
		return true;
	}
	
	bool Information::IsDDS(void* source){
		return (::strncmp((char*) source, "DDS ", 4) == 0);
	}
	
	size_t Information::ClampSize(size_t size){
		if(size <= 0)
			size = 1;
		return size;
	}
	
	size_t Information::GetLineWidth(size_t width, size_t bpp){
		return ((width * bpp + 31) & -32) >> 3;
	}
	
	size_t Information::SizeDXTC(size_t width, size_t height){
		return ((width + 3) / 4) * ((height + 3) / 4) * ((format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16);
	}
	
	size_t Information::SizeRGB(size_t width, size_t height){
		size_t size = 0;
		if(this->components <= 4){
			size = width * height * this->components;
		}
		else{
			switch(this->components){
				case GL_RGBA16F_ARB:{
					size = width * height * 8;
					break;
				}
				case GL_RGBA4444:{
					size = width * height * 2;
					break;
				}
				default:{
					break;
				}
			}
		}
		return size;
	}
	
	bool Information::IsNecessaryAlignMemory(size_t width, size_t height, size_t size){
		bool result = false;
		do{
			if(this->compressed || this->volume || this->cubemap || (this->components > 4))
				break;
			
			size_t lineSize  = this->GetLineWidth(width, this->components * 8);
			size_t imageSize = lineSize * height;
			
			if(size == imageSize)
				break;
			
			result = true;
		} ONCE;
		
		return result;
	}
	
	void Information::AlignMemory(Surface& target){
		size_t  destinationLineSize = this->GetLineWidth(target.GetWidth(), this->components * 8);
		size_t destinationImageSize = destinationLineSize * target.GetHeight();
		
		Surface temporary(target.GetWidth(), target.GetHeight(), target.GetDepth(), destinationImageSize);
		
		char*      sourceImage = (char*)    target.GetPixel();
		char* destinationImage = (char*) temporary.GetPixel();
		
		for(size_t n = 0; n < target.GetDepth(); n++){
			char*      sourceLine =      sourceImage;
			char* destinationLine = destinationImage;
			
			size_t sourceImageSize = target.GetSize() / target.GetDepth();
			size_t sourceLineSize  = sourceImageSize / target.GetHeight();
			
			for(size_t i = 0; i < target.GetHeight(); i++){
				::memcpy(destinationLine, sourceLine, sourceLineSize);
				destinationLine += destinationLineSize;
				     sourceLine +=      sourceLineSize;
			}
		}
		
		target = temporary;
	}
	
	bool Information::IsNecessarySwapEndianImage(size_t rgbBitCount){
		(void)rgbBitCount;
		bool result = false;
#if defined(GFL2_PS3) || defined(GFL2_X360) || defined(GFL2_WII)
			do{
				if(this->swapRestras32)
					break;
				if(rgbBitCount == 24)
					break;
	#if defined(GFL2_WII)
				if(rgbBitCount ==  8)
					break;
	#endif

	#if defined(GFL2_PS3)
				if(this->compressed)
					break;
	#endif
				result = true;
			}
			while(false);
#endif
		return result;
	}
	
	void Information::SwapEndianImage(void* target, size_t width, size_t height, size_t depth, size_t size){
		char* image = (char*) target;
		
		if(!this->compressed){
			size_t swapSize = size;
			while(swapSize >= 2){
				if((swapSize >= 4) && (this->format != GL_RGBA16) && (this->format != GL_LUMINANCE8_ALPHA8)){
					this->SwapEndian32(this->GetInstanceAllocator(), &image[size - swapSize]);
					swapSize -= 4;
				}
				else{
					this->SwapEndian16(this->GetInstanceAllocator(), &image[size - swapSize]);
					swapSize -= 2;
				}
			}
		}
#if defined(GFL2_X360) || defined(GFL2_WII)
		else{
			u16 s32* alpha;
			
			size_t remainSize = size;
			DXTColorBlock *block = (DXTColorBlock*) image;
			
			while(remainSize){
				// DXT alpha block
				if((format == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT)
				|| (format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT)){
					alpha = (u16 s32*) block;
					
					this->SwapEndian16(&alpha[0]);
					this->SwapEndian16(&alpha[1]);
					this->SwapEndian16(&alpha[2]);
					this->SwapEndian16(&alpha[3]);
					
					block++;
					
					if(format == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT){
						remainSize -= sizeof(DXT3AlphaBlock);
					}
					else
					if(format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT){
						remainSize -= sizeof(DXT5AlphaBlock);
					}
				}
				
				// DXT Color block
				this->SwapEndian16(&block->color0);
				this->SwapEndian16(&block->color1);
				
#if defined( GFL2_X360 )
				{
					unsigned char swapRow[4];
					swapRow[0] = block->row[0];
					swapRow[1] = block->row[1];
					swapRow[2] = block->row[2];
					swapRow[3] = block->row[3];
					
					block->row[0] = swapRow[1];
					block->row[1] = swapRow[0];
					block->row[2] = swapRow[3];
					block->row[3] = swapRow[2];
				}
#elif defined( GFL2_WII )
				{
					block->row[0] = DDS_Y_FLIP(block->row[0]);
					block->row[1] = DDS_Y_FLIP(block->row[1]);
					block->row[2] = DDS_Y_FLIP(block->row[2]);
					block->row[3] = DDS_Y_FLIP(block->row[3]);
				}
#endif
				block++;
				remainSize -= sizeof(DXTColorBlock);
			}
		}
#endif
		(void) width;
		(void) height;
		(void) depth;
	}
	
	void Information::Flip(void* target, size_t width, size_t height, size_t depth, size_t size){
		char* image = (char*) target;
		
		if(!this->compressed){
			GFL_ASSERT(depth > 0);
			
			size_t imageSize = size / depth;
			size_t  lineSize = imageSize / height;
			
			for(size_t n = 0; n < depth; n++){
				size_t offset   = imageSize * n;
				char *top    = image + offset;
				char *bottom = top + (imageSize - lineSize);
				
				for(size_t i = 0; i < (height >> 1); i++){
					Information::Swap(this->GetInstanceAllocator(), bottom, top, lineSize);
					
					top    += lineSize;
					bottom -= lineSize;
				}
			}
		}
		else{
			void (*flipBlocks)(IGLAllocator*, DXTColorBlock*, size_t);
			
			size_t xBlocks = width  / 4;
			size_t yBlocks = height / 4;
			size_t blockSize;
			
			switch(format){
				case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:{
					blockSize  = 8;
					flipBlocks = &Information::FlipBlocksDXTC1;
					break;
				}
				case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:{
					blockSize  = 16;
					flipBlocks = &Information::FlipBlocksDXTC3;
					break;
				}
				case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:{
					blockSize  = 16;
					flipBlocks = &Information::FlipBlocksDXTC5;
					break;
				}
				default:{
					return;
					break;
				}
			}
			
			size_t lineSize = xBlocks * blockSize;
			
			DXTColorBlock *top;
			DXTColorBlock *bottom;
			
			for(size_t j = 0; j < (yBlocks >> 1); j++){
				top    = (DXTColorBlock*)(image + j                * lineSize);
				bottom = (DXTColorBlock*)(image + (((yBlocks-j)-1) * lineSize));
				
				(*flipBlocks)(this->GetInstanceAllocator(), top,    xBlocks);
				(*flipBlocks)(this->GetInstanceAllocator(), bottom, xBlocks);
				
				Information::Swap(this->GetInstanceAllocator(), bottom, top, lineSize);
			}
		}
	}
	
// ----------------------------------------------------------------------------
	
	void Information::SwapEndian32(gfl2::gfx::IGLAllocator* pAllocator, void* source){
		(void)source;
#if defined( GFL2_BIG_ENDIAN )
		size_t* destination = (size_t*) source;
		*destination =
			  ((*destination >> 24) & 0x000000ff)
			| ((*destination >>  8) & 0x0000ff00)
			| ((*destination <<  8) & 0x00ff0000)
			| ((*destination << 24) & 0xff000000);
#endif
	}
	
	void Information::SwapEndian16(gfl2::gfx::IGLAllocator* pAllocator, void* source){
		(void)source;
#if defined(GFL2_BIG_ENDIAN)
		u16* destination = (u16*) source;
		*destination =
			  ((*destination >> 8) & 0x00ff)
			| ((*destination << 8) & 0xff00);
#endif
	}
	
	void Information::Swap(gfl2::gfx::IGLAllocator* pAllocator, void* left, void* right, size_t size){
		void* tmporary = pAllocator->SystemMemoryMalloc(static_cast<u32>(size));
		
		::memcpy(tmporary, left,     size);
		::memcpy(left,     right,    size);
		::memcpy(right,    tmporary, size);
		
		pAllocator->SystemMemoryFree(tmporary);
	}
	
	void Information::FlipBlocksDXTC1(gfl2::gfx::IGLAllocator* pAllocator, DXTColorBlock* line, size_t numberBlocks){
		DXTColorBlock *colorBlock = line;
		
		for(size_t i = 0; i < numberBlocks; i++){
			Swap(pAllocator, &colorBlock->row[0], &colorBlock->row[3], sizeof(unsigned char));
			Swap(pAllocator, &colorBlock->row[1], &colorBlock->row[2], sizeof(unsigned char));
			
			colorBlock++;
		}
	}
	
	void Information::FlipBlocksDXTC3(gfl2::gfx::IGLAllocator* pAllocator, DXTColorBlock* line, size_t numberBlocks){
		DXTColorBlock *colorBlock = line;
		DXT3AlphaBlock *alphaBlock;
		
		for(size_t i = 0; i < numberBlocks; i++){
			alphaBlock = (DXT3AlphaBlock*) colorBlock;
			
			Swap(pAllocator, &alphaBlock->row[0], &alphaBlock->row[3], sizeof(u16));
			Swap(pAllocator, &alphaBlock->row[1], &alphaBlock->row[2], sizeof(u16));
			
			colorBlock++;
			
			Swap(pAllocator, &colorBlock->row[0], &colorBlock->row[3], sizeof(unsigned char));
			Swap(pAllocator, &colorBlock->row[1], &colorBlock->row[2], sizeof(unsigned char));
			
			colorBlock++;
		}
	}
	
	void Information::FlipDXTC5Alpha(gfl2::gfx::IGLAllocator* pAllocator, DXT5AlphaBlock *block){
		unsigned char temporaryBit[4][4];
		
		const unsigned long mask = 0x00000007;
		
		unsigned long bit = 0;
		::memcpy(&bit, &block->row[0], sizeof(unsigned char) * 3);
		
		temporaryBit[0][0] = (unsigned char)(bit & mask);
		bit >>= 3;
		temporaryBit[0][1] = (unsigned char)(bit & mask);
		bit >>= 3;
		temporaryBit[0][2] = (unsigned char)(bit & mask);
		bit >>= 3;
		temporaryBit[0][3] = (unsigned char)(bit & mask);
		bit >>= 3;
		temporaryBit[1][0] = (unsigned char)(bit & mask);
		bit >>= 3;
		temporaryBit[1][1] = (unsigned char)(bit & mask);
		bit >>= 3;
		temporaryBit[1][2] = (unsigned char)(bit & mask);
		bit >>= 3;
		temporaryBit[1][3] = (unsigned char)(bit & mask);
		
		bit = 0;
		::memcpy(&bit, &block->row[3], sizeof(unsigned char) * 3);
		
		temporaryBit[2][0] = (unsigned char)(bit & mask);
		bit >>= 3;
		temporaryBit[2][1] = (unsigned char)(bit & mask);
		bit >>= 3;
		temporaryBit[2][2] = (unsigned char)(bit & mask);
		bit >>= 3;
		temporaryBit[2][3] = (unsigned char)(bit & mask);
		bit >>= 3;
		temporaryBit[3][0] = (unsigned char)(bit & mask);
		bit >>= 3;
		temporaryBit[3][1] = (unsigned char)(bit & mask);
		bit >>= 3;
		temporaryBit[3][2] = (unsigned char)(bit & mask);
		bit >>= 3;
		temporaryBit[3][3] = (unsigned char)(bit & mask);
		
		unsigned long *destinationBit = ((unsigned long*) &(block->row[0]));
		
#if defined(GFL2_BIG_ENDIAN)
		*destinationBit &= 0x000000ff;
#else
		*destinationBit &= 0xff000000;
#endif
		
		*destinationBit = *destinationBit | (temporaryBit[3][0] << 0);
		*destinationBit = *destinationBit | (temporaryBit[3][1] << 3);
		*destinationBit = *destinationBit | (temporaryBit[3][2] << 6);
		*destinationBit = *destinationBit | (temporaryBit[3][3] << 9);
		
		*destinationBit = *destinationBit | (temporaryBit[2][0] << 12);
		*destinationBit = *destinationBit | (temporaryBit[2][1] << 15);
		*destinationBit = *destinationBit | (temporaryBit[2][2] << 18);
		*destinationBit = *destinationBit | (temporaryBit[2][3] << 21);
		
		destinationBit = ((unsigned long*) &(block->row[3]));
		
#if defined(GFL2_BIG_ENDIAN)
		*destinationBit &= 0x000000ff;
#else
		*destinationBit &= 0xff000000;
#endif
		
		*destinationBit = *destinationBit | (temporaryBit[1][0] << 0);
		*destinationBit = *destinationBit | (temporaryBit[1][1] << 3);
		*destinationBit = *destinationBit | (temporaryBit[1][2] << 6);
		*destinationBit = *destinationBit | (temporaryBit[1][3] << 9);
		
		*destinationBit = *destinationBit | (temporaryBit[0][0] << 12);
		*destinationBit = *destinationBit | (temporaryBit[0][1] << 15);
		*destinationBit = *destinationBit | (temporaryBit[0][2] << 18);
		*destinationBit = *destinationBit | (temporaryBit[0][3] << 21);
	}
	
	void Information::FlipBlocksDXTC5(gfl2::gfx::IGLAllocator* pAllocator, DXTColorBlock* line, size_t numberBlocks){
		DXTColorBlock* colorBlock = line;
		DXT5AlphaBlock* alphaBlock;
		
		for(size_t i = 0; i < numberBlocks; i++){
			alphaBlock = (DXT5AlphaBlock*) colorBlock;
			
			FlipDXTC5Alpha(pAllocator, alphaBlock);
			
			colorBlock++;
			
			Swap(pAllocator, &colorBlock->row[0], &colorBlock->row[3], sizeof(unsigned char));
			Swap(pAllocator, &colorBlock->row[1], &colorBlock->row[2], sizeof(unsigned char));
			
			colorBlock++;
		}
	}
	
// ----------------------------------------------------------------------------
	
	Texture::Texture():
		surfaceContainerIndex(0){
	}
	
	Texture::Texture(const Texture& source):
		surfaceContainerIndex(0){
		for(size_t i = 0; i < source.GetNumberOfMipmap(); i++)
			this->AddMipmap((Surface*) &source.surfaceContainer[i]);
	}
	
	Texture::~Texture(){
	}
	
	Texture& Texture::operator =(const Texture& source){
		if(this != &source){
			this->surfaceContainerIndex = 0;
			for(size_t i = 0; i < source.GetNumberOfMipmap(); i++)
				this->AddMipmap((Surface*) &source.surfaceContainer[i]);
		}
		return *this;
	}
	
// ----------------------------------------------------------------------------
	
	Surface::Surface():
		instance(NULL),
		substance(NULL){
	}
	
	Surface::Surface(size_t width, size_t height, size_t depth, size_t size):
		width(width),
		height(height),
		depth(depth),
		size(size),
		instance(NULL),
		substance(NULL){
	}
	
	Surface::Surface(const Surface& source):
		instance(NULL),
		substance(NULL){
		
		this->Copy(source);
	}
	
	Surface::~Surface(){
		this->Clear();
	}
	
	Surface& Surface::operator =(const Surface& source){
		this->Copy(source);
		
		return *this;
	}
	
	void Surface::Copy(const Surface& source){
		if(this != &source){
			this->Clear();
			
			this->size   = source.size;
			this->width  = source.width;
			this->height = source.height;
			this->depth  = source.depth;
			
			if(source.instance != NULL){
				this->Instance(source.instance);
			}
			if(source.substance != NULL){
				this->Substance(source.substance);
			}
		}
	}
	
	void Surface::Instance(void* source){
		this->Clear();
		
		this->instance = this->GetInstanceAllocator()->SystemMemoryMalloc(static_cast<u32>(this->size));
		::memcpy(this->instance, source, this->size);
	}
	
	void Surface::Substance(void* source){
		this->Clear();
		
		this->substance = source;
	}
	
	void Surface::Clear(){
		if(this->instance != NULL){
			this->GetInstanceAllocator()->SystemMemoryFree(this->instance);
			this->instance = NULL;
		}
		if(this->substance != NULL){
			this->substance = NULL;
		}
	}

}}}

#undef DDS_Y_FLIP

#endif // #if defined(GF_PLATFORM_WIN32)


