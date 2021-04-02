#!/bin/bash

function runAnimationNSec
{
    animation="$1"
    duration="$2"

    python3 "$animation.py" $3 &
    pid=$!

    sleep "$duration"
    kill -9 "$pid"
    wait "$pid"
}

while [ true ]; do
    runAnimationNSec const_color 3 000022
    runAnimationNSec glim 10
    runAnimationNSec multicolor_stripe 5
    runAnimationNSec slowfill 11.5
    runAnimationNSec rainbow 5
    runAnimationNSec sparkle 7
done
