#pragma once

#include "config.hpp"
#include<string>
#include<vector>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace Path {

class Video {

    std::string path;

public:

    class OpenVideoException : public std::exception {
    public:
        const char * what() const throw() {
            return "OpenVideoException";
        }
    };

    typedef std::vector<Video> Vector;

    Video( const fs::path &full_path ) {

        path = full_path.native();
    }

    const std::string &GetFileName() const {

        return path;
    }

    bool operator < ( const Video & that ) const {

        return this->path < that.path;
    }

    static bool IsVideoFile(const char * ext) {

		for(std::vector<std::string>::const_iterator itor = args.fileExtensions.begin(); itor != args.fileExtensions.end(); itor++)
			if( strcasecmp(itor->c_str(), ext) == 0)
				return true;

		return false;
    }
};

class Directory {

public:

    typedef std::vector<Directory> Vector;

private:

    Video::Vector 	videos;
    Vector			subDirs;

public:

    class OpenDirException : public std::exception {
    public:
        const char * what() const throw() {
            return "OpenDirException";
        }
    };

    Directory(const fs::path &full_path) {

        Construct( full_path );
    }

    const Video::Vector &GetImages( ) 		const {
        return videos;
    }
    const Vector        &GetDirectories() 	const {
        return subDirs;
    }

    std::vector<std::string> AllVideoFiles() const {

		std::vector<std::string> allVideoFiles;

		AddVideoFiles(allVideoFiles);

		return allVideoFiles;
    }

private:

	void AddVideoFiles(std::vector<std::string> & all) const {

		for(Video::Vector::const_iterator itor = videos.begin(); itor != videos.end(); itor++)
			all.push_back( itor->GetFileName() );

		for(Vector::const_iterator itor = subDirs.begin(); itor != subDirs.end(); itor++)
			itor->AddVideoFiles( all );
    }

    void Construct(const fs::path &full_path) {

        if(!fs::exists(full_path))
            throw OpenDirException();

        if(!fs::is_directory(full_path))
            throw OpenDirException();

        fs::directory_iterator end;
        for( fs::directory_iterator itor = fs::directory_iterator( full_path ); itor != end; itor++) {

            if(fs::is_directory( itor->status() )) {

                subDirs.push_back( Directory( itor->path() ));

            } else if( fs::is_regular_file( itor->status() )) {

				if(Video::IsVideoFile( itor->path().extension().c_str() )) {

					try {

						videos.push_back( Video( itor->path() ));

					} catch(...) {
					}
				}
            }
        }
    }
};
}
