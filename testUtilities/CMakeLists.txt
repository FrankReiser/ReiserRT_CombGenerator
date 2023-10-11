add_library( TestUtilities STATIC "" )
target_sources( TestUtilities PRIVATE
        CommandLineParser.cpp
        SubSeedGenerator.cpp
        RandomPhaseDistributor.cpp
        RayleighDistributor.cpp
        ScintillationEngine.cpp
        CombScintillationEnvelopeFunctor.cpp
    )
# Added to accommodate the CombScintillationEnvelopFunctor.
target_include_directories( TestUtilities PUBLIC ../src )

target_compile_options( TestUtilities PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
