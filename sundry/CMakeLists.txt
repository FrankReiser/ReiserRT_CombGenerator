add_executable( twelveToneComb "" )
target_sources( twelveToneComb PRIVATE twelveToneComb.cpp )
target_include_directories( twelveToneComb PUBLIC ../src )
target_link_libraries( twelveToneComb ReiserRT_CombGenerator )

add_executable( energyCalc "" )
target_sources( energyCalc PRIVATE energyCalc.cpp )
target_include_directories( energyCalc PUBLIC ../src )
target_link_libraries( energyCalc ReiserRT_CombGenerator )

add_executable( streamCombGenerator )
target_sources( streamCombGenerator PRIVATE streamCombGenerator.cpp )
target_include_directories( streamCombGenerator PUBLIC ../src ../testUtilities )
target_link_libraries( streamCombGenerator ReiserRT_CombGenerator TestUtilities )
