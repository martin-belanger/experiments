# NVMe Controller Ownership Registry — Implementation Plan

**Authors:** Martin Belanger, Claude (claude-sonnet-4-6)
**Started:** 2026-05-21
**Status:** Ready for community review

---

## 1. Problem Statement

Multiple independent orchestration mechanisms can establish NVMe-oF controller connections on the same Linux host:

- Manual `nvme connect` invocations (stateless, one-shot)
- `nvme-stas` (stateful daemon — `stafd` / `stacd`)
- NBFT firmware boot connections
- Future orchestrators (e.g., `nvme-discoverd`)

All connected controllers appear in a single flat namespace (`/dev/nvmeX`, `/sys/class/nvme/nvmeX`) with no indication of which subsystem created or manages each one. Commands like `nvme disconnect-all` are therefore indiscriminate and dangerous.

This plan implements a **userspace ownership registry** that allows orchestrators to declare ownership of controllers and respect each other's boundaries.

The primary purpose of the registry is to **prevent accidental disconnection** of NVMe-oF controllers managed by one orchestrator by another. It is a **collaborative tool, not an enforcement mechanism**: all participants are assumed to be cooperative. No cryptographic or OS-level enforcement is possible since all orchestrators run as root. An orchestrator that ignores the registry can already disconnect any controller today without it — the registry gives well-behaved tools the information they need to avoid doing so accidentally.

---

## 2. Goals and Non-Goals

### In Scope

- Administrative ownership tracking of NVMe-oF controllers
- Safe multi-orchestrator coexistence
- Ownership-aware disconnect semantics (`nvme disconnect-all --owner <name>`)
- Userspace-only implementation (no kernel changes)
- Runtime coordination state (not persistent configuration)

### Explicitly Out of Scope

- Kernel NVMe driver changes
- Namespace ownership
- Filesystem permissions or access control
- Security policy enforcement
- Modifying NBFT firmware table entries (`/sys/firmware/acpi/tables/`) — those are read-only firmware data describing boot-time NVMe controllers; the ownership registry is separate and NBFT-configured controllers must participate in it (see Section 13)
- Persistent ownership across reboots
- Automatic ownership registration for PCIe and apple-nvme controllers (see Transport scope in Section 8)

---

## 3. Code Location

All implementation goes in:

```
nvme-cli/libnvme/src/nvme/
```

Both `nvme-cli` and `nvme-stas` already depend on `libnvme`, making it the natural shared library for registry access. This avoids duplicated implementations and ensures consistent behavior across all orchestrators.

New files:

```
libnvme/src/nvme/registry.h    — public API declarations
libnvme/src/nvme/registry.c    — implementation
```

Public symbols must be added to `libnvmf.ld` — registry support is only available when fabrics support is enabled (`-Dfabrics=enabled`).

---

## 4. Registry Layout

### Directory

```
/run/nvme/registry/
```

`/run/` is appropriate because ownership is runtime orchestration state (not static configuration), naturally tied to controller lifecycle, and not expected to survive a reboot.

### File naming — device name

One JSON file per live controller, named after the kernel device:

```
/run/nvme/registry/
    nvme0.json
    nvme3.json
    nvme7.json
```

**Absence means unowned.** A controller with no registry entry is not managed by any orchestrator. `nvme disconnect-all` treats unowned fabrics controllers as safe to disconnect. There is no explicit "unowned" marker — the missing file is the signal. PCIe and apple-nvme controllers never have registry entries but are also never disconnected by `nvme disconnect-all` — they are excluded by existing logic independent of the registry (see Transport scope in Section 8).

**Why device-name filenames (not hashed ctrl-params):**

The original proposal suggested hashed ctrl-params filenames for stability across reconnects. After analysis of the kernel reconnect path, device-name filenames are simpler and equally correct:

- **Kernel-driven reconnect (within `ctrl-loss-tmo`):** the kernel reuses the same `struct nvme_ctrl` and the same `ctrl->instance`. No `REMOVE` udev event is emitted. The device name (`nvme3`) is preserved throughout. The registry file is untouched.
- **`ctrl-loss-tmo` expiry:** the kernel calls `nvme_delete_ctrl()` → `nvme_uninit_ctrl()` → `cdev_device_del()` → `KOBJ_REMOVE` udev event. The file is deleted by a udev rule at this point (see Section 8). If an orchestrator reconnects, it creates a fresh registry entry under the new device name.

