#!/usr/bin/bash
POINTS=(
        8 7                     # ADD_INCIDENT (15p): test01, test02
        10                      # CHECK_UNITS_AVAILABILITY (10p): test03
        10                      # SHOW_UNIT (10p): test04
        5 5 5                   # DISPATCH (15p): test05, test06, test07
        8 7                     # UNDO_LAST_DISPATCH (15p): test08, test09
        5 5                     # SOLVED_INCIDENT (10p): test10, test11
        10                      # SHOW_INTERVENTIONS (10p): test12
        2 1 1 1 1 2 1 1         # SHOW_INCIDENT / Complex (10p): test13-test20
        )
TESTS=${#POINTS[@]}
TOTAL=95
TOTALTESTS=0
READMEPOINTS=0
VALGRINDPOINTS=0

GREEN=$(tput setaf 2)
RED=$(tput setaf 1)
YELLOW=$(tput setaf 3)
ORANGE=$(tput setaf 208)
CYAN=$(tput setaf 6)
WHITE=$(tput setaf 15)
BOLD=$(tput bold)
RESET=$(tput sgr0)

function readme() {
    if test -f "README" || test -f "README.md"
        then
            if (test -f "README" && test -s "README") || (test -f "README.md" && test -s "README.md")
                then
                    echo "README: ${GREEN}5/5${RESET}"
                    let READMEPOINTS+=5
            else
                echo "README: ${RED}0/5 - Empty${RESET}"
            fi
    else
        echo "README: ${RED}0/5 - Missing${RESET}"
    fi
}

function checkExe() {
    if test -f "tema1"
        then
            main
    else
        echo "The executable file needs to be named 'tema1'"
    fi
}

function valgrindTest() {
    local file_name="$1"

    valgrind --leak-check=full --track-origins=yes -q --log-file=rezultat_valgrind.txt ./tema1 < tema1.in > /dev/null

    if [ ! -s rezultat_valgrind.txt ]; then
        echo "  Valgrind ${GREEN}PASSED${RESET}"
        VALGRIND_PER_TEST=$(echo "scale=4; 20.0 / $TESTS" | bc)
        VALGRINDPOINTS=$(echo "$VALGRINDPOINTS + $VALGRIND_PER_TEST" | bc)
    else
        echo "  Valgrind ${RED}FAILED${RESET}"
    fi
}

function testInput() {
    local file="$1"
    local test_index="$2"
    local file_name
    file_name=$(basename "$file")
    local ref_file="tests/ref/${file_name%.in}.ref"

    if [ "${POINTS[$test_index-1]}" -eq 0 ]; then
        echo "$file_name: SKIPPED (0 points)"
        return
    fi

    local out_file="tests/out/${file_name%.in}.out"

    cat "$file" > tema1.in
    sed -i 's/\r$//' tema1.in
    timeout 10s ./tema1 < tema1.in > tema1.out

    cp tema1.out "$out_file"

    if [ "$?" -eq 139 ]; then
        echo "$file_name: segmentation fault"
        return
    fi

    if ! diff -B -Z --strip-trailing-cr "$ref_file" tema1.out > /dev/null 2>&1; then
        echo "$file_name: ${RED}0/${POINTS[$test_index-1]}${RESET}"
    else
        echo "$file_name: ${GREEN}${POINTS[$test_index-1]}/${POINTS[$test_index-1]}${RESET}"
        let TOTALTESTS+=${POINTS[$test_index-1]}
        valgrindTest "$file_name"
    fi
}

function makeCommand() {
    make 1> /dev/null
    if [ "$?" -ne 0 ]
        then
            exit 1
    fi
}

function main() {
    echo "========================================="
    echo "         -= TEMA 1 SDA 2026 =-"
    echo "========================================="
    echo

    touch tema1.in
    touch tema1.out
    mkdir -p tests/out

    local test_index=1
    for file in $(ls tests/input/*.in | sort); do
        testInput "$file" "$test_index"
        ((test_index++))
    done

    echo
    echo "-----------------------------------------"
    echo "  Total teste:  ${CYAN}$TOTALTESTS/95${RESET}"

    VALGRINDPOINTS_ROUNDED=$(printf "%.0f" "$VALGRINDPOINTS")
    echo "  Valgrind:     ${YELLOW}$VALGRINDPOINTS_ROUNDED/20${RESET}"

    echo -n "  "
    readme
    echo "-----------------------------------------"

    FINAL_TOTAL=$((TOTALTESTS + VALGRINDPOINTS_ROUNDED + READMEPOINTS))
    echo "  ${BOLD}${ORANGE}Total tema:   $FINAL_TOTAL/120${RESET}"
    echo "========================================="

    if [ "$FINAL_TOTAL" -eq 120 ]; then
        echo "  ${BOLD}${WHITE}Felicitari! Punctaj maxim!${RESET}"
        echo "========================================="
    fi
}
makeCommand
checkExe
