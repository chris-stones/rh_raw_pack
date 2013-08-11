
#pragma once

#include<string>
#include<vector>

struct arguments
{
  int verbose;
  int quiet;
  char *output_file;
  char *log_file;
  char *resources;
  int debug;

  std::vector<std::string> fileExtensions;

  arguments()
	:	verbose(0),
		quiet(0),
		output_file(NULL),
		log_file(NULL),
		resources(NULL),
		debug(0)
	{
	}
};

arguments read_args(int argc, char ** argv );

extern arguments args;


