

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

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "rh_raw_loader.h"
#include "file_header.h"

#ifdef __cplusplus
//class _rawpak_ctx;
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

	// returns -1 on error. 0 on EOF, else, the bytes read.
	static inline int _ReadAsset(AssetType * asset, void * ptr, size_t count) {

		size_t r = fread(ptr, 1, count, asset);

		if(r == 0) {
			if(feof(asset))
				return 0;
			return -1;
		}

		return r;
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

	void * avformat_buffer;
	size_t avformat_buffer_size;
};

#ifdef __cplusplus
}
#endif // __cplusplus

