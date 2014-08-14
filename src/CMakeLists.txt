cmake_minimum_required(VERSION 2.8)
project(PV-SysReader)

add_executable(PV-SysReader pv_inverters/victron_bs.c  battery_monitors/victron_bmv.c pv_inverters/victron_mk2.c pv_chargers/victron_mppt.c rs232/rs232.c PV-SysReader.c)

set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -DDEBUG -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -Wall")

install(TARGETS PV-SysReader RUNTIME DESTINATION bin)