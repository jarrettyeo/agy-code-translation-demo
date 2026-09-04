#!/usr/bin/env bash
# ==============================================================================
# Local Betting Company (c) 2002
# System: Legacy Totalisator Subsystem (C++98 Standard)
# File: baseline_test.sh
# ==============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BIN="${SCRIPT_DIR}/bin/tote_engine"

echo "================================================================================"
echo " [TEST HARNESS] Local Betting Company Legacy Pari-mutuel Engine Baseline Audit"
echo "================================================================================"

# Verify binary exists, or build it
if [ ! -x "${BIN}" ]; then
    echo ">> Binary not found at ${BIN}. Building via make..."
    (cd "${SCRIPT_DIR}" && make)
fi

echo ">> Executable: ${BIN}"
echo

# 1. Run Ticket Parser Subsystem Tests
echo "--------------------------------------------------------------------------------"
echo " STAGE 1: Ticket Parser Protocol Validation (ASCII Fixed-Width & AMT-400 Binary)"
echo "--------------------------------------------------------------------------------"
"${BIN}" --test-parser
if [ $? -eq 0 ]; then
    echo "[PASS] Stage 1: Ticket parser and checksum verification succeeded."
else
    echo "[FAIL] Stage 1: Ticket parser validation failed!"
    exit 1
fi
echo

# 2. Run Standard Race Simulation (Feature Stakes - 1400m)
echo "--------------------------------------------------------------------------------"
echo " STAGE 2: Standard Race Simulation (19% Statutory Takeout & 10c Floor Breakage)"
echo "--------------------------------------------------------------------------------"
"${BIN}" --standard
if [ $? -eq 0 ]; then
    echo "[PASS] Stage 2: Standard race lifecycle & dividend calculations succeeded."
else
    echo "[FAIL] Stage 2: Standard race simulation failed!"
    exit 1
fi
echo

# 3. Run Dead-Heat Float Truncation Bug Audit
echo "--------------------------------------------------------------------------------"
echo " STAGE 3: Dead-Heat Dividend Audit (Single-Precision Truncation Bug)"
echo "--------------------------------------------------------------------------------"
OUTPUT=$("${BIN}" --dead-heat)
echo "${OUTPUT}"

# Check for presence of key audit markers
if echo "${OUTPUT}" | grep -q "Shortfall per \$1.00 bet:      \$0.10"; then
    echo
    echo "================================================================================"
    echo " [VERIFIED] Deliberate floating-point truncation defect successfully captured:"
    echo "   - Expected Exact 64-bit Dividend: \$2.70"
    echo "   - Legacy Buggy 32-bit Dividend:   \$2.60"
    echo "   - Underpayment Shortfall:         \$0.10 per \$1 stake (\$0.50 on \$5 ticket)"
    echo "================================================================================"
else
    echo "[FAIL] Dead-heat truncation bug discrepancy was not detected as expected!"
    exit 1
fi

echo
echo ">> All baseline verification tests passed with exit code 0."
exit 0
