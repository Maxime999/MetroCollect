# MetroCollect Usage

<!--
Copyright 2018 CFM (www.cfm.fr)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
-->


## Contents
- [MetroCollect Usage](#metrocollect-usage)
  - [Contents](#contents)
  - [Metrics](#metrics)
    - [CPU Metrics](#cpu-metrics)
    - [Memory metrics](#memory-metrics)
    - [Network metrics](#network-metrics)
    - [Ethtool metrics](#ethtool-metrics)
  - [Standalone interfaces](#standalone-interfaces)
    - [MetroCollectValues](#metrocollectvalues)
    - [MetroCollectStats](#metrocollectstats)
    - [MetroCollectFiles](#metrocollectfiles)
  - [Snap Configuration](#snap-configuration)
    - [Configuration](#configuration)
    - [Parameters](#parameters)
    - [Metrics](#metrics-1)

## Metrics
MetroCollect gathers metrics from different sources:
 * CPU metrics from `/proc/stat`
 * Memory metrics from `/proc/meminfo`
 * Network metrics from `/proc/net/dev`
 * Additional network metrics with Ethtool

Those metrics are organized hierarchically. For instance, `/cfm/memory/cached` represents the amount of cached memory on the system, while `cfm/network/eth0/rx/bytes` represents the number of bytes received by `eth0`.

Most metric have a unit attached to it, and are expressed in units per second.

### CPU Metrics
CPU metrics are read from the `/proc/stat` file, you may refer to the relevant man page of your system to have a thorough description.

As a summary, there are three types of CPU metrics:
 - for each CPU core, proportion of time spent in different CPU states (user, system, nice, idle, etc.)
 - aggregated proportion of time spent in different CPU states
 - other CPU metrics: number of interrupts, soft interrupts, spawned processed, running and blocked processes, etc.

### Memory metrics
Memory metrics are read from the `/proc/memory` file, you may refer to the relevant man page of your system to have a thorough description.

### Network metrics
Network metrics are read from the `/proc/net/dev` file, you may refer to the relevant man page of your system to have a thorough description.

For each interface (either up or down), the number of bytes, packets and various errors are collected, for both reception ("rx") and transmission ("tx").

### Ethtool metrics
These are the same metrics provided by the `ethtool -S $iface` for each network interface `$iface`. It provides very detailed information about each interface, but these information depend on the interface's driver. you may refer to the driver documentation for more details.

MetroCollect supports all drivers supported by `ethtool`, and it is optimized for `ixgbe`, `igb` and `i40e`.

When a number appears in a metric name, it is moved at the end of it. For instance:
``` bash
$ ethtool -S eth0
rx_queue_0_packets: 0
rx_queue_0_bytes: 0
rx_queue_1_packets: 0
rx_queue_1_bytes: 0
...
```
becomes
```
/ethtool/ixgbe/eth0/rx_queue_i_packets/0	0
/ethtool/ixgbe/eth0/rx_queue_i_bytes/0		0
/ethtool/ixgbe/eth0/rx_queue_i_packets/1	0
/ethtool/ixgbe/eth0/rx_queue_i_bytes/1		0
...
```

## Standalone interfaces
### MetroCollectValues
This program collects all available metrics and prints them on the standard output. For each metric, it shows values read from the kernel at some time, values read some time later, and the computed variation. That is because most kernel values are counters that can only increase; for example the number of bytes sent by eth0 or the amount of interrupts serviced.

MetroCollectValues can take up to **two arguments**:
- the sampling interval in milliseconds, that is the duration to wait between two readings of kernel values. Default is 100 milliseconds
- how many times to report metrics. Default is 0, which means unlimited time

For example, `./MetroCollectValues 500 10` will read kernel values and print them every 500 milliseconds, and it will do so 10 times. The program will thus run for 5 seconds.

### MetroCollectStats
This program collects all available metrics, compute statistics on a moving window and prints them on the standard output. For each metric, it shows the minimum, maximum, average and standard deviation of its variation.

MetroCollectValues can take up to **four arguments**:
- the sampling interval in milliseconds, that is the duration to wait between two readings of kernel values. Default is 100 milliseconds
- the length of the moving window in number of samples. Default is 10 (statistics are computed every 10 samples)
- the overlap of the moving window in number of samples, it must be less than the length of the window. Default is 0 (no overlap)
- how many times to report metrics. Default is 0, which means unlimited time

By default, statistics are thus computed and printed every 1 second.

For example, `./MetroCollectStats 50 5 0 4` will read kernel values every 50 milliseconds, and compute and print stats every 5 samples (that is every 250 milliseconds), and it will do so 4 times. The program will thus run for 1 seconds.


### MetroCollectFiles
This program collects all available metrics and write them to a file.

MetroCollectValues can take up to **three arguments**:
- the sampling interval in milliseconds, that is the duration to wait between two readings of kernel values. Default is 100 milliseconds
- how many times to report metrics. Default is 0, which means unlimited time
- path of the file to write to. Default is `./output.csv`

For example, `./MetroCollectFile 0 20 maxspeed.csv` will read kernel values continuously (no delay between readings) 20 times, and write the variation to he file `./maxspeed.csv`.


## Snap Configuration
### Configuration
First set up the [Snap framework](https://github.com/intelsdi-x/snap/blob/master/README.md#getting-started).

The default configuration for a MetroCollect Snap task is the following:
```yaml
---
  version: 1
  schedule:
    type: "streaming"
  max-failures: 10
  workflow:
    collect:
      metrics:
        /cfm/*: {}
      config:
        /cfm:
          SendValues: false
          SendStats: true
          SamplingInterval: 100
          ProcessingWindowLength: 10
          ProcessingWindowOverlap: 0
          ConvertToUnitPerSecond: true
          UnchangedMetricTimeout: 120
          # MaxMetricsBuffer: 0
          # MaxCollectDuration: 0
      publish:
#	    ...
```

### Parameters
The parameters are (default values are given [above](#configuration)):
 - `SendValues` (type boolean): whether or not to send values to Snap
 - `SendStats` (type boolean): whether or not to send statistics to Snap
 - `SamplingInterval` (type int): delay in millisecond between two readings of the kernel values
 - `ProcessingWindowLength` (type int): moving window length (in number of samples). It is used to compute statistics
 - `ProcessingWindowOverlap` (type int): moving window overlap (in number of samples), it must be less than the length of the window. It is used to compute statistics
 - `ConvertToUnitPerSecond` (type boolean): wether or not to convert values in units per second if relevant (if you prefer the number of bytes sent per second instead of bytes sent per 100 milliseconds)
 - `UnchangedMetricTimeout` (type int): by default, for performance reasons, constant metrics are downsampled and thus not sent everytime to Snap. If the value changes it is sent immediately. This parameter is the downsample factor: how many null variations to ignore before sending the value to Snap again. If it is 0, downsample will not take place, if it is -1 constant values will never be sent
 - `MaxMetricsBuffer` (type int): maximum number of metrics to send to Snap at once
 - `MaxCollectDuration` (type int): maximum waiting time (in seconds) before sending metrics to Snap

By default, values will be read from the kernel every 100 milliseconds, compute stats every 10 samples (that is every 1 second) and send them to Snap. If a value has remained constant across 120 iterations (that is during 2 minutes), zeros will be sent to Snap.


### Metrics
Collected metrics are described [above](#metrics).

For each metric, you can select which statistics you are interested in (shown below for the metric of aggregated CPU proportion of time spent in user mode, **note the use of wildcards**):
```
/cfm/cpu/all/user/min/*: {}
/cfm/cpu/all/user/max/*: {}
/cfm/cpu/all/user/average/*: {}
/cfm/cpu/all/user/stddev/*: {}
```

You can use wildcards to specify groups of metrics easily:
```
/cfm/cpu/all/*: {}
/cfm/memory/*: {}
/cfm/network/eth0/*: {}
...
```
