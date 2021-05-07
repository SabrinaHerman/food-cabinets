#!/bin/bash
/usr/local/Cellar/mosquitto/2.0.7/bin/mosquitto_sub \
-h us-west.thethings.network -t '+/devices/+/up' \
-u 'yourAppNameHere' \
-P 'yourAccessKeyHere' -v