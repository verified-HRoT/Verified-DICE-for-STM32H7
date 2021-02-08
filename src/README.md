# Source 

  - ``core``: contains the application logic, boot things that lead to application logic (eg generated keys etc) in dice_engine and l0_engine
  - ``fw``: things to do with the actual low level binaries running on the microcontroller, also a bit of glue to help run as much of this as posible in simulation on the host. Note that we make four 'executables':
    - DICE: The basic boot loader.
    - L0: RIoT
    - L1: Application (Artifact extraction tool).
  - ``common``: Intended to be fundamental code which is common to the HW and host. Stuff like the logger, locks, abstractions of threads, io, time. Some is unfortunately rather application specific, so the ids of the message types are here, the framing scheme and all the types which can be exchanged over the link. This is where the evil blob.h and buffer.h live. Very many types are just derived from those and really are only half an attempt at not passing the wrong blob to an interface.
  - ``crypto``: Code concered with crypto, including producing certificates. In the end calls HACL* to do the work.
  - ``global``: some global things, timeouts and, importantly, version numbers that get embedded in the FW.
