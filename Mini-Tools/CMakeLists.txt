cmake_minimum_required(VERSION 3.19)
project(Mini-Tools LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Multimedia)

qt_standard_project_setup()

qt_add_executable(Mini-Tools
    WIN32 MACOSX_BUNDLE
    main.cpp
    deploywidget.h deploywidget.cpp deploywidget.ui
    core.h core.cpp core.ui
    chatformatwidget.h chatformatwidget.cpp chatformatwidget.ui
    resource.rc
    arithmeticwidget.h arithmeticwidget.cpp arithmeticwidget.ui
    tictactoewidget.h tictactoewidget.cpp tictactoewidget.ui
    blackjackwidget.h blackjackwidget.cpp blackjackwidget.ui
    breakoutwidget.h breakoutwidget.cpp breakoutwidget.ui
    cprlib.h
    cprlib.cpp
    translationwidget.h translationwidget.cpp translationwidget.ui
)

target_link_libraries(Mini-Tools
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Multimedia
)

include(GNUInstallDirs)

install(TARGETS Mini-Tools
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Mini-Tools
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
