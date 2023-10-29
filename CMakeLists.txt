cmake_minimum_required(VERSION 3.14)

project(my_back LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Sql Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Sql Network)

# заголовочные файлы тоже следует добавлять.
# так они будут отображаться во всяких иде как часть проекта.
# + менее вероятность что смейк пропустит их изменения для правил сборки
add_executable(my_back
  main.cpp
  database_connection.cpp
  template_query.cpp
  json_creator.cpp
  test_server.cpp
  socket_adapter.cpp

)
target_link_libraries(my_back Qt${QT_VERSION_MAJOR}::Core Qt6::Sql Qt6::Network)


install(TARGETS my_back
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
