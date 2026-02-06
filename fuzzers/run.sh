#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
"${SCRIPT_DIR}/../build/fuzzers/fuzz_parse" -workers=4 -jobs=4 -timeout=120 "${SCRIPT_DIR}/corpus"
