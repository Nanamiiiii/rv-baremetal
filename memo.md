## Virt
### `vector.s`
- 関数ポインタ配列 `IV`
    - `.align 2`で32bit境界に調整しているが、64bitの場合これを64に合わせる必要があるか？
### `bootup.c`
- Function `isr_synctrap`
    - `asm volatile("csrr %0,mcause" : "=r"(synctrap_cause));`
        - Output Operandsは何してる？
### `target.ld`
- HiFive1のやつは各所にPaddingがあるがその意味は？
- FLASHがない代わりにRAMに全部突っ込むことになるけども、Relocationとかはどうなるのか
    - riscv-probeでは全セクション`> RAM AT > RAM`で再ロケーションされてる？