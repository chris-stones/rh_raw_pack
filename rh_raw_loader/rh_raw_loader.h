
#pragma once

#include<stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RH_RAWPAK_FILESYSTEM	0x01
#define RH_RAWPAK_ANDROID_APK	0x02
#ifdef __ANDROID__
#define RH_RAWPAK_APP RH_RAWPAK_ANDROID_APK
#endif
#ifndef RH_RAWPAK_APP
#define RH_RAWPAK_APP RH_RAWPAK_FILESYSTEM
#endif

struct _rawpak_type;
typedef struct _rawpak_type * rh_rawpak_handle;

struct _rawpak_ctx;
typedef struct _rawpak_ctx * rh_rawpak_ctx;

int rh_rawpak_open 			(const char * rawpak_file, rh_rawpak_handle * handle, int flags);
int rh_rawpak_close			(rh_rawpak_handle handle);

int rh_rawpak_open_ctx		(rh_rawpak_handle handle, const char * name, rh_rawpak_ctx * ctx );
int rh_rawpak_close_ctx		(rh_rawpak_ctx ctx );

int rh_rawpak_read			(void* data, size_t size, size_t nbemb, rh_rawpak_ctx ctx );
int rh_rawpak_seek			(rh_rawpak_ctx ctx, long offset, int whence);

int rh_rawpak_open_avformatctx	(rh_rawpak_ctx ctx, size_t internalBufferSize, void ** format_ctx );


#ifdef __cplusplus
} // extern "C" {
#endif

