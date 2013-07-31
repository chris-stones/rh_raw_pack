
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct arguments
{
  int verbose;
  int quiet;
  char *output_file;
  char *log_file;
  char *resources;
  int debug;
};

struct arguments read_args(int argc, char ** argv );

struct arguments args;

#ifdef __cplusplus
} // extern "C" {
#endif

