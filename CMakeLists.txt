cmake_minimum_required(VERSION 3.22)
project(
		ussu
		LANGUAGES C
		VERSION 1.0.0
		DESCRIPTION "A account authentication tool for POSIX-compatible operating systems"
		HOMEPAGE_URL "https://github.com/zapomnij/ussu"
)

option(DISABLE_PERSIST "Disable session persist" OFF)

set(CMAKE_C_STANDARD 99)
if (DISABLE_PERSIST)
	add_compile_options(-DPERSIST_DISABLE)
endif()
add_compile_options(-DPREFIX="${CMAKE_INSTALL_PREFIX}")

add_executable(ussu main.c util/authorize.c util/authorize.h util/isAllowed.c util/isAllowed.h util/util.c util/util.h util/cleanup.c util/cleanup.h)
target_link_libraries(ussu -lcrypt)

install(
		TARGETS ussu
		PERMISSIONS SETUID OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ
)

install(
		FILES ${CMAKE_CURRENT_SOURCE_DIR}/ussu.conf
		DESTINATION ${CMAKE_INSTALL_PREFIX}/etc
)

install(
		FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
		DESTINATION ${CMAKE_INSTALL_PREFIX}/share/licenses/${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}
)