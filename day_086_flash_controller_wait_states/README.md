# Day 086: Flash Controller, Sector Erasure & Wait States

### Objective

Implement low-level bare-metal control over the embedded Flash memory interface (`0x40023C00`) on the `STM32F401RE` microcontroller. This milestone focuses on configuring CPU memory latency (**Wait States**) alongside the **ART Accelerator** (Prefetch, Instruction Cache, and Data Cache), executing the hardware unlock sequence, and performing **In-Application Programming (IAP)** via sector erasure and 32-bit word programming on an isolated Flash sector.

---

### Key Technical Implementations

- **Flash Access Control & ART Accelerator (`FLASH->ACR`)**:
  - Configured **2 Wait States** (`LATENCY = 2WS`) to ensure reliable instruction fetches when operating at higher HCLK frequencies (up to 84 MHz).
  - Enabled **Prefetch Buffer (`PRFTEN`)**, **Instruction Cache (`ICEN`)**, and **Data Cache (`DCEN`)** to mitigate the speed bottleneck between the ARM Cortex-M4 core and non-volatile Flash storage.
- **Hardware Key Unlock & Lock Mechanism (`FLASH->KEYR` / `FLASH->CR`)**:
  - Implemented the mandatory two-step hardware unlock sequence (`KEY1 = 0x45670123`, `KEY2 = 0xCDEF89AB`) to gain write access to `FLASH->CR`, followed by immediate re-locking (`FLASH_CR_LOCK`) post-operation to prevent accidental memory corruption.
- **Sector Erasure (`Sector 7` at `0x08060000`)**:
  - Targeted **Sector 7** (128KB, located at the upper boundary of the 512KB Flash space) to avoid overwriting active program code in Sector 0.
  - Cleared status error flags (`OPERR`, `WRPERR`, `PGAERR`, `PGPERR`, `PGSERR`), configured `x32` voltage parallelism (`PSIZE = 10`), and polled the `BSY` flag until all bits reset to `1` (`0xFFFFFFFF`).
- **32-Bit Word Programming (`FLASH_CR_PG`)**:
  - Performed direct memory-mapped 32-bit word write (`0xCAFEBABE`) into `0x08060000` while monitoring `FLASH->SR` completion flags.

---

### Verification and Results (From Debugger Expression View)

By stepping through execution (`Step Over`) and inspecting variables in the IDE **Expressions** panel during runtime debugging:

| Variable | Captured Decimal | Hexadecimal Equivalent | Hardware Verification Meaning |
| :--- | :--- | :--- | :--- |
| **`acr_status`** | `1794` | **`0x00000702`** | `PRFTEN` (bit 8), `ICEN` (bit 9), `DCEN` (bit 10), and `2WS` (`0x02`) active |
| **`val_after_erase`** | `4294967295` | **`0xFFFFFFFF`** | Sector 7 (`0x08060000`) completely erased to default logic `1` |
| **`val_after_write`** | `3405691582` | **`0xCAFEBABE`** | 32-bit test pattern permanently programmed into Sector 7 |

### Flash IAP Execution Flow:

- `Configure ACR` -> Enable Prefetch, I-Cache, D-Cache, and 2 Wait States (`0x00000702`)
- `Unlock Flash` -> Write `KEY1` and `KEY2` to `FLASH->KEYR`
- `Sector Erase` -> Select Sector 7, trigger `STRT`, wait for `BSY == 0` -> Verify `0xFFFFFFFF`
- `Word Program` -> Enable `PG`, write `0xCAFEBABE` to `0x08060000`, wait for `BSY == 0` -> Verify `0xCAFEBABE`
- `Lock Flash` -> Set `FLASH_CR_LOCK` bit in `FLASH->CR`

---

### Conclusion

Day 086 demonstrates complete bare-metal mastery over STM32F401 non-volatile memory management. Understanding Flash latency, cache acceleration, and safe sector programming provides the core foundation required for building custom bootloaders, Over-The-Air (OTA) firmware updaters, and non-volatile configuration storage without external EEPROMs.