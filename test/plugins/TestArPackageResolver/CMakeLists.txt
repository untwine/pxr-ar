add_library(TestArPackageResolver SHARED plugin.cpp)
target_link_libraries(TestArPackageResolver PUBLIC ar pxr::tf)
file(
    GENERATE
    OUTPUT "$<TARGET_FILE_DIR:TestArPackageResolver>/plugInfo_$<CONFIG>.json"
    INPUT plugInfo.json
)
