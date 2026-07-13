# VIJAY Executable Artifact Format (VEAF 3)

This document specifies the binary structure of compiled THALAPATHY artifacts (`.vijay`).

---

## 1. File Header
All `.vijay` artifacts start with an 11-byte header:

| Offset | Size | Type | Value / Description |
|---|---|---|---|
| 0 | 4 | Char | `VJAY` (Magic Signature: 0x56 4A 41 59) |
| 4 | 2 | uint16_t | Format Version: `3` |
| 6 | 1 | uint8_t | Language Version: Major (`0`) |
| 7 | 1 | uint8_t | Language Version: Minor (`3`) |
| 8 | 1 | uint8_t | Language Version: Patch (`0`) |
| 9 | 2 | uint16_t | Number of Sections |

---

## 2. Section Structure
After the header, sections are written sequentially. Each section follows this format:

| Field | Size | Type | Description |
|---|---|---|---|
| Section ID | 1 | uint8_t | Section type identifier |
| Section Length | 4 | uint32_t | Length of section payload in bytes |
| Payload | variable | bytes | Raw section payload |

---

## 3. Section Types
TDK defines the following Section IDs:

- **`0x01`**: Constant Pool
- **`0x02`**: Instructions
- **`0x03`**: Class Metadata
- **`0x04`**: Interface Metadata
- **`0x05`**: Property/Field Metadata
- **`0x06`**: Method/Function Metadata
- **`0x07`**: Declared Types list

Unknown optional sections are automatically skipped by reading the payload length and seeking forward.
