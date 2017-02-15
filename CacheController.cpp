#include "CacheController.h"

bool CacheController::memoryAccessPending(void) {
    return pendingMemoryAccess;   
}

void CacheController::setPendingMemoryAccess(bool value) {
    pendingMemoryAccess = value;   
}