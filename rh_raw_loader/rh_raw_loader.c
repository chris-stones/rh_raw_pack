
#include "rh_raw_internal.h"

#include<stdlib.h>
#include<assert.h>

static int _open_failed(rh_rawpak_handle loader, int line) {

	printf("rh_rawpak_open failed - line %d\n", line);

	if(loader) {
		free(loader->hmap);
		RHF_CLOSE(loader->file);
		free(loader);
	}
	return -1;
}

static int _has_asset(rh_rawpak_handle  _loader) {

	if(!_loader)
		return 0;
	if(_loader->file.asset_filesystem)
		return 1;
#ifdef __ANDROID__
	if(_loader->asset_android)
		return 1;
#endif
	return 0;
}

static int _setup_access_functionptrs(rh_rawpak_handle  _loader) {

#ifdef __ANDROID__
	if(loader->flags & RH_RAWPAK_ANDROID_APK) {

		_setup_file_android(&_loader->file);
		return 0;
	}
#endif

	_setup_file_filesystem(&_loader->file);
	return 0;
}

int rh_rawpak_open(const char * file, rh_rawpak_handle * _loader, int flags) {

	rh_rawpak_handle loader = NULL;

	if((loader = calloc(1, sizeof(struct _rawpak_type))) == NULL)
		return _open_failed(loader, __LINE__);

	loader->flags = flags;

	_setup_access_functionptrs(loader);

	RHF_GETMGR(loader->file);

	if((RHF_OPEN(loader->file, file)) != 0)
		return _open_failed(loader, __LINE__);
	if((RHF_READ(loader->file, &loader->header, sizeof loader->header)) != sizeof loader->header)
		return _open_failed(loader, __LINE__);
	if( strncasecmp("rockhopper.rpak", loader->header.magic, 16) != 0)
		return _open_failed(loader, __LINE__);
	if((loader->hmap = malloc(loader->header.resources * sizeof(struct rhrpak_hdr_hmap)))==NULL)
		return _open_failed(loader, __LINE__);
	if( RHF_SEEK(loader->file, loader->header.hmap_ptr, SEEK_SET) == -1 )
		return _open_failed(loader, __LINE__);
	if( RHF_READ(loader->file, loader->hmap, sizeof(struct rhrpak_hdr_hmap) * loader->header.resources ) != sizeof(struct rhrpak_hdr_hmap) * loader->header.resources )
		return _open_failed(loader, __LINE__);

	if(pthread_mutex_init(&loader->monitor, NULL) != 0) {
		pthread_mutex_destroy(&loader->monitor);
		return _open_failed(loader, __LINE__);
	}

	*_loader = loader;

	return 0;
}

int rh_rawpak_close(rh_rawpak_handle loader) {

	if(loader) {
		free(loader->hmap);
		RHF_CLOSE(loader->file);
		pthread_mutex_destroy(&loader->monitor);
		free(loader);
	}

	return 0;
}

static unsigned int pl_hash( const char* _s, unsigned int seed)
{
    const char * s = _s;
    unsigned int hash = seed;
    int len = strlen(s);
    int i;

    while(len) {
    	len--;
    	if(s[len]=='.')
    		break;
    }

    for(i=0;i<len;i++)
    {
    	char c = *s++;

    	switch(c) {
    	case '/':
    	case '\\':
    		c = '.';
    		break;
    	default:
    		c = tolower(c);
    		break;
    	}

    	// THANKS PAUL LARSON.
        hash = hash * 101 + c;
    }

    return hash;
}

static int compare_hash(const void * key, const void * memb) {

	const unsigned int    * k = (const unsigned int *)key;
	const struct rhrpak_hdr_hmap * m = (const struct rhrpak_hdr_hmap*)memb;

	if( *k < m->hash )
		return -1;
	if( *k > m->hash )
		return 1;
	return 0;
}

int rh_rawpak_open_ctx(rh_rawpak_handle loader, const char * name, rh_rawpak_ctx * _ctx ) {

	struct rhrpak_hdr_hmap *hash = NULL;
	unsigned int key;
	struct rhrpak_hdr_hmap * res;

	if(!loader || !_ctx)
		return 0;

	key = pl_hash( name, loader->header.seed );

	if((res = (struct rhrpak_hdr_hmap *)bsearch(&key, loader->hmap, loader->header.resources, sizeof(struct rhrpak_hdr_hmap ), &compare_hash))) {

		rh_rawpak_ctx ctx = calloc(1, sizeof(struct _rawpak_ctx));

		if(ctx) {

			ctx->loader = loader;
			ctx->hmap = res;
			*_ctx = ctx;
			return 0;
		}
	}

	printf("rh_rawpak_open_ctx() cant find %s\n", name);

	return -1;
}

int rh_rawpak_close_ctx(rh_rawpak_ctx ctx ) {

	if(ctx) {
		free(ctx->avformat_buffer);
	}
	free(ctx);
	return -1;
}

// returns bytes read, 0 on EOF, -1 on err.
int rh_rawpak_read(void* data, size_t size, size_t nbemb, rh_rawpak_ctx ctx ) {

	if(!data || !ctx || !ctx->hmap || !ctx->loader || !_has_asset(ctx->loader) )
		return -1;

	if(pthread_mutex_lock(&ctx->loader->monitor) == 0) {

		int err = 0;

		if( RHF_SEEK(ctx->loader->file, ctx->hmap->file_ptr + ctx->pos, SEEK_SET ) == -1 )
			err = -1;

		if( !err )
			err = RHF_READ(ctx->loader->file, data, size * nbemb);

		pthread_mutex_unlock(&ctx->loader->monitor);

		if(err < 0)
			return err;

		ctx->pos += size * nbemb;

		if(ctx->pos > ctx->hmap->file_length ) {
			ctx->pos = ctx->hmap->file_length;
			return -1; // read past end of embedded file.
		}

		return err;
	}
	return -1;
}

int rh_rawpak_seek(rh_rawpak_ctx ctx, long offset, int whence) {

	if(!ctx || !ctx->loader || !_has_asset(ctx->loader) || !ctx->hmap)
		return -1;

	switch(whence) {
		case SEEK_SET:
			ctx->pos = offset;
			break;
		case SEEK_CUR:
			ctx->pos += offset;
			break;
		case SEEK_END:
			ctx->pos = ctx->hmap->file_length + offset;
			break;
	}

	if(ctx->pos >= 0 && ctx->pos <= ctx->hmap->file_length)
		return 0;

	return -1;
}

