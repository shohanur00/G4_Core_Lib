# Bootloader Firmware Update Protocol

Host (PC) ⇄ Target (MCU) handshake and firmware transfer flow.

## Sequence Diagram

```mermaid
sequenceDiagram
    participant PC
    participant MCU

    Note over PC,MCU: 1. Sync
    PC->>MCU: SYNC_OBSERVED
    MCU-->>PC: ACK

    Note over PC,MCU: 2. Device identification
    PC->>MCU: DEVICE_ID_REQ
    MCU-->>PC: DEVICE_ID_RES + ID
    PC->>MCU: ACK

    Note over PC,MCU: 3. Firmware length
    PC->>MCU: FW_LENGTH_REQ + length
    MCU-->>PC: FW_LENGTH_RES
    PC->>MCU: ACK

    Note over PC,MCU: 4. Start update
    PC->>MCU: FW_UPDATE_REQ
    MCU-->>PC: READY_FOR_DATA

    Note over PC,MCU: 5. Data transfer (repeat for packets 1..N)
    loop DATA #1 .. #N-1
        PC->>MCU: DATA #k
        MCU-->>PC: ACK
    end
    PC->>MCU: DATA #N
    MCU-->>PC: UPDATE_SUCCESSFUL
```

## ASCII Version

```
PC                                      MCU
│                                        │
│──── SYNC_OBSERVED ───────────────────>│
│<──── ACK ─────────────────────────────│
│                                        │
│──── DEVICE_ID_REQ ───────────────────>│
│<──── DEVICE_ID_RES + ID ──────────────│
│──── ACK ─────────────────────────────>│
│                                        │
│──── FW_LENGTH_REQ + length ──────────>│
│<──── FW_LENGTH_RES ───────────────────│
│──── ACK ─────────────────────────────>│
│                                        │
│──── FW_UPDATE_REQ ───────────────────>│
│<──── READY_FOR_DATA ──────────────────│
│                                        │
│──── DATA #1 ─────────────────────────>│
│<──── ACK ─────────────────────────────│
│──── DATA #2 ─────────────────────────>│
│<──── ACK ─────────────────────────────│
│                 ...                    │
│──── DATA #N ─────────────────────────>│
│<──── UPDATE_SUCCESSFUL ───────────────│
```

## Message Summary

| # | Phase | Message | Direction | Payload | Expected reply |
|---|-------|---------|-----------|---------|----------------|
| 1 | Sync | `SYNC_OBSERVED` | PC → MCU | — | `ACK` |
| 2 | Identify | `DEVICE_ID_REQ` | PC → MCU | — | `DEVICE_ID_RES` |
| 3 | Identify | `DEVICE_ID_RES` | MCU → PC | Device ID | `ACK` |
| 4 | Length | `FW_LENGTH_REQ` | PC → MCU | Firmware length | `FW_LENGTH_RES` |
| 5 | Length | `FW_LENGTH_RES` | MCU → PC | — | `ACK` |
| 6 | Start | `FW_UPDATE_REQ` | PC → MCU | — | `READY_FOR_DATA` |
| 7 | Transfer | `DATA #1 … #N-1` | PC → MCU | Firmware chunk | `ACK` per packet |
| 8 | Finish | `DATA #N` | PC → MCU | Last chunk | `UPDATE_SUCCESSFUL` |

## Notes

- The final data packet is answered with `UPDATE_SUCCESSFUL` instead of a plain `ACK`.
- Opcodes, packet framing, chunk size, CRC/checksum and timeout/retry behaviour are not defined in this diagram — add them here once fixed in the firmware.
