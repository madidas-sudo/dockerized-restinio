RUNNING := $(shell docker ps -a -q -f name=restinio)

.PHONY: build run stop

build: stop
	docker build --target build -t dockerized-restinio/build .
	docker build -t dockerized-restinio/runtime .

run: build
	docker run -d -p 8080:8080 --name restinio dockerized-restinio/runtime

stop:
ifneq ($(RUNNING), )
	docker stop $(RUNNING)
	docker rm $(RUNNING)
endif