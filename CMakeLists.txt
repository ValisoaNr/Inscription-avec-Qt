cmake_minimum_required(VERSION 3.19)
project(Inscription LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(Inscription
    WIN32 MACOSX_BUNDLE
    main.cpp
    Inscription.cpp
    Inscription.h
    Inscription.ui
    Personne.h Personne.cpp
)

target_link_libraries(Inscription
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS Inscription
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Inscription
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
