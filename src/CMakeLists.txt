# SPDX-FileCopyrightText: 2023 Carl Schwan <carlschwan@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

add_library(newsfish_static STATIC)

ecm_qt_declare_logging_category(newsfish_static
    HEADER newsfish_debug.h
    IDENTIFIER NEWSFISH_LOG
    CATEGORY_NAME com.github.piggz.newsfish.ownnews
    DESCRIPTION "NewsFish internal library"
    DEFAULT_SEVERITY Warning
    EXPORT NEWSFISH
)

target_sources(newsfish_static PRIVATE
    Helper.cpp
    feedsmodel.cpp
    itemsmodel.cpp
    itemworker.cpp
    main.cpp
    newsinterface.cpp
)

target_link_libraries(newsfish_static
    PUBLIC
        Qt::Quick
        Qt::Qml
        Qt::Gui
        Qt::Sql
        Qt::Network
        Qt::QuickControls2
        KF${QT_MAJOR_VERSION}::I18n
        KF${QT_MAJOR_VERSION}::Kirigami2
        KF${QT_MAJOR_VERSION}::CoreAddons
)

# TODO add testing
# if (BUILD_TESTING)
#     add_subdirectory(autotests)
# endif()

add_executable(newsfish
    main.cpp
    resources.qrc
)

target_include_directories(newsfish PRIVATE ${CMAKE_BINARY_DIR})
target_link_libraries(newsfish PRIVATE newsfish_static)

if(ANDROID)
    target_link_libraries(newsfish PRIVATE Qt::Svg)
    kirigami_package_breeze_icons(ICONS
        "help-about"
    )
else()
    if(NOT SAILFISHOS)
        target_link_libraries(newsfish_static PUBLIC Qt::Widgets)
    endif()
endif()

if(SAILFISHOS)
    target_compile_definitions(newsfish_static PRIVATE -DQ_OS_SAILFISHOS)
endif()

install(TARGETS newsfish ${KF${QT_MAJOR_VERSION}_INSTALL_TARGETS_DEFAULT_ARGS})
