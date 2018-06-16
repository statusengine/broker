FROM ubuntu:bionic

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y libc6-dev libglib2.0-dev libgearman-dev uuid-dev libicu-dev libjson-c-dev libssl-dev librabbitmq-dev zip

ENTRYPOINT ["/bin/sh", "-c", "sleep 300"]
