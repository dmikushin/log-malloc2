set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")

set(EXAMPLES leak-01 leak-02 leak-03 api-01 segv-01)

foreach(EXAMPLE ${EXAMPLES})
    add_executable(${EXAMPLE} ${EXAMPLE}.c)
    target_link_libraries(${EXAMPLE} log-malloc2)
endforeach(EXAMPLE)

