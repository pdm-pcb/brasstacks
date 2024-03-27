function(add_warning_flags target_name)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU"
       OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(
            ${target_name} PUBLIC
            "-Wall"     # Enable most warnings
            "-Wextra"   # ...and a few more
            "-pedantic" # ...and language compliance checks
        )

        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            target_compile_options(
                ${target_name} PUBLIC
                "-Weverything"      # Clang supports a true "-Wall"
                "-Wno-c++98-compat" # ...but we don't care about C++98
                "-Wno-c++98-compat-pedantic"
                "-Wno-c++20-compat" # And we're targeting C++20
                "-Wno-newline-eof"  # I'm willing to accept this risk
                "-Wno-global-constructors"   # There are some statics I like
                "-Wno-exit-time-destructors" # Same as above
                # "-Wno-c99-designator" # And
            )
        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        target_compile_options(
            ${target_name} PUBLIC
            "/W4" # All warnings not disabled by default
        )
    endif()
endfunction()
