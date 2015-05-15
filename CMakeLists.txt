
# Copyright 2015 Stephan Menzel. Distributed under the Boost
# Software License, Version 1.0. (See accompanying file
# LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0)
project(moose_monty)

find_package(Boost COMPONENTS random chrono thread system REQUIRED)

include_directories(
	${CMAKE_SOURCE_DIR}
	${Boost_INCLUDE_DIRS}
)

add_executable(monty_hall
	monty_hall.cpp
)

target_link_libraries(monty_hall
	moose_tools
	${Boost_LIBRARIES}
)


