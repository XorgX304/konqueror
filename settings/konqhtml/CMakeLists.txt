add_definitions(-DTRANSLATION_DOMAIN=\"kcmkonqhtml\")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/css
    )


set(kcm_konqhtml_PART_SRCS 
   generalopts.cpp
   htmlopts.cpp 
   jsopts.cpp 
   javaopts.cpp 
   appearance.cpp 
   policydlg.cpp 
   main.cpp 
   jspolicies.cpp 
   policies.cpp 
   domainlistview.cpp 
   filteropts.cpp
   css/template.cpp
   css/kcmcss.cpp
   )

ki18n_wrap_ui(kcm_konqhtml_PART_SRCS
    advancedTabOptions.ui
    css/csscustom.ui
    css/cssconfig.ui
    )

add_library(kcm_konqhtml MODULE ${kcm_konqhtml_PART_SRCS})

target_link_libraries(kcm_konqhtml KF5::Parts KF5::ItemViews)

install(TARGETS kcm_konqhtml  DESTINATION ${KDE_INSTALL_PLUGINDIR})

########### install files ###############

install( FILES khtml_general.desktop khtml_behavior.desktop khtml_java_js.desktop khtml_appearance.desktop khtml_filter.desktop  DESTINATION  ${KDE_INSTALL_KSERVICES5DIR} )
install( FILES css/template.css  DESTINATION  ${KDE_INSTALL_DATADIR}/kcmcss )
