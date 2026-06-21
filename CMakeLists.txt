cmake_minimum_required(VERSION 3.20)

project(Assignment3)

set(CMAKE_CXX_STANDARD 23)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

add_executable(Assignment3
    Action.cpp
    Animation.cpp
    Assets.cpp
    main.cpp 
    Entity.cpp 
    EntityManager.cpp 
    GameEngine.cpp
    Physics.cpp
    Scene.cpp
    Scene_Menu.cpp
    Scene_Play.cpp
    vec2.cpp

    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp

    external/imgui-sfml/imgui-SFML.cpp
)

target_include_directories(Assignment3 PRIVATE
    external/imgui
    external/imgui-sfml
)

target_link_libraries(Assignment3 PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System

    opengl32
)