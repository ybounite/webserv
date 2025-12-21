#!/bin/bash

URL="http://127.0.0.1:8080"

for i in {1..20000}
do
  (
    echo "----- Request $i -----"
    curl "$URL"
    echo -e "\n----------------------"
  ) &
done

wait

