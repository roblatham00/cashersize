# set source files
set (server-src-files
     Provider.cpp
     Backend.cpp)

set (client-src-files
     Client.cpp
     CacheHandle.cpp
     AsyncRequest.cpp)

set (admin-src-files
     Admin.cpp)

set (dummy-src-files
     dummy/DummyBackend.cpp)

set (module-src-files
     BedrockModule.cpp)

# load package helper for generating cmake CONFIG packages
include (CMakePackageConfigHelpers)

# where to install files for "find_package"
set (cachersize-pkg "share/cmake/cachersize")

# set version fo shared library
set (cachersize-vers "${CACHERSIZE_VERSION_MAJOR}.${CACHERSIZE_VERSION_MINOR}")

# server library
add_library (cachersize-server ${server-src-files} ${dummy-src-files})
target_link_libraries (cachersize-server
    thallium
    PkgConfig::ABTIO
    PkgConfig::UUID
    nlohmann_json::nlohmann_json)
target_include_directories (cachersize-server PUBLIC $<INSTALL_INTERFACE:include>)
target_include_directories (cachersize-server BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
target_include_directories (cachersize-server BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
set_target_properties (cachersize-server
    PROPERTIES VERSION ${CACHERSIZE_VERSION}
    SOVERSION ${CACHERSIZE_VERSION_MAJOR})

# client library
add_library (cachersize-client ${client-src-files})
target_link_libraries (cachersize-client thallium PkgConfig::UUID)
target_include_directories (cachersize-client PUBLIC $<INSTALL_INTERFACE:include>)
target_include_directories (cachersize-client BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
target_include_directories (cachersize-client BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
set_target_properties (cachersize-client
    PROPERTIES VERSION ${CACHERSIZE_VERSION}
    SOVERSION ${CACHERSIZE_VERSION_MAJOR})

# admin library
add_library (cachersize-admin ${admin-src-files})
target_link_libraries (cachersize-admin thallium PkgConfig::UUID nlohmann_json::nlohmann_json)
target_include_directories (cachersize-admin PUBLIC $<INSTALL_INTERFACE:include>)
target_include_directories (cachersize-admin BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
target_include_directories (cachersize-admin BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
set_target_properties (cachersize-admin
    PROPERTIES VERSION ${CACHERSIZE_VERSION}
    SOVERSION ${CACHERSIZE_VERSION_MAJOR})

if (${ENABLE_BEDROCK})
# bedrock module library
add_library (cachersize-bedrock-module ${module-src-files})
target_link_libraries (cachersize-bedrock-module cachersize-server cachersize-admin cachersize-client)
target_include_directories (cachersize-bedrock-module PUBLIC $<INSTALL_INTERFACE:include>)
target_include_directories (cachersize-bedrock-module BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
target_include_directories (cachersize-bedrock-module BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
set_target_properties (cachersize-bedrock-module
    PROPERTIES VERSION ${CACHERSIZE_VERSION}
    SOVERSION ${CACHERSIZE_VERSION_MAJOR})
endif ()

# installation stuff (packaging and install commands)
write_basic_package_version_file(
    "cachersize-config-version.cmake"
    VERSION ${CACHERSIZE_VERSION}
    COMPATIBILITY AnyNewerVersion)

# generate our config file for find_package()
set(INCLUDE_INSTALL_DIR include/)
set(LIB_INSTALL_DIR lib/)
configure_package_config_file(cachersize-config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cachersize-config.cmake
  INSTALL_DESTINATION ${LIB_INSTALL_DIR}/cachersize/cmake
  PATH_VARS INCLUDE_INSTALL_DIR)

# some bits for the pkg-config file
set (DEST_DIR "${CMAKE_INSTALL_PREFIX}")
set (SERVER_PRIVATE_LIBS "-lcachersize-server")
set (CLIENT_PRIVATE_LIBS "-lcachersize-client")
set (ADMIN_PRIVATE_LIBS  "-lcachersize-admin")
configure_file ("cachersize-server.pc.in" "cachersize-server.pc" @ONLY)
configure_file ("cachersize-client.pc.in" "cachersize-client.pc" @ONLY)
configure_file ("cachersize-admin.pc.in"  "cachersize-admin.pc"  @ONLY)

# configure config.h
configure_file ("config.h.in" "config.h" @ONLY) 

# "make install" rules
install (TARGETS cachersize-admin cachersize-server cachersize-client
         EXPORT cachersize-targets
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
if (${ENABLE_BEDROCK})
    install (TARGETS cachersize-bedrock-module
             ARCHIVE DESTINATION lib
             LIBRARY DESTINATION lib)
endif ()
install (EXPORT cachersize-targets
         DESTINATION ${cachersize-pkg}
         FILE "cachersize-targets.cmake")
install (FILES "${CMAKE_CURRENT_BINARY_DIR}/cachersize-config.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/cachersize-config-version.cmake"
         DESTINATION ${cachersize-pkg} )
install (DIRECTORY ../include/cachersize
         DESTINATION include
         FILES_MATCHING PATTERN "*.h")
install (FILES "${CMAKE_CURRENT_BINARY_DIR}/cachersize-server.pc"
               "${CMAKE_CURRENT_BINARY_DIR}/cachersize-client.pc"
               "${CMAKE_CURRENT_BINARY_DIR}/cachersize-admin.pc"
         DESTINATION "lib/pkgconfig/")
