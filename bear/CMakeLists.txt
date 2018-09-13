set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets Qml Quick QuickControls2 REQUIRED)

set(
	BEAR_SOURCES
	main.cpp
	editor.hpp
	editor.cpp
	window.hpp
	window.cpp
	project.hpp
	project.cpp
	document.hpp document.cpp texteditor.hpp texteditor.cpp)

add_executable(bear qml.qrc monoidFont.qrc ${BEAR_SOURCES})

target_link_libraries(bear Qt5::Widgets Qt5::Qml Qt5::Quick Qt5::QuickControls2 stdc++fs purson Qt5::Widgets)
