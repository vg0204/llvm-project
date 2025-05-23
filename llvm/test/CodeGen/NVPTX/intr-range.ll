; NOTE: Assertions have been autogenerated by utils/update_test_checks.py UTC_ARGS: --check-attributes --version 5
; RUN: opt < %s -S -mtriple=nvptx-nvidia-cuda -mcpu=sm_20 -passes=nvvm-intr-range | FileCheck %s
; RUN: llvm-as < %s | llvm-dis | FileCheck %s --check-prefix=DEFAULT

define ptx_kernel i32 @test_maxntid() "nvvm.maxntid"="32,1,3" {
; CHECK-LABEL: define ptx_kernel i32 @test_maxntid(
; CHECK-SAME: ) #[[ATTR0:[0-9]+]] {
; CHECK-NEXT:    [[TMP1:%.*]] = call range(i32 0, 96) i32 @llvm.nvvm.read.ptx.sreg.tid.x()
; CHECK-NEXT:    [[TMP3:%.*]] = call range(i32 0, 96) i32 @llvm.nvvm.read.ptx.sreg.tid.y()
; CHECK-NEXT:    [[TMP2:%.*]] = call range(i32 0, 64) i32 @llvm.nvvm.read.ptx.sreg.tid.z()
; CHECK-NEXT:    [[TMP11:%.*]] = call range(i32 1, 97) i32 @llvm.nvvm.read.ptx.sreg.ntid.x()
; CHECK-NEXT:    [[TMP4:%.*]] = call range(i32 1, 97) i32 @llvm.nvvm.read.ptx.sreg.ntid.y()
; CHECK-NEXT:    [[TMP6:%.*]] = call range(i32 1, 65) i32 @llvm.nvvm.read.ptx.sreg.ntid.z()
; CHECK-NEXT:    [[TMP7:%.*]] = add i32 [[TMP1]], [[TMP3]]
; CHECK-NEXT:    [[TMP8:%.*]] = add i32 [[TMP7]], [[TMP2]]
; CHECK-NEXT:    [[TMP9:%.*]] = add i32 [[TMP8]], [[TMP11]]
; CHECK-NEXT:    [[TMP10:%.*]] = add i32 [[TMP9]], [[TMP4]]
; CHECK-NEXT:    [[TMP5:%.*]] = add i32 [[TMP10]], [[TMP6]]
; CHECK-NEXT:    ret i32 [[TMP5]]
;
  %1 = call i32 @llvm.nvvm.read.ptx.sreg.tid.x()
  %2 = call i32 @llvm.nvvm.read.ptx.sreg.tid.y()
  %3 = call i32 @llvm.nvvm.read.ptx.sreg.tid.z()
  %4 = call i32 @llvm.nvvm.read.ptx.sreg.ntid.x()
  %5 = call i32 @llvm.nvvm.read.ptx.sreg.ntid.y()
  %6 = call i32 @llvm.nvvm.read.ptx.sreg.ntid.z()
  %7 = add i32 %1, %2
  %8 = add i32 %7, %3
  %9 = add i32 %8, %4
  %10 = add i32 %9, %5
  %11 = add i32 %10, %6
  ret i32 %11
}