Benefits of device-name filenames:

- **Ownership lookup:** `open("nvme3.json")` — O(1), no scan
- **Udev REMOVE cleanup:** `unlink("nvme3.json")` — one syscall, no hash needed
- **Atomic initial claim:** tmp → rename — targeted write with no scan required
- **Human inspection:** `ls /run/nvme/registry/` is self-explanatory
- **Implementation complexity:** no hash function, no scan loop

---

## 5. Registry File Schema

### Example: `/run/nvme/registry/nvme3.json`

```json
{
  "device": "nvme3",    
  "owner": "stas"
}
```

### Field notes

- **`device`**: the kernel device name (e.g. `nvme3`), identical to the filename stem. The filename is authoritative; this field is included so the JSON is self-contained when read in isolation.
- **`owner`**: a string identifying the orchestrator that manages this controller. Exact vocabulary TBD in discussion with the community; suggested values: `"stas"`, `"nbft"`, `"discoverd"`.

Transport parameters (transport, traddr, trsvcid, etc.) are not stored in the registry. They are already available in sysfs at `/sys/class/nvme/nvmeX/` and can be read from there by any tool that needs them.

---

## 6. Ownership Semantics

**Exclusive ownership**: one authoritative owner per controller.

Ownership applies to **all NVMe-oF controllers** — both Discovery Controllers (DCs) and I/O Controllers (IOCs). `nvme disconnect-all` operates on both. Without registry protection, nvme-discoverd could accidentally disconnect DC connections that nvme-stas depends on for continuous fabric monitoring.

Ownership means **reconciliation authority** while the controller is connected. It identifies which orchestrator is managing a given controller, allowing others to leave it alone — in particular, to skip it during `disconnect-all`.

When a controller is lost (`KOBJ_REMOVE`), the registry entry is deleted and ownership is gone. Any orchestrator may attempt to reconnect; whoever succeeds first writes the new registry entry and becomes the new owner. Prior ownership confers no priority in that race.

### Operations

| Operation | Mechanism | Notes |
|---|---|---|
| **Create** | write-tmp → fsync → rename → fsync(dir) | Always overwrites; any pre-existing entry is stale by definition |
| **Retrieve** | `open()` + JSON parse | Read owner field |
| **Update** | write-tmp → fsync → rename → fsync(dir) | Same write path as Create; works whether or not an entry exists |
| **Delete** | `unlink()` | Called by the owner on intentional disconnect; udev rule calls `rm -f` directly (see Section 7) |

### Atomic write protocol (create / update)

```
write to nvme3.json.tmp
fsync(nvme3.json.tmp)
rename(nvme3.json.tmp, nvme3.json)
fsync(/run/nvme/registry/)
```

### Update semantics — stealing ownership (nvme-stas use case)

`nvme-stas` daemons (`stafd`/`stacd`) are written in Python and respond to events slower than nvme-discoverd. As a result, nvme-discoverd may claim ownership of a controller before nvme-stas has a chance to. nvme-stas needs to be able to steal ownership when it determines that a controller belongs under its management.

`libnvmf_registry_update()` performs an atomic registry file replacement (see above). The previous owner string is overwritten.

### Ownership protection

A "lock" flag to prevent stealing was considered and rejected. All orchestrators run as root (required by `/dev/nvme-fabrics`), so there is no OS-level enforcement mechanism. Any root process could remove the lock regardless. The registry is a cooperative coordination mechanism, not a security boundary.

---

## 7. Udev Rule for Stale Entry Cleanup

### Problem

When a controller is removed by the kernel, any orchestrator that wrote its registry entry may not clean up promptly — or at all, if it exited unexpectedly. Without automatic cleanup, stale entries would accumulate in `/run/nvme/registry/`.

### Solution

A udev rule fires on `KOBJ_REMOVE` for NVMe controller devices and removes the registry file directly:

```
ACTION=="remove", SUBSYSTEM=="nvme", \
    RUN+="/bin/rm -f /run/nvme/registry/%k.json"
```

Where `%k` is the kernel device name (e.g., `nvme3`). The `-f` flag makes the rule safe when no entry exists (unowned controllers, PCIe devices, etc.).

