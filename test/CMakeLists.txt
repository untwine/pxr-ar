add_subdirectory(plugins)

set(_arPlugin "$<SHELL_PATH:$<TARGET_FILE_DIR:ar>/usd/ar/resources/plugInfo_$<CONFIG>.json>")
set(_env "PXR_PLUGINPATH_NAME=${_arPlugin}")

# Helper function to set test environment variables
function(set_test_environment NAME)

    # Update environment to extend the DLL search path on Windows.
    if (WIN32)
        set(DLL_DIRS $<TARGET_RUNTIME_DLL_DIRS:${NAME}>)
        set(_env "${_env};PATH=$<JOIN:$<SHELL_PATH:${DLL_DIRS}>,\\;>")
    endif()

    string (JOIN ";" args "${ARGN}")
    set_tests_properties(${NAME} PROPERTIES ENVIRONMENT "${args};${_env}")
endfunction()

add_executable(testArURIResolver_CPP testArURIResolver.cpp)
target_link_libraries(testArURIResolver_CPP PUBLIC TestArURIResolver ar pxr::arch pxr::tf)
add_test(NAME testArURIResolver_CPP COMMAND testArURIResolver_CPP)
set_test_environment(testArURIResolver_CPP
    "PLUGIN_PATH=$<SHELL_PATH:$<TARGET_FILE_DIR:TestArURIResolver>/plugInfo_$<CONFIG>.json>"
)

add_executable(testArDefaultResolver_CPP testArDefaultResolver.cpp)
target_link_libraries(testArDefaultResolver_CPP PUBLIC ar pxr::arch pxr::tf)
add_test(NAME testArDefaultResolver_CPP COMMAND testArDefaultResolver_CPP)
set_test_environment(testArDefaultResolver_CPP)

add_executable(testArNotice_CPP testArNotice.cpp)
target_link_libraries(testArNotice_CPP PUBLIC ar pxr::arch pxr::tf)
add_test(NAME testArNotice_CPP COMMAND testArNotice_CPP)
set_test_environment(testArNotice_CPP)

add_executable(testArResolverContext_CPP testArResolverContext.cpp)
target_link_libraries(testArResolverContext_CPP PUBLIC ar pxr::arch pxr::tf)
add_test(NAME testArResolverContext_CPP COMMAND testArResolverContext_CPP)
set_test_environment(testArResolverContext_CPP)

add_executable(testArThreadedAssetCreation testArThreadedAssetCreation.cpp)
target_link_libraries(testArThreadedAssetCreation PUBLIC ar pxr::arch pxr::tf)
add_test(NAME testArThreadedAssetCreation COMMAND testArThreadedAssetCreation)
set_test_environment(testArThreadedAssetCreation)

if(BUILD_PYTHON_BINDINGS)
    pytest_discover_tests(
        TestAr
        LIBRARY_PATH_PREPEND
            $<TARGET_FILE_DIR:ar>
            $<TARGET_FILE_DIR:pyAr>
            $<TARGET_FILE_DIR:pxr::plug>
            $<TARGET_FILE_DIR:pxr::pyPlug>
            $<TARGET_FILE_DIR:pxr::tf>
            $<TARGET_FILE_DIR:pxr::pyTf>
        PYTHON_PATH_PREPEND
            "$<TARGET_FILE_DIR:pyAr>/../.."
            "$<TARGET_FILE_DIR:pxr::pyPlug>/../.."
            "$<TARGET_FILE_DIR:pxr::pyTf>/../.."
            "${plugins_src}/python"
        TRIM_FROM_NAME "^test_"
        TRIM_FROM_FULL_NAME "^TestAr"
        ENVIRONMENT
            "AR_ADVANCED_API_PLUGIN=$<SHELL_PATH:$<TARGET_FILE_DIR:TestArAdvancedAPI>/plugInfo_$<CONFIG>.json>"
            "AR_PACKAGE_RESOLVER_PLUGIN=$<SHELL_PATH:$<TARGET_FILE_DIR:TestArPackageResolver>/plugInfo_$<CONFIG>.json>"
            "AR_URI_RESOLVER_PLUGIN=$<SHELL_PATH:$<TARGET_FILE_DIR:TestArURIResolver>/plugInfo_$<CONFIG>.json>"
            "AR_OPTIONAL_IMPL_PLUGIN=$<SHELL_PATH:$<TARGET_FILE_DIR:TestArOptionalImplementation>/plugInfo_$<CONFIG>.json>"
            "${_env}"
        EXTRA_ARGS "-v"
        DEPENDS ar pyAr
    )

endif()