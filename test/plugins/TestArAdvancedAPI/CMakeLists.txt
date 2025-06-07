add_library(TestArAdvancedAPI SHARED plugin.cpp)
target_link_libraries(TestArAdvancedAPI PUBLIC ar pxr::tf)
file(
    GENERATE
    OUTPUT "$<TARGET_FILE_DIR:TestArAdvancedAPI>/plugInfo_$<CONFIG>.json"
    INPUT plugInfo.json
)