define ptx_kernel i32 @test_reqntid() "nvvm.reqntid"="20" {
; CHECK-LABEL: define ptx_kernel i32 @test_reqntid(
; CHECK-SAME: ) #[[ATTR1:[0-9]+]] {
; CHECK-NEXT:    [[TMP1:%.*]] = call range(i32 0, 20) i32 @llvm.nvvm.read.ptx.sreg.tid.x()
; CHECK-NEXT:    [[TMP5:%.*]] = call range(i32 0, 20) i32 @llvm.nvvm.read.ptx.sreg.tid.y()
; CHECK-NEXT:    [[TMP2:%.*]] = call range(i32 0, 20) i32 @llvm.nvvm.read.ptx.sreg.tid.z()
; CHECK-NEXT:    [[TMP4:%.*]] = call range(i32 1, 21) i32 @llvm.nvvm.read.ptx.sreg.ntid.x()
; CHECK-NEXT:    [[TMP3:%.*]] = call range(i32 1, 21) i32 @llvm.nvvm.read.ptx.sreg.ntid.y()
; CHECK-NEXT:    [[TMP6:%.*]] = call range(i32 1, 21) i32 @llvm.nvvm.read.ptx.sreg.ntid.z()
; CHECK-NEXT:    [[TMP7:%.*]] = add i32 [[TMP1]], [[TMP5]]
; CHECK-NEXT:    [[TMP8:%.*]] = add i32 [[TMP7]], [[TMP2]]
; CHECK-NEXT:    [[TMP9:%.*]] = add i32 [[TMP8]], [[TMP4]]
; CHECK-NEXT:    [[TMP10:%.*]] = add i32 [[TMP9]], [[TMP3]]
; CHECK-NEXT:    [[TMP11:%.*]] = add i32 [[TMP10]], [[TMP6]]
; CHECK-NEXT:    ret i32 [[TMP3]]
;
  %1 = call i32 @llvm.nvvm.read.ptx.sreg.tid.x()
  %2 = call i32 @llvm.nvvm.read.ptx.sreg.tid.y()
  %3 = call i32 @llvm.nvvm.read.ptx.sreg.tid.z()
  %4 = call i32 @llvm.nvvm.read.ptx.sreg.ntid.x()
  %5 = call i32 @llvm.nvvm.read.ptx.sreg.ntid.y()
  %6 = call i32 @llvm.nvvm.read.ptx.sreg.ntid.z()
  %7 = add i32 %1, %2
  %8 = add i32 %7, %3
  %9 = add i32 %8, %4
  %10 = add i32 %9, %5
  %11 = add i32 %10, %6
  ret i32 %5
}

;; A case like this could occur if a function with the sreg intrinsic was
;; inlined into a kernel where the tid metadata is present, ensure the range is
;; updated.
define ptx_kernel i32 @test_inlined() "nvvm.maxntid"="4" {
; CHECK-LABEL: define ptx_kernel i32 @test_inlined(
; CHECK-SAME: ) #[[ATTR2:[0-9]+]] {
; CHECK-NEXT:    [[TMP1:%.*]] = call range(i32 0, 4) i32 @llvm.nvvm.read.ptx.sreg.tid.x()
; CHECK-NEXT:    ret i32 [[TMP1]]
;
  %1 = call range(i32 0, 1024) i32 @llvm.nvvm.read.ptx.sreg.tid.x()
  ret i32 %1
}

define ptx_kernel i32 @test_cluster_ctaid() "nvvm.maxclusterrank"="8" {
; CHECK-LABEL: define ptx_kernel i32 @test_cluster_ctaid(
; CHECK-SAME: ) #[[ATTR3:[0-9]+]] {
; CHECK-NEXT:    [[TMP1:%.*]] = call range(i32 0, 8) i32 @llvm.nvvm.read.ptx.sreg.cluster.ctaid.x()
; CHECK-NEXT:    [[TMP2:%.*]] = call range(i32 0, 8) i32 @llvm.nvvm.read.ptx.sreg.cluster.ctaid.y()
; CHECK-NEXT:    [[TMP3:%.*]] = call range(i32 0, 8) i32 @llvm.nvvm.read.ptx.sreg.cluster.ctaid.z()
; CHECK-NEXT:    [[TMP5:%.*]] = call range(i32 1, 9) i32 @llvm.nvvm.read.ptx.sreg.cluster.nctaid.x()
; CHECK-NEXT:    [[TMP6:%.*]] = call range(i32 1, 9) i32 @llvm.nvvm.read.ptx.sreg.cluster.nctaid.y()
; CHECK-NEXT:    [[TMP7:%.*]] = call range(i32 1, 9) i32 @llvm.nvvm.read.ptx.sreg.cluster.nctaid.z()
; CHECK-NEXT:    [[TMP9:%.*]] = add i32 [[TMP1]], [[TMP2]]
; CHECK-NEXT:    [[TMP10:%.*]] = add i32 [[TMP9]], [[TMP3]]
; CHECK-NEXT:    [[TMP12:%.*]] = add i32 [[TMP10]], [[TMP5]]
; CHECK-NEXT:    [[TMP13:%.*]] = add i32 [[TMP12]], [[TMP6]]
; CHECK-NEXT:    [[TMP15:%.*]] = add i32 [[TMP13]], [[TMP7]]
; CHECK-NEXT:    ret i32 [[TMP15]]
;
  %1 = call i32 @llvm.nvvm.read.ptx.sreg.cluster.ctaid.x()
  %2 = call i32 @llvm.nvvm.read.ptx.sreg.cluster.ctaid.y()
  %3 = call i32 @llvm.nvvm.read.ptx.sreg.cluster.ctaid.z()
  %4 = call i32 @llvm.nvvm.read.ptx.sreg.cluster.nctaid.x()
  %5 = call i32 @llvm.nvvm.read.ptx.sreg.cluster.nctaid.y()
  %6 = call i32 @llvm.nvvm.read.ptx.sreg.cluster.nctaid.z()
  %7 = add i32 %1, %2
  %8 = add i32 %7, %3
  %9 = add i32 %8, %4
  %10 = add i32 %9, %5
  %11 = add i32 %10, %6
  ret i32 %11
}

