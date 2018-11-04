cmake_minimum_required(VERSION 2.8.12)

project(EncapsulateField)

# set_package_properties(Cppcheck PROPERTIES
#     DESCRIPTION "An encapsulate field tool for C++ code"
#     URL "missing"
#     PURPOSE "Make quick get/set functions for class members"
#     TYPE RUNTIME
# )

find_package(ECM "5.14.0" REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMQtDeclareLoggingCategory)
include(FeatureSummary)

find_package(KDevPlatform 5.1.40 REQUIRED)
find_package(KF5 REQUIRED COMPONENTS
    Config
)

add_definitions(-DTRANSLATION_DOMAIN=\"kdevencapsulatefield\")

add_subdirectory(src)

# kdebugsettings file
install(FILES kdevencapsulatefield.categories DESTINATION ${KDE_INSTALL_CONFDIR})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
