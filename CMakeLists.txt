cmake_minimum_required(VERSION 3.10)

project(log-malloc2 VERSION 0.4.1 DESCRIPTION "${PROJECT_NAME}")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(Unwind)

option(HAVE_UNWIND_DETAIL "Use libunwind detail" ON)

add_library(${PROJECT_NAME} SHARED src/${PROJECT_NAME}_api.c src/${PROJECT_NAME}.c)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(Unwind_FOUND)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_UNWIND)
    target_link_libraries(${PROJECT_NAME} PRIVATE unwind::unwind)
    if(HAVE_UNWIND_DETAIL)
        target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_UNWIND_DETAIL)
    endif()
endif()

add_subdirectory(examples)

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES include/${PROJECT_NAME}.h include/${PROJECT_NAME}_util.h DESTINATION include)

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