define ptx_kernel i32 @test_cluster_dim() "nvvm.cluster_dim"="4,4,1" {
; CHECK-LABEL: define ptx_kernel i32 @test_cluster_dim(
; CHECK-SAME: ) #[[ATTR4:[0-9]+]] {
; CHECK-NEXT:    [[TMP1:%.*]] = call range(i32 0, 16) i32 @llvm.nvvm.read.ptx.sreg.cluster.ctaid.x()
; CHECK-NEXT:    [[TMP2:%.*]] = call range(i32 0, 16) i32 @llvm.nvvm.read.ptx.sreg.cluster.ctaid.y()
; CHECK-NEXT:    [[TMP3:%.*]] = call range(i32 0, 16) i32 @llvm.nvvm.read.ptx.sreg.cluster.ctaid.z()
; CHECK-NEXT:    [[TMP5:%.*]] = call range(i32 1, 17) i32 @llvm.nvvm.read.ptx.sreg.cluster.nctaid.x()
; CHECK-NEXT:    [[TMP6:%.*]] = call range(i32 1, 17) i32 @llvm.nvvm.read.ptx.sreg.cluster.nctaid.y()
; CHECK-NEXT:    [[TMP7:%.*]] = call range(i32 1, 17) i32 @llvm.nvvm.read.ptx.sreg.cluster.nctaid.z()
; CHECK-NEXT:    [[TMP9:%.*]] = add i32 [[TMP1]], [[TMP2]]
; CHECK-NEXT:    [[TMP10:%.*]] = add i32 [[TMP9]], [[TMP3]]
; CHECK-NEXT:    [[TMP12:%.*]] = add i32 [[TMP10]], [[TMP5]]
; CHECK-NEXT:    [[TMP13:%.*]] = add i32 [[TMP12]], [[TMP6]]
; CHECK-NEXT:    [[TMP15:%.*]] = add i32 [[TMP13]], [[TMP7]]
; CHECK-NEXT:    ret i32 [[TMP15]]
;
  %1 = call i32 @llvm.nvvm.read.ptx.sreg.cluster.ctaid.x()
  %2 = call i32 @llvm.nvvm.read.ptx.sreg.cluster.ctaid.y()
  %3 = call i32 @llvm.nvvm.read.ptx.sreg.cluster.ctaid.z()
  %4 = call i32 @llvm.nvvm.read.ptx.sreg.cluster.nctaid.x()
  %5 = call i32 @llvm.nvvm.read.ptx.sreg.cluster.nctaid.y()
  %6 = call i32 @llvm.nvvm.read.ptx.sreg.cluster.nctaid.z()
  %7 = add i32 %1, %2
  %8 = add i32 %7, %3
  %9 = add i32 %8, %4
  %10 = add i32 %9, %5
  %11 = add i32 %10, %6
  ret i32 %11
}


