####################################################################
#                A bunch of utility functions 
####################################################################

# Function assert. Similar to the assert function in assert.h
proc assert {exp} {
    set result [uplevel "expr $exp"]
    if {$result == 0} {error "assertion failed"}
}

# Function to evaluate a list without parsing the arguments
proc evaluate {list} {
    set cmd [lindex $list 0]
    set arglist [lrange $list 1 end]
    set separator "\} \{"
    set fullcmd "$cmd {[join $arglist $separator]}"
    eval $fullcmd
}
    

# Function to remove an element from a list
# Note that this does not create a new list
proc rmListElement {list element} {
    upvar $list l			 ;# note the use of upvar
    set index [lsearch -exact $l $element]
    set cmd [format "set $list \[lreplace \$$list $index $index\]"]
    uplevel $cmd			 ;# note the use of uplevel
}

# Floating point increment
proc fincr {varName {increment 1.0}} {
    upvar $varName v
    set newval [expr $v + $increment]
    uplevel [list set $varName $newval]
}

# Unique Name Generator
set uniquecounter 0
proc getUniqueNumber {} {
    global uniquecounter
    incr uniquecounter
    return $uniquecounter
}

proc getUniqueName {} {
    global uniquecounter
    incr uniquecounter
    return uniquename$uniquecounter
}


# A routine to print out diagnostic messages if
# verboselevel is set greater than 0
set verboselevel 0
proc verbose {message_verbose_level string} {
    global verboselevel
    if {$message_verbose_level <= $verboselevel} {puts $string}
}

# A routine to strip out all whitespaces in a string
set white_space_regexp "\[ \t\n\]+"
proc string_trimall {string} {
    global white_space_regexp
    regsub -all $white_space_regexp $string {} retval
    return $retval
}

# A routine that removes whitespaces and converts all
# characters to lowercase. Useful (for example) to generate
# strings suitable for Tk window-names
proc string_makename {string} {
    return [string tolower [string_trimall $string]]
}

