

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
    rh_rawpak_ctx ctx = (rh_rawpak_ctx)ptr;
    size_t oldPos = ctx->pos;

	int err = rh_rawpak_read(buf, buf_size, 1, ctx);

	if( ctx->pos >= ctx->hmap->file_length )
		return AVERROR_EOF;

    if(err <= -1 && (ctx->pos == oldPos))
        return -1;

    return ctx->pos - oldPos;
}

static int64_t _seek_func(void* ptr, int64_t pos, int whence)
{
	rh_rawpak_ctx ctx = (rh_rawpak_ctx)ptr;

	if(whence == AVSEEK_SIZE)
		return ctx->hmap->file_length;

	rh_rawpak_seek(ctx, pos, whence);

    return ctx->pos;
}

int rh_rawpak_open_avformatctx(rh_rawpak_ctx ctx, size_t internalBufferSize, void ** format_ctx ) {

	AVFormatContext* pCtx = NULL;
	AVIOContext* pIOCtx;
	int probedBytes = 0;

	printf("rh_rawpak_open_avformatctx()\n");

	if(!ctx || !format_ctx)
		return -1;

	ctx->avformat_buffer_size = internalBufferSize ? internalBufferSize : (32 * 1024); // 32k default.

	if((ctx->avformat_buffer = calloc(1, ctx->avformat_buffer_size)) == NULL)
		return -1;

	pIOCtx = avio_alloc_context(
		ctx->avformat_buffer, ctx->avformat_buffer_size,
		0,
		ctx,
		&_read_func,
		NULL,
		&_seek_func);

	if(!pIOCtx) {
		free(ctx->avformat_buffer);
		ctx->avformat_buffer = NULL;
		ctx->avformat_buffer_size = 0;
		return -1;
	}

	pCtx = avformat_alloc_context();

	if(!pCtx) {
		av_free(pIOCtx);
		free(ctx->avformat_buffer);
		ctx->avformat_buffer = NULL;
		ctx->avformat_buffer_size = 0;
		return -1;
	}

	// Set the IOContext:
	pCtx->pb = pIOCtx;

	// Determining the input format:
	probedBytes = ctx->hmap->file_length > ctx->avformat_buffer_size ? ctx->avformat_buffer_size : ctx->hmap->file_length;
	if(rh_rawpak_read(ctx->avformat_buffer, probedBytes, 1, ctx) < 0) {
		av_free(pIOCtx);
		avformat_free_context(pCtx);
		free(ctx->avformat_buffer);
		ctx->avformat_buffer = NULL;
		ctx->avformat_buffer_size = 0;
		return -1;
	}

	rh_rawpak_seek(ctx, 0, SEEK_SET); // cannot fail.

	AVProbeData probeData;
	probeData.buf = (unsigned char*)ctx->avformat_buffer;
	probeData.buf_size = probedBytes;
	probeData.filename = "";

	pCtx->iformat = av_probe_input_format(&probeData, 1);

	printf("input format %s %s\n",pCtx->iformat->name, pCtx->iformat->long_name );

	pCtx->flags = AVFMT_FLAG_CUSTOM_IO;

	if((avformat_open_input(&pCtx, "", 0, 0)) != 0) {
		av_free(pIOCtx);
		avformat_free_context(pCtx);
		free(ctx->avformat_buffer);
		ctx->avformat_buffer = NULL;
		ctx->avformat_buffer_size = 0;
		return -1;
	}

	*format_ctx = pCtx;

	return 0;
}

