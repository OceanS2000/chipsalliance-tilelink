#ifndef __TYPE_H__
#define __TYPE_H__

#include <vector>
#include <stdint.h>

typedef std::vector<std::vector<uint64_t>> AddrRanges;
typedef std::vector<std::vector<bool>> ConnectivityMatrix;
typedef std::vector<std::size_t> IDSize;

#endif // __TYPE_H__