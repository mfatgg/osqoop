set(osqoop_SRCS
	SignalDisplayData.cpp
	SignalViewWidget.cpp
	DataConverter.cpp
	OscilloscopeWindow.cpp
	Osqoop.cpp
	Utilities.cpp
	ProcessingPluginDialog.cpp
	FFTWidget.cpp
)
qt4_automoc(${osqoop_SRCS})
qt4_add_resources(osqoop_RESS Osqoop.qrc)
add_executable(osqoop ${osqoop_SRCS} ${osqoop_RESS})
include_directories (${CMAKE_BINARY_DIR}/src)
include_directories (${CMAKE_SOURCE_DIR}/processing/lib)
include_directories (${QWT_INCLUDE_DIR})
target_link_libraries(osqoop ${QT_LIBRARIES} ${QWT_LIBRARY})
install(TARGETS osqoop RUNTIME DESTINATION bin)
