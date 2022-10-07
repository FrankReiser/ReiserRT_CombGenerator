add_executable( twelveToneComb "" )
target_sources( twelveToneComb PRIVATE twelveToneComb.cpp )
target_include_directories( twelveToneComb PUBLIC ../src )
target_link_libraries( twelveToneComb TsgNgCombGenerator )

add_executable( streamCombGen "" )
target_sources( streamCombGen PRIVATE streamCombGen.cpp )
target_include_directories( streamCombGen PUBLIC ../src  ../testUtilities )
target_link_libraries( streamCombGen TsgNgCombGenerator TestUtilities )