**Why `rm` rather than `nvme registry-delete`:** the registry lives in `libnvme`, so the udev rule ships with the `libnvme` package. `libnvme` can be installed independently of `nvme-cli`, so invoking the `nvme` executable from the udev rule would create an undeclared runtime dependency. `rm` is always available and does exactly what `libnvmf_registry_delete()` does internally — a single `unlink()` call.

### Why this is correct for all cases

| Case | REMOVE fires? | Effect |
|---|---|---|
| Kernel reconnect (within `ctrl-loss-tmo`) | **No** | Registry entry untouched; device name preserved |
| `nvme connect` / `nvme connect-all`, `ctrl-loss-tmo` expires | **Yes** | No entry was written (NULL owner); cleanup is a no-op |
| `nvme connect-all --nbft`, `ctrl-loss-tmo` expires | **Yes** | Entry (`owner=nbft`) deleted; systemd unit or NetworkManager dispatcher may reconnect and re-register |
| nvme-discoverd, `ctrl-loss-tmo` expires | **Yes** | Entry deleted; nvme-discoverd may reconnect and re-register |
| nvme-stas, `ctrl-loss-tmo` expires | **Yes** | Entry deleted; nvme-stas detects removal, reconnects, re-registers under new device name |

The kernel code confirms this: during `ctrl-loss-tmo`-bounded reconnect attempts, the kernel reuses the same `struct nvme_ctrl` (same `ctrl->instance`, same device name). Only `KOBJ_CHANGE` events are emitted (`NVME_EVENT=connected` on success). The `KOBJ_REMOVE` path only triggers when `nvme_delete_ctrl()` → `nvme_uninit_ctrl()` → `cdev_device_del()` is reached.

### Additional udev note

`nvme_class_uevent()` (kernel `core.c`) injects `NVME_TRTYPE`, `NVME_TRADDR`, `NVME_TRSVCID`, `NVME_HOST_TRADDR`, `NVME_HOST_IFACE` into every udev event.

### Stale entry handling (belt and suspenders)

The udev rule handles the normal case promptly, but one additional mechanism guards against the instance-number-recycling edge case.

**`libnvmf_registry_create()` always overwrites**

After a successful connect, the kernel responds to the `write()` on `/dev/nvme-fabrics` with `instance=N` in the `read()` response (parsed in `__nvmf_add_ctrl()`). This is how the device name `nvmeN` is known. Since the kernel just assigned this instance to a new connection, any pre-existing `nvmeN.json` is by definition stale — the old controller is gone and its instance number was recycled. `libnvmf_registry_create()` therefore writes the new entry unconditionally using the atomic tmp→rename protocol, overwriting whatever was there.

Checking whether `/dev/nvmeN` exists before writing is not useful: the device was just created by the kernel, so it will always exist at the time of the claim.

**`nvme registry-list` skips stale entries**

When listing registry entries, any `nvmeX.json` file whose corresponding `/dev/nvmeX` no longer exists is silently skipped. This keeps the output correct without requiring an explicit audit command. Orchestrators iterating the registry for decision-making should apply the same check.

---

## 8. Global Context Integration

The owner name is stored in `struct libnvme_global_ctx`, the existing per-orchestrator context that is created once at startup and passed as the first argument to every libnvme API. This means no existing call sites need to change.

```c
/* Set the owner name for this process. Called once at orchestrator startup. */
void libnvmf_ctx_set_owner(struct libnvme_global_ctx *ctx, const char *owner);
```

When `ctx->owner` is set and a connect operation succeeds, libnvme automatically writes the registry entry — the orchestrator does not call any registration function explicitly. If `ctx->owner` is NULL, no registry entry is written; tools that do not participate in the registry work exactly as before.

The owner name flows through the context to the connect path, which calls the internal `libnvmf_registry_create()` after the controller device has been assigned by the kernel.

### Connect path integration point

Both public connect APIs funnel through the internal `__nvmf_add_ctrl(struct libnvme_global_ctx *ctx, ...)`:

- `libnvmf_add_ctrl()` → `__nvmf_add_ctrl(h->ctx, argstr)`
- `libnvmf_connect_ctrl()` → `__nvmf_add_ctrl(c->s->h->ctx, argstr)`

