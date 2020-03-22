if(BUILD_VIEW)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif(BUILD_VIEW)

find_package(PkgConfig REQUIRED)
find_package(Boost 1.58 REQUIRED COMPONENTS program_options)
find_package(jsoncpp REQUIRED)

if(BUILD_VIEW)
    find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets OpenGL Svg)
    find_package(CGAL REQUIRED COMPONENTS CGAL Core Qt5)
endif(BUILD_VIEW)

set(SOURCES decomp.cpp file_io.cpp main.cpp mesh.cpp view.cpp)
set(UIS mcd-trap-decomp.ui)
set(RESOURCES mcd-trap-decomp.qrc)

add_executable(mcd-trap-decomp ${SOURCES} ${UIS} ${RESOURCES})

set_target_properties(mcd-trap-decomp PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

target_link_libraries(mcd-trap-decomp Boost::program_options)
target_link_libraries(mcd-trap-decomp jsoncpp)
target_link_libraries(mcd-ref-merge triangle)

if(BUILD_VIEW)
    target_link_libraries(mcd-trap-decomp CGAL::CGAL CGAL::CGAL_Core CGAL::CGAL_Qt5)
    target_link_libraries(mcd-trap-decomp Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL Qt5::Svg)
endif(BUILD_VIEW)
