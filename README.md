# Statusengine naemon/nagios broker module

The Statusengine Broker Module is a small c++ library that gets loaded into your Naemon or Nagios Core.

It will grab all status information, encode them as JSON, and put them into the Gearman Job Server or RabbitMQ. Due to the queuing engine (Gearman or RabbitMQ) your Monitoring Core will not get blocked by a slow database or disk io issues. It is highly recommended to run the Gearman Job Server on the same node as the monitoring core.

Visit the [documentation](https://statusengine.org/broker/) for more information about Statusengine C++ Broker Module

See [CHANGELOG.md](CHANGELOG.md) for what changed between releases.

## Build and Installation

Please compile and install the newest version of naemon or nagios. We need the header files for building the broker.

### Dependencies
#### Meson
https://mesonbuild.com/Getting-meson.html

#### Ubuntu/Debian
```bash
apt install git python3-pip gcc g++ build-essential libglib2.0-dev libgearman-dev uuid-dev libuchardet-dev libjson-c-dev pkg-config libssl-dev librabbitmq-dev
pip3 install meson ninja
```
#### CentOS
```bash
yum install git python-pip gcc gcc-c++ pkgconfig librabbitmq-devel libgearman-devel libuchardet-devel json-c-devel openssl-devel glib2-devel
pip install meson ninja
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
meson setup --buildtype=release build
ninja -C build
```

### Nagios

> **Note:** Nagios support is a deprecation candidate. It is kept for now, but it doubles
> the maintenance cost of some parts of the module (`Nebmodule::DeleteDowntime()` for
> example exists twice in nearly identical form). If you build the broker against Nagios,
> please open an issue and say so, otherwise the Nagios code paths may be removed in a
> future release.

Please make sure you have set up Nagios like described in the [docs](https://statusengine.org/tutorials/install-nagios4-focal/).

#### Ubuntu/Debian
```bash
meson setup -Dnagios=true -Dnagios_include_dir=/opt/nagios/include build
```

#### RHEL/CentOS
```bash
meson setup -Dnagios=true -Dnagios_include_dir=/opt/nagios/include build
```

### Build

```bash
ninja -C build
```

### Installation

```bash
ninja -C build install
```

## Additional meson build flags

### Installation path

The default installation path for the library is /usr/local, which means that the so file will be placed under /usr/local/lib/libstatusengine.so.

You can specify --prefix=/opt/naemon as meson argument to change the path.

### Disable RabbitMQ or Gearman

You can specify -Dgearman=false or -Drabbitmq=false as meson argument to disable gearman or rabbitmq.


## Configuration

Per default everything is disabled and the broker does nothing.

You can look at statusengine.toml as an example configuration file. In your naemon.cfg/nagios.cfg you have to specify the
broker module like this:
```ini
broker_module=/opt/naemon/lib/libstatusengine.so /path/to/statusengine.toml
```

### How long the worker may block the core

If you consume queues (`WorkerCommand`, `WorkerOCHP`, `WorkerOCSP`), the broker applies
those results from inside the monitoring core's event loop. While it does that, the core
does nothing else - it schedules no checks, reaps no results and reads no external
commands. A backlog therefore has to be bounded, or clearing it stops monitoring.

Two settings in `[Worker]` bound it:

| Key | Default | Bounds |
|---|---|---|
| `MaxRuntimeMilliseconds` | `100` | wall clock time of one worker run; `0` disables |
| `MaxWorkerMessagesPerInterval` | `1000000` | messages taken off the queues in one run |

`MaxRuntimeMilliseconds` is the one that matters. When a run stops on it with messages
still queued, the broker asks naemon to run it again on its very next event loop pass, so
the core keeps its responsiveness and the broker keeps its throughput - the gap between
two runs is a single loop iteration, not a second.

Two caveats worth knowing:

* **One message is indivisible.** A bulk message is a single job that has already been
  acknowledged, so it cannot be abandoned half way without throwing check results away.
  A run can therefore overshoot by the cost of the message it is busy with. The guarantee
  is "budget plus one message", and with large bulk messages the second term is the one
  that dominates: measured against naemon in docker with the default 100ms budget, single
  check results gave a longest run of 102ms, while bulk messages of 200 check results gave
  302ms - one such message costs around 220ms all by itself. Both drained 100000 check
  results in around two minutes. If you need a tighter bound, send smaller bulks.
* **`MaxWorkerMessagesPerInterval` counts messages, not check results.** One bulk message
  of 200 check results counts as `1`. That is why it is a poor bound on its own and why
  the time budget exists.

Under nagios `MaxRuntimeMilliseconds` has no effect and is ignored with a warning: nagios
schedules the worker as a recurring one second event and offers no way to ask for an
earlier run, so a budget there could only be paid for out of throughput.

## Message format

### `long_output` on events that have no long output

For the following event types the `long_output` field is a verbatim copy of `output` and
carries no additional information:

* `ContactNotificationData`
* `EventHandlerData`
* `NotificationData`
* `StateChange`
* `SystemCommandData`

The reason is that the underlying naemon/nagios `nebstruct_*` structs simply have no
`long_output` member for these events; only host and service checks have one. The
duplication is kept for backwards compatibility, so that existing consumers do not break.
Do not read a separate long plugin output out of these five event types.

### Compact JSON

Messages are serialised without insignificant whitespace (`{"a":1}` rather than
`{"a": 1, ...}`), which is 8 to 9 percent fewer bytes per message. This is a
whitespace only difference - the JSON is unchanged and any conformant parser
reads it identically. Do not match on the raw payload text.

### `end_time` on acknowledgements

The `AcknowledgementData` message carries `end_time`, the unix timestamp at which
the acknowledgement expires. The key is always present and always an integer, so
a consumer never has to handle it being absent or null:

| Core | Acknowledgement | `end_time` | Meaning |
|---|---|---|---|
| naemon | set with an expiry | `1787692952` | expires at that timestamp |
| naemon | set without one | `0` | does not expire |
| nagios | any | `0` | does not expire |

`0` means the same thing in both cases, so no distinction between the two cores is
needed anywhere - in a consumer or in a database schema. Under naemon `0` is a
real value and not a missing one: `ACKNOWLEDGE_SVC_PROBLEM` produces `0`,
`ACKNOWLEDGE_SVC_PROBLEM_EXPIRE` produces the timestamp. Nagios has no `end_time`
member in `nebstruct_acknowledgement_data` because it has no expiring
acknowledgements at all, so `0` is not a stand-in there either - a nagios
acknowledgement genuinely never expires.

For a consumer this means: `0` - does not expire; anything else - a unix
timestamp, treat it as the expiry.

### `timestamp` on core restart

The `RestartData` message carries the unix timestamp of the restart:

```json
{"object_type": 102, "timestamp": 1787678804}
```

The value comes from naemon's own event time. Consumers should keep treating a
missing field or a `0` as "not set" and fall back to their own clock - older
brokers did not populate it.

## Performance

The numbers below are the broker's own share of the work - the part that runs inside
naemon's single threaded event loop, where every microsecond is one the core does not
spend scheduling checks. naemon's own processing is not included.

Measured on the same machine directly before and after the changes, `--buildtype=release`,
GCC 11.4 on x86-64:

| | before | after | |
|---|---|---|---|
| Encoder, ASCII output | 286 ns | 48 ns | 6.0x |
| Encoder, UTF-8 output | 3997 ns | 48 ns | 83x |
| Encoder, Latin-1 output | 4298 ns | 4248 ns | unchanged |
| Building a service check message | 3449 ns | 2662 ns | -23% |
| Receiving one check result | 2550 ns | 2112 ns | -17% |
| Receiving a bulk of 100 | 435 us | 352 us | -19% |

Messages also got smaller, which is network traffic, queue memory and parsing work on the
consumer side:

| | before | after | |
|---|---|---|---|
| One service check message | 571 B | 519 B | -9.1% |
| Bulk of 100 check results | 63318 B | 58014 B | -8.4% |

Where this comes from:

* Charset detection is skipped when the output already is valid UTF-8, which ASCII is a
  subset of. That covers virtually all real plugin output; previously every string went
  through uchardet's statistical detection.
* The fields of an incoming check result are looked up directly instead of iterating every
  key and running it down a chain of string comparisons.
* JSON is serialised without insignificant whitespace.

### What these numbers do not say

* They are micro benchmarks from one machine and one compiler. The ratios are the useful
  part, the absolute values will differ on your hardware.
* Each timing is a single run. Repeated runs vary by a few percent, so read the timings as
  the order of magnitude they are, not as exact figures. The message sizes are
  deterministic and do reproduce exactly.
* The encoder gain depends on the input. Output that genuinely is not UTF-8 still goes
  through detection and conversion and is no faster - as the Latin-1 row shows.
* On the receive path most of what remains is json-c parsing, roughly 320 us of the 352 us
  in the bulk case. That is not broker code and none of this changed it.

### Reproducing

The benchmark ships with the sources:

```bash
meson setup --buildtype=release -Dtests=true build
ninja -C build
./build/tests/statusengine-bench
```

`--buildtype=release` is required; at `-O0` the numbers are meaningless.

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
apt-get install git python3-pip gcc g++ build-essential libglib2.0-dev libgearman-dev uuid-dev libuchardet-dev libjson-c-dev pkg-config libssl-dev librabbitmq-dev gearman-job-server
pip3 install meson ninja
```

3. Open the Project in Visual Studio Code.
Create a new folder named `build`.
Adjust the path of `${env:HOME}/git/naemon-core-original` in the `tasks.json` and `settings.json`.

Than run the Task `Terminal > Run Task... > configure` to setup `meson`

4. In case VS Code has issues with the include path, make sure the file `"${workspaceFolder}/build/compile_commands.json"` in `settings.json` exists
or adjust the path if necessary.

5. Make sure the Statusengine Broker is loaded in Naemon
```
broker_module=/home/dziegler/git/broker/build/src/libstatusengine.so /home/dziegler/git/broker/statusengine.toml
```

6. Start Debugging using the `Debug Shared Library` configuration

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
