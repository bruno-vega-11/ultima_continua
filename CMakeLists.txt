cmake_minimum_required(VERSION 3.31)
project(Sem14_Code2)

set(CMAKE_CXX_STANDARD 20)

add_executable(Sem14_Code2 main.cpp
                        ast.cpp
                        parser.cpp
                        scanner.cpp
                        token.cpp
                        visitor.cpp
                        )
