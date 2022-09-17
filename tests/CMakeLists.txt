add_executable( testScintillation "" )
target_sources( testScintillation PRIVATE testScintillation.cpp )
target_include_directories( testScintillation PUBLIC ../src )
target_link_libraries( testScintillation CombGenerator )
add_test( NAME runScintillationTest COMMAND $<TARGET_FILE:testScintillation> )

