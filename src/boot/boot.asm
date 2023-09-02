org 0x7C00
bits 16

%define ENDL 0x0D, 0x0A

; FAT 12 BIOS PARAMETER BLOCK
jmp short start
nop
bpb_oem:					db "CLPROS01"
bpb_bytes_per_sector:		dw 512
bpb_sectors_per_cluster:	db 1
bpb_reserved_sectors:		dw 1
bpb_fat_count:				db 2
bpb_dir_entries_count:		dw 0E0h
bpb_total_sectors:			dw 2880
bpb_media_descriptor_type:	db 0F0h
bpb_sectors_per_fat:		dw 9
bpb_sectors_per_track:		dw 18
bpb_heads:					dw 2
bpb_hidden_sectors:			dd 0
bpb_large_sector_count:		dd 0

; EXTENDED BOOT RECORD
ebr_drive_number:			db 0
							db 0
ebr_signature:				db 28h
ebr_volume_id:				db 63h, 6Ch, 70h, 72h
ebr_volume_label:			db "Clipper    "
ebr_system_identifier:		db "FAT12   "

start:
	jmp main

;
; Prints a string to the screen
; Params:
;	- ds:si points to string
puts:
	push si
	push ax

	mov ah, 0Eh

.loop:
	lodsb		; load byte at ds:si into al
	or al, al	; is null?
	jz .done

	int 10h
	jmp .loop

.done:
	pop si
	pop ax
	ret
	

main:
	; setup data segment
	mov ax, 0
	mov ds, ax
	mov es, ax

	; setup stack
	mov ss, ax
	mov sp, 0x7C00

	; print message
	mov si, msg_hello
	call puts

	; read something from disk
	mov [ebr_drive_number], dl
	mov ax, 1
	mov cl, 1
	mov bx, 0x7E00
	call disk_read

	mov di, 0
	mov cl, [ebr_drive_number]
.loop:
	xor ch, ch
	shl cx, 4
	add ch, 30h
	test cx, 9
	jle .write_disk_number
	add ch, 11h 

.write_disk_number:
	mov byte [msg_drive_number_value + di], ch
	inc di
	cmp di, 2
	jne .loop

	mov si, msg_drive_number
	call puts

	hlt
	jmp halt

floppy_error:
	mov si, msg_read_failed
	call puts
	jmp wait_key_and_reboot
	
wait_key_and_reboot:
	mov ah, 0
	int 16h
	jmp 0FFFFh:0 
	hlt

halt:
	cli
	hlt

;
; Convert LBA address to CHS
; Parameters:
;	- ax: LBA address
; Returns:
;	- cx [bits 0-5]: sector
;	- cx [bits 6-15]: cylinder
;	- dh: head
;
lba_to_chs:
	push ax
	push dx

	xor dx, dx
	div word [bpb_sectors_per_track]

	inc dx
	mov cx, dx

	xor dx, dx
	div word [bpb_heads]

	mov dh, dl
	mov ch, al
	shl ah, 6
	or cl, ah

	pop ax
	mov dl, al
	pop ax
	ret

;
; Reads from disk
; Parameters:
; 	- ax: LBA address
;	- cl: number of sectors to read (max 128)
;	- dl: drive number
;	- es:bx: memory address where to store data
;
disk_read:
	push ax
	push bx
	push cx
	push dx
	push di

	push cx
	call lba_to_chs
	pop ax

	mov ah, 02h
	mov di, 3

.retry:
	pusha
	stc 
	int 13h
	jnc .done

	popa
	call disk_reset

	dec di
	jnz .retry

.fail:
	jmp floppy_error

.done:
	popa

	pop di
	pop dx
	pop cx
	pop bx
	pop ax
	ret

;
; Resets a disk controller
; Parameters:
;	dl: drive number
;
disk_reset:
	pusha
	mov ah, 0
	stc
	int 13h
	jc floppy_error
	popa
	ret


msg_hello: 				db "Hello, Clipper!", ENDL, 0
msg_read_failed:		db "Failed to read from disk.", ENDL, 0
msg_drive_number:		db "Disk number: "
msg_drive_number_value: dw 0
msg_drive_number_end:	db ENDL, 0

times 510-($-$$) db 0
dw 0AA55h
