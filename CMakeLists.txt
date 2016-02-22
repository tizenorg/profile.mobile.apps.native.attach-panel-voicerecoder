CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(ug-attach-panel-voicerecorder C)

IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF("${CMAKE_BUILD_TYPE}" STREQUAL "")
MESSAGE("Build type: ${CMAKE_BUILD_TYPE}")

SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(LIBDIR "${PREFIX}/lib")
SET(RESDIR "${PREFIX}/res")
SET(DATADIR "${PREFIX}/data")
SET(LOCALEDIR "${RESDIR}/locale")
SET(IMAGEDIR "${RESDIR}/images")
SET(EDJEDIR "${RESDIR}/edje")
SET(ETCDIR "${RESDIR}/etc")

SET(VERSION 1.0)

ADD_DEFINITIONS("-D_USE_DLOG_")
ADD_DEFINITIONS("-D_USE_SECURE_LOG_")
ADD_DEFINITIONS("-D_USE_ROTATE_BG_GE")
ADD_DEFINITIONS("-DLOCALEDIR=\"${LOCALEDIR}\"")
#ADD_DEFINITIONS("-D_USE_CUSTOMIZED_GENGRID_STYLE")
#ADD_DEFINITIONS("-D_USE_SHOW_SELECTED_COUNT_IN_TITLE")
#ADD_DEFINITIONS("-D_USE_ADD_TO_HOME_FOR_IMAGES")

file(GLOB UG_SRCS
	"src/*.c"
	"src/utils/*.c"
	"src/view/*.c"
)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/inc
					${CMAKE_CURRENT_SOURCE_DIR}/inc/utils
					${CMAKE_CURRENT_SOURCE_DIR}/inc/view
					)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED
	elementary
	edje
	evas
	ecore
	dlog
	efl-extension
	capi-media-recorder
	capi-appfw-application
	ui-gadget-1
	capi-content-media-content
	libtzplatform-config
)

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fvisibility=hidden")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g -fpie")
SET(CMAKE_C_FLAGS_RELEASE "-O2 -fpie")
SET(CMAKE_LDFLAGS "-Wl,-zdefs" )

ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")

FIND_PROGRAM(UNAME NAMES uname)
EXEC_PROGRAM("${UNAME}" ARGS "-m" OUTPUT_VARIABLE "ARCH")
IF("${ARCH}" STREQUAL "arm")
	ADD_DEFINITIONS("-DTARGET")
	MESSAGE("add -DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")

#SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,--as-needed")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -pie")

ADD_LIBRARY(${PROJECT_NAME} SHARED ${UG_SRCS})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES SOVERSION 0.0.1)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${pkgs_LDFLAGS} "-ldl")

# EDJ
ADD_CUSTOM_COMMAND(OUTPUT audio_recorder.edj
                COMMAND edje_cc -id ${CMAKE_CURRENT_SOURCE_DIR}/res/images/
				-sd  ${CMAKE_CURRENT_SOURCE_DIR}/res/sounds
                ${CMAKE_CURRENT_SOURCE_DIR}/res/edje/audio_recorder.edc
				${CMAKE_BINARY_DIR}/audio_recorder.edj
                DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/res/edje/audio_recorder.edc
)


ADD_CUSTOM_TARGET(audio_recorder_edj_build DEPENDS audio_recorder.edj)
ADD_DEPENDENCIES(${PROJECT_NAME} audio_recorder_edj_build)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${LIBDIR})

INSTALL(FILES ${CMAKE_BINARY_DIR}/audio_recorder.edj DESTINATION ${RESDIR}/edje/${PROJECT_NAME}/)

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/res/images/ DESTINATION ${RESDIR}/images/attach-panel-voicerecorder)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/attach-panel-voicerecorder.xml DESTINATION ${TZ_SYS_RO_PACKAGES}/)

# i18n
ADD_SUBDIRECTORY(res/po)

# Install appcontrol icon
#INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/icon/attach-panel-voicerecorder.png DESTINATION /usr/ug/res/images/attach-panel-voicerecorder)

