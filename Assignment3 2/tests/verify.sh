#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
make all test
console_log="$(mktemp)"
trap 'rm -f "$console_log"' EXIT
app="./build/sensor_processor"
if [[ -f "$app.exe" ]]; then app="$app.exe"; fi
for n in 1 2 4 8; do
    "$app" data/sensor_small.txt "$n" </dev/null > "$console_log" 2>&1
    cmp report.csv tests/expected/report_expected.csv
    echo "Small N=$n: PASS"
done
"$app" data/sensor_invalid.txt 4 </dev/null > "$console_log" 2>&1
grep -qx '6,1,5' report.csv
grep -q 'Line 2:' "$console_log"
echo "Invalid: PASS"
if "$app" data/nonexistent.txt 4 </dev/null > "$console_log" 2>&1; then exit 1; fi
grep -q 'Cannot open' "$console_log"
echo "Missing file: PASS"
"$app" data/sensor_alarm.txt 4 </dev/null > "$console_log" 2>&1
test "$(grep -c '^ALARM ' "$console_log")" = 3
grep -q 'ALARM SPD_01 timestamp=2000' "$console_log"
grep -q 'ALARM TEMP_01 timestamp=1500' "$console_log"
grep -q 'ALARM VOLT_01 timestamp=1500' "$console_log"
echo "Alarms: PASS"
for run in 1 2 3 4 5; do
    "$app" data/sensor_big.txt 8 </dev/null > "$console_log" 2>&1
    cmp report.csv tests/expected/report_expected_big.csv
    echo "Big run $run: PASS ($(grep 'processing time' "$console_log"))"
done
printf 'TEMP_01\nUNKNOWN\n\n' | "$app" data/sensor_small.txt 2 > "$console_log" 2>&1
grep -q 'Found: TEMP_01' "$console_log"
grep -q 'Not found: UNKNOWN' "$console_log"
echo "Search: PASS"
"$app" data/sensor_empty.txt 4 </dev/null > "$console_log" 2>&1
grep -qx '0,0,0' report.csv
echo "Empty input: PASS"
