dockerized-restinio
---
[![Build Status](https://api.travis-ci.org/tpiekarski/dockerized-restinio.svg?branch=master)](https://travis-ci.org/tpiekarski/dockerized-restinio)

*A project for running [RESTinio](https://github.com/Stiffstream/restinio), 
an embedded and lightweight HTTP/Websocket server written in C++14 inside a 
[multi-stage build](https://docs.docker.com/develop/develop-images/multistage-build/) 
[Docker](https://www.docker.com/) container with separated build and runtime stages.*

## To build and run the container

```bash
make run
```

## Build dockerized-restinio/build 
```
docker build --target build -t dockerized-restinio/build . 
```

## Build dockerized-restinio/runtime
```
docker build -t dockerized-restinio/runtime .
```

## Run dockerized RESTinio
```
docker run -d -p 8080:8080 --name restinio dockerized-restinio/runtime
```

## Readyness Probe
```
curl -vX GET localhost:8080/ready
```

## Running with custom address and port
For running the container and RESTinio inside it with custom address and port there are two options exposed to set up and use different values from the default ones.

|#|Option|Default|
|---|---|---|
|1.|--address|0.0.0.0|
|2.|--port|8080|

### Example
```
docker run -d --rm -p 8085:8085 --expose 8085 dockerized-restinio/runtime --port 8085
curl -vX GET localhost:8085/ready
```