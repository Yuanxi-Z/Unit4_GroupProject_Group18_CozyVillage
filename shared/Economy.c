#include "Economy.h"

// The actual gold variable is hidden here. 
// Using 'static' prevents external files from forcefully modifying it via 'extern'.
// Initialized with 100 G as starting funds for testing purposes.
static int global_total_gold = 100;

// ==========================================
// Get Gold Balance
// ==========================================
int Get_Total_Gold(void) {
    return global_total_gold;
}

// ==========================================
// Add Gold
// ==========================================
void Add_Gold(int amount) {
    if (amount > 0) {
        global_total_gold += amount;
        
        // Placeholder: If saving to STM32 internal Flash is needed in the future 
        // to preserve data after power loss, just add Flash_Write() here. 
        // Other team members' code won't need to change a single line!
    }
}

// ==========================================
// Spend Gold (Safe deduction with balance check)
// ==========================================
bool Spend_Gold(int amount) {
    if (amount > 0 && global_total_gold >= amount) {
        global_total_gold -= amount;
        return true;  // Sufficient balance, deduction successful
    }
    return false; // Insufficient balance, transaction denied
}