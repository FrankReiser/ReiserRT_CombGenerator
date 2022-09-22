add_executable( testSubSeedGenerator "" )
target_sources( testSubSeedGenerator PRIVATE testSubSeedGenerator.cpp )
target_include_directories( testSubSeedGenerator PUBLIC ../src ../testUtilities )
target_link_libraries( testSubSeedGenerator CombGenerator TestUtilities )
add_test( NAME runSubSeedGeneratorTest COMMAND $<TARGET_FILE:testSubSeedGenerator> --seed=1234 )

add_executable( testScintillation "" )
target_sources( testScintillation PRIVATE testScintillation.cpp )
target_include_directories( testScintillation PUBLIC ../src )
target_link_libraries( testScintillation CombGenerator )
add_test( NAME runScintillationTest COMMAND $<TARGET_FILE:testScintillation> )

