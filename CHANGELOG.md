# Changelog

Усі помітні зміни в цьому репо фіксуються тут.
Формат - [Keep a Changelog](https://keepachangelog.com/uk/1.1.0/), дати в ISO 8601.

## 2026-04-30

### Added

- Block 2 / Lessons 2.3-2.4: VS Code tasks для CMake configure/build і
  видалення локальної `build/` директорії. `CMake: configure and build`
  зроблено default build task для `Ctrl+Shift+B`.
  ([#10](https://github.com/robot-dreams-code/C-PLUS-PLUS-FOR-MILITARY-TECHNOLOGY/pull/10))
- Block 2 / Lessons 2.3-2.6: clang-tidy конфіг, debug/diagnostics tools
  (`gdb`, `gdbserver`, `gdb-multiarch`, `valgrind`) і налаштування clangd через
  `build/compile_commands.json`.
  ([#9](https://github.com/robot-dreams-code/C-PLUS-PLUS-FOR-MILITARY-TECHNOLOGY/pull/9))

### Changed

- Block 2 / Lessons 2.1-2.6: оновлено setup docs під GitHub Template flow,
  додано пояснювальні коментарі до student-facing tooling/config файлів і
  зменшено `.gitignore` до мінімального C++/CMake набору.
  ([#9](https://github.com/robot-dreams-code/C-PLUS-PLUS-FOR-MILITARY-TECHNOLOGY/pull/9))

## 2026-04-25

### Changed

- Devcontainer user setup спростили. Прибрали хардкод `useradd -u 1000`;
  покладаємось на `updateRemoteUserUID` від VS Code і `devcontainers/ci`, який
  синхронізує UID контейнерного користувача з host-ом на старті. Dockerfile
  тепер лише видаляє дефолтного `ubuntu` умовно через `if id ubuntu`, щоб
  звільнити слот UID 1000.
  ([#5](https://github.com/robot-dreams-code/C-PLUS-PLUS-FOR-MILITARY-TECHNOLOGY/pull/5))

### Added

- GitHub Actions workflow `build.yml` через `devcontainers/ci@v0.3`. CI будує
  проєкт у тому самому devcontainer-і, що й локальний VS Code, запускається на
  push у `main` і на кожен PR.
  ([#4](https://github.com/robot-dreams-code/C-PLUS-PLUS-FOR-MILITARY-TECHNOLOGY/pull/4))

## 2026-04-23

### Added

- Пакети `ssh` і `git` у devcontainer з коробки. Базовий git-workflow і
  SSH-операції тепер працюють без донастройки.
  ([#3](https://github.com/robot-dreams-code/C-PLUS-PLUS-FOR-MILITARY-TECHNOLOGY/pull/3))
- Стартові матеріали блоку 2: корневий `CMakeLists.txt`, `homework_04/`
  (starter для wheel odometry), `.devcontainer/` з Dockerfile і
  initialize-скриптом, `preps/` з інструкціями для Linux, macOS, Windows 11
  + WSL2 і Dev Containers CLI.
