add_executable( testCombGenerator "" )
target_sources( testCombGenerator PRIVATE testCombGenerator.cpp )
target_include_directories( testCombGenerator PUBLIC ../src )
target_link_libraries( testCombGenerator ReiserRT_CombGenerator )
target_compile_options( testCombGenerator PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
add_test( NAME runCombGeneratorTest COMMAND $<TARGET_FILE:testCombGenerator> )
