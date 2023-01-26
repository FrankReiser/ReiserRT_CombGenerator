# ReiserRT_CombGenerator

Frank Reiser's C++17 implementation of a fast and accurate harmonic series, waveform generator.
The data produced is complex in nature, delivering both in-phase and quadrature components.
This library component is dependent on the ReiserRT_FlyingPhasor shared object library.
Interface files are not reliably usable from C++11 compiles. However, 
this is dependent on the actual compiler used. The test harness will not compile with
gcc 4.8.5 using the c++11 standard. However, the test harness will compile with gcc 8.5.0
using the c++11 standard. It is recommended that you use this library with c++17 or higher
compilation. The compiled library code is built using the c++17 standard.

## Overview

This generator evolved out of a desire to generate a harmonic series of tones as fast and accurate
as possible. It supports a number of tones spaced at some fundamental frequency.
The tones produced support independent (settable) phases and individual magnitude profiles,
including envelope modulation.

The implementation uses a container of ReiserRT_FlyingPhasor instances, specified for some maximum
use case at time of construction. Refer to project ReiserRT_FlyingPhasor for specifics.

## Details

The CombGenerator is instantiated for some maximum number of harmonics that will ever
be requested for an application. Once instantiated, it requires a `reset` operation.
It is as `reset` time, where initial phases, magnitudes are set. Additionally, an
envelope modulator may be hooked up at `reset` time.

Samples are obtained by invoking the `getSamples` operation. If an envelope modulator
was hooked up, it will be notified once per harmonic. This notification comes with
numerous 'hints' that the observer may utilize in returning an envelope. These are,
the current sample offset (a count), the number of samples of envelope to return,
the current harmonic (0=fundamental), and the nominal magnitude for the harmonic.
Additional state data may be managed by the observer instance.

## Thread Safety
This CombGenerator is NOT "thread safe". There are no concurrent access mechanisms
in place and there is no good reason for addressing this. To the contrary,
state left by one thread would make no sense to another, never mind the concurrency issues.
Have threads use their own unique instances.

## Building and Installation
Roughly as follows:
1) Obtain a copy of the project
2) Create a build folder within the project root folder.
3) Switch directory to the build folder and run the following
   to configure and build the project for your platform:
   ```
   cmake ..
   cmake --build .
   ```
4) Test the library
   ```
   ctest
   ```
5) Install the library as follows (You'll most likely
   need root permissions to do this):
   ```
   sudo cmake --install .
   ```
