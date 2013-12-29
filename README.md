mchckTupBlink
=============

Blink Example on the McHck board using the Tup Build system.

This is example code for the McHck development platform (https://mchck.org/) using the Tup build system (http://gittup.org/tup/).

By default, the McHck uses Make to build everything. To me, Makefiles are incomprehensible garbage. Tup is much more readable, compiles fewer files per re-spin, and includes a file system monitor for automatic rebuilding on each file change.

Unfortunately, to get this example to work, I had to rip out Link Time Optimization. I kept getting segfaults on the link step when running the command through Tup. This exacts a small hit on flash space (this example leaves 27828 byte remaining vs. the original's 28564, a difference of 736 bytes). Only time will tell if this hit is cumulative or one-time.
