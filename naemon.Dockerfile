FROM ubuntu:focal

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y cmake gcc g++ gdb build-essential libglib2.0-dev libgearman-dev uuid-dev libicu-dev libjson-c-dev pkg-config libssl-dev librabbitmq-dev gearman-job-server gearman-tools automake gperf help2man libtool rabbitmq-server wget tar libuchardet-dev git && \
    apt-get clean

ENV NAEMON_VERSION=1.3.1

RUN useradd -d /opt/naemon -m -s /bin/bash --system naemon && \
    wget -O /v${NAEMON_VERSION}.tar.gz https://github.com/naemon/naemon-core/archive/v${NAEMON_VERSION}.tar.gz  && \
    mkdir -p /build && tar xf /v${NAEMON_VERSION}.tar.gz -C /build --strip-components 1 && \
    cd /build && ./autogen.sh --prefix=/opt/naemon --with-naemon-user=naemon --with-naemon-group=naemon --with-pluginsdir=/usr/lib/nagios/plugins && \
    make -j2 && make install && \
    mkdir -p /opt/naemon/var/ /opt/naemon/var/cache/naemon /opt/naemon/var/spool/checkresults /opt/naemon/etc/naemon/module-conf.d && \
    chown naemon:naemon /opt/naemon/var /opt/naemon/etc  -R && \
    cd / && rm -r /build && rm /v${NAEMON_VERSION}.tar.gz

COPY devtools/buildenv/statusengine.toml /opt/naemon/etc/
COPY devtools/buildenv/statusengine.cfg /opt/naemon/etc/naemon/module-conf.d/
COPY devtools/buildenv/wait-for-it.sh devtools/buildenv/queuestatus /usr/bin/
RUN mkdir -p /broker && chmod +x /usr/bin/wait-for-it.sh /usr/bin/queuestatus

CMD ["/usr/bin/wait-for-it.sh", "rabbit:5672", "--", "/opt/naemon/bin/naemon", "/opt/naemon/etc/naemon/naemon.cfg"]

COPY --chown=naemon:naemon . /broker/
RUN chown -R naemon:naemon /broker

USER naemon
RUN rm -rf /broker/build && \
    mkdir -p /broker/build && \
    cd /broker/build && \
    export PKG_CONFIG_PATH=/opt/naemon/lib/pkgconfig && \
    cmake ..  -DWITH_GEARMAN=ON -DWITH_RABBITMQ=ON && make -j
