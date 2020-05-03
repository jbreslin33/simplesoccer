#!/bin/bash
# global_replace.sh
rpl -R -x'.h' -x'.cpp' '$1' '$2' ./
