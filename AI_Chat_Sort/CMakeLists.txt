cmake_minimum_required(VERSION 3.19)
project(AI_Chat_Sort LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(AI_Chat_Sort
    WIN32 MACOSX_BUNDLE
    main.cpp
    dialogformatter.h dialogformatter.cpp dialogformatter.ui



)

target_link_libraries(AI_Chat_Sort
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS AI_Chat_Sort
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET AI_Chat_Sort
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
