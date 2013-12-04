CircuitLocker
====

[![Build Status](https://drone.io/github.com/stephenplaza/CircuitLock/status.png)](https://drone.io/github.com/stephenplaza/CircuitLock/latest)

A tool that provides algorithms for locking and cracking digital circuits.  The tool has an
algorithm for random XOR insertion, test-invariant
MUX insertion, and a hill-climbing algorithm for discerning the key bits.

## Installation

    % mkdir build; cd build
    % cmake ..
    % make

## Running

For help

    % CircuitLock -h

To add 64 test-invariant MUX locks to the c3540 circuit and to find out the total number of potential MUX candidates, run the following:

    % CircuitLock c3540.blif --test-file c3540.test --lock-mux 64 --random-seed 1 --mux-cands 1

To add 64 random XOR locks to the c3540 circuit and then try to extract the correct key from this "locked" circuit, run the following:

    % CircuitLock c3540.blif --test-file c3540.test --lock-randxor 64 --random-seed 1 --crack-key

## Benchmarks

The benchmarks directory contains some modified versions of ISCAS89 and IWLS05 circuits.  The latches were removed from the IWLS
circuits and they was strashed using ABC.  The benchmarks directory also contains test patterns for each circuit.
Test vectors were produced using ATALANTA (default options)
on the bench formatted version of these files.  The format of the file is a list of input names on one line and a series of 0s
and 1s in the same order as the inputs on the following lines.  Each line is a different test pattern.
## Notes

This tool is for primarily exploring locking mechanisms in combinational circuits.  The circuit parser can read BLIF format with
latches.  If latches exist in the circuit, the latch output is treated as a primary input; the latch input is treated as
a primary output.

## To Do

* Cleanup debug output
* Add more comments to code
* Provide more robustness in reading test input vectors.

