# Lumino

Lumino is my attempt to make a framework for the Open-Source Microlight (OSMxyz)
to allow for modes mimicing the Kinetic microlights.

Future Goals:
* More Primes for single and dual modes
* More Modes that don't depend on Primes (accel-only based modes?)
* Conjuring mode


## Modes

Currently there is support for two types of modes: single and dual. Single modes
animate using a single prime and don't use the accelerometer for function. Dual
modes switch between two primes based on accelerometer settings, either via
shaking (high accel) or tilting (configurable on which axis to use).

## Primes

Primes are basic animations that have configurable timings. They are used in
single and dual modes.

* Strobe: color/blank while stepping through palette
* Tracer: color/tracer while stepping through palette
* BlinkE: color/color.../blank
