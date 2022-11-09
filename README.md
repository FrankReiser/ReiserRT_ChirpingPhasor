# ReiserRT_ChirpingPhasor

Frank Reiser's C++11 implementation of a fast and accurate, "chirping" sin/cos waveform pair (I/Q) generator.
This library component is dependent on the ReiserRT_FlyingPhasor shared object library.

## Overview

This tone generator evolved out of a desire to generate complex exponential waveforms (sinusoids) fast and accurately,
which would increase (or decrease) linearly in frequency over the course of time (samples). 
The traditional way of doing this, involved repeated calls to sin and cos functions with an accelerating, radian input argument.
This produces accurate results, at least over a limited domain interval. However, it is anything but fast.

If a continual sequence of accelerating complex values are what is required for an application,
the task of generating this sequence can be accomplished by simply rotating a phasor around the unit circle
by a linearly increasing (or decreasing) angular rate. 
The only trigonometric functions required, are invoked during initialization. 
After initialization, we require no more than a handful of arithmetic operations (maybe a couple handfuls).
This feat is accomplished by employing an instance of ReiserRT FlyingPhasorToneGenerator, a sprinkle of "state data"
and a little "loving care". It is essentially a reimplementation of FlyingPhasorToneGenerator which employs a
FlyingPhasorToneGenerator as a dynamic rate variable.

Care must be exercised to ensure that the dynamic rate variable
does not cross the nyquist point over the course of usage. If this is allowed to occur, positive frequency rolls over 
to negative and, in decelerating cases, negative frequency becomes positive. Otherwise, it is immune from
domain range limits since it does not track, or care about, "cycles". Angular velocity may be monitored via the
`getOmegaBar` operation, which returns the mean average velocity between the next two samples delivered.
Acceleration may be halted or otherwise modified via the `modifyAccel` operation.

## Details
A little more needs to be said regarding the "loving care" mentioned above.
This tone generator is taking advantage of Euler's mathematics of the unit circle. When you multiply two phasors,
you multiply the magnitudes and add the angles. When applied to unit vectors (magnitudes of one),
the resultant magnitude stays one, you simply add the angles. Our state data unit vectors are in rectangular form,
we simply preform a complex multiply in rectangular form, the resultant magnitude of which may drift away from one.
Because of this, a re-normalization cycle must be preformed on some periodic basis and this adds to the cost.
This tone generator performs this re-normalization every other sample. This was chosen for two reasons.
One, it puts any resultant spur at the Nyquist (edge of bandwidth). Two, because at every other sample,
the re-normalization required is minimal, keeping its noise contribution minimum.
Additionally, with such small errors, a simple and inexpensive linear approximation is all that is required to maintain
stability. Accuracy is such that it is "almost" immeasurably worse. You be the judge.

# Thread Safety
This tone generator is NOT "thread safe". There are no concurrent access mechanisms in place and there is no good reason
for addressing this. To the contrary, state left by one thread would make no sense to another,
never mind the concurrency issues. Have threads use their own unique instances.

# Acknowledgements
As with ReiserRT_FlyingPhasor, this algorithm was derived from something I saw on StackExchange. It however was
a non-chirping variety. This chirping variety is a compounding of that algorithm to achieve a dynamic rate.
It is essentially a FlyingPhasorToneGenerator within a FlyingPhasorToneGenerator.

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
   