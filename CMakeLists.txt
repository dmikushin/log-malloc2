cmake_minimum_required(VERSION 3.10)

project(log-malloc2 VERSION 0.4.1 DESCRIPTION "${PROJECT_NAME}")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(Unwind REQUIRED)
find_package(Backtrace REQUIRED)
find_package(Threads REQUIRED)

option(HAVE_UNWIND_DETAIL "Use libunwind detail" ON)

add_library(${PROJECT_NAME} SHARED src/${PROJECT_NAME}_api.c src/${PROJECT_NAME}.c)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${Backtrace_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_UNWIND HAVE_BACKTRACE HAVE_BACKTRACE_SYMBOLS_FD)
target_link_libraries(${PROJECT_NAME} PRIVATE unwind::unwind ${Backtrace_LIBRARIES} Threads::Threads)
if(HAVE_UNWIND_DETAIL)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_UNWIND_DETAIL)
endif()

add_subdirectory(examples)

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(PROGRAMS
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts/log-malloc
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts/log-malloc-trackusage
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts/log-malloc-findleak
    DESTINATION ${CMAKE_INSTALL_BINDIR})

install(FILES include/${PROJECT_NAME}.h include/${PROJECT_NAME}_util.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION 1)

configure_file(${PROJECT_NAME}.pc.in ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

execute_process(COMMAND perl -e "print substr $^V, 1;" OUTPUT_VARIABLE PERL_VERSION)
execute_process(COMMAND perl -e "print join($/,@INC);" OUTPUT_VARIABLE PERL_PATHS)
string(REPLACE "\n" ";" PERL_PATHS_LIST ${PERL_PATHS})

foreach(item ${PERL_PATHS_LIST})
     string(REGEX MATCH "${CMAKE_INSTALL_PREFIX}.*perl.*${PERL_VERSION}" PERL_INSTALL_PREFIX ${item})
     if(PERL_INSTALL_PREFIX)
         break()
     endif()
endforeach()

if(PERL_INSTALL_PREFIX)
    message(STATUS "Perl installation prefix: ${PERL_INSTALL_PREFIX}")
else()
    message(FATAL_ERROR "Cannot find a suitable Perl installation prefix that starts with ${CMAKE_INSTALL_PREFIX} and contains Perl version number ${PERL_VERSION} among the following options:\n${PERL_PATHS_LIST}")
endif()

configure_file(scripts/log-malloc.pm.in ${CMAKE_CURRENT_BINARY_DIR}/log-malloc.pm @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/log-malloc.pm DESTINATION ${PERL_INSTALL_PREFIX})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/scripts/backtrace2line.pl DESTINATION ${PERL_INSTALL_PREFIX})
