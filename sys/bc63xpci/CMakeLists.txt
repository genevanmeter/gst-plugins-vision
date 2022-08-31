set (SOURCES
  gstbc63xclock.c)
    
set (HEADERS
  gstbc63xclock.h)

include_directories (AFTER
  ${BC63XPCI_INCLUDE_DIR}
)

link_directories(${BC63XPCI_LIBRARY_DIR})

set (libname gstbc63xclock)

add_library (${libname} SHARED
  ${SOURCES}
  ${HEADERS})

  set (LIBRARIES
  ${GLIB2_LIBRARIES}
  ${GOBJECT_LIBRARIES}
  ${GSTREAMER_LIBRARY}
  ${GSTREAMER_BASE_LIBRARY}
  ${BC63XPCI_LIBRARY}
  )

  target_link_libraries (${libname}
  ${LIBRARIES}
  )


add_executable(bc63x_demo app/demo.c)
target_link_libraries (bc63x_demo
  ${libname}
)

if (WIN32)
  install (FILES $<TARGET_PDB_FILE:${libname}> DESTINATION ${PDB_INSTALL_DIR} COMPONENT pdb OPTIONAL)
endif ()
install(TARGETS ${libname} LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR})
