; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -mtriple=riscv32 -verify-machineinstrs < %s | FileCheck -check-prefix=RV32I %s
; RUN: llc -mtriple=riscv64 -verify-machineinstrs < %s | FileCheck -check-prefix=RV64I %s

; regression due to creation of temporary i32 avgfloors node
define signext i64 @PR95284(i32 signext %0) {
; RV32I-LABEL: PR95284:
; RV32I:       # %bb.0: # %entry
; RV32I-NEXT:    addi a1, a0, -1
; RV32I-NEXT:    seqz a0, a0
; RV32I-NEXT:    slli a2, a0, 31
; RV32I-NEXT:    srli a1, a1, 1
; RV32I-NEXT:    or a1, a1, a2
; RV32I-NEXT:    addi a1, a1, 1
; RV32I-NEXT:    seqz a2, a1
; RV32I-NEXT:    sub a2, a2, a0
; RV32I-NEXT:    andi a0, a1, -2
; RV32I-NEXT:    slli a1, a2, 1
; RV32I-NEXT:    srli a1, a1, 1
; RV32I-NEXT:    ret
;
; RV64I-LABEL: PR95284:
; RV64I:       # %bb.0: # %entry
; RV64I-NEXT:    addi a0, a0, -1
; RV64I-NEXT:    srli a0, a0, 1
; RV64I-NEXT:    addi a0, a0, 1
; RV64I-NEXT:    andi a0, a0, -2
; RV64I-NEXT:    ret
entry:
  %1 = zext nneg i32 %0 to i64
  %2 = add nsw i64 %1, -1
  %3 = lshr i64 %2, 1
  %4 = add nuw nsw i64 %3, 1
  %5 = and i64 %4, 9223372036854775806
  ret i64 %5
}
