# Copyright (c) 2010-2021 The Open-Transactions developers
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

protobuf_generate_cpp(
  cxx-sources
  cxx-headers
  Bitcoin.proto
  Generics.proto
  Markets.proto
  Moneychanger.proto
)

add_library(opentxs-core-otprotob OBJECT ${cxx-sources} ${cxx-headers})
target_link_libraries(opentxs-core-otprotob PUBLIC protobuf::libprotobuf-lite)
target_link_libraries(opentxs PUBLIC protobuf::libprotobuf-lite)
target_include_directories(
  opentxs-core-otprotob SYSTEM PUBLIC "${CMAKE_CURRENT_BINARY_DIR}"
)

if(MSVC)
  target_compile_options(opentxs-core-otprotob PRIVATE "/wd4244" "/wd4267")
else()
  target_compile_options(opentxs-core-otprotob PRIVATE "-Wno-effc++")
endif()

if(${CMAKE_CXX_COMPILER_ID}
   MATCHES
   Clang
)
  target_compile_options(
    opentxs-core-otprotob PRIVATE -Wno-extra-semi
                                  -Wno-suggest-destructor-override
  )
endif()

target_sources(opentxs PRIVATE $<TARGET_OBJECTS:opentxs-core-otprotob>)
add_dependencies(generated_code opentxs-core-otprotob)