; DEFAULT-DAG: declare noundef range(i32 0, 1024) i32 @llvm.nvvm.read.ptx.sreg.tid.x()
; DEFAULT-DAG: declare noundef range(i32 0, 1024) i32 @llvm.nvvm.read.ptx.sreg.tid.y()
; DEFAULT-DAG: declare noundef range(i32 0, 64)   i32 @llvm.nvvm.read.ptx.sreg.tid.z()
; DEFAULT-DAG: declare noundef range(i32 0, 1)    i32 @llvm.nvvm.read.ptx.sreg.tid.w()

; DEFAULT-DAG: declare noundef range(i32 1, 1025) i32 @llvm.nvvm.read.ptx.sreg.ntid.x()
; DEFAULT-DAG: declare noundef range(i32 1, 1025) i32 @llvm.nvvm.read.ptx.sreg.ntid.y()
; DEFAULT-DAG: declare noundef range(i32 1, 65)   i32 @llvm.nvvm.read.ptx.sreg.ntid.z()
; DEFAULT-DAG: declare noundef range(i32 0, 1)    i32 @llvm.nvvm.read.ptx.sreg.ntid.w()

; DEFAULT-DAG: declare noundef range(i32 0, 32)  i32 @llvm.nvvm.read.ptx.sreg.laneid()
; DEFAULT-DAG: declare noundef range(i32 32, 33) i32 @llvm.nvvm.read.ptx.sreg.warpsize()

; DEFAULT-DAG: declare noundef range(i32 0, 2147483647) i32 @llvm.nvvm.read.ptx.sreg.ctaid.x()
; DEFAULT-DAG: declare noundef range(i32 0, 65535)      i32 @llvm.nvvm.read.ptx.sreg.ctaid.y()
; DEFAULT-DAG: declare noundef range(i32 0, 65535)      i32 @llvm.nvvm.read.ptx.sreg.ctaid.z()
; DEFAULT-DAG: declare noundef range(i32 0, 1)          i32 @llvm.nvvm.read.ptx.sreg.ctaid.w()

; DEFAULT-DAG: declare noundef range(i32 1, -2147483648) i32 @llvm.nvvm.read.ptx.sreg.nctaid.x()
; DEFAULT-DAG: declare noundef range(i32 1, 65536)       i32 @llvm.nvvm.read.ptx.sreg.nctaid.y()
; DEFAULT-DAG: declare noundef range(i32 1, 65536)       i32 @llvm.nvvm.read.ptx.sreg.nctaid.z()
; DEFAULT-DAG: declare noundef range(i32 0, 1)           i32 @llvm.nvvm.read.ptx.sreg.nctaid.w()

; DEFAULT-DAG: declare noundef range(i32 0, 2147483647) i32 @llvm.nvvm.read.ptx.sreg.clusterid.x()
; DEFAULT-DAG: declare noundef range(i32 0, 65535)      i32 @llvm.nvvm.read.ptx.sreg.clusterid.y()
; DEFAULT-DAG: declare noundef range(i32 0, 65535)      i32 @llvm.nvvm.read.ptx.sreg.clusterid.z()
; DEFAULT-DAG: declare noundef range(i32 0, 1)          i32 @llvm.nvvm.read.ptx.sreg.clusterid.w()

; DEFAULT-DAG: declare noundef range(i32 1, -2147483648) i32 @llvm.nvvm.read.ptx.sreg.nclusterid.x()
; DEFAULT-DAG: declare noundef range(i32 1, 65536)       i32 @llvm.nvvm.read.ptx.sreg.nclusterid.y()
; DEFAULT-DAG: declare noundef range(i32 1, 65536)       i32 @llvm.nvvm.read.ptx.sreg.nclusterid.z()
; DEFAULT-DAG: declare noundef range(i32 0, 1)           i32 @llvm.nvvm.read.ptx.sreg.nclusterid.w()

