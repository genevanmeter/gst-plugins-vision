include(GNUInstallDirs)

find_library(BC63XPCI_LIBRARY
  NAMES BC637PCI
  HINTS "C:/Program Files (x86)/Microchip BC63x/bc63xPCIe-V2 Software Developers Kit"
  PATH_SUFFIXES "Lib/x64")

find_path(BC63XPCI_INCLUDE_DIR
  NAMES BC637PCI.h
  HINTS "C:/Program Files (x86)/Microchip BC63x/bc63xPCIe-V2 Software Developers Kit"
  PATH_SUFFIXES "Include/")
  
find_file(BC63XPCI_RUNTIME_LIBRARY
  NAMES BC637PCI.dll
  HINTS "C:/Program Files (x86)/Microchip BC63x/bc63xPCIe-V2 Software Developers Kit"
  PATH_SUFFIXES "Bin/x64")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(BC63XPCI DEFAULT_MSG
                                    BC63XPCI_LIBRARY
                                    BC63XPCI_INCLUDE_DIR
                                    )

if(BC63XPCI_FOUND)
  if(NOT TARGET BC63XPCI::BC63XPCI)
    if(WIN32 AND BC63XPCI_RUNTIME_LIBRARY)
      add_library(BC63XPCI::BC63XPCI SHARED IMPORTED)
      set_target_properties(BC63XPCI::BC63XPCI
        PROPERTIES
        IMPORTED_IMPLIB "${BC63XPCI_LIBRARY}"
        IMPORTED_LOCATION "${BC63XPCI_RUNTIME_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${BC63XPCI_INCLUDE_DIR}"  
      )
    else()
      add_library(BC63XPCI::BC63XPCI SHARED IMPORTED)
      set_target_properties(BC63XPCI::BC63XPCI PROPERTIES
            IMPORTED_IMPLIB "${BC63XPCI_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${BC63XPCI_INCLUDE_DIR}"  
      IMPORTED_LOCATION "${BC63XPCI_LIBRARY}"
      )
    endif()
  endif()
endif()

if(WIN32)
mark_as_advanced(BC63XPCI_LIBRARY
BC63XPCI_RUNTIME_LIBRARY
BC63XPCI_INCLUDE_DIR)
else()
mark_as_advanced(BC63XPCI_LIBRARY
BC63XPCI_INCLUDE_DIR)
endif()
