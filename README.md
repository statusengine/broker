# statusengine naemon/nagios broker module (alpha)

For a production ready broker module [click here](https://github.com/statusengine/module).

The Statusengine Broker Module is a small c++ library that gets loaded into your Naemon or Nagios Core.

It will grab all status information, encode them as JSON, and put them into the Gearman Job Server or RabbitMQ. Due to the queuing engine (Gearman or RabbitMQ) your Monitoring Core will not get blocked by a slow database or disk io issues. It is highly recommended to run the Gearman Job Server on the same node as the monitoring core.

Visit the [documentation](https://statusengine.org/) for more information about Statusengine Broker Module

## Build and Installation

Please compile and install the newest version of naemon or nagios. We need the header files for building the broker.

### Dependencies
```bash
apt install cmake gcc g++ build-essential libglib2.0-dev libgearman-dev uuid-dev libicu-dev libjson-c-dev pkg-config libssl-dev librabbitmq-dev
```

### Sources

```bash
cd /tmp
git clone https://github.com/statusengine/broker
mkdir build
cd build
```

### Naemon

If you didn't install naemon to /usr or /usr/local you have to create a symlink for naemon.pc, so that cmake can find the header files.

In this example I installed naemon to /opt/naemon:
```bash
ln -s /opt/naemon/lib/pkgconfig/naemon.pc /usr/local/lib/pkgconfig/naemon.pc
```

Then create the make files
```bash
cmake ../broker
```

### Nagios

```bash
cmake -DBUILD_NAGIOS=ON -DNAGIOS_INCLUDE_DIR=/opt/nagios/include ../broker
```

### Build

```bash
make -j2
```

### Installation

```bash
make install
```

## Additional cmake build flags

### Installation path

The default installation path for the library is /usr/local, which means that the so file will be placed under /usr/local/lib/libstatusengine.so.

You can specify -DCMAKE_INSTALL_PREFIX:PATH=/opt/naemon as cmake argument to change the path.

### Disable RabbitMQ or Gearman

You can specify -DWITH_GEARMAN=OFF or -DWITH_RABBITMQ=OFF as cmake argument to disable gearman or rabbitmq.


## Configuration

Per default everything is disabled and the broker does nothing.

You can look at statusengine.toml as an example configuration file. In your naemon.cfg/nagios.cfg you have to specify the
broker module like this:
```ini
broker_module=/opt/naemon/lib/libstatusengine.so /path/to/statusengine.toml
```

## Developer build + test

If you want to build and test the broker, you can use the docker-compose configuration:
```bash
docker-compose up --build
```

This will create docker containers with naemon, rabbitmq and gearman. The broker module will be build on the naemon container. The up command will also run the containers, so you can test if it works.

For deeper inspection you can switch into the naemon container on a second console:
```bash
docker exec -t -i broker_naemon_1 /bin/bash
```

I have also placed a small script inside the container that shows you the queue status of rabbitmq and gearman:
```bash
docker exec -t -i broker_naemon_1 /usr/bin/queuestatus
```

## License

    statusengine - the missing event broker
    Copyright (C) 2018  The statusengine team

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
