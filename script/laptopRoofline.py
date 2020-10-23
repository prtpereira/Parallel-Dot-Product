#!/usr/local/bin/python


Physical_Cores      = 2
Threads_per_Core    = 2
Logical_Cores       = Physical_Cores * Threads_per_Core

Clock_Rate          = 2.7

#sysctl -a | grep machdep.cpu
#flag AVX1.0 ***avx2
Vec_Bits            = 256
Float_Size          = 4 * 8
Inst_per_Clock      = Vec_Bits / Float_Size

Peak_Perf_GFlops    = Physical_Cores * Clock_Rate * Inst_per_Clock

print "Physical_Cores = ",   Physical_Cores
print "Threads_per_Core = ", Threads_per_Core
print "Logical_Cores = ",    Logical_Cores
                       
print "Clock_Rate = ",       Clock_Rate
                       
print "Vec_Bits = ",         Vec_Bits     
print "Float_Size = ",       Float_Size     
print "Inst_per_Clock = ",   Inst_per_Clock 
                       
print "Peak_Perf_GFlops = ", Peak_Perf_GFlops
