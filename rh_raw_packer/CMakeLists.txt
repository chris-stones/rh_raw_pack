
cmake_minimum_required(VERSION 2.8)

add_executable(rh_raw_packer main.cpp hash.c config.cpp)

target_link_libraries (rh_raw_packer boost_filesystem boost_system)

install(TARGETS rh_raw_packer RUNTIME DESTINATION bin)

