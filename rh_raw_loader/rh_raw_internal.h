

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

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "rh_raw_loader.h"
#include "file_header.h"
#include "rh_file.h"

#ifdef __cplusplus
extern "C" {
#endif //  __cplusplus


	struct _rawpak_type {

		struct rhrpak_hdr 		header;
		struct rhrpak_hdr_hmap *hmap;

		pthread_mutex_t monitor;

		int flags;

		rh_file_t file;
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

