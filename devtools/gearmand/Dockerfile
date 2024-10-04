FROM ubuntu:focal

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y gearman-job-server gearman-tools && apt-get clean

EXPOSE 4730
USER gearman
CMD ["/usr/sbin/gearmand", "--log-file=none"]
