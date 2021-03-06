# Copyright (c) 2010-2021 The Open-Transactions developers
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_subdirectory(client)
add_subdirectory(crypto)
add_subdirectory(network)
add_subdirectory(server)
add_subdirectory(storage)

add_library(
  opentxs-api OBJECT
  "${opentxs_SOURCE_DIR}/include/opentxs/api/Legacy.hpp"
  "${opentxs_SOURCE_DIR}/src/internal/api/Api.hpp"
  "Context.cpp"
  "Context.hpp"
  "Core.cpp"
  "Core.hpp"
  "Endpoints.cpp"
  "Endpoints.hpp"
  "Factory.cpp"
  "Factory.hpp"
  "HDSeed.cpp"
  "HDSeed.hpp"
  "Legacy.cpp"
  "Legacy.hpp"
  "Log.cpp"
  "Log.hpp"
  "Periodic.cpp"
  "Periodic.hpp"
  "Primitives.cpp"
  "Primitives.hpp"
  "Scheduler.cpp"
  "Scheduler.hpp"
  "Settings.cpp"
  "Settings.hpp"
  "StorageParent.cpp"
  "StorageParent.hpp"
  "ThreadPool.cpp"
  "ThreadPool.hpp"
  "Wallet.cpp"
  "Wallet.hpp"
  "ZMQ.cpp"
  "ZMQ.hpp"
)
set(cxx-install-headers
    "${opentxs_SOURCE_DIR}/include/opentxs/api/Context.hpp"
    "${opentxs_SOURCE_DIR}/include/opentxs/api/Core.hpp"
    "${opentxs_SOURCE_DIR}/include/opentxs/api/Editor.hpp"
    "${opentxs_SOURCE_DIR}/include/opentxs/api/Endpoints.hpp"
    "${opentxs_SOURCE_DIR}/include/opentxs/api/Factory.hpp"
    "${opentxs_SOURCE_DIR}/include/opentxs/api/HDSeed.hpp"
    "${opentxs_SOURCE_DIR}/include/opentxs/api/Periodic.hpp"
    "${opentxs_SOURCE_DIR}/include/opentxs/api/Primitives.hpp"
    "${opentxs_SOURCE_DIR}/include/opentxs/api/Settings.hpp"
    "${opentxs_SOURCE_DIR}/include/opentxs/api/ThreadPool.hpp"
    "${opentxs_SOURCE_DIR}/include/opentxs/api/Wallet.hpp"
)

target_link_libraries(opentxs-api PRIVATE Boost::headers opentxs::messages)
target_link_libraries(opentxs PUBLIC Boost::filesystem)
target_include_directories(
  opentxs-api SYSTEM PRIVATE "${opentxs_SOURCE_DIR}/deps/"
)

if(OT_ENABLE_RPC)
  target_compile_definitions(opentxs-api PRIVATE OT_RPC=1)
else()
  target_compile_definitions(opentxs-api PRIVATE OT_RPC=0)
endif()

target_sources(opentxs-api PRIVATE ${cxx-install-headers})
target_sources(opentxs PRIVATE $<TARGET_OBJECTS:opentxs-api>)
add_dependencies(opentxs-api generated_code)

if(OPENTXS_STANDALONE)
  install(
    FILES ${cxx-install-headers}
    DESTINATION include/opentxs/api
    COMPONENT dev
  )
endif()
