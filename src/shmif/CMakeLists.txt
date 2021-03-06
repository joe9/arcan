
# This is used by the main arcan build system to define the shmif platform
# and install targets, but also for some external projects that need to
# do an in-source build of shmif- and its platform (automated build/
# test systems etc) without a full installation.
#
# Defines:
# ARCAN_SHMIF_INCLUDE_DIR
# ARCAN_SHMIF_LIBRARY (set to arcan_shmif_int)
#
# Targets:
# arcan_shmif
# arcan_shmif_int
# arcan_shmif_tui
# arcan_shmif_ext
# arcan_shmif_tuiext
# arcan_shmif_intext
#
# Installs: (if ARCAN_SOURCE_DIR is not set)
#
set(ASHMIF_MAJOR 0)
set(ASHMIF_MINOR 9)

if (ARCAN_SOURCE_DIR)
	set(ASD ${ARCAN_SOURCE_DIR})
else()
	set(ASD ${CMAKE_CURRENT_SOURCE_DIR}/../)
endif()

if (STDLIB)
	set(ASHMIF_STDLIB ${STDLIB})
else()
	set(ASHMIF_STDLIB -lrt)
endif()

set(ASHMIF_INSTPATH lib)

set (SHMIF_HEADERS
	${ASD}/shmif/arcan_shmif_control.h
	${ASD}/shmif/arcan_shmif_interop.h
	${ASD}/shmif/arcan_shmif_event.h
	${ASD}/shmif/arcan_shmif_tui.h
	${ASD}/shmif/arcan_shmif.h
)

set (SHMIF_SOURCES
	${SHMIF_HEADERS}
	${ASD}/shmif/arcan_shmif_control.c
	${ASD}/platform/posix/shmemop.c
)

if (LWA_PLATFORM_STR AND IS_DIRECTORY "${ASD}/shmif/${LWA_PLATFORM_STR}" AND
	EXISTS "${ASD}/shmif/${LWA_PLATFORM_STR}/${LWA_PLATFORM_STR}.c")
	set (SHMIF_EXT_SOURCES "${ASD}/shmif/${LWA_PLATFORM_STR}/${LWA_PLATFORM_STR}.c")

else()
	set (SHMIF_EXT_SOURCES "${ASD}/shmif/stub/stub.c")
endif()

if (BSD_BUILD)
	set(SHMIF_PLATFORM
		${ASD}/platform/posix/warning.c
		${ASD}/platform/posix/time.c
		${ASD}/platform/posix/sem.c
		${ASD}/platform/posix/fdpassing.c
	)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(SHMIF_PLATFORM
		${ASD}/platform/posix/warning.c
		${ASD}/platform/posix/time.c
		${ASD}/platform/posix/sem.c
		${ASD}/platform/posix/fdpassing.c
	)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(SHMIF_PLATFORM
		${ASD}/platform/posix/warning.c
		${ASD}/platform/darwin/time.c
		${ASD}/platform/darwin/sem.c
		${ASD}/platform/posix/fdpassing.c
	)
else()
	message(FATAL_ERROR "attempted to build arcan-shmif on an unsupported OS/platform")
endif()

set_property(SOURCE ${ASD}/platform/posix/fdpassing.c
	APPEND PROPERTY COMPILE_FLAGS -w)
set_property(SOURCE ${ASD}/platform/posix/fdpassing.c
	APPEND PROPERTY COMPILE_DEFINITIONS NONBLOCK_RECV)

#
# FIXME:
# We should have a linker script here that filters out all the non-TUI defined
# symbols so we don't collide with programs linking TSM* (however unlikely)
#
set(SHMIF_TUI_SOURCES
	${ASD}/shmif/tui/tui.c
	${ASD}/shmif/tui/tsm_screen.c
	${ASD}/shmif/tui/tsm_unicode.c
	${ASD}/shmif/tui/shl_htable.c
	${ASD}/shmif/tui/wcwidth.c
	${ASD}/engine/arcan_ttf.c
)

set_property(SOURCE ${ASD}/engine/arcan_ttf.c
	APPEND PROPERTY COMPILE_DEFINITIONS SHMIF_TTF)

set(TARGET_LIST arcan_shmif arcan_shmif_int arcan_shmif_ext arcan_shmif_intext arcan_shmif_tui)

add_library(arcan_shmif_int STATIC ${SHMIF_SOURCES} ${SHMIF_PLATFORM})
add_library(arcan_shmif SHARED ${SHMIF_SOURCES} ${SHMIF_PLATFORM})
add_library(arcan_shmif_ext SHARED ${SHMIF_EXT_SOURCES})
add_library(arcan_shmif_intext SHARED ${SHMIF_EXT_SOURCES})

