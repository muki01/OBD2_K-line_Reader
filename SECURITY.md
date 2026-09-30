# Security Policy

## Supported Versions

Security fixes are applied to the latest code on the `main` branch and the most recent release.

## Reporting a Vulnerability

If you find a security issue, for example in the web server, OTA update endpoint or WiFi configuration handling, **please do not open a public issue**.

Instead, email **muksin.muksin04@gmail.com** with:

- A description of the issue and its potential impact
- Steps to reproduce (board, build, configuration)
- Any suggested fix, if you have one

You will receive a response as soon as possible, and credit in the release notes if you wish.

## Hardening Tips for Users

- Change the default Access Point password (`AP_password` in `WebServer_Code/WEB_SERVER.ino`) before regular use.
- The web dashboard and OTA endpoint have no authentication. Only connect the device to networks you trust.
- Unplug the device from the vehicle when not in use.
