

#pragma once

#ifdef __ANDROID__
  #ifndef RH_TARGET_API_GLES2
    #define RH_TARGET_API_GLES2
  #endif
  #ifndef RH_TARGET_OS_ANDROID
    #define RH_TARGET_OS_ANDROID
  #endif
#endif

#ifdef __ANDROID__
	#include <android/log.h>
	#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
	#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))
	#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "native-activity", __VA_ARGS__))
#else
	#define LOGI(...) ((void)printf(__VA_ARGS__))
	#define LOGW(...) ((void)printf(__VA_ARGS__))
	#define LOGE(...) ((void)printf(__VA_ARGS__))
#endif

#ifdef RH_TARGET_OS_ANDROID
	#include<android/asset_manager.h>
#endif

#define GL_GLEXT_PROTOTYPES 1

#ifdef RH_TARGET_API_GLES2
  #include <GLES2/gl2.h>
  #include <GLES2/gl2ext.h>
#else
  #include <GL/gl.h>
  #include <GL/glext.h>
#endif

#ifndef GL_TEXTURE_2D_ARRAY_EXT
#define GL_TEXTURE_2D_ARRAY_EXT 0x8C1A
#endif
#ifndef GL_COMPRESSED_RGB_S3TC_DXT1_EXT
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT  0x83F0
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#endif

#ifdef RH_TARGET_API_GLES2

  #define GL_COMPRESSED_TEX_IMAGE_3D 		glCompressedTexImage3DOES
  #define GL_TEX_IMAGE_3D 			glTexImage3DOES
  #define GL_TEX_SUMBIMAGE_3D 			glTexSubImage3DOES
  #define GL_COMPRESSED_TEX_SUBIMAGE_3D		glCompressedTexSubImage3DOES

#else

  #define GL_COMPRESSED_TEX_IMAGE_3D 		glCompressedTexImage3D
  #define GL_TEX_IMAGE_3D 			glTexImage3D
  #define GL_TEX_SUMBIMAGE_3D 			glTexSubImage3D
  #define GL_COMPRESSED_TEX_SUBIMAGE_3D 	glCompressedTexSubImage3D

#endif

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "rh_raw_loader.h"
#include "file_header.h"

#ifdef __cplusplus

class _rawpak_ctx;
extern "C" {
#endif //  __cplusplus

	#ifdef RH_TARGET_OS_ANDROID
	typedef AAsset 			AssetType;
	typedef AAssetManager		AssetManagerType;

	static inline AssetType * _OpenAsset( AssetManagerType * manager, const char * file) {

		return AAssetManager_open( manager, file, AASSET_MODE_STREAMING);
	}

	static inline int _ReadAsset(AssetType * asset, void * ptr, size_t count) {

		return AAsset_read(asset, ptr, count);
	}

	// returns -1 on error.
	static inline int _SeekAsset(AssetType * asset, off_t offset, int whence ) {

		return AAsset_seek(asset, offset, whence);
	}

	static inline void _CloseAsset(AssetType * asset) {

		AAsset_close(asset);
	}

	static inline AssetManagerType * _GetAssetManager() {

	  /*** MEGGA HACK! ***/

	  extern AAssetManager * __rh_hack_get_android_asset_manager();

	  return __rh_hack_get_android_asset_manager();
	}
#else
	typedef FILE	AssetType;
	typedef void	AssetManagerType;

	static inline AssetType * _OpenAsset( AssetManagerType * manager, const char * file) {

		return fopen(file, "rb");
	}

	// returns -1 on error.
	static inline int _ReadAsset(AssetType * asset, void * ptr, size_t count) {

		size_t r = fread(ptr, count, 1, asset);

		if(r == 1)
			return count;

		return -1;
	}

	static inline int _SeekAsset(AssetType * asset, off_t offset, int whence ) {

		return fseek(asset, offset, whence);
	}

	static inline void _CloseAsset(AssetType * asset) {

		fclose(asset);
	}

	static inline AssetManagerType * _GetAssetManager() {

	  return NULL;
	}
#endif

struct _rawpak_type {

	struct rhrpak_hdr 		header;
	struct rhrpak_hdr_hmap *hmap;

	AssetManagerType * assetManager;
	AssetType * asset;

	pthread_mutex_t monitor;
};

struct _rawpak_ctx {

	struct _rawpak_type * loader;
	struct rhrpak_hdr_hmap *hmap;
	size_t pos;
};

#ifdef __cplusplus
}
#endif // __cplusplus

