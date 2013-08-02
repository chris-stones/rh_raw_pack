

cmake_minimum_required(VERSION 2.8)


add_executable(rh_video_packer main.cpp hash.c config.c)

target_link_libraries (rh_video_packer boost_filesystem boost_system)

install(TARGETS rh_video_packer RUNTIME DESTINATION bin)

