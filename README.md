# Statusengine naemon/nagios broker module

The Statusengine Broker Module is a small c++ library that gets loaded into your Naemon or Nagios Core.

It will grab all status information, encode them as JSON, and put them into the Gearman Job Server or RabbitMQ. Due to the queuing engine (Gearman or RabbitMQ) your Monitoring Core will not get blocked by a slow database or disk io issues. It is highly recommended to run the Gearman Job Server on the same node as the monitoring core.

Visit the [documentation](https://statusengine.org/broker/) for more information about Statusengine C++ Broker Module

## Build and Installation

Please compile and install the newest version of naemon or nagios. We need the header files for building the broker.

### Dependencies

#### Ubuntu/Debian
```bash
apt install git gcc g++ cmake build-essential libglib2.0-dev libgearman-dev uuid-dev libuchardet-dev libjson-c-dev pkg-config libssl-dev librabbitmq-dev
```
#### CentOS
```bash
yum install git gcc gcc-c++ cmake3 pkgconfig librabbitmq-devel libgearman-devel libuchardet-devel json-c-devel openssl-devel glib2-devel
```

### Sources

```bash
cd /tmp
git clone https://github.com/statusengine/broker
cd broker
```

### Naemon
Please make sure you have set up Naemon like described in the [docs](https://statusengine.org/tutorials/install-naemon-focal/).


Then create the make files
```bash
export PKG_CONFIG_PATH=/opt/naemon/lib/pkgconfig/
mkdir build && cd build && cmake .. -DWITH_GEARMAN=ON -DWITH_RABBITMQ=ON
```

### Nagios
Please make sure you have set up Nagios like described in the [docs](https://statusengine.org/tutorials/install-nagios4-focal/).

```bash
mkdir build && cd build && cmake .. -DWITH_GEARMAN=ON -DWITH_RABBITMQ=ON -DNAGIOS_INCLUDE_DIR=/opt/nagios/include
```


### Build

```bash
make -j
```

### Installation

```bash
make install
```

## Additional cmake build flags

### Installation path

The default installation path for the library is /usr/local, which means that the so file will be placed under /usr/local/lib/libstatusengine.so.

You can specify -DCMAKE_INSTALL_PREFIX=/opt/naemon as cmake argument to change the path.

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

### Development with Visual Studio Code

1. Setup Naemon Core
The easiest method is to clone the original Naemon repository and run the `initial Task` as described here:
https://github.com/naemon/naemon-core/tree/master/.vscode


2. Install Statusengine Broker dependencies
```
apt-get install git python3-pip gcc g++ cmake build-essential libglib2.0-dev libgearman-dev uuid-dev libuchardet-dev libjson-c-dev pkg-config libssl-dev librabbitmq-dev gearman-job-server
pip3 install meson ninja
```

3. Open the Project in Visual Studio Code.
Create a new folder named `build`.
Adjust the path of `/home/dziegler/git/naemon-core-original` in the `tasks.json` and `settings.json`.

Than run the Task `Terminal > Run Task... > configure` to setup `meson`

4. Make sure the Statusengine Broker is loaded in Naemon
```
broker_module=/home/dziegler/git/broker/build/src/libstatusengine.so /home/dziegler/git/broker/statusengine.toml
```

5. Start Debugging using the `Debug Shared Library` configuration

![vscode debugger via WSL2](/vscode.png)


## License

    statusengine - the missing event broker
    Copyright (C) 2019  The statusengine team

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
