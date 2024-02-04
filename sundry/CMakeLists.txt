add_executable( twelveToneComb "" )
target_sources( twelveToneComb PRIVATE twelveToneComb.cpp )
target_include_directories( twelveToneComb PUBLIC ../src )
target_link_libraries( twelveToneComb ReiserRT_CombGenerator )
target_compile_options( twelveToneComb PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

add_executable( energyCalc "" )
target_sources( energyCalc PRIVATE energyCalc.cpp )
target_include_directories( energyCalc PUBLIC ../src )
target_link_libraries( energyCalc ReiserRT_CombGenerator )
target_compile_options( energyCalc PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

add_executable( streamCombGenerator )
target_sources( streamCombGenerator PRIVATE streamCombGenerator.cpp )
target_include_directories( streamCombGenerator PUBLIC ../src ../testUtilities )
target_link_libraries( streamCombGenerator ReiserRT_CombGenerator TestUtilities )
#[[
target_compile_options( streamCombGenerator PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
#]]
