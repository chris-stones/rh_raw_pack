

/**************************************************************************
 * Custom avformat IO context.
 * Thanks Philipp Sch
 * http://www.codeproject.com/Tips/489450/Creating-Custom-FFmpeg-IO-Context
 */

/*** INCLUDE FFMPEG ***************/
#ifndef UINT64_C
#define UINT64_C(c) (c ## ULL)
#endif
#ifdef __cplusplus
extern "C" {
#endif
#include <libavformat/avformat.h>
#ifdef __cplusplus
} // extern "C" {
#endif
/**********************************/


#include "rh_raw_internal.h"



static int _read_func(void* ptr, uint8_t* buf, int buf_size)
{
    rh_rawpak_ctx* ctx = (rh_rawpak_ctx)ptr;
    size_t oldPos = ctx->pos;

	int err = rh_rawpak_read(buf, buf_size, 1, ctx);

	if( ctx->pos >= ctx->hmap->file_length )
		return AVERROR_EOF;

    if(err == -1 && (ctx->pos == oldPos))
        return -1;

    return ctx->pos - oldPos;
}

static int64_t _seek_func(void* ptr, int64_t pos, int whence)
{
	rh_rawpak_ctx* ctx = (rh_rawpak_ctx)ptr;

	if(whence == AVSEEK_SIZE)
		return ctx->hmap->file_length;

	rh_rawpak_seek(ctx, pos, whence);

    return ctx->pos;
}