`ctx` is already the first parameter of `__nvmf_add_ctrl()`. The natural hook point is immediately before the `return ret` at the line where `instance=N` is parsed from the kernel's read-back response (`fabrics.c` line 1065):

```c
if (sscanf(p, "instance=%d", &ret) == 1) {
    if (ctx->owner)
        libnvmf_registry_create(/* "nvme%d" % ret */, ctx->owner);
    return ret;
}
```

The device name is derived as `"nvme%d"` from `ret`. The owner comes from `ctx->owner`. This single hook point covers all fabrics transports and both connect APIs with no duplication.

### Transport scope

The automatic registry hook only fires for fabrics transports — those that go through `/dev/nvme-fabrics`. This was confirmed by reading the libnvme source and the `Documentation/fabrics-options.txt` man page:

| Transport | Path | Registry hook |
|-----------|------|---------------|
| tcp | `/dev/nvme-fabrics` | Yes — automatic |
| rdma | `/dev/nvme-fabrics` | Yes — automatic |
| fc | `/dev/nvme-fabrics` | Yes — automatic |
| loop | `/dev/nvme-fabrics` | Yes — automatic |
| pcie | Kernel hardware enumeration, sysfs only | **No hook** |
| apple-nvme | Kernel hardware enumeration, sysfs only | **No hook** |

PCIe and apple-nvme controllers are never connected by userspace. The kernel discovers them at boot or hot-plug; libnvme reads them from sysfs (`libnvme_scan_ctrl()` → `libnvme_ctrl_alloc()`). There is no write to `/dev/nvme-fabrics`, no `__nvmf_add_ctrl()` call, and no moment where userspace triggers the device appearance. Consequently there is no hook point for automatic registry entry creation.

This is consistent with existing nvme-cli behavior: `nvme disconnect-all` already skips PCIe controllers explicitly (`fabrics.c`: `!strcmp(transport, "pcie") → continue`), and the `nvme connect` man page (`fabrics-options.txt`) enumerates exactly four accepted transport values — rdma, tcp, fc, loop — with no mention of pcie or apple-nvme.

The pcie/apple-nvme transport strings that appear in `libnvme/src/nvme/tree.c` exist solely to handle sysfs address format differences (PCIe exposes a PCI bus address like `0000:01:00.0` rather than a comma-separated fabrics address string) and to exempt these transports from the "traddr required" validation. They are not connect-path code.

PCIe and apple-nvme controllers will therefore never appear in the registry. No special exclusion logic is needed — they are simply outside the scope of the fabrics connect path that drives registry entries.

---

## 9. API

All functions will live in `libnvme/src/nvme/registry.c` with declarations in `registry.h`.

`libnvmf_registry_create()` is **internal** — called automatically from the connect path when `ctx->owner` is set:

```c
/*
 * Write a registry entry for a newly connected controller.
 * Internal — called from the connect path once the kernel returns instance=N
 * via the /dev/nvme-fabrics read() response. Always overwrites any existing
 * entry: a pre-existing nvmeN.json at this point is stale by definition.
 * Returns 0 on success, negative errno otherwise.
 */
int libnvmf_registry_create(const char *device, const char *owner);
```

The public API follows CRUD semantics:

```c
/* Set the registry owner name for this process (called once at startup). */
void libnvmf_ctx_set_owner(struct libnvme_global_ctx *ctx, const char *owner);

/*
 * Retrieve the value of a specific field from a controller's registry entry.
 * Returns 0 and fills *value on success, -ENOENT if not registered or key
 * not found. Caller must free *value.
 * Example: libnvmf_registry_retrieve("nvme1", "owner", &value);
 */
int libnvmf_registry_retrieve(const char *device, const char *key,
			       char **value);

/*
 * Update a field in a controller's registry entry (creates the entry if it
 * does not exist). Used to steal ownership from the current owner.
 * Returns 0 on success, negative errno otherwise.
 * Example: libnvmf_registry_update("nvme1", "owner", "stas");
 */
int libnvmf_registry_update(const char *device, const char *key,
			     const char *value);

/*
 * Delete the registry entry for a controller.
 * Called by the owner on intentional disconnect, or by orchestrators that
 * monitor udev KOBJ_REMOVE events. Note: the udev cleanup rule uses rm -f
 * directly and does not call this function.
 * Returns 0 on success, negative errno otherwise.
 */
int libnvmf_registry_delete(const char *device);
```

