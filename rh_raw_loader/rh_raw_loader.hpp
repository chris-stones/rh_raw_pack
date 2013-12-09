
#pragma once

#include "rh_raw_loader.h"

#include <type_traits>
#include <cstdint>
#include <exception>
#include <stdexcept>
#include <cassert>
#include <memory>

namespace rh {

	class RawPak;

	class RawCtx {

		rh_rawpak_ctx ctx;

		friend class RawPak;

		std::shared_ptr<RawPak> pak; // only to prevent destruction of parent TexturePak

		RawCtx( std::shared_ptr<RawPak> pak, rh_rawpak_ctx ctx )
			:	pak(pak),
				ctx(ctx)
		{}

	public:

		class ReadException 		: public std::exception {public: const char * what() const throw() { return "RawPak::ReadException";     } };
		class SeekException 		: public std::exception {public: const char * what() const throw() { return "RawPak::SeekException";     } };
		class AvFormatException 	: public std::exception {public: const char * what() const throw() { return "RawPak::AvFormatException"; } };


		~RawCtx() {
			rh_rawpak_close_ctx( ctx );
		}

		template<typename _T>
		int Read(_T* data, size_t nbemb=1) {

			if( rh_rawpak_read(data,sizeof(_T),nbemb,ctx) != nbemb )
				throw ReadException();

			return nbemb;
		}

		int Read(void* data, size_t size, size_t nbemb) {

			if( rh_rawpak_read(data,size,nbemb,ctx) != nbemb )
				throw ReadException();

			return nbemb;
		}

		int Seek(long offset, int whence) {

			if( rh_rawpak_seek(ctx, offset, whence) != 0 )
				throw SeekException();

			return 0;
		}

		template<AVFORMATCONTEXT>
		int GetAvFormatContext(AVFORMATCONTEXT & avformatcontext, size_t internalBufferSize=0) {

			if( rh_rawpak_open_avformatctx(ctx, internalBufferSize, &avformatcontext) != 0 )
				throw AvFormatException();

			return 0;
		}
	};

	class RawPak {

		rh_rawpak_handle handle;

		bool isloaded{false};

		std::weak_ptr<RawPak> wthis;

		RawPak(const char * pakfile, int flags) {

			if( rh_rawpak_open( pakfile, &handle, flags ) != 0 )
				throw OpenException();
		}

	public:

		class OpenException 	: public std::exception {public: const char * what() const throw() { return "RawPak::OpenException"; } };
		class LoadException 	: public std::exception {public: const char * what() const throw() { return "RawPak::LoadException"; } };
		class LookupException 	: public std::runtime_error { using std::runtime_error::runtime_error; };

		typedef enum {

			FILESYSTEM 				= RH_RAWPAK_FILESYSTEM,				/* load from the filesystem */
			ANDROID_APK 			= RH_RAWPAK_ANDROID_APK,			/* load from android APK */
			PLATFORM_DEFAULT		= RH_RAWPAK_APP,					/* load from default for this platform */
		} texture_pak_flags_enum_t;

		static std::shared_ptr<RawPak> MakeShared(const char * pakfile, int flags) {

			std::shared_ptr<RawPak> obj =
				std::shared_ptr<RawPak>( new RawPak( pakfile, flags ) );

			obj->wthis = obj;

			return obj;
		}

		static std::shared_ptr<RawCtx> GetResource(const char * resourceName) {

			rh_rawpak_ctx _ctx = NULL;

			if( rh_rawpak_open_ctx(handle,resourceName, &_ctx) != 0)
				throw LookupException( resourceName );

			std::shared_ptr<RawCtx> ctx =
				std::shared_ptr<RawCtx>(wthis.lock(), _ctx);

			return ctx;
		}

		virtual ~RawPak() {

			int e = rh_rawpak_close(handle);
			assert(e == 0);
		}
	};
}

