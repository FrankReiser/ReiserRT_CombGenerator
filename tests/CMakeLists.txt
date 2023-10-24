add_executable( testResetDefaults "" )
target_sources( testResetDefaults PRIVATE testResetDefaults.cpp )
target_include_directories( testResetDefaults PUBLIC ../src )
target_link_libraries( testResetDefaults ReiserRT_CombGenerator )
target_compile_options( testResetDefaults PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
add_test( NAME runResetDefaultsTest COMMAND $<TARGET_FILE:testResetDefaults> )

add_executable( testMagAndPhaseNoEnvelope "" )
target_sources( testMagAndPhaseNoEnvelope PRIVATE testMagAndPhaseNoEnvelope.cpp )
target_include_directories( testMagAndPhaseNoEnvelope PUBLIC ../src )
target_link_libraries( testMagAndPhaseNoEnvelope ReiserRT_CombGenerator )
target_compile_options( testMagAndPhaseNoEnvelope PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
add_test( NAME runMagAndPhaseNoEnvelopeTest COMMAND $<TARGET_FILE:testMagAndPhaseNoEnvelope> )

add_executable( testDefaultMagWithEnvelope "" )
target_sources( testDefaultMagWithEnvelope PRIVATE testDefaultMagWithEnvelope.cpp )
target_include_directories( testDefaultMagWithEnvelope PUBLIC ../src )
target_link_libraries( testDefaultMagWithEnvelope ReiserRT_CombGenerator )
target_compile_options( testDefaultMagWithEnvelope PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
add_test( NAME runDefaultMagWithEnvelopeTest COMMAND $<TARGET_FILE:testDefaultMagWithEnvelope> )

add_executable( testMagWithEnvelope "" )
target_sources( testMagWithEnvelope PRIVATE testMagWithEnvelope.cpp )
target_include_directories( testMagWithEnvelope PUBLIC ../src )
target_link_libraries( testMagWithEnvelope ReiserRT_CombGenerator )
target_compile_options( testMagWithEnvelope PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
add_test( NAME runMagWithEnvelopeTest COMMAND $<TARGET_FILE:testMagWithEnvelope> )
