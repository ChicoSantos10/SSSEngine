find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(PY_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/generateClangTidy.py)

execute_process(
    COMMAND ${Python3_EXECUTABLE} ${PY_SCRIPT}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
