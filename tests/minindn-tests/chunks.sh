#!/bin/bash

topos=("12.5" "25" "37.5" "50" "75" "100")

for topo in ${topos[*]}; do
  sudo mn -c
  nfd-stop
  echo $topo
  sed  "s/delay/delay="$topo"ms/g" perf_chunks.conf > temp.conf
  sleep 0.1
  sudo minindn temp.conf --exp chunks --no-cli --cs-size=0 --no-nlsr
done