; DEFAULT-DAG: declare noundef range(i32 0, 2147483647) i32 @llvm.nvvm.read.ptx.sreg.cluster.ctaid.x()
; DEFAULT-DAG: declare noundef range(i32 0, 65535)      i32 @llvm.nvvm.read.ptx.sreg.cluster.ctaid.y()
; DEFAULT-DAG: declare noundef range(i32 0, 65535)      i32 @llvm.nvvm.read.ptx.sreg.cluster.ctaid.z()
; DEFAULT-DAG: declare noundef range(i32 0, 1)          i32 @llvm.nvvm.read.ptx.sreg.cluster.ctaid.w()

; DEFAULT-DAG: declare noundef range(i32 1, -2147483648) i32 @llvm.nvvm.read.ptx.sreg.cluster.nctaid.x()
; DEFAULT-DAG: declare noundef range(i32 1, 65536)       i32 @llvm.nvvm.read.ptx.sreg.cluster.nctaid.y()
; DEFAULT-DAG: declare noundef range(i32 1, 65536)       i32 @llvm.nvvm.read.ptx.sreg.cluster.nctaid.z()
; DEFAULT-DAG: declare noundef range(i32 0, 1)           i32 @llvm.nvvm.read.ptx.sreg.cluster.nctaid.w()

declare i32 @llvm.nvvm.read.ptx.sreg.tid.x()
declare i32 @llvm.nvvm.read.ptx.sreg.tid.y()
declare i32 @llvm.nvvm.read.ptx.sreg.tid.z()
declare i32 @llvm.nvvm.read.ptx.sreg.tid.w()

declare i32 @llvm.nvvm.read.ptx.sreg.ntid.x()
declare i32 @llvm.nvvm.read.ptx.sreg.ntid.y()
declare i32 @llvm.nvvm.read.ptx.sreg.ntid.z()
declare i32 @llvm.nvvm.read.ptx.sreg.ntid.w()

declare i32 @llvm.nvvm.read.ptx.sreg.warpsize()
declare i32 @llvm.nvvm.read.ptx.sreg.laneid()

declare i32 @llvm.nvvm.read.ptx.sreg.ctaid.x()
declare i32 @llvm.nvvm.read.ptx.sreg.ctaid.y()
declare i32 @llvm.nvvm.read.ptx.sreg.ctaid.z()
declare i32 @llvm.nvvm.read.ptx.sreg.ctaid.w()

declare i32 @llvm.nvvm.read.ptx.sreg.nctaid.x()
declare i32 @llvm.nvvm.read.ptx.sreg.nctaid.y()
declare i32 @llvm.nvvm.read.ptx.sreg.nctaid.z()
declare i32 @llvm.nvvm.read.ptx.sreg.nctaid.w()

declare i32 @llvm.nvvm.read.ptx.sreg.clusterid.x()
declare i32 @llvm.nvvm.read.ptx.sreg.clusterid.y()
declare i32 @llvm.nvvm.read.ptx.sreg.clusterid.z()
declare i32 @llvm.nvvm.read.ptx.sreg.clusterid.w()

declare i32 @llvm.nvvm.read.ptx.sreg.nclusterid.x()
declare i32 @llvm.nvvm.read.ptx.sreg.nclusterid.y()
declare i32 @llvm.nvvm.read.ptx.sreg.nclusterid.z()
declare i32 @llvm.nvvm.read.ptx.sreg.nclusterid.w()

declare i32 @llvm.nvvm.read.ptx.sreg.cluster.ctaid.x()
declare i32 @llvm.nvvm.read.ptx.sreg.cluster.ctaid.y()
declare i32 @llvm.nvvm.read.ptx.sreg.cluster.ctaid.z()
declare i32 @llvm.nvvm.read.ptx.sreg.cluster.ctaid.w()

declare i32 @llvm.nvvm.read.ptx.sreg.cluster.nctaid.x()
declare i32 @llvm.nvvm.read.ptx.sreg.cluster.nctaid.y()
declare i32 @llvm.nvvm.read.ptx.sreg.cluster.nctaid.z()
declare i32 @llvm.nvvm.read.ptx.sreg.cluster.nctaid.w()
