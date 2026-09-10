FROM ubuntu:24.04

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y gcc g++ gdb build-essential meson ninja-build libglib2.0-dev libgearman-dev uuid-dev libuchardet-dev libjson-c-dev pkg-config libssl-dev librabbitmq-dev gearman-job-server gearman-tools automake gperf help2man libtool rabbitmq-server wget tar && \
    apt-get clean

ENV NAEMON_VERSION=1.5.2 NAEMON_CHECKSUM=7cc09f25d6350a778fa475f1fe60b4dbb2386a79c6c4e8151446231ece0cd345

RUN useradd -d /opt/naemon -m -s /bin/bash --system naemon && \
    wget -O /v${NAEMON_VERSION}.tar.gz https://github.com/naemon/naemon-core/archive/v${NAEMON_VERSION}.tar.gz  && \
    echo "${NAEMON_CHECKSUM} /v${NAEMON_VERSION}.tar.gz" | sha256sum -c - && \
    mkdir -p /build && tar xf /v${NAEMON_VERSION}.tar.gz -C /build --strip-components 1 && \
    cd /build && ./autogen.sh --prefix=/opt/naemon --with-naemon-user=naemon --with-naemon-group=naemon --with-pluginsdir=/usr/lib/nagios/plugins && \
    make -j2 && make install && \
    mkdir -p /opt/naemon/var/ /opt/naemon/var/cache/naemon /opt/naemon/var/spool/checkresults /opt/naemon/etc/naemon/module-conf.d && \
    chown naemon:naemon /opt/naemon/var /opt/naemon/etc  -R && \
    cd / && rm -r /build && rm /v${NAEMON_VERSION}.tar.gz

COPY devtools/buildenv/statusengine.toml /opt/naemon/etc/
COPY devtools/buildenv/statusengine.cfg /opt/naemon/etc/naemon/module-conf.d/
COPY devtools/buildenv/wait-for-it.sh devtools/buildenv/queuestatus /usr/bin/
RUN mkdir -p /broker/source /broker/build && chmod +x /usr/bin/wait-for-it.sh /usr/bin/queuestatus

CMD ["/usr/bin/wait-for-it.sh", "rabbit:5672", "--", "/opt/naemon/bin/naemon", "/opt/naemon/etc/naemon/naemon.cfg"]

COPY . /broker/source/

RUN export PKG_CONFIG_PATH=/opt/naemon/lib/pkgconfig && \
    meson setup --buildtype=release --prefix=/opt/naemon /broker/build /broker/source && \
    ninja -C /broker/build install

USER naemon
