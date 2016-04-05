# cpp-rpc [![Build Status](https://travis-ci.org/pdxjohnny/cpp-rpc.svg?branch=master)](https://travis-ci.org/pdxjohnny/cpp-rpc)


Install
---

From source:

```bash
git clone https://github.com/pdxjohnny/cpp-rpc
make clean && make && sudo make install
```

From Travis-CI build
```bash
wget https://github.com/pdxjohnny/cpp-rpc/releases/download/0.1/librpc.tar.gz
sudo tar xvf librpc.tar.gz -C /
rm librpc.tar.gz
```


Building With librpc
---

The simplest example is in example/simple.ccp and once you have run `make &&
make install` then it will compile with the following command.

```bash
g++ -static test.cpp -lrpc -o test
```


Apache Benchmark
---

Single Threaded server

![AB](http://i.imgur.com/Uqq2oVH.gif)

```
This is ApacheBench, Version 2.3 <$Revision: 1528965 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking 192.168.1.20 (be patient)
Completed 1000 requests
Completed 2000 requests
Completed 3000 requests
Completed 4000 requests
Completed 5000 requests
Completed 6000 requests
Completed 7000 requests
Completed 8000 requests
Completed 9000 requests
Completed 10000 requests
Finished 10000 requests


Server Software:        cpp-rpc
Server Hostname:        192.168.1.20
Server Port:            40000

Document Path:          /hello
Document Length:        13 bytes

Concurrency Level:      1
Time taken for tests:   34.609 seconds
Complete requests:      10000
Failed requests:        0
Total transferred:      320000 bytes
HTML transferred:       130000 bytes
Requests per second:    288.94 [#/sec] (mean)
Time per request:       3.461 [ms] (mean)
Time per request:       3.461 [ms] (mean, across all concurrent requests)
Transfer rate:          9.03 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        1    2   0.7      1      18
Processing:     1    2   0.8      2      22
Waiting:        1    1   0.7      1      21
Total:          2    3   1.1      3      23
WARNING: The median and mean for the initial connection time are not within a
normal deviation
        These results are probably not that reliable.

Percentage of the requests served within a certain time (ms)
  50%      3
  66%      3
  75%      4
  80%      4
  90%      5
  95%      6
  98%      7
  99%      7
 100%     23 (longest request)
```