---

## 10. CLI Integration

All registry operations are exposed through the existing `nvme` executable as built-in commands, registered via `ENTRY()` in `nvme-builtin.h` with man pages in `Documentation/nvme-registry-*.txt`.

### Registry subcommands

| Command | Description |
|---|---|
| `nvme registry-list` | List all registry entries as a human-readable table |
| `nvme registry-retrieve <device> [<key>]` | Show a specific field (e.g. `owner`) or all fields if no key given; calls `libnvmf_registry_retrieve()` |
| `nvme registry-update <device> <key>=<value>` | Update a field in the registry entry (e.g. `owner=stas`); calls `libnvmf_registry_update()` |
| `nvme registry-delete <device>` | Remove a registry entry — for manual use; the udev rule uses `rm -f` directly (see Section 7) |

### Ownership-aware extensions to existing commands

| Command | Description |
|---|---|
| `nvme connect` | No registry entry is written; the controller is unowned and can be disconnected by `nvme disconnect-all` |
| `nvme connect-all` | No registry entry is written; all connected controllers are unowned |
| `nvme connect-all --nbft` | Registers all connected controllers as owned by `"nbft"` — sets `ctx->owner = "nbft"` internally before connecting |
| `nvme disconnect-all` | Disconnect only unowned controllers (not in registry) — new safe default |
| `nvme disconnect-all --owner <name>` | Disconnect only controllers owned by `<name>` |
| `nvme disconnect-all --force` | Disconnect all controllers regardless of ownership — requires confirmation prompt |
| `nvme list --show-owner` | Add an owner column to the controller listing |

**Behavior change note:** the current unconditional behavior of `nvme disconnect-all` becomes `--force`. The new default is safe — it only touches controllers that have no registered owner.

Exact command-line interface to be decided in discussion with nvme-cli maintainers.

---

## 11. Kernel Reconnect Behavior — Reference

Confirmed by reading `drivers/nvme/host/tcp.c` and `core.c`:

- **`nvme_reset_ctrl_work`** (the reconnect work): reuses the same `struct nvme_ctrl`. No `nvme_delete_ctrl()`, no `nvme_uninit_ctrl()`, no `cdev_device_del()`. The instance number and device name are preserved. Only `KOBJ_CHANGE` udev events are emitted.
- **`nvme_tcp_reconnect_or_remove`**: if `nvmf_should_reconnect()` returns false (max reconnects exceeded / `ctrl-loss-tmo` expired), calls `nvme_delete_ctrl()` → triggers the full teardown and `KOBJ_REMOVE`.
- **`nvme_free_ctrl`**: `ida_free(&nvme_instance_ida, ctrl->instance)` — the instance number is only returned to the pool here, after the controller struct is fully freed.

---

## 12. Approaches Considered and Rejected

### Hashed ctrl-params filenames

Originally proposed for stability across reconnects. Rejected in favor of device-name filenames because:

- The stability argument is moot: the file is deleted on `KOBJ_REMOVE`, and orchestrators re-register under the new device name after reconnect.
- Kernel reconnects (within `ctrl-loss-tmo`) don't change the device name, so no file rename is needed.
- Device-name filenames are simpler: O(1) lookup, trivial udev cleanup, no hash function needed.

### Kernel sysfs ownership attribute (`/sys/class/nvme/nvmeX/owner`)

Rejected: introduces userspace policy into the kernel. Upstream maintainers would object. Sysfs ABI stability concerns.

### Unix group ownership on `/dev/nvmeX`

Rejected: overloads permission semantics, multiple device nodes per controller, poor extensibility.

### udev property database

Rejected: ownership coordination is higher-level than udev's intended role; properties would need replay after device recreation.

### Ownership lock flag

Rejected: all orchestrators run as root (required by `/dev/nvme-fabrics`), so there is no OS-level mechanism to enforce a lock. Any root process could clear it. The registry is a cooperative coordination mechanism, not a security boundary.

### ctrl-params in the registry file

Rejected: transport parameters (transport, traddr, trsvcid, hostnqn, etc.) are already fully available in sysfs at `/sys/class/nvme/nvmeX/`. Storing them in the registry file would be purely cosmetic — no registry operation uses them for matching or decision-making. Removing them also simplifies ownership transfer: `libnvmf_registry_update()` only needs to write the new owner, with no ctrl-params to preserve.

