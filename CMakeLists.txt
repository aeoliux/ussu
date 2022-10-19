cmake_minimum_required(VERSION 3.22)
project(runas C)

option(DISABLE_PERSIST "Disable session persist" OFF)

set(CMAKE_C_STANDARD 99)
if (DISABLE_PERSIST)
	add_compile_options(-DPERSIST_DISABLE)
endif()
add_compile_options(-DPREFIX="${CMAKE_INSTALL_PREFIX}")

add_executable(runas main.c util/authorize.c util/authorize.h util/isAllowed.c util/isAllowed.h util/util.c util/util.h util/cleanup.c util/cleanup.h)
target_link_libraries(runas -lcrypt)

install(
		TARGETS runas
		PERMISSIONS SETUID OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ
)

install(
		FILES ${CMAKE_CURRENT_SOURCE_DIR}/runas.conf
		DESTINATION ${CMAKE_INSTALL_PREFIX}/etc
)