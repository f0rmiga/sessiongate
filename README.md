# Session Gate

Session Gate is a Redis module to ease session management using tokens.

## How it works

This module provides creation and administration of sessions using tokens. Each session can have more than one payload and those payloads can be accessed individually. A single instance of Redis loaded with this module can handle sessions from multiple applications.

Redis is a high performance, in-memory data structure store. This module is built on top of it, meaning this module operates in a very similar way Redis itself operates.

To access this module, any Redis compatible driver can be used. The commands to operate this module are exposed like any other Redis command.

To know more about Redis modules, follow this [link](http://antirez.com/news/106).

## How to build

The module is written in C++ and uses Cmake. The script `run_build.sh` is a convenient script that does all the work under the `build/` directory if you have all dependencies installed. There is another convenient script called `run_docker_linux_build.sh` that spins off a new Docker container and do all the work under `docker/compile-linux/ubuntu-16.10` directory.

The dependencies are:
- [Crypto++](https://cryptopp.com/) 5.6.3+
- [Boost String](http://www.boost.org/doc/libs/1_63_0/doc/html/string_algo.html)

If you are running Linux, most package managers do not provide the required version of Crypto++. You might want to run the Docker build process or compile the latest version of the library by yourself.
If you are running OS X, [Homebrew](http://brewformulas.org/Cryptopp) got you covered.

## How to run tests

Tests are located under `tests/` directory and are written in Python. The script `run_tests.sh` runs them.

- Install Python 2.7+
- Install [rmtest package](https://github.com/RedisLabs/rmtest) by running `pip install rmtest`
- Run it: `sh run_tests.sh`

## Loading the module for use

The module can be loaded in Redis 4+. The most convenient way to do that is by passing --loadmodule parameter when starting the Redis server:

```
$ redis-server --loadmodule <path_to_libsessiongate.so>
```

Just make sure to pass the right `libsessiongate.so` path value to the --loadmodule parameter.

## Commands

### Start a session

Command: `SESSIONGATE.START <sign_key> <ttl>`

- `<sign_key>` is the secret string used by the HMAC algorithm to generate the token signature.
- `<ttl>` is the positive integer that represents the seconds that the session will live. If set to 0, the session is non-expiable.

##### Example
```
SESSIONGATE.START 'qwerty' 300
```

Returns: a token that is used to manage the session.

### Set a session TTL

Command: `SESSIONGATE.EXPIRE <sign_key> <token> <ttl>`

- `<sign_key>` is the secret string used by the HMAC algorithm to verify the token signature.
- `<token>` is the token returned by the START command.
- `<ttl>` is the positive integer that represents the seconds that the session will live. If set to 0, the session becomes non-expiable.

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
