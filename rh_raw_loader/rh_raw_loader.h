
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct _rawpak_type;
typedef struct _rawpak_type * rh_rawpak_handle;

struct _rawpak_ctx;
typedef struct _rawpak_ctx * rh_rawpak_ctx;

int rh_rawpak_open 			(const char * gfx_file, rh_rawpak_handle * loader);
int rh_rawpak_close			(rh_rawpak_handle loader);

int rh_rawpak_open_ctx		(rh_rawpak_handle loader, const char * name, rh_rawpak_ctx * ctx );
int rh_rawpak_close_ctx		(rh_rawpak_handle loader, rh_rawpak_ctx ctx );

int rh_rawpak_read			(void* data, size_t size, size_t nbemb, rh_rawpak_ctx ctx );
int rh_rawpak_seek			(rh_rawpak_ctx ctx, long offset, int whence);

int rh_rawpak_avformatctx	(rh_rawpak_ctx ctx, void ** format_ctx );

#ifdef __cplusplus
} // extern "C" {
#endif

