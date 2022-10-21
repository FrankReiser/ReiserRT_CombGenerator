add_executable( twelveToneComb "" )
target_sources( twelveToneComb PRIVATE twelveToneComb.cpp )
target_include_directories( twelveToneComb PUBLIC ../src )
target_link_libraries( twelveToneComb TsgNgCombGenerator )

add_executable( energyCalc "" )
target_sources( energyCalc PRIVATE energyCalc.cpp )
target_include_directories( energyCalc PUBLIC ../src )
target_link_libraries( energyCalc TsgNgCombGenerator )
