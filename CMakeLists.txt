# SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16)

set(PROJECT_VERSION 1.0)
project(newsFish VERSION ${PROJECT_VERSION})

option(SAILFISHOS "Build for sailfishOS" OFF)

set(KF_MIN_VERSION "5.100.0")
set(QT_MIN_VERSION "5.15.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FeatureSummary)
include(ECMSetupVersion)
include(KDEInstallDirs)
include(ECMQMLModules)
include(KDEClangFormat)
include(ECMQtDeclareLoggingCategory)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddAppIcon)
include(ECMAddTests)
include(KDEGitCommitHooks)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX NEWSFISH
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/newsfish-version.h
)

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} NO_MODULE COMPONENTS Core Quick Gui QuickControls2 Test Sql)
set_package_properties(Qt${QT_MAJOR_VERSION} PROPERTIES
    TYPE REQUIRED
    PURPOSE "Basic application components"
)
find_package(KF${QT_MAJOR_VERSION} ${KF_MIN_VERSION} COMPONENTS Kirigami2 I18n CoreAddons)
set_package_properties(KF${QT_MAJOR_VERSION} PROPERTIES
    TYPE REQUIRED
    PURPOSE "Basic application components"
)
set_package_properties(KF${QT_MAJOR_VERSION}Kirigami2 PROPERTIES
    TYPE REQUIRED
    PURPOSE "Kirigami application UI framework"
)
find_package(KF${QT_MAJOR_VERSION}KirigamiAddons 0.8.0 REQUIRED)

if (NOT ANDROID AND NOT SAILFISHOS)
    find_package(KF${QT_MAJOR_VERSION}QQC2DesktopStyle REQUIRED)
    find_package(KF${QT_MAJOR_VERSION}DBusAddons REQUIRED)
    find_package(Qt${QT_MAJOR_VERSION}Svg REQUIRED)
    find_package(Qt${QT_MAJOR_VERSION}Keychain)
    set_package_properties(Qt${QT_MAJOR_VERSION}Keychain PROPERTIES
        TYPE REQUIRED
        PURPOSE "Secure storage of account secrets"
    )
endif()

add_definitions(-DQT_NO_FOREACH -DQT_NO_KEYWORDS)

add_subdirectory(src)

ki18n_install(po)

if (ANDROID)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/android/version.gradle.in ${CMAKE_BINARY_DIR}/version.gradle)
    kirigami_package_breeze_icons(ICONS bookmarks)
endif()

if (SAILFISHOS)
    install(FILES uk.co.piggz.newsfish.sailfish.desktop DESTINATION ${KDE_INSTALL_APPDIR})
else()
    install(FILES uk.co.piggz.newsfish.desktop DESTINATION ${KDE_INSTALL_APPDIR})
endif()

install(FILES uk.co.piggz.newsfish.png DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/96x96/apps)
# install(FILES uk.co.piggz.newsFish.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

ecm_qt_install_logging_categories(
    EXPORT NEWSFISH
    FILE newsfish.categories
    SORT DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES src/*.cpp src/*.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
