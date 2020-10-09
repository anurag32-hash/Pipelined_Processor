# Pipelined Processor

In this we will extend the processor by introducing
**PIPELINING** into the processor.

1. Use the pipelining structure and ideas for the MIPS processor.
2. Apply data hazard techniques when any type of hazard is detected.

Sometimes, operations such as floating point arithmetic or memory accesses in a pipelined
processor require variable delays. A floating point computation may depend on the data
operands. A memory access may exhibit variable delays due to the cache hierarchy. Let us try
to model the memory architecture by introducing a data memory that works as follows:

1. The operation completes in one cycle with a probability x (this is considered a HIT).
2. The operation requires N cycles with a probability 1-x (this is considered a MISS).
3. Upon a memory request, at end of one cycle, the memory indicates whether the
    operation was a HIT or MISS. If HIT, the operation is complete. If not, the operation
    completes N-1 cycles later.

Implemented this variable delay feature in this pipeline.

