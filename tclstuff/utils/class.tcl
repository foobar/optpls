####################################################################
#        A bunch of functions to implement classes in TCL
#        Version 2.0
#          This version creates one global variable for each
#          data member of each instance
#          It creates one procedure for each data member per class
#          Note: no procedures are created for the methods (other
#          than the ones defined by the user), and the data-access
#          procedures are per class and not per instance
####################################################################

rename unknown unknown.old

# Our version of the unknown command ensures that when
# some proc named $classinstance.name is called, the
# call is redirected to the proc called <classname>.name

proc unknown args {
    set name [lindex $args 0]

    if {[is_class $name] == 1} {
	return [uplevel [linsert $args 0 "declare"]]
    }

    set match [cl_parse_name $name classname uniquifier separator rest]

    if {$match == 0} {
	uplevel unknown.old $args
    } else {
	set procname $classname$separator$rest
	set objname [cl_getobjname $classname $uniquifier]
	set newargs [lreplace $args 0 0 $procname $objname]
	return [uplevel [lreplace $args 0 0 $procname $objname]]
    }
}

# Returns 1 if name is a defined class. 0 otherwise
proc is_class {classname} {
    global cl_datamembers
    set classnames [array names cl_datamembers]
    set classidx [lsearch -exact $classnames $classname]
    if {$classidx < 0} {return 0} else {return 1}
}

# Return the name of the real variable corresponding
# to a given data member of a given instance of a given class.
# The use of this is not recommended because it has to be
# used in the context of an 'uplevel #0' or in a "global ..." command
proc lvalue compoundname {
    global cl_datamembers
    set match [cl_parse_name $compoundname classname uniquifier separator rest]
    if {$match == 0} {
	error "No lvalue for $compoundname"
    } else {
	if {[lsearch -exact $cl_datamembers($classname) $rest] < 0} {
	    error "$rest is not a data member of class $classname"
	}
	set objname [cl_getobjname $classname $uniquifier]
	return [cl_getname $classname $objname $rest]
    }
}

# Given a classname and a uniquifier, create a (unique) name
# for this class instance
proc cl_getobjname {classname uniquifier} {
    return class.$classname.$uniquifier
}

# Given classname, instancename and name of datamember
# return name of the global variable that holds the value
# of the data member
proc cl_getname {classname objname dataname} {
    return ${objname}__${dataname}
}

# Given the name of a class instance or a method, break
# that name up into its "classname", "uniquifier" and "rest"
# components
proc cl_parse_name {name classnamevar uniquifiervar separatorvar restvar} {
    upvar $classnamevar classname
    upvar $uniquifiervar uniquifier
    upvar $separatorvar separator
    upvar $restvar rest

    regexp {^class\.([a-zA-Z0-9_]+)\.([0-9]+)(\.)([a-zA-Z0-9_\.]+)?$} \
	    $name fullname classname uniquifier separator rest
}

# Return the value of given data member of given instance
# of given class (after setting the value to newvalue if
# new value is non nil)
proc cl_value {classname objname dataname newvalue} {
    set varname [cl_getname $classname $objname $dataname]
    if {$newvalue == ""} {
	uplevel #0 "set $varname"
    } else {
	uplevel #0 "set $varname \"$newvalue\""
    }
}

# Create the unique global variable that will hold the value
# of the given data member of the given instance of the given class
proc cl_data {classname objname dataname} {
    # objname is guaranteed to be unique

    # create the unique global variable that
    # will hold the current value of this data member
    # for this object
    uplevel #0 "set [cl_getname $classname $objname $dataname] {}"
}

# Unset the unique global variable that holds the value
# of the given data member of the given instance of the given class
proc cl_delete_data {classname objname dataname} {
    uplevel #0 "unset [cl_getname $classname $objname $dataname]"
}

# Destructor for the given object (instance)
proc delete {objname} {

    global cl_datamembers

    set match [cl_parse_name $objname classname uniquifier separator rest]

    foreach datamember $cl_datamembers($classname) {
	cl_delete_data $classname $objname $datamember
    }

    trace vdelete $objname u auto_delete
}

