
#pragma once

#include "file_header.h"
#include "hash.h"
#include "config.h"

#include<vector>
#include<string>
#include<algorithm>
#include<assert.h>
#include<stdlib.h>
#include<string.h>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

static int tohashable(int c) {

  switch(c) {
    default:
      return tolower(c);
    case '/':
    case '\\':
      return '.';
  }
}

static std::string get_game_resource_name(std::string hashString, const char * resource_root) {

  int dot = hashString.find_last_of(".");
  assert(dot != std::string::npos );
  hashString = hashString.substr(0, dot);
  std::transform(hashString.begin(), hashString.end(), hashString.begin(), tohashable);
  hashString = std::string( hashString.begin()+strlen(resource_root), hashString.end() );
  if(hashString[0] == '.')
    hashString = std::string( hashString.begin()+1, hashString.end() );
  return hashString;
}

class Output {

	rhvpak_hdr header;
	std::vector<rhvpak_hdr_hmap> hmap;

	FILE * file;

public:

	class OutputOpenException : public std::exception {public: const char * what() const throw() { return "OutputOpenException"; } };
	class OutputWriteException : public std::exception {public: const char * what() const throw() { return "OutputWriteException"; } };
	class OutputSeekException : public std::exception {public: const char * what() const throw() { return "OutputSeekException"; } };
	class OutputReadException : public std::exception {public: const char * what() const throw() { return "OutputReadException"; } };
	class OutputMemoryException : public std::exception {public: const char * what() const throw() { return "OutputReadException"; } };

	Output(const char * fn, const char * res_root, const std::vector<std::string> &allVideos, unsigned int seed) {

		file = fopen(fn, "wb");
		if(!file)
			throw OutputOpenException();

		memset(&header, 0, sizeof header);

		header.seed = seed;
		strcpy(header.magic, "rockhopper.vpak");
		header.resources = allVideos.size();

		Write(header);
		header.hmap_ptr = ftell(file);
		Seek(0, SEEK_SET);
		Write(header);

		std::map<unsigned int, rhvpak_hdr_hmap> hmap;
		rhvpak_hdr_hmap hdr;
		memset(&hdr,0,sizeof hdr);

		// make space for hash-map;
		for(int i=0;i<header.resources;i++)
			Write(hdr);

		// write video data.
		for(std::vector<std::string>::const_iterator itor = allVideos.begin(); itor != allVideos.end(); itor++) {

			std::string resName = get_game_resource_name( *itor, res_root);
			hdr.hash = hash(resName.c_str(), seed);
			hdr.file_ptr = ftell(file);
			hdr.file_length = fs::file_size(*itor);
			hmap[hdr.hash] = hdr;

			if(!args.quiet)
				printf("Adding:%s Hash:0x%08X Offset:0x%08X Length:0x%08X\n", resName.c_str(), hdr.hash, hdr.file_ptr, hdr.file_length);

			FILE * f = fopen(itor->c_str(), "rb");
			if(!f) {
				fclose(file);
				file = NULL;
				throw OutputReadException();
			}

			void * p = malloc(hdr.file_length);
			if(!p) {
				fclose(file);
				file = NULL;
				fclose(f);
				throw OutputMemoryException();
			}

			int r = fread(p, hdr.file_length, 1, f);
			fclose(f);

			if(r != 1) {
				free(p);
				fclose(file);
				file = NULL;
				throw OutputReadException();
			}

			try {
				Write(p, hdr.file_length);
				free(p);
			}
			catch( std::exception ex) {
				free(p);
				fclose(file);
				file = NULL;
				throw ex;
			}
		}

		// go back and write hash-map.
		Seek(header.hmap_ptr, SEEK_SET);
		for(std::map<unsigned int, rhvpak_hdr_hmap>::const_iterator itor = hmap.begin(); itor != hmap.end(); itor++)
			Write(itor->second);
	}

	virtual ~Output() {

		if(file)
			fclose(file);
	}

	static const char * WhenceStr(int whence) {

		switch(whence) {
		default: return "???";
		case SEEK_SET: return "SEEK_SET";
		case SEEK_CUR: return "SEEK_CUR";
		case SEEK_END: return "SEEK_END";
		}
	}

	void Seek( long offset, int whence ) {

		if( fseek( file, offset, whence ) != 0 ) {

		printf("oops - bad seek %d %s\n", (int)offset, WhenceStr((int)whence));

		throw OutputSeekException();
		}
	}

	void Write( const void * data, unsigned int size ) {

		if(fwrite(data,size,1,file) != 1)
		throw OutputWriteException();
	}

	template<typename _T> void Write( const _T & data ) {

		Write(&data,sizeof data);
	}
};

