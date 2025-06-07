add_library(TestArOptionalImplementation SHARED plugin.cpp)
target_link_libraries(TestArOptionalImplementation PUBLIC ar pxr::tf)
file(
    GENERATE
    OUTPUT "$<TARGET_FILE_DIR:TestArOptionalImplementation>/plugInfo_$<CONFIG>.json"
    INPUT plugInfo.json
)
