

#include<assert.h>
#include<stdio.h>
#include<stdlib.h>
#include<map>
#include<vector>
#include<string>

#include "config.hpp"
#include "hash.h"
#include "FindContent.hpp"
#include "Output.hpp"


static void CheckForFileCollisions( const std::vector<std::string> &allFiles, const char * res_root ) {

  std::vector<std::string>::const_iterator itor = allFiles.begin();
  std::vector<std::string>::const_iterator end  = allFiles.end();

  std::map<std::string, int> colmap;

  int collisions = 0;

  while(itor != end) {

    std::string gameresname = get_game_resource_name( *itor, res_root );

    int count = ++colmap[ gameresname ];

    if(count != 1) {

      printf("RESOURCE NAME COLLISION: %s\n", gameresname.c_str());
      collisions++;
    }

    itor++;
  }

  assert(collisions==0);
  if(collisions!=0)
	  exit(-1);
}

static int GetHashSeed( const std::vector<std::string> &allFiles, const char * res_root ) {

	int seed = 0x69; // LOL

	for(;;) {

		bool good = true;

		std::map<int,int> hashCountMap;

		for(std::vector<std::string>::const_iterator itor = allFiles.begin(); itor != allFiles.end(); itor++) {

			std::string resName = get_game_resource_name( *itor, args.resources );

			if((++hashCountMap[hash(resName.c_str(),seed) ]) != 1) {
				good = false; // hash collision, try a different seed.
				break;
			}
		}
		if(good)
			return seed;
		seed++;
	}
}

int main(int argc, char **argv) {

	// cmd line args.
	args = read_args(argc, argv);

	// parse filesystem.
	Path::Directory dir( args.resources );

	// collect filenames.
	std::vector< std::string > allVideos =
		dir.AllVideoFiles();

	// check for case-sencitivity filename collisions.
	CheckForFileCollisions( allVideos, args.resources );

	// write video file.
	Output oput(args.output_file, args.resources, allVideos, GetHashSeed(allVideos, args.resources) );

	return 0;
}
