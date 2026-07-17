# HTTP/HTTPS Server

A HTTP and HTTPS server written in C++ using ASIO for networking.
It runs a plaintext HTTP listener and a TLS-encrypted HTTPS listener side by
side on separate ports.

## Dependencies

- [CMake](https://cmake.org/) 
- [vcpkg](https://github.com/microsoft/vcpkg)

#### Installed through vcpkg
- [ASIO](https://think-async.com/Asio/) — networking
- [nlohmann-json](https://github.com/nlohmann/json) — JSON serialization
- [fmt](https://github.com/fmtlib/fmt) — string formatting
- [sqlitecpp](https://github.com/srombauts/sqlitecpp) - database
- [OpenSSL](https://www.openssl.org/) — TLS/HTTPS support

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
protocol — for example `http://localhost:6767/api/user/getAll` and
`https://localhost:6969/api/user/getAll` hit the same handler.

> The HTTPS certificate is self-signed, so clients will flag it as untrusted.



