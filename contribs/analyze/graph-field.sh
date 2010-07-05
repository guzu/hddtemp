#!/bin/sh

./plot-field.pl $1 | graph --top-label "field($1)" -NX -Tpng > field-$1.png
