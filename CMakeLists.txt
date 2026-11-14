cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0167 NEW)
project(Kettle LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#################################


find_package(Boost 1.83.0 REQUIRED COMPONENTS program_options)


#################################

add_executable(${PROJECT_NAME}
    src/main.cpp
    #Здесь перечисление файлов
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Boost::program_options
)