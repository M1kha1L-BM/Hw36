cmake_minimum_required(VERSION 3.12)

project(Chat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if ( CMAKE_COMPILER_IS_GNUCC )
   set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()
if ( MSVC )
   set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
endif()

set(CHAT_LIBRARIES)

set(Qt6Modules Core Widgets)

foreach(QtModule IN LISTS Qt6Modules)
 list(APPEND CHAT_LIBRARIES Qt6::${QtModule})
endforeach(QtModule)

#здесь проверяется наличие всех необходимых модулей и компонентов Qt
find_package(Qt6 COMPONENTS ${Qt6Modules} REQUIRED) #LinguistTools)
#set(Qt6_LUPDATE_EXECUTABLE "C:/Qt/6.10.0/msvc2022_64/bin/lupdate.exe")
set(PostgreSQL_INCLUDE_DIR "C:/Program Files/PostgreSQL/18/include")
set(PostgreSQL_LIBRARY "C:/Program Files/PostgreSQL/18/lib/libpq.lib")

include_directories(${PostgreSQL_INCLUDE_DIR})
link_libraries(${PostgreSQL_LIBRARY})

file(GLOB_RECURSE SRC_FILES sources/*.cpp)
file(GLOB_RECURSE HEADERS_FILES sources/*.h)
# file(GLOB_RECURSE LIBRARY sources/*.dll)
file(GLOB_RECURSE RC_FILES sources/*.qrc)
file(GLOB_RECURSE UI_FILES sources/*.ui)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

qt6_wrap_ui(QUI ${UI_FILES})
qt6_add_resources(QRC ${RC_FILES})


add_executable(Chat ${QM_FILES} ${SRC_FILES} ${HEADERS_FILES}
 ${RC_FILES} ${UI_FILES})

#подключаем к проекту библиотеки
target_link_libraries(Chat
 PUBLIC
 ${CHAT_LIBRARIES}
 )

