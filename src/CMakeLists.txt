FILE(GLOB FOLDER_SOURCES *.c*)
FILE(GLOB FOLDER_HEADERS *.h*)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
SET(TARGET_NAME ${PROJECT_NAME})

link_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DRASPBERRY_PI -DBCM2835_NO_DELAY_COMPATIBILITY)

include_directories(${crocore_INCLUDE_DIRS})
add_executable(${TARGET_NAME} ${FOLDER_SOURCES} ${FOLDER_HEADERS})
target_link_libraries(${TARGET_NAME} ${LIBS})

