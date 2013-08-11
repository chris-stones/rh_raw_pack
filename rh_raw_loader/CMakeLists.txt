
cmake_minimum_required(VERSION 2.8)

add_library(rh_raw_loader SHARED rh_raw_loader)

#target_link_libraries (rh_raw_packer boost_filesystem boost_system)

install(TARGETS rh_raw_loader LIBRARY DESTINATION lib)


