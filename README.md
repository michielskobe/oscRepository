# Sensor Monitoring System [oscRepository]

## Description

For the C labs of the Operating Systems course of the Bachelor in Engineering Technology at KU Leuven (academic year 2023-2024), I implemented a sensor monitoring system that consists of client-side sensor nodes measuring the room temperature, and a central server-side sensor-gateway that acquires all sensor data from the sensor nodes. A sensor node uses a TCP connection to transfer the sensor data to the sensor gateway. Working with real embedded sensor nodes was not an option for this assignment. Therefore, sensor nodes will be simulated in software using a client-side sensor-node.

In this repository, there will be a distinction between homeworks, basic labs, project learning labs and the final project. The homeworks are just for setting up our Linux environment and dev tools for C. 
The basic C labs are meant to get familiar with C. At the end of these labs, there was be a first milestone. The project learning labs tackle 4 key OS concepts we needed for our final project: file I/O, inter-process communication, threads and thread synchronization. During these learning labs, we created 4 subsystems of the project. After the second and the fourth lab, there was another milestone. The final project is a composition and integration of the code we worked on in the previous weeks.

## Clone the repository

```bash
$ git clone git@github.com:michielskobe/oscRepository.git
$ cd oscRepository
```
## Start the Sensor Monitoring System

First, compile all the necessary program files using the Makefile:

```bash
$ cd projectLearningLabs/final_project
$ make all
```

Next, the connection manager is started and listens on a TCP socket for incoming connection requests from new sensor node. The commandline argument that starts it, also defines the port number it listens to and the maximum number of clients it will handle before shutting down:

```bash
$ ./sensor_gateway 1234 3
```

When the connection manager is active, the sensor nodes that will feed the system data can be started. The *./sensor_node_* has 4 commandline arguments:
* 'ID'            : a unique sensor node ID
* 'sleep time'    : node sleep time (in sec) between two measurements
* 'server IP'     : TCP server IP address
* 'server port'   : TCP server port number


```bash
$ ./sensor_node 15 1 127.0.0.1 1234
$ ./sensor_node 21 3 127.0.0.1 1234
$ ./sensor_node 37 2 127.0.0.1 1234
```

We can inspect the sensor data of the data manager thread in the _./sensor_gateway_ terminal window. The storage manager thread will read sensor measurements from a shared data buffer and insert them in a csv-file _data.csv_. Furthermore, all threads can write log-events to a file _gateway.log_.

The sensor nodes can be terminated by running the killall command. When all the sensor nodes are terminated, the gateway server will be automatically shut down as well.

```bash
$  killall sensor_node 
```
