# Copyright (c) 2010-2021 The Open-Transactions developers
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_library(
  opentxs-ui-activitysummary OBJECT
  "ActivitySummary.cpp"
  "ActivitySummary.hpp"
  "ActivitySummaryItem.cpp"
  "ActivitySummaryItem.hpp"
)
set(cxx-install-headers
    "${opentxs_SOURCE_DIR}/include/opentxs/ui/ActivitySummary.hpp"
    "${opentxs_SOURCE_DIR}/include/opentxs/ui/ActivitySummaryItem.hpp"
)
target_link_libraries(opentxs-ui-activitysummary PRIVATE opentxs::messages)

if(OT_WITH_QT)
  set(ot-qobject-install-headers
      "${opentxs_SOURCE_DIR}/include/opentxs/ui/qt/ActivitySummary.hpp"
  )
  qt5_wrap_cpp(MOC_SRCS ${ot-qobject-install-headers})
  target_sources(opentxs-ui-activitysummary PRIVATE ${MOC_SRCS})

  if(OPENTXS_STANDALONE)
    install(
      FILES ${ot-qobject-install-headers}
      DESTINATION include/opentxs/ui/qt
      COMPONENT dev
    )
  endif()
endif()

opentxs_set_qt(opentxs-ui-activitysummary)
target_sources(opentxs-ui-activitysummary PRIVATE ${cxx-install-headers})
target_sources(opentxs PRIVATE $<TARGET_OBJECTS:opentxs-ui-activitysummary>)
add_dependencies(opentxs-ui-activitysummary generated_code)

if(OPENTXS_STANDALONE)
  install(
    FILES ${cxx-install-headers}
    DESTINATION include/opentxs/ui
    COMPONENT dev
  )
endif()
