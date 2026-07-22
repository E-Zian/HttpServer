# HTTP/HTTPS Server

A HTTP and HTTPS server written in C++ using ASIO for networking.
It runs a plaintext HTTP listener and a TLS-encrypted HTTPS listener side by
side on separate ports.

## Features

- **HTTP/1.1 and HTTPS** — plaintext and TLS listeners run side by side on separate ports
- **TLS / HTTPS** via OpenSSL
- **Token-bucket rate limiting** per client IP — a 100-request burst that refills at
  1 tokens/second, with periodic sweeping of idle clients
- **SQLite persistence** via SQLiteCpp
- **Keep-alive** persistent connections, with request timeouts and size limits
  (maximum header and body sizes)
- Asynchronous I/O built on C++20 coroutines (Asio), with the event loop running
  across a thread pool for multi-core scaling

## Dependencies

- [CMake](https://cmake.org/) 
- [vcpkg](https://github.com/microsoft/vcpkg)

#### Installed through vcpkg
- [ASIO](https://think-async.com/Asio/) — networking
- [nlohmann-json](https://github.com/nlohmann/json) — JSON serialization
- [fmt](https://github.com/fmtlib/fmt) — string formatting
- [sqlitecpp](https://github.com/srombauts/sqlitecpp) - database
- [OpenSSL](https://www.openssl.org/) — TLS/HTTPS support
- gtest — unit test framework (GoogleTest + GoogleMock)
## Prerequisites

### 1. Install vcpkg
```bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat  # Windows
```

### 2. Set the `VCPKG_ROOT` environment variable
```bash
# Windows (System Environment Variables)
VCPKG_ROOT = C:\path\to\vcpkg
```

> The CMake preset uses `$env{VCPKG_ROOT}` to locate the vcpkg toolchain file. The build will fail if this variable is not set.

## Building

### 1. Clone the repository
```bash
git clone https://github.com/E-Zian/HttpServer.git
cd HttpServer
```

### 2. Install dependencies via vcpkg
```bash
vcpkg install
```
This reads the `vcpkg.json` manifest and installs the stated dependencies automatically.

### 3. Generate a self-signed TLS certificate

The server listens over HTTPS, so it needs a certificate (`server.crt`) and a
private key (`server.key`) at startup. For local development you can generate a
self-signed pair with OpenSSL:

```bash
openssl req -x509 -newkey rsa:2048 -nodes -keyout server.key -out server.crt -days 365 -subj "/CN=localhost"
```
> **Windows:** if `openssl` is not on your `PATH`, it ships with Git for Windows.
> Run it by full path from PowerShell:
> ```powershell
> & "C:\Program Files\Git\usr\bin\openssl.exe" req -x509 -newkey rsa:2048 -nodes -keyout server.key -out server.crt -days 365 -subj "/CN=localhost"
> ```
What the flags do:

| Flag | Meaning |
| --- | --- |
| `req -x509` | Produce a self-signed certificate (not a certificate signing request) |
| `-newkey rsa:2048` | Generate a new 2048-bit RSA key pair |
| `-nodes` | Do not password-protect the private key (so the server can load it unattended) |
| `-keyout server.key` | Write the private key to `server.key` |
| `-out server.crt` | Write the certificate to `server.crt` |
| `-days 365` | Certificate is valid for 365 days |
| `-subj "/CN=localhost"` | Set the certificate identity (Common Name) to `localhost` |



Place `server.crt` and `server.key` in the server's working directory so it can
find them at startup. Because the certificate is self-signed, clients will warn
that it is untrusted — in a browser proceed past the warning, with `curl` use
`-k`, and in Postman disable **SSL certificate verification**.

## Running

The server starts two listeners at once, one per protocol:

| Protocol | Port | Base URL |
| --- | --- | --- |
| HTTP  | `6767` | `http://localhost:6767` |
| HTTPS | `6969` | `https://localhost:6969` |

Both listeners share the same routes, so any endpoint is reachable over either
protocol — for example `http://localhost:6767/api/user` and
`https://localhost:6969/api/user` hit the same handler.

> The HTTPS certificate is self-signed, so clients will flag it as untrusted.

## API

All request and response bodies are JSON. Successful responses return the affected
resource plus a `message`; errors return `{ "errorMessage": "..." }`.

Every response also carries rate-limit headers so clients can see their remaining quota:

| Header | Meaning |
| --- | --- |
| `x-token-capacity` | Maximum request tokens for your IP |
| `x-tokens-left` | Tokens remaining before requests are throttled |

### Users

| Method | Path | Description |
| --- | --- | --- |
| GET | `/api/user` | List all users |
| POST | `/api/user` | Create a user |
| GET | `/api/user/:id` | Get a user by id |
| PUT | `/api/user/:id` | Update a user |
| DELETE | `/api/user/:id` | Delete a user |

**Create a user** — `POST /api/user`

Request:
```json
{ "user": { "name": "alice", "email": "alice@example.com" } }
```
Response `200 OK`:
```json
{
  "user": { "id": 1, "name": "alice", "email": "alice@example.com" },
  "message": "User created successfully"
}
```
Errors: `400` — missing `user` field, malformed JSON, or the email already exists.

Get, update, and delete follow the same shape. `:id` must be an integer — a
non-numeric id returns `400`, and a user that does not exist returns `404`.

### Pokemon

| Method | Path | Description |
| --- | --- | --- |
| GET | `/api/pokemon` | List all pokemon |
| POST | `/api/pokemon` | Create a pokemon |
| GET | `/api/pokemon/:id` | Get a pokemon by id |
| PUT | `/api/pokemon/:id` | Update a pokemon |
| DELETE | `/api/pokemon/:id` | Delete a pokemon |

**Create a pokemon** — `POST /api/pokemon`

Request:
```json
{ "pokemon": { "name": "pikachu" } }
```
Response `200 OK`:
```json
{
  "pokemon": { "id": 1, "name": "pikachu" },
  "message": "Pokemon created successfully"
}
```
Errors: `400` — missing `pokemon` field, malformed JSON, or the pokemon already exists.

### Status codes

| Code | Meaning |
| --- | --- |
| `200` | Success |
| `400` | Bad request — malformed JSON, missing field, or invalid id |
| `404` | Resource not found |
| `408` | Request timed out |
| `413` | Request body too large |
| `429` | Too many requests — rate limit exceeded |
| `431` | Request headers too large |
| `500` | Internal server error |

### Utility routes

| Method | Path | Description |
| --- | --- | --- |
| GET | `/` | Basic test response |
| GET | `/testing` | Plain-text test route |
| GET | `/json` | Sample JSON response |

## Testing

Unit tests use **GoogleTest** and **GoogleMock**. The controllers are tested against a
mocked repository, so the suite needs no running database.

Build the project, then run the tests with CTest from your build directory:

```bash
ctest --test-dir out/build/x64-debug --output-on-failure
```

Or run the test executable directly for the full GoogleTest report:

```bash
out/build/x64-debug/test/ServerTest        # ServerTest.exe on Windows
```



