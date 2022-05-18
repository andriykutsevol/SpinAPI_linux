cmake_minimum_required(VERSION 3.6)

project(interpreter-build)
add_subdirectory(src)

install(FILES interpreter.cfg CMakeLists.txt DESTINATION interpreter/)
install(DIRECTORY doc/ DESTINATION interpreter/doc)
install(DIRECTORY examples/ DESTINATION interpreter/examples)
install(DIRECTORY src/ DESTINATION interpreter/src)

