
if [ -z "$BASH_VERSION" ]; then
    return 0
fi

_bash_led_completion() {
    local cur prev words
    local opts="-l|-i|-c|-t|-h"
    local copts="-l -i -c -t -h"
    local long_opts="--list|--info|--colors|--trigger|--help"
    local clong_opts="--list --info --colors --trigger --help"
    local triggers
    local completed

    COMPREPLY=()

    _get_comp_words_by_ref -n : cur prev words

    if [ "${prev}" = "-t" -o "${prev}" = "--trigger" ]; then
        _count_args
        if [ -n "$_bash_led_completion_led_name" -a "$args" != "1" ]; then
            triggers=(`lsled -t $_bash_led_completion_led_name 2>/dev/null`)
            if [ -n "${triggers}" ] ; then
                COMPREPLY=( $(compgen -W "${triggers[*]}" -- ${cur}) )
            else
                COMREPLY=""
            fi
        else
            _bash_led_completion_led_name=""
        fi
    elif [[ ${cur} == --* ]] ; then
        COMPREPLY=( $(compgen -W "${clong_opts}" -S ' ' -- ${cur}) )
    elif [[ ${cur} == "-" ]] ; then
        COMPREPLY=( $(compgen -W "${copts}" -S ' ' -- ${cur}) )
    else
        completed=(`lsled -n 2>/dev/null`)
        if [ -n "${completed}" ] ; then
            COMPREPLY=( $(compgen -W "${completed[*]}" -- ${cur}) )
            _bash_led_completion_led_name=$COMPREPLY
        fi
    fi

    __ltrim_colon_completions "$cur"

    return 0
}
complete -F _bash_led_completion led



_bash_lsled_completion() {
    local cur prev
    local opts="-n|-t|-h"
    local copts="-n -t -h"
    local long_opts="--names|--triggers|--help"
    local clong_opts="--names --triggers --help"
    local names
    local completed

    COMPREPLY=()

    _get_comp_words_by_ref -n : cur prev

    if [ "${prev}" = "-t" -o "${prev}" = "--triggers" ]; then
        completed=(`lsled -n 2>/dev/null`)
        if [ -n "${completed}" ] ; then
            COMPREPLY=( $(compgen -W "${completed[*]}" -- ${cur}) )
        fi
    elif [[ ${cur} == --* ]] ; then
        COMPREPLY=( $(compgen -W "${clong_opts}" -S ' ' -- ${cur}) )
    elif [[ ${cur} == "-" ]] ; then
        COMPREPLY=( $(compgen -W "${copts}" -S ' ' -- ${cur}) )
    fi

    __ltrim_colon_completions "$cur"

    return 0
}
complete -F _bash_lsled_completion lsled
