db 'Hello World from the just loaded sector!', 0

; fill up the rest of the sector
times 512-($-$$) db 0
