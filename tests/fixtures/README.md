# Message format fixtures

Captured output of the broker, taken from the statusengine-worker repository
(`.claude/specs/`). They are the contract between the broker and the worker: the
worker reads these fields, so a renamed or dropped key is a breaking change.

`test_messageformat.cpp` builds each message from hand made `nebstruct_*` values
and asserts that the produced JSON carries exactly the same keys as the fixture.
Only the shape is compared, the recorded values are not.

Update a fixture only together with a deliberate, documented format change.
