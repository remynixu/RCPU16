target_sources(${PROJECT_NAME}
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/main.c"
)

include("${CMAKE_CURRENT_LIST_DIR}/cpu/sources.cmake")