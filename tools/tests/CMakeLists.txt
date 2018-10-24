# targets
macro(GEN name)
    add_executable(test_${name} test_${name}.cpp)
    target_link_libraries(test_${name} PUBLIC tools)
endmacro()

GEN(chain)
GEN(singleton)
GEN(coroutine)
GEN(concurrentqueue)
GEN(threadpool)
GEN(delaytaskqueue)
GEN(taskgroup)
GEN(reflection)