### version field in the registry file

Rejected: the JSON is self-describing and key-based. Readers query specific fields and ignore unknown ones. No version negotiation is needed because we do not anticipate breaking schema changes — the schema is intentionally minimal (two fields). The filesystem mtime already provides file-creation timing for debugging purposes. A version field would add complexity without providing any real benefit.

---

## 13. Open Questions

1. **`libnvme_global_ctx` extension**: exact field name and type for `owner`; accessor/setter naming; where in the struct it lives.
2. **Owner string length limit**: impose a practical maximum (e.g. 256 bytes) as a sanity bound; JSON itself imposes no limit.
3. ~~**`nvme list --show-owner` output format**~~ — **resolved**:
   - Unowned fabrics controllers (no registry entry): `-`
   - PCIe / apple-nvme controllers: `kernel` (synthesized from transport type by `nvme list`; not read from the registry — these controllers are managed by the kernel via hardware enumeration, not by any userspace orchestrator)
4. ~~**nvme-stas integration**: which nvme-stas code paths call `libnvmf_registry_update()`? What triggers `libnvmf_registry_delete()`?~~ — **resolved**:

   nvme-stas always begins by checking whether a controller connection already exists:

   - **No existing connection** → nvme-stas calls `libnvmf_connect_ctrl()` (or equivalent); the connect hook in `__nvmf_add_ctrl()` fires `libnvmf_registry_create()` automatically (because `ctx->owner` is set). No explicit registry call needed from nvme-stas.

   - **Existing connection, unowned** (no registry entry) → nvme-stas calls `libnvmf_registry_update(device, "owner", "stas")` to claim it.

   - **Existing connection, owned by another orchestrator** → nvme-stas calls `libnvmf_registry_update()` to steal ownership, except for controllers owned by `"nbft"` (boot volumes), which nvme-stas leaves alone.

   For `libnvmf_registry_delete()`, nvme-stas has three natural trigger points and can use any or all of them:
   1. When nvme-stas initiates a disconnect.
   2. On a udev `KOBJ_REMOVE` event for the device — nvme-stas already monitors kernel events.
   3. Not at all — the udev rule (`rm -f /run/nvme/registry/%k.json`) handles cleanup automatically on device removal.

   Option 3 is the simplest and sufficient for the cooperative model. Options 1 and 2 are belt-and-suspenders additions that nvme-stas may choose to implement.

---

## 14. The Orchestrator Ecosystem — Background and Motivation

This section provides context on the known orchestrators, their capabilities, and why the registry is essential to their coexistence.

### Known orchestrators

**Human (nvme-cli, one-shot)**

A human operator invoking `nvme connect`, `nvme connect-all`, or `nvme disconnect-all` in a terminal. Humans are not reactive orchestrators — they do not monitor kernel events in real time and cannot always retry failed operations. No registry entry is written for manual connections (`ctx->owner` is NULL in the CLI path). This means manually connected controllers are unowned: `nvme disconnect-all` can disconnect them freely, which is the correct behavior since no daemon will attempt to reconnect them anyway.

**udev rules → nvme-discoverd**

Distros universally install nvme-cli (the `nvme` executable, libnvme, and a set of udev rules) by default. The udev rules cause `udevd` to react to kernel events — typically by invoking `nvme connect-all` when a discovery controller is found — but the reaction is one-shot. If the connect fails there is no retry; if the controller is later lost there is no reconnect. This is sufficient for simple, stable environments but inadequate for production storage networking.

`nvme-discoverd` (under planning) is a daemon-based replacement for these udev rules. It addresses the main limitations of udev: it can retry failed connections and its behavior is easier to configure. Because nvme-cli is a default distro package, nvme-discoverd will also be installed and started by default once it ships.

**nvme-stas (stafd + stacd)**

nvme-stas is an optional package. Installing it on a host signals an intent to use production-grade NVMe-oF connectivity management. It consists of two daemons:

- `stafd` (STorage Appliance Finder Daemon): discovers NVMe-oF Discovery Controllers using DNS-SD/mDNS as defined in TP8009, connects to them, and reads their discovery log pages.
- `stacd` (STorage Appliance Connector Daemon): uses the discovery information gathered by `stafd` to establish and maintain I/O controller connections.

