#include <linux/moduleparam.h>

bool simulate_allocs = false;
module_param(simulate_allocs, bool, 0444);

bool simulate_frees = false;
module_param(simulate_frees, bool, 0444);