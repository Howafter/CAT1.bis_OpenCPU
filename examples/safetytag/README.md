# Safetytag Luat compatibility skeleton

Build from the repository root with `build.bat build PROJECT=safetytag`. This is a
registration and RTOS-adapter smoke test, **not a usable safety-tag firmware**.
No GNSS, alarm, network, cloud, GPIO or production configuration is started.
The existing `PROJECT=app` target remains unchanged.

- `compat/luat/include/luat_init_export.h` retains `INIT_HW_EXPORT`,
  `INIT_DRV_EXPORT` and `INIT_TASK_EXPORT` syntax. `app.ld` uses KEEP and explicit
  `__start/__stop` boundaries; `src/user_main.c` invokes the runner once from
  the SDK's appUserTask. HW runs before DRV, then TASK; levels ascend within a
  stage. Equal levels have **link order**, not a promised source order.
- Every C file containing `INIT_*_EXPORT` must be listed as a direct `obj-y`
  input in this project's Makefile. Do not archive registration-only objects:
  static-library extraction occurs before KEEP can preserve their sections.
- Add non-registration adapter C files to the compat library explicitly.
  `luat_rtos.h` currently declares only task create/delete and millisecond
  sleep; other Luat headers and APIs are **not implemented** yet. Verify
  signature/return/timeout/ISR semantics against the selected LIOT SDK before
  adding them. The probe in `src/safetytag_boot.c` exercises DRV and TASK
  registrations without enabling hardware.
- A TASK callback must return after starting its own long-lived tasks, or
  subsequent registration callbacks cannot run. Avoid re-entering the runner.
- Inspect `gccout/safetytag/safetytag_*.map` for `luat_init_drv`,
  `luat_init_task`, their objects, and `__start/__stop` symbols. On device
  verify the probe logs appear exactly once in DRV-before-TASK order.

The older safetytag project is still an incomplete stage-1 port. Port its
business modules and hardware mappings separately; do not copy its old
`luat_init_section.c` marker workaround or assume old board pins match.
