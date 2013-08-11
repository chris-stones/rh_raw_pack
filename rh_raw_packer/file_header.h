
#pragma once

struct rhvpak_hdr {

  char magic[16]; 				// "rockhopper.rpak"
  unsigned int version; 		// impl version
  unsigned int flags;			// flags
  unsigned int resources;		// number of videos
  unsigned int seed;			// hash seed
  unsigned int hmap_ptr;		// pointer to rhvpak_hdr_hmap ( array length is rhvpak_hdr::resources )
};

struct rhvpak_hdr_hmap {

	unsigned int hash;			// video name hash.
    unsigned int file_ptr;		// pointer to video file data.
    unsigned int file_length;	// file length of video.
};
