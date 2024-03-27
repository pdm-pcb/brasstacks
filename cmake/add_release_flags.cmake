function(add_release_flags target_name)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU"
       OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        if(CMAKE_BUILD_TYPE MATCHES "Release")
            target_compile_options(
                ${target_name} PUBLIC
                "-O3" # Optimize for speed, sacrifice size
            )
        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        if(CMAKE_BUILD_TYPE MATCHES "Release")
            target_compile_options(
                ${target_name} PUBLIC
                "/O2" # Optimize for speed. Implies:
                      # - global optimization
                      # - intrinsics
                      # - "favor fast code"
                      # - frame/pointer omission
                      # - inline function expansion
                      # - eliminate duplicate strings
                      # - function-level linking
            )

            target_link_options(
                ${target_name} PUBLIC
                "/OPT:NOREF" # Remove unused functions
            )
        endif()
    endif()
endfunction()