# Auto delete. This is called whenever a statically defined
# class variable goes out of scope
#
# auto_delete IS NOT WORKING. Probably due to a bug in Tcl
#
proc auto_delete {varname element op} {
    # ignore the "element" and "op" for the time-being
    upvar $varname v
    delete $v
}

 
# Create a new instance of the given class
# constructor_args is a list of arguments for the class constructor
proc new {classname {constructor_args {}}} {
    global cl_datamembers cl_methods

    # generate a unique name for this instance of this class

    set uniquifier [getUniqueNumber]
    set objname [cl_getobjname $classname $uniquifier]
    
    # now generate the data members
    foreach datamember $cl_datamembers($classname) {
	cl_data $classname $objname $datamember
    }

    # call the constructor
    # note that there is some tricky coding here because we
    # want each element of $args to go to the constructor
    # as a separate argument, but we dont want to replace
    # any of those arguments
    uplevel $classname.construct $objname $constructor_args

    return $objname
}

# Declare "instancename" to be a static instance of class "classname"
# It should be destroyed (by calling the destructor) when it goes
# out of scope
proc declare {classname instancename {constructor_args {}}} {
    #
    # auto_delete IS NOT WORKING. Probably due to a bug in Tcl
    # Hence we don't set up the trace on the variable
    #
    # uplevel "trace variable $instancename u auto_delete"

    uplevel "set $instancename \[new $classname \{$constructor_args\}\]"
}

# This proc is called when a class is being created. 
# It does the necessary things to set up "dataname" as
# a datamember of the class.
# Specifically, it creates the data-access proc 
# and also registers this dataname as one of the members
# of this class.
proc cl_mkdatamember {classname dataname} {
    global cl_datamembers

    lappend cl_datamembers($classname) $dataname

    # Now create the proc that will be used to access this datamember
    # This proc takes two arguments: the objname and the newvalue 
    # It sets the value of the datamember of that object to the newvalue
    # if no newvalue is specified, it returns the current value
    set procname $classname.$dataname
    set arglist [list this [list lvalue ""]]
    set body "cl_value $classname \$this $dataname \$lvalue"
    proc $procname $arglist $body
}

# This proc is called when a class is being created.
# It does the necessary processing to set up "methodname" as
# a method for this class.
# Currently, no processing is required.
proc cl_mkmethod {classname methodname} {
    global cl_methods
    
    lappend cl_methods($classname) $methodname
}

# This proc is called when a class is being created.
# The body of a declaration contains a bunch of 
# "body commands", one per line. This proc processes
# one body command
proc cl_body_cmd {classname str} {
    if {$str == ""} {return}

    if {[regexp {^[ 	]*$} $str] == 1} {
	return
    }

    set match [ \
	    regexp {^[ 	]*(data|method)[ 	]+([a-zA-Z0-9_]+)[ 	]*$} \
	    $str fullmatch declarator name]

    if {$match == 0} {
	error "expected <data/method name>, found <$str>"
    } 

    switch -exact -- $declarator {
	data {cl_mkdatamember $classname $name}
	method {cl_mkmethod $classname $name}
	default {error "internal error: declarator is <$declarator>"}
    }
}

# Create a class named "classname" and having specifications
# as described in "body"
proc class {classname body {inherits {}} {parentclass {}}} {
    global cl_datamembers cl_methods

    if {$parentclass == {}} {
	set cl_datamembers($classname) {}
	set cl_methods($classname) {}
    } else {
	set cl_datamembers($classname) $cl_datamembers($parentclass)
	set cl_methods($classname) $cl_methods($parentclass)

	foreach datamembername $cl_datamembers($classname) {
	    cl_mkdatamember $classname $datamembername
	}
	foreach methodname $cl_methods($classname) {
	    cl_mkmethod $classname $methodname
	}
    }

    # each individual line of the body is a separate command 
    set body_cmds [split $body "\n"]

    foreach string $body_cmds {
	cl_body_cmd $classname $string
    }
}

# Autoloading a class

# It is required that whoever defines a class must
# have an empty procedure called provide.$classname 
# defined in the file which defines that class. 
# Thus if one defines a class called tree then a
# the following definition "proc provide.tree {} {}" should
# be present in that file.
# To use that class, we just invoke the provide.$classname
# function and the whole file is automatically autoloaded
proc useclass {classname} {
    uplevel #0 "provide.$classname" 
}

