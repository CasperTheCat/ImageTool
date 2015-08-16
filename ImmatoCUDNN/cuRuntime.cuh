#include <cstdint>

// externally reachable function declaration
void gpuProcess(unsigned char* data, uint64_t dataLength, uint32_t pX, uint32_t pY, int GPU);

// GPU Count Workaround
int gpuCount();