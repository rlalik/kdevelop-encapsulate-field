set(kdevencapsulatefield_SRCS
    encapsulatefield.cpp
    parameters.cpp
    pattern.cpp
    patternmodel.cpp

    config/globalconfigpage.cpp
    config/projectconfigpage.cpp
)

kconfig_add_kcfg_files(kdevencapsulatefield_SRCS
    config/globalsettings.kcfgc
    config/projectsettings.kcfgc
)

ecm_qt_declare_logging_category(kdevencapsulatefield_SRCS
    HEADER debug.h
    IDENTIFIER PLUGIN_KDEVENCAPSULATEFIELD
    CATEGORY_NAME "kdevelop.plugin.encapsulatefield"
)

ki18n_wrap_ui(kdevencapsulatefield_SRCS
    config/globalconfigpage.ui
    config/projectconfigpage.ui
)

kdevplatform_add_plugin(kdevencapsulatefield
    JSON kdevencapsulatefield.json
    SOURCES ${kdevencapsulatefield_SRCS}
)

target_link_libraries(kdevencapsulatefield
    KDev::Interfaces
    KDev::Project
    KDev::Language
)
