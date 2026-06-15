if (NOT DEFINED SOURCE_DLL OR NOT DEFINED TARGET_DIR)
    message(WARNING "DLL copy skipped: SOURCE_DLL or TARGET_DIR not defined; assuming necessary libraries are in PATH variable.")
    return()
endif ()

if (NOT EXISTS "${SOURCE_DLL}")
    message(WARNING "DLL copy skipped: '${SOURCE_DLL}' does not exist; assuming necessary libraries are in PATH variable.")
    return()
endif ()

execute_process(
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${SOURCE_DLL}" "${TARGET_DIR}"
        RESULT_VARIABLE COPY_RESULT
)

if (NOT COPY_RESULT EQUAL 0)
    message(WARNING "Could not copy DLL '${SOURCE_DLL}' to '${TARGET_DIR}'; assuming necessary libraries are in PATH variable.")
endif ()
