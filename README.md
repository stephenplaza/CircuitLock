CircuitLocker
====

[![Build Status](https://drone.io/github.com/stephenplaza/CircuitLock/status.png)](https://drone.io/github.com/stephenplaza/CircuitLock/latest)

A tool that provides algorithms for locking and cracking digital circuits.

## Installation

    % mkdir build; cd build
    % cmake ..
    % make

## Running

For help

    % CircuitLocker -h

## Notes

This tool is for primarily exploring locking mechanisms in combinational circuits.  The circuit parser can read BLIF format with
latches.  If latches exist in the circuit, the latch output is treated as a primary input; the latch input is treated as
a primary output.
