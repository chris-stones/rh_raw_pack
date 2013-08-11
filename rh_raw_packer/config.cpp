#include "config.hpp"

#include <stdlib.h>
#include <argp.h>
#include <string.h>
#include <libimg.h>


const char *argp_program_version = "rh_video_packer 0.1";
const char *argp_program_bug_address = NULL; // "NULL";
static char doc[] = "rh_video_packer -- a program to package videos.";
static char args_doc[] = "RESOURCE_ROOT_PATH";

static struct argp_option options[] = {
  {"verbose",  'v', 0,      OPTION_ARG_OPTIONAL,  "Produce verbose output" },
  {"quiet",    'q', 0,      OPTION_ARG_OPTIONAL,  "Don't produce any output" },
  {"output",   'o', "FILE", 0,  "Output to FILE" },
  {"debug",    'D', 0,      OPTION_ARG_OPTIONAL,  "debug mode" },
  {"logfile",  'l', "FILE", 0,  "write log to FILE"},
  {"include",  'i', "FILE EXT", 0, "file extentions to include" },
  { 0 }
};

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = (struct arguments *)state->input;

  switch (key)
    {
    case 'q':
      arguments->quiet = 1;
      break;
    case 'v':
      arguments->verbose = 1;
      break;
    case 'o':
      arguments->output_file = arg;
      break;
    case 'l':
      arguments->log_file = arg;
      break;
    case 'D':
      arguments->debug = 1;
      break;
    case ARGP_KEY_ARG:
      if (state->arg_num >= 1)
	/* Too many arguments. */
        argp_usage (state);

      arguments->resources = arg;
      break;

	case 'i':
	{
		std::string argString(arg);
		std::size_t pos = 0;
		std::size_t newPos = 0;

		for(;;) {
			newPos = argString.find_first_of(",",pos);
			std::string ext;
			if(newPos == std::string::npos)
				ext = std::string(".") + std::string(arg + pos);
			else
				ext = std::string(".") + std::string(arg + pos, arg + newPos);

			arguments->fileExtensions.push_back(ext);

			if(newPos == std::string::npos)
				break;

			pos = newPos+1;
		}

		break;
	}

    case ARGP_KEY_END:
      if (state->arg_num < 1 || !arguments->output_file)
        argp_usage (state);
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

arguments args;

struct arguments read_args(int argc, char ** argv ) {

  struct arguments _args;

  argp_parse (&argp, argc, argv, 0, 0, &_args);

  return _args;
}
