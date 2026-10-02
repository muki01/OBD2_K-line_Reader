# Contributing to OBD2 K-Line Reader

Thank you for taking the time to contribute! 🎉 Every bug report, vehicle test, fix and idea makes this project better for the whole car-hacking and maker community.

By participating, you agree to follow our [Code of Conduct](CODE_OF_CONDUCT.md).

## Ways to Contribute

| | |
|---|---|
| 🚗 **Report a tested vehicle** | Tell us which cars work (or don't). Use the **Vehicle compatibility report** issue template. |
| 🐛 **Report a bug** | Use the **Bug report** template and include serial logs whenever possible. |
| 💡 **Suggest a feature** | Use the **Feature request** template. |
| 🔧 **Submit code** | New PIDs, board ports (STM32, RP2040, …), protocol fixes, performance improvements. |
| 📝 **Improve docs** | Typos, clearer instructions, wiring photos, translations of guides. |

## Reporting Bugs

Before opening an issue, please search [existing issues](https://github.com/muki01/OBD2_K-line_Reader/issues). A good report includes:

- Build (`Basic_Code` or `WebServer_Code`) and commit / release version
- Board (e.g. ESP32-S3 DevKitC-1, Arduino Nano) and Arduino core version
- Interface circuit used (transistor, LM393, L9637D, …)
- Vehicle make, model, year and engine
- Selected and detected protocol
- **Serial debug output** (enable `DEBUG_Serial`). The raw `➡️ Sending Data` / received bytes are the most useful information.

## Development Workflow

1. **Fork** the repository and create a branch from `main`:
   ```bash
   git checkout -b feature/my-improvement
   ```
2. Make your changes, keeping them **focused**: one fix or feature per pull request.
3. **Test on real hardware** when your change touches communication code, and say in the PR which vehicle / protocol you tested on.
4. Make sure both sketches still **compile** for the boards they support.
5. Commit with a clear message, e.g. `Add PID 0x5C oil temperature`.
6. Push and open a **Pull Request** against `main`, filling in the template.

## Coding Guidelines

- Follow the existing style of the file you are editing (naming, indentation, comment density).
- Wrap constant debug strings in `F()` to save RAM on AVR boards.
- Keep timing-critical K-Line code **non-blocking** where possible and do not change protocol timings without testing.
- Put board-specific code behind `#ifdef ESP32` / `#elif defined(ESP8266)` / `#elif defined(ARDUINO)` guards.
- Do not commit binaries, credentials or personal data (e.g. your own VIN) in code or logs.

## Web Dashboard Changes

The web UI source lives in **[OBD2 Diagnostic UI](https://github.com/muki01/OBD2-Diagnostic-UI)**. Please open UI pull requests there; `WebServer_Code/data` only holds the gzipped build.

## License

By contributing, you agree that your contributions are licensed under the [GNU General Public License v3.0](LICENSE), and that the author may also offer them under a commercial license.
