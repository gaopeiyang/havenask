#include "build_service/admin/test/MockGenerationKeeper.h"
#include "build_service/test/unittest.h"
#include "fslib/fs/FileSystem.h"
#include "worker_framework/ZkState.h"

using namespace fslib;
using namespace fslib::fs;

namespace worker_framework {
static std::atomic<bool> MOCK_COMMIT_ERROR(false);

WorkerState::ErrorCode ZkState::write(const std::string& content)
{
    if (MOCK_COMMIT_ERROR.load(std::memory_order_relaxed)) {
        return WorkerState::EC_FAIL;
    }
    if (FileSystem::writeFile(_stateFile, content) != fslib::EC_OK) {
        return WorkerState::EC_FAIL;
    }
    return WorkerState::EC_OK;
}

}; // namespace worker_framework
