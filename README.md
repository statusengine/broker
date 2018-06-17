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

cd /tmp
git clone https://github.com/statusengine/broker
mkdir build
cd build

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

This is an example config that works perfectly with statusengine worker:
```toml
[Queues]
HostStatus = true
HostCheck = true
ServiceStatus = true
ServiceCheck = true
ServicePerfData = true
StateChange = true
LogData = true
AcknowledgementData = true
FlappingData = true
DowntimeData = true
ContactNotificationMethodData = true
RestartData = true
#SystemCommandData = false
#CommentData = false
#ExternalCommandData = false
#NotificationData = false
#ProgramStatusData = false
#ContactStatusData = false
#ContactNotificationData = false
#EventHandlerData = false
#ProcessData = false
#BulkOCSP = false
#OCSP = false
#BulkOCHP = false
#OCHP = false

# You can specify multiple gearman connections, by adding more [[Gearman]] sections.
[[Gearman]]
URL = "127.0.0.1:4730"

# You can specify multiple rabbitmq connections, by adding more [[Rabbitmq]] sections.
#[[Rabbitmq]]
#Hostname = "localhost"
#Username = "statusengine"
#Password = "statusengine"

[Scheduler]
# Used to fix a nasty scheduler bug in naemon, needed when you often restart naemon
# If next_schedule of a service or host is in the past during startup, we reschedule
# the service/host within $StartupScheduleMax seconds.
StartupScheduleMax = 30
```
