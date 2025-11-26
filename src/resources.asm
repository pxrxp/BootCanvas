global seg_stack_setup_msg

; 13 = \r
; 10 = \n
;  0 = \0

seg_stack_setup_msg db "Segment registers cleared", 13, 10, "Stack setup", 13, 10, 0
