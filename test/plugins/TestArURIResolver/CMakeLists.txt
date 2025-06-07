add_library(TestArURIResolver SHARED plugin.cpp)
target_link_libraries(TestArURIResolver PUBLIC ar pxr::tf)
file(
    GENERATE
    OUTPUT "$<TARGET_FILE_DIR:TestArURIResolver>/plugInfo_$<CONFIG>.json"
    INPUT plugInfo.json
)

target_include_directories(TestArURIResolver
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_compile_definitions(TestArURIResolver
    PRIVATE
        AR_EXPORTS=1
)
