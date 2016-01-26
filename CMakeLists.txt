cmake_minimum_required (VERSION 3.0)
project (lineartrails)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set (CMAKE_CXX_FLAGS			"-Wall -Wpedantic -std=c++11")
set (CMAKE_CXX_FLAGS_RELEASE	"-O3 -march=native -DNDEBUG")
set (CMAKE_CXX_FLAGS_DEBUG		"-O0 -g")

add_executable (lin
	tool/main.cpp

	tinyxml2/tinyxml2.cpp
	tinyxml2/tinyxml2.h

	tool/cache.h
	tool/cache.hpp
	tool/commandlineparser.cpp
	tool/commandlineparser.h
	tool/configparser.cpp
	tool/configparser.h
	tool/guessmask.cpp
	tool/guessmask.h
	tool/layer.cpp
	tool/layer.h
	tool/lrucache.h
	tool/lrucache.hpp
	tool/mask.cpp
	tool/mask.h
	tool/permutation.cpp
	tool/permutation.h
	tool/permutation_list.cpp
	tool/permutation_list.h
	tool/search.cpp
	tool/search.h
	tool/statemask.h
	tool/statemask.hpp
	tool/step_linear.h
	tool/step_linear.hpp
	tool/step_nonlinear.h
	tool/step_nonlinear.hpp

	target/ascon.cpp
	target/ascon.h
	target/ascon_permutation.cpp
	target/ascon_permutation.h
	target/icepole.cpp
	target/icepole.h
	target/icepole_permutation.cpp
	target/icepole_permutation.h
	target/keccak1600.cpp
	target/keccak1600.h
	target/keccak1600_permutation.cpp
	target/keccak1600_permutation.h
	target/prost256.cpp
	target/prost256.h
	target/prost256_permutation.cpp
	target/prost256_permutation.h
)
target_compile_definitions (lin PRIVATE -DTERMINALCOLORS)
target_include_directories (lin PRIVATE tool/ target/)
