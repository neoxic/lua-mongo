cmake_minimum_required(VERSION 3.0)
project(lua-mongo)

set(USE_LUA_VERSION "" CACHE STRING "Build against Lua version 'X.Y' (or 'JIT' for LuaJIT).")
set(LUA_MODULE_PATH "" CACHE STRING "Path for Lua modules (or default if empty).")

if(USE_LUA_VERSION MATCHES "^[0-9]\\.[0-9]$")
	set(lua_ver ${USE_LUA_VERSION})
	string(REGEX REPLACE "\\." "" lua_ver2 ${lua_ver})
	set(lua_names lua-${lua_ver} lua${lua_ver} lua${lua_ver2})
elseif(USE_LUA_VERSION MATCHES "^(jit|JIT)$")
	set(lua_ver 5.1)
	set(lua_names luajit)
elseif(USE_LUA_VERSION STREQUAL "")
	set(lua_names lua)
else()
	message(FATAL_ERROR "Invalid Lua version: ${USE_LUA_VERSION}")
endif()

find_package(libmongoc-1.0 1.9 REQUIRED)
find_package(PkgConfig)
pkg_search_module(LUA REQUIRED ${lua_names})

if(NOT LUA_FOUND)
	message(FATAL_ERROR "Lua not found - set USE_LUA_VERSION to match your configuration")
elseif(NOT lua_ver)
	string(REGEX MATCH "^[0-9]\\.[0-9]" lua_ver ${LUA_VERSION})
	message(STATUS "Lua ${lua_ver} detected (set USE_LUA_VERSION to override)")
endif()

if(NOT LUA_MODULE_PATH)
	set(LUA_MODULE_PATH lib/lua/${lua_ver})
endif()

add_definitions(-Wall -Wextra -Wpedantic -Wundef -Wshadow -Wredundant-decls
	-Wstrict-prototypes -Wmissing-prototypes -Wno-variadic-macros)

include_directories(${MONGOC_INCLUDE_DIRS} ${LUA_INCLUDE_DIRS})
link_directories(${MONGOC_LIBRARY_DIRS})

file(GLOB srcs src/*.c)
add_library(mongo SHARED ${srcs})
target_link_libraries(mongo ${MONGOC_LIBRARIES})
set_target_properties(mongo PROPERTIES PREFIX "")
if(APPLE)
	target_link_libraries(mongo "-undefined dynamic_lookup")
	set_target_properties(mongo PROPERTIES SUFFIX ".so")
endif()

install(TARGETS mongo DESTINATION ${LUA_MODULE_PATH})

enable_testing()
find_program(LUA_COMMAND NAMES ${lua_names})
file(GLOB tests test/test-*.lua)
foreach(test ${tests})
	string(REGEX REPLACE "^.*(test-[^/\\]+\\.lua)$" "\\1" name ${test})
	add_test(${name} ${LUA_COMMAND} ${test})
	set_tests_properties(${name} PROPERTIES ENVIRONMENT "LUA_PATH=${CMAKE_CURRENT_SOURCE_DIR}/test/?.lua")
endforeach()
