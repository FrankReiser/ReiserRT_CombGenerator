add_library( TestUtilities STATIC "" )
target_sources( TestUtilities PRIVATE
        CommandLineParser.cpp
        SubSeedGenerator.cpp
        RandomPhaseDistributor.cpp
        RayleighDistributor.cpp
        ScintillationEngine.cpp
    )
