#!/usr/bin/env python3
import json
import os
import copy
import matplotlib.pyplot as plt
import numpy as np
import dateutil.parser
from datetime import date
import re
import collections
import sys
from dateutil.parser import parse
import pytz


#devicesOfInterest = ['my_test_device']

rssiHist = False
droppedPackets = False


"""
Use Python's os methods to find all json files in the working directory.
"""
def is_json(p):
    nm, ex = os.path.splitext(p)
    return ex == ".json"


"""
Use a Python set to find all unique devices represented in a file.
"""
def find_devices(p):
    devices = set([])
    for e in p:
        devices.add(e["dev_id"])
    return devices

"""
Return a tuple of one rssi from a json object.
"""
def jObj2rssiTup(p):
    return (p["metadata"]["gateways"][0]["rssi"])



"""
Start the main program here. (This file isn't meant to be imported.)
"""
textFiles = filter(is_json, os.listdir("."))
rex = re.compile("d([0-9]+)s([0-9]+)m_([0-9]+).json")

if (len(sys.argv)<2):
    print("Need an input file", file=sys.stderr)
    exit(1)

"""
Try reading the entire file all at once. This may fail if you have a 
sufficiently large file, but it works fine for testing.
"""
fi = open(sys.argv[1], "r")
njStrings = fi.readlines() #Not-json strings... they're almost json, though.

# Every line has a bit of non-json stuff that precedes it. This
# line removes the non-json stuff and gives a list (rather than a
# single-use Python generator)
jObjs = list(map(lambda s: json.loads(list(map(str.strip, s.split("up ")))[1]), njStrings))
"""
Every json object in jObjs represents one data point in the
time series. You can get payload fields from the kth point
with jObjs[k]["payload_fields"]["myField"]. There is also cool stuff
in the metadata. See below.
"""

#devicesOfInterest = find_devices(jObjs)

countBatPair = {}

# Alpha for a complementary filter.
alpha = 0.3

for p in jObjs:
    print(p["payload_fields"])
    
    
# Okay, these are triplets, not pairs.
countBatPair = [
    (parse(p["metadata"]["time"]).astimezone(pytz.timezone('US/Eastern')),
    p["payload_fields"]["volume"],
    p["payload_fields"]["weight"]) for p in jObjs]



# Plot can't work on tuples in the way we want it to, so we have to use
# zip(*thingToUnzip). The * is essential.
count, volume, weight = zip(*countBatPair)

dates = []

for c in count:
    t = str(count[0])
    date = t[0:10]
    time = t[12:22]
    # print(date + " " + time + "\n")

# plot weight
plt.plot(count, weight, label = "weight") 

# plotting volume  
plt.plot(count, volume, label = "volume")

# naming the x axis 
plt.xlabel('Time') 
# naming the y axis 
plt.ylabel('Measurments') 
# giving a title to my graph 
plt.title('Weight and Volume Over Time') 
  
# show a legend on the plot 
plt.legend() 
  
# function to show the plot 
plt.show() 



# #plt.figure()
# fig, ax1 = plt.subplots()
# color = 'tab:red'
# ph1 = ax1.twinx()


# #plt.plot(count, bat)
# plt.title(f"Weight vs Volume (n={len(count)})")
# plt.xlabel("Hours")
# ax1.set_ylabel('Weight (lbs)', color='tab:red')
# ax1.set_ylim([3.5,4])
# ph1.set_ylabel('Volume (%)', color='tab:blue')
# ax1.tick_params(axis='y', labelcolor='tab:red')
# ph1.tick_params(axis='y', labelcolor='tab:blue')

# Try a complementary filter on the battery voltage series.
#count = [q/60*k.p_min for q in count]
# bfilt = np.zeros(len(count))
# bfilt[0] = bat[0]
# for i in range(len(bfilt) -1):
#     bfilt[i+1] = alpha*bat[i+1] + (1-alpha)*bfilt[i]
# #print(f"Plot {k}")
# #print(count, bfilt)

#plt.plot(count, bat)
# # ax1.plot(count[0:], weight, color='tab:red')
# # ph1.step(count[0:], volume)
# #plt.plot(count[1:], bfilt)

# plt.show()


