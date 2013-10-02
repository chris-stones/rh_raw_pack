
#pragma once

#include <stdio.h>

#ifdef __ANDROID__
	#include<android/asset_manager.h>
#endif

#define RHF_OPEN(   x,a  ) x.open(   &(x),a)
#define RHF_CLOSE(  x    ) x.close(  &(x))
#define RHF_READ(   x,a,b) x.read(   &(x),a,b)
#define RHF_SEEK(   x,a,b) x.seek(   &(x),a,b)
#define RHF_GETMGR( x    ) x.getmgr( &(x))

#ifdef __cplusplus
extern "C" {
#endif

	struct rh_file;
	typedef struct rh_file rh_file_t;

	typedef int (*OPENASSET_FUNC_T  )	(rh_file_t * p, const char * file);
	typedef int (*READASSET_FUNC_T  )	(rh_file_t * p, void * ptr, size_t count);
	typedef int (*SEEKASSET_FUNC_T  )	(rh_file_t * p, off_t offset, int whence);
	typedef int (*CLOSEASSET_FUNC_T )	(rh_file_t * p);
	typedef int (*GETASSETMGR_FUNC_T)	(rh_file_t * p);

	typedef struct rh_file {

		OPENASSET_FUNC_T	open;
		READASSET_FUNC_T	read;
		SEEKASSET_FUNC_T	seek;
		CLOSEASSET_FUNC_T	close;
		GETASSETMGR_FUNC_T	getmgr;

		#ifdef __ANDROID__
			AAssetManager 	* assetManager_android;
			AAsset 			* asset_android;
		#endif

		void			* assetManager_filesystem;
		FILE			* asset_filesystem;

	} rh_file_t;

	#ifdef __ANDROID__

	static inline int _OpenAsset_android( rh_file_t * p, const char * file) {

		if((p->asset_android = AAssetManager_open( p->assetManager_android, file, AASSET_MODE_STREAMING)))
			return 0;

		return -1;
	}

	static inline int _ReadAsset_android(rh_file_t * p, void * ptr, size_t count) {

		return AAsset_read(p->asset_android, ptr, count);
	}

	// returns -1 on error.
	static inline int _SeekAsset_android(rh_file_t * p, off_t offset, int whence ) {

		return AAsset_seek(p->asset_android, offset, whence);
	}

	static inline int _CloseAsset_android(rh_file_t * p) {

		if(p->asset_android) {
			AAsset_close(p->asset_android);
			p->asset_android = NULL;
		}

		return 0;
	}

	static inline int _GetAssetManager_android(rh_file_t * p) {

	  /*** MEGGA HACK! ***/
	  extern AAssetManager * __rh_hack_get_android_asset_manager();

	  p->assetManager_android = __rh_hack_get_android_asset_manager();

	  return 0;
	}

	static inline void _setup_file_android(rh_file_t * p) {

		p->open = &_OpenAsset_android;
		p->close = &_CloseAsset_android;
		p->read = &_ReadAsset_android;
		p->seek = &_SeekAsset_android;
		p->getmgr = &_GetAssetManager_android;
	}
#endif

	static inline int _OpenAsset_filesystem( rh_file_t * p, const char * file) {

		if(( p->asset_filesystem = fopen(file, "rb")))
			return 0;

		return -1;
	}

	// returns -1 on error. 0 on EOF, else, the bytes read.
	static inline int _ReadAsset_filesystem(rh_file_t * p, void * ptr, size_t count) {

		size_t r = fread(ptr, 1, count, p->asset_filesystem);

		if(r == 0) {
			if(feof(p->asset_filesystem))
				return 0;
			return -1;
		}

		return r;
	}

	static inline int _SeekAsset_filesystem(rh_file_t * p, off_t offset, int whence ) {

		return fseek(p->asset_filesystem, offset, whence);
	}

	static inline int _CloseAsset_filesystem(rh_file_t * p) {

		if(p->asset_filesystem) {
			fclose(p->asset_filesystem);
			p->asset_filesystem = NULL;
		}

		return 0;
	}

	static inline int _GetAssetManager_filesystem(rh_file_t * p) {

	  return 0;
	}

	static inline void _setup_file_filesystem(rh_file_t * p) {

		p->open = &_OpenAsset_filesystem;
		p->close = &_CloseAsset_filesystem;
		p->read = &_ReadAsset_filesystem;
		p->seek = &_SeekAsset_filesystem;
		p->getmgr = &_GetAssetManager_filesystem;
	}

#ifdef __cplusplus
} // extern "C" {
#endif

