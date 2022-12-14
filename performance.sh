# perf_test_mode.sh 
# disable ASLR
echo 0 > /proc/sys/kernel/randomize_va_space
echo "cat /proc/sys/kernel/randomize_va_space"
cat /proc/sys/kernel/randomize_va_space

# turn off turbo mode
echo 1 > /sys/devices/system/cpu/intel_pstate/no_turbo
echo ""
echo "cat /sys/devices/system/cpu/intel_pstate/no_turbo"
cat /sys/devices/system/cpu/intel_pstate/no_turbo

# turn cpu0 to perf mode
echo performance > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
echo ""
echo "cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor

# show isolated cpu core
echo ""
echo "cat /sys/devices/system/cpu/isolated"
cat /sys/devices/system/cpu/isolated
# expect cpu0 is isolated