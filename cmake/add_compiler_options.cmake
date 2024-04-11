function(add_compiler_options target_name)
    set_target_properties(
        ${target_name} PROPERTIES
        # Use standard C++20
            CXX_STANDARD          20
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS        OFF

        # Use standard C17
            C_STANDARD            17
            C_STANDARD_REQUIRED   ON
            C_EXTENSIONS          OFF
    )

# For x86_64, target Haswell/Excavator or newer
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64")
        target_compile_options(${target_name} PUBLIC "-march=x86-64-v3")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64")
        target_compile_options(${target_name} PUBLIC "/arch:AVX2")
    endif()
endif()
endfunction()