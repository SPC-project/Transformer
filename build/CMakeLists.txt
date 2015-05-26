# Для сборки проекта перейдите в директорию Transformer (родительская для этой)
#   и выполните в командной строке сначала 'cmake .' (под ОС Linux),
#   а потом 'make'
# Вы также можете собрать проект под Code::Blocks, выполнив 
#   'cmake . --DCMAKE_BUILD_TYPE=Debug' - это сформирует под эту IDE

# Переменные для сборки проекта
set(headers ../include)
set(resources ../res)
set(sources ../src)
file(GLOB code 
	${sources}/*.cpp	
)
# Необходимо явно подключить заголовочные файлы инструкцией add_executable
# 	в случае, если они лежат не в той же папке, что и CMakeLists.txt
file(GLOB code_h
	${headers}/*.h
)

include_directories(${headers} ${CMAKE_CURRENT_BINARY_DIR})

#find_package( Qt4 REQUIRED )

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt4 REQUIRED)
qt4_wrap_ui(wrapped_ui ${resources}/transgui.ui)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

add_executable( ${prog_name} ${code} ${code_h} ${wrapped_ui})
target_link_libraries( ${prog_name} ${QT_LIBRARIES} )

IF(WIN32) # Check if we are on Windows
  if(MSVC) # Check if we are using the Visual Studio compiler
    set_target_properties(${prog_name} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
  elseif(CMAKE_COMPILER_IS_GNUCXX)
    # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows") # Not tested
  else()
    message(SEND_ERROR "You are using an unsupported Windows compiler! (Not MSVC or GCC)")
  endif()
elseif(UNIX)
  # Nothing special required
else()
  message(SEND_ERROR "You are on an unsupported platform! (Not Win32 or Unix)")
ENDIF()