Both daemons monitor kernel events continuously, maintain persistent connections with complex retry and audit loops, and support the full TP8009 + TP8010 feature set. Critically, nvme-stas supports TP8010 **fabric zoning** via a Centralized Discovery Controller (CDC). A CDC aggregates discovery information from multiple Direct Discovery Controllers (DDCs) and enforces which hosts are allowed to see which NVMe subsystems — a network-level access control mechanism entirely absent from udev rules and nvme-discoverd. This CDC integration gives nvme-stas a network-level view of controller connectivity that no other host-side tool currently provides.

### Coexistence and the role of the registry

Because nvme-discoverd ships by default and nvme-stas is optional, both may run simultaneously on the same host. This creates race conditions: both daemons react to the same kernel events and may attempt to connect to the same controllers. Without the registry, there is no way for either daemon to know whether the other has already claimed a controller, and `nvme disconnect-all` has no way to avoid disconnecting controllers managed by a daemon it knows nothing about.

The registry resolves this: each daemon registers ownership as it connects controllers, and respects entries written by others.

When nvme-stas determines that a controller belongs to its fabric zone — because the CDC's discovery log page includes that controller and the host is permitted to connect to it — nvme-stas will steal ownership from whichever orchestrator connected first. This is the intended use of `libnvmf_registry_update()`: a higher-capability orchestrator asserting management authority over a connection that a lower-capability one established opportunistically. nvme-stas has the most complete picture of the fabric topology and connectivity policy; its ownership decisions reflect that knowledge.

### Why fabric zoning matters for ownership

TP8010 defines fabric zoning as a CDC-enforced policy that controls which hosts may connect to which NVMe subsystems. A host running nvme-stas participates in this policy: `stafd` registers with the CDC, retrieves a filtered discovery log page containing only the controllers the host is permitted to connect to, and `stacd` connects to exactly those controllers — no more, no less.

An orchestrator without CDC awareness (udev rules, nvme-discoverd in its current scope) has no knowledge of zoning policy. It connects to whatever controllers it discovers locally. This is correct behavior in the absence of zoning, but in a zoned fabric it may connect to controllers the host is not permitted to access, or miss controllers that are only visible through the CDC.

This asymmetry is why nvme-stas ownership takes precedence in a zoned fabric: only nvme-stas has the full picture.

However, the ownership registry alone cannot fully prevent conflicts in a zoned fabric. Consider the following sequence: nvme-stas determines that a controller is not permitted by the fabric zone policy (or is excluded by local nvme-stas configuration) and intentionally disconnects it. nvme-discoverd, unaware of the zoning policy or the nvme-stas configuration, sees the controller disappear, detects a discovery log page entry for it, and reconnects it. This creates a connect/disconnect bounce loop between the two daemons.

The registry prevents nvme-discoverd from *accidentally* disconnecting controllers that nvme-stas is managing, but it cannot prevent nvme-discoverd from reconnecting a controller that nvme-stas deliberately chose not to connect.

For users who require true fabric zoning support (TP8010) and want to avoid these conflicts, the recommended approach is to disable nvme-discoverd entirely:

    systemctl disable --now nvme-discoverd

nvme-stas covers everything nvme-discoverd does and more. Running both simultaneously only makes sense in environments without fabric zoning, where nvme-discoverd's simpler discovery model is sufficient and no conflict with nvme-stas policy decisions can arise.

---

## 15. Related Resources

| Resource | Purpose |
|---|---|
| NVMe-oF TP8009 (ratified) | Automated Discovery of NVMe-oF Discovery Controllers via DNS-SD/mDNS |
| NVMe-oF TP8010 (ratified) | Centralized Discovery Controller (CDC) and fabric zoning |
| `Documentation/fabrics-options.txt` (nvme-cli) | Authoritative list of supported connect transports (rdma, tcp, fc, loop) |
| `libnvme/src/nvme/fabrics.c` (nvme-cli) | Connect path — `__nvmf_add_ctrl()` where `instance=N` is parsed and `libnvmf_registry_create()` will be called |
| `drivers/nvme/host/tcp.c` (kernel) | Kernel reconnect path (`nvme_reset_ctrl_work`) |
| `drivers/nvme/host/core.c` (kernel) | Controller lifecycle, udev events, `nvme_free_ctrl` |
