base_folder="tests"

red=`tput setaf 1`
green=`tput setaf 2`
reset=`tput sgr0`

current_folder=${2:-./}
passed=true
(( failed = 0 ))
test_num=$(ls ${base_folder}/test*.dat | wc -l)

for file in ${current_folder}/${base_folder}/test*.dat; do
    
    count=`echo $file | egrep -o [0-9]+`
    
    echo -n "Testing ${green}${file}${reset} ..."

    if [ -z "$1" ]; then
        build/lfucpp < $file > ${current_folder}/${base_folder}/temp.dat
    else
        $1 < $file > ${current_folder}/${base_folder}/temp.dat
    fi

    if diff ${current_folder}/${base_folder}/answ${count}.dat ${current_folder}/${base_folder}/temp.dat; then
        echo "${green}Passed${reset}"
    else
        echo "${red}Failed${reset}"
        passed=false
        (( failed += 1 ))
    fi
done

echo "${failed} tests failed out of ${test_num}"

if ${passed}; then
    exit 0
else
    exit 666
fi