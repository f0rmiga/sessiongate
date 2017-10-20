# Session Gate

Session Gate is a Redis module to ease session management using tokens. This module does NOT do user management, don't get confused. In the real world, most of the sessions are related to a user but do your crazy logic to manage the users the way you want.

## How it works

This module provides creation and administration of sessions using tokens. Each session can have more than one payload and those payloads can be accessed individually. A single instance of Redis loaded with this module can handle sessions from multiple applications.

Redis is a high performance, in-memory data structure store. This module is built on top of it, meaning this module operates in a very similar way Redis itself operates.

To access this module, any Redis compatible driver can be used. The commands to operate this module are exposed like any other Redis command.

To know more about Redis modules, follow this [link](http://antirez.com/news/106).

## How to build

The module is written in C and uses Cmake. The scripts `bootstrap.sh` and `build.sh` are convenient scripts that do all the work under the `build/` directory if you have all dependencies installed.

The dependencies are:
- [The Sodium crypto library (libsodium)](https://download.libsodium.org/doc/)

## How to run tests

Tests are located under `tests/` directory and are written in Python.

- Install Python 2.7
- Install [rmtest package](https://github.com/RedisLabs/rmtest) by running `pip install rmtest`

Then run it:
```
$ cd tests/
$ python all.py
```

## Loading the module for use

The module can be loaded in Redis 4+. The most convenient way to do that is by passing --loadmodule parameter when starting the Redis server:

```
$ redis-server --loadmodule <path_to_sessiongate.so>
```

For example, starting Redis open to the world and the Session Gate loaded:
```
$ redis-server --protected-mode no --loadmodule $(pwd)/lib/sessiongate.so
```

Just make sure to pass the right `sessiongate.so` path value to the --loadmodule parameter.

## Commands

### Start a session

Command: `SESSIONGATE.START <sign_key> <ttl>`

- `<sign_key>` is the secret string used by the HMAC algorithm to generate the token signature.
- `<ttl>` is the positive integer that represents the seconds that the session will live. If set to 0, the session expires immediately.

##### Example
```
SESSIONGATE.START 'qwerty' 300
```

Returns: a token that is used to manage the session.

### Set a session TTL

Command: `SESSIONGATE.EXPIRE <sign_key> <token> <ttl>`

- `<sign_key>` is the secret string used by the HMAC algorithm to verify the token signature.
- `<token>` is the token returned by the START command.
- `<ttl>` is the positive integer that represents the seconds that the session will live. If set to 0, the session expires immediately.

##### Example
```
SESSIONGATE.EXPIRE 'qwerty' <token> 300
```

Returns: OK.

### Set a session payload

Command: `SESSIONGATE.PSET <sign_key> <token> <payload_name> <payload_data>`

- `<sign_key>` is the secret string used by the HMAC algorithm to verify the token signature.
- `<token>` is the token returned by the START command.
- `<payload_name>` is the payload name that is used to identify the payload data.
- `<payload_data>` is the payload data. It can be any string, for example, a JSON stringified object.

##### Example
```
SESSIONGATE.PSET 'qwerty' <token> 'user' '{"name":"John Doe"}'
```

Returns: OK.

### Get a session payload

Command: `SESSIONGATE.PGET <sign_key> <token> <payload_name>`

- `<sign_key>` is the secret string used by the HMAC algorithm to verify the token signature.
- `<token>` is the token returned by the START command.
- `<payload_name>` is the payload name that is used to retrieve the payload data.

##### Example
```
SESSIONGATE.PGET 'qwerty' <token> 'user'
```

Returns: a string containing the payload data.

### Delete a session payload

Command: `SESSIONGATE.PDEL <sign_key> <token> <payload_name>`

- `<sign_key>` is the secret string used by the HMAC algorithm to verify the token signature.
- `<token>` is the token returned by the START command.
- `<payload_name>` is the payload name that is used to identify the payload data being deleted.

##### Example
```
SESSIONGATE.PDEL 'qwerty' <token> 'user'
```

Returns: OK.

### End a session

Command: `SESSIONGATE.END <sign_key> <token>`

- `<sign_key>` is the secret string used by the HMAC algorithm to verify the token signature.
- `<token>` is the token returned by the START command.

##### Example
```
SESSIONGATE.END 'qwerty' <token>
```

Returns: OK.

## Specific language drivers

Here is a list of drivers implemented in specific languages to ease the use of the SessionGate module:

### Go

https://github.com/f0rmiga/sessiongate-go
