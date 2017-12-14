# Utilities specific to Tk

proc diagnostic {string} {
    tk_dialog .diagnostic "Message" $string "" 0 OK
}
