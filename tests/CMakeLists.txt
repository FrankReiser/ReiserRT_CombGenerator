add_executable( testSubSeedGenerator "" )
target_sources( testSubSeedGenerator PRIVATE testSubSeedGenerator.cpp )
target_include_directories( testSubSeedGenerator PUBLIC ../src ../testUtilities )
target_link_libraries( testSubSeedGenerator CombGenerator TestUtilities )
add_test( NAME runSubSeedGeneratorTest1 COMMAND $<TARGET_FILE:testSubSeedGenerator> --seed=1234 )
add_test( NAME runSubSeedGeneratorTest2 COMMAND $<TARGET_FILE:testSubSeedGenerator> --seed=5674457 )
add_test( NAME runSubSeedGeneratorTest3 COMMAND $<TARGET_FILE:testSubSeedGenerator> --seed=8348489 )

add_executable( testScintillation "" )
target_sources( testScintillation PRIVATE testScintillation.cpp )
target_include_directories( testScintillation PUBLIC ../src )
target_link_libraries( testScintillation CombGenerator )
add_test( NAME runScintillationTest COMMAND $<TARGET_FILE:testScintillation> )