# somewhat messy, if the engine is building a static freetype version we should
# use that one and provide a tui to match, but we should also try and provide a
# dynamic one.
if (STATIC_FREETYPE)
	add_library(arcan_shmif_tui STATIC ${SHMIF_TUI_SOURCES})
	target_link_libraries(arcan_shmif_tui PRIVATE ${STDLIB} ${FREETYPE_DEFAULT_LIBRARIES})
	find_package(Freetype QUIET)
	if (FREETYPE_FOUND)
		add_library(arcan_shmif_tuiext SHARED ${SHMIF_TUI_SOURCES})
		target_link_libraries(arcan_shmif_tuiext PRIVATE ${STDLIB} arcan_shmif ${FREETYPE_LIBRARIES})
		set_target_properties(arcan_shmif_tuiext PROPERTIES
			COMPILE_FLAGS -fPIC
			OUTPUT_NAME arcan_shmif_tui
			VERSION ${ASHMIF_MAJOR}.${ASHMIF_MINOR}
		)
		set(TARGET_LIST ${TARGET_LIST} arcan_shmif_tuiext)
		target_include_directories(arcan_shmif_tuiext PRIVATE
			${INCLUDE_DIRS} ${ASD}/frameserver ${ASD}/shmif)
	else()
		amsg("${CL_YEL}only static freetype found, dynamic shmif_tui disabled ${CL_RST}")
	endif()
else()
	add_library(arcan_shmif_tui SHARED ${SHMIF_TUI_SOURCES})
	target_link_libraries(arcan_shmif_tui PRIVATE ${STDLIB} arcan_shmif ${FREETYPE_LIBRARIES})
	set_target_properties(arcan_shmif_tui PROPERTIES VERSION ${ASHMIF_MAJOR}.${ASHMIF_MINOR})
endif()

target_link_libraries(arcan_shmif PRIVATE ${STDLIB})
target_link_libraries(arcan_shmif_int PRIVATE ${STDLIB})
target_link_libraries(arcan_shmif_ext PRIVATE ${STDLIB} ${VIDEO_LIBRARIES})
target_link_libraries(arcan_shmif_intext PRIVATE ${STDLIB} ${VIDEO_LIBRARIES})

target_include_directories(arcan_shmif_ext PRIVATE ${INCLUDE_DIRS})
target_include_directories(arcan_shmif_intext PRIVATE ${INCLUDE_DIRS})
target_include_directories(arcan_shmif_tui PRIVATE
	${INCLUDE_DIRS} ${ASD}/frameserver ${ASD}/shmif)

# note that we enable fpic for the static here as some
# subdirectores need to pull it in as part of building a shared library
set_target_properties(arcan_shmif_int PROPERTIES
	COMPILE_FLAGS -fPIC
	OUTPUT_NAME arcan_shmif
	VERSION ${ASHMIF_MAJOR}.${ASHMIF_MINOR}
)

if (CMAKE_C_FLAGS)
	string (REGEX REPLACE "-fsanitize=[^ ]+" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
endif()

set_target_properties(arcan_shmif PROPERTIES
	COMPILE_FLAGS -fPIC
	OUTPUT_NAME arcan_shmif
	VERSION ${ASHMIF_MAJOR}.${ASHMIF_MINOR}
	SOVERSION ${ASHMIF_MAJOR}.${ASHMIF_MINOR}
)

set_target_properties(arcan_shmif_ext PROPERTIES
	COMPILE_FLAGS -fPIC
	OUTPUT_NAME arcan_shmif_ext
	VERSION ${ASHMIF_MAJOR}.${ASHMIF_MINOR}
)

set_target_properties(arcan_shmif_intext PROPERTIES
	COMPILE_FLAGS -fPIC
	OUTPUT_NAME arcan_shmif_ext
	VERSION ${ASHMIF_MAJOR}.${ASHMIF_MINOR}
)

set(ARCAN_SHMIF_INCLUDE_DIR ${ASD}/shmif PARENT_SCOPE)
set(ARCAN_SHMIF_LIBRARY arcan_shmif_int ${ASHMIF_STDLIB} PARENT_SCOPE)

target_include_directories(arcan_shmif_int PRIVATE ${ASD}/shmif)
target_include_directories(arcan_shmif PRIVATE ${ASD}/shmif)

if (NOT ARCAN_SOURCE_DIR)
	install(TARGETS ${TARGET_LIST}
		LIBRARY DESTINATION ${ASHMIF_INSTPATH}
		ARCHIVE DESTINATION ${ASHMIF_INSTPATH}
	)
	install(FILES ${SHMIF_HEADERS} DESTINATION include/arcan/shmif)

# The config file is more similar to a regular find module,
# the procedure for doing something this trivial ("install library,
# provide means for other projects to find library") seems excessively
# verbose and 'cmake:y'.
	install(FILES ${PLATFORM_ROOT}/cmake/arcan_shmif-config.cmake
		DESTINATION CMake COMPONENT dev)
endif()
