FROM ubuntu:bionic

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y cmake gcc g++ gdb build-essential libglib2.0-dev libgearman-dev uuid-dev libicu-dev libjson-c-dev pkg-config libssl-dev librabbitmq-dev gearman-job-server gearman-tools automake gperf help2man libtool rabbitmq-server wget tar && \
    apt-get clean

RUN useradd -d /opt/naemon -m -s /bin/bash --system naemon && \
    wget -O /v1.0.7.tar.gz https://github.com/naemon/naemon-core/archive/v1.0.7.tar.gz  && \
    echo "6e99d3b9074f8fc2ef8516de611b3759c8c7bef432559b143152dafc17a104b8 /v1.0.7.tar.gz" | sha256sum -c - && \
    mkdir -p /build && tar xf /v1.0.7.tar.gz -C /build --strip-components 1 && \
    cd /build && ./autogen.sh --prefix=/opt/naemon --with-naemon-user=naemon --with-naemon-group=naemon --with-pluginsdir=/usr/lib/nagios/plugins && \
    make -j2 && make install && \
    mkdir -p /opt/naemon/var/ /opt/naemon/var/cache/naemon /opt/naemon/var/spool/checkresults /opt/naemon/etc/naemon/module-conf.d && \
    chown naemon:naemon /opt/naemon/var /opt/naemon/etc  -R && \
    cd / && rm -r /build && rm /v1.0.7.tar.gz

COPY devtools/buildenv/statusengine.toml /opt/naemon/etc/
COPY devtools/buildenv/statusengine.cfg /opt/naemon/etc/naemon/module-conf.d/
COPY devtools/buildenv/wait-for-it.sh devtools/buildenv/queuestatus /usr/bin/
RUN mkdir -p /broker/source /broker/build && chmod +x /usr/bin/wait-for-it.sh /usr/bin/queuestatus

CMD ["/usr/bin/wait-for-it.sh", "rabbit:5672", "--", "/opt/naemon/bin/naemon", "/opt/naemon/etc/naemon/naemon.cfg"]

COPY . /broker/source/

RUN cd /broker/build && export PKG_CONFIG_PATH=/opt/naemon/lib/pkgconfig && cmake -DCMAKE_INSTALL_PREFIX:PATH=/opt/naemon ../source && make -j2 && make install

USER naemon
