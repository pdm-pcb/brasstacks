# Given that we're building on x86_64 chips, target Haswell/Excavator or newer

function(set_target_architecture)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU"
       OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64")
            add_compile_options("-march=x86-64-v3")
        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64")
            add_compile_options("/arch:AVX2")
        endif()
    endif()
endfunction()
