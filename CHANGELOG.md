# Changelog

All notable changes to the Statusengine broker module are recorded here. The format
follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [Unreleased]

Everything since v4.0.4. The headline reasons to upgrade are the RabbitMQ 4 support and a
set of memory errors that could take the monitoring core down with them.

### Fixed

- **RabbitMQ 4 support.** Queues and the exchange are now declared durable by default.
  RabbitMQ 4 refuses to declare a queue that is neither durable nor exclusive, so the
  broker previously failed to connect at all against a 4.x server; 3.13 warns about it.
  The options existed before but defaulted to off. This stores the queue and exchange
  *definitions* on disk, not the events - messages are still published transient, so the
  queues buffer in RAM and a broker restart still empties them, as before.
- **Character set conversion did the opposite of its job.** An inverted comparison meant
  non-UTF-8 plugin output was passed through unconverted, while output that already was
  UTF-8 went through a conversion. The same function also freed a string owned by
  uchardet, freed a buffer through a pointer iconv had advanced past its start, and used
  iconv's return value as if it were a length. Any of those can corrupt the heap of the
  monitoring core.
- **Mismatched allocators on incoming check results.** Strings handed to naemon were
  allocated with `new[]` but released by naemon with `free()`. Also fixes a leak of
  `perf_data` when a message carried no `output`.
- **RabbitMQ connection leak.** Every failed reconnect leaked roughly 9 kB. Since a send
  retries the connection for every message, this grew without bound whenever the broker
  was unreachable.
- **Crash on startup failure.** If one message handler failed to connect, destroying the
  remaining ones dereferenced a connection that had never been opened.
- **The worker loop could freeze naemon.** A handler reporting more work without having
  processed anything - which the gearman worker does when its socket would block - spun
  the loop forever inside naemon's event loop, so the core stopped scheduling entirely.
  The gearman poll that goes with it was also unbounded and is now capped.
- **Log levels were wrong in both directions.** At `Level = Error` warnings were still
  written, and at `Level = Info` - the most verbose setting - warnings were dropped.
- The module title was overwritten by the copyright string in the naemon module info.
- **The gearman client now reconnects after a job server restart.** It never did:
  `gearman_client_add_servers()` ran once at startup and a failed send had no recovery
  path, so once the job server had been restarted the broker stopped delivering to gearman
  until naemon itself was restarted. The worker side was unaffected, which made this easy
  to miss - libgearman resets the worker's connection internally but does nothing for the
  client.
- **An unreachable gearman job server no longer floods the log.** Every failed message
  produced its own line - measured at over 350 in five minutes on a small test
  installation, and it scales with the number of checks. An outage is now reported when it
  starts and then at most every five minutes while it lasts, with a count of failed
  attempts, and once more when it recovers. Worker side errors are named via
  `gearman_strerror()` instead of being reported as "Unknown gearman worker error: 12".

### Changed

- **Messages are serialised without insignificant whitespace**, which is 8 to 9 percent
  fewer bytes. This is a whitespace only difference; the JSON is unchanged and any
  conformant parser reads it identically. Do not match on the raw payload text.
- **`RestartData` now carries the restart timestamp.** Consumers should keep treating a
  missing value or a `0` as "not set", as older brokers do not send it.
- **The module installs into `<prefix>/lib`** again. The move to meson had changed this to
  a multiarch subdirectory, where naemon would not find it at the documented path.
- Built as C++17, and the vendored toml11 was updated from the 2018 version to 4.4.0.
- The reported module version now comes from the build instead of a hardcoded string.
  Version numbers had drifted apart across four places.
- Nagios support is a deprecation candidate. It still builds and is still tested in CI; if
  you rely on it, please say so in an issue.

- **`AcknowledgementData` now carries `end_time`**, the point at which an acknowledgement
  expires. Under naemon this is the value from the core, where `0` means it does not
  expire. Under nagios it is always `0`: nagios has no expiring acknowledgements, so that
  is not a placeholder but the truth for every acknowledgement it reports. Consumers need
  no case distinction between the two cores.

### Added

- A unit test suite, run in CI both plain and under the address and undefined behaviour
  sanitizers. The recorded message formats are pinned against fixtures, so an accidental
  change to the wire format fails the build.
- Micro benchmarks for the message paths, see the Performance section in the README.
- The vendored naemon and nagios headers used for building without an installed core were
  six years old (naemon 1.0.6, nagios 4.4.5) and are now naemon 1.5.2 and nagios 4.5.14.
  The test suite also builds and runs against the nagios headers now, which it never did.
- A GitHub Actions workflow covering current Ubuntu and Debian releases and all build
  options. The configuration it replaces still invoked cmake, which the project dropped in
  2020, was GitLab CI in a repository hosted on GitHub, and targeted distributions that
  have all reached end of life.

### Performance

Measured with the included benchmark; see the README for the caveats that go with these
numbers.

- Character set handling on output that already is UTF-8 or ASCII: 6x to 83x faster,
  because detection is skipped entirely. Output that genuinely is not UTF-8 is unchanged.
- Building a service check message: 23 percent faster.
- Receiving check results: 17 to 19 percent faster overall. Most of what remains is JSON
  parsing inside json-c.
- Messages are 8 to 9 percent smaller.
